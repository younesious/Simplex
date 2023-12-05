#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

void get_input(int &objectiveType, int &numVariables, int &numConstraints,
               vector<double> &objectiveCoefficients, vector<vector<double>> &constraintCoefficients,
               vector<string> &constraintSigns, vector<double> &constraintRHS) {
    cout << "Enter '1' for Maximization or '2' for Minimization: ";
    cin >> objectiveType;

    cout << "Enter the number of variables: ";
    cin >> numVariables;
    cout << "Enter the number of constraints: ";
    cin >> numConstraints;

    cout << "Enter coefficients for the objective function:\n";
    objectiveCoefficients.resize(numVariables);
    for (int i = 0; i < numVariables; ++i) {
        cout << "Coefficient for variable x" << i + 1 << ": ";
        cin >> objectiveCoefficients[i];
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
                         const vector<vector<double>> &constraintCoefficients, const vector<double> &constraintRHS) {
    size_t numVariables = objectiveCoefficients.size();
    size_t numConstraints = constraintCoefficients.size();
    size_t numRows = numConstraints + 1;
    size_t numCols = numVariables + numConstraints + 2; // Variables, Slack and Artificial Variables, RHS

    // Resize the simplex table
    simplexTable.resize(numRows, vector<double>(numCols, 0.0));

    // Fill the table with objective coefficients and coefficients from constraints
    for (int i = 0; i < numVariables; ++i) {
        simplexTable[0][i] = -objectiveCoefficients[i];
    }

    for (int i = 1; i <= numConstraints; ++i) {
        for (int j = 0; j < numVariables; ++j) {
            simplexTable[i][j] = constraintCoefficients[i - 1][j];
        }
    }

    // Fill the table with slack and artificial variables
    for (int i = 1; i <= numConstraints; ++i) {
        simplexTable[i][numVariables + i - 1] = 1.0; // Slack variable
    }

    // Fill the table with RHS values
    for (int i = 1; i <= numConstraints; ++i) {
        simplexTable[i][numCols - 1] = constraintRHS[i - 1];
    }
}

// Function to print the simplex table with highlights
void print_simplex(const vector<vector<double>> &simplexTable, int inputColumn, int outputRow) {
    cout << "Simplex Table:\n";

    // Print header row
    cout << "   ";
    for (size_t i = 0; i < simplexTable[0].size(); ++i) {
        if (i == inputColumn) {
            cout << setw(8) << "x" << i + 1 << "*";
        } else {
            cout << setw(8) << "x" << i + 1;
        }
    }
    cout << endl;

    // Print the table
    for (size_t i = 0; i < simplexTable.size(); ++i) {
        if (i == outputRow) {
            cout << setw(2) << "x" << i + 1 << "* |";
        } else {
            cout << setw(2) << "x" << i + 1 << " |";
        }

        for (double j : simplexTable[i]) {
            cout << setw(8) << fixed << setprecision(2) << j;
        }
        cout << endl;
    }
}

void simplex_method(std::vector<std::vector<double>> &simplexTable) {
    size_t numRows = simplexTable.size();
    size_t numCols = simplexTable[0].size();

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

        // Print the simplex table with highlights
        print_simplex(simplexTable, inputColumn, outputRow);

        // Step 7: Check optimality condition
        bool optimal = true;
        for (int i = 1; i < numCols - 1; ++i) {
            if (simplexTable[0][i] < 0) {
                optimal = false;
                break;
            }
        }

        // If the solution is optimal, break out of the loop
        if (optimal) {
            break;
        }
    }
}

int main() {
    int objectiveType, numVariables, numConstraints;
    vector<double> objectiveCoefficients;
    vector<vector<double>> constraintCoefficients;
    vector<string> constraintSigns;
    vector<double> constraintRHS;

    // Get input for the Linear Programming problem
    get_input(objectiveType, numVariables, numConstraints, objectiveCoefficients,
              constraintCoefficients, constraintSigns, constraintRHS);

    // Print the Linear Programming problem
    print_problem(objectiveType, objectiveCoefficients, constraintCoefficients,
                  constraintSigns, constraintRHS);

    // Create and print the initial simplex table
    vector<vector<double>> simplexTable;
    enter_simplex_table(simplexTable, objectiveCoefficients, constraintCoefficients, constraintRHS);

    print_simplex(simplexTable, -1, -1); // -1 means no input and output variables highlighted

    // Run the simplex algorithm
    simplex_method(simplexTable);

    return 0;
}
