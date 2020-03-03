import sys
import cv2
import csv
import re
import random
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from collections import Counter
from create_circle import midPointCircleDraw

def isNumber(x):
	try:
		float(x)
		return True
	except ValueError:
		return False

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

def interpolateImg(img_path, px_per_km, hosp_coord=None, max_dist=None):
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

	small_img = np.ones((new_height, new_width, 3), np.uint8)*255
	img = np.ones((new_height*px_per_km, new_width*px_per_km, 3), np.uint8)*255

	for x, y in interpolated_grid:
		color1 = (0, 170, 255)
		color2 = (0, 0, 255)
		tmpx = x * px_per_km
		tmpy = y * px_per_km
		if((x + y) % 2 == 0):
			small_img[y, x] = color1
			img[tmpy:tmpy+px_per_km, tmpx:tmpx+px_per_km] = color1
		else:
			small_img[y, x] = color2
			img[tmpy:tmpy+px_per_km, tmpx:tmpx+px_per_km] = color2

	small_img_dest_path = img_path.rsplit("/", 1)[0] + "/interpolated_small_img.png"
	img_dest_path = img_path.rsplit("/", 1)[0] + "/interpolated_img.png"
	cv2.imwrite(small_img_dest_path, small_img)
	cv2.imwrite(img_dest_path, img)
	
	if((hosp_coord != None) and (max_dist != None)):
		color1 = (255, 0, 0)
		color2 = (255, 170, 170)
		i = 0
		for x, y, c in hosp_coord:
			h = new_height * px_per_km
			w = new_width * px_per_km
			tmpx = int(x * px_per_km)
			tmpy = int(y * px_per_km)
			img[h-tmpy:h-tmpy+px_per_km, tmpx:tmpx+px_per_km] = color1
			circle = midPointCircleDraw((tmpx, h-tmpy), max_dist*px_per_km)
			hrange, wrange = range(h), range(w)
			for xc, yc in circle:
				if(xc in wrange and yc in hrange):
					img[int(yc), int(xc)] = color2
				
		img_dest_path = img_path.rsplit("/", 1)[0] + "/interpolated_hosp_img.png"
		cv2.imwrite(img_dest_path, img)


	interpolated_grid[:,1] = abs(interpolated_grid[:,1] - new_height)
	return interpolated_grid


def gaussPopulation(grid, population, mean, cov):
	grid_dict = {}

	# put at least one person in every square in the grid if the population is greater than the grid size
	init_value = 1 if population > grid.shape[0] else 0
	# adjust the number of point to generate based on how many are already present
	npoints = population - grid.shape[0] if init_value == 1 else population
	
	for x, y in grid:
		grid_dict[(x, y)] = init_value

	print("Generating the random population:")
	fr = np.floor(population / 10)
	for i in range(npoints):
		point = intGaussPoint2D(mean, cov, grid_dict)
		grid_dict[point] += 1
		if i % fr == 0:
			percent = int(np.round(i * 100 / npoints))
			print("Generated " + str(i) + " points: " + str(percent) + "%")
	
	return grid_dict


def gaussPopulationCSV(grid, population, csv_path, cov, px_per_km):
	with open(csv_path, mode='r') as csv_file:
		# initialise the means and the probability distribution form the CSV
		csv_reader = csv.reader(csv_file)
		
		csv_pop = []
		csv_means = []
		for line in csv_reader:
			if(isNumber(line[0])):
				csv_pop.append(int(line[2]))
				tmp = re.split('[(), ]', line[3])
				mean = tuple([float(x) for x in tmp if isNumber(x)])
				csv_means.append(mean)

		#csv_pop = np.sqrt(csv_pop)
		csv_pop = np.array(csv_pop) / np.sum(csv_pop)
		csv_means = np.floor(np.array(csv_means) / px_per_km).astype(int)

		# initialise the grid dictionary
		grid_dict = {}

		# put at least one person in every square in the grid if the population is greater than the grid size
		init_value = 1 if population > grid.shape[0] else 0
		# adjust the number of point to generate based on how many are already present
		npoints = population - grid.shape[0] if init_value == 1 else population
		
		for x, y in grid:
			grid_dict[(x, y)] = init_value

		rng = np.random.default_rng()
		rand_means = rng.choice(csv_means, npoints, p=csv_pop)

		print("Generating the random population:")
		fr = np.floor(population / 10)
		i = population - npoints
		for mean in rand_means:
			point = intGaussPoint2D(mean, cov, grid_dict)
			grid_dict[point] += 1
			if i % fr == 0:
				percent = int(np.round(i * 100 / population))
				print("Generated " + str(i) + " points: " + str(percent) + "%")
			i += 1

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
	
	for i in range(0, 360, 90):
		title = "Population distribution\nelev=0, azimut=" + str(i)
		file_path = dest_folder + "pop_distribution_0_" + str(i) + ".pdf"
		ax.view_init(0, i)
		fig.suptitle(title)
		plt.savefig(file_path, format='pdf', bbox_inches='tight')


def imgExtractRequest(img_path, px_per_km, population, mean=None, csv_file=None, hosp_coord=None, max_dist=None, plot=False):
	print("Generating the interpolated images...")
	grid = interpolateImg(img_path, px_per_km, hosp_coord, max_dist)
	
	print("Interpolated images generated.")

	txt = input("Do you want to generate the gaussian distributed population? [y/n]\n")

	done = False
	while(not(done)):
		if(txt == "n"):
			return None
		elif(txt == "y"):
			done = True
		else:
			txt = input("Do you want to generate the gaussian distributed population? [y/n]\n")

	# define the covariance matrix rows
	cov_row1 = np.array([max(grid[:,0]), 0])
	cov_row2 = np.array([0, max(grid[:,1])])
	cov = np.matrix([cov_row1, cov_row2])

	if(csv_file == None):
		# normalize the mean on px_per_km
		mean = np.floor(np.array(mean) / px_per_km).astype(int)
		mean = tuple(mean)

		cov *= 5

		grid_dict = gaussPopulation(grid, population, mean, cov)
	else:
		grid_dict = gaussPopulationCSV(grid, population, csv_file, cov, px_per_km)

	'''
	s = 0
	for i in range(33, 45):
		for j in range(27, 40):
			s += grid_dict[(i, j)]

	print(s)
	'''

	if plot:
		plotPopulation(grid_dict, img_path)

	return grid_dict


if __name__ == '__main__':
	img_path = sys.argv[1]
	mu = (39, 33)
	px_per_km = 11
	population = int(sys.argv[2])
	imgExtractRequest(img_path, mu, px_per_km, population)