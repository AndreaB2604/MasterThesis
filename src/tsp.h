#ifndef TSP_H_
#define TSP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <float.h>
#include <Cbc_C_Interface.h>

#define VERBOSE 10000 // printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log) 
#define LOG_LEVEL 0
#define TOLERANCE 0.5

typedef struct
{
	// input data
	int nnodes;
	double *xcoord; 						// they could be integer
	double *ycoord;

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
void connected_components(instance *inst, double *best_sol, int columns_number, int *nodes_components, int *components_values, int *number_cc);
double dist(int i, int j, instance *inst);
void free_instance(instance *inst);
void init_instance(instance *inst);
unsigned long microseconds();
void parse_command_line(int argc, char** argv, instance *inst);
void print_best_sol(instance *inst);
void print_error(const char *err);
void print_plot(instance *inst, char *plot_file_name);
void read_input(instance *inst);
double tour_dist(instance *inst, int *v);
int xpos(int i, int j, instance *inst);
int xpos_mtz(int i, int j, instance *inst);
int zpos_compact_custom(int i, int j, instance *inst);

// defined in subtour.c
void build_cbc_model(instance *inst, Cbc_Model *mod);
int TSPopt_subtour(instance *inst);

// defined in sec_loop.c
int sec_loop(Cbc_Model *old_mod, Cbc_Model *new_mod, instance *inst);
int TSPopt_sec_loop(instance *inst);

// defined in write_sec_loop.c
int TSPopt_write_sec_loop(instance *inst);

// defined in sec_callback.c
int TSPopt_sec_callback(instance *inst);
void sec_callback(void *mod, void *mod_cut, void *appData);
int myseparation(instance *inst, double *xstar, void *mod_cut);

#endif
