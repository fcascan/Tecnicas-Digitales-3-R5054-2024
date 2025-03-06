#include "../inc/driver_handler.h"

/*****************************************************************************/

void prodSensorData() 
{
    char finalDataVector[DATA_VECTOR_SIZE];  // Final data to save in shared memory
    Measurements data_s;    // Data structure for measurements
    SensorBuffers buffer;   // Data structure for sensor buffers
    NormalizedData normalizedData;  // Data structure for normalized data

    // Specify the file path
    const char *file_path = DRIVER_PATH;
    printf("[DRIVER_HANDLER]: file_path=%s\n", file_path);

    /* Open driver as Input/output system call */
    int driverFileDesc = open(file_path, O_RDONLY);

    if (driverFileDesc < 0)
    {
        perror("[DRIVER_HANDLER] ERROR when opening the driver file\n");
        my_exit(-40);
    }

    uint8_t *driverBuffer = malloc(MPU6050_BUFFER_BYTES*sizeof(uint8_t)); // Allocate memory for the driver buffer
    if (driverBuffer == NULL)
    {
        perror("[DRIVER_HANDLER] ERROR when allocating memory for the driver buffer -> driverBuffer = null");
        close(driverFileDesc);
        my_exit(-41);
    }

    setup_sigusr2();    //SIGUSR2 to reload config.ini

    //printf("[DRIVER_HANDLER] before while loop: isDriverRunning = %d\n", isDriverRunning);

    while(isDriverRunning)
    {
        /* FIR window validation */
        if (fir_window_length > FIR_WINDOW_MAX_SIZE) {
            fir_window_length = FIR_WINDOW_MAX_SIZE;
        }

        /* Buffer cleanup */
        for (int i = 0; i < fir_window_length; i++) {
            buffer.ac_x[i] = 0.0;
            buffer.ac_y[i] = 0.0;
            buffer.ac_z[i] = 0.0;
            buffer.gy_x[i] = 0.0;
            buffer.gy_y[i] = 0.0;
            buffer.gy_z[i] = 0.0;
            buffer.temp[i] = 0.0;
        }

        /* Read data from the driver */
        read(driverFileDesc, driverBuffer, MPU6050_BUFFER_BYTES);     // Read driver as Input/output system call

        /* Raw data */
        data_s.raw_ac_x = (0xFF & driverBuffer[0]) << 8 | (0xFF & driverBuffer[1]);
        data_s.raw_ac_y = (0xFF & driverBuffer[2]) << 8 | (0xFF & driverBuffer[3]);
        data_s.raw_ac_z = (0xFF & driverBuffer[4]) << 8 | (0xFF & driverBuffer[5]);
        data_s.raw_temp = (0xFF & driverBuffer[6]) << 8 | (0xFF & driverBuffer[7]);
        data_s.raw_gy_x = (0xFF & driverBuffer[8]) << 8 | (0xFF & driverBuffer[9]);
        data_s.raw_gy_y = (0xFF & driverBuffer[10]) << 8 | (0xFF & driverBuffer[11]);
        data_s.raw_gy_z = (0xFF & driverBuffer[12]) << 8 | (0xFF & driverBuffer[13]);

        /* Filtered Data */
        data_s.fir_ac_x = firFilter(buffer.ac_x, fir_window_length, data_s.raw_ac_x, filter_coeffs);
        data_s.fir_ac_y = firFilter(buffer.ac_y, fir_window_length, data_s.raw_ac_y, filter_coeffs);
        data_s.fir_ac_z = firFilter(buffer.ac_z, fir_window_length, data_s.raw_ac_z, filter_coeffs);
        data_s.fir_temp = firFilter(buffer.temp, fir_window_length, data_s.raw_temp, filter_coeffs);
        data_s.fir_gy_x = firFilter(buffer.gy_x, fir_window_length, data_s.raw_gy_x, filter_coeffs);
        data_s.fir_gy_y = firFilter(buffer.gy_y, fir_window_length, data_s.raw_gy_y, filter_coeffs);
        data_s.fir_gy_z = firFilter(buffer.gy_z, fir_window_length, data_s.raw_gy_z, filter_coeffs);

        /* Normalization of the data */
        normalizedData.accel_xout = (float)data_s.raw_ac_x * ARES_2G;
        normalizedData.accel_yout = (float)data_s.raw_ac_y * ARES_2G;
        normalizedData.accel_zout = (float)data_s.raw_ac_z * ARES_2G;
        normalizedData.gyro_xout = (float)data_s.raw_gy_x * GYR_250;
        normalizedData.gyro_yout = (float)data_s.raw_gy_y * GYR_250;
        normalizedData.gyro_zout = (float)data_s.raw_gy_z * GYR_250;
        normalizedData.temp_out = ((float)data_s.raw_temp) / 340 + 36.53;
        normalizedData.fir_accel_xout = data_s.fir_ac_x * ARES_2G;
        normalizedData.fir_accel_yout = data_s.fir_ac_y * ARES_2G;
        normalizedData.fir_accel_zout = data_s.fir_ac_z * ARES_2G;
        normalizedData.fir_gyro_xout = data_s.fir_gy_x * GYR_250;
        normalizedData.fir_gyro_yout = data_s.fir_gy_y * GYR_250;
        normalizedData.fir_gyro_zout = data_s.fir_gy_z * GYR_250;
        normalizedData.fir_temp_out = data_s.fir_temp / 340 + 36.53;

        sprintf(
            finalDataVector, 
            "accel_xout = %.5f\naccel_yout = %.5f\naccel_zout = %.5f\n"
            "gyro_xout = %.5f\ngyro_yout = %.5f\ngyro_zout = %.5f\n"
            "temp_out = %.5f\n"
            "fir_accel_xout = %.5f\nfir_accel_yout = %.5f\nfir_accel_zout = %.5f\n"
            "fir_gyro_xout = %.5f\nfir_gyro_yout = %.5f\nfir_gyro_zout = %.5f\n"
            "fir_temp_out = %.5f\n",
            normalizedData.accel_xout, normalizedData.accel_yout, normalizedData.accel_zout, 
            normalizedData.gyro_xout, normalizedData.gyro_yout, normalizedData.gyro_zout, 
            normalizedData.temp_out,
            normalizedData.fir_accel_xout, normalizedData.fir_accel_yout, normalizedData.fir_accel_zout, 
            normalizedData.fir_gyro_xout, normalizedData.fir_gyro_yout, normalizedData.fir_gyro_zout, 
            normalizedData.fir_temp_out
        );
        printf("[DRIVER_HANDLER] finalDataVector=\n%s\n", finalDataVector);

        /* Copy the data to the shared memory */
        write_to_shared_memory(finalDataVector);

        /* [Optional] Inclination to give a more meaningful  */
        calculate_inclination(normalizedData.accel_xout, normalizedData.accel_yout, normalizedData.accel_zout);

        /* reload server config.ini */
        //printf("[DRIVER_HANDLER] Signal SIGUSR2 with PID: %d\n", driver_pid);
        kill(driver_pid, SIGUSR2);

        sleep(DRIVER_HANDLER_SLEEP);
    }

    printf("[DRIVER_HANDLER] driver_handler resources released\n");

    free(driverBuffer);
    close(driverFileDesc);  // Close driver as Input/output system call

    return;
}

