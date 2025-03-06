#include "../inc/config_handler.h"

/*****************************************************************************/

/* load configuration from ini file silently */
void silent_load_server_config()
{
    load_server_config_internal(1);
}

/* load configuration from ini file with logs */
void load_server_config()
{
    load_server_config_internal(0);
}

/* internal function to load configuration from ini file */
void load_server_config_internal(int silent)
{
    char line[LINE_LENGHT];
    int max_clients_success_read = 0;
    int random_script_location_success_read = 0;
    int producer_mode_success_read = 0;
    int fir_window_length_aux = 0;
    int fir_window_length_read = 0;
    int filter_coeffs_index = 0;
    float filter_coeffs_aux = 0.0;
    int filter_coeffs_read = 0;

    FILE *file = fopen(CONFIG_INI_FILE, "r");
    if (file)
    {
        while (fgets(line, sizeof(line), file))
        {
            if (sscanf(line, "max_simultaneous_clients = %d", &max_simultaneous_clients) == 1)
            {
                max_clients_success_read = 1; // successfully read
            }
            else if (sscanf(line, "random_script_location = %s", random_script_location) == 1)
            {
                random_script_location_success_read = 1; // successfully read
            }
            else if (sscanf(line, "producer_mode = %d", &producer_mode) == 1)
            {
                producer_mode_success_read = 1; // successfully read
            }
            else if (sscanf(line, "fir_window_length = %d", &fir_window_length_aux) == 1)
            {
                fir_window_length = fir_window_length_aux;
                fir_window_length_read = 1; // successfully read
            }
            else if (sscanf(line, "filter_coeffs_%d = %f", &filter_coeffs_index, &filter_coeffs_aux) == 2)
            {
                filter_coeffs[filter_coeffs_index] = filter_coeffs_aux;
                filter_coeffs_read += 1; // +1 for each successfully read filter coefficient
            }
        }
        fclose(file);

        /* Assign default values if they were not read correctly */
        if (!max_clients_success_read)
        {
            max_simultaneous_clients = MAX_SIMULTANEOUS_CLIENTS_DEFAULT;
            printf("[CONFIG_HANDLER] Error: Cannot read max_simultaneous_clients from config.ini. Using default value: %d\n", MAX_SIMULTANEOUS_CLIENTS_DEFAULT);
        }
        if (!random_script_location_success_read)
        {
            strcpy(random_script_location, RANDOM_GENERATOR_DEFAULT_LOCATION);
            printf("[CONFIG_HANDLER] Error: Cannot read random_script_location from config.ini. Using default value: %s\n", RANDOM_GENERATOR_DEFAULT_LOCATION);
        }
        if (!producer_mode_success_read)
        {
            producer_mode = PRODUCER_MODE_DEFAULT;
            printf("[CONFIG_HANDLER] Error: Cannot read producer_mode from config.ini. Using default value: %d\n", PRODUCER_MODE_DEFAULT);
        }
        if (!fir_window_length_read)
        {
            fir_window_length = FIR_WINDOW_LENGTH_DEFAULT;
            printf("[CONFIG_HANDLER] Error: Cannot read fir_window_length from config.ini. Using default value: %d\n", FIR_WINDOW_LENGTH_DEFAULT);
        }
        if (filter_coeffs_read != 10)
        {
            for (int i = 0; i < 10; i++)
            {
                filter_coeffs[i] = i;
            }
            printf("[CONFIG_HANDLER] Error: Cannot read filter_coeffs from config.ini.\n");
        }
        if (!silent) printf("[CONFIG_HANDLER] max_simultaneous_clients = %d\n", max_simultaneous_clients);
        if (!silent) printf("[CONFIG_HANDLER] random_script_location = %s\n", random_script_location);
        if (!silent) printf("[CONFIG_HANDLER] producer_mode = %d (0 = Random, 1 = I2C MPU6050)\n", producer_mode);
        if (!silent) printf("[CONFIG_HANDLER] fir_window_length = %d\n", fir_window_length);
        for(int i = 0; i < 10 ; i++)
        {
            if (!silent) printf("[CONFIG_HANDLER] filter_coeffs_%d = %f\n", i, filter_coeffs[i]);
        }
    }
    else 
    {
        if (!silent) printf("[CONFIG_HANDLER] Error: Cannot open config.ini. Using default values from server.h\n");
        max_simultaneous_clients = MAX_SIMULTANEOUS_CLIENTS_DEFAULT;
        strcpy(random_script_location, RANDOM_GENERATOR_DEFAULT_LOCATION);
        producer_mode = PRODUCER_MODE_DEFAULT;
        fir_window_length = FIR_WINDOW_LENGTH_DEFAULT;
    }
}

/*****************************************************************************/

/* write filter coefficients to config.ini */
void write_in_config_filter_coeff() {
    FILE *file = fopen(CONFIG_INI_FILE, "r+");
    if (!file) {
        perror("[CONFIG_HANDLER] Error opening config.ini for writing");
        return;
    }

    char line[LINE_LENGHT];
    long int last_pos = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "filter_coeffs_", 14) == 0) {
            last_pos = ftell(file) - strlen(line);
            break;
        }
    }
    fseek(file, last_pos, SEEK_SET);

    for (int i = 0; i < 10; i++) {
        fprintf(file, "filter_coeffs_%d = %.4f\n", i, filter_coeffs[i]);
    }

    fclose(file);
}
