#include "SATSolver.h"
#include "Logger.h"

int main() {
    SATSolver solver;

    // Set logging verbosity level (0 = None, 1 = Minimal, 2 = Detailed, 3 = Trace)
    Logger::setLevel(0);

    SATSolver::Formula formula = {
        {1, 2, -3},
        {1, -2, 3},
        {1, -2, -3},
        {-1, 2, 3},
        {-1, 2, -3},
        {-1, -2, 3},
        {-1, -2, -3},
    };

    solver.solve(formula);

    return 0;
}