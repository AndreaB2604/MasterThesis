#ifndef GRID_H_
#define GRID_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <float.h>

#define VERBOSE 10000 // printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log) 
#define DOUBLE_TOL 10e-7

typedef struct {
	double x;
	double y;
} point_2d;


/* PROTOTYPES */

// defined in grid_generator.c
int contains(point_2d *list, int list_size, point_2d *a);
int equals(point_2d *a, point_2d *b);
double random_double(double min, double max);
void random_hospital_generator(point_2d *hosp_points, int hosp_num, int x_lim, int y_lim, double sqr_dim);
long random_int(long min, long max);


#endif