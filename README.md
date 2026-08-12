# Adaptive Reconciliation Based Post-Quantum Key Exchange for Energy-Efficient IoT Networks

This project implements a lightweight, LWE-based (Learning With Errors) post-quantum key exchange protocol for resource-constrained IoT devices. The implementation provides energy-efficient cryptographic operations suitable for IoT networks while maintaining post-quantum security, and evaluates the protocol's network-level behavior using NS-3.

The project was developed for research and experimentation purposes in post-quantum cryptography for IoT environments, as part of coursework at Northeastern University (EECE 5155: Wireless Sensor Networks and the IoT).

## Overview

This implementation combines:

- **LWE-based key exchange**: Lattice-based cryptographic protocol resistant to classical and quantum attacks
- **Python-based cryptographic benchmarking**: Computation time, memory, and energy measurement over 100 independent key exchange runs
- **NS-3 network simulation**: Point-to-point IoT topology (2–50 nodes) evaluating packet delivery, delay, throughput, and energy at the network level
- **Scalability analysis**: Performance evaluated as the number of IoT nodes increases

## Sample Output

### LWE Cryptographic Performance (Python)

Running `lwe_performance.py` executes 100 independent LWE key exchange tests and reports computation time, memory usage, and energy estimates:

<img width="451" height="797" alt="LWE performance benchmark output" src="https://github.com/user-attachments/assets/40012df3-2552-466a-b89e-3d75bf76d2a0" />

### NS-3 Network Simulation

Running the NS-3 scenario with 5 IoT nodes over point-to-point links shows packet delivery, delay, throughput, and energy results:

<img width="1107" height="895" alt="NS-3 simulation output" src="https://github.com/user-attachments/assets/a341ce0c-3945-44e5-a87c-c8aff5ded46a" />

## Building and Running

### Python Implementation

The Python implementation has been tested on Windows 10 and Ubuntu 22.04 with Python 3.8+.

#### Requirements

- Python 3.8 or higher
- NumPy >= 1.21.0
- Matplotlib >= 3.4.0
- Pandas >= 1.3.0
- Cryptography >= 3.4.0

#### Installation

```
pip install numpy matplotlib pandas cryptography
```

#### Running

```
python lwe_performance.py
```

This generates `lwe_performance_results.csv` containing performance metrics including computation time, memory usage, and energy estimates for 100 LWE key exchange tests.

### NS-3 Network Simulation

The NS-3 simulation has been tested on Ubuntu 22.04 with NS-3.36+ using GNU GCC version 11.3.

#### Prerequisites

- NS-3 Network Simulator (version 3.36 or higher)
- C++ compiler (g++ or clang)
- WSL2 (for Windows users) or native Linux environment

#### Building

1. Copy the simulation file to your NS-3 scratch directory:

```
cp LWE.cc /path/to/ns-3-dev/scratch/
```

2. Copy the Python performance results into the NS-3 working directory (the simulation reads real cryptographic timing/energy data from this file):

```
cp lwe_performance_results.csv /path/to/ns-3-dev/
```

3. Navigate to the NS-3 directory and build:

```
cd /path/to/ns-3-dev
./ns3 configure --enable-examples
./ns3 build
```

#### Running

To run the simulation with default parameters (5 nodes):

```
NS_LOG=LWE_IoT_Network=info ./ns3 run "scratch/LWE --nodes=5"
```

To vary the number of IoT nodes:

```
./ns3 run "scratch/LWE --nodes=10"
./ns3 run "scratch/LWE --nodes=20"
./ns3 run "scratch/LWE --nodes=50"
```

This generates/appends to `lwe_ns3_results.csv`, containing network simulation metrics including packet delivery ratio, average delay, throughput, and energy consumption.

## Project Structure

```
.
├── LWE.cc                          # NS-3 simulation implementation
├── lwe_performance.py              # Python cryptographic implementation
├── lwe_ns3_results.csv             # Network simulation results
├── lwe_performance_results.csv     # Cryptographic performance data
├── LICENSE                         # MIT License
└── README.md                       # This file
```

## Performance Metrics

### Cryptographic Operations (Python)

- **Computation time**: Time to complete one LWE key exchange
- **Memory usage**: Peak memory footprint per exchange
- **Energy estimate**: Modeled energy cost per exchange, scaled for 100 exchanges
- **Success rate**: Fraction of exchanges that successfully reconcile to a matching shared key

### Network Performance (NS-3)

- **Packet Delivery Ratio (PDR)**: Fraction of packets successfully delivered to the central gateway
- **Average end-to-end delay**: Time from packet transmission to reception
- **Throughput**: Effective data transmission rate across the network
- **Total energy consumption**: Combined cryptographic + transmission energy, scaled by node count

## Technical Details

### LWE Parameters

