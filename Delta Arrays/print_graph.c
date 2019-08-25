#include <stdio.h>

// separate function to print upper and lower parts of the print_graph
// not necessary, but being used to avoid repetition
void printFancyLine(int len);

void print_graph(int *data, int len, int max_height) {

  // Initialize min and max to the first value in the array,
  // in case if array consists of negative numbers.
  int min = data[0];
  int max = data[0];
  // loop through the array in order to find the min and max values
  for(int i = 1; i < len; i++){
    if(data[i] > max){
      max = data[i];
    }
    if(data[i] < min){
      min = data[i];
    }
  }
  // calculating range and units per height according to their definitions
  // (and some mathematical common sense)
  int range = max - min;
  double units_per_height = (double) range / max_height;
  // printing the general information
  printf("length: %d\n", len);
  printf("min: %d\n", min);
  printf("max: %d\n", max);
  printf("range: %d\n", range);
  printf("max_height: %d\n", max_height);
  printf("units_per_height: %.2f\n", units_per_height);
  // printing the fancy line
  printFancyLine(len);
  // using double loop to print the main body of the graph
  for(int i = max_height; i >= 0; i--){
    // cutoff represents all the marked points on y axis
    // used for reference and depends on units per height and min
    int cutoff = (int) min + i * units_per_height;
    printf("%3d |", cutoff);
    for(int j = 0; j < len; j++){
      // if the value in the array is greater then cuttoff, print 'X'
      if(data[j] >= cutoff){
        printf("X");
      }
      // otherwise, empty space
      else{
        printf(" ");
      }
    }
    printf("\n");
  }
  // fancy line again
  printFancyLine(len);
  int index = 0;
  printf("     ");
  // print the values along x axis
  // do-while loop guarantees that 0 will always be printed
  do{
    printf("%-5d", index);
    index+=5;
  }
  while((index % len) != 0 && len > index);

  printf("\n");
  return;
}

void printFancyLine(int len){
  // prints the beginning and ending line, which depends on length of the function
  printf("     ");
  int repeat = 0;
  while(repeat < len){
    // print '+' once in every five iterations
    if((repeat % 5) == 0){
      printf("+");
      repeat++;
      continue;
    }
    // print '-' otherwise
    printf("-");
    repeat++;
  }
  printf("\n");

  return;
}
