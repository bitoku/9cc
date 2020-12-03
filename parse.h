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
    ND_RETURN, // return
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCCALL,
} NodeKind;

typedef struct NodeList NodeList;
typedef struct Node Node;

struct Node;
struct NodeList;

struct NodeList {
    NodeList *next;
    Node *node;
};

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};


struct Node {
    NodeKind kind;
    Node *left;
    Node *right;
    int val;  // used only if "integer"
    LVar *lvar; // used only if "variable"
    Node *init; // used if "for"
    Node *loop; // used if "for"
    Node *condition; // used if "for", "while", "if"
    Node *main_statement; // used if "for", "while", "if"
    Node *alt_statement; // used if "if"
    NodeList *statements; // used if "block"
    char *funcname; // function call
    NodeList *args; // function call
};

typedef struct Function Function;
struct Function {
    char *name;
    Node *body;
    LVar *locals;
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
Node *equality(Token**, Token*);
Node *expr(Token**, Token*);
void program(Token*);

#endif //INC_9CC_PARSE_H