- **Modulus (q)**: 1024
- **Dimension (n)**: 64
- **Noise bound**: 8
- **Key size**: 256 bytes (derived symmetric key)
- **Public exchange data size**: 16,640 bytes
- **Security basis**: Hardness of the Learning With Errors problem, reducible to worst-case lattice problems (e.g., Shortest Vector Problem)

### Error Reconciliation

Each device computes an intermediate value from its secret and the peer's public value, then applies a simple rounding/thresholding reconciliation function (`Recon`) to derive matching preliminary key bits. A key derivation function (KDF) compresses and randomizes the reconciled bitstring into the final fixed-length symmetric key.

### Network Configuration

- **Topology**: Point-to-point (P2P) links from each IoT node to a central gateway
- **Number of nodes**: Configurable, evaluated at 2, 5, 10, 20, and 50 devices
- **Traffic model**: One-time application-layer transmission per node, 16,640-byte packets
- **Simulation duration**: 100 seconds (default)

> **Why point-to-point instead of wireless?** The current implementation intentionally uses P2P wired links rather than a wireless PHY/MAC stack. This isolates the computational and protocol overhead introduced by the LWE cryptographic operations from wireless-specific effects like channel variability, association delay, and MAC-layer contention — giving a clean baseline for evaluating post-quantum overhead. Extending this to wireless (e.g., 802.15.4/Zigbee) is planned future work; see below.

## Results Summary

| Nodes | Packets Sent | Packets Received | PDR | Avg Delay (ms) | Throughput (kbps) | Total Energy (mJ) |
|------:|-------------:|------------------:|----:|----------------:|-------------------:|--------------------:|
| 2     | 2            | 2                 | 100%| 29.06           | 26.67              | 0.176                |
| 5     | 5            | 5                 | 100%| 29.06           | 66.67              | 0.440                |
| 10    | 10           | 10                | 100%| 29.06           | 133.34             | 0.879                |
| 20    | 20           | 20                | 100%| 29.06           | 213.35             | 1.759                |
| 50    | 50           | 50                | 100%| 29.06           | 222.24             | 4.397                |

*Table regenerated from a cleaned run of `lwe_ns3_results.csv` (duplicate and inconsistent rows removed — see Limitations below).*

## Limitations

- Evaluated only through software-based execution (Python) and packet-level simulation (NS-3); no hardware-level validation on real microcontrollers yet.
- Focuses solely on key exchange; does not yet include post-quantum digital signatures or mutual authentication.
- Network simulation considers a moderate scale (up to 50 nodes); denser deployments may introduce congestion and queuing effects not modeled here.
- Current topology is point-to-point wired, not wireless — see note above.

## Future Work

- Deploy on real IoT hardware (ESP32, Arduino, Raspberry Pi, ARM Cortex-M)
- Extend to wireless PHY/MAC (WiFi, 802.15.4/Zigbee, LoRaWAN)
- Add post-quantum authentication and key refresh mechanisms
- Compare against standardized schemes (CRYSTALS-Kyber, NTRU, Falcon)
- Large-scale testing with 100+ nodes
- Explore hardware acceleration (FPGA/ASIC) for further latency reduction

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

## References

[1] A. K. Kwala, A. Mishra, and A. Kumar, "Securing IoT communication: a module-LWE key exchange protocol with efficient error reconciliation technique," *Discover Internet of Things*, vol. 5, no. 84, pp. 1–16, 2025.

[2] A. K. Kwala, S. Kant, and A. Mishra, "Comparative analysis of lattice-based cryptographic schemes for secure IoT communications," *Discover Internet of Things*, vol. 4, no. 13, pp. 1–14, 2024.

[3] A. Sharma and S. Rani, "Post-quantum cryptography (PQC) for IoT-consumer electronics devices integrated with deep learning," *IEEE Transactions on Consumer Electronics*, vol. 71, no. 2, pp. 4925–4938, 2025.

[4] O. Regev, "On lattices, learning with errors, random linear codes, and cryptography," in *Proc. 37th Annu. ACM Symp. Theory of Computing (STOC)*, 2005, pp. 84–93.

[5] J. Bos et al., "CRYSTALS–Kyber: A CCA-secure module-lattice-based KEM," in *Proc. IEEE European Symp. Security Privacy (EuroS&P)*, 2018.

## Contributing

This project welcomes contributions and suggestions, including:

- Performance optimizations
- Additional error reconciliation strategies
- Support for other IoT communication protocols
- Extended network topologies
- Security analysis and improvements

Please open an issue or submit a pull request for any contributions.

## Author

**Sreenidhi Ramani**, **Cherisma Tanduru**
Department of Electrical and Computer Engineering, Northeastern University

## Acknowledgments

This work demonstrates practical post-quantum cryptographic techniques for IoT security, combining lightweight LWE-based key exchange with network-level simulation for comprehensive performance evaluation.

---

**Note**: This implementation is intended for research and experimentation purposes. For production deployments, additional security analysis, mutual authentication, and hardware validation are recommended.
