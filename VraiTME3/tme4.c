#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

#define SIZE_OF_LINE 81


typedef struct Element Element;
struct Element
{
    int nombre;
    Element *suivant;
};


typedef struct {
  unsigned n;
  unsigned m;
  unsigned * cd;
  unsigned * adj;
}
adjarray;

int find_elem_pos(int * arr, int size, int toSearch) {

  int found = -1;
  int i = 0;
  for (i = 0; i < size; i++) {
    if (arr[i] == toSearch) {
      printf("SSS");
      found = i;
      break;
    }
  }
  return found;
}



int *access_matrix(int *adja_matrix, int i, int j, int nb_nodes){
  return adja_matrix+(nb_nodes*(sizeof(int))*i)+(sizeof(int))*j;
}

int *adjacency_matrix(char * ENTREE) {

  FILE * f_in;
  char line[SIZE_OF_LINE];
  int nb_nodes;
  
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  int i = 0, j=0;
  int nbr_e = 0;
  int max = 0;
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%d %d", & i, & j);
    int m = MAX(i, j);
    if (m > max) {
      max = m;
    }
    nbr_e++;
  }
  fseek(f_in, 0, SEEK_SET);

  nb_nodes = max+1;

  int *adja_matrix = (int *)malloc(sizeof(int)*nb_nodes*nb_nodes);

  
  
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%d %d", & i, & j);
    *(access_matrix(adja_matrix, i, j, nb_nodes)) = 1;
  }
  fclose(f_in);

  return adja_matrix;

}


int main(int argc, char *argv[]) {

  int *m = adjacency_matrix(argv[1]);

  
  //printf("%d", special_quantity("email-Eu-core.txt", 1005));
  //degree("email-Eu-core.txt",1005);
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  return 0;
}
