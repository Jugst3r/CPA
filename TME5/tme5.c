#include <stdio.h>
#include<time.h>
#include <stdlib.h>
#define SIZE_OF_LINE 81

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    printf("Commentaire, skip line\n");         \
    continue;                                   \
  }                                             \

#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

typedef struct NodDegree *NodDegree;
struct NodDegree{
  unsigned int *voisins;
  unsigned int degree;
};

double *prodmatvect(NodDegree *G, double *A, unsigned n){
  double *B = malloc(sizeof(double)*n);
  unsigned i, j;
  for(i=0; i < n; i++){
    B[i] = 0;
    for(j=0; j <  G[i]->degree; j++){
      unsigned v = G[i]->voisins[j];
      B[v] += (A[i]/G[i]->degree);
    }
  }
  return B;
}

double *page_rank(NodDegree *G, float alpha, unsigned t, unsigned n){
  double *I = malloc(sizeof(double)*n);
  unsigned i,j;
  for(i = 0; i < n; i++){
    I[i] = 1.0/n;
  }

  double *P = I;
  unsigned norm = 0;
  for(i = 0; i < t; i++){

    P = prodmatvect(G, P, n);
  
    norm = 0;
    for(j=0; j<n; j++){
      P[j] = (1-alpha)*P[j] + alpha*I[j];
      norm += P[j];
    }
    for(j=0; j<n; j++)
      //printf("P[j] before vaut %lf\n", P[j]);
      P[j] += (1-norm)/(1.0*n);
    //printf("P[j] after vaut %lf\n", P[j]);
      printf("Finished iteration\n");
  }
   
  return P;
}


//efficiency can be gained using adjacency_array which is much more compact
NodDegree *adjacency_tab(FILE *f, unsigned int *rename_tab, unsigned int nb_nodes){

  NodDegree *nodes = (NodDegree*)malloc(sizeof(NodDegree)*nb_nodes);
  unsigned int nb_neighbors_added[nb_nodes];
  
  unsigned int i, j;
  char line[SIZE_OF_LINE];
  for(i=0; i < nb_nodes; i++){
    nodes[i] = (NodDegree)malloc(sizeof(struct NodDegree));
    nodes[i]->degree=0;
    nb_neighbors_added[i] = 0;
  }


  //must get nod degrees first
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    nodes[i]->degree++;
    //    nodes[j]->degree++;
  }

  //allocating nod neighbors
  for(i=0; i < nb_nodes; i++){
    nodes[i]->voisins = (unsigned int*)malloc(nodes[i]->degree*sizeof(unsigned int));
  }
  
  fseek(f, 0, SEEK_SET);


  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];
    nodes[i]->voisins[nb_neighbors_added[i]] = j;
    //    nodes[j]->voisins[nb_neighbors_added[j]] = i;

    nb_neighbors_added[i]++;
    //    nb_neighbors_added[j]++;
  }
  return nodes;
}


void print_highest_page_rank(double *P, unsigned nb_nodes, unsigned *page_tab){
  double max = 0;
  unsigned max_page=0;
  unsigned i;
  for(i=0; i<nb_nodes; i++){
    if(P[i]>1.0)
      printf("P[i] vaut %lf\n", P[i]);
    if(P[i]>max){
      max = P[i];
      max_page = i;
    }
  }
  printf("Highest page rank is page %u with score %g\n", page_tab[max_page], max);
}

void do_it_all(char * ENTREE) {

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

  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    unsigned int m = MAX(i, j);
    
    if (m > max) {
      max = m;
    }
  }


 
  unsigned int *rename_tab = (unsigned int*)malloc(sizeof(unsigned int)*(max+1));
  unsigned int *page_tab = (unsigned int*)malloc(sizeof(unsigned int)*(max+1));
  i=0;
  for(i = 0; i < max+1; i++){
    rename_tab[i] = 0;
  }

  fseek(f_in, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    rename_tab[i] = 1;
    rename_tab[j] = 1;
  }
  /*cpt is at the end the number of nodes*/
  unsigned int cpt = 0;

  for(i = 0; i < max+1; i++){
    if(rename_tab[i] == 1){
      rename_tab[i] = cpt;
      page_tab[cpt] = i;
      cpt++;
    }
  }
  
  //not renaming this time
  unsigned nb_nodes = cpt;
  printf("Computing graph data structure\n");
  NodDegree *G = adjacency_tab(f_in, rename_tab, nb_nodes);
  printf("Starting page rank\n");
  double *P = page_rank(G, 0.15, 10, nb_nodes);
  print_highest_page_rank(P, nb_nodes, page_tab);
}
  

int main(int argc, char *argv[]) {
	
  clock_t start = clock();
  do_it_all(argv[1]);
  clock_t end = clock();

  srand(time(NULL));
  //print_matrix(m, nb_nodes);
  
  //printf("%d", special_quantity("email-Eu-core.txt", 1005));
  //degree("email-Eu-core.txt",1005);
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  return 0;
}
   
