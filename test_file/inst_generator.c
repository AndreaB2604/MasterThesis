#include "grid.h"

/*   MAIN   */
int main(int argc, char **argv) {

	const int X_KM = 10, Y_KM = 10;
	const double SQR_DIM = 1;
	const int HOSP_NUM = 4;
	const int MAX_DISTANCE = 7;
	
	int x_lim = (int)(X_KM/SQR_DIM + 0.5);
	int y_lim = (int)(Y_KM/SQR_DIM + 0.5);

	char *name = "SQUARE_GRID";
	char *dist_type = "EUC_2D";
	char *file_name = "grid100.flow";
	FILE *file = fopen(file_name, "w+");

	fprintf(file, "NAME : %s\n", name);
	fprintf(file, "DIMENSION : %d\n", (X_KM*Y_KM));
	fprintf(file, "NUMBER_HOSPITALS : %d\n", HOSP_NUM);
	fprintf(file, "MAX_DISTANCE : %d\n", MAX_DISTANCE);
	fprintf(file, "EDGE_WEIGHT_TYPE : %s\n", dist_type);
	fprintf(file, "COMMENT : FOR POINTS (n_point x_coord y_coord request)\n");
	fprintf(file, "COMMENT : FOR HOSPITALS (n_point x_coord y_coord max_capacity)\n");
	fprintf(file, "NODE_COORD_SECTION\n");

	for(int j = 0; j < y_lim; j++)
		for(int i = 0; i < x_lim; i++) {
			double x_coord = SQR_DIM * (i + 0.5);
			double y_coord = SQR_DIM * (j + 0.5);
			fprintf(file, "%d %f %f %f\n", (j*Y_KM+i+1), x_coord, y_coord, random_double(0, 10));
		}

	fprintf(file, "\nHOSPITALS_COORD_SECTION\n");
	point_2d *hosp_points = (point_2d *) calloc(HOSP_NUM, sizeof(point_2d));
	random_hospital_generator(hosp_points, HOSP_NUM, x_lim, y_lim, SQR_DIM);

	for(int i = 0; i < HOSP_NUM; i++) {
		fprintf(file, "%d %f %f %d\n", (i+1), hosp_points[i].x, hosp_points[i].y, ((int) random_int(5, 10)));
	}

	free(hosp_points);
	fclose(file);

}

void random_hospital_generator(point_2d *hosp_points, int HOSP_NUM, int x_lim, int y_lim, double SQR_DIM) {

	int i = 0;
	while(i < HOSP_NUM) {
		point_2d new_point;
		new_point.x = random_int(0, x_lim) + SQR_DIM/2;
		new_point.y = random_int(0, y_lim) + SQR_DIM/2;

		if(!contains(hosp_points, i, &new_point)) {
			hosp_points[i].x = new_point.x;
			hosp_points[i].y = new_point.y;
			i++;
		}	
	}
}

int contains(point_2d *list, int list_size, point_2d *a) {
	for(int i = 0; i < list_size; i++) {
		if(equals(&(list[i]), a)) {
			printf("found equal point in contains\n");
			return 1;
		}
	}
	return 0;
}

int equals(point_2d *a, point_2d *b) {
	if((fabs(a->x - b->x) < DOUBLE_TOL) && (fabs(a->y - b->y) < DOUBLE_TOL)) {
		printf("found equal point\n");
		return 1;
	}
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