#include<assert.h>
#include<stdio.h>
#include<stdlib.h>

#define NodeAlloc malloc


#define MAX(a, b) ((a) > (b)) ? (a)  :  (b)

#define HEIGHT(n)  ((n) == NULL) ? 0 : (n)->height

#define BFACTOR(n)  ((n) == NULL) ? 0 : ((HEIGHT(n->left)) - (HEIGHT(n->right)))

typedef struct Node {
	struct Node* left;
	struct Node* right;
	int key;
	int height;
	char* value;
	} Node;

Node* newNode(int key) {
	Node* n = NodeAlloc(1*sizeof(Node*));
	n->key = key;
	n->left = NULL;
	n->right = NULL;
	n->height = 1;

	return n;
	}




Node* _rightRotate(Node* n) {
	Node* x = n->left;
	Node* temp = n->right;
	x->right = n;
	n->left = temp;
	n->height = MAX(HEIGHT(n->left), HEIGHT(n->right)) + 1;
	x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1;

	return x;

	}

Node* _leftRotate(Node* n) {
	Node* x = n->right;
	Node* temp = n->left;
	x->left = n;
	n->right = temp;
	n->height = MAX(HEIGHT(n->left), HEIGHT(n->right)) + 1;
	x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1;

	return x;

	}

Node* insertNode(Node* n, int key) {
	if (n == NULL) {
		return newNode(key);
		}

	if(key < n->key) {
		n->left = insertNode(n->left, key);
		}
	else if(key > n->key) {
		n->right = insertNode(n->right, key);
		}

	else {
		return n;
		}

	n->height = 1 + MAX(HEIGHT(n->left), HEIGHT(n->right));
	//TODO static nop lets compiler
	int balance = BFACTOR(n);
	if (balance > 1 && key < n->left->key)
		return _rightRotate(n);

	if (balance < -1 && key > n->right->key)
		return _leftRotate(n);

	if (balance > 1 && key > n->left->key) {
		n->left = _leftRotate(n->left);
		return _rightRotate(n);
		}

	if (balance < -1 && key < n->right->key) {
		n->right = _rightRotate(n->right);
		return _leftRotate(n);
		}

	return n;
	}


Node* minValueNode(Node *n) {
  Node *current = n;

  while (current->left != NULL)
    current = current->left;

  return current;
}
Node* deleteNode(Node* n, int key)
{
    // STEP 1: PERFORM STANDARD BST DELETE

    if (n == NULL)
        return n;

    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if ( key < n->key )
        n->left = deleteNode(n->left, key);

    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if( key > n->key )
        n->right = deleteNode(n->right, key);

    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if( (n->left == NULL) || (n->right == NULL) )
        {
          Node *temp = n->left ? n->left : n->right;

            // No child case
            if (temp == NULL)
            {
                temp = n;
                n = NULL;
            }
            else // One child case
             *n = *temp; // Copy the contents of
                            // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            Node* temp = minValueNode(n->right);

            // Copy the inorder successor's data to this node
            n->key = temp->key;

            // Delete the inorder successor
            n->right = deleteNode(n->right, temp->key);
        }
    }

    // If the tree had only one node then return
    if (n == NULL)
      return n;

		//return root;

  n->height = 1 + MAX(HEIGHT(n->left), HEIGHT(n->right));
	//TODO static nop lets compiler
	int balance = BFACTOR(n);
	if (balance > 1 && key < n->left->key)
		return _rightRotate(n);

	if (balance < -1 && key > n->right->key)
		return _leftRotate(n);

	if (balance > 1 && key > n->left->key) {
		n->left = _leftRotate(n->left);
		return _rightRotate(n);
		}

	if (balance < -1 && key < n->right->key) {
		n->right = _rightRotate(n->right);
		return _leftRotate(n);
		}

	return n;
}// Print the tree
void printPreOrder(Node *root) {
  if (root != NULL) {
    printf("%d ", root->key);
    printPreOrder(root->left);
    printPreOrder(root->right);
  }
}




int main() {
	Node *root = NULL;

  root = insertNode(root, 24);
  root = insertNode(root, 13);
  root = insertNode(root, 7);
  root = insertNode(root, 4);
  root = insertNode(root, 51);
  root = insertNode(root, 33);
  root = insertNode(root, 81);

  printPreOrder(root);

  root = deleteNode(root, 7);

  printf("\nAfter deletion: ");
  printPreOrder(root);


	}
