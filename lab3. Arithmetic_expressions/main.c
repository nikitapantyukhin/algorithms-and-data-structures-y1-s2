#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NodeOR, // операция
    NodeVAR, // переменная
    NodeValue // значение
} NodeType;

typedef struct Node {
    NodeType type;
    char op; // знак операции
    char var_name; // имя переменной
    double value; // значение
    struct Node *left;
    struct Node *right;
} Node;

Node* createNode(NodeType t) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        exit(1);
    }

    newNode->type = t;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->op = 0;
    newNode->var_name = 0;
    newNode->value = 0;
    return newNode;
}

const char* expr;

void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void printTree(Node* root, int level) {
    if (root == NULL) return;

    printTree(root->right, level + 1);
    
    for (int i = 0; i < level; i++) {
        printf("    ");
    }

    if (root->type == NodeOR) {
        printf("[%c]\n", root->op);
    }
    else if (root->type == NodeVAR) {
        printf("(%c)\n", root->var_name);
    }
    else if (root->type == NodeValue) {
        printf("%.2f\n", root->value);
    }

    printTree(root->left, level + 1);
}

void printExpression(Node* root) {
    if (root == NULL) {
        return;
    }

    if (root->type == NodeOR) {
        printf("(");
    }

    printExpression(root->left);

    if (root->type == NodeOR) {
        printf("%c", root->op);
    }
    else if (root->type == NodeVAR) {
        printf("%c", root->var_name);
    }
    else if (root->type == NodeValue) {
        printf("%.2f", root->value);
    }

    printExpression(root->right);

    if (root->type == NodeOR) {
        printf(")");
    }
}

void simplify(Node* root) {
    if (root == NULL) {
        return;
    }

    simplify(root->left);
    simplify(root->right);

    if (root->type == NodeOR && root->op == '*') {
        Node* L = root->left;
        Node* R = root->right;

        if (L != NULL && R != NULL && 
            L->type == NodeOR && L->op == '^' && R->type == NodeOR && R->op == '^') {
            if (L->left != NULL && R->left != NULL && L->left->type == NodeVAR && R->left->type == NodeVAR && \
                L->left->var_name == R->left->var_name) {
                    root->op = '^';

                Node* base = L->left;
                Node* powL = L->right;
                Node* powR = R->right;
                Node* baseR = R->left;

                root->op = '^';
                Node* NewPlus = createNode(NodeOR);
                NewPlus->op = '+';
                NewPlus->left = powL;
                NewPlus->right = powR;

                root->left = base;
                root->right = NewPlus;

                free(L); 
                free(R);
                freeTree(baseR);
            }
        }
    }
}

void skipSpaces() {
    while (*expr == ' ' || *expr == '\t') {
        expr++;
    }
}

Node* parseExpression();

Node* parseFactor() {
    skipSpaces();

    if (*expr >= '0' && *expr <= '9') {
        Node* n = createNode(NodeValue);
        n->value = strtod(expr, (char**)&expr);
        return n;
    }
    if (*expr >= 'a' && *expr <= 'z') {
        Node* n = createNode(NodeVAR);
        n->var_name = *expr;
        expr++;
        return n;
    }
    if (*expr == '(') {
        expr++;
        Node* n = parseExpression();
        skipSpaces();
        if (*expr == ')') {
            expr++;
        }
        return n;
    }

    return NULL;
}

Node* parsePower() {
    Node* left = parseFactor();
    skipSpaces();

    if (*expr == '^') {
        Node* root = createNode(NodeOR);
        root->op = '^';
        root->left = left;
        expr++;
        root->right = parsePower();
        return root;
    }
    return left;
}

Node* parseTerm() {
    Node* left = parsePower();
    skipSpaces();

    while (*expr == '*' || *expr == '/') {
        Node* root = createNode(NodeOR);
        root->op = *expr;
        expr++;
        root->left = left;
        root->right = parsePower();
        left = root;
        skipSpaces();
    }

    return left;
}

Node* parseExpression() {
    Node* left = parseTerm();
    skipSpaces();

    while (*expr == '+' || *expr == '-') {
        char op = *expr;
        expr++;
        Node* root = createNode(NodeOR);
        root->op = op;
        root->left = left;
        root->right = parseTerm();
        left = root;
        skipSpaces();
    }
    return left;
}

int main() {
    char input[100];
    printf("Введите выражение: ");
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\r\n")] = 0;
    }

    expr = input;
    Node* root = parseExpression();

    printf("Дерево до упрощения: \n");
    printTree(root, 0);

    simplify(root);

    printf("Деверо после упрощения: \n");
    printTree(root, 0);

    printf("Результат: ");
    printExpression(root);
    printf("\n");

    freeTree(root);

    return 0;
}