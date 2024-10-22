#ifndef AVL_TREE_H
#define AVL_TREE_H


#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define NODE_MEMCPY memcpy
#define NODE_STRLEN strlen


#define NODE_ALLOC malloc

#define MAX_SIZE_OF_VALUE 1024

#define VALUE_CHECK(size) if(size >=  MAX_SIZE_OF_VALUE) size = MAX_SIZE_OF_VALUE 



#include<stdint.h>
//KEY IS GLOBAL STATE
uint64_t key = 0;


// Create Node
typedef struct Node {
  struct Node *left;
  struct Node *right;
  uint64_t key;
	int height;
  char value[MAX_SIZE_OF_VALUE];
}Node;


Node *newNode(int key, char* value, size_t size);
//Node *insertNode(Node *node, int key, char* value, size_t size);
//Node *deleteNode(Node *root, int key);
void printTree(Node *root);
void printValueTree(Node* root);
void destroyTree(Node *root);

//USE THIS MACROS
#define INSERT_NODE(...)   insertNode(__VA_ARGS__); key++;
#define DELETE_NODE(...)   deleteNode(__VA_ARGS__); key--;


#ifdef AVL_TREE_IMPLEMETATION
// Calculate height
static inline int _height(Node *N) {
  if (N == NULL)
    return 0;
  return N->height;
}

static inline int _max(int a, int b) {
  return (a > b) ? a : b;
}





// Create a node
Node *newNode(int key, char* value, size_t size) {

  Node *node = NODE_ALLOC(sizeof(Node));
  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;
	VALUE_CHECK(size);
	//LET BUFFER BE FULL
  //node->value = NODE_ALLOC(sizeof(char) * MAX_SIZE_OF_VALUE);
  //assert(strlen(value) >= MAX_SIZE_OF_VALUE);
	NODE_MEMCPY(node->value, value, sizeof(char) * size);
  return node;
}

// Right rotate
Node *_rightRotate(Node *y) {

  //Node *x = NODE_ALLOC(sizeof(Node*)); 
  //Node *T2 = NODE_ALLOC(sizeof(Node*));
  Node *x = y->left;
  Node *T2 = x->right;

  x->right = y;
  y->left = T2;

  y->height = _max(_height(y->left), _height(y->right)) + 1;
  x->height = _max(_height(x->left), _height(x->right)) + 1;
  //free(T2);
  return x;
}

// Left rotate
Node *_leftRotate(Node *x) {
  //Node *y = NODE_ALLOC(sizeof(Node*)); 
  //Node *T2 = NODE_ALLOC(sizeof(Node*));
  Node *y = x->right;
  Node *T2 = y->left;

  y->left = x;
  x->right = T2;

  x->height = _max(_height(x->left), _height(x->right)) + 1;
  y->height = _max(_height(y->left), _height(y->right)) + 1;
  //free(T2);
  return y;
}

// Get the balance factor
int _getBalance(Node *N) {
  if (N == NULL)
    return 0;
  return _height(N->left) - _height(N->right);
}

// Insert node
Node *insertNode(Node *node, char* value, size_t size) {
  // Find the correct position to insertNode the node and insertNode it
  if (node == NULL)
    return (newNode(key, value ,size));

  if (key < node->key)
    node->left = insertNode(node->left, value, size);
  else if (key > node->key)
    node->right = insertNode(node->right, value, size);
  else
    return node;

  // Update the balance factor of each node and
  // Balance the tree
  node->height = 1 + _max(_height(node->left), _height(node->right));

  const int balance = _getBalance(node);
  if (balance > 1 && key < node->left->key)
    return _rightRotate(node);

  if (balance < -1 && key > node->right->key)
    return _leftRotate(node);

  if (balance > 1 && key > node->left->key) {
    node->left = _leftRotate(node->left);
    return _rightRotate(node);
  }

  if (balance < -1 && key < node->right->key) {
    node->right = _rightRotate(node->right);
    return _leftRotate(node);
  }

  return node;
}

Node *_minValueNode(Node *node) {
  Node *current = node;

  while (current->left != NULL)
    current = current->left;

  return current;
}

// Delete a nodes
Node *deleteNode(Node *root, int key) {
  // Find the node and delete it
  if (root == NULL)
    return root;

  if (key < root->key)
    root->left = deleteNode(root->left, key);

  else if (key > root->key)
    root->right = deleteNode(root->right, key);

  else {
    if ((root->left == NULL) || (root->right == NULL)) {
      Node *temp = root->left ? root->left : root->right;
			
      if (temp == NULL) {
        temp = root;
        root = NULL;
        //
      } else
        *root = *temp;
      
			//FREE resources
			//free(temp->value);
      free(temp);
    } else {
      Node *temp = _minValueNode(root->right);

      root->key = temp->key;
			//root->value = temp->value;
      root->right = deleteNode(root->right, temp->key);
      free(temp);
    }
  }

  if (root == NULL)
    return root;

  // Update the balance factor of each node and
  // balance the tree
  root->height = 1 + _max(_height(root->left), _height(root->right));

  const int balance = _getBalance(root);
  if (balance > 1 && _getBalance(root->left) >= 0)
    return _rightRotate(root);

  if (balance > 1 && _getBalance(root->left) < 0) {
    root->left = _leftRotate(root->left);
    return _rightRotate(root);
  }

  if (balance < -1 && _getBalance(root->right) <= 0)
    return _leftRotate(root);

  if (balance < -1 && _getBalance(root->right) > 0) {
    root->right = _rightRotate(root->right);
    return _leftRotate(root);
  }

  return root;
}

// Print the tree
void printTree(Node *root) {
  if (root != NULL) {
    printf("%d ", root->key);
    printTree(root->left);
    printTree(root->right);
  }
}

void printValueTree(Node* root){
	if (root != NULL) {
    printf("%s ", root->value);
    printValueTree(root->left);
    printValueTree(root->right);
  }
}


void destroyTree(Node *root){
    if(root != NULL){
        destroyTree(root->left);
        destroyTree(root->right);
        free(root->value);
				free(root);
				
    }
    return;

}







#endif
#endif
