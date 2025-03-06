#include "../inc/server.h"

/*****************************************************************************/

/* global variables */
int server_socket = -1;
struct sockaddr_in server_address;
int server_port = 0;

int max_simultaneous_clients = MAX_SIMULTANEOUS_CLIENTS_DEFAULT;
char random_script_location[512];
int producer_mode = PRODUCER_MODE_DEFAULT;
char *random_number_file_path = RANDOM_NUMBER_FILE;

volatile int random_value_ready = 0;
int random_value = 0;
int fir_window_length = FIR_WINDOW_LENGTH_DEFAULT;
float filter_coeffs[10] = {0};
pid_t random_pid = 0;
pid_t driver_pid = 0;

int isDriverRunning = 1;
int isServerRunning = 1;
int isSendingData = 1;       // Flag to know if data is being sent or not

/*****************************************************************************/

void setup_server_port()
{
    if (server_port < 1 || server_port > 65535)
    {
        fprintf(stderr, "[SERVER] Error: wrong parameter: port = %d\n", server_port);
        my_exit(-2);
    }

    /* create IPV4 TCP socket */
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0)
    {
        fprintf(stderr, "[SERVER] Error: cannot create server_socket = %d\n", server_socket);
        my_exit(-3);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);     //converts unsigned short from host byte order to network byte order

    /* bind socket to port */
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "[SERVER] Error: cannot bind socket to port %d\n", server_port);
        my_exit(-4);
    }

    /* listen on port */
    if (listen(server_socket, 5) < 0)
    {
        fprintf(stderr, "[SERVER] Error: cannot listen on port %d\n", server_port);
        my_exit(-5);
    }
    printf("Server ready and listening on port %d\n", server_port);
}

/*****************************************************************************/

/* set up data producer selected */
void setup_producer(int port, const char *program_name)
{
    if (producer_mode == 0) // random number mode
    {
        /* set file to read data from */
        random_number_file_path = RANDOM_NUMBER_FILE;
        
        /* execute random number generator script as producer */
        execute_random_number_generator_script();
        printf("Server listening the random generator producer\n");
    }
    else if (producer_mode == 1)    // I2C2 with MPU6050 mode
    {
        execute_i2c_driver_generator();
        printf("Server listening the I2C2 MPU6050 producer\n");
    }
}

/*****************************************************************************/

/* my_exit to free resources and then exit */
void my_exit(int exit_code)
{
    printf("my_exit(%d): Freeing resources and exiting\n", exit_code);

    isDriverRunning = 0;
    isServerRunning = 0;
    isSendingData = 0;

    cleanup_shm_sem();

    kill(random_pid, SIGUSR1);  //TODO ver si por descomentar esto no rompi el random_gen
    kill(driver_pid, SIGUSR2);

    /* close the server socket */
    shutdown(server_socket, SHUT_RDWR);

    exit(exit_code);
}

/*****************************************************************************/

/* execute the random number generator script in a child process */
void execute_random_number_generator_script()
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error forking process");
        my_exit(-6);
    }
    else if(pid > 0)    //This is the parent process (server)
    {
        printf("Fork executed, Driver Child Process PID: %d\n", pid);
        random_pid = pid;

        /* Ignore SIGCHLD signal */
        signal(SIGUSR2, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);   //TODO revisar, evita que al cerrar el driver se dispare sigchld
    }
    else if (pid == 0)   // This is for the Child process for the random generator script running in bash
    {
        random_pid = getpid();  // save pid from itself
        char server_pid_str[10];
        sprintf(server_pid_str, "%d", getppid());  // get parent process ID
        printf("Server PID to send to random generator script: %s\n", server_pid_str);

        /* Ensure the script has execution permissions */
        if (chmod(random_script_location, S_IRUSR | S_IWUSR | S_IXUSR) < 0)
        {
            perror("Error setting random generator script permissions");
            my_exit(-7);
        }

        /* Execute the script */
        execl("/bin/bash", "bash", random_script_location, server_pid_str, NULL);
        perror("Error executing random generator script"); // if execl returns, there was an error
        my_exit(-8);
    }
}

/*****************************************************************************/

/* execute the I2C driver generator in a child process */
void execute_i2c_driver_generator()
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in setup_producer() -> forking process");
        my_exit(-9);
    }
    else if(pid > 0)    //This is the parent process (server)
    {
        printf("Fork executed, Driver Child Process PID: %d\n", pid);
        driver_pid = pid;

        /* Ignore SIGCHLD signal */
        signal(SIGUSR1, SIG_IGN);
    }
    else if (pid == 0)   // This is for the Child process for the driver
    {
        /* Ignore interrupt signal */
        signal(SIGINT, SIG_IGN);

        driver_pid = getpid();  // save pid from itself

        /* Stay alive while data is being read */
        printf("Server listening the producer\n");
        prodSensorData();
        printf("After calling prodSensorData()\n");
        my_exit(0);
    }
}

