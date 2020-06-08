//
// Created by 徳備彩人 on 2020/06/08.
//

#ifndef INC_9CC_NODE_H
#define INC_9CC_NODE_H

#include <stdlib.h>
#include "token.h"

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *left;
    Node *right;
    int val;
};

Node *new_node(NodeKind, Node*, Node*);
Node *new_node_num(int val);
Node *expr();
Node *mul();
Node *primary();
Node *unary();

#endif //INC_9CC_NODE_H
