#include <iostream>      // For cout/endl for printing
#include <unistd.h>      // For fork() and sleep() system calls
#include <sys/types.h>   // For pid_t type
#include <cstdlib>       // For exit() function

// This function simulates Process 1's behavior.
// It initializes a counter at 0 and increments it forever,
// printing the value once per second.
void process1() {
    int counter = 0;
    while (true) {
        std::cout << "Process 1: Counter = " << counter << std::endl;
        counter++;
        sleep(1); // Sleep for 1 second to slow output
    }
}

// This function simulates Process 2's behavior.
// It also initializes a counter at 0 and increments it forever,
// printing the value once per second.
void process2() {
    int counter = 0;
    while (true) {
        std::cout << "Process 2: Counter = " << counter << std::endl;
        counter++;
        sleep(1); // Sleep for 1 second to slow output
    }
}

int main() {
    // The fork() system call creates a new child process.
    // The child process is an exact copy of the parent.
    pid_t pid = fork();

    // If fork() returns 0, this is the child process.
    if (pid == 0) {
        // Call the function for child process logic
        process2();
    } 
    // If fork() returns a positive value (child's PID), this is the parent process.
    else if (pid > 0) {
        // Call the function for parent process logic
        process1();
    } 
    // If fork() returns -1, there was an error creating the process.
    else {
        perror("fork"); // Print the error message
        exit(1);        // Exit the program with error code
    }
    return 0; // Never reached, as both functions loop forever
}
