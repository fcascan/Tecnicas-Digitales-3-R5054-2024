#ifndef GLOBALS_H
#define GLOBALS_H

#include "shared_memory.h"

/* External Variables */
extern int max_simultaneous_clients;
extern int isServerRunning;
extern int producer_mode;
extern char random_script_location[512];
extern char *random_number_file_path;
extern int fir_window_length;
extern float filter_coeffs[10];
extern int driver_pid;
extern int shmId;
extern int semId;
extern struct sembuf sb;
extern int isDriverRunning;
extern int isSendingData;

/* Function Prototipes */
void my_exit(int exit_code);
void write_to_shared_memory(const char * value);
char* read_from_shared_memory();
void increment_client_count();
void decrement_client_count();
int get_client_count();
void silent_load_server_config();
void setup_sigusr2();

#endif // GLOBALS_H