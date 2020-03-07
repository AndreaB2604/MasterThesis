import sys
import os
import random
import numpy as np

def demandPointsGenerator(x_points, y_points, sqr_dim):
	x_lim = x_points * sqr_dim
	y_lim = y_points * sqr_dim
	
	x_coord = np.arange(x_lim, step=sqr_dim, dtype=float)
	y_coord = np.arange(y_lim, step=sqr_dim, dtype=float)
	request = np.random.random_sample(x_points * y_points)

	demand_points = np.array([[x, y] for y in y_coord for x in x_coord])
	demand_points += sqr_dim / 2

	demand_points = np.vstack((demand_points[:,0], demand_points[:,1], request)).T

	return demand_points


def servicePointsRandomGenerator(demand_points, n_hosp):
	rng = np.random.default_rng()
	service_points = rng.choice(demand_points, n_hosp, replace=False)	

	return service_points


def exampleInstGenerator():
	x_points = 10
	y_points = 10
	sqr_dim = 1
	n_hosp = 4
	hosp_capacity = 1000
	max_dist = 7
	output_file_name = os.path.join(os.path.dirname(__file__), "grid.flow")

	demand_points = demandPointsGenerator(x_points, y_points, sqr_dim)
	service_points = servicePointsRandomGenerator(demand_points, n_hosp)

	with open(output_file_name, "w") as new_file:
		new_file.write("NAME : MINIMUM_AMBULANCES\n")
		new_file.write("DIMENSION : " + str(x_points * y_points) + "\n")
		new_file.write("NUMBER_HOSPITALS : " + str(n_hosp) + "\n")
		new_file.write("MAX_DISTANCE : " + str(max_dist) + "\n")
		new_file.write("EDGE_WEIGHT_TYPE : MOD_EUC_2D\n")
		new_file.write("COMMENT : FOR POINTS (n_point x_coord y_coord request)\n")
		new_file.write("COMMENT : FOR HOSPITALS (n_point x_coord y_coord max_capacity)\n")
		new_file.write("NODE_COORD_SECTION\n")

		i = 1
		for x, y, r in demand_points:
			x_str = f"{x:.6f}"
			y_str = f"{y:.6f}"
			r_str = f"{r:.6f}"
			new_file.write(str(i) + " " + x_str + " " + y_str + " " + r_str + "\n")
			i += 1

		i = 1
		new_file.write("\nHOSPITALS_COORD_SECTION\n")
		for x, y, r in service_points:
			x_str = f"{x:.6f}"
			y_str = f"{y:.6f}"
			new_file.write(str(i) + " " + x_str + " " + y_str + " " + str(hosp_capacity) + "\n")
			i += 1

	print(os.path.realpath(__file__))
	print(os.path.dirname(__file__))

if __name__ == '__main__':
	exampleInstGenerator()
