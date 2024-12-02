import random
import os
import argparse
import uuid


def generate_simple_cnf(num_vars, num_clauses, filename):
    """
    Generate a simple and traceable CNF formula with a specified number of variables and clauses.

    Args:
        num_vars (int): Number of variables.
        num_clauses (int): Number of clauses.
        filename (str): Name of the file to save the formula.
    """
    clauses = set()

    # Generate a mix of clauses that ensure traceability
    for _ in range(num_clauses):
        # Random clause with 3 literals
        clause = [random.choice(range(1, num_vars + 1)) * random.choice([-1, 1]) for _ in range(3)]
        clauses.add(tuple(sorted(clause)))

    # Include single-literal (unit) clauses for easier traceability
    for var in range(1, num_vars + 1):
        if len(clauses) >= num_clauses:
            break
        clauses.add((random.choice([-1, 1]) * var,))

    # Include binary clauses for diversity
    for _ in range(num_vars // 2):
        if len(clauses) >= num_clauses:
            break
        var1 = random.choice(range(1, num_vars + 1))
        var2 = random.choice(range(1, num_vars + 1))
        if var1 != var2:
            clauses.add((random.choice([-1, 1]) * var1, random.choice([-1, 1]) * var2))

    # Write CNF to the file
    with open(filename, 'w') as f:
        f.write(f"p cnf {num_vars} {len(clauses)}\n")
        for clause in clauses:
            f.write(" ".join(map(str, clause)) + " 0\n")


def generate_small_test_cases(output_dir, num_cases=10):
    """
    Generate small and traceable test cases with varying parameters.

    Args:
        output_dir (str): Directory to save the CNF files.
        num_cases (int): Number of test cases to generate.
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for _ in range(num_cases):
        # Vary the number of variables and clauses for diversity
        num_vars = random.randint(5, 15)
        num_clauses = random.randint(num_vars + 3, num_vars * 2)

        # Generate a unique filename to avoid conflicts
        unique_id = uuid.uuid4().hex[:8]  # Use the first 8 characters of a UUID
        filename = os.path.join(output_dir, f"small_test_case_{unique_id}.cnf")

        generate_simple_cnf(num_vars, num_clauses, filename)
        print(f"Generated {filename} with {num_vars} variables and {num_clauses} clauses.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate small and traceable CNF test cases.")
    parser.add_argument("--output-dir", type=str, default="small_traceable_test_cases", help="Directory to save the CNF files.")
    parser.add_argument("--num-tests", type=int, default=10, help="Number of test cases to generate (default: 10).")
    args = parser.parse_args()

    # Generate test cases
    generate_small_test_cases(args.output_dir, args.num_tests)