/*****************************************************************************/

/* Finite Impulse Response filter:
    y[n] = sum_{i=0 -> N-1} h[i] * x[n-i]
    y[n]: output sample at time n
    h[i]: filter coefficients
    x[n-i]: input sample at time n-i 

    example: y[n] = h0 * x[n] + h1 * x[n-1] + h2 * x[n-2]
*/
float firFilter(float* buffer, int length, int sample, float* coeffs)
{
  float aux = 0.0;

  for (int i = 0; i < (length - 1); i++)
  {
    buffer[length - (i+1)] = buffer[length - (i+1) - 1];
  }

  buffer[0] = (float)sample;

  for (int i = 0; i < length; i++)
  {
    aux = aux + buffer[i] * coeffs[i];
  }

  return aux;
}

/*****************************************************************************/

/* Calculation of inclination angles using accelerometer data:

    Pitch (rotation around X-axis):
        theta_x = atan2(accel_x, sqrt(accel_y^2 + accel_z^2)) * (180.0 / M_PI)

    Roll (rotation around Y-axis):
        theta_y = atan2(accel_y, sqrt(accel_x^2 + accel_z^2)) * (180.0 / M_PI)

    Yaw (rotation around Z-axis, typically obtained from gyroscope):
        theta_z = atan2(sqrt(accel_x^2 + accel_y^2), accel_z) * (180.0 / M_PI)
    
    These calculations determine the device's orientation based on gravity.
*/
void calculate_inclination(float accel_x, float accel_y, float accel_z) {
    float pitch = atan2(accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * (180.0 / M_PI);
    float roll  = atan2(accel_y, sqrt(accel_x * accel_x + accel_z * accel_z)) * (180.0 / M_PI);
    float yaw   = atan2(sqrt(accel_x * accel_x + accel_y * accel_y), accel_z) * (180.0 / M_PI);

    printf("[INCLINATION] Pitch: %.2f°\n", pitch);
    printf("[INCLINATION] Roll: %.2f°\n", roll);
    printf("[INCLINATION] Yaw: %.2f°\n", yaw);
}
