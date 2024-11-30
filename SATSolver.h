#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <vector>
#include <unordered_set>
#include <iostream>

class SATSolver {
public:
    // Type aliases for readability
    using Clause = std::vector<int>;          // Represents a single clause
    using Formula = std::vector<Clause>;      // Represents a CNF formula
    using Assignment = std::unordered_set<int>;  // Stores assigned literals (positive for True, negative for False)

    // Public method to solve a SAT problem
    bool solve(const Formula& formula);

private:
    // Core DPLL algorithm
    bool DPLL(Formula formula, Assignment& assignment);

    // Helper methods for DPLL
    bool isSatisfied(const Formula& formula) const;     // Check if the formula is satisfied
    bool hasEmptyClause(const Formula& formula) const;  // Check if the formula contains an empty clause
    Formula propagate(Formula formula, int literal, Assignment& assignment);  // Perform propagation for a literal
    void undoPropagatedLiterals(Assignment& assignment, const std::vector<int>& propagatedLiterals) const;
    int selectVariable(const Formula& formula) const;   // Choose a literal to branch on
    int findUnitClause(const Formula& formula) const;   // Find a unit clause, if any
    int findPureLiteral(const Formula& formula) const;  // Find a pure literal, if any

    // Debugging and utility
    void printFormula(const Formula& formula, int logLevel) const;  // Print the current assignment
    void printAssignment(const Assignment& assignment, int logLevel) const;  // Print the current assignment
};

#endif // SAT_SOLVER_H