/*****************************************************************************/

void handle_client(int client_socket) 
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    pid_t client_pid;
    int sem_toggle = 0;
    int received_value;

    /* Set socket timeout to avoid indefinite blocking */
    struct timeval tv;
    tv.tv_sec = SOCKET_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    // Initial handshake
    if (send(client_socket, "OK", 2, 0) < 0) {
        perror("Error sending handshake to client");
        close(client_socket);
        return;
    }
    if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
        perror("Error receiving handshake from client");
        close(client_socket);
        return;
    }
    if (strncmp(buffer, "ACK", 3) != 0) {
        printf("Handshake failed. Closing connection.\n");
        close(client_socket);
        return;
    }

    if (send(client_socket, "OK", 2, 0) < 0) {
        perror("Error sending 2nd handshake to client");
        close(client_socket);
        return;
    }

    printf("[CLIENT_HANDLER]: handshake success\n");

    printf("[CLIENT_HANDLER]: Child process with PID = %d\n", getpid());

    static char last_sent[SHM_COPY_CHAR_DATA_SIZE] = "";

    while(isSendingData)
    {
        // Read and send a portion of the shared memory
        const char* shm_copy = read_from_shared_memory();
        if (strcmp(last_sent, shm_copy) != 0) 
        {
            //printf("[CLIENT_HANDLER]: Sending data to client: \n%s\n", shm_copy);

            if (send(client_socket, shm_copy, strlen(shm_copy), 0) < 0) 
            {
                perror("Error sending data to client");
                break;
            }
            strncpy(last_sent, shm_copy, sizeof(last_sent) - 1);
            last_sent[sizeof(last_sent) - 1] = '\0';
        }
        else
        {
            //Commented to avoid spamming the console
            //printf("[CLIENT_HANDLER]: Data not changed, not sending\n");
        }
        sleep(1);
    }
    close(client_socket);
    return;
}

/*****************************************************************************/

/* server main function */
int main(int argc, char ** argv)
{
    printf("[SERVER] Main started with PID: %d\n", getpid());

    /* create connection context */
    connection_t * connection = (connection_t *)malloc(sizeof(connection_t));

    /* signal handlers */
    setup_sigchld();    //SIGCHLD to kill zombie processes
    setup_sigint();     //SIGINT to kill zombie processes
    if (producer_mode)
    {
        // I2C MPU6050 mode
        //setup_sigusr2();    //SIGUSR2 to reload config.ini
    }
    else 
    {
        // Random number mode
        setup_sigusr1();    //SIGUSR1 to read values from the random generator producer
    }

    /* check for command line arguments */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        my_exit(-12);
    }

    /* obtain port number */
    sscanf(argv[1], "%d", &server_port);
    setup_server_port();

    /* load config.ini */
    load_server_config();

    /* setup shared memory and semaphore */
    setup_shared_memory_and_semaphore();

    /* setup data producer */
    setup_producer(server_port, argv[0]);

    printf("[SERVER] Before While loop - isServerRunning = %d\n", isServerRunning);
    while (isServerRunning)
    {
        /* accept incoming connections from clients (blocking IO) */
        connection->addr_len = sizeof(connection->address);
        connection->sock = accept(server_socket, (struct sockaddr *)&connection->address, &connection->addr_len);

        if (connection->sock <= 0)
        {
            free(connection);
            perror("Error: Failed to accept connection");
            my_exit(-13);
        }
        else
        {
            printf("[SERVER] Client accepted with IP=%s and port=%d\n", inet_ntoa(connection->address.sin_addr), ntohs(connection->address.sin_port));
            if (get_client_count() < max_simultaneous_clients)
            {
                int client_pid = fork();
                if (client_pid < 0)
                {
                    perror("Error forking process");
                    my_exit(-14);
                }
                else if (client_pid == 0)
                {
                    /* Client Child Thread */
                    handle_client(connection->sock);
                    //printf("[CLIENT]: After handle_client()\n");
                    close(connection->sock);
                }
                else if (client_pid > 0)
                {
                    /* Parent Process (Server) */
                    increment_client_count();
                    printf("Active clients connections = %d\n", get_client_count());
                    close(connection->sock);
                }
            }
            else
            {
                close(connection->sock);
                printf("Client rejected, too many active connections\n");
                //sleep added to avoid spamming the console and to reduce CPU usage
                sleep(1);
            }
        }
    }

    printf("[SERVER_MAIN] Server stopped\n");
    my_exit(200);
}
