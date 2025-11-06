#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstdlib>

// This function handles what Process 1 does
// It increments a counter and only prints multiples of 3
void process1() {
    pid_t pid = getpid();  // Get the process ID for display
    int counter = 0;           // Counter starts at 0 and goes up
    int num_cycles = 0;     // Tracks how many iterations we've done
    
    // Keep running indefinitely
    while (1) {
        // Check if the current count is divisible by 3
        if (counter % 3 == 0) {
            // Print when it's a multiple of 3
            std::cout << "Process 1 (PID: " << pid << "): Cycle number: " 
                      << num_cycles << " - " << counter << " is a multiple of 3" << std::endl;
        } else {
            // Just print the cycle number if it's not a multiple of 3
            std::cout << "Process 1 (PID: " << pid << "): Cycle number: " << num_cycles << std::endl;
        }
        
        counter++;        // Increment the counter
        num_cycles++;  // Move to next cycle
        sleep(1);     // Wait 1 second before next output
    }
}

int main() {
    // Create a child process using fork()
    pid_t child_pid = fork();
    
    if (child_pid == 0) {
        // This is the child process
        // Execute the second program which will replace this process
        execl("./process2", "process2", NULL);
        
        // If execl fails, print error
        perror("execl");
        exit(1);
    } else if (child_pid > 0) {
        // This is the parent process
        // Run Process 1 logic
        process1();
    } else {
        // Fork failed
        perror("fork");
        exit(1);
    }
    
    return 0;
}
