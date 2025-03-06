

    *********************************************************************************************************
    *                                         TP01 - Ejercicio 1                                            *
    *********************************************************************************************************
    -> fcastrocanosa
    -> Legajo: 141324-7
    *********************************************************************************************************
    *                                        ABI Y ASSEMBLER ARMV7                                          *
    *                                                                                                       *
    * Modificar el ejemplo del segundo test provisto en Gitlab en el item 2, que en el vector de arranque   *
    * pueda copiar una  sección de código de lo que será el micro kernel de nuestro sistema operativo, en   *
    * la dirección 0x70030000. Para tal fin implementar:                                                    *
    *                                                                                                       *
    *                 void *td3_memcopy(void *destino, const void *origen, unsigned int num_bytes);         *
    *                                                                                                       *
    * Una vez que se copia esta sección, el programa en el vector de arranque debe saltar al código de la   *
    * sección copiada, en donde quedará en un ciclo infinito.                                               *
    * La pila debe ubicarse en 0x70020000.                                                                  *
    * Las dos secciones de código deben quedar ensambladas una atrás de la otra. Para tal fin se sugiere    *
    * utilizar un archivo de linker script de modo de gestionar las áreas de memoria durante el ensamblado  *
    * del binario.                                                                                          *
    *                                                                                                       *
    * Objetivos conceptuales:                                                                               *
    * I. Familiarizarse con los comandos de Qemu y su utilización                                           *
    * II. Familiarizarse con Linker Script                                                                  *
    * III. Entender el concepto de ABI para poder utilizar convenientemente C y ASM, aprovechando sus       *
    * ventajas para cada caso.                                                                              *
    * IV.Entender manejo de pila R13                                                                        *
    * V. Entender como cargar el registro LR para poder retornar de las funciones.                          *
    *********************************************************************************************************
    Comentarios: -
