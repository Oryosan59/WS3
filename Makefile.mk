# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic # Add any other desired compiler flags

# --- Directory Definitions ---
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include # Project's own include directory

# --- External Library Paths ---
# Use the specific path provided in the command
NAVIGATOR_LIB_PATH = /home/pi/navigator-lib/target/debug

# --- Include Directories ---
# Add project's include dir and external lib include dir
INCLUDES = -I$(INC_DIR) -I$(NAVIGATOR_LIB_PATH)

# --- Library Directories ---
LDFLAGS = -L$(NAVIGATOR_LIB_PATH) \
          -Wl,-rpath=$(NAVIGATOR_LIB_PATH) # Set rpath for runtime linking

# --- Libraries to link ---
# Use the specific library name provided in the command
LIBS = -lbluerobotics_navigator -lpthread -lm

# --- Target Executable ---
TARGET_NAME = navigator_control
TARGET = $(BIN_DIR)/$(TARGET_NAME)

# --- Source and Object Files ---
# Find all .cpp files in SRC_DIR
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Generate object file names in OBJ_DIR based on source file names
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# --- Default target: build the executable ---
all: $(TARGET)

# --- Rule to link the executable ---
$(TARGET): $(OBJS) | $(BIN_DIR) # Ensure BIN_DIR exists before linking
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo "Build complete: $(TARGET)"

# --- Rule to compile source files into object files ---
# Compile .cpp files from SRC_DIR into .o files in OBJ_DIR
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) # Ensure OBJ_DIR exists before compiling
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# --- Directory Creation ---
# These targets create the directories if they don't exist
# They are prerequisites for the compile and link rules using Order-only prerequisites (|)
$(OBJ_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

# --- Target to clean up build artifacts ---
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned."

# --- Phony targets (targets that don't represent files) ---
.PHONY: all clean $(OBJ_DIR) $(BIN_DIR)

# --- Prevent intermediate files from being deleted ---
.SECONDARY: $(OBJS)
