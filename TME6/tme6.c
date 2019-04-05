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
    continue;                                   \
  }                                             \

#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))


typedef struct __pair{
  unsigned ident;
  unsigned degree;
}Pair;

typedef struct __tas
{
  Pair *tab;
  unsigned *pos_elem_in_tas;
  unsigned last; 
} Tas;



typedef struct NodDegree *NodDegree;
struct NodDegree{
  unsigned int *voisins;
  unsigned int degree;
  unsigned int ident;
  double score;
};

void afficheTas(Tas *t)
{
  int i=0;
  for(i=0; i<t->last; i++)
    printf ("%u, ", t->tab[i].ident);
  printf("\n");
}


void permute(Tas *t, unsigned i , unsigned j )
{
  Pair temp=t->tab[i];
  t->tab[i]=t->tab[j];
  t->tab[j]=temp;
  t->pos_elem_in_tas[(t->tab[i]).ident] = i;
  t->pos_elem_in_tas[(t->tab[j]).ident] = j;
}

void afficherPosElem(unsigned *t, unsigned n){
  unsigned i;
  for(i=0; i<n; i++){
    printf("%u ", t[i]);
  }
  printf("\n");
}

void inserer (Tas *pTas, NodDegree n)
{
  /*placer n en dernière position*/
  unsigned i=pTas->last;
  pTas->tab[i].ident = n->ident;
  pTas->tab[i].degree = n->degree;
  (pTas->last)++;
  /*incrémenter le nb d'éléments*/
  /*recuperation de l'indice du parent*/
  unsigned iParent = i/2;
  pTas->pos_elem_in_tas[n->ident] = i;
  
  while ((i!=1) && (pTas->tab[i].degree < pTas->tab[iParent].degree))
    {
      permute(pTas, iParent, i);
      i = iParent;
      iParent  = i / 2;
    }
}

Pair retireTete(Tas *a, NodDegree *G)
{
  Pair result = a->tab[1];
  unsigned i,j;
  /*s'il y a au moins une permutation possible*/

  /* printf("last vaut %u \n", a->last); */
  /* printf("Pos elem:"); */
  /* afficherPosElem(a->pos_elem_in_tas, 4); */
  /* printf("Tas : "); */
  /* afficheTas(a); */
  a->pos_elem_in_tas[a->tab[1].ident] = 0 ;
  (a->last)--;
  //  printf("elem retire: %u\n", result.ident);
  if (a->last > 0)
    {
      /*décrémentation du nombre d'éléments*/
      /*échange de valeur avec le dernier élément*/
      a->tab[1] = a->tab[a->last];
      i =1;
      int filsTrouve = 1;
      /*tant que l'élément échangé n'est pas arrivé en bout de liste et qu'il est possible de l'échanger avec un de ses fils*/
    
      while ((i < a->last ) && filsTrouve)
        {
          int filsG = 2*i;
          int filsD = 2*i+1;
          int plusPetit = filsG;
          /*s'il existe un fils à gauche*/
          if(plusPetit < a->last)
            {
              int minValFils = a->tab[filsG].degree;
              /*s'il existe un fils à droite*/
              if(filsD<a->last)
                if(a->tab[filsD].degree<minValFils)
                  {
                    plusPetit = filsD;
                    minValFils = a->tab[filsD].degree;
                  }
              /*si l'élément est plus grand que le plus petit de ses fils, on les permute*/
              if (a->tab[i].degree>minValFils)
                permute(a, i, plusPetit);
              /*on recommence avec la nouvelle position de l'élément*/
              i = plusPetit;
            }
          else filsTrouve = 0;
        }
    }




  //  printf("result vaut %u\n", result.ident);
  for(j=0; j<G[result.ident]->degree; j++){
    unsigned voisin = G[result.ident]->voisins[j];
    //    printf("je diminue le degre de %u\n", voisin);
    unsigned pos_tas = a->pos_elem_in_tas[voisin];
    if(pos_tas > 0 ){
      a->tab[pos_tas].degree--;
    
      if(a->tab[pos_tas].ident !=voisin)
        printf("Erreur le noeud n'a pas le bon identifiant\n");
      unsigned i=pos_tas;
      unsigned iParent = i/2;
      while ((i!=1) && (a->tab[i].degree < a->tab[iParent].degree))
        {
          permute(a, iParent, i);
          i = iParent;
          iParent  = i / 2;
        }
    }
  }

  
  return result;
}
void print_l (Pair* p, unsigned n){
  unsigned i;
  for(i = 0;i<n;i++){
    printf("%u %u \n", p[i].ident, p[i].degree);
  }
  printf("\n");
}


