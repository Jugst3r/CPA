#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

#define SIZE_OF_LINE 81

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    printf("Commentaire, skip line\n");         \
    printf("%s\n", line);                       \
    continue;                                   \
  }                                             \


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
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    *(access_matrix(adja_matrix, rename_tab[i], rename_tab[j], nb_nodes)) = 1;
    *(access_matrix(adja_matrix, rename_tab[j], rename_tab[i], nb_nodes)) = 1;
  }
  return adja_matrix;
}

void free_matrix(int *m){
  free(m);
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
      IGNORE_COMMENTS
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

void free_adjacency_list(Element **lst, int nb_nodes){
  unsigned int i, j;
  for(i=0; i < nb_nodes; i++){
    Element *node = lst[i];
    while(node){
      Element *tmp = node->suivant;
      free(node);
      node = node->suivant;
    }
  }
  free(lst);
}


//can be optimized
//can be optimized
adjarray *adjacency_array(FILE *f, int *rename_tab, unsigned int nb_nodes, unsigned int nb_edges){

  adjarray *res = (adjarray *)malloc(sizeof(adjarray));
  unsigned int edges = 0;
  res->n = nb_nodes;
  res->m = nb_edges;
  unsigned int *cd = (unsigned int*)malloc(sizeof(unsigned int)*nb_nodes);
  unsigned int *where_to_add = (unsigned int*)malloc(sizeof(unsigned int)*nb_nodes);
  unsigned int *adj = (unsigned int*)malloc(sizeof(unsigned int)*nb_edges*2);

  char line[SIZE_OF_LINE];
  unsigned int i, j;
  for(i=0; i < nb_nodes; i++){
    where_to_add[i] = 0;
    cd[i] = 0;
  }

  fseek(f, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    //    printf("arete examinee %u %u\n", i, j);
    i = rename_tab[i];
    j = rename_tab[j];
    cd[i]++;
    cd[j]++;
  }

  cd[0] = 0;
  unsigned int tmp = cd[0];
  for(i=1; i < nb_nodes; i++){
    tmp = cd[i];
    cd[i] = tmp + cd[i-1];
    
  }
  fseek(f, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    unsigned int save_i = i;
    unsigned int save_j = j;
    i = rename_tab[i];
    j = rename_tab[j];
    adj[cd[i]+(where_to_add[i]++)] = j;
    adj[cd[j]+(where_to_add[j]++)] = i;

  }

  res->cd = cd;
  res->adj = adj;
  free(where_to_add);
  return res;
}

void free_adj_array(adjarray *arr){
  free(arr->cd);
  free(arr->adj);
  free(arr);
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
  return res;
}

int is_empty(fifo f){
  return f->last == NULL;
}

//returns farest point from start point (put in variable s) and its distance
unsigned int bfs(adjlist G, unsigned int *s, unsigned int nb_nodes){
  fifo F = create_fifo();
  unsigned int i;
  unsigned int cpt2 = 0;
  unsigned int cpt = 0;
  unsigned int *marquage = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  //unsigned int h = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  unsigned int *distances = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
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
    while(voisins){
      if(marquage[voisins->nombre] == 0){
        add_to_fifo(F, voisins->nombre);
        marquage[voisins->nombre] = 1;
		cpt2++;
        distances[voisins->nombre] = distances[u] + 1;
      }
      voisins = voisins->suivant;
    }
    cpt++;
	//h[cpt] = cpt2;
  }
  *s=u;

  return distances[u];
  
}


