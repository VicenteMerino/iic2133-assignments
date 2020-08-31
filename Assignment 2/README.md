# Base-T2-2020-1

Repositorio para el código base de la T2 del primer semestre del año 2020 (Flip Puzzle Hash) 

# Dependencias y librerías

## C

### stdint.h

El código hace uso del módulo `<stdint.h>`, el cual provee números enteros de tamaño específico. En particular se usan:

* `uintX_t`: Número entero no negativo de X bits. Puede guardar valores desde el 0 hasta el 2^X - 1.

Con X = 8, 16, 32, 64.

### GTK 3 w/JPEG support

La interfaz gráfica, contenida en `src/beholder` y accesible desde tu codigo mediante `src/watcher`, utiliza las siguientes librerías

* libgtk-3-dev (GTK 3)
* libpeg-dev (JPEG)
  
Para instalarlas sigue la guía [aquí](https://github.com/IIC2133-PUC/2020-1/wiki/3.0-GTK)

### PCG Random

Además viene incluido dentro del código la librería **pcg**, dentro de la carpeta `src/random`. Esta permite generar números aleatorios de 64 bits con la función 

```c
uint64_t get_random();
```

## Python

Los scripts de Python utilizan Python 3.6+ y las siguientes librerías:

* altair
* progress
* numpy
* pandas
* scipy

Puedes instalarlas usando pip (o pip3 si estás en linux y estás ocupando python3).

# Ejecución

## C

Primero, para compilar utiliza 

```sh
make
```

Para ejecutar el programa utiliza

```sh
./solver test
```

Por ejemplo

```sh
./solver tests/012.txt
```

Esto abrirá la ventana que muestra la resolución del puzzle una vez que se termine, además de imprimir cuanto tiempo tomó la resolución.

## Python

El script `run_homework.py` se ejecuta de la siguiente manera:

```sh
python3.6+ run_homework.py
```

Este compilará tu solución y la probará con cada test en orden de dificultad ascendente, sin utilizar la interfaz gráfica.

Para cada uno de los tests generará resultados dentro de la carpeta `results/timestamp/data`, donde `timestamp` es la fecha y hora en la que ejecutaste el script.

Si alguno de los test toma más de 10 segundos en resolverse, será detenido y no se ejecutarán los siguientes tests.

Luego utilizará las funciones de `generate_report.py` para generar dentro de la carpeta `results/timestamp/graphs` gráficos que visualizan la información obtenida. 

En particular, para un test `n`, 

* `results/timestamp/data/histograms/n.txt`: Contiene los histogramas de colisiones de la tabla de hash para cada tamaño de la tabla.
* `results/timestamp/data/hash/n.txt`: Contiene todos los valores de hash de estados distintos.
* `results/timestamp/data/stats_file.csv`: Contiene las métricas de "Estados Totales", "Estados Distintos", "Colisiones totales", "Cantidad de Resizes de la tabla" y "Tiempo total de resolución", para cada test que se ejecutó correctamente.
* `results/timestamp/graphs/histograms/n/*.html`: Contiene gráficos para los histogramas de colisiones para cada tamaño de la tabla. Uno de los gráficos son las colisiones producidas al avanzar con la función de probing, y el otro las colisiones producidas al encontrarte que en la celda que te corresponde por función de hash ya hay algo guardado.
* `results/timestamp/graphs/hash/n.html`: Contiene el histograma de los valores de la función de hash.
* `results/timestamp/graphs/x_vs_y.html`: Contiene un scatterplot de 2 de las columnas de `results/timestamp/data/stats_file.csv`, junto con una regresión para identificar tendencia. Los valores de esta regresión, junto con el R² se imprimiran en consola al terminar el script.



