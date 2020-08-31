import os
import random
import subprocess
import sys
from datetime import datetime
from os import listdir, path
# pip install progress 
from progress.bar import IncrementalBar
from generate_report import HashTester, HistogramGenerator, RegressionGenerator

def call(args, stdout=subprocess.DEVNULL, timeout=None):
    """Ejecuta un comando de consola."""
    return subprocess.call(
        args.split(" "),
        stdout=stdout,
        stderr=subprocess.DEVNULL,
        timeout=timeout
    )

if __name__ == "__main__":
    # Directorio que contiene los tests
    tests_path = "tests"

    # Obtenemos todos los tests en la carpeta de tests
    test_list = [path.join(tests_path, test) for test in listdir(tests_path)]

    # Ordenamos los tests en orden ascendiente de dificultad 
    test_list.sort()

    # El momento en el que ejecutamos este script 
    base_folder = path.join("results", datetime.now().isoformat("_","seconds"))

    # Nombre que llevará la carpeta con estos resultados
    data_path = path.join(base_folder, "data")

    # Nombre de la carpeta donde se guardarán los histogramas
    hist_path = path.join(data_path, "histograms")
    os.makedirs(hist_path)

    # Nombre de la carpeta donde se guardará la información del hash
    hash_path = path.join(data_path, "hash")
    os.makedirs(hash_path)

    # Archivo donde guardaremos los stats de todos los tests
    stats_file = path.join(data_path, "stats_file.csv")
    with open(stats_file, "w") as stats_file_p:
        # Escribimos el header del archivo CSV
        stats_file_p.write("total_states,unique_states,collisions,resizes,time\n")

    # Volvemos a compilar los archivos que hayan sido modificados desde la ultima compilación
    call('make')

    # Barra de progreso
    bar = IncrementalBar(f"Ejecutando tests", max=len(test_list))

    # Tests que hemos resuelto
    solved_tests = {}

    for test in test_list:
        # Obtenemos el nombre del test, sin ruta ni extensión        
        test_name = path.splitext(path.basename(test))[0]

        # Archivo donde guardaremos los histogramas de este test
        hist_file = path.join(hist_path, f"{test_name}.txt")      

        # Archivo donde guardaremos los hashes de este test
        hash_file = path.join(hash_path, f"{test_name}.txt") 

        # El comando con el que ejecutaremos el solver
        command = f"./solver {test} {hist_file} {hash_file} {stats_file}"

        try:
            # Ejecutamos el comando con un timeout de 10 segundos
            call(command, timeout=10)
        except subprocess.TimeoutExpired:
            bar.finish()
            print(f"Tu programa ha sido derrotado por el test {test_name}")
            os.remove(hist_file)
            os.remove(hash_file)
            break
        except:
            bar.finish()
            print(f"Tu programa se cayó. El comando fue\n\t{command}")
            os.remove(hist_file)
            os.remove(hash_file)
            break
        
        # Agregamos los archivos generados
        solved_tests[test_name] = {"hist": hist_file, "hash": hash_file}

        bar.next()
    bar.finish()

    # Nombre que llevará la carpeta con estos resultados
    graf_path = path.join(base_folder, "graphs")

    # Ruta a la carpeta donde se guardarán los graficos de histogramas
    hist_graph_base_path = path.join(graf_path, "histograms")

    # Ruta a la carpeta donde se guardarán los graficos del hash
    hash_graph_base_path = path.join(graf_path, "hash")
    os.makedirs(hash_graph_base_path)
    
    # Barra de progreso
    bar = IncrementalBar(f"Generando gráficos", max=len(solved_tests))

    for test, files in solved_tests.items():

        # La carpeta donde guardaremos los histogramas de este test
        hist_graph_path = path.join(hist_graph_base_path, test)
        os.makedirs(hist_graph_path)

        # Generamos los histogramas de colisiones
        HistogramGenerator(files["hist"]).generate_report(hist_graph_path)

        # El archivo donde guardaremos el análisis del hash de este test
        hash_graph_file = path.join(hash_graph_base_path, test)

        # Generamos el histograma de la distribución del hash
        HashTester(files["hash"]).generate_report(hash_graph_file)
        
        bar.next()
    
    bar.finish()
    
    # Generamos el reporte que compara el rendimiento de cada test 
    RegressionGenerator(stats_file).generate_report(graf_path)

