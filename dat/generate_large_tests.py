import random
import os
import subprocess
import time
import argparse
import uuid


def generate_random_cnf(num_vars, num_clauses, filename):
    """
    Generate a random 3-SAT CNF formula and save to a file.

    Args:
        num_vars (int): Number of variables.
        num_clauses (int): Number of clauses.
        filename (str): Name of the file to save the formula.
    """
    clauses = set()
    while len(clauses) < num_clauses:
        clause = [random.choice(range(1, num_vars + 1)) * random.choice([-1, 1]) for _ in range(3)]
        clauses.add(tuple(sorted(clause)))

    with open(filename, 'w') as f:
        f.write(f"p cnf {num_vars} {num_clauses}\n")
        for clause in clauses:
            f.write(" ".join(map(str, clause)) + " 0\n")


def measure_solver_time(solver_path, cnf_file):
    """
    Run the SAT solver on a CNF file and measure its execution time.

    Args:
        solver_path (str): Path to the SAT solver binary.
        cnf_file (str): Path to the CNF file.

    Returns:
        float: Execution time in seconds.
    """
    start_time = time.time()
    try:
        subprocess.run([solver_path, cnf_file], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
    except subprocess.CalledProcessError:
        pass  # Ignore errors for this timing process
    return time.time() - start_time


def generate_test_cases(solver_path, output_dir, num_cases=10, target_time=1.0):
    """
    Generate CNF test cases aiming for ~1 second execution time.

    Args:
        solver_path (str): Path to the SAT solver binary.
        output_dir (str): Directory to save the CNF files.
        num_cases (int): Number of test cases to generate.
        target_time (float): Target runtime in seconds for each test case.
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for _ in range(num_cases):
        num_vars = 50
        clause_to_var_ratio = 4.26
        adjustment_factor = 0.1
        unique_id = uuid.uuid4().hex[:8]  # Use the first 8 characters of a UUID
        filename = os.path.join(output_dir, f"large_test_case_{unique_id}.cnf")

        while True:
            num_clauses = int(num_vars * clause_to_var_ratio)
            generate_random_cnf(num_vars, num_clauses, filename)

            runtime = measure_solver_time(solver_path, filename)

            if abs(runtime - target_time) < 0.1:  # Close enough to the target time
                print(f"Generated {filename}: {runtime:.2f}s (vars: {num_vars}, clauses: {num_clauses})")
                break
            elif runtime < target_time:
                num_vars = int(num_vars * (1 + adjustment_factor))
                clause_to_var_ratio *= 1.1  # Add complexity by increasing clause density
            else:
                num_vars = int(num_vars * (1 - adjustment_factor))
                clause_to_var_ratio *= 0.9  # Reduce complexity

            # Prevent too small or too large values
            num_vars = max(10, min(num_vars, 1000))
            clause_to_var_ratio = max(1.5, min(clause_to_var_ratio, 10))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate large SAT test cases.")
    parser.add_argument("--output-dir", type=str, default="large_test_cases", help="Directory to save the generated CNF files.")
    parser.add_argument("--num-tests", type=int, default=10, help="Number of test cases to generate.")
    parser.add_argument("--solver-path", type=str, default="../bin/SATSolver", help="Path to the SAT solver binary.")
    args = parser.parse_args()

    generate_test_cases(args.solver_path, args.output_dir, args.num_tests)