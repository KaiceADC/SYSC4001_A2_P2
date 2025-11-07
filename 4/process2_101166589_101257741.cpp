#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>

struct SharedData {
    int multiple;
    int counter;
};

void process2(SharedData* shared) {
    pid_t my_id = getpid();
    int num_cycles = 0;
    
    // Wait until Process 1's counter exceeds 100
    while (shared->counter <= 100) {
        std::cout << "Process 2 (PID: " << my_id << "): Waiting for counter > 100 (current: " 
                  << shared->counter << ")" << std::endl;
        sleep(1);
    }
    
    // When counter > 100, start processing
    std::cout << "Process 2 (PID: " << my_id << "): Counter > 100! Starting..." << std::endl;
    
    // Keep running while counter <= 500
    while (shared->counter <= 500) {
        // Display if multiple of the shared multiple value
        if (shared->counter % shared->multiple == 0) {
            std::cout << "Process 2 (PID: " << my_id << "): Cycle " 
                      << num_cycles << " -- " << shared->counter 
                      << " (multiple of " << shared->multiple << ")" << std::endl;
        } else {
            std::cout << "Process 2 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }
        
        num_cycles++;
        sleep(1);
    }
    
    // When counter exceeds 500, exit
    std::cout << "Process 2 (PID: " << my_id << "): Counter exceeded 500. Exiting." << std::endl;
}

int main() {
    // Attach to shared memory created by Process 1
    int shmid = shmget(12345, sizeof(SharedData), 0);
    
    if (shmid == -1) {
        perror("shmget failed in Process 2");
        exit(1);
    }
    
    // Attach to shared memory
    SharedData* shared = (SharedData*) shmat(shmid, NULL, 0);
    
    if (shared == (SharedData*)-1) {
        perror("shmat failed in Process 2");
        exit(1);
    }
    
    // Run Process 2
    process2(shared);
    
    // Detach from shared memory
    shmdt(shared);
    
    return 0;
}
