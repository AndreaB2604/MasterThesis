#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>

/**
 * This structure represents a bidimensional point with two coordinates
 */
typedef struct {
	double x;
	double y;
} point_2d;


int equals(point_2d *a, point_2d *b);
int contains(point_2d *list, int list_size, point_2d *a);
void random_hospital_generator(point_2d *hosp_points, int hosp_num, int x_lim, int y_lim, double sqr_dim);

/*   MAIN   */
int main(int argc, char **argv) {

	const int x_km = 10, y_km = 10;
	const double sqr_dim = 1;
	const int hosp_num = 4;
	
	int x_lim = (int)(x_km/sqr_dim + 0.5);
	int y_lim = (int)(y_km/sqr_dim + 0.5);

	char *name = "SQUARE_GRID";
	char *dist_type = "EUC_2D";
	char *file_name = "grid100.txt";
	FILE *file = fopen(file_name, "w");

	fprintf(file, "NAME : %s\n", name);
	fprintf(file, "DIMENSION : %d\n", (x_km*y_km));
	fprintf(file, "NUMBER_HOSPITAL : %d\n", hosp_num);
	fprintf(file, "EDGE_WEIGHT_TYPE : %s\n", dist_type);
	fprintf(file, "SERVICE_POINTS_SECTION\n");

	for(int j = 0; j < y_lim; j++)
		for(int i = 0; i < x_lim; i++) {
			double x_coord = sqr_dim * (i + 0.5);
			double y_coord = sqr_dim * (j + 0.5);
			fprintf(file, "%d %f %f\n", (j*10+i+1), x_coord, y_coord);
		}

	fprintf(file, "\nHOSPITALS_POINTS_SECTION\n");
	point_2d *hosp_points = (point_2d *) calloc(hosp_num, sizeof(point_2d));
	random_hospital_generator(hosp_points, hosp_num, x_lim, y_lim, sqr_dim);

	for(int i = 0; i < hosp_num; i++) {
		fprintf(file, "%d %f %f\n", i, hosp_points[i].x, hosp_points[i].y);
	}

	free(hosp_points);

}

/**
 * @brief      Generate a random sequence of different point_2d which represent
 *             the coordinates of the hospitals in a rectangle of dimension
 *             x_lim*y_lim. 
 *             Note that hosp_num must be at most equal to the hosp_points size  
 *
 * @param      hosp_points  The array that will contain the random point_2d
 * @param[in]  hosp_num     The number of hospitals
 * @param[in]  x_lim        The x limit
 * @param[in]  y_lim        The y limit
 */
void random_hospital_generator(point_2d *hosp_points, int hosp_num, int x_lim, int y_lim, double sqr_dim) {

	struct timeval start;

	int i = 0;
	while(i < hosp_num) {
		gettimeofday(&start, NULL);
		srandom(start.tv_usec);
		point_2d new_point;
		new_point.x = random() % x_lim;
		new_point.y = random() % y_lim;

		if(!contains(hosp_points, i, &new_point)) {
			hosp_points[i].x = new_point.x + sqr_dim/2;
			hosp_points[i].y = new_point.y + sqr_dim/2;
			i++;
		}	
	}
}

/**
 * @brief      Check if a point_2d is contained in a point_2d array
 *
 * @param      list       The array of point_2d
 * @param[in]  list_size  The array size
 * @param      a          The point_2d
 *
 * @return     - 1 if the points are equal
 *             - 0 otherwise
 */
int contains(point_2d *list, int list_size, point_2d *a) {
	for(int i = 0; i < list_size; i++)
		if(equals(&(list[i]), a))
			return 1;
	return 0;
}


/**
 * @brief      Check if two point_2d are equal
 *
 * @param      a     The first point
 * @param      b     The second point
 *
 * @return     - 1 if the points are equal
 *             - 0 otherwise
 */
int equals(point_2d *a, point_2d *b) {
	if(a->x == b->x && a->y == b->y)
		return 1;
	return 0;
}