#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define DRIVER_PATH     "/dev/td3_i2c_fcc"
#define ARES_2G         2.0f/32768.0f
#define GYR_250         250.0f/32768.0f
#define BUFFER_SIZE     14
