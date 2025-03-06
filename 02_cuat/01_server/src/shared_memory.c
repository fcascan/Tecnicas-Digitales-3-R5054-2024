#include "../inc/shared_memory.h"

/*****************************************************************************/

int shmId;              // Shared memory ID
int semId;              // Semaphore ID
struct sembuf sb;       // Semaphore buffer
shared_memory_t *shm_ptr;

/* setup shared memory for data between processes, and semaphore for access control */
void setup_shared_memory_and_semaphore()
{
    // Generate a unique key
    //key_t key = ftok("shmfile", 65); 
    key_t key = IPC_PRIVATE;

    // Create shared memory segment -> shmget(key, size, flags)
    shmId = shmget(key, SHMGET_SIZE, 0666 | IPC_CREAT);
    if (shmId < 0)
    {
        perror("[SHARED_MEMORY] Error in setup_shared_memory_and_semaphore() -> shmget");
        my_exit(-21);
    }

    // Attach to the shared memory
    shm_ptr = (shared_memory_t *)shmat(shmId, NULL, 0);
    if (shm_ptr == (shared_memory_t *)-1)
    {
        perror("[SHARED_MEMORY] Error in setup_shared_memory_and_semaphore() -> shmat");
        my_exit(-22);
    }

    // Initialize client_count
    shm_ptr->client_count = 0;
    printf("[SHARED_MEMORY] Initialized client_count to %d\n", shm_ptr->client_count);

    // Create semaphore
    semId = semget(key, 1, 0666 | IPC_CREAT);
    if (semId < 0)
    {
        perror("[SHARED_MEMORY] Error in setup_shared_memory_and_semaphore() -> semget");
        my_exit(-23);
    }

    // Initialize semaphore to 1 = unlocked
    if (semctl(semId, 0, SETVAL, 1) < 0)
    {
        perror("[SHARED_MEMORY] Error in setup_shared_memory_and_semaphore() -> semctl");
        my_exit(-24);
    }

    // Initialize sembuf structure
    sb.sem_num = 0; // Semaphore index
    sb.sem_flg = 0; // No special flags

    printf("[SHARED_MEMORY] Shared memory and semaphore initialized.\n");
}

/*****************************************************************************/

/* write a value to shared memory data field */
void write_to_shared_memory(const char * value) 
{
    lock_semaphore();

    strncpy(shm_ptr->data, value, sizeof(shm_ptr->data) - 1);
    shm_ptr->data[sizeof(shm_ptr->data) - 1] = '\0'; // To ensure null-termination

    //printf("Written to shared memory: %s\n", value);

    unlock_semaphore();
}

/*****************************************************************************/

/* read a value from shared memory data field */
char* read_from_shared_memory() 
{
    lock_semaphore();

    static char shm_copy[SHM_CHAR_DATA_SIZE];
    strncpy(shm_copy, shm_ptr->data, sizeof(shm_copy) - 1);
    shm_copy[sizeof(shm_copy) - 1] = '\0'; // To ensure null-termination

    unlock_semaphore();
    
    return shm_copy;
}

/*****************************************************************************/

void increment_client_count()
{
    lock_semaphore();
    shm_ptr->client_count++;
    //printf("[SHARED_MEMORY] Incremented client_count to %d\n", shm_ptr->client_count);
    unlock_semaphore();
}

/*****************************************************************************/

void decrement_client_count()
{
    lock_semaphore();
    shm_ptr->client_count--;
    //printf("[SHARED_MEMORY] Decremented client_count to %d\n", shm_ptr->client_count);
    unlock_semaphore();
}

/*****************************************************************************/

int get_client_count()
{
    lock_semaphore();
    int count = shm_ptr->client_count;
    //printf("[SHARED_MEMORY] Current client_count = %d\n", count);
    unlock_semaphore();
    return count;
}

/*****************************************************************************/

/* lock semaphore */
void lock_semaphore()
{
    sb.sem_op = -1; // lock
    int retries = 0;
    while (semop(semId, &sb, 1) == -1)
    {
        if (retries >= MAX_RETRIES_SEMAPHORE)
        {
            perror("[SHARED_MEMORY] Error semop - lock");
            my_exit(-25);
        }
        retries++;
        usleep(RETRY_SEMAPHORE_DELAY);
    }
}

/*****************************************************************************/

/* unlock semaphore */
void unlock_semaphore()
{
    sb.sem_op = 1; // unlock
    int retries = 0;
    while (semop(semId, &sb, 1) == -1)
    {
        if (retries >= MAX_RETRIES_SEMAPHORE)
        {
            perror("[SHARED_MEMORY] Error semop - unlock");
            my_exit(-26);
        }
        retries++;
        usleep(RETRY_SEMAPHORE_DELAY);
    }
}

/*****************************************************************************/

/* cleanup shared memory and semaphore */
void cleanup_shm_sem()
{
    shmdt(shm_ptr);                 // detach shared memory
    shmctl(shmId, IPC_RMID, NULL);  // remove shared memory
    semctl(semId, 0, IPC_RMID);     // remove semaphore
}