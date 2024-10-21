// AVL tree implementation in C

#include <stdio.h>
#include <stdlib.h>

#define NODE_ALLOC malloc

// Create Node
typedef struct Node {
  int key;
  struct Node *left;
  struct Node *right;
  int height;
}Node;

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
Node *newNode(int key) {
  Node *node = NODE_ALLOC(sizeof(Node));
  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;
  return node;
}

// Right rotate
struct Node *_rightRotate(Node *y) {

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
Node *insertNode(Node *node, int key) {
  // Find the correct position to insertNode the node and insertNode it
  if (node == NULL)
    return (newNode(key));

  if (key < node->key)
    node->left = insertNode(node->left, key);
  else if (key > node->key)
    node->right = insertNode(node->right, key);
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
      } else
        *root = *temp;
      free(temp);
    } else {
      Node *temp = _minValueNode(root->right);

      root->key = temp->key;

      root->right = deleteNode(root->right, temp->key);
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

void destroyTree(Node *root){
    if(root != NULL){
        destroyTree(root->left);
        destroyTree(root->right);
        free(root);

    }
    return;

}


int main() {
  srand(4444);
  Node *root = NULL;
  for(int i = 0; i < 1000000; i++){
    //int key = rand()%10000 + i;
    root = insertNode(root, i);
  }
  //printTree(root);
  for(int i = 0; i < 1000000; i++){
    root = deleteNode(root, i);
    //printf("Tree %d\n\n", i);
    //printTree(root);
  }
  
  destroyTree(root);
  //int a;
  //scanf("%d", &a);
  return 0;
}