unsigned int nbr_compo(adjlist G, unsigned int *s, unsigned int nb_nodes){
  fifo F = create_fifo();
  unsigned int i;
  unsigned int connex_size = 0;
  unsigned int max_connex_size = 0;
	unsigned int nb_nodes_visited;
  unsigned int max = 0;
  unsigned int *marquage = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  //unsigned int h = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  unsigned int *distances = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  for(i=0; i < nb_nodes; i++){
    marquage[i] = 0;
    distances[i] = 0;
  }

	while(nb_nodes_visited< nb_nodes){
	connex_size = 0;
	  unsigned int u;
	for(i=0; i < nb_nodes; i++){
	if(marquage[i] == 0){
		u = i;
	}
	}
	connex_size++;
			nb_nodes_visited++;
	  add_to_fifo(F, *s);
	  marquage[*s] = 1;
	  i=0;
	  while(!is_empty(F)){
		u = pop(F);

		Element *voisins = G[u];
		while(voisins){
		  if(marquage[voisins->nombre] == 0){
		    add_to_fifo(F, voisins->nombre);
		    marquage[voisins->nombre] = 1;
			nb_nodes_visited++;
			connex_size++;
		    distances[voisins->nombre] = distances[u] + 1;
		  }
		  voisins = voisins->suivant;
		}
	   	if (connex_size > max_connex_size){
			max_connex_size = connex_size;
		}
		//h[cpt] = cpt2;
	  }
	}
  printf("%u",max_connex_size);
  return max_connex_size;
  
}


//returns farest point from start point (put in variable s) and its distance
unsigned int largest_component(adjlist G, unsigned int *s, unsigned int nb_nodes){
  fifo F = create_fifo();
  unsigned int i;
  unsigned int cpt2 = 0;
  unsigned int cpt = 0;
  unsigned int *marquage = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  //unsigned int h = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  unsigned int *distances = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
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
    while(voisins){
      if(marquage[voisins->nombre] == 0){
        add_to_fifo(F, voisins->nombre);
        marquage[voisins->nombre] = 1;
		cpt2++;
        distances[voisins->nombre] = distances[u] + 1;
      }
      voisins = voisins->suivant;
    }
    cpt++;
	//h[cpt] = cpt2;
  }
  *s=u;

  return distances[u];
  
}


unsigned int diametre(adjlist G, unsigned int nb_nodes){
  unsigned int prv_diameter = 0;
  unsigned int new_diameter = 0;
  unsigned int res = 0;
  int has_changed = 1;
    
  unsigned int bound = 10, i=0, j=0;
  
  while(i++<10){
    unsigned int start = rand() % nb_nodes;
    j=0;
    while(j++<10){
      //start is modified by bfs to fit new call
      new_diameter = bfs(G, &start, nb_nodes);
	  printf("%u\n",new_diameter);
      if(new_diameter == prv_diameter) break;
      prv_diameter = new_diameter;
    }
    //result can be inferior
    if(new_diameter > res) 
	res = new_diameter;
  }
  return res;
}


typedef struct NodDegree *NodDegree;
struct NodDegree{
  unsigned int *voisins;
  unsigned int ident;
  unsigned int degree;
};



int comp (const void * elem1, const void * elem2) 
{
  NodDegree f = *((NodDegree *)elem1);
  NodDegree s = *((NodDegree *)elem2);
  if (f->degree > s->degree) return  1;
  if (f->degree < s->degree) return -1;
  return 0;
}

int compEntier (const void * elem1, const void * elem2) 
{
  unsigned int f = *((unsigned int *)elem1);
  unsigned int s = *((unsigned int *)elem2);
  //printf("f faut %d, s, vaut %d\n", f->degree, s->degree);
  if (f > s) return  1;
  if (f < s) return -1;
  return 0;
}


