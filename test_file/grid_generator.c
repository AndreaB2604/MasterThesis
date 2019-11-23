#include "grid.h"

/*   MAIN   */
int main(int argc, char **argv) {

	const int x_km = 10, y_km = 10;
	const double sqr_dim = 1;
	const int hosp_num = 4;
	
	int x_lim = (int)(x_km/sqr_dim + 0.5);
	int y_lim = (int)(y_km/sqr_dim + 0.5);

	char *name = "SQUARE_GRID";
	char *dist_type = "EUC_2D";
	char *file_name = "grid100.flow";
	FILE *file = fopen(file_name, "w");

	fprintf(file, "NAME : %s\n", name);
	fprintf(file, "DIMENSION : %d\n", (x_km*y_km));
	fprintf(file, "NUMBER_HOSPITALS : %d\n", hosp_num);
	fprintf(file, "EDGE_WEIGHT_TYPE : %s\n", dist_type);
	fprintf(file, "COMMENT : FOR POINTS (n_point x_coord y_coord request)\n");
	fprintf(file, "COMMENT : FOR HOSPITALS (n_point x_coord y_coord max_capacity)\n");
	fprintf(file, "NODE_COORD_SECTION\n");

	for(int j = 0; j < y_lim; j++)
		for(int i = 0; i < x_lim; i++) {
			double x_coord = sqr_dim * (i + 0.5);
			double y_coord = sqr_dim * (j + 0.5);
			fprintf(file, "%d %f %f %f\n", (j*10+i+1), x_coord, y_coord, random_double(0, 10));
		}

	fprintf(file, "\nHOSPITALS_COORD_SECTION\n");
	point_2d *hosp_points = (point_2d *) calloc(hosp_num, sizeof(point_2d));
	random_hospital_generator(hosp_points, hosp_num, x_lim, y_lim, sqr_dim);

	for(int i = 0; i < hosp_num; i++) {
		fprintf(file, "%d %f %f %d\n", (i+1), hosp_points[i].x, hosp_points[i].y, ((int) random_int(50, 100)));
	}

	free(hosp_points);

}

void random_hospital_generator(point_2d *hosp_points, int hosp_num, int x_lim, int y_lim, double sqr_dim) {

	int i = 0;
	while(i < hosp_num) {
		point_2d new_point;
		new_point.x = (int) random_int(0, x_lim);
		new_point.y = (int) random_int(0, y_lim);

		if(!contains(hosp_points, i, &new_point)) {
			hosp_points[i].x = new_point.x + sqr_dim/2;
			hosp_points[i].y = new_point.y + sqr_dim/2;
			i++;
		}	
	}
}

int contains(point_2d *list, int list_size, point_2d *a) {
	for(int i = 0; i < list_size; i++)
		if(equals(&(list[i]), a))
			return 1;
	return 0;
}

int equals(point_2d *a, point_2d *b) {
	if(a->x == b->x && a->y == b->y)
		return 1;
	return 0;
}

long random_int(long min, long max) {
	struct timeval start;
	gettimeofday(&start, NULL);
	srandom(start.tv_usec);
	long rand = random()%(max-min) + min;
	return rand;
}

double random_double(double min, double max) {
	struct timeval start;
	gettimeofday(&start, NULL);
	srandom(start.tv_usec);
	double rand = ((double)random())/RAND_MAX * (max-min) + min;
	return rand;
}