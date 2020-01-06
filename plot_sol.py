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

	nhosp = h_coord.shape[0]
	G = nx.Graph()

	for i in range(p_coord.shape[0]):
		pos = (p_coord[i][0], p_coord[i][1])
		G.add_node(i+1+nhosp, pos=pos)

	for i in range(nhosp):
		pos = (h_coord[i][0], h_coord[i][1])
		G.add_node(i+1, pos=pos)

	if flag:
		edges[:,1] = edges[:,1] + nhosp

		weighted_edges = np.transpose(np.vstack((edges[:,0], edges[:,1], weights)))
		G.add_weighted_edges_from(weighted_edges)

		# create the edge color map based on the edge order of the graph
		edge_color_map = []	
		for (u, v) in G.edges():
			w_u_v = list(G.get_edge_data(u, v).values())[0]
			edge_color_map.append(w_u_v)

	print(max(edge_color_map), min(edge_color_map))
	node_color_map = ["orange" if i <=nhosp else "blue" for i in G.nodes()]
	
	pos = nx.get_node_attributes(G, 'pos')
	node_size = 1/p_coord.shape[0]
	nodes = nx.draw_networkx_nodes(G, pos, node_size=node_size, node_color=node_color_map, cmap=mplcm.get_cmap("Blues"), vmin=0.0, vmax=1.0)
	
	if flag:
		edges = nx.draw_networkx_edges(G, pos, edge_color=edge_color_map, edge_cmap=mplcm.get_cmap("Blues"), edge_vmin=min(weights), edge_vmax=max(weights))

	px = p_coord[:,0]
	py = p_coord[:,1] 
	x_unique = np.unique(px)
	y_unique = np.unique(py)
	
	x_edge_dim = x_unique[1]-x_unique[0]
	y_edge_dim = y_unique[1]-y_unique[0]
	
	start_grid_x = np.amin(px) - x_edge_dim/2
	start_grid_y = np.amin(py) - y_edge_dim/2

	end_grid_x = np.amax(px) + x_edge_dim/2
	end_grid_y = np.amax(py) + y_edge_dim/2
	
	plt.xticks(np.arange(start_grid_x, end_grid_x, x_edge_dim))
	plt.yticks(np.arange(start_grid_y, end_grid_y, y_edge_dim))

	ax.tick_params(left=True, bottom=True, labelleft=True, labelbottom=True)

	if flag:
		norm = plt.Normalize(vmin=min(weights), vmax=max(weights))
		sm = plt.cm.ScalarMappable(cmap=mplcm.get_cmap("Blues"), norm=norm)
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
