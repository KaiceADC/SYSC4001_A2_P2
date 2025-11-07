#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <cstring>

// Struct must match Process 1 (same memory layout!)
struct SharedData {
    int multiple;
    int counter;
};

// Semaphore helpers (same as Process 1)
void semWait(int semid) {
    struct sembuf p = {0, -1, SEM_UNDO};
    semop(semid, &p, 1);
}
void semSignal(int semid) {
    struct sembuf v = {0, 1, SEM_UNDO};
    semop(semid, &v, 1);
}

void process2(SharedData* shared, int semid) {
    pid_t my_id = getpid();
    int num_cycles = 0;
    // First, wait for Process 1 to get counter > 100
    while (true) {
        semWait(semid); // Lock shared memory
        int cval = shared->counter;
        semSignal(semid); // Unlock
        if (cval > 100) break;
        std::cout << "Process 2 (PID: " << my_id << "): Waiting for counter > 100 (current: "
                  << cval << ")" << std::endl;
        sleep(1);
    }
    std::cout << "Process 2 (PID: " << my_id << "): Counter > 100! Starting..." << std::endl;

    // Main loop, keeps reading and printing while counter <= 500
    while (true) {
        semWait(semid); // Guard all shared memory access
        int cval = shared->counter;
        int mval = shared->multiple;
        semSignal(semid);
        if (cval > 500) break;
        if (cval % mval == 0) {
            std::cout << "Process 2 (PID: " << my_id << "): Cycle "
                      << num_cycles << " -- " << cval
                      << " (multiple of " << mval << ")" << std::endl;
        } else {
            std::cout << "Process 2 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }
        num_cycles++;
        sleep(1);
    }
    std::cout << "Process 2 (PID: " << my_id << "): Counter exceeded 500. Exiting." << std::endl;
}

int main() {
    // Re-attach to same shared memory and semaphore as Process 1
    int shmid = shmget(12345, sizeof(SharedData), 0);
    if (shmid == -1) { perror("shmget failed"); exit(1); }
    SharedData* shared = (SharedData*) shmat(shmid, NULL, 0);
    if (shared == (SharedData*)-1) { perror("shmat failed"); exit(1); }
    int semid = semget(54321, 1, 0);
    if (semid == -1) { perror("semget failed"); exit(1); }

    process2(shared, semid);

    shmdt(shared);
    return 0;
}
