import sys
import cv2
import random
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from collections import Counter
from threading import Thread, RLock

threadLock = RLock()

class GaussPopThread(Thread):
	def __init__(self, grid_dict, npoints, mean, cov):
		Thread.__init__(self)
		self.grid_dict = grid_dict
		self.npoints = npoints
		self.mean = mean
		self.cov = cov
	def run(self):
		copy_dict = self.grid_dict.copy()
		for i in range(self.npoints):
			point = intGaussPoint2D(self.mean, self.cov, self.grid_dict)
			copy_dict[point] += 1
		
		threadLock.acquire()
		try:
			for point, value in copy_dict.items():
				self.grid_dict[point] += value
		finally:
			threadLock.release()

'''
def addItemsToDict(grid_dict, point):
	threadLock.acquire()
	try:
		grid_dict[point] += 1
	finally:
		threadLock.release()
'''
def intGaussPoint2D(mean, cov, grid):
	point = tuple(np.round(np.random.multivariate_normal(mean, cov)).astype(int))
	while(not(point in grid)):
		point = tuple(np.round(np.random.multivariate_normal(mean, cov)).astype(int))
	return point

def truncGauss(mu, sigma, bottom, top):
	rand = int(random.gauss(mu, sigma))
	while(rand < bottom or rand > top):
		rand = int(random.gauss(mu, sigma))
	return rand

def interpolateImg(img_path, px_per_km):
	#np.set_printoptions(threshold = sys.maxsize)

	image = cv2.imread(img_path)
	lowerbound = np.array([0, 0, 0])  # BGR-code of the lowest black
	upperbound = np.array([30, 30, 30])   # BGR-code of the highest black 
	mask = cv2.inRange(image, lowerbound, upperbound)  
	#get all non zero values
	coord = cv2.findNonZero(mask)

	points = coord[:,0]
	grid = np.floor(points / px_per_km).astype(int)

	threshold = 1 / 2 * px_per_km**2
	pair_counter = Counter([tuple(pair) for pair in grid])
	interpolated_grid = sorted([key for key, value in pair_counter.items() if value >= threshold])
	interpolated_grid = np.array(interpolated_grid)
	new_width = max(interpolated_grid[:,0]) + 1
	new_height = max(interpolated_grid[:,1]) + 1

	img = np.ones((new_height, new_width, 3), np.uint8)*255

	for x, y in interpolated_grid:
		color1 = (0, 170, 255)
		color2 = (0, 0, 255)
		if((x + y) % 2 == 0):
			img[y, x] = color1
		else:
			img[y, x] = color2

	interpolated_grid[:,1] = abs(interpolated_grid[:,1] - new_height)
	img_dest_path = img_path.rsplit("/", 1)[0] + "/interpolated_img.png"
	cv2.imwrite(img_dest_path, img)
	return interpolated_grid

def gaussPopulation(grid, population, mean, cov):
	grid_dict = {}

	# put at least one person in every square in the grid if the population is greater than the grid size
	init_value = 1 if population > grid.shape[0] else 0
	# adjust the number of point to generate based on how many are already present
	npoints = population - grid.shape[0] if init_value == 1 else population
	
	for x, y in grid:
		grid_dict[(x, y)] = init_value

	nprocessor = 1
	grid_range = np.linspace(start=0, stop=npoints, num=nprocessor+1, dtype=int)
	thread_list = []
	print("Generating the random population:")
	for i in range(nprocessor):
		t = GaussPopThread(grid_dict, grid_range[i+1]-grid_range[i], mean, cov)
		print("Starting thread", i)
		t.start()
		thread_list.append(t)

	for t in thread_list:
		t.join()

	return grid_dict

def plotPopulation(grid_dict, img_path=None):
	fig = plt.figure()
	ax = Axes3D(fig)

	sequence_containing_x_vals = []
	sequence_containing_y_vals = []

	for x, y in grid_dict:
		sequence_containing_x_vals.append(x)
		sequence_containing_y_vals.append(y)

	sequence_containing_z_vals = list(grid_dict.values())

	ax.scatter(sequence_containing_x_vals, sequence_containing_y_vals, sequence_containing_z_vals)
	plt.gca().set_aspect('equal', adjustable='box')
	
	dest_folder = img_path.rsplit("/", 1)[0] + "/" if img_path != None else ""
	
	ax.view_init(20, 290)
	fig.suptitle('Population distribution\nelev=20, azimut=290')
	plt.savefig(dest_folder + "pop_distribution_20_290.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(90, 270)
	fig.suptitle('Population distribution\nelev=90, azimut=270')
	plt.savefig(dest_folder + "pop_distribution_90_270.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(0, 0)
	fig.suptitle('Population distribution\nelev=0, azimut=0')
	plt.savefig(dest_folder + "pop_distribution_0_0.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(0, 90)
	fig.suptitle('Population distribution\nelev=0, azimut=90')
	plt.savefig(dest_folder + "pop_distribution_0_90.pdf", format='pdf', bbox_inches='tight')
	

def imgExtractRequestMT(img_path, mean, px_per_km, population, plot=False):
	grid = interpolateImg(img_path, px_per_km)
	
	# normalize the mean on px_per_km
	mean = np.floor(np.array(mean) / px_per_km).astype(int)
	mean = tuple(mean)

	cov_row1 = np.array([max(grid[:,0]), 0])
	cov_row2 = np.array([0, max(grid[:,1])])	
	cov = np.matrix([cov_row1, cov_row2])*5

	grid_dict = gaussPopulation(grid, population, mean, cov)
	print(grid_dict)
		
	s = 0
	for v in grid_dict.values():
		s += v

	print("Total number of people", s)
	
	if plot:
		plotPopulation(grid_dict, img_path)

	return grid_dict


if __name__ == '__main__':
	img_path = sys.argv[1]
	mu = (39, 33)
	px_per_km = 11
	population = int(sys.argv[2])
	imgExtractRequest(img_path, mu, px_per_km, population)