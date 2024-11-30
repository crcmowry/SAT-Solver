# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executable
TARGET = bin/SATSolver

# Source and object files
SRC_DIR = src
OBJ_DIR = obj
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Default rule
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Run rule
INPUT_FILE = dat/hard_3sat.cnf
run: $(TARGET)
	@echo "Running SATSolver with input file $(INPUT_FILE)..."
	./$(TARGET) $(INPUT_FILE)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(TARGET)