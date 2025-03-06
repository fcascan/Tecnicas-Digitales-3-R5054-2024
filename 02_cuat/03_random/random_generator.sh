#!/bin/bash
#En resumen, este script genera continuamente números aleatorios, los guarda en un archivo, y envía una señal al servidor cada vez que se genera un número.

SERVER_PID=$1  # PID del proceso del servidor
OUTPUT_FILE="03_random/random_number.txt"
#OUTPUT_FILE="/home/fcc/Documentos/TD3-2024/workspace/r5054-CASTRO-CANOSA-FERNANDO-ARIEL/02_cuat/03_random/random_number.txt"
SLEEP_DURATION=0.10 # Generar números cada 0.1 segundos

while true
do
  # Verificar si el proceso del servidor está en ejecución
  kill -0 $SERVER_PID
  if [ $? -ne 0 ]; then
    echo "Server process not running. Exiting."
    exit 1
  fi

  RANDOM_NUMBER=$RANDOM
  #echo $RANDOM_NUMBER
  echo $RANDOM_NUMBER > $OUTPUT_FILE  # Guardar el número generado en un archivo
  kill -SIGUSR1 $SERVER_PID           # Enviar señal SIGUSR1 al servidor
  sleep $SLEEP_DURATION               
done
