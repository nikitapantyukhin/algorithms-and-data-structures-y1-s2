#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_MAX_LEN 6

enum node_colors { RED, BLACK };

typedef struct Node {
    double value;
    char key[KEY_MAX_LEN + 1];
    enum node_colors color;
    struct Node* parent;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct {
    Node* root;
    Node* nil;
} RBTree;

// Создание дерева и инициализация пустого листа
RBTree* init_Tree() {
    RBTree* tree = (RBTree*)malloc(sizeof(RBTree));
    tree->nil = (Node*)malloc(sizeof(Node));
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil->right = tree->nil->parent = NULL;
    tree->root = tree->nil;
    return tree;
}

// Выделение памяти под новый узел
Node* init_Node(RBTree* tree, double value, const char* key, enum node_colors color) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;
    node->value = value;
    strncpy(node->key, key, KEY_MAX_LEN);
    node->key[KEY_MAX_LEN] = '\0';
    node->color = color;
    node->parent = tree->nil;
    node->left = tree->nil;
    node->right = tree->nil;
    return node;
}

// Поиск дедушки
Node* grandparent(Node* node, Node* nil) {
    if (node != nil && node->parent != nil) {
        return node->parent->parent;
    }
    return nil;
}

// Левый поворот вокруг узла x
void rotate_left(RBTree* tree, Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// Правый поворот вокруг узла x
void rotate_right(RBTree* tree, Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != tree->nil) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

// Балансировка после вставки (цвета и повороты)
void insert_rebalance(RBTree* tree, Node* z) {
    while (z->parent->color == RED) {
        Node* g = grandparent(z, tree->nil);
        if (z->parent == g->left) {
            Node* y = g->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                g->color = RED;
                z = g;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotate_left(tree, z);
                }
                z->parent->color = BLACK;
                g->color = RED;
                rotate_right(tree, g);
            }
        } else {
            Node* y = g->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                g->color = RED;
                z = g;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right(tree, z);
                }
                z->parent->color = BLACK;
                g->color = RED;
                rotate_left(tree, g);
            }
        }
    }
    tree->root->color = BLACK;
}

// Добавление нового элемента
void insert_node(RBTree* tree, const char* key, double value, FILE* out) {
    Node* z = init_Node(tree, value, key, RED);
    Node* y = tree->nil;
    Node* x = tree->root;

    while (x != tree->nil) {
        y = x;
        int cmp = strcmp(z->key, x->key);
        if (cmp == 0) {
            x->value = value;
            free(z);
            if (out) fprintf(out, "Updated: %s\n", key);
            return;
        } else if (cmp < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;
    if (y == tree->nil) {
        tree->root = z;
    } else if (strcmp(z->key, y->key) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }

    insert_rebalance(tree, z);
    if (out) fprintf(out, "Inserted: %s\n", key);
}

// Вспомогательная функция для замены поддеревьев
void transplant(RBTree* tree, Node* u, Node* v) {
    if (u->parent == tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

// Поиск самого левого (минимального) узла
Node* tree_minimum(RBTree* tree, Node* x) {
    while (x->left != tree->nil) {
        x = x->left;
    }
    return x;
}

// Балансировка после удаления черного узла
void remove_rebalance(RBTree* tree, Node* x) {
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotate_left(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotate_right(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotate_left(tree, x->parent);
                x = tree->root;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotate_left(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotate_right(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = BLACK;
}

// Удаление узла по ключу
void remove_node(RBTree* tree, const char* key, FILE* out) {
    Node* z = tree->root;
    while (z != tree->nil) {
        int cmp = strcmp(key, z->key);
        if (cmp == 0) break;
        if (cmp < 0) z = z->left;
        else z = z->right;
    }

    if (z == tree->nil) {
        if (out) fprintf(out, "Not found\n");
        return;
    }

    Node* y = z;
    Node* x;
    enum node_colors y_original_color = y->color;

    if (z->left == tree->nil) {
        x = z->right;
        transplant(tree, z, z->right);
    } else if (z->right == tree->nil) {
        x = z->left;
        transplant(tree, z, z->left);
    } else {
        y = tree_minimum(tree, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK) {
        remove_rebalance(tree, x);
    }
    free(z);
    if (out) fprintf(out, "Deleted: %s\n", key);
}

// Обычный поиск по ключу
Node* find_node(RBTree* tree, const char* key) {
    Node* z = tree->root;
    while (z != tree->nil) {
        int cmp = strcmp(key, z->key);
        if (cmp == 0) return z;
        if (cmp < 0) z = z->left;
        else z = z->right;
    }
    return NULL;
}

// Рекурсивный вывод дерева с отступами
void print_tree(FILE* file_output, RBTree* tree, Node* root, int deep) {
    if (root == tree->nil) {
        if (deep == 0) fprintf(file_output, "Empty tree\n");
        return;
    }
    for (int i = 0; i < deep; i++) {
        fprintf(file_output, "|   ");
    }
    if (deep != 0) fprintf(file_output, "_");
    
    if (root->parent != tree->nil) {
        fprintf(file_output, (root == root->parent->left) ? "L: " : "R: ");
    }

    fprintf(file_output, "color: %s key: %s value: %lf\n", 
            (root->color == RED) ? "R" : "B", root->key, root->value);
    
    print_tree(file_output, tree, root->left, deep + 1);
    print_tree(file_output, tree, root->right, deep + 1);
}

// Рекурсивная очистка памяти
void delete_tree_nodes(RBTree* tree, Node* root) {
    if (root != tree->nil) {
        delete_tree_nodes(tree, root->left);
        delete_tree_nodes(tree, root->right);
        free(root);
    }
}

// Полное удаление дерева
void destroy_tree(RBTree* tree) {
    delete_tree_nodes(tree, tree->root);
    free(tree->nil);
    free(tree);
}

int main(void) {
    FILE* file_input = fopen("input.txt", "r");
    FILE* file_output = fopen("output.txt", "w");
    
    if (!file_input || !file_output) {
        if (file_input) fclose(file_input);
        if (file_output) fclose(file_output);
        return 1;
    }

    RBTree* tree = init_Tree();
    int command = 0;
    char key[KEY_MAX_LEN + 1];
    double value;

    // Читаем команды из файла, пока не дойдем до конца или команды 0
    while (fscanf(file_input, "%d", &command) == 1) {
        if (command == 0) break;
        
        switch (command) {
            case 1:
                fscanf(file_input, "%6s %lf", key, &value);
                fprintf(file_output, "1 %s %lf\n", key, value);
                insert_node(tree, key, value, file_output);
                break;
            case 2:
                fscanf(file_input, "%6s", key);
                fprintf(file_output, "2 %s\n", key);
                remove_node(tree, key, file_output);
                break;
            case 3:
                fprintf(file_output, "3\n");
                print_tree(file_output, tree, tree->root, 0);
                break;
            case 4:
                fscanf(file_input, "%6s", key);
                fprintf(file_output, "4 %s\n", key);
                Node* found = find_node(tree, key);
                if (found) {
                    fprintf(file_output, "Found: %s -> %lf\n", found->key, found->value);
                } else {
                    fprintf(file_output, "Key not exist\n");
                }
                break;
            default:
                fprintf(file_output, "Unknown command\n");
                break;
        }
        fprintf(file_output, "\n");
    }

    destroy_tree(tree);
    fclose(file_input);
    fclose(file_output);
    return 0;
}