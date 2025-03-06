    *********************************************************************************************************
    *                                         TP01 - Ejercicio 5                                            *
    *********************************************************************************************************
    -> fcastrocanosa
    -> Legajo: 141324-7
    *********************************************************************************************************
    *                                        TASK SCHEDULER SIMPLE                                          *
    *                                                                                                       *
    * Tomar el ejercicio anterior, y agregar dos tareas.                                                    *
    * -> Tarea 1: deberá corroborar el correcto funcionamiento de la RAM (escribiendo y luego leyendo lo    *
    * escrito) en el rango 0x70A00000 a 0x70A0FFFF, utilizando el word 0x55AA55AA. Debe resguardar la       *
    * información antes de escribir cada bloque.                                                            *
    * -> Tarea 2: deberá recorrer la RAM en el rango 0x70A10000 a 0x70A1FFFF. En cada porción de memoria,   *
    * deberá hacer un toggle de lo leído, es decir, si lee un bit en 1 lo debe pasar a 0, y viceversa.      *
    *                                                                                                       *
    * La adminsitración de la ejecución de las tareas se realizará desde un scheduler que operará dentro    *
    * de un timeframe de 100ms. Las tareas se ejecutarán durante 10ms antes de ser suspendidas hasta el     *
    * próximo time frame.                                                                                   *
    * Las tareas corren indefinidamente.                                                                    *
    * El tiempo remanente del timeframe lo ocupará una tercer tarea que ponga el procesador en un estado de *
    * bajo consumo de energía, hasta el inicio del próximo frame, o hasta que ocurra un evento asincrónico. *
    *                                                                                                       *
    * El mapa de memoria propuesto es el siguiente:                                                         *
    *                                                                                                       *
    *   |——————————————————————————————————————————————————————————————————————————————|                    *
    *   |            Sección             |   Dirección lineal   |   Dirección Física   |                    *
    *   |——————————————————————————————————————————————————————————————————————————————|                    *
    *   | Tarea 2 .readingArea           |      70A10000h       |      80010000h       |                    *
    *   | Tarea 1 .readingArea           |      70A00000h       |      80000000h       |                    *
    *   | Tarea 1 .text                  |      70F50000h       |      80750000h       |                    *
    *   | Tarea 1 .data                  |      70F51000h       |      80751000h       |                    *
    *   | Pila Tarea 1                   |      70F52000h       |      80752000h       |                    *
    *   | Tarea 1 .bss                   |      70F53000h       |      80753000h       |                    *
    *   | Tarea 1 .rodata                |      70F54000h       |      80754000h       |                    *
    *   | Tarea 2 .text                  |      70F40000h       |      80740000h       |                    *
    *   | Tarea 2 .data                  |      70F41000h       |      80741000h       |                    *
    *   | Tarea 2 .bss                   |      70F43000h       |      80743000h       |                    * 
    *   | Tarea 2 .rodata                |      70F44000h       |      80744000h       |                    * 
    *   | Pila Tarea 2                   |      70F42000h       |      80742000h       |                    *
    *   | Kernel .data (datos y tablas)  |      70031000h       |      70031000h       |                    *
    *   | Kernel .text                   |      70030000h       |      70030000h       |                    *
    *   | Kernel .rodata                 |      70023000h       |      70023000h       |                    *
    *   | Kernel .bss                    |      70022000h       |      70022000h       |                    *
    *   | Pila Kernel                    |      70020000h       |      70020000h       |                    *
    *   | Secuencia inicialización ROM   |      70010000h       |      70010000h       |                    *
    *   | Vector de INT                  |      00000000h       |      00000000h       |                    *
    *   |——————————————————————————————————————————————————————————————————————————————|                    *
    *                                                                                                       *
    * Observar que no se define un área para la tercer tarea.                                               *
    *                                                                                                       *
    * Objetivos conceptuales:                                                                               *
    * I. Comprender la protección de espacios de memoria por paginación.                                    *
    * II. Comprender como funciona una sistema operativo básico.                                            *
    *********************************************************************************************************
    Comentarios: -
