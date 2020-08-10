//
// Created by 徳備彩人 on 2020/06/08.
//

#ifndef INC_9CC_PARSE_H
#define INC_9CC_PARSE_H

#include <stdlib.h>
#include "token.h"

typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // =
    ND_LVAR, // local var
    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *left;
    Node *right;
    int val;  // used only if integer
    int offset; // used only if variable
};

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

LVar *locals;

#define CODE_LENGTH 100
Node *code[CODE_LENGTH];

Node *new_node(NodeKind, Node*, Node*);
Node *new_node_num(int val);
Node *primary();
Node *unary();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *expr();
void program();

#endif //INC_9CC_PARSE_H