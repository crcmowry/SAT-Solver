#include "SATSolver.h"
#include "Logger.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <climits> // For INT_MAX

// Main entry point for solving a SAT problem
bool SATSolver::solve(const Formula& formula) {
    finalAssignment.clear();
    decisions = 0;
    backtracks = 0;

    return DPLL(formula, finalAssignment);
}

// Recursive DPLL algorithm
bool SATSolver::DPLL(Formula formula, Assignment& assignment) {
    Logger::log(3, "Current formula:");
    printFormula(formula, 3);

    Logger::log(3, "Current assignment:");
    printAssignment(assignment, 3);

    std::vector<int> propagatedLiterals;

    // Unit propagation
    while (true) {
        int unitLiteral = findUnitClause(formula);
        if (unitLiteral == 0) break;
        Logger::logf(2, "Unit propagation with literal: %", unitLiteral);
        formula = propagate(formula, unitLiteral, assignment);
        propagatedLiterals.push_back(unitLiteral);
    }

    // Pure literal elimination
    while (true) {
        int pureLiteral = findPureLiteral(formula);
        if (pureLiteral == 0) break;
        Logger::logf(2, "Pure literal elimination with literal: %", pureLiteral);
        formula = propagate(formula, pureLiteral, assignment);
        propagatedLiterals.push_back(pureLiteral);
    }

    if (isSatisfied(formula)) {
        Logger::log(2, "Formula satisfied!");
        finalAssignment = assignment; // Store the final assignment
        return true;
    }

    if (hasEmptyClause(formula)) {
        Logger::log(2, "Encountered an empty clause. Backtracking...");
        undoPropagatedLiterals(assignment, propagatedLiterals);
        backtracks++; // Increment backtracks
        return false;
    }

    int literal = selectLiteral(formula);
    Logger::logf(2, "Choosing literal: %", literal);
    decisions++; // Increment decisions

    // Try assigning True
    Logger::logf(2, "Trying literal % = True", literal);
    assignment.insert(literal);
    Formula formulaTrue = propagate(formula, literal, assignment);
    if (DPLL(formulaTrue, assignment)) return true;
    assignment.erase(literal);

    // Try assigning False
    Logger::logf(2, "Trying literal % = False", literal);
    assignment.insert(-literal);
    Formula formulaFalse = propagate(formula, -literal, assignment);
    if (DPLL(formulaFalse, assignment)) return true;
    assignment.erase(-literal);

    Logger::logf(2, "Both True and False failed for literal: %. Backtracking further...", literal);
    undoPropagatedLiterals(assignment, propagatedLiterals);
    backtracks++; // Increment backtracks
    return false;
}

bool SATSolver::isSatisfied(const Formula& formula) const {
    return formula.empty();
}

bool SATSolver::hasEmptyClause(const Formula& formula) const {
    for (const auto& clause : formula) {
        if (clause.empty()) return true;
    }
    return false;
}

// Heuristic for selecting the next literal
int SATSolver::selectLiteral(const Formula& formula) const {
    switch (heuristic) {
        case Heuristic::NAIVE: return selectNaive(formula);
        case Heuristic::MOMS: return selectMOMS(formula);
        case Heuristic::DLIS: return selectDLIS(formula);
        case Heuristic::RANDOM: return selectRandom(formula);
        default: throw std::runtime_error("Unknown heuristic");
    }
}

// Logging heuristic summary
void SATSolver::logHeuristicSummary(Heuristic heuristic, bool result, double timeTaken) const {
    // Add a line separator between normal logging and summary
    Logger::log(0, "-----------------------------------");
    
    // Log result and time taken at level 0
    Logger::logf(0, "Result: %", result ? "SATISFIABLE" : "UNSATISFIABLE");
    Logger::logf(0, "Time taken: % seconds", timeTaken);

    // Log heuristic, assignment, decisions, and backtracks at level 1
    Logger::logf(1, "Heuristic: %", heuristicToString(heuristic));
    Logger::log(1, "Assignment:");
    printAssignment(finalAssignment, 1); // Use Logger-controlled log level for assignment
    Logger::logf(1, "Decisions made: %", decisions);
    Logger::logf(1, "Backtracks: %", backtracks);

    // Add a line separator after the summary
    Logger::log(0, "-----------------------------------");
}

// Convert heuristic enum to string
std::string heuristicToString(Heuristic heuristic) {
    switch (heuristic) {
        case Heuristic::NAIVE:
            return "naive";
        case Heuristic::MOMS:
            return "moms";
        case Heuristic::DLIS:
            return "dlis";
        case Heuristic::RANDOM:
            return "random";
        default:
            throw std::runtime_error("Unknown heuristic");
    }
}

