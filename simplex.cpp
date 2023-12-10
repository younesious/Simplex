#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

vector<double> oldObjectiveCoefficients;
static bool two_phase_flag = true;

void min_to_max(vector<double> &objectiveCoefficients) {
    for (double &coeff: objectiveCoefficients) {
        coeff *= -1;
    }
}

vector<string> initialize_variables(size_t numVars) {
    vector<string> variables(numVars, "");
    for (size_t i = 0; i < variables.size(); ++i) {
        variables[i] = "x" + to_string(i + 1);
    }
    return variables;
}

void get_input(int &objectiveType, int &numVariables, int &numConstraints,
               vector<double> &objectiveCoefficients, vector<vector<double>> &constraintCoefficients,
               vector<string> &constraintSigns, vector<double> &constraintRHS, vector<string> &variables) {
    cout << "Enter '1' for Maximization or '2' for Minimization: ";
    cin >> objectiveType;

    cout << "Enter the number of variables: ";
    cin >> numVariables;
    variables = initialize_variables(numVariables);

    cout << "Enter the number of constraints: ";
    cin >> numConstraints;

    cout << "Enter coefficients for the objective function:\n";
    objectiveCoefficients.resize(numVariables);
    for (int i = 0; i < numVariables; ++i) {
        cout << "Coefficient for variable x" << i + 1 << ": ";
        cin >> objectiveCoefficients[i];
    }

    if (objectiveType == 2) {
        min_to_max(objectiveCoefficients);
    }

    cout << "Enter coefficients, signs, and RHS for each constraint:\n";
    constraintCoefficients.resize(numConstraints, vector<double>(numVariables));
    constraintSigns.resize(numConstraints);
    constraintRHS.resize(numConstraints);

    for (int i = 0; i < numConstraints; ++i) {
        cout << "Constraint " << i + 1 << ":\n";
        for (int j = 0; j < numVariables; ++j) {
            cout << "Coefficient for variable x" << j + 1 << ": ";
            cin >> constraintCoefficients[i][j];
        }

        cout << "Sign ('=', '<=', '>='): ";
        cin >> ws; // Consume any leading white spaces
        getline(cin, constraintSigns[i]);

        cout << "Right Hand Side (RHS): ";
        cin >> constraintRHS[i];
    }
}

void print_problem(int objectiveType, const vector<double> &objectiveCoefficients,
                   const vector<vector<double>> &constraintCoefficients, const vector<string> &constraintSigns,
                   const vector<double> &constraintRHS) {
    cout << (objectiveType == 1 ? "\nMaximize" : "Minimize") << " Z = ";
    for (size_t i = 0; i < objectiveCoefficients.size(); ++i) {
        cout << objectiveCoefficients[i] << "x" << i + 1;
        if (i < objectiveCoefficients.size() - 1) {
            cout << " + ";
        }
    }
    cout << endl;

    cout << "\nSubject to the constraints:\n";
    for (size_t i = 0; i < constraintCoefficients.size(); ++i) {
        cout << i + 1 << ". ";
        for (size_t j = 0; j < constraintCoefficients[i].size(); ++j) {
            cout << constraintCoefficients[i][j] << "x" << j + 1;
            if (j < constraintCoefficients[i].size() - 1) {
                cout << " + ";
            }
        }
        cout << " " << constraintSigns[i] << " " << constraintRHS[i] << endl;
    }
}

void enter_simplex_table(vector<vector<double>> &simplexTable, const vector<double> &objectiveCoefficients,
                         const vector<vector<double>> &constraintCoefficients, const vector<double> &constraintRHS,
                         const vector<string> &variables) {
    size_t numVariables = variables.size();
    size_t numConstraints = constraintCoefficients.size();
    size_t numRows = numConstraints + 1;
    size_t numCols = numVariables + 2;

    simplexTable.resize(numRows, vector<double>(numCols - 1, 0.0));

    for (int i = 0; i <= numVariables; ++i) {
        simplexTable[0][i] = -objectiveCoefficients[i];
    }

    for (size_t i = 1; i <= numConstraints; ++i) {
        for (size_t j = 0; j <= numVariables; ++j) {
            simplexTable[i][j] = constraintCoefficients[i - 1][j];
        }
        simplexTable[i][numVariables + i - 1] = 1.0; // Slack or artificial variable
        simplexTable[i][numCols - 2] = constraintRHS[i - 1];
    }
}

void print_simplex(const vector<vector<double>> &simplexTable, int inputColumn, int outputRow, int iteration_counter,
                   const vector<string> &basicVariables, const vector<string> &variables, int objectiveType) {
    cout << "\n\nIteration " << iteration_counter << ":\n";

    cout << setw(8) << "BV";
    for (size_t i = 0; i < variables.size(); ++i) {
        if (i == inputColumn) {
            cout << setw(13) << variables[i] << "*";
        } else {
            cout << setw(13) << variables[i];
        }
    }
    cout << setw(15) << "R.H.S" << endl;

    cout << setw(10) << "Z |";
    for (double i: simplexTable[0]) {
        double coefficient = (objectiveType == 2) ? -i : i;
        cout << setw(13) << fixed << setprecision(2) << coefficient;
    }
    cout << endl;

    for (size_t i = 1; i < simplexTable.size(); ++i) {
        if (i == outputRow) {
            cout << setw(7) << basicVariables[i - 1] << "* |";
        } else {
            cout << setw(8) << basicVariables[i - 1] << " |";
        }
        for (double j: simplexTable[i]) {
            cout << setw(13) << fixed << setprecision(2) << j;
        }
        cout << endl;
    }
}

