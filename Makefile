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
TARGET = tsp

SRCDIR = src
BINDIR = bin
CBC_ROOT_DIR = $(HOME)/CBC-COINOR/build
CBCLIBS = $(CBC_ROOT_DIR)/lib
INCL = -I $(CBC_ROOT_DIR)/include/coin -I $(CBC_ROOT_DIR)/include/coin/ThirdParty
RPATH = -Wl,-rpath=$(CBCLIBS) #
LIBSFLAG1 = -lpthread -lCbcSolver -lCbc -lOsiCbc -lm -ldl -lOsiClp -lClpSolver -lClp #-lcoinasl -lcoinmumps
LIBS = -L $(CBCLIBS) $(RPATH) $(LIBSFLAG1)

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