// Individual heuristic implementations
int SATSolver::selectNaive(const Formula& formula) const {
    for (const auto& clause : formula) {
        for (int literal : clause) {
            return abs(literal);
        }
    }
    return 0;
}

int SATSolver::selectMOMS(const Formula& formula) const {
    std::unordered_map<int, int> literalCounts;
    int minClauseSize = INT_MAX;

    for (const auto& clause : formula) {
        int clauseSize = clause.size();
        if (clauseSize < minClauseSize) {
            minClauseSize = clauseSize;
            literalCounts.clear();
        }
        if (clauseSize == minClauseSize) {
            for (int literal : clause) {
                literalCounts[abs(literal)]++;
            }
        }
    }

    return std::max_element(literalCounts.begin(), literalCounts.end(),
                            [](const auto& a, const auto& b) { return a.second < b.second; })->first;
}

int SATSolver::selectDLIS(const Formula& formula) const {
    std::unordered_map<int, int> posCounts, negCounts;

    for (const auto& clause : formula) {
        for (int literal : clause) {
            if (literal > 0) {
                posCounts[literal]++;
            } else {
                negCounts[-literal]++;
            }
        }
    }

    int bestLiteral = 0;
    int maxCount = -1;

    for (const auto& [literal, count] : posCounts) {
        if (count > maxCount) {
            bestLiteral = literal;
            maxCount = count;
        }
    }

    for (const auto& [literal, count] : negCounts) {
        if (count > maxCount) {
            bestLiteral = -literal;
            maxCount = count;
        }
    }

    return bestLiteral;
}

int SATSolver::selectRandom(const Formula& formula) const {
    std::vector<int> literals;
    for (const auto& clause : formula) {
        for (int literal : clause) {
            literals.push_back(abs(literal));
        }
    }
    if (literals.empty()) return 0;
    return literals[rand() % literals.size()];
}

// Simplify the formula by propagating a literal
SATSolver::Formula SATSolver::propagate(Formula formula, int literal, Assignment& assignment) const {
    assignment.insert(literal);
    Formula newFormula;

    for (auto& clause : formula) {
        if (std::find(clause.begin(), clause.end(), literal) != clause.end()) {
            continue;
        }

        Clause newClause;
        for (int lit : clause) {
            if (lit != -literal) newClause.push_back(lit);
        }

        newFormula.push_back(newClause);
    }

    return newFormula;
}

void SATSolver::undoPropagatedLiterals(Assignment& assignment, const std::vector<int>& propagatedLiterals) const {
    for (int literal : propagatedLiterals) {
        assignment.erase(literal);
    }
}

// Utility functions
int SATSolver::findUnitClause(const Formula& formula) const {
    for (const auto& clause : formula) {
        if (clause.size() == 1) return clause[0];
    }
    return 0;
}

int SATSolver::findPureLiteral(const Formula& formula) const {
    std::unordered_map<int, int> literalCounts;
    for (const auto& clause : formula) {
        for (int literal : clause) literalCounts[literal]++;
    }

    for (const auto& [literal, _] : literalCounts) {
        if (literalCounts.find(-literal) == literalCounts.end()) return literal;
    }
    return 0;
}

void SATSolver::printFormula(const Formula& formula, int logLevel) const {
    if (logLevel == -1 || Logger::getLevel() >= logLevel) {
        if (formula.empty()) {
            std::cout << "None\n";
        } else {
            for (const auto& clause : formula) {
                std::cout << "(";
                for (size_t i = 0; i < clause.size(); ++i) {
                    std::cout << clause[i];
                    if (i < clause.size() - 1) {
                        std::cout << " ";
                    }
                }
                std::cout << ") ";
            }
            std::cout << "\n";
        }
    }
}

void SATSolver::printAssignment(const Assignment& assignment, int logLevel) const {
    if (logLevel == -1 || Logger::getLevel() >= logLevel) {
        if (assignment.empty()) {
            std::cout << "None\n";
        } else {
            std::vector<int> sortedAssignment(assignment.begin(), assignment.end());
            std::sort(sortedAssignment.begin(), sortedAssignment.end(),
                      [](int a, int b) { return std::abs(a) < std::abs(b); });
            for (int literal : sortedAssignment) {
                std::cout << literal << " ";
            }
            std::cout << "\n";
        }
    }
}