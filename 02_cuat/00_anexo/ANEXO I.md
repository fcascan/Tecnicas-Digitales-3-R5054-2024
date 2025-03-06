    *********************************************************************************************************
    *                                          Apéndice I                                                   *
    *********************************************************************************************************
    -> fcastrocanosa
    -> Legajo: 141324-7
    *********************************************************************************************************
    * • Noción de la comunicación HTTP:                                                                     *
    * En el marco del protocolo HTTP, el navegador (cliente) enviará una de las siguientes solicitudes      *
    * GET ( request en términos HTTP), cuyas respuestas (status en términos HTTP) por parte del server se   *
    * especifican en cada caso:                                                                             *
    *                                                                                                       *
    * 1. Solicitud de contenido:                                                                            *
    *   Request:                                                                                            *
    *   GET / HTTP/1.1                                                                                      *
    *                                                                                                       *
    *   Respuesta:                                                                                          *
    *   Si el método es correcto:                                                                           *
    *   HTTP/1.1 200 Ok                                                                                     *
    *   Content-Type: text/html; charset=UTF-8\n                                                            *
    *   Content-Length: %d\n El número es: strlen(HTML)                                                     *
    *   \n                                                                                                  *
    *   ... acá escribir el código HTML ...                                                                 *
    *                                                                                                       *
    *   Si el método solicitado no es GET:                                                                  *
    *   HTTP/1.1 400 Bad method                                                                             *
    *   Si el recurso no es /                                                                               *
    *   HTTP/1.1 404 Bad resource                                                                           *
    *                                                                                                       *
    * 2. Solicitud de elemento gráfico:                                                                     *
    *   Request:                                                                                            *
    *   GET /Archivo-grafico HTTP/1.1                                                                       *
    *   Respuesta:                                                                                          *
    *                                                                                                       *
    *   Si el método es correcto:                                                                           *
    *   HTTP/1.1 200 Ok                                                                                     *
    *   Content-Type: image/png\n                                                                           *
    *   Content-Length: %d\n El número es: sizeof(buffer_imagen)                                            *
    *   \n                                                                                                  *
    *   ... acá copiar el gráfico en binario ...                                                            *
    *                                                                                                       *
    *   Si el método solicitado no es GET:                                                                  *
    *   HTTP/1.1 400 Bad method                                                                             *
    *   Si el recurso no es /Archivo-grafico                                                                *
    *   HTTP/1.1 404 Bad resource                                                                           *
    *                                                                                                       *
    * 3. Para errores 400 o 404:                                                                            *
    *   Content-Type: text/html; charset=UTF-8\n                                                            *
    *   Content-Length: %d\n El número es: strlen(HTML)                                                     *
    *   \n                                                                                                  *
    *   ... acá escribir el código HTML que muestre el error en pantalla ...                                *
    *                                                                                                       *
    *********************************************************************************************************
