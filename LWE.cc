// LWE IoT Simulation - Point-to-Point Network (Simpler, Always Works)
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LWE_IoT_Network");

struct LweMetrics {
    double avg_computation_time_ms = 0.5;
    double avg_memory_kb = 12.0;
    double avg_energy_mj = 0.5;
    int key_size_bytes = 256;
    int public_data_bytes = 16640;
};

LweMetrics g_metrics;
uint32_t g_totalPacketsSent = 0;
uint32_t g_totalPacketsReceived = 0;

void LoadMetricsFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NS_LOG_WARN("CSV not found, using defaults");
        return;
    }

    std::string line;
    std::getline(file, line);

    double total_time = 0, total_memory = 0, total_energy = 0;
    int count = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> row;

        while (std::getline(ss, item, ',')) {
            row.push_back(item);
        }

        if (row.size() >= 6) {
            total_time += std::stod(row[1]);
            total_memory += std::stod(row[2]);
            total_energy += std::stod(row[5]);
            count++;
        }
    }

    if (count > 0) {
        g_metrics.avg_computation_time_ms = total_time / count;
        g_metrics.avg_memory_kb = total_memory / count;
        g_metrics.avg_energy_mj = total_energy / count;
        
        NS_LOG_INFO("CSV loaded: " << g_metrics.avg_computation_time_ms << "ms, " 
                    << g_metrics.avg_memory_kb << "KB, " << g_metrics.avg_energy_mj << "mJ");
    }
    file.close();
}

void RxCallback(Ptr<const Packet> packet, const Address& from) {
    g_totalPacketsReceived++;
    NS_LOG_INFO(Simulator::Now().GetSeconds() << "s - Central received packet " 
                << g_totalPacketsReceived << " (" << packet->GetSize() << " bytes)");
}

int main(int argc, char *argv[]) {
    uint32_t nIoTNodes = 2;
    double simTime = 10.0;
    
    CommandLine cmd;
    cmd.AddValue("nodes", "Number of IoT nodes", nIoTNodes);
    cmd.AddValue("time", "Simulation time", simTime);
    cmd.Parse(argc, argv);

    LoadMetricsFromCSV("lwe_performance_results.csv");

    // Create nodes
    NodeContainer iotNodes;
    iotNodes.Create(nIoTNodes);
    
    NodeContainer centralNode;
    centralNode.Create(1);

    // Point-to-Point links (each IoT node directly connected to central)
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // Install internet stack
    InternetStackHelper stack;
    stack.Install(iotNodes);
    stack.Install(centralNode);

    // Connect each IoT node to central node
    Ipv4AddressHelper address;
    std::vector<Ipv4InterfaceContainer> interfaces;
    
    for (uint32_t i = 0; i < nIoTNodes; i++) {
        NodeContainer pair;
        pair.Add(iotNodes.Get(i));
        pair.Add(centralNode.Get(0));
        
        NetDeviceContainer devices = p2p.Install(pair);
        
        std::ostringstream subnet;
        subnet << "10.1." << (i + 1) << ".0";
        address.SetBase(subnet.str().c_str(), "255.255.255.0");
        
        Ipv4InterfaceContainer iface = address.Assign(devices);
        interfaces.push_back(iface);
    }

    // Enable routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Server on central node
    uint16_t port = 9;
    UdpServerHelper server(port);
    ApplicationContainer serverApp = server.Install(centralNode.Get(0));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(simTime));
    
    Ptr<UdpServer> udpServer = DynamicCast<UdpServer>(serverApp.Get(0));
    udpServer->TraceConnectWithoutContext("Rx", MakeCallback(&RxCallback));

    // Clients on IoT nodes
    for (uint32_t i = 0; i < nIoTNodes; i++) {
        Ipv4Address serverAddr = interfaces[i].GetAddress(1); // Central node address
        
        UdpClientHelper client(serverAddr, port);
        client.SetAttribute("MaxPackets", UintegerValue(1));
        client.SetAttribute("Interval", TimeValue(Seconds(1.0)));
        client.SetAttribute("PacketSize", UintegerValue(g_metrics.public_data_bytes));
        
        ApplicationContainer clientApp = client.Install(iotNodes.Get(i));
        clientApp.Start(Seconds(2.0 + i * 0.5 + g_metrics.avg_computation_time_ms / 1000.0));
        clientApp.Stop(Seconds(simTime));
        
        g_totalPacketsSent++;
        
        NS_LOG_INFO("IoT Node " << i << " will send to " << serverAddr 
                    << " at " << (2.0 + i * 0.5) << "s");
    }

    // FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    NS_LOG_INFO("\n========================================");
    NS_LOG_INFO("LWE IoT Network Simulation (Point-to-Point)");
    NS_LOG_INFO("========================================");
    NS_LOG_INFO("IoT Nodes: " << nIoTNodes);
    NS_LOG_INFO("Computation: " << g_metrics.avg_computation_time_ms << " ms");
    NS_LOG_INFO("Memory: " << g_metrics.avg_memory_kb << " KB");
    NS_LOG_INFO("Energy: " << g_metrics.avg_energy_mj << " mJ");
    NS_LOG_INFO("Packet size: " << g_metrics.public_data_bytes << " bytes");
    NS_LOG_INFO("========================================\n");

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // Statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    double totalThroughput = 0;
    double totalDelay = 0;
    uint32_t totalRxPackets = 0;
    uint32_t totalTxPackets = 0;

    for (auto const& flow : stats) {
        totalTxPackets += flow.second.txPackets;
        totalRxPackets += flow.second.rxPackets;
        
        if (flow.second.rxPackets > 0) {
            totalDelay += flow.second.delaySum.GetSeconds();
            totalThroughput += flow.second.rxBytes * 8.0 / simTime / 1000.0;
        }
    }

    double avgDelay = (totalRxPackets > 0) ? (totalDelay / totalRxPackets * 1000.0) : 0;
    double pdr = (totalTxPackets > 0) ? (double)totalRxPackets / totalTxPackets * 100.0 : 0;
    double totalEnergy = g_totalPacketsSent * g_metrics.avg_energy_mj;

    NS_LOG_INFO("\n========================================");
    NS_LOG_INFO("RESULTS");
    NS_LOG_INFO("========================================");
    NS_LOG_INFO("Packets sent: " << g_totalPacketsSent);
    NS_LOG_INFO("Packets received: " << g_totalPacketsReceived);
    NS_LOG_INFO("PDR: " << pdr << "%");
    NS_LOG_INFO("Avg delay: " << avgDelay << " ms");
    NS_LOG_INFO("Throughput: " << totalThroughput << " kbps");
    NS_LOG_INFO("Total energy: " << totalEnergy << " mJ");
    NS_LOG_INFO("========================================\n");

    // Save CSV
    std::ofstream outfile("lwe_ns3_results.csv", std::ios::app);
    bool newFile = (outfile.tellp() == 0);
    
    if (newFile) {
        outfile << "nodes,packets_sent,packets_received,pdr,avg_delay_ms,throughput_kbps,"
                << "total_energy_mj,computation_ms,memory_kb\n";
    }
    
    outfile << nIoTNodes << "," << g_totalPacketsSent << "," << g_totalPacketsReceived << ","
            << pdr << "," << avgDelay << "," << totalThroughput << ","
            << totalEnergy << "," << g_metrics.avg_computation_time_ms << ","
            << g_metrics.avg_memory_kb * nIoTNodes << "\n";
    
    outfile.close();
    NS_LOG_INFO("Saved to lwe_ns3_results.csv\n");

    Simulator::Destroy();
    return 0;
}