Tas *make_tas(NodDegree *G, unsigned nb_nodes){
  Tas* tas = (Tas*) malloc(sizeof(Tas));
  tas->last = 1;
  tas->pos_elem_in_tas = (unsigned*)malloc(sizeof(unsigned)*(nb_nodes+1));
  tas->tab = (Pair*)malloc(sizeof(Pair)*(nb_nodes+1));
  unsigned i = 0;
  for(i=0; i < nb_nodes; i++){
    inserer(tas, G[i]);
  }
  return tas;
}


unsigned* k_core_decomp(NodDegree* G, unsigned nb_nodes, FILE *f){
  unsigned int i  = 0;
  unsigned c = 0;
  Tas *t = make_tas(G, nb_nodes);
  unsigned *l = (unsigned*)malloc(sizeof(unsigned)*(nb_nodes));
    
    

  while(t->last > 1){  
    Pair p =  retireTete(t, G);
    c = MAX(c, p.degree);

    p.degree = c;
    l[nb_nodes-i-1] = p.ident;
    i++;
    if (f!=NULL){
      fprintf(f,"%u %u\n", G[p.ident]->degree,c); 
    }
    //pour exercice 2

      
  }
  
  if(i!=nb_nodes){
    printf("i=%u n'a pas la valeur attendue\n",i);
  }
  return l;
}

unsigned *densest_core_order(NodDegree* G, unsigned nb_nodes, unsigned *prefix_size_res, FILE *f_out){
  unsigned *l = k_core_decomp(G,nb_nodes, f_out);
  unsigned *tab = (unsigned*)malloc(sizeof(unsigned)*(nb_nodes));
  unsigned i, j;
  double  max_average_degree = 0;
  double  max_edge_density = 0;
  unsigned m = 0;
  unsigned n = 0;
  unsigned prefix_size;
  for(i=0;i<nb_nodes;i++){
    tab[l[i]] = i;
  }

  for(i=0;i<nb_nodes;i++){
    n++;
    for(j=0;j<G[l[i]]->degree;j++){
      if (tab[G[l[i]]->voisins[j]] < i){
        m++;
      }
    }
    if (((double) m)/n>max_average_degree) {
      max_average_degree = ((double) m)/n ;
      max_edge_density = ((double) (2*m))/(n*(n-1));
      prefix_size = n;
    }
  }

  printf("For prefix of size %u, average degree density is %g and edge density is %g\n", prefix_size, max_average_degree, max_edge_density);
  *prefix_size_res = prefix_size;
  free(tab);
  return l;
}

NodDegree *adjacency_tab(FILE *f, unsigned int *rename_tab, unsigned int nb_nodes){

  NodDegree *nodes = (NodDegree*)malloc(sizeof(NodDegree)*nb_nodes);
  unsigned *nb_neighbors_added= (unsigned *)malloc(sizeof(unsigned)*nb_nodes);
  
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
    nodes[i]->degree++;

    j = rename_tab[j];
    nodes[j]->degree++;
    
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


    nb_neighbors_added[i]++;



    nodes[j]->voisins[nb_neighbors_added[j]] = i;
    nb_neighbors_added[j]++;
    
  }
  free(nb_neighbors_added);
  return nodes;
}



void afficheGraphe(NodDegree *G, unsigned nb_nodes){
  unsigned i = 0;
  for(i=0; i<nb_nodes; i++){
    printf("%u a pour degre %u\n", i, G[i]->degree);
  }
  printf("\n");
}



