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

// Shared memory struct for communication
struct SharedData {
    int multiple;
    int counter;
};

volatile bool child_exited = false;

void single_child(int sig) {
    child_exited = true;
}

// P (wait) operation on semaphore
void semWait(int semid) {
    struct sembuf p = {0, -1, SEM_UNDO};
    semop(semid, &p, 1);
}

// V (signal) operation on semaphore
void semSignal(int semid) {
    struct sembuf v = {0, 1, SEM_UNDO};
    semop(semid, &v, 1);
}

void process1(SharedData* shared, int semid) {
    pid_t my_id = getpid();
    int counter = 0;
    int num_cycles = 0;

    while (!child_exited && counter <= 500) {
        // ENTER CRITICAL SECTION
        semWait(semid);

        shared->counter = counter;

        // Output logic
        if (counter % shared->multiple == 0) {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " 
                      << num_cycles << " -- " << counter 
                      << " (multiple of " << shared->multiple << ")" << std::endl;
        } else {
            std::cout << "Process 1 (PID: " << my_id << "): Cycle " << num_cycles << std::endl;
        }

        // LEAVE CRITICAL SECTION
        semSignal(semid);

        counter++;
        num_cycles++;
        sleep(1);
    }
    std::cout << "Process 1 (PID: " << my_id << "): Exiting." << std::endl;
}

int main() {
    signal(SIGCHLD, single_child);

    // Make shared memory
    int shmid = shmget(12345, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) { perror("shmget failed"); exit(1); }
    SharedData* shared = (SharedData*)shmat(shmid, NULL, 0);
    if (shared == (SharedData*)-1) { perror("shmat failed"); exit(1); }
    shared->multiple = 3;
    shared->counter = 0;

    // Create semaphore for mutual exclusion
    int semid = semget(54321, 1, IPC_CREAT | 0666);
    if (semid == -1) { perror("semget failed"); exit(1); }
    semctl(semid, 0, SETVAL, 1); // Initialize to 1

    pid_t child = fork();

    if (child == 0) {
        execl("./process2", "process2", NULL);
        perror("execl error");
        exit(1);
    } else if (child > 0) {
        process1(shared, semid);
        wait(NULL);

        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID); // Clean up semaphore
        exit(0);
    } else {
        perror("fork error");
        exit(1);
    }
    return 0;
}
