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
WARN = #-g -Wall -Wpedantic -fsanitize=address -Wextra 
#-Wall -g
TARGET = main

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
CURRENT_DIR = $(shell pwd)

CPLEX_128 = /opt/ibm/ILOG/CPLEX_Studio128
CPLEX_129 = /opt/ibm/ILOG/CPLEX_Studio129
CPLEX_1210 = /opt/ibm/ILOG/CPLEX_Studio1210
CPLEX_201 = /opt/ibm/ILOG/CPLEX_Studio201

ifneq "$(wildcard $(CPLEX_BLADE) )" ""
	CPLEX_LOC = $(CPLEX_BLADE)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_BLADE)/cplex/lib/x86-64_linux/static_pic
else ifneq "$(wildcard $(CPLEX_201) )" ""
	CPLEX_LOC = $(CPLEX_201)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_201)/cplex/lib/x86-64_linux/static_pic
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

CPLEXLIB = cplex$(dynamic:yes=2010)
CPLEX_LIBS = -L ${LIB_LOC} -l$(CPLEXLIB) -lm -lpthread -ldl

INCL = $(COPT) -I $(CPLEX_LOC)

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) #this function lists all .c files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES)) #substitute file.c -> file.o

# C Compiler options
CFLAGS = #
#-O3 -pipe -DNDEBUG -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wcast-qual -Wall -Wno-unknown-pragmas -Wno-long-long   -DCBC_BUILD

.PHONY: createdir all clean again

all: createdir $(TARGET)

$(TARGET):$(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET) $(CPLEX_LIBS) $(WARN) $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(INCL) $(CPLEX_LIBS) $(WARN) $(CFLAGS)

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