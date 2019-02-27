#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <dirent.h>
/* for ETIMEDOUT */
#include <errno.h>
#include <string.h>



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

void free_matrix(int *m){
  free(m);
}

Element **adjacency_list(FILE *f, int *rename_tab, unsigned int nb_nodes){


  Element **nodes = (Element **)malloc(sizeof(Element*)*nb_nodes);
  printf("hellfdsfdo\n");
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
  free(F->last);
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
  free(F);
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
      if(new_diameter == prv_diameter) break;
      prv_diameter = new_diameter;
    }
    //result can be inferior
    if(res == new_diameter) break;
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
    sscanf(line, "%u %u", & i, & j);
    unsigned int save_i = i;
    unsigned int save_j = j;

    i = new_ident[rename_tab[i]];
    j = new_ident[rename_tab[j]];
    unsigned int *U = nodes[i]->voisins;
    unsigned int *V = nodes[j]->voisins;
    printf("examining edge %u %u, position in tab is %d %d\n", rename_tab[save_i], rename_tab[save_j],save_i, save_j);

    //compute intersection
    unsigned int W[MIN(nodes[i]->degree, nodes[j]->degree)];
    unsigned int cpt_u = 0, cpt_v = 0;
    unsigned int cpt_w = 0;
    trips_connected += nodes[i]->degree +nodes[j]->degree;
    while (cpt_u < nodes[i]->degree && cpt_v < nodes[j]->degree){

      unsigned int u = nodes[i]->voisins[cpt_u];
      unsigned int v = nodes[j]->voisins[cpt_v];
      printf("u vaut %u, u vaut %d\n", u, v);
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
          printf("triangle {%u, %u, %u}\n",rename_tab[save_i],rename_tab[save_j],v); 
          cpt_u++;
          cpt_v++;
          nb_triangles++;
        }

      }
    }

  }
  printf("clustering coefficient is %f\n", nb_triangles*3.0/trips_connected);
  return nb_triangles;
    
}

NodDegree *compute_triangles_aux(FILE *f, unsigned int *rename_tab, unsigned int nb_nodes){

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

    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];

    nodes[i]->voisins[nb_neighbors_added[i]] = j;
    nodes[j]->voisins[nb_neighbors_added[j]] = i;

    nb_neighbors_added[i]++;
    nb_neighbors_added[j]++;
  }



  qsort (nodes, sizeof(NodDegree)*nb_nodes/sizeof(NodDegree), sizeof(NodDegree), comp);

  unsigned int new_ident[nb_nodes];
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
    

  
  for(i=0; i < nb_nodes; i++){
    printf("%d a pour voisins ", nodes[i]->ident);
    for(j=0; j < nodes[i]->degree; j++){
      printf("%d ", nodes[i]->voisins[j]);
    }
    printf("\n");
  }
  for(i=0; i < nb_nodes; i++){
    printf("%d s'est fait renommer en %d\n", i,new_ident[i]);
  }
  

  //Finished building data structure
  unsigned int nb_triangles = computeTriangles(f,nodes,rename_tab,new_ident);
  printf("le nombre de triangles est %u\n", nb_triangles);

  return nodes;
}

int op;
char *dir_name;


pthread_mutex_t calculating = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t done = PTHREAD_COND_INITIALIZER;

void *menu(char * ENTREE, int op);

//put all parameters global to use them in testing functions without having to completely change functions signature

FILE *output_file;
FILE *output_file_2;
DIR *d;
struct dirent *dir;

FILE *f_in;

clock_t start;
clock_t end;
double elapsed_time;
//not a good style of programming ..
/* void *test_bfs(void *data) */
/* { */
/*   int oldtype; */

/*   /\* allow the thread to be killed at any time *\/ */
/*   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype); */

/*   /\* ... calculations and expensive io here, for example: */
/*    * infinitely loop */
/*    *\/ */
/*   start = clock(); */
/*   printf("Computing adj_list time\n"); */
/*   lst = adjacency_list(f_in, rename_tab, nb_nodes); */
/*   end = clock(); */
/*   elapsed_time = (end-start)/(double)CLOCKS_PER_SEC; */
/*   printf("%s took %f time processing adjacency list\n", dir->d_name, elapsed_time); */
/*   fprintf(output_file,"%d %f\n", nb_nodes, elapsed_time); */
/*   start = clock(); */
/*   printf("Computing bfs time\n"); */
/*   first_nod = 0; */
/*   bfs(lst, &first_nod, nb_nodes); */
/*   end = clock(); */

/*   elapsed_time = (end-start)/(double)CLOCKS_PER_SEC; */
/*   printf("%s took %f time processing bfs\n", dir->d_name, elapsed_time); */
/*   fprintf(output_file,"%d %f\n", nb_nodes, elapsed_time); */
/*   free_adjacency_list(lst, nb_nodes); */

/*   /\* wake up the caller if we've completed in time *\/ */
/*   pthread_cond_signal(&done); */
/*   return NULL; */
/* } */


/* int do_or_timeout(struct timespec *max_wait) */
/* { */
/*   struct timespec abs_time; */
/*         pthread_t tid; */
/*         int err; */

