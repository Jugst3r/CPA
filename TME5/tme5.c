#include <stdio.h>
#include<time.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
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
  unsigned int ident;
  double score;
};

void prodmatvect(NodDegree *G, double *A, unsigned n, double *B){
  unsigned i, j;
  for(i=0; i < n; i++){
    B[i] = 0;
  }
  for(i=0; i < n; i++){
    for(j=0; j <  G[i]->degree; j++){
      unsigned v = G[i]->voisins[j];
      //      printf("v: %u, A[i] = %g, degre : %u\n", v, A[i], G[i]->degree); 
      B[v] += (A[i]/G[i]->degree);
    }
  }
}

double *page_rank(NodDegree *G, float alpha, unsigned t, unsigned n){
  double *I = malloc(sizeof(double)*n);
  double *P = malloc(sizeof(double)*n);
  double *P_tmp = malloc(sizeof(double)*n);
  unsigned i,j;
  for(i = 0; i < n; i++){
    I[i] = 1.0/n;
    P_tmp[i] = 1.0/n;
    P[i] = 1.0/n;
  }


  double norm = 0;
  int converging = 0;
  i =0;
  while (!converging && i < t){
      converging = 1;
      prodmatvect(G, P_tmp, n, P);
  
      norm = 0;
      for(j=0; j<n; j++){
        P[j] = (1-alpha)*P[j] + alpha*I[j];
        norm += P[j];
        //printf("P[%d] before vaut %g\n", j,P[j]);
      }
      for(j=0; j<n; j++){
        P[j] += (1-norm)/(1.0*n);
        if(P_tmp[j] != P[j])
          converging = 0;
        P_tmp[j] = P[j];
      }
      //printf("P[j] after vaut %lf\n", P[j]);
      printf("Finished iteration %d\n", i);
      i = i+1;
  }
  free(I);
  free(P_tmp);
   
  return P;
}

//must be done before sorting tab
unsigned *in_degree (NodDegree *G, unsigned n){
unsigned *in_degree_tab = (unsigned *)malloc(sizeof(unsigned)*n);
unsigned i, j;
for(i=0; i<n; i++)
  in_degree_tab[i] = 0;

for(i=0; i<n; i++){
     for(j=0; j <  G[i]->degree; j++){
       in_degree_tab[G[i]->voisins[j]]++;
     }
  }
return in_degree_tab;
  
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
    nodes[i]->ident = i;
    nb_neighbors_added[i] = 0;
  }


  //must get nod degrees first
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    //j = rename_tab[j];
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


void print_highest_lowest_page_rank(char *link, NodDegree *G, unsigned nb_nodes, unsigned *page_tab){
  double max = 0;
  unsigned max_page=0;
  unsigned i, j;
  unsigned start=0;
  for(j=0;j<2;j++){
    if(j==1)
      printf("Highest scores : \n");
    else
      printf("------------------------------------------\nLowest scores : \n");
    for(i=start; i<start+5; i++){
      printf("score is %g for page %u\n", G[i]->score, page_tab[G[i]->ident]);
      if(strcmp(link, "") != 0 && !fork()){
        char *argv[5];
        argv[0] = "grep";
        argv[1] = "-E";
        argv[2] = malloc(sizeof(char)*128);
        sprintf(argv[2], "^%d[[:blank:]]", page_tab[G[i]->ident]);
        argv[3] = link;
        argv[4] = NULL;
        execv("/bin/grep", argv);
      }
      
      else{
        wait(NULL);
      }
    }
    start = nb_nodes-5;
  }
}


int cmpfunc (const void * a, const void * b)
  {
    NodDegree f = *((NodDegree *)a);
    NodDegree s = *((NodDegree *)b);
    if (f->score > s->score) return  1;
    if (f->score < s->score) return -1;

    return 0;  
  }


  void do_it_all(char * ENTREE, char * linking) {

    clock_t start = clock();
    clock_t end;
    double elapsed_time;

    FILE * f_in;
    char line[SIZE_OF_LINE];
  
    if ((f_in = fopen(ENTREE, "r")) == NULL) {
      fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
      return;
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


    printf("Getting nodes in-degree\n");
    unsigned *in_degree_tab = in_degree (G, nb_nodes);
    char *c_out_1 = ("exo2-1.txt");
    FILE *f_out_1 = fopen(c_out_1, "w");

    char *c_out_2 = ("exo2-2.txt");
    FILE *f_out_2 = fopen(c_out_2, "w");
    
    char *c_out_3 = ("exo2-3.txt");
    FILE *f_out_3 = fopen(c_out_3, "w");

    char *c_out_4 = ("exo2-4.txt");
    FILE *f_out_4 = fopen(c_out_4, "w");

    char *c_out_5 = ("exo2-5.txt");
    FILE *f_out_5 = fopen(c_out_5, "w");
    
    char *c_out_6 = ("exo2-6.txt");
    FILE *f_out_6 = fopen(c_out_6, "w");
      
    
    printf("Starting page rank\n");
    double *P = page_rank(G, 0.15, 10, nb_nodes);
    double *P1 = page_rank(G, 0.1, 10, nb_nodes);
    double *P2 = page_rank(G, 0.2, 10, nb_nodes);
    double *P3 = page_rank(G, 0.5, 10, nb_nodes);
    double *P4 = page_rank(G, 0.9, 10, nb_nodes);
    
    printf("Retrieving scores from computed page rank\n");
    for(i=0;i<nb_nodes;i++)
      G[i]->score = P[i];
    printf("Sorting nods on page rank\n");  
    qsort(G, nb_nodes, sizeof(NodDegree), cmpfunc);

    //Retrieving outputs
    for(i=0; i<nb_nodes; i++){
      fprintf(f_out_1, "%g %d\n", G[i]->score, in_degree_tab[G[i]->ident]);
      fprintf(f_out_2, "%g %d\n", G[i]->score, G[i]->degree);
      fprintf(f_out_3, "%g %g\n", G[i]->score, P1[G[i]->ident]);
      fprintf(f_out_4, "%g %g\n", G[i]->score, P2[G[i]->ident]);
      fprintf(f_out_5, "%g %g\n", G[i]->score, P3[G[i]->ident]);
      fprintf(f_out_6, "%g %g\n", G[i]->score, P4[G[i]->ident]);
      
    }

    //Close all output files
    fclose(f_out_1);
    fclose(f_out_2);
    fclose(f_out_3);
    fclose(f_out_4);
    fclose(f_out_5);
    fclose(f_out_6);
    
    printf("Printing\n");
    print_highest_lowest_page_rank(linking ,G, nb_nodes, page_tab);
    fclose(f_in);
  }
  

  int main(int argc, char *argv[]) {
    clock_t start = clock();
    do_it_all(argv[1], argv[2]);
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
   