void print_authors(char *link, unsigned *prefix, unsigned prefix_size, unsigned *page_tab){
  double max = 0;
  unsigned max_page=0;
  unsigned i, j;
  unsigned start=0;
  for(i=0; i<prefix_size; i++){
    if(strcmp(link, "") != 0 && !fork()){
      char *argv[5];
      argv[0] = "grep";
      argv[1] = "-E";
      argv[2] = malloc(sizeof(char)*128);
      sprintf(argv[2], "^%d[[:blank:]]", page_tab[prefix[i]]);
      argv[3] = link;
      argv[4] = NULL;
      execv("/bin/grep", argv);
    }
      
    else{
      wait(NULL);
    }
  }
}

//exercice 3
void mkscore(NodDegree *G, unsigned n, unsigned t){
  unsigned *presency_tab = (unsigned *)malloc(sizeof(unsigned)*n);
  unsigned i, j, k, l;

  for(i=0; i<n; i++)
    G[i]->score = 0;
  for(l=0; l<t; l++)
    for(i=0; i<n; i++){
      for(k=0; k<G[i]->degree; k++){
        j = G[i]->voisins[k];
        //avoid treating 2 times the nod
        if(j>i)
          if (G[i]->score <= G[j]->score){
            G[i]->score ++;
          }
          else
            G[j]->score ++;

      }

    }

  for(i=0; i<n; i++){
    G[i]->score = G[i]->score/t;
    //printf("Score for %u is %g\n", G[i]->ident, G[i]->score);      
  }
    
}



int cmpfunc (const void * a, const void * b)
{
  NodDegree f = *((NodDegree *)a);
  NodDegree s = *((NodDegree *)b);
  if (f->score < s->score) return  1;
  if (f->score > s->score) return -1;

  return 0;  
}

void calculate_density_second_algo(NodDegree *G, unsigned n, unsigned t){

  unsigned *presency_tab = (unsigned *)malloc(sizeof(unsigned)*n);
  unsigned i, j;
  mkscore(G, n, t);

  qsort(G, n, sizeof(NodDegree), cmpfunc);

  for(i=0; i<n; i++)
    presency_tab[i] = 0;

  double  max_average_degree = 0;
  double  max_edge_density = 0;
  double sum_degree_density = 0;
  unsigned m = 0;
  unsigned prefix_size=0;


  for(i=0;i<n;i++){
    for(j=0;j<G[i]->degree;j++){
      if (presency_tab[G[i]->voisins[j]]){
        m++;
      }
    }
    //hackish..
    i++;
    if (sum_degree_density/i>=max_average_degree) {
      max_average_degree = sum_degree_density/i ;
      if(i>0)
        max_edge_density = ((double) (2*m))/(i*(i-1));
      prefix_size = i;
    }
    i--;
    sum_degree_density += G[i]->score;
    presency_tab[G[i]->ident]=1;
  }
  printf("For prefix of size %u, average degree density is %g and edge density is %g\n", prefix_size, max_average_degree, max_edge_density);
  free(presency_tab);
}


int cmpunsigned (const void * a, const void * b)
{
  unsigned f = *((unsigned *)a);
  unsigned s = *((unsigned *)b);
  if (f > s) return  1;
  if (f < s) return -1;

  return 0;  
}

void afficher_G(NodDegree *G, unsigned n){
  unsigned i, j;
  for(i=0; i<n; i++){
    printf("sommet %u a pour degre %u et pour voisins ",G[i]->ident, G[i]->degree);
    for(j=0; j<G[i]->degree; j++)
      printf("%u, ",G[i]->voisins[j]);
    printf("\n");
  }
}

