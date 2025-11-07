#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <cstring>
#include <signal.h>

// Struct keeps our shared values in memory accessible by both processes
struct SharedData {
    int multiple; // What we check counter against (multiples)
    int counter;  // Main counter, updated by Process 1, visible to Process 2
};

// Track if the child has finished
volatile bool child_exited = false;

// Simple handler to record when Process 2 exits
void single_child(int sig) {
    child_exited = true;
}

// These functions are for semaphore P (wait/lock) and V (signal/unlock)
// Only one process can "lock" (own) the semaphore at a time
void semWait(int semid) {
    struct sembuf p = {0, -1, SEM_UNDO};
    semop(semid, &p, 1);
}
void semSignal(int semid) {
    struct sembuf v = {0, 1, SEM_UNDO};
    semop(semid, &v, 1);
}

void process1(SharedData* shared, int semid) {
    pid_t my_id = getpid();
    int counter = 0;
    int num_cycles = 0;
    // This loop keeps running until the child signals it's done or counter hits 500
    while (!child_exited && counter <= 500) {
        // Enter critical section: block until we hold the lock
        semWait(semid);

        // We are now the only process that can mess with shared memory
        shared->counter = counter;
        if (counter % shared->multiple == 0) {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle "
                      << num_cycles << " -- " << counter
                      << " (multiple of " << shared->multiple << ")" << std::endl;
        } else {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }

        // Done with shared variables, unlock and let other process go
        semSignal(semid);

        counter++;
        num_cycles++;
        sleep(1); // Sleep to slow things down and view output easily
    }
    std::cout << "Process 1 (PID: " << my_id << "): Exiting." << std::endl;
}

int main() {
    // Watch for child termination to know when to exit early
    signal(SIGCHLD, single_child);

    // Create shared memory segment
    int shmid = shmget(12345, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget failed"); exit(1); }
    SharedData* shared = (SharedData*) shmat(shmid, NULL, 0);
    if (shared == (SharedData*)-1) { perror("shmat failed"); exit(1); }
    shared->multiple = 3;
    shared->counter = 0;

    // Set up a binary semaphore (mutex) to guard shared memory
    int semid = semget(54321, 1, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget failed"); exit(1); }
    semctl(semid, 0, SETVAL, 1);

    pid_t child = fork();

    if (child == 0) {
        // Launch Process 2
        execl("./process2", "process2", NULL);
        perror("execl error");
        exit(1);
    } else if (child > 0) {
        // This runs our main Process 1 logic
        process1(shared, semid);
        wait(NULL);

        // Detach and clean up resources
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
        exit(0);
    } else {
        perror("fork error");
        exit(1);
    }
    return 0;
}
