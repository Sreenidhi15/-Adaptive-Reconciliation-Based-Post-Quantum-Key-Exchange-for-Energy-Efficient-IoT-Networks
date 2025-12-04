# Adaptive Reconciliation Based Post-Quantum Key Exchange for Energy-Efficient IoT Networks

This project implements an adaptive reconciliation-based post-quantum key exchange protocol for resource-constrained IoT devices using the Learning with Errors (LWE) problem. The implementation provides energy-efficient cryptographic operations suitable for IoT networks while maintaining post-quantum security.

The project was developed for research and experimentation purposes in post-quantum cryptography for IoT environments.

## Overview

This implementation combines:
- **Module-LWE cryptographic operations**: Lattice-based key exchange protocol
- **Adaptive error reconciliation**: Optimized for low-bandwidth IoT communications
- **NS-3 network simulation**: Performance evaluation in realistic IoT network scenarios
- **Performance benchmarking**: Comprehensive analysis of cryptographic and network metrics

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
To install dependencies, type:
```bash
pip install numpy matplotlib pandas cryptography
```

#### Running
To run the performance analysis:
```bash
python lwe_performance.py
```

This generates `lwe_performance_results.csv` containing performance metrics including key generation time, encryption/decryption latency, and error reconciliation efficiency.

### NS-3 Network Simulation

The NS-3 simulation has been tested on Ubuntu 22.04 with NS-3.36+ using GNU GCC version 11.3.

#### Prerequisites
- NS-3 Network Simulator (version 3.36 or higher)
- C++ compiler (g++ or clang)
- WSL2 (for Windows users) or native Linux environment

#### Building
1. Copy the simulation file to your NS-3 scratch directory:
```bash
cp LWE.cc /path/to/ns-3-dev/scratch/
```

2. Navigate to NS-3 directory and build:
```bash
cd /path/to/ns-3-dev
./ns3 configure --enable-examples
./ns3 build
```

#### Running
To run the simulation with default parameters (5 nodes):
```bash
NS_LOG=LWE_IoT_Network=info ./ns3 run "scratch/LWE --nodes=5"
```

To modify the number of IoT nodes:
```bash
./ns3 run "scratch/LWE --nodes=10"
```

This generates `lwe_ns3_results.csv` containing network simulation metrics including throughput, latency, packet delivery ratio, and energy consumption.

## Project Structure
```
.
├── LWE.cc                          # NS-3 simulation implementation
├── lwe_performance.py              # Python cryptographic implementation
├── lwe_ns3_results.csv            # Network simulation results
├── lwe_performance_results.csv    # Cryptographic performance data
├── LICENSE                         # MIT License
└── README.md                       # This file
```

## Performance Metrics

The implementation provides comprehensive performance analysis:

### Cryptographic Operations
- **Key Generation Time**: LWE key pair generation latency
- **Encryption Time**: Message encryption performance
- **Decryption Time**: Ciphertext decryption performance
- **Error Reconciliation Overhead**: Adaptive reconciliation efficiency

### Network Performance
- **Throughput**: Data transmission rate in IoT network
- **Latency**: End-to-end key exchange delay
- **Packet Delivery Ratio**: Reliability metric
- **Energy Consumption**: Power efficiency for battery-operated devices

## Technical Details

### LWE Parameters
- **Lattice dimension (n)**: 512 (configurable)
- **Modulus (q)**: Optimized for IoT constraints
- **Error distribution**: Discrete Gaussian with standard deviation σ
- **Security level**: 128-bit post-quantum security

### Adaptive Reconciliation
The error reconciliation mechanism adaptively adjusts parameters based on:
- Channel noise levels
- Available bandwidth
- Device computational capacity
- Energy constraints

### Network Configuration
- **Topology**: Star topology with IoT gateway
- **Number of nodes**: Configurable (5-50 devices)
- **Communication protocol**: IEEE 802.15.4 (Zigbee)
- **Simulation duration**: 100 seconds (default)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## References

[1] A. K. Kwala, A. Mishra, and A. Kumar, "Securing IoT communication: a module-LWE key exchange protocol with efficient error reconciliation technique," *Discover Internet of Things*, vol. 5, no. 84, pp. 1–16, 2025.

[2] A. K. Kwala, S. Kant, and A. Mishra, "Comparative analysis of lattice-based cryptographic schemes for secure IoT communications," *Discover Internet of Things*, vol. 4, no. 13, pp. 1–14, 2024.

[3] A. Sharma and S. Rani, "Post-quantum cryptography (PQC) for IoT-consumer electronics devices integrated with deep learning," *IEEE Transactions on Consumer Electronics*, vol. 71, no. 2, pp. 4925–4938, 2025.

[4] Joppe W. Bos et al., "Frodo: Take off the Ring! Practical, Quantum-Secure Key Exchange from LWE," ACM CCS 2016. Extended version: https://eprint.iacr.org/2016/659

## Contributing

This project welcomes contributions and suggestions. Contributions may include:
- Performance optimizations
- Additional error reconciliation strategies
- Support for other IoT communication protocols
- Extended network topologies
- Security analysis and improvements

Please open an issue or submit a pull request for any contributions.

## Contact

For questions, issues, or collaboration opportunities, please open an issue on GitHub or contact the author.

## Author

**Sreenidhi**  
[Your University/Institution]

## Acknowledgments

This work demonstrates practical post-quantum cryptographic techniques for IoT security, combining lattice-based cryptography with adaptive error reconciliation and network simulation for comprehensive performance evaluation.

---

**Note**: This implementation is intended for research and experimentation purposes. For production deployments, additional security analysis and hardening are recommended.
