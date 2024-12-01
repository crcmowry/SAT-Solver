#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

// Heuristic enum for branching strategies
enum class Heuristic {
    NAIVE,
    MOMS,
    DLIS,
    DSIDS,
    RANDOM
};

// Converts a heuristic enum to a string representation
std::string heuristicToString(Heuristic heuristic);  // Add this line

class SATSolver {
public:
    using Clause = std::vector<int>;
    using Formula = std::vector<Clause>;
    using Assignment = std::unordered_set<int>;

    SATSolver() : heuristic(Heuristic::NAIVE) {}

    bool solve(const Formula& formula);

    void setHeuristic(Heuristic h) { heuristic = h; }

    void logHeuristicSummary(Heuristic heuristic, bool result, double time) const;

private:
    bool DPLL(Formula formula, Assignment& assignment);
    bool isSatisfied(const Formula& formula) const;
    bool hasEmptyClause(const Formula& formula) const;
    int findUnitClause(const Formula& formula) const;
    int findPureLiteral(const Formula& formula) const;
    Formula propagate(Formula formula, int literal, Assignment& assignment) const;
    void undoPropagatedLiterals(Assignment& assignment, const std::vector<int>& propagatedLiterals) const;
    int selectLiteral(const Formula& formula) const;
    int selectNaive(const Formula& formula) const;
    int selectMOMS(const Formula& formula) const;
    int selectDLIS(const Formula& formula) const;
    int selectDSIDS(const Formula& formula) const;
    int selectRandom(const Formula& formula) const;

    void printFormula(const Formula& formula, int logLevel) const;
    void printAssignment(const Assignment& assignment, int logLevel) const;

    // DSIDS-specific data
    mutable std::unordered_map<int, double> literalScores; // Dynamic scores for literals
    double decayFactor = 0.95;                             // Score decay factor

    // DSIDS methods
    void initializeDSIDS(const Formula& formula);
    void updateScores(int literal, const Formula& formula);
    void decayScores();
    int selectDSIDS(const Formula& formula);

    Heuristic heuristic;

    // Metrics and results
    mutable Assignment finalAssignment; // Store the final assignment
    mutable int decisions;             // Count the number of decisions made
    mutable int backtracks;            // Count the number of backtracks
};

#endif // SAT_SOLVER_H