/*         pthread_mutex_lock(&calculating); */
/*         printf("creating thread %d\n", abs_time.tv_sec); */
/*         /\* pthread cond_timedwait expects an absolute time to wait until *\/ */
/*         clock_gettime(CLOCK_REALTIME, &abs_time); */
/*         abs_time.tv_sec += max_wait->tv_sec; */
/*         abs_time.tv_nsec += max_wait->tv_nsec; */

/*         pthread_create(&tid, NULL, test_bfs, NULL); */

/*         /\* pthread_cond_timedwait can return spuriously: this should */
/*          * be in a loop for production code */
/*          *\/ */
/*         err = pthread_cond_timedwait(&done, &calculating, &abs_time); */
/*         printf("i am waking up\n"); */
/*         if (err == ETIMEDOUT) */
/*                 fprintf(stderr, "%s: calculation timed out\n", __func__); */


/*         pthread_mutex_unlock(&calculating); */

/*         return err; */
/* } */


void do_op(char *d_path, int op){
     char line[SIZE_OF_LINE];

     unsigned int *rename_tab;
     unsigned int nb_nodes;
     unsigned int nb_edges;

printf("hello\n");
      if ((f_in = fopen(d_path, "r")) == NULL) {
        fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", dir->d_name);
        return;
      }
      unsigned int i = 0, j=0;
      unsigned int max = 0;

      nb_nodes = 0;
      nb_edges = 0;
      start = clock();
      /**Get node number to allocate the adjacency matrix**/
      while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
        sscanf(line, "%u %u", & i, & j);


        unsigned int m = MAX(i, j);
    
        if (m > max) {
          max = m;
        }
        nb_edges++;
      }

      printf("nb nodes calculated..\n");
      
      rename_tab = (unsigned int*)malloc(sizeof(unsigned int)*(max+1));
      printf("now renaming..\n");

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


      struct timespec max_wait;

      memset(&max_wait, 0, sizeof(max_wait));
      
      /* wait at most 100 seconds */
      max_wait.tv_sec = 10;

      printf("Beginning operation\n");

      Element **lst;
      adjarray *arr;
      unsigned int first_nod;;
      int *m;
      switch(op){
        //testing adjacency matrix
      case 0:
        m = adjacency_matrix(f_in, rename_tab, nb_nodes);

        end = clock();
        free_matrix(m);
        break;
        //adjacency list
      case 1:
        lst = adjacency_list(f_in, rename_tab, nb_nodes);        
        end = clock();
        free_adjacency_list(lst, nb_nodes);
        break;
        //adjacency array
      case 2:
        arr = adjacency_array(f_in, rename_tab, nb_nodes, nb_edges);
        free_adj_array(arr);
        end = clock();
        break;
        //bfs
      case 3:
//        do_or_timeout(&max_wait);

        lst = adjacency_list(f_in, rename_tab, nb_nodes);
        printf("hello\n");
        end = clock();
        elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
        printf("%s took %f time processing adjacency list\n", dir->d_name, elapsed_time);
        start = clock();
        first_nod = 1;
        bfs(lst, &first_nod, nb_nodes);
        end = clock();
        //free_adjacency_list(lst, nb_nodes);
  
        break;
      default:
        break;
      }




      //double elapsed_time = (end-start)/(double)CLOCKS_PER_SEC;
      printf("File %s took %f time processing the operation\n", dir->d_name, elapsed_time);

      //fprintf(output_file, "%d %f\n", nb_edges, elapsed_time);
      //      adjacency_list_sorted_by_degree(f_in, rename_tab, nb_nodes);
      free(rename_tab);
      fclose(f_in);
}


void *menu(char * ENTREE, int op) {


  printf("hello %s\n", ENTREE);
  d = opendir(ENTREE);
  

  char output_file_name[255];
  char output_file_name2[255];
  switch(op){
  case 0:
    sprintf(output_file_name, "%s", "res_adj_matrix.txt");
    break;
  case 1:
    sprintf(output_file_name, "%s", "res_adj_list.txt");
    break;
  case 2:
    sprintf(output_file_name, "%s", "res_adj_arr.txt");
    break;
  case 3:
    //hackish and very ugly
    output_file_2 = fopen("res_adj_arr.txt", "w"); 
    sprintf(output_file_name, "%s", "res_bfs.txt");
    break;
    
  }

  output_file = fopen(output_file_name, "w");
  
  if(d){
    printf("Entering %s directory\n", ENTREE);
    while((dir = readdir(d)) != NULL){
      if(dir->d_type != DT_REG) 
        continue;
      printf("Reading file %s\n", dir->d_name);
      char d_path[255]; 
      sprintf(d_path, "%s/%s", ENTREE, dir->d_name);
      do_op(d_path, op);
    closedir(d);
    }
  }
  else{
    printf("Couldnot open directory\n");
  }
  fclose(output_file);
  fclose(output_file_2);

  return NULL;

}





int main(int argc, char *argv[]) {
  printf("hello\n");
  op = atoi(argv[2]);
  dir_name = argv[1];
  srand(time(NULL));
  //print_matrix(m, nb_nodes);

  /* wait at most 2 seconds */
//  max_wait.tv_sec = 10000;
//  do_or_timeout(&max_wait, argv[1], op);
  do_op(argv[1], op);
  //fclose(output_file);
  //printf("%d", special_quantity("email-Eu-core.txt", 1005));
  //degree("email-Eu-core.txt",1005);
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  return 0;
}
