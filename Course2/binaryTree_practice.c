#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    char data;
    struct node *left;
    struct node *right;
} Node;

int main() {
    Node *root = (Node *)malloc(sizeof(Node));
    Node *leftChild = (Node *)malloc(sizeof(Node));
    Node *rightChild = (Node *)malloc(sizeof(Node));

    root->data = 'M';
    root->left = leftChild;
    root->right = rightChild;

    leftChild->data = 'X';
    leftChild->left = NULL;
    leftChild->right = NULL;

    rightChild->data = 'Q';
    rightChild->left = NULL;
    rightChild->right = NULL;

    printf("%c%c", root->data, root->right->data);

    return 0;
}