//could be clearer, rename_tab is to translate input identifier to renamed ones
//new_ident is to know in which position of the newly sorted nodes tab it is
unsigned int computeTriangles(FILE *f, NodDegree *nodes, unsigned int *rename_tab, unsigned int *new_ident){
  unsigned int nb_triangles = 0, i, j;
  unsigned int trips_connected = 0;
  char line[SIZE_OF_LINE];
  
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    unsigned int save_i = i;
    unsigned int save_j = j;

    i = new_ident[rename_tab[i]];
    j = new_ident[rename_tab[j]];
    unsigned int *U = nodes[i]->voisins;
    unsigned int *V = nodes[j]->voisins;
    //printf("examining edge %u %u, position in tab is %d %d\n", rename_tab[save_i], rename_tab[save_j],save_i, save_j);

    //compute intersection
    unsigned int *W = (unsigned int *)malloc(sizeof(MIN(nodes[i]->degree, nodes[j]->degree)));
    unsigned int cpt_u = 0, cpt_v = 0;
    unsigned int cpt_w = 0;
    trips_connected += nodes[i]->degree +nodes[j]->degree;
    while (cpt_u < nodes[i]->degree && cpt_v < nodes[j]->degree){

      unsigned int u = nodes[i]->voisins[cpt_u];
      unsigned int v = nodes[j]->voisins[cpt_v];
      //printf("u vaut %u, u vaut %d\n", u, v);
      if(u < v){
        cpt_u++;
      }
      else{
        if(u > v){
          cpt_v++;
        }
        //in intersection
        else{
          //triangle detected
          //printf("triangle {%u, %u, %u}\n",rename_tab[save_i],rename_tab[save_j],v); 
          cpt_u++;
          cpt_v++;
          nb_triangles++;
        }

      }
    }
free(W);

  }
  printf("clustering coefficient is %f\n", nb_triangles*3.0/trips_connected);
  return nb_triangles;
    
}

//efficiency can be gained using adjacency_array which is much more compact
NodDegree *adjacency_list_sorted_by_degree(FILE *f, unsigned int *rename_tab, unsigned int nb_nodes){

  NodDegree *nodes = (NodDegree*)malloc(sizeof(NodDegree)*nb_nodes);
  unsigned int *nb_neighbors_added = (unsigned int *)malloc(sizeof(unsigned)*nb_nodes);
  if(nb_neighbors_added == NULL){
	printf("y a un pb");
	return 0;
  }
  unsigned int i, j;
  char line[SIZE_OF_LINE];
  for(i=0; i < nb_nodes; i++){
    nodes[i] = (NodDegree)malloc(sizeof(struct NodDegree));
    nodes[i]->degree=0;
    nodes[i]->ident = i;
    nb_neighbors_added[i] = 0;
  }


  //must get nod degrees first
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];
    nodes[i]->degree++;
    nodes[j]->degree++;
  }

  //allocating nod neighbors
  for(i=0; i < nb_nodes; i++){
    nodes[i]->voisins = (unsigned int*)malloc(nodes[i]->degree*sizeof(unsigned int));
  }
  
  fseek(f, 0, SEEK_SET);


  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];

    nodes[i]->voisins[nb_neighbors_added[i]] = j;
    nodes[j]->voisins[nb_neighbors_added[j]] = i;

    nb_neighbors_added[i]++;
    nb_neighbors_added[j]++;
  }



  qsort (nodes, sizeof(NodDegree)*nb_nodes/sizeof(NodDegree), sizeof(NodDegree), comp);

  unsigned int *new_ident = (unsigned int*)malloc(nb_nodes*sizeof(unsigned int));
  for(i=0; i < nb_nodes; i++){
    new_ident[nodes[i]->ident] = i;
  }

  //remove duplicates, build new truncated list and sort
  for(i=0; i < nb_nodes; i++){
  unsigned int compute_new_degree = 0;
  for(j=0; j < nodes[i]->degree; j++){
    if(new_ident[nodes[i]->voisins[j]] > i){
      compute_new_degree++;
    }
  }
  unsigned int *voisins = (unsigned int*)malloc(compute_new_degree*sizeof(unsigned int));
  unsigned int cpt = 0;
  for(j=0; j < nodes[i]->degree; j++){
    if(new_ident[nodes[i]->voisins[j]] > i){
      voisins[cpt++] = nodes[i]->voisins[j];
    }
  }
  qsort(voisins, compute_new_degree*sizeof(unsigned int)/sizeof(unsigned int), sizeof(unsigned int), compEntier);
  nodes[i]->degree = compute_new_degree;
  nodes[i]->voisins = voisins;
}
    
  //Finished building data structure
  unsigned int nb_triangles = computeTriangles(f,nodes,rename_tab,new_ident);
