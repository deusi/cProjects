#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int *read_text_deltas(char *fname, int *len) {
  // opening a file to read based on name provided by user
  FILE *fp = NULL;
  fp = fopen(fname, "r");
  // checking if the given file exists
  // if no, set len to -1 and return NULL pointer
  if(!fp){
    *len = -1;
    return NULL;
  }
  // looping through the file to check how many integers are there
  // int check is just for storing a temporary variable from fscanf()
  int count = 0;
  int check = 0;
  while(fscanf(fp, "%d", &check) != EOF){
    count++;
  }
  // checks is the file is empty
  // sets length to -1 and returns NULL is so
  if(count == 0){
    *len = -1;
    fclose(fp);
    return NULL;
  }
  // sets len to the number of elements in the file and creates a new array
  // of that length
  *len = count;
  int *arr = malloc(count*sizeof(int));
  // puts the pointer to the beginning of the array
  rewind(fp);
  int temp = 0;
  // using fscanf() in a loop to set the first element of an array to be
  // equal to the first integer in the file and then doing arithmetic
  // operations on all the following elements of the array
  // e.g. if the first int in the file is 20 and the second is -5, then
  // 1st element of the array will be 20 and second 15
  for(int i = 0; i < count; i++) {
    if(i == 0) {
      fscanf(fp, "%d", arr);
      continue;
    }
    fscanf(fp, "%d", &temp);
    // set the next element to be the previous element plus the current element
    // in the file
    arr[i] = arr[i-1] + temp;
  }
  // closes the file and sets its pointer to NULL
  fclose(fp);
  fp = NULL;
  return arr;
}

int *read_int_deltas(char *fname, int *len) {
  // opening a file to read based on name provided by user
  FILE *fp = NULL;
  fp = fopen(fname, "r");
  // checking if the given file exists
  // if no, set len to -1 and return NULL pointer
  if(!fp){
    *len = -1;
    return NULL;
  }
  // using sruct to determine the size of named file
  //  if something went wrong or size of file is smaller then one int,
  // set len to -1, close the file and return NULL
  struct stat sb;
  int result = stat(fname, &sb);
  if(result==-1 || sb.st_size < sizeof(int)){
    *len = -1;
    fclose(fp);
    return NULL;
  }
  // size of file in bytes
  int total_bytes = sb.st_size;
  // size of file in int capacity
  int total_int = total_bytes/sizeof(int);
  // set len to the total number of ints
  *len = total_int;
  // allocate new array of the size of total amount of bytes
  int *arr = malloc(total_bytes);
  // similar to the text array
  // allocate the first element and then do arithmetic on all the following ones
  int temp = 0;
  for(int i = 0; i < total_int; i++) {
    if(i == 0) {
      // fread() is being used to read binary data from fp,
      // interpret it, and then put it into array
      fread(&arr[i], sizeof(int), 1, fp);
      continue;
    }
    fread(&temp, sizeof(int), 1, fp);
    arr[i] = arr[i-1] + temp;
  }
  // closes the file and sets the pointer to NULL
  fclose(fp);
  fp = NULL;
  return arr;
}
  // testament to my laziness
int *read_4bit_deltas(char *fname, int *len) {
  return len;
}
