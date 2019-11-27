import sys
import numpy as np
import matplotlib.pyplot as plt

def init_points(file, n, x, y):
	line = file.readline()
	while(line.strip() != "NODE_COORD_SECTION"):
		line = file.readline()
	for i in range(n):
		line = file.readline()
		chunks = line.split(" ")
		x[i] = float(chunks[1])
		y[i] = float(chunks[2])

def init_hospital(file, n, x, y):
	line = file.readline()
	while(line.strip() != "HOSPITALS_COORD_SECTION"):
		line = file.readline()
	for i in range(n):
		line = file.readline()
		chunks = line.split(" ")
		x[i] = float(chunks[1])
		y[i] = float(chunks[2])


def plot_grid(px, py, hx, hy):
	fig = plt.figure()

	x_unique = np.unique(px)
	y_unique = np.unique(py)

	#plt.figure(figsize=(1, 1))
	plt.xticks(np.arange(0, np.ceil(np.amax(px)), x_unique[1]-x_unique[0]))
	plt.yticks(np.arange(0, np.ceil(np.amax(py)), y_unique[1]-y_unique[0]))

	# draw the service points in blue
	plt.scatter(px, py, color='blue', label='Service points')
	# draw the hospitals points in yellow
	plt.scatter(hx, hy, color='yellow', label='Hospitals')
	print(hy[0:3])
	plt.grid(True, linewidth=0.25)

	# set the legend outside the plot
	plt.legend(bbox_to_anchor=(1.04, 1), loc='upper left')
	
	# scale correctly the axis
	plt.gca().set_aspect('equal', adjustable='box')
	plt.savefig('points_grid.pdf', format='pdf', bbox_inches='tight')


def main(toplot):
	file = open(toplot,"r")
	line = file.readline()
	while (not(line.startswith("DIMENSION"))):
		line = file.readline()
	npoints = int(line.split(" ")[2])
	
	while (not(line.startswith("NUMBER_HOSPITAL"))):
		line = file.readline()
	nhosp = int(line.split(" ")[2])

	p_xcoord = np.zeros(npoints)
	p_ycoord = np.zeros(npoints)

	h_xcoord = np.zeros(nhosp)
	h_ycoord = np.zeros(nhosp)

	init_points(file, npoints, p_xcoord, p_ycoord)
	init_hospital(file, nhosp, h_xcoord, h_ycoord)
	plot_grid(p_xcoord, p_ycoord, h_xcoord, h_ycoord)
			
	
if __name__ == '__main__':
	main(sys.argv[1])
