#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>     //for usleep()

/*****************************************************************************/

/* my libraries */
#include "globals.h"

/*****************************************************************************/
/* defines */
#define SHM_CHAR_DATA_SIZE 1024
#define SHMGET_SIZE (sizeof(shared_memory_t))
#define MAX_RETRIES_SEMAPHORE 3
#define RETRY_SEMAPHORE_DELAY 250    //usegs

/*****************************************************************************/

/* data structure for shared memory between all processes */
typedef struct {
    int client_count;   // Current clients connected counter
    char data[SHM_CHAR_DATA_SIZE];
} shared_memory_t;

/*****************************************************************************/

/* functions prototypes */
void setup_shared_memory_and_semaphore();
void write_to_shared_memory(const char * value);
char* read_from_shared_memory();
void increment_client_count();
void decrement_client_count();
int get_client_count();
void lock_semaphore();
void unlock_semaphore();
void cleanup_shm_sem();

#endif // SHARED_MEMORY_H