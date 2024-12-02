# Python scripts
LARGE_TEST_GENERATOR = generate_large_tests.py
SMALL_TEST_GENERATOR = generate_small_traceable_tests.py

# Test case directories
LARGE_TEST_DIR = large_test_cases
SMALL_TEST_DIR = small_traceable_test_cases

# Default rule to generate all test cases
all: generate-large-tests generate-small-tests

# Rule to generate large test cases
generate-large-tests:
	mkdir -p $(LARGE_TEST_DIR)
	python3 $(LARGE_TEST_GENERATOR) --output-dir=$(LARGE_TEST_DIR) --num-tests=10

# Rule to generate small traceable test cases
generate-small-tests:
	mkdir -p $(SMALL_TEST_DIR)
	python3 $(SMALL_TEST_GENERATOR) --output-dir=$(SMALL_TEST_DIR) --num-tests=10

# Clean rule
clean:
	rm -rf $(LARGE_TEST_DIR) $(SMALL_TEST_DIR)