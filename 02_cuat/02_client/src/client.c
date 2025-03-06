#include "../inc/client.h"

/*****************************************************************************/

/* global variables */
int client_lifetime = CLIENT_LIFETIME_DEFAULT;

/*****************************************************************************/

/* load config.ini function */
void load_client_config() 
{
    char line[1024];
    int client_lifetime_read = 0;

    FILE *file = fopen("config.ini", "r");
    if (file) 
    {
        while (fgets(line, sizeof(line), file))
        {
            if (sscanf(line, "client_lifetime = %d", &client_lifetime) == 1)
            {
                client_lifetime_read = 1; // successfully read
            }
        }
        fclose(file);

        /* Assign default values if they were not read correctly */
        if (!client_lifetime_read)
        {
            client_lifetime = CLIENT_LIFETIME_DEFAULT;
            printf("Error: Cannot read client_lifetime from config.ini. Using default value: %d\n", CLIENT_LIFETIME_DEFAULT);
        }
    } 
    else 
    {
        printf("Error: Cannot open config.ini. Using client_lifetime default value: %d\n", CLIENT_LIFETIME_DEFAULT);
        client_lifetime = CLIENT_LIFETIME_DEFAULT;
    }
}

/*****************************************************************************/

/* client main function */
int main(int argc, char *argv[])
{
    int port;
    int sockfd = -1;
    struct sockaddr_in server_address;
    struct hostent* host;
    int connection_status;
    char buffer[MAX_READ_SIZE];
    struct timespec start_time;

    /* Checking command line parameters */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(-1);
    }

    /* Obtain port number */
    sscanf(argv[2], "%d", &port);
    if (port < 1 || port > 65535)
    {
        fprintf(stderr, "%s: error: invalid port parameter\n", argv[0]);
        exit(-2);
    }

    /* Create TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
        exit(-3);
    }

    /* Get host */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    host = gethostbyname(argv[1]);
    if (!host)
    {
        fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
        close(sockfd);
        exit(-4);
    }
    memcpy(&server_address.sin_addr, host->h_addr, host->h_length);

    /* Connect to server */
    connection_status = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status < 0)
    {
        fprintf(stderr, "%s: error: cannot connect to host %s %s\n", argv[0], argv[1], argv[2]);
        close(sockfd);
        exit(-5);
    }

    printf("[CLIENT]: Connected to server %s:%d\n", argv[1], port);

    /* Set socket timeout to avoid indefinite blocking */
    struct timeval tv;
    tv.tv_sec = SOCKET_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    /* Handshake */
    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
        perror("Error receiving handshake from server");
        close(sockfd);
        exit(-6);
    }
    if (strncmp(buffer, "OK", 2) != 0) {
        fprintf(stderr, "Invalid handshake. Closing.");
        close(sockfd);
        exit(-7);
    }

    if (send(sockfd, "ACK", 3, 0) < 0) {
        perror("Error sending handshake to server");
        close(sockfd);
        exit(-8);
    }

    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
        perror("Error receiving second handshake from server");
        close(sockfd);
        exit(-9);
    }
    if (strncmp(buffer, "OK", 2) != 0) {
        printf("Second handshake failed. Closing connection.\n");
        close(sockfd);
        exit(-10);
    }

    printf("[CLIENT]: Handshake successful\n");

    /* Load config.ini */
    load_client_config();

    /* Set start time */
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    printf("Before while loop\n");

    while (1)
    {
        /* Check if client lifetime has expired */
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        double elapsed = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
        double remaining = client_lifetime - elapsed;
        printf("Remaining time: %.2f secs\n", remaining);

        if (elapsed >= client_lifetime)
        {
            printf("Client lifetime expired. Disconnecting...\n");
            break;
        }

        /* Receive value from server */
        ssize_t n = recv(sockfd, buffer, MAX_READ_SIZE - 1, 0);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* No data available, continue the loop */
                continue;
            } else {
                printf("Error reading from socket. Exiting.\n");
                perror("Error receiving value from server");
                break;
            }
        } 
        else if (n == 0)
        {
            printf("Server closed the connection. Exiting.\n");
            break;
        }

        buffer[n] = '\0'; /* Ensure the buffer is null-terminated */
        printf("Received value: %s\n", buffer);

        /* Send ACK to server */
        send(sockfd, "KA", 2, 0);

        /* Add a delay to reduce excessive CPU usage */
        // sleep(1);
    }

    /* Close socket */
    close(sockfd);

    return 0;
}
