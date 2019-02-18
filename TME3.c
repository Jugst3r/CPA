#include <stdio.h>

#define MIN(a, b)(((a) < (b)) ? (a) : (b))
# define MAX(a, b)(((a) > (b)) ? (a) : (b))

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

void nbr_n_e(char * ENTREE) {

  FILE * f_in;
  char line[81];

  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }

  int nbr_e = 0;
  int a;
  int b;
  int max = 0;

  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
    int m = MAX(a, b);
    if (m > max) {
      max = m;
    }
    nbr_e++;
  }

  fclose(f_in);
  printf("%d,%d", max + 1, nbr_e);

}

int * degree(char * ENTREE, int size) {

  FILE * f_in;
  char line[81];

  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  int i = 0;
  int tab1[size];
  int a;
  int b;
  int pos;

  for (i = 0; i < size; i++) {
    tab1[i] = 0;

  }

  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
    tab1[a] = tab1[a] + 1;
  }

  fclose(f_in);
  for (i = 0; i < size; i++) {
    printf("node = %d, degree = %d \n", i, tab1[i]);

  }
  return tab1;

}

int special_quantity(char * ENTREE, int size) {

  FILE * f_in;
  char line[81];

  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  int i = 0;
  int tab1[size];
  int a;
  int b;
  int pos;
  int m = 0;
  int s = 0;

  for (i = 0; i < size; i++) {
    tab1[i] = 0;

  }

  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
    tab1[a] = tab1[a] + 1;
  }

  f_in = fopen(ENTREE, "r");

  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
    m = tab1[a] * tab1[b];
    //printf("%d %d",tab1[a],tab1[b]);
    s = s + m;
  }

  fclose(f_in);

  return s;

}

void distribution(char * ENTREE, int size) {

  FILE * f_in;
  char line[81];

  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  int tab1[size];
  int a;
  int b;
  int pos;
  int m = 0;
  int s = 0;
  int* tab2;
  int i = 0 ;
  int j= 0;

  for (i = 0; i < size; i++) {
    tab1[i] = 0;

  }

  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
    tab1[a] = tab1[a] + 1;
  }

  fclose(f_in);

  for (i = 0; i < size; i++) {
   for (j = 0; j < size; j++) {
	if (tab1[i]==tab1[j] && i!=j){
		s++;
		tab1[i];
	}
   }
   printf("%d %d\n",tab1[i],s);
   s = 0;
  }

	//A ENLVER LES DOUBLONS
}

int main() {

  distribution("email-Eu-core.txt", 1005);
  //printf("%d", special_quantity("email-Eu-core.txt", 1005));
  //degree("email-Eu-core.txt",1005);
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  return 0;
}
