# ============================================================================
# MAKEFILE — BDbg (Byte Debugger)
# ============================================================================
#
# HOW THIS MAKEFILE WORKS:
# ------------------------
# 1. Find all .c files in src/ using wildcard
# 2. Transform those paths (src/*.c) into object file paths (obj/*.o)
# 3. Compile each .c file into a .o file
# 4. Link all .o files together into the final executable in bin/
#
# SPECIAL CHARACTERS DICTIONARY:
# -------------------------------
# $()     = Variable expansion (like ${} in bash)
# :       = "target: dependencies" or pattern substitution $(VAR:old=new)
# %       = Wildcard pattern capture/reuse
# $@      = The target name
# $<      = First dependency
# $^      = All dependencies
# |       = Order-only prerequisite (must exist, doesn't trigger rebuild)
# @       = Silence command echo
# .PHONY  = Declares targets that aren't actual files

# ============================================================================

# ~ Compiler
CC = gcc
FLAGS = -Wall -Wextra -std=gnu17 -g

# ~ Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# ~ Silent
NDIR = --no-print-directory

# ~ Target exec
TARGET = $(BIN_DIR)/bdbg

# ~ Source files and objects
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ~ Default
all: $(TARGET)

# ~ Link objects to create the executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@$(CC) $(OBJECTS) -o $@

# ~ Compile source files to objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(FLAGS) -I$(INC_DIR) -c $< -o $@

# ~ Create directories if they do not exist
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# ~ Run the executable (ptrace requires elevated privileges in some configurations)
run: $(TARGET)
	@sudo $(TARGET)
	@$(MAKE) $(NDIR) clean

# ~ Clean directory
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

# ~ Phony targets
.PHONY: all run clean