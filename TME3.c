#include <stdio.h>
#include <stdlib.h>

#define SIZE_OF_LINE 81
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

  int x = 0;
  int i = 0;
  for (i = 0; i < size; i++) {
    if (arr[i] == toSearch) {
		x++;
    }
  }
  if (x>0){
	return x;	
	}
  else{
	return -1;
  }
}

unsigned int nb_nodes;


void nbr_n_e(char * ENTREE) {


  FILE * f_in;
  char line[SIZE_OF_LINE];
  
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  unsigned int i = 0, j=0;
  unsigned int nbr_e = 0;
  unsigned int max = 0;

  /**Get node number to allocate the adjacency matrix**/
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    unsigned int m = MAX(i, j);
    
    if (m > max) {
      max = m;
    }
    nbr_e++;
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
  fclose(f_in);
  printf("%d %d\n",nb_nodes,nbr_e);


}


int * degree(char * ENTREE) {

  FILE * f_in;
  char line[SIZE_OF_LINE];
  
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
  }
  unsigned int i = 0, j=0;
  unsigned int nbr_e = 0;
  unsigned int max = 0;

  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    sscanf(line, "%u %u", & i, & j);
    unsigned int m = MAX(i, j);
    
    if (m > max) {
      max = m;
    }
    nbr_e++;
  }

  int tab1[max];
  int a;
  int b;
  int pos;
  int m;
  int s;
  int x;
  int y;



  for (i = 0; i <= max; i++) {
    tab1[i] = 0;

  }

  fseek(f_in, 0, SEEK_SET);

  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
    tab1[a] = tab1[a] + 1;
	tab1[b] = tab1[b] + 1;
  }
 
  for (i = 0; i <= max; i++) {
    //printf("%d ",tab1[i]);
  }


  for (i = 0; i < max; i++) {
	if(tab1[i] != 0){
    	printf("node = %d, degree = %d \n", i, tab1[i]);
	}

  }

  fseek(f_in, 0, SEEK_SET);
  s = 0;
  m = 0;
  while (fgets(line, 81, f_in) != NULL) {
    sscanf(line, "%d %d", & a, & b);
  	x = tab1[a];
    y = tab1[b];
	m = x * y;
    s = s + m;
   // printf("degree a = %d, %d , degree b = %d m = %d s = %d \n", tab1[a],b, tab1[b],m,s);
  }
  printf("Special quant %d",s);

/*
  int *tab2[max];
  for (i = 0; i < max; i++) {
	//printf("%d\n",tab1[i]);
   if ( find_elem_pos(tab2,max,tab1[i])==-1){
	 if (find_elem_pos(tab1,max,tab1[i])!=-1){
		printf("%d, %d\n",tab1[i],find_elem_pos(tab1,max,tab1[i]));
		tab2[i] = tab1[i];
	  }
	}
  }*/

  return tab1;

}

int main() {

  //degree("emails.txt");
 // distribution("emails.txt");
 // printf("%d", special_quantity("emails.txt"));
  degree("com-amazon.ungraph.txt");
  //sscanf("23 24","%d %d",&a,&b);
  //printf("%d",a);
  //printf("%d",b);
  return 0;
}
