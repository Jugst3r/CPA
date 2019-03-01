#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Fri Mar  1 09:16:16 2019

@author: 3535008
"""

import networkx as nx
import matplotlib.pyplot as plt
from random import random, shuffle, seed
from collections import defaultdict

import colorsys

def get_N_HexCol(N=5):

    HSV_tuples = [(x*1.0/N, 0.5, 0.5) for x in range(N)]
    hex_out = []
    for rgb in HSV_tuples:
        rgb = map(lambda x: int(x*255),colorsys.hsv_to_rgb(*rgb))
        hex_out.append("#" + str("".join(map(lambda x: chr(x).encode("utf-8").hex(),rgb))))
    return hex_out

print(get_N_HexCol())

def generate_random_graph (f, nb_cluster, cluster_size, p, q):
    #could iterate over probabilities and pick random nods
    edges = []
    for n in range(nb_cluster*cluster_size):
        for n2 in range(n+1, nb_cluster*cluster_size):
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
            

def parse_label(f):
    d = defaultdict(list)
    for line in f:
        (nod, label) = map(int, line.split())
        d[label].append(nod)
        print(str(nod) + " " + str(label))
    return d
    

def draw_graph(graph, dic_label, labels=None, graph_layout='spectral',
               node_size=20, node_alpha=1,
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
    i=0
    colors = get_N_HexCol(len(dic_label))
    print (colors)
    for _,nods in dic_label.items():
        nx.draw_networkx_nodes(G,graph_pos,nodelist=nods,node_size=node_size, 
                           alpha=node_alpha, node_color=colors[i])
        i = i+1
    
    nx.draw_networkx_edges(G,graph_pos,width=edge_tickness,
                           alpha=edge_alpha,edge_color=edge_color)
    if labels is None:
        labels = range(len(graph))


    # show graph
    plt.show()

    
import os

seed(1)
f = open("graph00.txt", "w")
G = generate_random_graph(f, 4, 100, 0.3, 0.01)
print("over")
os.execv("./tme4", ["tme4", "graph00.txt", "output.txt"])
labels = open("output.txt", "r")
d=parse_label(labels)

f.close()
labels.close()
#print(G)
draw_graph(G, d)