#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

#define SIZE_OF_LINE 81


typedef struct Element Element;
struct Element
{
    unsigned int nombre;
    Element *suivant;
};


typedef struct {
  unsigned n;
  unsigned m;
  unsigned * cd;
  unsigned * adj;
}
adjarray;


Element *add(int voisin, Element *liste){
  Element *new_elem = (Element *)malloc(sizeof(Element));
  new_elem->suivant = liste;
  new_elem->nombre = voisin;
  return new_elem;
}




int *access_matrix(int *adja_matrix, unsigned int i, unsigned int j, unsigned int nb_nodes){
  return adja_matrix+(nb_nodes*(sizeof(int))*i)+(sizeof(int))*j;
}



void print_matrix(int *matrix, unsigned int n){
  unsigned int i, j;
  for(i=0; i < n; i++){
    for(j=0; j < n; j++)
      printf("%u ", *(access_matrix(matrix, i, j, n)));
    printf("\n");
  }
}

void init_matrix(int *matrix, unsigned int n){
  unsigned int i, j;
  for(i=0; i < n; i++){
    for(j=0; j < n; j++){
      *(access_matrix(matrix, i, j, n))=0;
    }
  }
}


int *adjacency_matrix(FILE *f_in, int *rename_tab, unsigned int nb_nodes){
    

  int *adja_matrix = (int *)malloc(sizeof(int)*nb_nodes*nb_nodes);
  if(adja_matrix == NULL){
    printf("Probleme rencontre lors de l'allocation\n");
    return 0;
  }
  init_matrix(adja_matrix, nb_nodes);

  char line[SIZE_OF_LINE];
  unsigned int i, j;
  fseek(f_in, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    *(access_matrix(adja_matrix, rename_tab[i], rename_tab[j], nb_nodes)) = 1;
    *(access_matrix(adja_matrix, rename_tab[j], rename_tab[i], nb_nodes)) = 1;
  }
  return adja_matrix;
}


Element **adjacency_list(FILE *f, int *rename_tab, unsigned int nb_nodes){

  Element **nodes = (Element **)malloc(sizeof(Element*)*nb_nodes);
  unsigned int i, j;
  char line[SIZE_OF_LINE];
  for(i=0; i < nb_nodes; i++){
    nodes[i] = NULL;
  }
  
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];
    nodes[i] = add(j, nodes[i]);
    nodes[j] = add(i, nodes[j]);
  }

  return nodes;
}

void print_adjacency_list(Element **lst, int nb_nodes){
  unsigned int i, j;
  for(i=0; i < nb_nodes; i++){
    Element *node = lst[i];
    while(node){
      printf("%u ", node->nombre);
      node = node->suivant;
    }
    printf("\n");
  }
}


//can be optimized
adjarray *adjacency_array(FILE *f, int *rename_tab, unsigned int nb_nodes, unsigned int nb_edges){
  adjarray *res = (adjarray *)malloc(sizeof(adjarray));
  unsigned int edges = 0;
  res->n = nb_nodes;
  res->m = nb_edges;
  Element **adj_list = adjacency_list(f, rename_tab, nb_nodes);
  unsigned int *cd = (unsigned int*)malloc(sizeof(unsigned int)*nb_nodes);
  unsigned int *adj = (unsigned int*)malloc(sizeof(unsigned int)*nb_edges*2);

  print_adjacency_list(adj_list, nb_nodes);

  unsigned int cpt = 0;
  unsigned int i;
  for(i=0; i < nb_nodes; i++){
    printf("i vaut %d\n", i);
    Element *node = adj_list[i];
    while(node){
        printf("%d a pdsqdqsdour voisin %d \n", i, node->nombre);
      adj[cpt] = node->nombre;
      node = node->suivant;
      cpt ++;
    }
    cd[i] = cpt;
  }

  res->cd = cd;
  res->adj = adj;
 
  return res;
}



void print_adjacency_array(adjarray *a, int nb_nodes){
  unsigned int i, j = 0;
  for(i=0; i < nb_nodes; i++){
    printf("%d a pour voisins ", i);
    while(j < (a->cd)[i]){
      printf("%u ", (a->adj)[j]);
      j++;
    }
    printf("\n");
  }
}

typedef struct ElementFifo *ElementFifo;
typedef struct fifo* fifo;
struct ElementFifo
{
    unsigned int nombre;
    ElementFifo prec;
};

struct fifo{
    ElementFifo first;
    ElementFifo last;
};



typedef Element **adjlist;

fifo create_fifo(){
    fifo F = (fifo)malloc(sizeof(struct fifo));
    F->first = NULL;
    F->last = NULL;
    return F;
}


void add_to_fifo(fifo F, unsigned int s){
    ElementFifo toAdd = (ElementFifo)malloc(sizeof(struct ElementFifo));
    toAdd->nombre = s;
    if(F->last == NULL){
        F->last = toAdd;
    }
    else{
        //at least 1 element then F->first is not null
        F->first->prec = toAdd;
    }
    toAdd->prec = NULL;
    F->first = toAdd;
}

unsigned int pop(fifo F){
    if(F->last == NULL){
        printf("Popping on empty list\n");
        exit(0);
    }
    unsigned int res = F->last->nombre;
    F->last = F->last->prec;
    printf("toAdd vaut %x\n", F->last);
    return res;
}

