#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector<string> initialize_variables(size_t numVars, char varName) {
    vector<string> variables(numVars, "");
    for (size_t i = 0; i < variables.size(); ++i) {
        variables[i] = varName + to_string(i + 1);
    }
    return variables;
}

void get_input(int &objectiveType, int &numVariables, int &numConstraints,
               vector<double> &objectiveCoefficients, vector<vector<double>> &constraintCoefficients,
               vector<string> &constraintSigns, vector<double> &constraintRHS, vector<string> &variables,
               vector<string> &variableSigns) {
    cout << "Enter '1' for Maximization or '2' for Minimization: ";
    cin >> objectiveType;

    cout << "Enter the number of variables: ";
    cin >> numVariables;
    variables = initialize_variables(numVariables, 'x');

    cout << "Enter the signs for the variables ('>=', '<=', 'R'):\n";
    for (int i = 0; i < numVariables; ++i) {
        cout << "Sign for " << variables[i] << ": ";
        cin >> ws;
        string temp;
        getline(cin, temp);
        variableSigns.emplace_back(temp);
    }

    cout << "Enter coefficients for the objective function:\n";
    objectiveCoefficients.resize(numVariables);
    for (int i = 0; i < numVariables; ++i) {
        cout << "Coefficient for variable x" << i + 1 << ": ";
        cin >> objectiveCoefficients[i];
    }

    cout << "Enter the number of constraints: ";
    cin >> numConstraints;

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
                   const vector<double> &constraintRHS, const vector<string> &variableSigns,
                   const vector<string> &variables) {
    cout << (objectiveType == 1 ? "\nMaximize" : "Minimize") << " Z = ";
    for (size_t i = 0; i < objectiveCoefficients.size(); ++i) {
        cout << objectiveCoefficients[i] << variables[i];
        if (i < objectiveCoefficients.size() - 1) {
            cout << " + ";
        }
    }
    cout << endl;

    cout << "\nSubject to the constraints:\n";
    for (size_t i = 0; i < constraintCoefficients.size(); ++i) {
        cout << i + 1 << ". ";
        for (size_t j = 0; j < constraintCoefficients[i].size(); ++j) {
            cout << constraintCoefficients[i][j] << variables[j];
            if (j < constraintCoefficients[i].size() - 1) {
                cout << " + ";
            }
        }
        cout << " " << constraintSigns[i] << " " << constraintRHS[i] << endl;
    }

    cout << "\nVariable Signs:\n";
    for (size_t i = 0; i < variableSigns.size(); ++i) {
        cout << "Sign for " << variables[i] << ": " << variableSigns[i] << ", ";
    }
    cout << endl;
}

void convert_to_conventional_model(vector<vector<double>> &constraintCoefficients,
                                   vector<double> &objectiveCoefficients, vector<string> &constraintSigns,
                                   vector<double> &constraintRHS, vector<string> &variableSigns,
                                   vector<string> &variables) {
    for (size_t i = 0; i < constraintSigns.size(); ++i) {
        if (constraintSigns[i] == ">=") {
            for (double &j: constraintCoefficients[i]) {
                j *= -1;
            }
            constraintRHS[i] *= -1;

            constraintSigns[i] = "<=";
        } else if (constraintSigns[i] == "=") {
            constraintSigns[i] = "<=";

            vector<double> newCoefficients = constraintCoefficients[i];
            double newRHS = constraintRHS[i];
            for (double &newCoefficient: newCoefficients) {
                newCoefficient *= -1;
            }
            newRHS *= -1;

            for (double &j: constraintCoefficients[i]) {
                j *= -1;
            }
            constraintRHS[i] *= -1;

            constraintCoefficients.push_back(newCoefficients);
            constraintSigns.emplace_back(">=");
            constraintRHS.push_back(newRHS);
        }
    }

    for (size_t j = 0; j < variableSigns.size(); ++j) {
        if (variableSigns[j] == "<=") { // xi' = -xi, xi' >= 0
            variables[j] = variables[j] + "'";
            variableSigns[j] = ">= 0";

            for (size_t i = 0; i < constraintSigns.size(); ++i) {
                for (double &c: constraintCoefficients[i]) {
                    c *= -1;
                }
            }
            objectiveCoefficients[j] *= -1;
        } else if (variableSigns[j] == "R") {
            variableSigns[j] = ">= 0";
            variables[j] += "'";

            string newVar = variables[j] + "'";
            variables.push_back(newVar);
            variableSigns.emplace_back(">= 0");
            objectiveCoefficients.push_back(objectiveCoefficients[j] * -1);

            for (auto &row: constraintCoefficients) {
                row[j] *= -1;
                row.push_back(row[j] * -1);
            }
        }
    }
}

