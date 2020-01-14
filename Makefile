# ------------------------
#	Compiler
# ------------------------
CC = gcc

# -------------------------------------------------------------
#	Flags and stuff needed for compiling
# 	
#	-I <path> is needed to include
# -------------------------------------------------------------
RMF = rm -f
WARN = -g -Wall -Wpedantic -fsanitize=address -Wextra 
#-Wall -g
TARGET = main

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

CPLEX_128 = /opt/ibm/ILOG/CPLEX_Studio128
CPLEX_129 = /opt/ibm/ILOG/CPLEX_Studio129
CPLEX_1210 = /opt/ibm/ILOG/CPLEX_Studio1210

ifneq "$(wildcard $(CPLEX_BLADE) )" ""
	CPLEX_LOC = $(CPLEX_BLADE)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_BLADE)/cplex/lib/x86-64_linux/static_pic
else ifneq "$(wildcard $(CPLEX_1210) )" ""
	CPLEX_LOC = $(CPLEX_1210)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_1210)/cplex/lib/x86-64_linux/static_pic
else ifneq "$(wildcard $(CPLEX_129) )" ""
	CPLEX_LOC = $(CPLEX_129)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_129)/cplex/lib/x86-64_linux/static_pic
else ifneq "$(wildcard $(CPLEX_128) )" ""
	CPLEX_LOC = $(CPLEX_128)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_128)/cplex/lib/x86-64_linux/static_pic
endif

INCL = -I $(CPLEX_LOC)
LIBS = -L ${LIB_LOC}  -lcplex -lm -lpthread -ldl

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) #this function lists all .c files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES)) #substitute file.c -> file.o

current_dir = $(shell pwd)

# C Compiler options
CFLAGS = #
#-O3 -pipe -DNDEBUG -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wcast-qual -Wall -Wno-unknown-pragmas -Wno-long-long   -DCBC_BUILD

.PHONY: createdir all clean again

all: createdir $(TARGET)

$(TARGET):$(OBJ_FILES)
	$(CC) $(WARN) $(OBJ_FILES) -o $(TARGET) $(LIBS) $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(WARN) -c -o $@ $< $(INCL) $(LIBS) $(CFLAGS)

clean:
	$(RMF) $(BUILD_DIR)/*.o
	$(RMF) $(TARGET)
	$(RMF) *.lp
	$(RMF) *.mps
	$(RMF) *.sav
	$(RMF) *.log
	$(RMF) *.txt

createdir:
	mkdir -p build

again:
	make clean
	make