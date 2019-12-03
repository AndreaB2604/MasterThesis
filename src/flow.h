#ifndef FLOW_H_
#define FLOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cplex.h>
#include <cplexx.h>
#include <sys/time.h>
#include <float.h>
#include <limits.h>

#define VERBOSE 100 // printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log) 
#define DOUBLE_TOL 10e-7

typedef struct
{
	// input data
	int nnodes;
	int nhosp;
	double max_distance;
	double *x_nodes;							// they could be integer
	double *y_nodes;
	double *r_nodes;
	double *x_hosp;
	double *y_hosp;
	double *c_hosp;

	// parameters
	double time_limit; 						// long can be used
	char *dist_type;
	char *input_file;
	char *model_type;
	double *best_sol;						// best solution available
	long random_seed;						// random seed
} instance;

/* PROTOTYPES */

// defined in utilities.c
double dist(int i, int j, instance *inst);
void free_instance(instance *inst);
void init_instance(instance *inst);
unsigned long microseconds();
void parse_command_line(int argc, char** argv, instance *inst);
void print_error(const char *err);
void print_plot(instance *inst, char *plot_file_name);
void read_input(instance *inst);

// defined in flow.c
void build_model(instance *inst, CPXENVptr env, CPXLPptr lp);
int flow_opt(instance *inst);
int xpos_flow(int i, int j, instance *inst);

#endif
