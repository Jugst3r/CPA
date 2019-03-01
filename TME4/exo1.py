#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Fri Mar  1 09:16:16 2019

@author: 3535008
"""

import networkx as nx
import matplotlib.pyplot as plt
from random import random, shuffle, seed

def generate_random_graph (f, nb_cluster, cluster_size, p, q):
    nods = range (nb_cluster*cluster_size)
    #could iterate over probabilities and pick random nods
    edges = []
    for n in nods:
        for n2 in nods:
            #same cluster
            if n//100 == n2//100:
                if(random()<p):
                    edges.append([n, n2])
                    f.write(str(n) + "\t" + str(n2) + "\n")
            else:
                if(random()<q):
                    edges.append([n, n2])
                    f.write(str(n) + "\t" + str(n2) + "\n")
    return edges
            

def draw_graph(graph, labels=None, graph_layout='spectral',
               node_size=0.1, node_color='blue', node_alpha=0.1,
               node_text_size=0,
               edge_color='blue', edge_alpha=0.3, edge_tickness=0.1,
               edge_text_pos=0.3,
               text_font='sans-serif'):

    # create networkx graph
    G=nx.Graph()

    # add edges
    for edge in graph:
        G.add_edge(edge[0], edge[1])

    # these are different layouts for the network you may try
    # shell seems to work best
    if graph_layout == 'spring':
        graph_pos=nx.spring_layout(G)
    elif graph_layout == 'spectral':
        graph_pos=nx.spectral_layout(G)
    elif graph_layout == 'random':
        graph_pos=nx.random_layout(G)
    else:
        graph_pos=nx.shell_layout(G)

    # draw graph
    nx.draw_networkx_nodes(G,graph_pos,node_size=node_size, 
                           alpha=node_alpha, node_color=node_color)
    nx.draw_networkx_edges(G,graph_pos,width=edge_tickness,
                           alpha=edge_alpha,edge_color=edge_color)
    if labels is None:
        labels = range(len(graph))


    # show graph
    plt.show()

    

seed(1)
f = open("graph00.txt", "w")
G = generate_random_graph(f, 4, 100, 0.5, 0.001)
f.close()
print(G)
draw_graph(G)