import sys
import re
import numpy as np
from img_extract_request import isNumber, imgExtractRequest

def writeInstNodes(file, complete_grid_dict, nhosp, max_dist):
	with open(file, "w") as new_file:
		new_file.write("NAME : MINIMUM_AMBULANCES\n")
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

def writeHospNodes(new_path, hcoord, px_per_km):
	with open(new_path, "a") as w_file:
		w_file.write("\nHOSPITALS_COORD_SECTION\n")
		i = 0
		for x, y, c in hcoord:
			tmpx = f"{x:.6f}"
			tmpy = f"{y:.6f}"
			w_file.write(str(i+1) + " " + tmpx + " " + tmpy + " " + str(int(c)) + "\n")
			i += 1


def imgInstGenerator(inst_path, new_path):
	with open(inst_path, "r") as file:
		img_path, csv_file, d_node = None, None, None
		mean, px_per_km, population, edge_dim, max_req, nhosp, max_dist = 0, 0, 0, 0, 0, 0, 0

		request_generation_dict = {	"AGENAS_FORMULA": "_agenas", 
									"MAX_REQUESTS_DISTRIBUTION" : "_request"}

		# parse the headers
		line = file.readline()
		while line:
			line = line.replace("\n", "")
			if line.startswith("IMG_SRC"):
				img_path = line.split(" ")[2]
			elif line.startswith("POP_DISTRIBUTION_CSV"):
				csv_file = line.split(" ")[2]
			elif line.startswith("IMG_CENTER"):
				tmp = re.split('[(), ]', line)
				mean = tuple([float(x) for x in tmp if isNumber(x)])
			elif line.startswith("IMG_PX_PER_KM"):
				px_per_km = int(line.split(" ")[2])
			elif line.startswith("SQUARE_EDGE_DIM"):
				edge_dim = float(line.split(" ")[2])
			elif line.startswith("POPULATION"):
				population = int(line.split(" ")[2])
			elif line.startswith("DEMAND_GENERATION_TYPE"):
				d_node = line.split(" ")[2]
			elif line.startswith("MAX_REQUESTS"):
				max_req = float(line.split(" ")[2])
			elif line.startswith("NUMBER_HOSPITALS"):
				nhosp = int(line.split(" ")[2])
			elif line.startswith("MAX_DISTANCE"):
				max_dist = int(line.split(" ")[2])
			line = file.readline()

		if(nhosp <= 0):
			print("ERROR: NUMBER_HOSPITALS field must be greater than zero in the instance file")
			sys.exit(1)
		if(max_dist <= 0):
			print("ERROR: MAX_DISTANCE field must be greater than zero in the instance file")
			sys.exit(1)		

		if(not(d_node in request_generation_dict.keys() or d_node == "ALL")):
			print("ERROR: DEMAND_GENERATION_TYPE not found or wrong: it must be AGENAS_FORMULA or MAX_REQUESTS_DISTRIBUTION or ALL")
			sys.exit(1)

		# move the cursor to the beginning of the file to read the hospital coordinates
		hcoord = []
		file.seek(0)
		line = file.readline()
		while line:
			if line.startswith("HOSPITALS_COORD_SECTION"):
				for i in range(nhosp):
					line = file.readline()
					chunks = line.split(" ")
					x = np.floor(float(chunks[1]) / px_per_km)
					y = np.floor(float(chunks[2]) / px_per_km)
					c = float(chunks[3])
					hcoord.append([x, y, c])
				line = False
			else:
				line = file.readline()

		grid_dict = imgExtractRequest(img_path, px_per_km, population, mean=mean, csv_file=csv_file, hosp_coord=hcoord, max_dist=max_dist, plot=True)
		
		if(grid_dict == None):
			return
			
		grid = list(grid_dict.keys())

		np_grid = np.array(grid)
		xpoints = np.amax(np_grid[:,0]) + 1
		ypoints = np.amax(np_grid[:,1]) + 1

		file_list = []
		if(d_node == "ALL"):
			file_list = [elem for elem in request_generation_dict.values()]
		else:
			file_list.append(request_generation_dict[d_node])
		
		for elem in file_list:
			output_file = None
			if(len(file_list) == 1):
				output_file = new_path
			else:
				output_file = new_path.rsplit(".", 1)[0] + elem + "." + new_path.rsplit(".", 1)[1]
		
			complete_grid_dict = {}

			for y in range(ypoints):
				for x in range(xpoints):
					if (x, y) in grid_dict:
						if(elem == "_agenas"):
							complete_grid_dict[(x, y)] = grid_dict[(x, y)] / 120000 + edge_dim**2 / 700
						else:
							complete_grid_dict[(x, y)] = grid_dict[(x, y)] * max_req / population
					else:
						complete_grid_dict[(x, y)] = 0
			'''
			s = 0
			for v in complete_grid_dict.values():
				s = s + v
			print("s =", s)
			'''
			
			writeInstNodes(output_file, complete_grid_dict, nhosp, max_dist)
			writeHospNodes(output_file, hcoord, px_per_km)



if __name__ == '__main__':
	imgInstGenerator(sys.argv[1], sys.argv[2])