cdef class Variable():
  cdef public:
    int value
    set domain
    int row
    list cols
    int id
    int largo
  def __cinit__(self, int row, int id):
    self.value = -1
    self.domain = {0, 1}
    self.row = row
    self.cols = []
    self.id = id
    self.largo = 0
  cpdef void append(self, int col):
    self.cols.append(col)
    self.largo += 1



cdef class Aquarium():
  """Representa una instancia del puzzle"""

  cdef public:
    list right_cumulative
    list left_cumulative
    list bot_cumulative
    list top_cumulative
    list top
    list bot
    list right
    list left
    list ids
    list matrix
    list unassigned
    str path
  def __cinit__(self,str path,str input_file):

  # Leemos el archivo de input
    with open(input_file, "r") as puzzle_file:

      height = int(puzzle_file.readline().strip())
      width = int(puzzle_file.readline().strip())

      self.top = [int(x) for x in puzzle_file.readline().strip().split(" ")]  
      self.bot = [height - x for x in self.top]

      self.left = [int(x) for x in puzzle_file.readline().strip().split(" ")]
      self.right = [width - x for x in self.left]

      tank_count = int(puzzle_file.readline().strip())

      ids_set = set()

      for i in range(tank_count):
        ids_set.add(i)

      initial = []

      for row in range(height):
        ids_row = [int(x) for x in puzzle_file.readline().strip().split(" ")]
        initial.append(ids_row)

    self.right_cumulative  = [0 for _ in range(len(initial))]
    self.left_cumulative = [0 for _ in range(len(initial))]
    self.bot_cumulative  = [0 for _ in range(len(initial[0]))]
    self.top_cumulative = [0 for _ in range(len(initial[0]))]

    self.ids = [set() for _ in ids_set]

    variables = [{} for _ in range(len(initial))]

    for row in range(len(initial)):
      for col in range(len(initial[0])):
        if initial[row][col] not in variables[row]:
          variables[row][initial[row][col]] = Variable(row, initial[row][col])
        variables[row][initial[row][col]].append(col)

    self.unassigned = []

    for row in range(len(initial)):
      for id in variables[row]:
        var = variables[row][id]
        self.ids[id] |= {var}
        self.unassigned.append(var)

    for var in self.unassigned:
      if var.largo > self.right[var.row]:
        var.domain -= {1}
        for vecino in self.ids[var.id]:
          if vecino.row > var.row:
            vecino.domain -= {1}
      elif var.largo > self.left[var.row]:
        var.domain -= {0}
        for vecino in self.ids[var.id]:
          if vecino.row < var.row:
            vecino.domain -= {0}


    self.path = path
    
    self.matrix = [[-1 for j in range(len(initial[0]))] for i in range(len(initial))]

  cpdef list assign(self, x, int value):
    x.value = value

    modified = []
    if value == 1:
      self.right_cumulative[x.row] += x.largo
      for col in x.cols:
        self.bot_cumulative[col] += 1
        self.matrix[x.row][col] = 1
      for vecino in self.ids[x.id]:
        if vecino.row < x.row and vecino.value == -1 and len(vecino.domnain) == 2:
          vecino.domain -= {0}
          modified.append(vecino)
    else:
      self.left_cumulative[x.row] += x.largo
      for col in x.cols:
        self.top_cumulative[col] += 1
        self.matrix[x.row][col] = 0
      for vecino in self.ids[x.id]:
        if vecino.row > x.row and vecino.value == -1 and len(vecino.domain) == 2:
          vecino.domain -= {1}
          modified.append(vecino)
    return modified


  cpdef void unassign(self, x, int value, list modified):
    """Desasigna una variable y actualiza los contadores"""
    x.value = -1

    if value == 1:
      self.right_cumulative[x.row] -= x.largo
      for col in x.cols:
        self.matrix[x.row][col] = -1
        self.bot_cumulative[col] -= 1
      for vecino in modified:
        vecino.domain |= {0}
    else:
      self.left_cumulative[x.row] -= x.largo
      for col in x.cols:
        self.matrix[x.row][col] = -1
        self.top_cumulative[col] -= 1
      for vecino in modified:
        vecino.domain |= {1}

  def is_solution(self):
    return len(self.unassigned) == 0

  def choose_unnasigned_variable(self):
    return self.unassigned.pop(0)
  
  def is_valid(self, x,int value):
    if value == 1:
      if x.largo + self.right_cumulative[x.row] > self.right[x.row]:
        return False
      for col in x.cols:
        if self.bot_cumulative[col] == self.bot[col]:
          return False
    else:
      if x.largo + self.left_cumulative[x.row] > self.left[x.row]:
        return False
      for col in x.cols:
        if self.top_cumulative[col] == self.top[col]:
          return False
    return True

  def is_solvable(self):

    if self.is_solution():
      self.write_file()
      return True
    x = self.choose_unnasigned_variable()
    for value in x.domain:
      if self.is_valid(x, value):
        modified = self.assign(x, value)
        if self.is_solvable():
          return True
        self.unassign(x, value, modified)
    self.unassigned.insert(0, x)
    return False

  cpdef void write_file(self):
    with open(self.path, 'w') as f:
      for line in self.matrix:
        for elem in line:
          f.write(str(elem)+ ' ')
        f.write('\n')
