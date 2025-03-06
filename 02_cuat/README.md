    *********************************************************************************************************
    *                                                TP02                                                   *
    *********************************************************************************************************
    -> fcastrocanosa
    -> Legajo: 141324-7
    *********************************************************************************************************
    *                                         EJERCICIO INTEGRADOR                                          *
    *                                                                                                       *
    * • Objetivo:                                                                                           *
    * El presente desarrollo tiene por objeto obtener, por encuesta y en forma remota, lecturas de un       *
    * sensor, procesarlas y visualizarlas en el navegador de una PC.                                        *
    * Se busca ejercitar al alumno en el diseño de un driver para acceder a un dispositivo, e               *
    * implementación de un servidor para distribuir esa información de forma remota.                        *
    * La funcionalidad trata de cubrir una eventual facilidad para ensayos sísmicos en dispositivos         *
    * electromecánicos, análisis sísmicos y resonantes en puentes, estructuras, etc                         *
    *                                                                                                       *
    * • Requisitos:                                                                                         *
    * Este año nos proponemos procesar información de un acelerómetro de 3 ejes I2C. La información de cada *
    * eje será recolectada con una frecuencia apropiada para poder realizar el análisis frecuencial         *
    * correspondiente. Se procesará dicha información para obtener valores absolutos de aceleración, y      *
    * capacidad de análisis de aceleración por ventana de frecuencia.                                       *
    *                                                                                                       *
    * • Arquitectura:                                                                                       *
    * El sistema a implementar debe estar compuesto por una estación de visualización (PC), un dispositivo  *
    * de adquisición (BeagleBone Black) y un acelerómetro I2C.                                              *
    *                                                                                                       *
    * • Estación de visualización:                                                                          *
    * Esta parte del proyecto solo se limita a hacer solicitudes periódicas al servidor y presentarlo en    *
    * pantalla. A tal fin se utilizará en una PC una de estas dos soluciones a elección:                    *
    * a) Una aplicación escrita en Python provista por la cátedra encargada de la visualización de datos y  *
    * presentación de gráficos en tiempo real, permitiendo seleccionar a cada dispositivo de adquisición a  *
    * interrogar, mediante su dirección IP.                                                                 *
    * b) Un navegador de Internet para lo cual se empaquetará la información de acuerdo con el protocolo    *
    * HTML 1.1. de acuerdo con lo estipulado en el Apéndice I.                                              *
    *                                                                                                       *
    * • Dispositivo de adquisición:                                                                         *
    * El dispositivo de adquisición deberá satisfacer las siguientes funcionalidades y requerimientos:      *
    * 1. Gestionar el dispositivo de medición (sensor), mediante un controlador (driver) implementado como  *
    * un módulo de kernel (.ko), acorde con la especificación del Linux Kernel Driver Model (LKDM).         *
    * 2. El driver deberá implementar la API POSIX (open, close, read, write, ioctl) de un char device,     *
    * gestionar el acceso y configuración del sensor elegido mediante la interfaz serie disponible en el    *
    * módulo de hardware, configurándolo en modo maestro e inicializar el periférico correspondiente,       *
    * empleando parámetros de configuración establecidos en el device tree.                                 *
    * 3. El módulo deberá poder instalarse y desinstalarse en forma dinámica                                *
    * 4. Una vez instalado, el módulo deberá recolectar, procesar y almacenar los datos del dispositivo de  *
    * medición, teniendo presente la eventual concurrencia de procesos lectores.                            *
    * 5. Se deberá mantener activo un servidor TCP concurrente que recibirá solicitudes periódicas desde    *
    * las estaciones de visualización a las que enviará los datos. El servidor deberá cumplir los           *
    * siguientes requisitos / funcionalidades:                                                              *
    *   5.1. Archivo de configuración:                                                                      *
    *   Archivo de texto plano que contiene las variables de estado del servidor, a razón de una línea por  *
    *   variable cada una de las cuales es un valor numérico en representación ASCII:                       *
    *                                                                                                       *
    *   [Cantidad Máxima de Conexiones]. Valor defaut = 100.                                                *
    *   [Cantidad de pedidos de conexión que bufferea (backlog)].                                           *
    *   Valor defaut = 20.                                                                                  *
    *   [Coeficientes del filtro]. Valor default = FIR pasabajos                                            *
    *   Equiripple, Grid Density=16, N=10. Ver Apendice II. Lista separada por comas de los valores que     *
    *   surgen de la herramienta pyfda.                                                                     *
    *                                                                                                       *
    *   En caso de no existir el archivo de configuración, el servidor utilizará como valor inicial los     *
    *   tres valores señalados como defaut.                                                                 *
    *   5.2.Cada vez que el servidor concurrente recibe la señal SIGUSR2, relee el archivo de configuración *
    *   y recarga sus variables de estado con los parámetros allí encontrados de cantidad de conexiones     *
    *   activas, y tamaño de la ventana del filtro. Esta actividad no debe interrumpir la labor de los      *
    *   procesos child, ni afectar el normal funcionamiento del proceso padre. Por tal motivo el parámetro  *
    *   backlog solo se aplica cuando inicia el server. Nunca se recarga con SIGUSR2 ya que implica         *
    *   reiniciar el server.                                                                                *
    *                                                                                                       *
    *   5.3.Control de conexiones y pedidos de conexión:                                                    *
    *   Limitar la cantidad de pedidos de conexiones simultáneos (en cola o backlog), así como la cantidad  *
    *   máxima de procesos que se creen para atender a los clientes que requieran datos (o sea la cantidad  *
    *   de conexiones activas), de acuerdo con las dos primeras variables en el archivo de configuración.   *
    *                                                                                                       *
    *   5.4.Adquisición de datos:                                                                           *
    *   Obtener las muestras (con periodicidad adecuada) y almacenar apropiadamente en un buffer.           *
    *   Se sugiere consultar:                                                                               *
    *   AM335x and AMIC110 SitaraTM Processors, Technical Reference Manual. Texas Instruments.              *
    *   Disponible en: https://www.ti.com/lit/ug/spruh73q/spruh73q.pdf                                      *
    *   Siendo que se debe satisfacer las syscalls definidas en el estándar POSIX, donde las funciones      *
    *   read () y write () reciben un argumento entero en el que se especifica la cantidad de bytes a leer  *
    *   o a escribir, y cuyo rango de valores es cualquier entero positivo, el driver debe manejar esta     *
    *   situación, sin afectar el comportamiento de las syscalls.                                           *
    *                                                                                                       *
    *   5.5.Procesamiento digital.                                                                          *
    *   Obtener el valor absoluto de aceleración instantánea como:                                          *
    *   a = √((a_x)² + (a_y)² + (a_z)²)                                                                     *
    *                                                                                                       *
    *   Aplicar un filtrado digital de tipo pasa banda de corte abrupto y con ancho de banda y frecuencia   *
    *   central ajustable en 3 valores distintos. Estos conjuntos de valores serán seleccionados mediante   *
    *   el archivo de configuración La función que implementa el filtro se escribirá en un archivo fuente   *
    *   separado del resto.                                                                                 *
    *   Para calcular los coeficientes del filtro que le haya sido asignado a cada estudiante, se detalla   *
    *   la aplicación a utilizar en Apéndice II.                                                            *
    *                                                                                                       *
    *   ------------------------------------------------------------------------------------------------    *
    *   Curso Piloto:                                                                                       *
    *   La función de filtrado se escribe utilizando las extensiones NEON del Cortex A8.                    *
    *   ------------------------------------------------------------------------------------------------    *
    *   Curso estándar:                                                                                     *
    *   Opción 1: Implementarla como el curso piloto (deseable).                                            *
    *   Opción 2: La función se escribe en C. Compilar con -OFast de modo de forzar al compilador a una     *
    *   optimización más agresiva respecto del procesador. De este modo es esperable que el compilador      *
    *   utilice instrucciones SIMD disponibles en el procesador CORTEX-A8.                                  *
    *   Además de los programas fuente se pide un documento con el análisis de las instrucciones SIMD       *
    *   (NEON) empleadas por el compilador en esta función. Para ello compilar este archivo fuente con la   *
    *   opción -OFast y -S, esta última generará el código en lenguaje ensamblador. Analizando la           *
    *   descripción de las instrucciones SIMD involucradas, se pide la descripción del algoritmo empleado   *
    *   con la mirada puesta en las instrucciones vectoriales y como estas eventualmente inciden en la      *
    *   aceleración del mismo.                                                                              *
    *   ------------------------------------------------------------------------------------------------    *
    *                                                                                                       *
    *   El set de instrucciones NEON se describe en el apéndice C del documento NEON Programmer’s Guide,    *
    *   versión 1.0, disponible en:                                                                         *
    *   https://documentation-service.arm.com/static/5f731b591b758617cd95559c?token=                        *
    *                                                                                                       *
    *   5.6.Transmisión de datos                                                                            *
    *   Por cada solicitud de los clientes, el servidor transmite a las estaciones de monitoreo el vector   *
    *   de los valores adquiridos en los dos canales, seguido por el vector de los respectivos valores      *
    *   filtrados, sin ningún separador.                                                                    *
    *                                                                                                       *
    *   5.7.Presentación de datos                                                                           *
    *   Se lleva a cambo en el cliente utilizando las capacidades gráficas de la Aplicación Python o HTML   *
    *   1.1                                                                                                 *
    *                                                                                                       *
    * • Primer Recuperatorio:                                                                               *
    * Implementar un segundo filtro.                                                                        *
    *                                                                                                       *
    * • Segundo Recuperatorio:                                                                              *
    * En caso que un proceso intente abrir el dispositivo cuando éste ya está siendo utilizado, el mismo    *
    * deberá dormir hasta que se libere el recurso.                                                         *
    *                                                                                                       *
    *********************************************************************************************************
    Comentarios: -



