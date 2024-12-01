#include "SATSolver.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <cstdlib> // For std::stoi
#include <cctype>  // For std::isdigit
#include <algorithm> // For std::all_of

SATSolver::Formula readFormulaFromFile(const std::string& filename) {
    SATSolver::Formula formula;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c' || line[0] == 'p') {
            continue;
        }

        std::istringstream iss(line);
        SATSolver::Clause clause;
        int literal;
        while (iss >> literal) {
            if (literal == 0) break;
            clause.push_back(literal);
        }
        if (!clause.empty()) {
            formula.push_back(clause);
        }
    }

    file.close();
    return formula;
}

Heuristic parseHeuristic(const std::string& heuristicName) {
    if (heuristicName == "naive") return Heuristic::NAIVE;
    if (heuristicName == "moms") return Heuristic::MOMS;
    if (heuristicName == "dlis") return Heuristic::DLIS;
    if (heuristicName == "dsids") return Heuristic::DSIDS;
    if (heuristicName == "random") return Heuristic::RANDOM;
    if (heuristicName == "compare") throw std::invalid_argument("--heuristic=compare is a special option.");
    throw std::invalid_argument("Unknown heuristic: " + heuristicName);
}

bool isInteger(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void compareHeuristics(const SATSolver::Formula& formula, int logLevel) {
    Logger::setLevel(logLevel);
    std::vector<Heuristic> heuristics = {
        Heuristic::NAIVE, Heuristic::MOMS, Heuristic::DLIS, Heuristic::DSIDS, Heuristic::RANDOM
    };

    for (size_t i = 0; i < heuristics.size(); ++i) {
        SATSolver solver;
        solver.setHeuristic(heuristics[i]);

        auto start = std::chrono::high_resolution_clock::now();
        bool result = solver.solve(formula);
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = std::chrono::duration<double>(end - start).count();
        solver.logHeuristicSummary(heuristics[i], result, elapsed);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [--heuristic=<heuristic>] [--log-level=<log_level>]" << std::endl;
        return 1;
    }

    try {
        std::string filename;
        std::string heuristicName = "naive";
        int logLevel = 1;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.rfind("--heuristic=", 0) == 0) {
                heuristicName = arg.substr(12);
            } else if (arg.rfind("--log-level=", 0) == 0) {
                std::string levelStr = arg.substr(12);
                if (!isInteger(levelStr)) {
                    throw std::invalid_argument("Log level must be an integer.");
                }
                logLevel = std::stoi(levelStr);
            } else {
                filename = arg;
            }
        }

        if (filename.empty()) {
            throw std::invalid_argument("Input file must be specified.");
        }

        SATSolver::Formula formula = readFormulaFromFile(filename);

        if (heuristicName == "compare") {
            compareHeuristics(formula, logLevel);
            return 0;
        }

        Heuristic heuristic = parseHeuristic(heuristicName);
        Logger::setLevel(logLevel);

        SATSolver solver;
        solver.setHeuristic(heuristic);

        auto start = std::chrono::high_resolution_clock::now();
        bool result = solver.solve(formula);
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = std::chrono::duration<double>(end - start).count();
        solver.logHeuristicSummary(heuristic, result, elapsed);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}