#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
//#include <linux/time.h>
#include <errno.h>
#include <unistd.h> // For getpid() and usleep()

/*****************************************************************************/

/* defines */
#define CLIENT_LIFETIME_DEFAULT 10
#define SOCKET_TIMEOUT 10    //in seconds
#define MAX_READ_SIZE 4096  //1024

/*****************************************************************************/

/* functions prototypes */
void load_client_config();
