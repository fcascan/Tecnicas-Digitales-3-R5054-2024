#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/

/* my libraries */
#include "globals.h"

/*****************************************************************************/

/* default values */
#define LINE_LENGHT 1024
#define CONFIG_INI_FILE "config.ini"
#define MAX_SIMULTANEOUS_CLIENTS_DEFAULT 5
#define RANDOM_GENERATOR_DEFAULT_LOCATION "03_random/random_generator.sh"
#define PRODUCER_MODE_DEFAULT 0   // 0 = random_script, 1 = I2C2 MPU6050
#define FIR_WINDOW_LENGTH_DEFAULT 10
#define RANDOM_NUMBER_FILE "03_random/random_number.txt"

/*****************************************************************************/

/* functions prototypes */
void silent_load_server_config();
void load_server_config();
void load_server_config_internal(int);
void write_in_config_filter_coeff();
