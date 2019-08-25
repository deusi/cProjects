#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

// initialize a new bst by setting its size to zero and pointer to NULL
void bst_init(bst_t *tree){
  tree->root = NULL;
  tree->size = 0;
  return;
}

int bst_insert(bst_t *tree, char name[]){
  node_t *ptr = tree->root;
  node_t *iter = tree->root;
  // start at the root of the tree and compare the name value of node to given name
  // when the null node is found, the loop stops
  while(iter){
    // ptr points to the last node before null
    ptr = iter;
    // compares names to chose where to store the to-be inserted name
    if(strcmp(iter->name, name) > 0){
      iter = iter->left;
    }
    else if(strcmp(iter->name, name) < 0){
      iter = iter->right;
    }
    // if two previous cases are not true, then names are equal
    else{
      return 0;
    }
  }
  // create a new node, allocate memory and initialize variables
  node_t *new_node = malloc(sizeof(node_t));
  strcpy(new_node->name, name);
  // supposedly it is required to initialize left and right to point to null
  // gave me a lot of headaches before I was able to track it down
  new_node->left = NULL;
  new_node->right = NULL;
  // inserts a new node depending on where it belongs
  if(tree->size == 0){  // case of empty tree
    tree->root = new_node;
  }
  else if(strcmp(ptr->name, name) > 0){ // letf node
    ptr->left = new_node;
  }
  else{ //right node
    ptr->right = new_node;
  }
  tree->size++; // increment
  return 1;
}

int bst_find(bst_t *tree, char name[]){
  // similar to insert loop, but with reversed return values

  node_t *iter = tree->root;

  while(iter){
    if(strcmp(iter->name, name) == 0){
      return 1;
    }
    else if(strcmp(iter->name, name) < 0){
      iter = iter->right;
    }
    else{
      iter = iter->left;
    }
  }
  return 0;
}

void bst_clear(bst_t *tree){
  // calls a helper function, then sets size to 0 and root node to null
  node_remove_all(tree->root);
  tree->size = 0;
  tree->root = NULL;
  return;
}

void node_remove_all(node_t *cur){
  // if the node is already null, do nothing
  if(!cur){
    return;
  }

  node_t *ptr = cur;
  // otherwise, recursively call left and right subtrees
  if(ptr){
    node_remove_all(ptr->left);
    node_remove_all(ptr->right);
  }
  // set subtrees to zero and free the memory
  ptr->left = NULL;
  ptr->right = NULL;
  free(ptr);
  ptr = NULL; // set ptr to null, just in case
  return;
}

void bst_print_revorder(bst_t *tree){
  node_print_revorder(tree->root, 0);
  return;
}

void node_print_revorder(node_t *cur, int indent){
  // do nothing if the root points to null
  if(cur == NULL){
    return;
  }
  // recursive call to the right, then print, then recursive call to the left
  // does it up until the node is empty
  // increases indent after each call
  node_print_revorder(cur->right, indent+1);
  printf("%*s%s\n",2*indent, "", cur->name);
  node_print_revorder(cur->left, indent+1);
  return;
}

void bst_print_preorder(bst_t *tree){
  // passes stdout to print it on the terminal
  node_write_preorder(tree->root, stdout, 0);
  return;
}

void bst_save(bst_t *tree, char *fname){
  // opens the file and passes it as an argument to put the tree inside the file
  FILE *fp = NULL;
  fp = fopen(fname, "w");
  if(!fp){  // check is the file exists
    printf("File \'%s\' does not exists", fname);
    return;
  }
  node_write_preorder(tree->root, fp, 0);
  fclose(fp); // close the file after writing to it
  return;
}

void node_write_preorder(node_t *cur, FILE *out, int depth){
  if(cur == NULL){  // does nothing if node points to null
    return;
  }
  // print the node, then left subnode and right subnode
  fprintf(out, "%*s%s\n",2*depth, "", cur->name);
  node_write_preorder(cur->left, out, depth+1); // increase depth after each recursion
  node_write_preorder(cur->right, out, depth+1);
  return;
}

int bst_load(bst_t *tree, char *fname ){
  // open a new file for reading
  FILE *fp = NULL;
  fp = fopen(fname, "r");
  if(!fp){ // check if the file exists
    fclose(fp);
    return 0;
  }
  // clear the old tree
  bst_clear(tree);
  // insert each element from the file to the empty tree
  char str[128];
  while(fscanf(fp, "%s", str) != EOF){
    bst_insert(tree, str);
  }
  fclose(fp); // close the file
  return 1;
}
