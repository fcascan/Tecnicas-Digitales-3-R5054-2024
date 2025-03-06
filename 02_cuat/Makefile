SERVER_PATH= ./01_server/src
SERVER_FILENAME_OUT= server
CLIENT_PATH= ./02_client/src
CLIENT_FILENAME_OUT= client
RANDOM_SCRIPT_PATH = ./03_random
RANDOM_FILENAME = random_generator.sh

PORT= 5679	# 5678 5679 5680 5682
IP ?= localhost  # If IP is not set, default to localhost

quick: do_server do_clients do_random

# Compilación y ejecución del servidor
do_server: clean_server compile_server run_server

# Compilación y ejecución de los clientes
do_client: clean_client compile_client run_client

# Apertura de una terminal con el script random.sh
do_random: run_random

#######################################################################################

clean_server:
	rm -f $(CLIENT_PATH)/$(CLIENT_FILENAME_OUT)

compile_server:
	gcc -o $(SERVER_PATH)/$(SERVER_FILENAME_OUT) $(SERVER_PATH)/server.c $(SERVER_PATH)/driver_handler.c $(SERVER_PATH)/my_signals.c $(SERVER_PATH)/shared_memory.c $(SERVER_PATH)/config_handler.c -lpthread -lm

run_server:
	@echo ""
	$(SERVER_PATH)/$(SERVER_FILENAME_OUT) $(PORT)

clean_client:
	rm -f $(SERVER_PATH)/$(SERVER_FILENAME_OUT)

compile_client:
	gcc -o $(CLIENT_PATH)/$(CLIENT_FILENAME_OUT) $(CLIENT_PATH)/$(CLIENT_FILENAME_OUT).c

run_client:
	@echo ""
	$(CLIENT_PATH)/$(CLIENT_FILENAME_OUT) $(IP) $(PORT)

run_random:
	gnome-terminal -- /bin/bash -c "chmod +x $(RANDOM_SCRIPT_PATH)/$(RANDOM_FILENAME); $(RANDOM_SCRIPT_PATH)/$(RANDOM_FILENAME); exec bash"
