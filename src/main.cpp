#include "SATSolver.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>

SATSolver::Formula readFormulaFromFile(const std::string& filename) {
    SATSolver::Formula formula;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Ignore comments and problem line
        if (line.empty() || line[0] == 'c' || line[0] == 'p') {
            continue;
        }

        // Parse a clause
        std::istringstream iss(line);
        SATSolver::Clause clause;
        int literal;
        while (iss >> literal) {
            if (literal == 0) {
                break; // Clause termination
            }
            clause.push_back(literal);
        }
        if (!clause.empty()) {
            formula.push_back(clause);
        }
    }

    file.close();
    return formula;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    try {
        // Read formula from file
        std::string filename = argv[1];
        SATSolver::Formula formula = readFormulaFromFile(filename);

        // Set logging verbosity level (0 = None, 1 = Minimal, 2 = Detailed, 3 = Trace)
        Logger::setLevel(1);

        // Initialize SAT solver
        SATSolver solver;

        // Measure time
        auto start = std::chrono::high_resolution_clock::now();
        bool result = solver.solve(formula);
        auto end = std::chrono::high_resolution_clock::now();

        // Output results
        std::chrono::duration<double> elapsed = end - start;
        std::cout << (result ? "SATISFIABLE" : "UNSATISFIABLE") << std::endl;
        std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}