import re
import sys

def number_of_nodes_per_community(s):
    f = open(s,"r")
    d = dict()
    while True :
        r = f.readline()
        if not r:
            break
        match = re.search('([0-9]+)',r)
        i = int(match.group().rstrip("\n\r"))
        if i in d:
            d[i] = d[i]+1
        else:
            d[i] = 1
    f.close()
    cpt = 0
    for x in d:
        cpt = cpt + 1
    #    print(str(cpt)+ " " + str(d[x])) 
    print(cpt)
    return cpt

'''
178269 communities
338093 communities
183428 communities
177936 communities
186182
183062
180242
181161

def number_of_communities(s):
    f = open(s,"r")
    l = []
    cpt = 0
    while True :
        r = f.readline()
        if not r:
            break
        match = re.search('([0-9]+)',r)
        i = int(match.group().rstrip("\n\r"))
        if i not in l :
            l.append(i)
            ctp = cpt + 1
    f.close()
    for x in d:
        print(cpt) 
'''

input_name = sys.argv[1]
print(number_of_nodes_per_community(input_name))

'''
orig_stdout = sys.stdout
f = open('histogramme_1000.txt', 'w')
sys.stdout = f

for i in range(0,1001):
    print(str(i) + " " +str(number_of_nodes_per_community(input_name)))

sys.stdout = orig_stdout
f.close()
number_of_communities(sys.argv[1])
'''
