import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as mplcm
import matplotlib.colors as colors
import networkx as nx
from matplotlib.lines import Line2D

def init_nodes(file, p_coord):
	line = file.readline()
	while(line.strip() != "NODE_COORD_SECTION"):
		line = file.readline()
	for i in range(p_coord.shape[0]):
		line = file.readline()
		chunks = line.split(" ")
		x = float(chunks[1])
		y = float(chunks[2])
		r = float(chunks[3])
		if(r > 0):
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

def plot_graph(p_coord, edges, weights, h_coord, h_weight, flag, background_img=None):

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

		edge_color_map = G.edges()
		edge_color_map = (np.array(edge_color_map)[:,1]/(nhosp+1)).astype(float)
		print("Number of edges = ", edge_color_map.size)

	#color_map = [(i/(nhosp+1)) if i <=nhosp else 1 for i in G.nodes()]
	#color_map = np.array(color_map).astype(float)

	color_map = ["yellow" if i <=nhosp else "blue" for i in G.nodes()]
		

	px = p_coord[:,0]
	py = p_coord[:,1] 
	x_unique = np.unique(px)
	y_unique = np.unique(py)

	x_edge_dim = x_unique[1]-x_unique[0]
	y_edge_dim = y_unique[1]-y_unique[0]
	
	if background_img != None:
		img = plt.imread(background_img)
		extent = [-y_edge_dim/2, img.shape[1]-y_edge_dim/2, x_edge_dim/2, img.shape[0]+x_edge_dim/2]
		#ax.imshow(img, extent=[-0.5, img.shape[1]-0.5, 0.5, img.shape[0]+0.5])
		ax.imshow(img, extent=extent)

	pos = nx.get_node_attributes(G, 'pos')
	cmap = mplcm.get_cmap("rainbow")
	nx.draw_networkx_nodes(G, pos, node_size=20, node_color=color_map, cmap=cmap, vmin=0.0, vmax=1.0)
	
	if flag:
		nx.draw_networkx_edges(G, pos, edge_color=edge_color_map, edge_cmap=cmap, edge_vmin=0.0, edge_vmax=1.0)
		labels = nx.get_edge_attributes(G,'weight')
		round_labels = {}
		for k, v in labels.items():
			round_labels[k] = round(v, 2)
		weights_bbox = dict(boxstyle="round, pad=0.3", fc="cyan", lw=0.1)
		nx.draw_networkx_edge_labels(G, pos, edge_labels=round_labels, font_size=4, label_pos=0.8,  bbox=weights_bbox)

	
	start_grid_x = np.amin(px) - x_edge_dim/2
	start_grid_y = np.amin(py) - y_edge_dim/2

	end_grid_x = np.amax(px) + x_edge_dim/2
	end_grid_y = np.amax(py) + y_edge_dim/2
	

	plt.xticks(np.arange(start_grid_x, end_grid_x, x_edge_dim))
	plt.yticks(np.arange(start_grid_y, end_grid_y, y_edge_dim))

	ax.tick_params(left=True, bottom=True, labelleft=True, labelbottom=True)
	
	legend_elements = [Line2D([0], [0], marker='o', color='w', label='Demand points', markerfacecolor='blue', markersize=7),
						Line2D([0], [0], marker='o', color='w', label='Service points', markerfacecolor='yellow', markersize=7)]

	# Put a legend outside the plot
	ax.legend(handles=legend_elements, loc='upper left', bbox_to_anchor=(1.04, 1))

	plt.grid(True)
	ax.set_axisbelow(True)
	plt.gca().set_aspect('equal', adjustable='box')
	plt.savefig("solution_weighted_flow.pdf", format='pdf', bbox_inches='tight')
	plt.show()


def main(toplot, background_img=None):
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

	plot_graph(p_coord, np.array(edges), np.array(weights), h_coord, h_weight, flag, background_img)
	
if __name__ == '__main__':
	if len(sys.argv) == 2:
		sys.argv.append(None)
	main(sys.argv[1], sys.argv[2])
