# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/allocator/MemoryManager.cpp \
          $(SRC_DIR)/buddy/BuddyAllocator.cpp \
          $(SRC_DIR)/cache/Cache.cpp \
          $(SRC_DIR)/virtual_memory/VirtualMemory.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/memsim

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/allocator
	@mkdir -p $(BUILD_DIR)/buddy
	@mkdir -p $(BUILD_DIR)/cache
	@mkdir -p $(BUILD_DIR)/virtual_memory
	@mkdir -p $(BIN_DIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Run the simulator
run: all
	./$(TARGET)

# Install (optional)
install: all
	@echo "Installing to /usr/local/bin..."
	@cp $(TARGET) /usr/local/bin/
	@echo "Installation complete"

# Uninstall
uninstall:
	@echo "Removing from /usr/local/bin..."
	@rm -f /usr/local/bin/memsim
	@echo "Uninstall complete"

# Help
help:
	@echo "Memory Management Simulator - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build the simulator (default)"
	@echo "  clean     - Remove build files"
	@echo "  run       - Build and run the simulator"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"

.PHONY: all directories clean run install uninstall help