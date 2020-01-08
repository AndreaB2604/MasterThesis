import sys
import re
import numpy as np
from img_extract_request import imgExtractRequest

def isNumber(x):
	try:
		float(x)
		return True
	except ValueError:
		return False

def writeInstNodes(file, complete_grid_dict, nhosp, max_dist):
	with open(file, "w") as new_file:
		new_file.write("NAME : SQUARE_GRID\n")
		new_file.write("DIMENSION : " + str(len(complete_grid_dict)) + "\n")
		new_file.write("NUMBER_HOSPITALS : " + str(nhosp) + "\n")
		new_file.write("MAX_DISTANCE : " + str(max_dist) + "\n")
		new_file.write("EDGE_WEIGHT_TYPE : MOD_EUC_2D\n")
		new_file.write("COMMENT : FOR POINTS (n_point x_coord y_coord request)\n")
		new_file.write("COMMENT : FOR HOSPITALS (n_point x_coord y_coord max_capacity)\n")
		new_file.write("NODE_COORD_SECTION\n")

		k = 1
		for key, value in complete_grid_dict.items():
			xcoord = f"{key[0]:.6f}"
			ycoord = f"{key[1]:.6f}"
			request = f"{value:.15f}"
			new_file.write(str(k) + " " + xcoord + " " + ycoord + " " + request + "\n")
			k += 1

def writeHospNodes(inst_path, new_path, nhosp, px_per_km):
	with open(inst_path, "r") as r_file:
		with open(new_path, "a") as w_file:
			w_file.write("\nHOSPITALS_COORD_SECTION\n")
			line = r_file.readline()
			while line:
				if line.startswith("HOSPITALS_COORD_SECTION"):
					for i in range(nhosp):
						line = r_file.readline()
						chunks = line.split(" ")
						x = np.floor(float(chunks[1]) / px_per_km)
						x = f"{x:.6f}"
						y = np.floor(float(chunks[2]) / px_per_km)
						y = f"{y:.6f}"
						c = chunks[3]
						w_file.write(str(i+1) + " " + x + " " + y + " " + c)
					line = False
				else:
					line = r_file.readline()



def imgInstGenerator(inst_path, new_path):
	with open(inst_path, "r") as file:
		img_path = ""
		mean, px_per_km, population, max_amb, nhosp, max_dist = 0, 0, 0, 0, 0, 0

		# parse the headers
		line = file.readline()
		while line:
			line = line.replace("\n", "")
			if line.startswith("IMG_SRC"):
				img_path = line.split(" ")[2]
			elif line.startswith("IMG_CENTER"):
				tmp = re.split('[(), ]', line)
				mean = tuple([float(x) for x in tmp if isNumber(x)])
			elif line.startswith("IMG_PX_PER_KM"):
				px_per_km = int(line.split(" ")[2])
			elif line.startswith("POPULATION"):
				population = int(line.split(" ")[2])
			elif line.startswith("MAX_AMBULANCES"):
				max_amb = float(line.split(" ")[2])
			elif line.startswith("NUMBER_HOSPITALS"):
				nhosp = int(line.split(" ")[2])
			elif line.startswith("MAX_DISTANCE"):
				max_dist = int(line.split(" ")[2])
			line = file.readline()

		grid_dict = imgExtractRequest(img_path, mean, px_per_km, population, plot=True)
		grid = list(grid_dict.keys())

		np_grid = np.array(grid)
		xpoints = np.amax(np_grid[:,0]) + 1
		ypoints = np.amax(np_grid[:,1]) + 1

		complete_grid_dict = {}

		for y in range(ypoints):
			for x in range(xpoints):
				if (x, y) in grid_dict:
					complete_grid_dict[(x, y)] = max_amb * grid_dict[(x, y)] / population
				else:
					complete_grid_dict[(x, y)] = 0
		
		writeInstNodes(new_path, complete_grid_dict, nhosp, max_dist)
		writeHospNodes(inst_path, new_path, nhosp, px_per_km)



if __name__ == '__main__':
	imgInstGenerator(sys.argv[1], sys.argv[2])