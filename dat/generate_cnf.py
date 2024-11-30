import random

def generate_random_cnf(num_vars, num_clauses, filename):
    with open(filename, 'w') as f:
        f.write(f"p cnf {num_vars} {num_clauses}\n")
        for _ in range(num_clauses):
            clause = [random.choice(range(1, num_vars + 1)) * random.choice([-1, 1])
                      for _ in range(3)]
            f.write(" ".join(map(str, clause)) + " 0\n")

# Generate a CNF file with 20 variables and 85 clauses
generate_random_cnf(20, 85, "hard_3sat.cnf")