int is_empty(fifo f){
  return f->last == NULL;
}

//returns farest point from start point (put in variable s) and its distance
unsigned int bfs(adjlist G, unsigned int *s, unsigned int nb_nodes){
  fifo F = create_fifo();
  unsigned int i;
  unsigned int marquage[nb_nodes];
  unsigned int distances[nb_nodes];
  for(i=0; i < nb_nodes; i++){
    marquage[i] = 0;
    distances[i] = 0;
  }

  unsigned int u;
  add_to_fifo(F, *s);
  marquage[*s] = 1;
  i=0;
  while(!is_empty(F)){
    u = pop(F);

    Element *voisins = G[u];
    printf("%u\n", u);
    while(voisins){
      if(marquage[voisins->nombre] == 0){
        add_to_fifo(F, voisins->nombre);
        marquage[voisins->nombre] = 1;
        distances[voisins->nombre] = distances[u] + 1;
      }
      voisins = voisins->suivant;
    }
  }
  *s=u;
  return distances[u];
  
}

unsigned int diametre(adjlist G, unsigned int nb_nodes){
    unsigned int prv_diameter = 0;
    unsigned int new_diameter = 0;
    unsigned int res = 0;
    int has_changed = 1;
    
    while(1){
        unsigned int start = rand() % nb_nodes;
        while(1){
            //start is modified by bfs to fit new call
            new_diameter = bfs(G, &start, nb_nodes);
            if(new_diameter == prv_diameter) break;
            prv_diameter = new_diameter;
        }
        if(res == new_diameter) break;
    }
    return res;
}


typedef struct NodDegree *NodDegree;
struct NodDegree{
  Element *nod;
  unsigned int ident;
  unsigned int degree;
};


int comp (const void * elem1, const void * elem2) 
{
    NodDegree f = *((NodDegree *)elem1);
    NodDegree s = *((NodDegree *)elem2);
    //printf("f faut %d, s, vaut %d\n", f->degree, s->degree);
    if (f->degree > s->degree) return  1;
    if (f->degree < s->degree) return -1;
    return 0;
}

//efficiency can be gained using adjacency_array which is much more compact
NodDegree *adjacency_list_sorted_by_degree(FILE *f, int *rename_tab, unsigned int nb_nodes){

  NodDegree *nodes = (NodDegree*)malloc(sizeof(NodDegree));
  unsigned int i, j;
  char line[SIZE_OF_LINE];
  for(i=0; i < nb_nodes; i++){
    nodes[i] = (NodDegree)malloc(sizeof(struct NodDegree));
    nodes[i]->nod = NULL;
    nodes[i]->degree=0;
    nodes[i]->ident = i;
  }
  
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];
    nodes[i]->nod = add(j, nodes[i]->nod);
    nodes[i]->degree++;
    nodes[j]->nod = add(i, nodes[j]->nod);
    nodes[j]->degree++;
  }

  qsort (nodes, sizeof(unsigned)*nb_nodes/sizeof(unsigned), sizeof(unsigned), comp);
  
    for(i=0; i < nb_nodes; i++){
        printf("nodes[i] vaut %d\n", nodes[i]->ident);
    }
  return NULL;
}

unsigned int nb_nodes;
unsigned int nb_edges;
void *menu(char * ENTREE) {

  FILE * f_in;
  char line[SIZE_OF_LINE];
  
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  unsigned int i = 0, j=0;
  unsigned int max = 0;

  /**Get node number to allocate the adjacency matrix**/
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    unsigned int m = MAX(i, j);
    
    if (m > max) {
      max = m;
    }
    nb_edges++;
  }

 
  unsigned int *rename_tab = (unsigned int*)malloc(sizeof(unsigned int)*(max+1));
  i=0;
  for(i = 0; i < max+1; i++){
    rename_tab[i] = 0;
  }

  fseek(f_in, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    rename_tab[i] = 1;
    rename_tab[j] = 1;
  }
  /*cpt is at the end the number of nodes*/
  unsigned int cpt = 0;

  for(i = 0; i < max+1; i++){
    if(rename_tab[i] == 1){
      rename_tab[i] = cpt++;
    }
  }
  nb_nodes = cpt;





  //int *adjacency_matrix_res = adjacency_matrix(f_in, rename_tab, nb_nodes);
  Element **adjacency_list_res = adjacency_list(f_in, rename_tab, nb_nodes);
  adjarray *adjacency_array_res = adjacency_array(f_in, rename_tab, nb_nodes, nb_edges);
  //print_adjacency_list(adjacency_list_res, nb_nodes);
  print_adjacency_array(adjacency_array_res, nb_nodes);
  unsigned int start = 1;
  printf("la distance du graphe est %d\n", bfs(adjacency_list_res, &start, nb_nodes));
  adjacency_list_sorted_by_degree(f_in, rename_tab, nb_nodes);
  free(rename_tab);
  fclose(f_in);
  return adjacency_array_res;

}





int main(int argc, char *argv[]) {

  int *m = menu(argv[1]);
  srand(time(NULL));
  //print_matrix(m, nb_nodes);
  
  //printf("%d", special_quantity("email-Eu-core.txt", 1005));
  //degree("email-Eu-core.txt",1005);
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  return 0;
}