void big_m_method(vector<double> &constraintRow, double constraintRHS, vector<double> &objectiveCoefficients, int M) {
    objectiveCoefficients.push_back(M);

    for (size_t j = 0; j <= constraintRow.size()-1; ++j) {
        objectiveCoefficients[j] += -M * constraintRow[j];
    }

    objectiveCoefficients.emplace_back(-M * constraintRHS);

    cout << "\nhi:\n";
    for (double oc : objectiveCoefficients) {
        cout << oc << "      ";
    }
}

void two_phase_method_phase_one(vector<double> &constraintRow, double constraintRHS,
                                vector<double> &objectiveCoefficients) {
    oldObjectiveCoefficients = objectiveCoefficients;
    oldObjectiveCoefficients.emplace_back(0);
    oldObjectiveCoefficients.emplace_back(0);
    vector<double> newObjectiveCoefficients(objectiveCoefficients.size(), 0);
    newObjectiveCoefficients.push_back(-1);

    for (size_t j = 0; j <= constraintRow.size(); ++j) {
        newObjectiveCoefficients[j] += constraintRow[j];
    }

    newObjectiveCoefficients.emplace_back(constraintRHS);
    objectiveCoefficients = newObjectiveCoefficients;
}

vector<string> select_basic_variables(vector<string> &variables, const vector<string> &constraintSigns,
                                      vector<vector<double>> &constraintCoefficients,
                                      vector<double> &objectiveCoefficients, const vector<double> &constraintRHS) {
    vector<string> basicVariables;
    size_t nextVariableIndex = variables.size() + 1;

    // Loop over constraints to add x(i+1) to variables when constraintSigns is <= or >=
    for (size_t i = 0; i < constraintCoefficients.size(); ++i) {
        const string &constraintSign = constraintSigns[i];
        if (constraintSign == "<=") {
            // Add x(i+1) to variables
            variables.push_back("x" + to_string(nextVariableIndex++));

            for (size_t j = 0; j < constraintCoefficients.size(); ++j) {
                constraintCoefficients[j].push_back((j == i) ? 1.0 : 0.0);
            }
            objectiveCoefficients.push_back(0.0);

            basicVariables.push_back(variables.back());
        } else if (constraintSign == ">=") {
            variables.push_back("x" + to_string(nextVariableIndex++));

            for (size_t j = 0; j < constraintCoefficients.size(); ++j) {
                constraintCoefficients[j].push_back((j == i) ? -1.0 : 0.0);
            }
            objectiveCoefficients.push_back(0.0);


        }
    }

    for (size_t i = 0; i < constraintCoefficients.size(); ++i) {
        if (constraintSigns[i] == "<=") {
            continue; // Skip the entire loop for this constraint
        }

        bool hasBasicVariable = false;
        size_t basicVariableIndex;

        for (size_t j = 0; j < constraintCoefficients[i].size(); ++j) {
            // Check if the coefficient is 1 and all other coefficients in the constraint are 0
            if (constraintCoefficients[i][j] == 1.0) {
                bool isBasic = true;

                for (size_t k = 0; k < constraintCoefficients.size(); ++k) {
                    if (k != i && constraintCoefficients[k][j] != 0.0) {
                        isBasic = false;
                        break;
                    }
                }
                // Check if in coefficients in objective function are 0
                if (isBasic && objectiveCoefficients[j] != 0.0) {
                    isBasic = false;
                }

                if (isBasic) {
                    hasBasicVariable = true;
                    basicVariableIndex = j;
                    break;
                }
            }
        }

        if (hasBasicVariable) {
            basicVariables.push_back(variables[basicVariableIndex]);
        } else {
            // Create an artificial variable and add it to basic variables
            variables.push_back("x" + to_string(nextVariableIndex++));
            basicVariables.push_back(variables.back());
            for (size_t j = 0; j < constraintCoefficients.size(); ++j) {
                constraintCoefficients[j].push_back((j == i) ? 1.0 : 0.0);
            }

            int method;
            cout << "Enter '1' for BigM or '2' for Two-Phase method: ";
            cin >> method;
            if (method == 1) {
                big_m_method(constraintCoefficients[i], constraintRHS[i],
                             objectiveCoefficients, -100);
            } else if (method == 2) {
                two_phase_method_phase_one(constraintCoefficients[i],
                                           constraintRHS[i], objectiveCoefficients);
            } else {
                cerr << "Invalid input. Exiting program." << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    return basicVariables;
}

void simplex_method(vector<vector<double>> &simplexTable, int objectiveType, vector<string> &basicVariables,
                     vector<string> &variables);

void two_phase_method_phase_two(vector<string> &basicVariables, vector<vector<double>> &simplexTable, int objectiveType,
                                 vector<string> &variables) {

    static bool columnRemoved = false;
    if (!columnRemoved) {
        variables.pop_back();

        size_t columnIndexToRemove = variables.size();

        for (auto &row: simplexTable) {
            if (columnIndexToRemove < row.size()) {
                row.erase(
                        row.begin() + static_cast<std::vector<double>::iterator::difference_type>(columnIndexToRemove));
            }
        }
        columnRemoved = true;
    }

    size_t numVariables = variables.size() + 1;
    for (int i = 0; i <= numVariables; ++i) {
        simplexTable[0][i] = -oldObjectiveCoefficients[i];
    }

    for (const string &bv: basicVariables) {
        bool isValid = true;
        double multiplier;

        size_t columnIndex = find(variables.begin(), variables.end(), bv) - variables.begin();
        for (int i = 0; i < simplexTable.size(); ++i) {
            if (i == 0) {
                double a = simplexTable[0][columnIndex];
                if (a != 0) {
                    isValid = false;
                    multiplier = a / -1;
                }
            }
        }

        if (!isValid) {
            for (int y = 0; y <= variables.size(); ++y) {
                size_t basicVariableIndex = find(basicVariables.begin(), basicVariables.end(), bv) -
                                            basicVariables.begin() + 1;
                simplexTable[0][y] += multiplier * simplexTable[basicVariableIndex][y];
            }
        }
    }

    two_phase_flag = false;
    simplex_method(simplexTable, objectiveType, basicVariables, variables);
}

void simplex_method(vector<vector<double>> &simplexTable, int objectiveType, vector<string> &basicVariables,
                     vector<string> &variables) {
    size_t numRows = simplexTable.size();
    size_t numCols = simplexTable[0].size();
    int iteration_counter = 1;

    while (true) {
        // Step 3: Select the input variable
        int inputColumn = 0;
        for (int i = 1; i < numCols - 1; ++i) {
            if (simplexTable[0][i] < simplexTable[0][inputColumn]) {
                inputColumn = i;
            }

        }

        // Step 4: Select the output variable
        int outputRow = -1;
        double minRatio = -1.0;
        for (int i = 1; i < numRows; ++i) {
            if (simplexTable[i][inputColumn] > 0) {
                double ratio = simplexTable[i][numCols - 1] / simplexTable[i][inputColumn];
                if (outputRow == -1 || ratio < minRatio) {
                    outputRow = i;
                    minRatio = ratio;
                }
            }
        }

        // If no output row is found, the solution is optimal
        if (outputRow == -1) {
            break;
        }

        print_simplex(simplexTable, inputColumn, outputRow, iteration_counter, basicVariables, variables,
                      objectiveType);

        // Step 5: Pivot the selected element
        double pivotElement = simplexTable[outputRow][inputColumn];
        for (int i = 0; i < numCols; ++i) {
            simplexTable[outputRow][i] /= pivotElement;
        }

        // Step 6: Make other elements in the pivot column zero
        for (int i = 0; i < numRows; ++i) {
            if (i != outputRow) {
                double factor = simplexTable[i][inputColumn];
                for (int j = 0; j < numCols; ++j) {
                    simplexTable[i][j] -= factor * simplexTable[outputRow][j];
                }
            }
        }

        basicVariables[outputRow - 1] = "x" + to_string(inputColumn + 1);

        iteration_counter++;

        // Step 7: Check optimality condition
        bool optimal = true;
        for (int i = 0; i < numCols - 1; ++i) {
            if (simplexTable[0][i] < 0) {
                optimal = false;
                break;
            }
        }

        // If the solution is optimal, break out of the loop
        if (optimal) {
            print_simplex(simplexTable, -1, -1, iteration_counter,
                          basicVariables, variables, objectiveType);
            break;
        }
    }

    if (simplexTable[0][numCols] == 0 && two_phase_flag) {
        two_phase_method_phase_two(basicVariables, simplexTable, objectiveType, variables);
    }
}

int main() {
    int objectiveType, numVariables, numConstraints;
    vector<double> objectiveCoefficients;
    vector<vector<double>> constraintCoefficients;
    vector<string> constraintSigns;
    vector<double> constraintRHS;
    vector<string> variables;


    // Get input for the Linear Programming problem
    get_input(objectiveType, numVariables, numConstraints, objectiveCoefficients,
              constraintCoefficients, constraintSigns, constraintRHS, variables);

    vector<string> basicVariables = select_basic_variables(variables, constraintSigns,
                                                           constraintCoefficients, objectiveCoefficients,
                                                           constraintRHS);

    // Print the Linear Programming problem
    print_problem(objectiveType, objectiveCoefficients, constraintCoefficients,
                  constraintSigns, constraintRHS);

    // Create and print the initial simplex table
    vector<vector<double>> simplexTable;
    enter_simplex_table(simplexTable, objectiveCoefficients, constraintCoefficients, constraintRHS, variables);

    print_simplex(simplexTable, -1, -1, 0, basicVariables, variables, objectiveType);

    // Run the simplex algorithm
    simplex_method(simplexTable, objectiveType, basicVariables, variables);

    return 0;
}
