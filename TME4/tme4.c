#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

#define SIZE_OF_LINE 81

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    printf("Commentaire, skip line\n");         \
    continue;                                   \
  }                                             \

static int labels=0;

typedef struct Element Element;
struct Element
{
  unsigned int nombre;
  Element *suivant;
};

Element *add(int voisin, Element *liste){
  Element *new_elem = (Element *)malloc(sizeof(Element));
  new_elem->suivant = liste;
  new_elem->nombre = voisin;
  return new_elem;
}


struct __LabeledElement{
  Element *voisins;
  unsigned int label;
  unsigned int ident;
};
typedef struct __LabeledElement *LabeledElement;
typedef LabeledElement *adjlist;

LabeledElement *adjacency_list(FILE *f, int *rename_tab, unsigned int nb_nodes){

  LabeledElement *nodes = (LabeledElement *)malloc(sizeof(LabeledElement)*nb_nodes);
  unsigned int i, j;
  char line[SIZE_OF_LINE];
  for(i=0; i < nb_nodes; i++){
    nodes[i] = (LabeledElement)malloc(sizeof(struct __LabeledElement));
    nodes[i]->voisins = NULL;
    nodes[i]->label = labels++;
    nodes[i]->ident = i;
    
  }
  
  fseek(f, 0, SEEK_SET);

  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    IGNORE_COMMENTS
      sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];
    nodes[i]->voisins = add(j, nodes[i]->voisins);
    nodes[j]->voisins = add(i, nodes[j]->voisins);
  }

  return nodes;
}

void print_adjacency_list(adjlist lst, int nb_nodes){
  unsigned int i, j;
  for(i=0; i < nb_nodes; i++){
    printf("Les voisins de %d sont ", i);
    Element *node = lst[i]->voisins;
    while(node){
      printf("%u ", node->nombre);
      node = node->suivant;
    }
    printf("\n");
  }
}

void free_adjacency_list(adjlist lst, int nb_nodes){
  unsigned int i, j;
  for(i=0; i < nb_nodes; i++){
    Element *node = lst[i]->voisins;
    while(node){
      Element *tmp = node->suivant;
      free(node);
      node = node->suivant;
    }
  }
  free(lst);
}

unsigned int* rename_nods(FILE *f_in, unsigned int *nb_nodes){
  char line[SIZE_OF_LINE];

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
  *nb_nodes = cpt;
  return rename_tab;

}
void swap(adjlist G, size_t i, size_t j, unsigned int *pos_tab) {
  LabeledElement temp = G[i];
  int tmp = pos_tab[i];
  pos_tab[G[i]->ident] = j;//pos_tab[j];
  pos_tab[G[j]->ident] = i;//tmp;
  G[i] = G[j];
  G[j] = temp;
}

void fisher_yates_shuffle(adjlist G,unsigned int n, unsigned int *pos_tab) {
  unsigned int i;    
  for (i = 0; i < n; i++){
    swap(G, i, i+(rand()%(n-i)), pos_tab); // swap element with random later element
  }
}


void label_propagation(adjlist G, unsigned int nb_nodes){
  size_t memory_block = sizeof(unsigned int)*nb_nodes;
  unsigned int *pos_tab = malloc(memory_block);
  unsigned int *freq = malloc(memory_block);	
  unsigned int *found_labels = malloc(memory_block);	
  unsigned int i, j, cpt, max_freq, max_label;
  unsigned int step = 0;
  memset(freq, 0, memory_block);
  memset(found_labels, 0, memory_block);

  for(i=0;i<nb_nodes;i++){
    pos_tab[i] = i;
  }
  int flag, stopped=0;

  unsigned int count = 0;
  while(!stopped){
    stopped = 1;
    printf("turn over\n");
    fisher_yates_shuffle(G, nb_nodes, pos_tab);
    for(i=0;i<nb_nodes;i++){
      cpt=0;
      max_freq = 0;
      Element *voisins = G[i]->voisins;
      while(voisins){
        if(freq[G[pos_tab[voisins->nombre]]->label] == 0){
          found_labels[cpt++] = G[pos_tab[voisins->nombre]]->label;			
        }
          //printf("voisin de %d est  %d le label est %d\n", G[i]->ident, G[pos_tab[voisins->nombre]]->ident, G[pos_tab[voisins->nombre]]->label);
        freq[G[pos_tab[voisins->nombre]]->label] ++;
        voisins = voisins->suivant;
      }
      for (j=0; j<cpt;j++){
          //printf("max_label : %d\n", max_label);
        //        printf("la frequence vaut %u, max_freq vaut %u\n", freq[found_labels[j]], max_freq);
        if(freq[found_labels[j]] > max_freq){
          max_freq = freq[found_labels[j]];
          max_label = found_labels[j];
          flag=0;
        }
        else{
          if(freq[found_labels[j]] == max_freq){
            flag=1;
          }
        }
        freq[found_labels[j]] = 0;
      }
      if(G[i]->label != max_label && !flag)
        stopped=0;
      G[i]->label = max_label;
        

        
    }
    step++;
  }
	
}

void write_labels_to_file(FILE *f, adjlist G, unsigned int n){
  unsigned i = 0;

  
  for(i=0;i<n;i++){
    fprintf(f, "%u %u\n", G[i]->ident, G[i]->label);
  }
}

int main(int argc, char *argv[]) {
	
  clock_t start = clock();
  
  clock_t end = clock();
  srand(time(NULL));
  FILE * f_in, *f_out;
  char *ENTREE = argv[1];
  char *output = argv[2];
	
  
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  f_out = fopen(output, "w");
  if (f_out == NULL){
    printf("Erreur open file\n");
  }
  unsigned int nb_nodes;
  unsigned int *rename_tab = rename_nods(f_in, &nb_nodes);
  adjlist G = adjacency_list(f_in, rename_tab, nb_nodes);
    

  label_propagation(G, nb_nodes);
  //number of community is number of differents labels
  write_labels_to_file(f_out, G, nb_nodes);
  
    
  //print_matrix(m, nb_nodes);
  
  //printf("%d", special_quantity("email-Eu-core.txt", 1005));
  //degree("email-Eu-core.txt",1005);
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  fclose(f_in);
  fclose(f_out);
  return 0;
}

