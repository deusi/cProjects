#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

// tree main implemented similarly to list main from the lab exercise

int main(int argc, char *argv[]){
  int echo = 0;
  if(argc > 1 && strcmp("-echo",argv[1])==0) {  // check if echo is enabled
    echo=1;
  }
  // prints the command list
  printf("BST Demo\n");
  printf("Commands:\n");
  printf("  print:          shows contents of the tree in reverse sorted order\n");
  printf("  clear:          eliminates all elements from the tree\n");
  printf("  quit:           exit the program\n");
  printf("  add <name>:     inserts the given string into the tree, duplicates ignored\n");
  printf("  find <name>:    prints FOUND if the name is in the tree, NOT FOUND otherwise\n");
  printf("  preorder:       prints contents of the tree in pre-order which is how it will be saved\n");
  printf("  save <file>:    writes the contents of the tree in pre-order to the given file\n");
  printf("  load <file>:    clears the current tree and loads the one in the given file\n");

  char cmd[128];
  bst_t tree;
  int success;
  bst_init(&tree);  //initialize the tree

  // uses strcmp to check user input for commands given above
  while(1){
    printf("BST> ");                 // print prompt
    success = fscanf(stdin,"%s",cmd); // read a command
    if(success==EOF){                 // check for end of input
      printf("\n");                   // found end of input
      break;                          // break from loop
    }

    if( strcmp("quit", cmd)==0 ){     // check for exit command
      if(echo){
        printf("quit\n");
      }
      break;                          // break from loop
    }

    else if( strcmp("add", cmd)==0 ){ // insertion
      fscanf(stdin,"%s",cmd);            // read string to insert
      if(echo){
        printf("add %s\n",cmd);
      }

      success = bst_insert(&tree, cmd); // call tree function
      if(!success){                      // check for success
        printf("add failed\n");
      }
    }

    else if( strcmp("clear", cmd)==0 ){   // clear command
      if(echo){
        printf("clear\n");
      }
      bst_clear(&tree);
    }

    // finds if the name is present in the tree
    else if( strcmp("find", cmd)==0){
      char name[128];
      fscanf(stdin,"%s", name);
      if(echo){
        printf("find %s\n", name);
      }
      if(bst_find(&tree, name)){
        printf("FOUND\n");
      }
      else{
        printf("NOT FOUND\n");
      }
    }

    else if( strcmp("print", cmd)==0 ){   // print command
      if(echo){                       // prints tree in revorder
        printf("print\n");
      }
      bst_print_revorder(&tree);
    }

    else if( strcmp("preorder", cmd)==0 ){   // prints the tree in preorder
      if(echo){
        printf("preorder\n");
      }
      bst_print_preorder(&tree);
    }

    else if( strcmp("save", cmd)==0){   // saves the tree in a specified filename
      char name[128];
      fscanf(stdin,"%s", name);
      if(echo){
        printf("save %s\n", name);
      }
      bst_save(&tree, name);
    }

    else if( strcmp("load", cmd)==0){   // load the tree from the file
      char name[128];
      fscanf(stdin,"%s", name);
      if(echo){
        printf("load %s\n", name);
      }
      bst_load(&tree, name);
    }

    else{                                 // unknown command
      if(echo){
        printf("%s\n",cmd);
      }
      printf("Unknown command \'%s\'\n",cmd);
    }
  }

  // end main while loop
  bst_clear(&tree); //clear the tree after end of the loop

  return 0;
}
