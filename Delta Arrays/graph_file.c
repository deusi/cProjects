#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deltas.h"

// the bain is copied from deltas_main.c

// 4bit is not implemented, hence removed
int main(int argc, char *argv[]){
  if(argc < 4){
    printf("usage: %s <format> <filename> <height>\n",argv[0]);
    printf(" <format> is one of\n");
    printf(" text : text ints are in the given filename\n");
    printf(" int  : binary ints are in the given filename\n");
    //printf(" 4bit : 4bit binary ints are in the given filename\n");
    return 1;
  }
  char *format = argv[1];
  char *fname = argv[2];
  // converts string to an integer
  int height = atoi(argv[3]);

  // similar to part 1
  // checks the input and uses text or int function (or prints unknown)
  // saves the data in an int array
  int data_len = -1;
  int *data_vals = NULL;
  if( strcmp("text", format)==0 ){
    printf("Reading text format\n");
    data_vals = read_text_deltas(fname, &data_len);
  }
  else if( strcmp("int", format)==0 ){
    printf("Reading binary int format\n");
    data_vals = read_int_deltas(fname, &data_len);
  }
  //else if( strcmp("4bit", format)==0 ){
  //  printf("Reading 4bit binary int format\n");
  //  data_vals = read_4bit_deltas(fname, &data_len);
  //}
  else{
    printf("Unknown format '%s'\n",format);
    return 1;
  }
  // uses the data obtained from the previous part to build the graph
  print_graph(data_vals, data_len, height);
  // frees an integer array (since malloc() is being used in called functions)
  free(data_vals);
  return 0;
}
