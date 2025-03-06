

    *********************************************************************************************************
    *                                         TP01 - Ejercicio 3                                            *
    *********************************************************************************************************
    -> fcastrocanosa
    -> Legajo: 141324-7
    *********************************************************************************************************
    *                           INTERRUPCIONES DE HW Y EXCEPCIONES - TIMER TICK                             *
    *                                                                                                       *
    * Se requiere continuar el ejercicio anterior habilitando únicamente la IRQ asociada al temporizador y  *
    * la atención de excepciones de los siguientes tipos:                                                   *
    *  - Invalid instructions and trap exceptions                                                           *
    *  - Memory accesses                                                                                    *
    *  - Exception-generating instructions. Supervisor Call (SVC) solamente                                 *
    *                                                                                                       *
    * Cada rutina de atención de excepción deberá almacenar en r10 los caracteres del tipo que la generó    *
    * ("INV", "MEM", "SVC"), y permitir que el procesador puede continuar con la ejecución normal del       *
    * código.                                                                                               *
    * Cada excepción se debe generar por una función específica. Se sugiere que la ejecución de dichas      *
    * funciones pueda des/habilitarse mediante una opción de construcción del binario, como por ejemplo     *
    * make exceptions.                                                                                      *
    *                                                                                                       *
    * Se debe configurar el TIMER0 de forma que genere una interrupción cada 10ms y la rutina de atención a * 
    * la IRQ asociada (#36) debe almacenar en r10 la cantidad de veces que fue invocada. Recordar           *
    * configurar adecuadamente el GIC0.                                                                     *
    *                                                                                                       *
    * El código una vez finalizada la configuración de todos los recursos de HW descriptos y generadas las  *
    * excepciones, según la metodología de construcción del binario escogida, debe quedarse en un bucle que *
    * suspenda la ejecución (detener el reloj principal del core) hasta recibir un evento IRQ               *
    *                                                                                                       *
    * Objetivos conceptuales:                                                                               *
    * I. Familiarizarse con IRQ vs FIQ.                                                                     *
    * II. Comprender la atención de excepciones.                                                            *
    * III. Comprender la importancia de poner el procesador en alta impedancia para reducir el consumo de   *
    * energía mientras esperamos por eventos.                                                               *
    *********************************************************************************************************
    Comentarios: -
