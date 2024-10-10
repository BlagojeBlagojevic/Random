#include<stdio.h>
#include<assert.h>
#include<stdlib.h>

#define DB_ALLOC  malloc
#define DB_ASSERT assert

typedef struct node{
	
	struct node *left;
	struct node *right;
	char *record;

	
}Node;

Node* Create_Node(const char* record){
	
	Node* node = DB_ALLOC(sizeof(Node));
	node->left = NULL;
	node->right = NULL;
	node->record = record;
	
	return node;
}


#define MAX_DEPTH 100
//In memory insertion
void Insert_Record(Node **root, const char* record){
	
	//CHECK IS EMPTY
	Node* newNode = Create_Node(record);
	if(*root == 0){
		*root = newNode;
		return;
	}	
	
	//
	Node *temp;
	Node *queue[MAX_DEPTH];
	int front = -1, rear = -1;
	queue[++rear] = *root;
	while(front != rear){
		temp = queue[++front];
		//IF LEFT EMPTY
		if(temp->left == NULL){
			
			temp->left = newNode;
			return;
		}
		else{
			queue[++rear] = temp->left;
		}
		//IF RIGHT EMPTY
		if(temp->right == NULL){
			
			temp->right = newNode;
			return;
		}
		else{
			queue[++rear] = temp->right;
		}
		
	}
	
}

Node* Right_Node(Node *root)
{
    Node* temp = NULL;
    Node* queue[100];
    int front = -1, rear = -1;
    queue[++rear] = root;

    while (front != rear) {
        temp = queue[++front];

        if (temp->left != NULL) {
            queue[++rear] = temp->left;
        }

        if (temp->right != NULL) {
            queue[++rear] = temp->right;
        }
    }
    return temp;
}
 
 void Delete_Right(Node* root, Node* dNode)
{
    Node* temp;
    Node* queue[100];
    int front = -1, rear = -1;
    queue[++rear] = root;

    while (front != rear) {
        temp = queue[++front];

        if (temp == dNode) {
            temp = NULL;
            free(dNode);
            return;
        }

        if (temp->right != NULL) {
            if (temp->right == dNode) {
                temp->right = NULL;
                free(dNode);
                return;
            }
            else {
                queue[++rear] = temp->right;
            }
        }

        if (temp->left != NULL) {
            if (temp->left == dNode) {
                temp->left = NULL;
                free(dNode);
                return;
            }
            else {
                queue[++rear] = temp->left;
            }
        }
    }
}
 
 
 
Node* Left_Node(Node *root)
{
    Node* temp = NULL;
    Node* queue[100];
    int front = -1, rear = -1;
    queue[++rear] = root;

    while (front != rear) {
        temp = queue[++front];

				if (temp->right != NULL) {
            queue[++rear] = temp->right;
        }
        
				if (temp->left != NULL) {
            queue[++rear] = temp->left;
        }

        
    }
    return temp;
}

void Delete (Node** root, char *record)
{
    if (*root == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    if ((*root)->left == NULL && (*root)->right == NULL) {
        if ((*root)->record == record) {
            free(*root);
            *root = NULL;
            return;
        }
        else {
            printf("Node not found.\n");
            return;
        }
    }

    Node* temp;
    Node* queue[100];
    int front = -1, rear = -1;
    queue[++rear] = *root;
    Node* keyNode = NULL;

    while (front != rear) {
        temp = queue[++front];

        if (temp->record == record) {
            keyNode = temp;
        }

        if (temp->left != NULL) {
            queue[++rear] = temp->left;
        }

        if (temp->right != NULL) {
            queue[++rear] = temp->right;
        }
    }

    if (keyNode != NULL) {
        Node* deepestNode = Right_Node(*root);
        keyNode->record = deepestNode->record;
        Delete_Right(*root, deepestNode);
    }
    else {
        printf("Node not found.\n");
    }
}





void Print_Tree(Node* tree){
	if(tree == NULL){
		return;
	}
	printf("%s", tree->record);
	Print_Tree(tree->left);
	
	Print_Tree(tree->right);
	
}

int main(){
	Node *tree = Create_Node("Nesto\n");
	for(int i = 0; i < 100; i++){
		Insert_Record(&tree, "Nesto");
	}
	//Delete(&tree, a);
	//Node *print = Left_Node(tree);
	Print_Tree(tree);
	
}
