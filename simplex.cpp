#include <iostream>
#include <vector>
#include <string>

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

int main() {
    int objectiveType, numVariables, numConstraints;
    vector<double> objectiveCoefficients;
    vector<vector<double>> constraintCoefficients;
    vector<string> constraintSigns;
    vector<double> constraintRHS;

    get_input(objectiveType, numVariables, numConstraints, objectiveCoefficients,
              constraintCoefficients, constraintSigns, constraintRHS);

    print_problem(objectiveType, objectiveCoefficients, constraintCoefficients,
                  constraintSigns, constraintRHS);

    return 0;
}
