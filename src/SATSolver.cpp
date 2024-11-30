#include "SATSolver.h"
#include "Logger.h"
#include <algorithm>
#include <unordered_map>

// Main entry point for solving a SAT problem
bool SATSolver::solve(const Formula& formula) {
    Assignment assignment;
    if (DPLL(formula, assignment)) {
        // Print the assignment at log level 1
        Logger::log(1, "Assignment:");
        printAssignment(assignment, 1);
        return true;
    } else {
        return false;
    }
}

// Recursive DPLL algorithm
bool SATSolver::DPLL(Formula formula, Assignment& assignment) {
    Logger::log(3, "Current formula:");
    printFormula(formula, 3);

    Logger::log(3, "Current assignment:");
    printAssignment(assignment, 3);

    // Track literals assigned during propagation
    std::vector<int> propagatedLiterals;

    // Unit propagation
    while (true) {
        int unitLiteral = findUnitClause(formula);
        if (unitLiteral == 0) break;
        Logger::logf(2, "Unit propagation with literal: %", unitLiteral);
        formula = propagate(formula, unitLiteral, assignment);
        propagatedLiterals.push_back(unitLiteral);
        Logger::log(3, "Current formula after propagation:");
        printFormula(formula, 3);
        Logger::log(3, "Current assignment after propagation:");
        printAssignment(assignment, 3);
    }

    // Pure literal elimination
    while (true) {
        int pureLiteral = findPureLiteral(formula);
        if (pureLiteral == 0) break;
        Logger::logf(2, "Pure literal elimination with literal: %", pureLiteral);
        formula = propagate(formula, pureLiteral, assignment);
        propagatedLiterals.push_back(pureLiteral);
        Logger::log(3, "Current formula after pure literal elimination:");
        printFormula(formula, 3);
        Logger::log(3, "Current assignment after pure literal elimination:");
        printAssignment(assignment, 3);
    }

    // Stopping conditions
    if (formula.empty()) {
        Logger::log(2, "Formula satisfied!");
        return true;
    }
    if (hasEmptyClause(formula)) {
        Logger::log(2, "Encountered an empty clause. Backtracking...");
        undoPropagatedLiterals(assignment, propagatedLiterals);
        return false;
    }

    // Choose a literal and branch
    int literal = selectVariable(formula);
    Logger::logf(2, "Choosing literal: %", literal);

    // Try assigning True
    Logger::logf(2, "Trying literal % = True", literal);
    assignment.insert(literal);
    Formula formulaTrue = propagate(formula, literal, assignment);
    if (DPLL(formulaTrue, assignment)) return true;
    assignment.erase(literal);  // Backtrack branching literal

    // Try assigning False
    Logger::logf(2, "Trying literal % = False", literal);
    assignment.insert(-literal);
    Formula formulaFalse = propagate(formula, -literal, assignment);
    if (DPLL(formulaFalse, assignment)) return true;
    assignment.erase(-literal);  // Backtrack branching literal

    // Undo propagated literals and backtrack further
    Logger::logf(2, "Both True and False failed for literal: %. Backtracking further...", literal);
    undoPropagatedLiterals(assignment, propagatedLiterals);
    return false;
}

// Check if the formula is satisfied (no clauses left)
bool SATSolver::isSatisfied(const Formula& formula) const {
    return formula.empty();
}

// Check if the formula contains an empty clause (conflict)
bool SATSolver::hasEmptyClause(const Formula& formula) const {
    for (const auto& clause : formula) {
        if (clause.empty()) return true;
    }
    return false;
}

// Simplify the formula by propagating a literal
SATSolver::Formula SATSolver::propagate(Formula formula, int literal, Assignment& assignment) {
    assignment.insert(literal);
    Formula newFormula;
    int clausesRemoved = 0;

    for (auto& clause : formula) {
        // Skip satisfied clauses
        if (std::find(clause.begin(), clause.end(), literal) != clause.end()) {
            clausesRemoved++;
            continue;
        }

        // Remove negated literal from the clause
        Clause newClause;
        for (int lit : clause) {
            if (lit != -literal) newClause.push_back(lit);
        }

        newFormula.push_back(newClause);
    }

    Logger::logf(3, "Propagation summary: % clauses removed", clausesRemoved);

    return newFormula;
}

void SATSolver::undoPropagatedLiterals(Assignment& assignment, const std::vector<int>& propagatedLiterals) const {
    for (int literal : propagatedLiterals) {
        assignment.erase(literal);
    }
}

// Select the first unassigned variable from the formula
int SATSolver::selectVariable(const Formula& formula) const {
    for (const auto& clause : formula) {
        for (int literal : clause) return abs(literal);
    }
    return 0;  // Should never reach here if called correctly
}

// Find a unit clause (clause with a single literal)
int SATSolver::findUnitClause(const Formula& formula) const {
    for (const auto& clause : formula) {
        if (clause.size() == 1) return clause[0];
    }
    return 0;  // No unit clause found
}

// Find a pure literal (literal appearing only positively or negatively)
int SATSolver::findPureLiteral(const Formula& formula) const {
    std::unordered_map<int, int> literalCounts;
    for (const auto& clause : formula) {
        for (int literal : clause) literalCounts[literal]++;
    }

    for (const auto& [literal, _] : literalCounts) {
        if (literalCounts.find(-literal) == literalCounts.end()) return literal;
    }
    return 0;  // No pure literal found
}

// Print the current formula
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

// Print the current assignment
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