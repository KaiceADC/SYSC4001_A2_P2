#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <signal.h>

// Flag to know when the child process exits
volatile bool child_exited = false;

// Handle the signal when child process ends
void handle_sigchld(int sig) {
    child_exited = true;
}

// Process 1 - increments counter and displays output
// Runs while process 2 is still going
void process1() {
    pid_t my_id = getpid();
    int counter = 0;
    int num_cycles = 0;
    
    // Keep looping until child process finishes
    while (!child_exited) {
        // Only display multiples of 3
        if (counter % 3 == 0) {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " 
                      << num_cycles << " -- " << counter << " multiple of 3" << std::endl;
        } else {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }
        
        counter++;
        num_cycles++;
        sleep(1);
    }
    
    // When child exits, print this
    std::cout << "Process 1 (PID: " << my_id << "): Process 2 has finished. Exiting." << std::endl;
}

int main() {
    // Set up signal handler so we know when child exits
    signal(SIGCHLD, handle_sigchld);
    
    // Create child process
    pid_t child = fork();
    
    // Child process code
    if (child == 0) {
        // Replace this process with process2 executable
        execl("./process2", "process2", NULL);
        
        // If we get here, execl failed
        perror("execl error");
        exit(1);
    } 
    
    // Parent process code
    else if (child > 0) {
        // Run process1 while child is running
        // Both processes output at same time
        process1();
        
        // Make sure child is cleaned up
        wait(NULL);
        
        exit(0);
    } 
    
    // Error handling if fork fails
    else {
        perror("fork error");
        exit(1);
    }
    
    return 0;
}
