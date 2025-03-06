#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/in.h>
#include <sys/stat.h>   // Para S_IRUSR | S_IWUSR | S_IXUSR
#include <unistd.h>     // Para sleep
#include <sys/wait.h>   // Para waitpid
#include <signal.h>     // Para SIGCHLD, SIGUSR1 y sigaction
#include <errno.h>      // Para errno
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

/*****************************************************************************/

/* my libraries */
#include "config_handler.h"
#include "driver_handler.h"
#include "my_signals.h"
#include "shared_memory.h"

/*****************************************************************************/

/* default values */
#define MAX_SIMULTANEOUS_CLIENTS_DEFAULT 5
#define RANDOM_GENERATOR_DEFAULT_LOCATION "03_random/random_generator.sh"
#define PRODUCER_MODE_DEFAULT 0   // 0 = random_script, 1 = I2C2 MPU6050
#define RANDOM_NUMBER_FILE "03_random/random_number.txt"
#define FIR_WINDOW_LENGTH_DEFAULT 10
#define SOCKET_TIMEOUT 5; //Segs

#define SHM_COPY_CHAR_DATA_SIZE 1024

/*****************************************************************************/

/* data structure for connection */
typedef struct
{
    int sock;                       //the socket of the connection to one client
    struct sockaddr_in address;     //the address of a connected client
    int addr_len;                   //the length of the address field
} connection_t;

/*****************************************************************************/

/* functions prototypes */
void setup_server_port();
void load_server_config();
void setup_producer(int port, const char *program_name);
void my_exit(int exit_code);
void execute_random_number_generator_script();
void execute_i2c_driver_generator();
void handle_client(int client_socket);
