import sys
import cv2
import random
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from collections import Counter

def trunc_gauss(mu, sigma, bottom, top):
	rand = int(random.gauss(mu, sigma))
	while(rand < bottom or rand > top):
		rand = int(random.gauss(mu, sigma))
	return rand

def interpolateImg(img_path, px_per_km):
	np.set_printoptions(threshold = sys.maxsize)

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
	new_width = max(interpolated_grid[:,0])+1
	new_height = max(interpolated_grid[:,1])+1

	img = np.ones((new_height, new_width, 3), np.uint8)*255

	for x, y in interpolated_grid:
		color1 = (85, 85, 85)
		color2 = (170, 170, 170)
		if((x+y) % 2 == 0):
			img[y, x] = color1
		else:
			img[y, x] = color2

	interpolated_grid[:,1] = abs(interpolated_grid[:,1] - new_height)
	cv2.imwrite('interpolated_img.png', img)
	return interpolated_grid


def gaussPopulation(grid, population, mu, sigma):
	min_x = min(grid[:,0])
	max_x = max(grid[:,0])
	min_y = min(grid[:,1])
	max_y = max(grid[:,1])

	grid_dict = {}

	# put at least one person in every square in the grid if the population is greater than the grid size
	init_value = 1 if population > grid.shape[0] else 0
	# adjust the number of point to generate based on how many are already present
	npoints = population-grid.shape[0] if init_value == 1 else population
	
	for x, y in grid:
		grid_dict[(x, y)] = init_value

	for i in range(npoints):
		point = (trunc_gauss(mu[0], sigma[0], min_x, max_x), trunc_gauss(mu[1], sigma[1], min_y, max_y))
		while(not(point in grid_dict)):
			point = (trunc_gauss(mu[0], sigma[0], min_x, max_x), trunc_gauss(mu[1], sigma[1], min_y, max_y))
		grid_dict[point] = grid_dict[point] + 1
	
	return grid_dict

def plotPopulation(grid_dict):
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
	
	ax.view_init(20, 200)
	fig.suptitle('Population distribution\nelev=20, azimut=200')
	plt.savefig("pop_distribution_20_200.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(90, 270)
	fig.suptitle('Population distribution\nelev=90, azimut=270')
	plt.savefig("pop_distribution_90_270.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(0, 0)
	fig.suptitle('Population distribution\nelev=0, azimut=0')
	plt.savefig("pop_distribution_0_0.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(0, 90)
	fig.suptitle('Population distribution\nelev=0, azimut=90')
	plt.savefig("pop_distribution_0_90.pdf", format='pdf', bbox_inches='tight')
	
	ax.view_init(0, 180)
	fig.suptitle('Population distribution\nelev=0, azimut=180')
	plt.savefig("pop_distribution_0_180.pdf", format='pdf', bbox_inches='tight')


def imgExtractRequest(img_path, mu, px_per_km, population):
	grid = interpolateImg(img_path, px_per_km)
	
	sigma = (max(grid[:,0])/2, max(grid[:,1])/2)
	
	grid_dict = gaussPopulation(grid, int(population), mu, sigma)
	plotPopulation(grid_dict)


if __name__ == '__main__':
	mu = (37, 33)
	px_per_km = 11
	imgExtractRequest(sys.argv[1], mu, px_per_km, sys.argv[2])