#ifndef GRID_H_
#define GRID_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <float.h>

#define VERBOSE 10 // printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log) 
#define DOUBLE_TOL 10e-7

typedef struct {
	double x;
	double y;
} point_2d;


/* PROTOTYPES */

// defined in grid_generator.c
double random_double(double min, double max);
void random_hospital_generator(point_2d *points_coord, long points_coord_size, point_2d *hosp_points, int HOSP_NUM);
long random_int(long min, long max);


#endif