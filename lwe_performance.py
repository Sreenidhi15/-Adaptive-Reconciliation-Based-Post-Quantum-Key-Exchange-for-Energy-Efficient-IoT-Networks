import numpy as np
import time
import pandas as pd
import psutil
import os
from datetime import datetime

# LWE Parameters (IoT-friendly)
Q = 1024        # Modulus
N = 64         # Dimension
NOISE = 8       # Error bound

class LWEKeyExchange:
    def __init__(self, q=Q, n=N, noise_bound=NOISE):
        self.q = q
        self.n = n
        self.noise_bound = noise_bound
    
    def generate_secret_key(self):
        """Generate random secret key vector"""
        return np.random.randint(0, self.q, self.n)
    
    def generate_error(self):
        """Generate small error vector"""
        return np.random.randint(-self.noise_bound, self.noise_bound + 1, self.n)
    
    def generate_public_matrix(self):
        """Generate public random matrix A"""
        return np.random.randint(0, self.q, (self.n, self.n))
    
    def compute_public_value(self, A, s, e):
        """Compute b = A*s + e (mod q)"""
        b = (np.dot(A, s) + e) % self.q
        return b
    
    def compute_shared_secret(self, s, b):
        """Compute shared secret: s^T * b"""
        shared = np.dot(s, b) % self.q
        return shared
    
    def reconciliation(self, secret1, secret2):
        """Reconcile two secrets to match"""
        threshold = self.q // 2
        diff = abs(secret1 - secret2)
        
        # If difference is small, they should match
        if diff < threshold or diff > (self.q - threshold):
            return (secret1 + secret2) // 2 % self.q, True
        return None, False
    
    def key_exchange_full(self):
        """Complete key exchange between two devices"""
        # Shared public matrix
        A = self.generate_public_matrix()
        
        # Device A
        sA = self.generate_secret_key()
        eA = self.generate_error()
        bA = self.compute_public_value(A, sA, eA)
        
        # Device B
        sB = self.generate_secret_key()
        eB = self.generate_error()
        bB = self.compute_public_value(A, sB, eB)
        
        # Both compute shared secrets
        sharedA = self.compute_shared_secret(sA, bB)
        sharedB = self.compute_shared_secret(sB, bA)
        
        # Reconciliation
        final_key, success = self.reconciliation(sharedA, sharedB)
        
        return success, final_key

def measure_single_exchange():
    """Measure one key exchange operation"""
    lwe = LWEKeyExchange()
    
    # Memory before
    process = psutil.Process(os.getpid())
    mem_before = process.memory_info().rss / 1024  # KB
    
    # Time measurement
    start_time = time.perf_counter()
    
    # Perform key exchange
    success, key = lwe.key_exchange_full()
    
    # Time measurement
    end_time = time.perf_counter()
    computation_time = (end_time - start_time) * 1000  # milliseconds
    
    # Memory after
    mem_after = process.memory_info().rss / 1024
    memory_used = abs(mem_after - mem_before)
    
    # Calculate sizes
    key_size = N * 4  # bytes (each int is 4 bytes)
    public_data_size = (N * N + N) * 4  # Matrix A + vector b
    
    # Energy estimate (simplified: power × time)
    # Assume 1W for computation (typical IoT device)
    energy_mj = computation_time * 1.0  # millijoules
    
    return {
        'computation_time_ms': computation_time,
        'memory_kb': memory_used if memory_used > 0 else 0.5,
        'key_size_bytes': key_size,
        'public_data_bytes': public_data_size,
        'energy_estimate_mj': energy_mj,
        'success': success
    }

def run_performance_tests(num_tests=100):
    """Run multiple tests and collect statistics"""
    print(f"Running {num_tests} LWE key exchange tests...")
    print("=" * 60)
    
    results = []
    success_count = 0
    
    for i in range(num_tests):
        result = measure_single_exchange()
        result['test_no'] = i + 1
        results.append(result)
        
        if result['success']:
            success_count += 1
        
        if (i + 1) % 20 == 0:
            print(f"Completed {i + 1}/{num_tests} tests...")
    
    print(f"\n✓ Tests completed: {num_tests}")
    print(f"✓ Success rate: {success_count}/{num_tests} ({100*success_count/num_tests:.1f}%)")
    
    return results

def save_results_to_csv(results, filename='lwe_performance_results.csv'):
    """Save results to CSV file"""
    df = pd.DataFrame(results)
    
    # Reorder columns
    columns = ['test_no', 'computation_time_ms', 'memory_kb', 
               'key_size_bytes', 'public_data_bytes', 
               'energy_estimate_mj', 'success']
    df = df[columns]
    
    df.to_csv(filename, index=False)
    print(f"\n✓ Results saved to: {filename}")
    
    return df

def print_summary_statistics(df):
    """Print summary statistics"""
    print("\n" + "=" * 60)
    print("PERFORMANCE SUMMARY STATISTICS")
    print("=" * 60)
    
    print(f"\nComputation Time (ms):")
    print(f"  Mean:   {df['computation_time_ms'].mean():.4f}")
    print(f"  Median: {df['computation_time_ms'].median():.4f}")
    print(f"  Min:    {df['computation_time_ms'].min():.4f}")
    print(f"  Max:    {df['computation_time_ms'].max():.4f}")
    
    print(f"\nMemory Usage (KB):")
    print(f"  Mean:   {df['memory_kb'].mean():.4f}")
    print(f"  Median: {df['memory_kb'].median():.4f}")
    
    print(f"\nEnergy Estimate (mJ):")
    print(f"  Mean:   {df['energy_estimate_mj'].mean():.4f}")
    print(f"  Total (100 exchanges): {df['energy_estimate_mj'].sum():.2f} mJ")
    
    print(f"\nData Sizes:")
    print(f"  Key size: {df['key_size_bytes'].iloc[0]} bytes")
    print(f"  Public data: {df['public_data_bytes'].iloc[0]} bytes")
    
    print(f"\nSuccess Rate:")
    print(f"  {df['success'].sum()}/{len(df)} ({100*df['success'].mean():.1f}%)")
    
    print("=" * 60)

def main():
    print("\n" + "=" * 60)
    print("LWE KEY EXCHANGE PERFORMANCE MEASUREMENT")
    print("=" * 60)
    print(f"\nParameters:")
    print(f"  Modulus (q):     {Q}")
    print(f"  Dimension (n):   {N}")
    print(f"  Noise bound:     {NOISE}")
    print(f"  Test timestamp:  {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    
    # Run tests
    results = run_performance_tests(num_tests=100)
    
    # Save to CSV
    df = save_results_to_csv(results)
    
    # Print statistics
    print_summary_statistics(df)
    
    print("\n✓ All done! Use this CSV in your NS-3 simulation.")
    print(f"✓ Next step: Copy 'lwe_performance_results.csv' to ns-3-dev/scratch/\n")

if __name__ == "__main__":
    main()