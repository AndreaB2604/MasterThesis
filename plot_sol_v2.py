import sys
import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.cm as mplcm
import matplotlib.colors as colors
import networkx as nx

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
		if not(line):
			return False
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
	return True

def plot_graph(p_coord, edges, weights, h_coord, h_weight, flag):

	fig, ax = plt.subplots()

	npoints = p_coord.shape[0]
	nhosp = h_coord.shape[0]
	G = nx.Graph()

	for i in range(p_coord.shape[0]):
		pos = (p_coord[i][0], p_coord[i][1])
		G.add_node(i+1+nhosp, pos=pos)

	for i in range(nhosp):
		pos = (h_coord[i][0], h_coord[i][1])
		G.add_node(i+1, pos=pos)

	node_color_map = np.zeros(nhosp+npoints)
	cmap = mplcm.get_cmap("Reds")

	if flag:
		# compute the weights of the nodes to initialize their color map
		weighted_edges = np.transpose(np.vstack((edges[:,1], edges[:,0], np.around(weights, decimals=2))))

		for i in range(weighted_edges.shape[0]):
			idx = int(weighted_edges[i][0]) - 1
			value = weighted_edges[i][2]
			node_color_map[idx] = node_color_map[idx] + value
		
		#print(node_color_map[:npoints])

		edges[:,1] = edges[:,1] + nhosp

		#print(weighted_edges)

		G.add_edges_from(edges)

		#edge_color_map = G.edges()
		#edge_color_map = (np.array(edge_color_map)[:,1]/(nhosp+1)).astype(float)
		print("Number of edges = ", edges.size)
	else:
		for i in range(nhosp):
			node_color_map[npoints+i] = 1

	#print(G.nodes())
	#node_color_map = np.array(node_color_map).astype(float)
	
	pos = nx.get_node_attributes(G, 'pos')
	node_size = math.floor(npoints/4)
	nodes = nx.draw_networkx_nodes(G, pos, node_size=node_size, node_color=node_color_map, cmap=cmap, vmin=math.floor(min(node_color_map)), vmax=math.ceil(max(node_color_map)))
	
	if flag:
		edges = nx.draw_networkx_edges(G, pos, edge_color="grey")

	px = p_coord[:,0]
	py = p_coord[:,1] 
	x_unique = np.unique(px)
	y_unique = np.unique(py)

	plt.xticks(np.arange(0, np.ceil(np.amax(px)), x_unique[1]-x_unique[0]))
	plt.yticks(np.arange(0, np.ceil(np.amax(py)), y_unique[1]-y_unique[0]))

	ax.tick_params(left=True, bottom=True, labelleft=True, labelbottom=True)

	if flag:
		norm = plt.Normalize(vmin=math.floor(min(node_color_map[:npoints])), vmax=math.ceil(max(node_color_map[:npoints])))
		sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
		sm.set_array([])
		plt.colorbar(sm)
	
	plt.grid(True)
	ax.set_axisbelow(True)
	plt.gca().set_aspect('equal', adjustable='box')
	plt.savefig("solution_flow.pdf", format='pdf', bbox_inches='tight')
	plt.show()


def main(toplot):
	with open(toplot, "r") as file:
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
		flag = init_edges(file, edges, weights, h_weight)

	plot_graph(p_coord, np.array(edges), np.array(weights), h_coord, h_weight, flag)
	
if __name__ == '__main__':
	main(sys.argv[1])
