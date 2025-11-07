#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cstring>
#include <signal.h>

// Struct to hold shared memory data
struct SharedData {
    // Multiple value - default 3, can be changed
    int multiple;
    
    // Counter from Process 1 - shared with Process 2
    // Process 2 starts when this > 100
    int counter;
};

volatile bool child_exited = false;

void single_child(int sig) {
    child_exited = true;
}

// Process 1 - increments counter and updates shared memory
void process1(SharedData* shared) {
    pid_t my_id = getpid();
    int counter = 0;
    int num_cycles = 0;
    
    // Run until child exits
    while (!child_exited) {
        // Update the shared counter
        shared->counter = counter;
        
        // Display if multiple of the shared multiple value
        if (counter % shared->multiple == 0) {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " 
                      << num_cycles << " -- " << counter 
                      << " (multiple of " << shared->multiple << ")" << std::endl;
        } else {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }
        
        counter++;
        num_cycles++;
        sleep(1);
    }
    
    std::cout << "Process 1 (PID: " << my_id << "): Counter exceeded 500. Exiting." << std::endl;
}

int main() {
    signal(SIGCHLD, single_child);
    
    // Create shared memory segment
    int shmid = shmget(12345, sizeof(SharedData), IPC_CREAT | 0666);
    
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    
    // Attach to shared memory
    SharedData* shared = (SharedData*) shmat(shmid, NULL, 0);
    
    if (shared == (SharedData*)-1) {
        perror("shmat failed");
        exit(1);
    }
    
    // Initialize shared data
    shared->multiple = 3;  // Can be changed here
    shared->counter = 0;
    
    pid_t child = fork();
    
    if (child == 0) {
        // Child becomes Process 2
        execl("./process2", "process2", NULL);
        perror("execl error");
        exit(1);
    } 
    else if (child > 0) {
        // Parent runs Process 1
        process1(shared);
        
        wait(NULL);
        
        // Cleanup shared memory
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        
        exit(0);
    } 
    else {
        perror("fork error");
        exit(1);
    }
    
    return 0;
}
