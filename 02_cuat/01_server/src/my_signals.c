#include "../inc/my_signals.h"

/*****************************************************************************/

/* set up SIGCHLD signal action */
/* signal to clean up child processes after they finish */
void setup_sigchld()
{
    //signal SIGCHLD once to remove previous zombie processes
    signal(SIGCHLD, sigchld_handler);

    struct sigaction sa_chld;
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART;  /* Restart syscall on signal return. -> To prevent interrupt from chidl */
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("Error in setup_sigchld() -> Error in set up SIGCHLD handler");
        my_exit(-31);
    }
}

/*****************************************************************************/

/* set up SIGINT signal action */
/* Signal to catch interruptions - used to clean up zombie processes before exiting server */
void setup_sigint()
{
    struct sigaction sa_int;
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;   /* No flags  */
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("Error in setup_sigint() -> Error in set up SIGINT handler");
        my_exit(-32);
    }
}

/*****************************************************************************/

/* set up SIGUSR1 signal action */
/* signal to process Random Number */
void setup_sigusr1()
{
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = SA_RESTART;  /* Restart syscall on signal return. -> To prevent interrupt from child */
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Error in setup_sigusr1() -> Error in set up SIGUSR1 handler");
        my_exit(-33);
    }
}

/*****************************************************************************/

/* set up SIGUSR2 signal action */
/* signal to handle driver */
void setup_sigusr2()
{
    struct sigaction sa_usr2;
    sa_usr2.sa_handler = sigusr2_handler;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;  /* Restart syscall on signal return. -> To prevent interrupt from chidl */
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1) {
        perror("Error in setup_sigusr2() -> Error in set up SIGUSR2 handler");
        my_exit(-34);
    }
}

/*****************************************************************************/

/* handle SIGCHLD signal */
/* clean up each client process when they finish */
void sigchld_handler(int signal_number)
{
    int childPid;
    int childStatus;

    while ((childPid = waitpid(-1, &childStatus, WNOHANG)) > 0) {
        if (WIFEXITED(childStatus) || WIFSIGNALED(childStatus)) {
            printf("Received SIGCHLD with pid: %d\n", childPid);
            decrement_client_count();
            printf("Child process with PID = %d and status = %d ended\n", childPid, childStatus);
        }
    }
    if (childPid == -1 && errno != ECHILD) {
        perror("Error in sigchld_handler() -> Error in waitpid");
        my_exit(-30);
    }
}

/*****************************************************************************/

/* handle SIGINT signal */
/* when server is stopped with interruption or manually stopped (Ctrl+C)) */
/* stop all child processes to avoid zombie processes */
void sigint_handler(int sig)
{
    int childPid = getpid();
    int childStatus;

    printf("Received SIGINT with pid: %d\n", childPid);

    isServerRunning = 0;  //Stop server

    /* kill driver process if running */
    if (driver_pid > 0) 
    {
        printf("Killing child process with PID = %d\n", driver_pid);
        kill(driver_pid, SIGTERM); // Enviar señal para terminar el proceso hijo
        waitpid(driver_pid, NULL, 0); // Esperar a que termine para evitar zombies
    }

    // Wait for all dead child processes without blocking the parent
    do {
        childPid = waitpid(-1, &childStatus, WNOHANG);
        if(childPid > 0) {
            printf("Child process with PID = %d and status = %d ended\n", childPid, childStatus);
        }
    } while(childPid > 0);

    my_exit(-36);
}

/*****************************************************************************/

/* handle SIGUSR1 signal */
/* read data from random generator producer */
void sigusr1_handler(int sig) 
{
    //printf("Received SIGUSR1\n");

    FILE *file = fopen(random_number_file_path, "r");

    if (file) 
    {
        int temp_value;
        char temp_value_str[12];
        int result = fscanf(file, "%d", &temp_value);
        if (result == 1) 
        {
            printf("Random value read: %d\n", temp_value);
            snprintf(temp_value_str, sizeof(temp_value_str), "%d", temp_value);
            write_to_shared_memory(temp_value_str);
        }
        else
        {
            printf("Error reading random number from file. fscanf returned: %d\n", result);
        }
        fclose(file);
    }
    else 
    {
        printf("Error opening file %s\n", random_number_file_path);
    }
}

/*****************************************************************************/

/* handle SIGUSR2 signal */
/* reload server config.ini */
void sigusr2_handler(int sig) 
{
    //printf("Received SIGUSR2 - Reload server config\n");
    silent_load_server_config();
}