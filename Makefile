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

SRCDIR = src
BINDIR = bin

CPLEX_128 = /opt/ibm/ILOG/CPLEX_Studio128
CPLEX_129 = /opt/ibm/ILOG/CPLEX_Studio129

ifneq "$(wildcard $(CPLEX_BLADE) )" ""
	CPLEX_LOC = $(CPLEX_BLADE)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_BLADE)/cplex/lib/x86-64_linux/static_pic
else ifneq "$(wildcard $(CPLEX_129) )" ""
	CPLEX_LOC = $(CPLEX_129)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_129)/cplex/lib/x86-64_linux/static_pic
else ifneq "$(wildcard $(CPLEX_128) )" ""
	CPLEX_LOC = $(CPLEX_128)/cplex/include/ilcplex
	LIB_LOC = $(CPLEX_128)/cplex/lib/x86-64_linux/static_pic
endif

INCL = -I $(CPLEX_LOC)
LIBS = -L ${LIB_LOC}  -lcplex -lm -lpthread -ldl

FILES = $(wildcard $(SRCDIR)/*.c) #this function lists all .c files
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(FILES)) #substitute file.c -> file.o

current_dir = $(shell pwd)

# C Compiler options
CFLAGS = #
#-O3 -pipe -DNDEBUG -Wimplicit -Wparentheses -Wsequence-point -Wreturn-type -Wcast-qual -Wall -Wno-unknown-pragmas -Wno-long-long   -DCBC_BUILD

all: $(TARGET)

$(TARGET):$(OBJECTS)
	$(CC) $(WARN) $(OBJECTS) -o $(TARGET) $(LIBS) $(CFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(WARN) -c -o $@ $< $(INCL) $(LIBS) $(CFLAGS)

clean:
	$(RMF) $(BINDIR)/*.o
	$(RMF) $(TARGET)
	$(RMF) *.lp
	$(RMF) *.mps
	$(RMF) *.sav
	$(RMF) *.log
	$(RMF) *.txt

again:
	make clean
	make