printf("le nombre de triangles est %u\n", nb_triangles);

  return nodes;
}

unsigned int nb_nodes;
unsigned int nb_edges;
void *menu(char * ENTREE, int op) {

clock_t start = clock();
clock_t end;
double elapsed_time;

  FILE * f_in;
  char line[SIZE_OF_LINE];
  
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  unsigned int i = 0, j=0;
  unsigned int max = 0;

  /**Get node number to allocate the adjacency matrix**/
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    unsigned int m = MAX(i, j);
    
    if (m > max) {
      max = m;
    }
    nb_edges++;
  }

 
  unsigned int *rename_tab = (unsigned int*)malloc(sizeof(unsigned int)*(max+1));
  unsigned int *degrees = (unsigned int*)malloc(sizeof(unsigned int)*(max+1));
  i=0;
  for(i = 0; i < max+1; i++){
    rename_tab[i] = 0;
    degrees[i] = 0;
  }

  fseek(f_in, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
      IGNORE_COMMENTS
    sscanf(line, "%u %u", & i, & j);
    rename_tab[i] = 1;
    rename_tab[j] = 1;
    degrees[i]++;
    degrees[j]++;
  }
  /*cpt is at the end the number of nodes*/
  unsigned int cpt = 0;

  for(i = 0; i < max+1; i++){
    if(rename_tab[i] == 1){
      rename_tab[i] = cpt++;
    }
  }
  nb_nodes = cpt;


      Element **lst;
      adjarray *arr;
      unsigned int first_nod;;
      int *m;
	  unsigned int d;
	
      switch(op){
        //testing adjacency matrix
      case 0:
        m = adjacency_matrix(f_in, rename_tab, nb_nodes);

        end = clock();
        elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
        free_matrix(m);
        printf("%s took %f time processing adjacency matrix\n", ENTREE, elapsed_time);
        break;
        //testing adjacency list
      case 1:
        lst = adjacency_list(f_in, rename_tab, nb_nodes);   
        end = clock();
	elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
        free_adjacency_list(lst, nb_nodes);
        printf("%s took %f time processing adjacency list\n", ENTREE, elapsed_time);
        break;
        //testing adjacency array
      case 2:
        arr = adjacency_array(f_in, rename_tab, nb_nodes, nb_edges);
        end = clock();
        elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
        printf("%s took %f time processing adjacency array\n", ENTREE, elapsed_time);
        free_adj_array(arr);
        break;
        //bfs
      case 3:
		//Number of nodes in the max compenent 
        lst = adjacency_list(f_in, rename_tab, nb_nodes);
        d = nbr_compo(lst, &first_nod, nb_nodes);
        printf("%s took a %u noeud dans le plus grand composants\n", ENTREE, d);
        free_adjacency_list(lst, nb_nodes);
        break;
	  case 4:
		//Tetsing dimaters 
		lst = adjacency_list(f_in, rename_tab, nb_nodes);
		d = diametre(lst,nb_nodes);
		printf("%s had diameter %d \n", ENTREE,d);
		break;
	  case 5:
		start = clock();		
		adjacency_list_sorted_by_degree(f_in,rename_tab,nb_nodes);
		end = clock();		
		elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
		printf("%s took %f time counting triangle numbers\n", ENTREE, elapsed_time);
	  break;
      default:
        break;
      }
  free(rename_tab);
  fclose(f_in);
  return NULL;
}



int main(int argc, char *argv[]) {
	
  clock_t start = clock();
  int *m = menu(argv[1], atoi(argv[2]));
  clock_t end = clock();
  srand(time(NULL));
  return 0;
}


