*********************************************************************************************************
*                                         Apéndice II                                                   *
*********************************************************************************************************
-> fcastrocanosa
-> Legajo: 141324-7
*********************************************************************************************************
* Este Trabajo Práctico no pretende más que una simple aplicación de la teoría de Filtrado Digital.     *
* Por tal motivo, el objetivo es implementar un algoritmo IIR o FIR que simplemente implemente un tipo  *
* de filtro digital de acuerdo con lo especificado a cada estudiante.                                   *
* Por lo tanto, para el cálculo de los coeficientes de cada filtro, se sugiere el uso de pyfda, una     *
* aplicación pública de uso libre, que permitirá obtener muy sencillamente los coeficientes para        *
* incluir en el algoritmo.                                                                              *
*                                                                                                       *
* Para instalar la aplicación, se requieren estos simples pasos (Ubuntu 20.04):                         *
* 1. Verificar la versión de numpy: `pip3 list | grep numpy`                                            *
* 2. Si no se verifica que 1.18.5 <= numpy < 1.25.0, ejecutar:                                          *
*    pip3 install numpy==1.23.2                                                                         *
*    La versión 1.23.2 es la más reciente que se dispone en Ubuntu 20.04. En caso de no estar presente  *
*    esta versión, el comando pip3 mostrará la lista de versiones disponibles: Se recomienda instalar   *
*    la más nueva entre las que se adapten a la condición requerida.                                    *
* 3. Descargar el siguiente repositorio:                                                                *
*    https://github.com/chipmuenk/pyfda                                                                 *
* 4. Ejecutar: `pip3 install pyfda --user                                                               *
* 5. En caso que el paso 2 arroje el siguiente error:                                                   *
*    ERROR: scipy 1.9.0 has requirement numpy<1.25.0,>=1.18.5, but you'll have numpy 1.17.4 which is    *
*    incompatible.`                                                                                     *
*    Verificar si el paso 2 salió correctamente. Si la versión de numpy se instaló correctamente,       *
*    este mensaje no debe aparecer.                                                                     *
* 6. Recibirá un warning en la instalación respecto de modificar la variable de entorno PATH. Para      *
* ello, editar el archivo `~/.bashrc`. Agregar al final la siguiente línea:                             *
*    export PATH=$PATH:~/data/git/pyfda:~/.local/bin                                                    *
*    Reemplace `~/data/git/pyfda` por el path en el que se encuentre su repositorio.                    *
*                                                                                                       *
* Para ejecutarla desde el prompt (en cualquier punto del File System, si el paso 6 se realizó          *
* correctamente), ejecutar: `pyfdax`                                                                    *
*                                                                                                       *
* Una vez abierta la aplicación, se arriba a la siguiente pantalla:                                     *
*                                                                                                       *
* (Ver Fig. 1)                                                                                          *
*                                                                                                       *
* En la ventana resaltada seleccionar las características del filtro.                                   *
* Seguidamente pulsar el botón Filter Design que habrá cambiado su color verde a amarillo indicando     *
* que hay que refrescar la vista.                                                                       *
* Una vez pulsado, se verá el gráfico correspondiente. En ese momento se selecciona la pestaña “b,a”    *
* en la parte superior izquierda para visualizar los coeficientes que deben utilizarse en el algoritmo  *
* del filtro, vista que se puede apreciar en la siguiente figura.                                       *
* Pulsando el sexto botón desde la izquierda en el panel de los coeficientes se puede salvar en un      *
* archivo CSV los coeficientes para luego copiarlos en el archivo de configuración.                     *
*                                                                                                       *
* Eso es todo.                                                                                          *
*                                                                                                       *
* (Ver Fig.2)                                                                                           *
*                                                                                                       *
*********************************************************************************************************
