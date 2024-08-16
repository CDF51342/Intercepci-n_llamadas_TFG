Primero de todo, es necesario ejecutar el servidor, debido a que indicará la dirección IP a conectarse y su puerto de escucha.

# SERVIDOR
1. Antes de ejecutar el servidor, es necesario establecer la variable de entorno PATH_LOGSTASH, la cuál indica la ruta al fichero log donde se almacenan los datos:

    ```bash
    export PATH_LOGSTASH=<path>
    ```

    Donde `<path>` es la ruta deseada.

2. Para ejecutar el servidor, es necesario tener el ejecutable e insertar en terminal:
    
    ```bash
    ./servidor -p <puerto>
    ```

    Donde `<puerto>` es el puerto de escucha deseado.

3. Una vez ejecutado, por pantalla aparece la dirección IP junto al puerto del servidor:

    ```
    Operating in >>> <IP>:<puerto>
    ```

# CLIENTE
1. Para poder inicializar el programa de client, es necesario exportar 3 variables de entorno:

    ```bash
    export PORT_SERVER=<puerto>
    export IP_SERVER=<IP>
    ```

    Donde `<puerto>` y `<IP>` son los campos dados del servidor.

2. Por último ejecutar el programa deseado con LD_PRELOAD

    ```bash
    LD_PRELOAD=./libfunciones.so ./program
    ```

    Donde ./program es el programa deseado
