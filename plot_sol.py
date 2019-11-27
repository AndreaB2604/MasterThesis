import sys
import numpy as np
import matplotlib.pyplot as plt
import toyplot # pip install toyploy
import networkx as nx

import toyplot.pdf

def init_nodes(file, p_coord):
	line = file.readline()
	while(line.strip() != "NODE_COORD_SECTION"):
		line = file.readline()
	for i in range(p_coord.shape[0]):
		line = file.readline()
		chunks = line.split(" ")
		x = float(chunks[1])
		y = float(chunks[2])
		p_coord[i] = [x, y]


def init_hospital(file, h_coord):
	line = file.readline()
	while(line.strip() != "HOSPITALS_COORD_SECTION"):
		line = file.readline()
	for i in range(h_coord.shape[0]):
		line = file.readline()
		chunks = line.split(" ")
		x = float(chunks[1])
		y = float(chunks[2])
		h_coord[i] = [x, y]

def init_edges(file, edges, weights, h_weight):
	line = file.readline()
	while(line.strip() != "SOLUTION"):
		line = file.readline()
	for i in range(h_weight.shape[0]):
		line = file.readline()
		chunks = line.split(" ")
		h_weight[i] = chunks[2]

	while(len(line) != 0):
		line = file.readline()
		if line.rstrip():
			chunks = line.split(" ")
			weight = float(chunks[2])
			tmp = chunks[0].split("_")
			w_edge = [int(tmp[1]), int(tmp[2])]
			edges.append(w_edge)
			weights.append(weight)
	np_edges = np.array(edges)
	#print(np_edges)
	np_weights = np.array(weights)
	#print(np_weights)

def plot_graph(p_coord, edges, weights, h_coord, h_weight):

	G = nx.Graph()

	for i in range(p_coord.shape[0]):
		pos = (p_coord[i][0], p_coord[i][1])
		G.add_node(i+1+h_coord.shape[0], pos=pos)

	for i in range(h_coord.shape[0]):
		pos = (h_coord[i][0], h_coord[i][1])
		G.add_node(i+1, pos=pos, color='red')

	edges[:,1] = edges[:,1] + h_coord.shape[0]
	weighted_edges = np.transpose(np.vstack((edges[:,0], edges[:,1], np.around(weights, decimals=2))))

	print(weighted_edges)
	G.add_weighted_edges_from(weighted_edges)

	color_map = []
	for node in G:
		if node <= h_coord.shape[0]:
			color_map.append('blue')
		else: color_map.append('green')

	pos = nx.get_node_attributes(G,'pos')    

	#nx.draw(G, pos, node_color = color_map, node_size=60)
	nx.draw_networkx_nodes(G, pos, node_color=color_map, node_size=60)
	nx.draw_networkx_edges(G, pos)

	labels = nx.get_edge_attributes(G,'weight')
	nx.draw_networkx_edge_labels(G, pos, edge_labels=labels, font_size=4)

	px = p_coord[:,0]
	py = p_coord[:,1] 
	x_unique = np.unique(px)
	y_unique = np.unique(py)

	#plt.figure(figsize=(1, 1))
	plt.xticks(np.arange(0, np.ceil(np.amax(px)), x_unique[1]-x_unique[0]))
	plt.yticks(np.arange(0, np.ceil(np.amax(py)), y_unique[1]-y_unique[0]))

	plt.axis('on')
	plt.grid(True)
	plt.gca().set_aspect('equal', adjustable='box')
	plt.show()

	#plt.savefig("nodes.pdf", format='pdf', bbox_inches='tight')


def main(toplot):
	with open(toplot,"r") as file:
		line = file.readline()
		while (not(line.startswith("DIMENSION"))):
			line = file.readline()
		npoints = int(line.split(" ")[2])
		
		while (not(line.startswith("NUMBER_HOSPITAL"))):
			line = file.readline()
		nhosp = int(line.split(" ")[2])

		p_coord = np.zeros(shape=(npoints, 2))

		h_coord = np.zeros(shape=(nhosp, 2))
		h_weight = np.zeros(nhosp)

		edges = []
		weights = []

		init_nodes(file, p_coord)
		init_hospital(file, h_coord)
		init_edges(file, edges, weights, h_weight)
		plot_graph(p_coord, np.array(edges), np.array(weights), h_coord, h_weight)
	
if __name__ == '__main__':
	main(sys.argv[1])
