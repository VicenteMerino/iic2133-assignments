import sys
import os
import os.path
from aquarium import Aquarium
import time
if __name__ == "__main__":
  tiempo_inicial = time.time()

  if len(sys.argv) != 3:
    print(f"Modo de uso: python3.6+ {sys.argv[0]} input_file output_file")
    print("Donde")
    print("\tinput_file es un archivo del test a resolver")
    print("\toutput_file es el archivo donde el programa guardará el output")
    sys.exit(1)

  # El primer parámetro es el archivo de input
  input_file = sys.argv[1]

  # El segundo parámetro es el archivo de output
  output_file = sys.argv[2]

       
  path = os.path.join(os.getcwd(), output_file)
 
  directory = os.path.dirname(path)
  if not os.path.exists(directory):
    os.makedirs(directory)
  aquarium = Aquarium(path, input_file)
  aquarium.is_solvable()
  print(f"Tiempo de ejecución: {time.time()-tiempo_inicial}")