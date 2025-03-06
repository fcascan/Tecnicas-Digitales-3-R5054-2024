#include "../inc/driver_test.h"

int main() 
{
    printf("[TEST]: Test started\n");

    float   rawaccelxout, rawaccelyout, rawaccelzout, 
            rawgyroxout, rawgyroyout, rawgyrozout, 
            rawtempout,
            mvaaccelxout, mvaaccelyout, mvaaccelzout, 
            mvatempout,
            mvagyroxout, mvagyroyout, mvagyrozout;

    // Specify the file path
    const char *file_path = DRIVER_PATH;
    printf("[TEST]: file_path=%s\n", file_path);

    // Open the file for reading
    printf("[TEST]: before open()\n");
    int file_descriptor = open(file_path, O_RDONLY);
    printf("[TEST]: file_descriptor=%i\n", file_descriptor);
    if (file_descriptor == -1) 
    {
        perror("Error opening file");
        return 1;
    }

    printf("[TEST]: before malloc\n");
    uint8_t *buff_driver = malloc(BUFFER_SIZE * sizeof(uint8_t));
    printf("[TEST]: mallloc is working correctly\n");

    if (buff_driver == NULL)
    {
        printf("[TEST]: Error al asignar memoria dinamica\n");
        close(file_descriptor);  // Cerrar el descriptor de archivo antes de salir
        return 1;
    }

    // Read and display the contents of the file
    printf("[TEST]: Reading started\n");

    while(1)
    {
        ssize_t bytes_read = read(file_descriptor, buff_driver, BUFFER_SIZE);

        if (bytes_read < 0) 
        {
            perror("Error reading from file");
            close(file_descriptor);     // Close the file descriptor before exiting
            free(buff_driver);  // Liberar la memoria antes de salir
            return 1;
        }
        
        if (bytes_read == 0) 
        {
            // Se llegó al final del archivo
            printf("[TEST]: Fin del archivo alcanzado.\n");
            break;
        }

        if (bytes_read != BUFFER_SIZE)
        {
            printf("[TEST]: Warning: Se leyeron solo %ld bytes, se esperaban %d bytes\n", bytes_read, BUFFER_SIZE);
            break;
        }

        printf("[TEST]: Leí %ld bytes\n", bytes_read);

        if (bytes_read == BUFFER_SIZE)
        {
            rawaccelxout = (float)((0xFF & buff_driver[0]) << 8 | (0xFF & buff_driver[1]))	* ARES_2G;
            printf("[TEST]: rawaccelxout = %lf\n", rawaccelxout);
            rawaccelyout = (float)((0xFF & buff_driver[2]) << 8 | (0xFF & buff_driver[3]))	* ARES_2G;
            printf("[TEST]: rawaccelyout = %lf\n", rawaccelyout);
            rawaccelzout = (float)((0xFF & buff_driver[4]) << 8 | (0xFF & buff_driver[5]))	* ARES_2G;
            printf("[TEST]: rawaccelzout = %lf\n", rawaccelzout);

            rawtempout	 = (float)((0xFF & buff_driver[6]) << 8 | (0xFF & buff_driver[7]))/ 340 + 36.53;
            printf("[TEST]: rawtempout = %lf\n", rawtempout);

            rawgyroxout  = (float)((0xFF & buff_driver[8]) << 8 | (0xFF & buff_driver[9])) 	* GYR_250;
            printf("[TEST]: rawgyroxout = %lf\n", rawgyroxout);
            rawgyroyout  = (float)((0xFF & buff_driver[10]) << 8 | (0xFF & buff_driver[11])) * GYR_250;
            printf("[TEST]: rawgyroyout = %lf\n", rawgyroyout);
            rawgyrozout  = (float)((0xFF & buff_driver[12]) << 8 | (0xFF & buff_driver[13])) * GYR_250;
            printf("[TEST]: rawgyrozout = %lf\n", rawgyrozout);
        }
    }

    printf("[TEST]: Read: %p \n", (void*)buff_driver);

    free(buff_driver);  // Liberar la memoria al final

    // Close the file when done
    close(file_descriptor);

    return 0;
}
