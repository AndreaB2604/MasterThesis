#include "grid.h"

/*   MAIN   */
int main(int argc, char **argv) {

	const int X_KM = 10, Y_KM = 10;
	const double SQR_DIM = 0.2;
	const int HOSP_NUM = 4;
	const int MAX_DISTANCE = 7;
	
	int x_lim = (int)(X_KM/SQR_DIM + 0.5);
	int y_lim = (int)(Y_KM/SQR_DIM + 0.5);

	char *name = "SQUARE_GRID";
	char *dist_type = "EUC_2D";
	char *file_name = "grid.flow";
	FILE *file = fopen(file_name, "w+");

	fprintf(file, "NAME : %s\n", name);
	fprintf(file, "DIMENSION : %d\n", x_lim*y_lim);
	fprintf(file, "NUMBER_HOSPITALS : %d\n", HOSP_NUM);
	fprintf(file, "MAX_DISTANCE : %d\n", MAX_DISTANCE);
	fprintf(file, "EDGE_WEIGHT_TYPE : %s\n", dist_type);
	fprintf(file, "COMMENT : FOR POINTS (n_point x_coord y_coord request)\n");
	fprintf(file, "COMMENT : FOR HOSPITALS (n_point x_coord y_coord max_capacity)\n");
	fprintf(file, "NODE_COORD_SECTION\n");

	for(int j = 0, k = 1; j < y_lim; j++)
		for(int i = 0; i < x_lim; i++) {
			double x_coord = SQR_DIM * (i + 0.5);
			double y_coord = SQR_DIM * (j + 0.5);
			double request = random_double(0, 10);
			fprintf(file, "%d %f %f %f\n", k++, x_coord, y_coord, request);
		}

	fprintf(file, "\nHOSPITALS_COORD_SECTION\n");
	point_2d *hosp_points = (point_2d *) calloc(HOSP_NUM, sizeof(point_2d));
	random_hospital_generator(hosp_points, HOSP_NUM, X_KM, Y_KM, SQR_DIM);

	for(int i = 0; i < HOSP_NUM; i++) {
		int h_capacity = random_int(5, 10);
		fprintf(file, "%d %f %f %d\n", (i+1), hosp_points[i].x, hosp_points[i].y, h_capacity);
	}

	free(hosp_points);
	fclose(file);

}

void random_hospital_generator(point_2d *hosp_points, int HOSP_NUM, int x_km, int y_km, double SQR_DIM) {

	int i = 0;
	while(i < HOSP_NUM) {
		point_2d new_point;
		new_point.x = random_int(0, x_km) + SQR_DIM/2;
		new_point.y = random_int(0, y_km) + SQR_DIM/2;

		if(!contains(hosp_points, i, &new_point)) {
			hosp_points[i].x = new_point.x;
			hosp_points[i].y = new_point.y;
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
	if((fabs(a->x - b->x) < DOUBLE_TOL) && (fabs(a->y - b->y) < DOUBLE_TOL))
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