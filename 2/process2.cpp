#include <iostream>
#include <unistd.h>
#include <cstdlib>

// Process 2 is similar to Process 1 but counts DOWN instead of up
void process2() {
    pid_t pid = getpid();  // Get PID for identification
    int counter = 0;           // Counter starts at 0 and decrements
    int num_cycles = 0;     // Track iterations
    
    // Infinite loop to keep this process running
    while (1) {
        // Only display when counter is multiple of 3
        if (counter % 3 == 0) {
            std::cout << "Process 2 (PID: " << pid << "): Cycle number: " 
                      << num_cycles << " - " << counter << " is a multiple of 3" << std::endl;
        } else {
            // Otherwise just show cycle number
            std::cout << "Process 2 (PID: " << pid << "): Cycle number: " << num_cycles << std::endl;
        }
        
        counter--;        // Decrement instead of increment
        num_cycles++;  // Still count cycles forward
        sleep(1);     // 1 second pause
    }
}

int main() {
    // Start Process 2 and keep it running
    process2();
    
    return 0;
}
