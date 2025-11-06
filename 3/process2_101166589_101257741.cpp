#include <iostream>
#include <unistd.h>
#include <cstdlib>

// Process 2 - the child process
// Decrements counter from 0 down to -500
void process2() {
    // Get the process ID for display
    pid_t my_id = getpid();
    
    // Start at 0 and count down
    int counter = 0;
    
    // Keep track of cycles
    int num_cycles = 0;
    
    // Loop while counter is above -500
    // When it goes to -500 or lower, we exit
    while (counter > -500) {
        // Display if multiple of 3
        if (counter % 3 == 0) {
            std::cout << "Process 2 (PID: " << my_id << "): Cycle " 
                      << num_cycles << " -- " << counter << " multiple of 3" << std::endl;
        } else {
            // Just show cycle number
            std::cout << "Process 2 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }
        
        // Decrement the counter
        counter--;
        
        // Increment cycle counter
        num_cycles++;
        
        // Wait 1 second before next iteration
        sleep(1);
    }
    
    // When counter reaches -500, exit
    std::cout << "Process 2 (PID: " << my_id << "): Reached -500. Exiting." << std::endl;
    
    // Exit the process - tells parent we're done
    exit(0);
}

int main() {
    // Start process 2
    process2();
    
    // Won't reach here because process2 calls exit(0)
    return 0;
}
