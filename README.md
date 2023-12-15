# Sipmlex

Simlex is a C++ implementation of the Simplex method for solving linear programming (LP) problems. The repository includes two files: `simplex.cpp` for solving primal LP problems and `dual.cpp` for generating the dual LP problem.

## Table of Contents

+ Introduction
+ Features
+ Usage
    + Primal LP Solver (simplex.cpp)
    + Dual LP Generator and Solver (dual.cpp)
+ Installation
+ Contributing
+ License
   
## Introduction

The Simplex method is an iterative optimization algorithm for solving linear programming problems. The primal LP solver `simplex.cpp` takes user input for a linear programming problem and uses the Simplex method to find the optimal solution. The dual LP generator `dual.cpp` automatically generate the dual LP problem corresponding to the given primal LP problem.

## Features

### Primal LP Solver (`simplex.cpp`)

+ Solves linear programming problems (maximization or minimization) using the Simplex method.
+ Handles constraints of the form <=, >=, and =.
+ Allows the user to input coefficients for the objective function, constraint coefficients, signs, and RHS values.
+ Supports both Big M method and Two-Phase method for solving LP problems.

### Dual LP Generator (`dual.cpp`)

+ Automatically generates the dual LP problem for a given primal LP problem.
+ This file accepts primal LP problem input from user handles problems with >=, <=, and =        constraints, handles free variables marked "R".
+ Converts the primal LP problem to conventional form before generating the dual.

## Usage

### Primal LP Solver (`simplex.cpp`)

1. Compile the simplex.cpp file using a C++ compiler:

```bash
g++ simplex.cpp -o simplex
```

2. Run the compiled executable:

```bash
./simplex
```

3. Follow the on-screen prompts to input the details of the primal LP problem.

### Dual LP Generator and Solver (`dual.cpp`)

1. Compile the dual.cpp file using a C++ compiler:

```bash
g++ dual.cpp -o dual
```

2. Run the compiled executable:

```bash
./dual
```

3. Follow the on-screen prompts to input the details of the primal LP problem.

## Installation

No installation is required. Simply compile the C++ files using a C++ compiler.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.