void afficher_t(unsigned *t, unsigned n){
  printf("Tableau pos_elem vaut ");
  unsigned i;
  for(i=0; i<n; i++)
    printf("%u, ", t[i]);
  printf("\n");
}
void densest_triangle_subgraph(NodDegree *G, unsigned n, unsigned t){
  unsigned *presency_tab = (unsigned *)malloc(sizeof(unsigned)*n);
  unsigned *pos_elem = (unsigned *)malloc(sizeof(unsigned)*n);
  unsigned i, j,  v, cpt_u, cpt_v;
  mkscore(G, n, t);

  qsort(G, n, sizeof(NodDegree), cmpfunc);

  for(i=0; i<n; i++)
    presency_tab[i] = 0;

  double  max_average_degree = 0;
  double  max_edge_density = 0;
  double sum_degree_density = 0;
  double max_triangle_density = 0;
  unsigned m = 0;
  unsigned prefix_size=0;
  unsigned nb_triangles = 0, nb_triangles_i = 0;

  for(i=0; i<n; i++)
    pos_elem[G[i]->ident] = i;
  for(i=0; i<n; i++){
    for(j=0; j<G[i]->degree; j++)
      G[i]->voisins[j] = pos_elem[G[i]->voisins[j]];
    qsort(G[i]->voisins, G[i]->degree, sizeof(unsigned), cmpunsigned);
  }
    
  for(i=0;i<n;i++){
    nb_triangles_i=0;
    for(j=0;j<G[i]->degree;j++){
      v = G[i]->voisins[j];
      if (v<i){
        //look for triangles
        cpt_u=0;
        cpt_v=0;
        while(cpt_u < G[i]->degree && cpt_v < G[v]->degree && G[v]->voisins[cpt_v] < i){
          if(G[i]->voisins[cpt_u] < G[v]->voisins[cpt_v]){
            cpt_u++;
          }
          else {
            if (G[i]->voisins[cpt_u] > G[v]->voisins[cpt_v]){
              cpt_v++;
            }
            else{
              cpt_u++;
              cpt_v++;
              nb_triangles_i ++;
            }
          }
        }
        m++;
      }
    }
    nb_triangles += nb_triangles_i/2;
    //hackish..
    i++;
    if ((double)nb_triangles/i>=max_triangle_density) {
      max_triangle_density = ((double)nb_triangles)/i;
      max_average_degree = sum_degree_density/i;
      if(i>1)
        max_edge_density = ((double) (2*m))/(i*(i-1));
      prefix_size = i;
    }
    i--;
    sum_degree_density += G[i]->score;
    presency_tab[G[i]->ident]=1;
  }
  
  printf("For the whole graph, i have found %u triangles\n", nb_triangles);
  printf("For prefix of size %u, triangle density is %g average degree density is %g and edge density is %g\n", prefix_size, max_triangle_density,max_average_degree, max_edge_density);
  free(presency_tab);
}

void do_it_all(char * ENTREE, char * linking, int op, char *out) {

  clock_t start = clock();
  clock_t end;
  double elapsed_time;

  FILE * f_in;
  char line[SIZE_OF_LINE];

  printf("Demarreage\n");
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

  unsigned prefix_size;
  unsigned *l;
  FILE *f_out = NULL;

  switch(op){
  case 1:
    if(out != NULL){
      f_out = fopen(out, "w");
    }
    l = densest_core_order(G, nb_nodes, &prefix_size, f_out);
    if(out != NULL){
      fclose(f_out);
    }
    printf("Printing\n");
    printf("linking vaut %s\n", linking);
    
    if(linking != NULL){
      print_authors(linking,l,prefix_size,page_tab);
    }

    
    free(l);
    break;
  case 3:
    calculate_density_second_algo(G, nb_nodes, 2);
    break;
  case 5:
        
    
    printf("Computing densest triangle graph\n");
    densest_triangle_subgraph(G, nb_nodes, 2);
    break;
  default:
    printf("Unspecified operation\n");
    break;
  }
  fclose(f_in);
}
  

int main(int argc, char *argv[]) {
  clock_t start = clock();
  if(argc < 3){
    printf("./tme5 sourcefile operation (linkingfile:optional)\n");
    printf("Operations: 1 for k-core decomposition, 3 for densest subgraph, 5 for triangle densest subgraph\n");
    return 0;
  }
  do_it_all(argv[1], argv[3], atoi(argv[2]), argv[4]);

  return 0;
}
   
