#include <sys/sem.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <stdint.h>
#include <sys/select.h>
#include <stddef.h>
#include <unistd.h>
#include "stdio.h"
#include <string.h>
#include <sys/signal.h>
#include <math.h>

/*****************************************************************************/

/* my libraries */
#include "globals.h"

/*****************************************************************************/

/* defines */
#define FIR_WINDOW_MAX_SIZE 250
#define MPU6050_BUFFER_BYTES 14
#define _XOPEN_SOURCE 700
#define ARES_2G 2.0f/32768.0f       // Se divide en 2¹⁵ para normalizar a (-1, 1)
#define GYR_250 250.0f/32768.0f     // Se divide en 2¹⁵ para normalizar a (-1, 1)
#define DRIVER_PATH     "/dev/td3_i2c_fcc"
#define DATA_VECTOR_SIZE 1024
#define DRIVER_HANDLER_SLEEP 1      //in secs

/*****************************************************************************/

/* data structure SensorBuffers */
typedef struct {
    float ac_x[FIR_WINDOW_MAX_SIZE];
    float ac_y[FIR_WINDOW_MAX_SIZE];
    float ac_z[FIR_WINDOW_MAX_SIZE];
    float gy_x[FIR_WINDOW_MAX_SIZE];
    float gy_y[FIR_WINDOW_MAX_SIZE];
    float gy_z[FIR_WINDOW_MAX_SIZE];
    float temp[FIR_WINDOW_MAX_SIZE];
} SensorBuffers;

/* data structure measurements */
typedef struct {   
    // raw data
    int16_t raw_ac_x;
    int16_t raw_ac_y;
    int16_t raw_ac_z;
    int16_t raw_temp;
    int16_t raw_gy_x;
    int16_t raw_gy_y;
    int16_t raw_gy_z;
    
    // filtered data
    float fir_ac_x;
    float fir_ac_y;
    float fir_ac_z;
    float fir_temp;
    float fir_gy_x;
    float fir_gy_y;
    float fir_gy_z;
} Measurements;

/* data structure for normalized data */
typedef struct {
    float accel_xout;
    float accel_yout;
    float accel_zout;
    float gyro_xout;
    float gyro_yout;
    float gyro_zout;
    float temp_out;
    float fir_accel_xout;
    float fir_accel_yout;
    float fir_accel_zout;
    float fir_gyro_xout;
    float fir_gyro_yout;
    float fir_gyro_zout;
    float fir_temp_out;
} NormalizedData;

/*****************************************************************************/

/* functions prototypes */
void prodSensorData();
float firFilter(float* buffer, int size, int sample, float* coeffs);
void calculate_inclination(float accel_x, float accel_y, float accel_z);