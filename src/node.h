#include <stdbool.h>
#include <stdlib.h>

#include "commands.h"

#ifndef NODE_H
#define NODE_H

#define MAX_NODES 128

typedef int node_id_t;

typedef struct Node {
    node_id_t id;
    pid_t pid;
} node_t;

#endif // NODE_H




















// typedef struct Tree {
//     size_t size;
//     node_t *data;
// } tree_t;

// tree_t *init(int size);

// node_id_t find(tree_t *tree, node_id_t node_id);

// int insert_node(tree_t *tree, node_t node);

// bool is_exist(tree_t *tree, node_id_t node_id);

// void print_tree(tree_t *tree);

// int find_parent(tree_t *tree, node_id_t node_id);


// useless
// int remove_node(tree_t node);
