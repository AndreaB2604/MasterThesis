#include "grid.h"

/*   MAIN   */
int main(int argc, char **argv) {

	const int X_KM = 10, Y_KM = 10;
	const double SQR_DIM = 1.0;
	const int HOSP_NUM = 4;
	const int MAX_DISTANCE = 7;
	
	int x_lim = (int)(X_KM/SQR_DIM + 0.5);
	int y_lim = (int)(Y_KM/SQR_DIM + 0.5);
	point_2d *points_coord = (point_2d *) calloc(x_lim*y_lim, sizeof(point_2d));

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

	for(int j = 0, k = 0; j < y_lim; j++)
		for(int i = 0; i < x_lim; i++) {
			points_coord[k].x = SQR_DIM * (i + 0.5);
			points_coord[k].y = SQR_DIM * (j + 0.5);
			double request = random_double(0, 1);
			fprintf(file, "%d %f %f %f\n", k+1, points_coord[k].x, points_coord[k].y, request);
			k++;
		}


	fprintf(file, "\nHOSPITALS_COORD_SECTION\n");
	point_2d *hosp_points = (point_2d *) calloc(HOSP_NUM, sizeof(point_2d));
	random_hospital_generator(points_coord, x_lim*y_lim, hosp_points, HOSP_NUM);

	for(int i = 0; i < HOSP_NUM; i++) {
		int h_capacity = 1000;
		fprintf(file, "%d %f %f %d\n", (i+1), hosp_points[i].x, hosp_points[i].y, h_capacity);
	}

	free(hosp_points);
	free(points_coord);
	fclose(file);

}

void random_hospital_generator(point_2d *points_coord, long points_coord_size, point_2d *hosp_points, int HOSP_NUM) {
	int i = 0;
	long added_points[HOSP_NUM];
	
	while(i < HOSP_NUM) {
		long rand_p = random_int(0, points_coord_size);
		int contains = 0;

		for(int j = 0; j < i && !contains; j++) {
			if(rand_p == added_points[j]) {
				contains = 1;
			}
		}

		if(!contains) {
			hosp_points[i].x = points_coord[rand_p].x;
			hosp_points[i].y = points_coord[rand_p].y;
			added_points[i] = rand_p;
			i++;
		}	
	}
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