void dual(int &objectiveType, vector<double> &objectiveCoefficients,
          vector<vector<double>> &constraintCoefficients, vector<string> &constraintSigns,
          vector<double> &constraintRHS, vector<string> &variableSigns, vector<string> &variables) {
    objectiveType = (objectiveType == 1) ? 2 : 1;

    size_t dualNumVariables = variables.size();
    size_t dualNumConstraints = constraintCoefficients.size();

    vector<double> dualObjectiveCoefficients(dualNumConstraints, 0.0);
    vector<vector<double>> dualConstraintCoefficients(dualNumVariables, vector<double>(dualNumConstraints, 0.0));
    vector<string> dualConstraintSigns(dualNumConstraints);
    vector<double> dualConstraintRHS(dualNumConstraints, 0.0);
    variables = initialize_variables(dualNumConstraints, 'y');

    for (size_t i = 0; i < dualNumConstraints; ++i) {
        dualObjectiveCoefficients[i] = constraintRHS[i];
    }

    for (size_t i = 0; i < dualNumConstraints; ++i) {
        for (size_t j = 0; j < dualNumVariables; ++j) {
            dualConstraintCoefficients[j][i] = constraintCoefficients[i][j];
        }
    }

    for (size_t i = 0; i < dualNumConstraints; ++i) {
        if (constraintSigns[i] == "<=") {
            dualConstraintSigns[i] = ">=";
            dualConstraintRHS[i] = objectiveCoefficients[i];
        } else if (constraintSigns[i] == ">=") {
            dualConstraintSigns[i] = "<=";
            dualConstraintRHS[i] = objectiveCoefficients[i];
        } else if (constraintSigns[i] == "=") {
            // No change needed for "=" constraints
            dualConstraintSigns[i] = "=";
            dualConstraintRHS[i] = constraintRHS[i];
        }
    }

    cout << "\nDual LP:\n";
    print_problem(objectiveType, dualObjectiveCoefficients,
                  dualConstraintCoefficients, dualConstraintSigns,
                  dualConstraintRHS, variableSigns, variables);
}

int main() {
    int objectiveType, numVariables, numConstraints;
    vector<double> objectiveCoefficients;
    vector<vector<double>> constraintCoefficients;
    vector<string> constraintSigns;
    vector<double> constraintRHS;
    vector<string> variables;
    vector<string> variableSigns;

    // Get input for the Linear Programming problem
    get_input(objectiveType, numVariables, numConstraints, objectiveCoefficients,
              constraintCoefficients, constraintSigns, constraintRHS, variables, variableSigns);

    // Print the original problem
    cout << "\nOriginal LP:\n";
    print_problem(objectiveType, objectiveCoefficients,
                  constraintCoefficients, constraintSigns,
                  constraintRHS, variableSigns, variables);

    // Convert the primal LP to conventional form
    convert_to_conventional_model(constraintCoefficients, objectiveCoefficients, constraintSigns, constraintRHS,
                                  variableSigns, variables);

    // Print the modified problem
    cout << "\nModified LP after conversion to conventional form:\n";
    print_problem(objectiveType, objectiveCoefficients, constraintCoefficients,
                  constraintSigns, constraintRHS, variableSigns, variables);

    dual(objectiveType, objectiveCoefficients, constraintCoefficients,
         constraintSigns, constraintRHS, variableSigns, variables);
}
