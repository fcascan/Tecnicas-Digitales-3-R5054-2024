#include <signal.h>     // Para SIGCHLD, SIGUSR1 y sigaction
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>   // Para waitpid
#include <sys/ipc.h>
#include <unistd.h>     // Para getpid()
#include <sys/shm.h>    // Para shmdt, shmctl
//#include <sys/sem.h>    // Para semop, semget, semctl
#include <errno.h>
#include <poll.h>
#include <string.h>

/*****************************************************************************/

/* my libraries */
#include "globals.h"
#include "shared_memory.h"

/*****************************************************************************/

/* functions prototypes */
void setup_sigchld();
void setup_sigusr1();
void setup_sigusr2();
void setup_sigint();
void sigchld_handler(int signal_number);
void sigint_handler(int sig);
void sigusr1_handler(int sig);
void sigusr2_handler(int sig);