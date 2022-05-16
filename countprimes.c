#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define MAX_CONST 25000 // Default upper limit

int processes; // Number of processes
int id; // Process ID

/* Check if value is prime */
bool is_prime(long n) {
    if(n == 2 || n == 3)
        return true;

    if(n <= 1 || n % 2 == 0 || n % 3 == 0)
        return false;

    for(long i = 5; i * i <= n; i += 6) {
        if(n % i == 0 || n % (i + 2) == 0)
            return false;
    }

    return true;
}

/* Compute the amount of prime numbers between min and max, inclusive */
long count_primes(long min, long max) {
   long count = 0;
   for(long i = min; i <= max; i++)
        if(is_prime(i)) count++;
   return count;
}

/* Execute processes */
void execute(int argc, char *argv[]) {
    double start = MPI_Wtime(); // Start time
    long total_primes_count; // Result of the computation
    double total_time; // Total time taken by all processes
    long delta = MAX_CONST / processes; // Divide amount of work equally
    long min = (id * delta) + 1; // Lower limit
    long max = (id + 1) * delta; // Upper limit
    long primes_count = count_primes(min, max); // Number of primes within range
    MPI_Send(&primes_count, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD); // Send number of primes to master
    MPI_Reduce(&primes_count, &total_primes_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD); // Get total number of primes
    double elapsed_time = MPI_Wtime() - start; // End time
    MPI_Reduce(&elapsed_time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // Get total time taken by all processes
    
    if(id == 0) { // Master process
        for(int j = 1; j <= processes; j++) {
            long data;
            MPI_Status status;
            MPI_Recv(&data, 1, MPI_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Receive data from process j
            printf("Process ID: %d, Prime count: %ld\n", status.MPI_SOURCE, data);
        }
        
        printf("\nTotal number of primes between 1 and %d: %ld\n", MAX_CONST, total_primes_count);
        printf("Execution time: %f seconds\n", total_time);
    }
}

/* Main function */
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // Get process ID
    MPI_Comm_size(MPI_COMM_WORLD, &processes); // Get number of processes
    
    execute(argc, argv);
    
    return MPI_Finalize(); // Finalize and return
}
