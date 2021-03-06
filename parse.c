//
// Created by 徳備彩人 on 2020/06/08.
//

#include "parse.h"

Node *new_node(NodeKind kind, Node *left, Node *right) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->left = left;
    node->right = right;
    return node;
}

Node *new_node_if(Node *condition,
                  Node *main_statement,
                  Node *alt_statement)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->condition = condition;
    node->main_statement = main_statement;
    node->alt_statement = alt_statement;
    return node;
}

Node *new_node_for(Node *init,
                   Node *condition,
                   Node *loop,
                   Node *main_statement)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    node->init = init;
    node->condition = condition;
    node->loop = loop;
    node->main_statement = main_statement;
    return node;
}

Node *new_node_while(Node *condition,
                     Node *main_statement)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->condition = condition;
    node->main_statement = main_statement;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *new_node_call(char *funcname) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FUNCCALL;
    node->funcname = funcname;
    return node;
}

Var *find_var(Token *tok) {
    for (VarList *varList = locals; varList; varList = varList->next) {
        Var *var = varList->var;
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}

Var *new_var(Token *token) {
    Var *var = find_var(token);
    if (var) {
        return var;
    }
    var = calloc(1, sizeof(Var));
    var->name = token->str;
    var->len = token->len;

    VarList *varList = calloc(1, sizeof(VarList));
    varList->var = var;
    varList->next = locals;
    locals = varList;
    return var;
}

Node *new_node_var(Token *token) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    node->var = new_var(token);
    return node;
}

Node *funccall(Token **rest, Token *token, Token *tok) {
    Node *node = new_node_call(strndup(tok->str, tok->len));
    if (consume(")", &token)) {
        *rest = token;
        return node;
    }
    NodeList *head = calloc(1, sizeof(NodeList));
    NodeList *current = head;
    while (true) {
        NodeList *nodelist = calloc(1, sizeof(NodeList));
        nodelist->node = expr(&token, token);
        current->next = nodelist;
        current = nodelist;
        if (consume(")", &token)) {
            break;
        }
        expect(",", &token);
    }
    node->args = head->next;
    *rest = token;
    return node;
}

// primary = "(" expr ")" | ident args? | num
// args = "(" (arg ("," arg)*)? ")"
Node *primary(Token **rest, Token *token) {
    Node *node;
    if (consume("(", &token)) {
        node = expr(&token, token);
        expect(")", &token);
    } else {
        Token *tok = consume_ident(&token);
        if (tok) {
            // function call
            if (consume("(", &token)) {
                node = funccall(&token, token, tok);
            } else {
                node = new_node_var(tok);
            }
        } else {
            node = new_node_num(expect_number(&token));
        }
    }
    *rest = token;
    return node;
}

// unary = "+"? primary
//       | "-"? primary
//       | "*" unary
//       | "&" unary
Node *unary(Token **rest, Token *token) {
    Node *node;
    if (consume("-", &token)) {
        node = new_node(ND_SUB, new_node_num(0), primary(&token, token));
    } else if (consume("&", &token)) {
        node = new_node(ND_ADDR, unary(&token, token), NULL);
    } else if (consume("*", &token)) {
        node = new_node(ND_DEREF, unary(&token, token), NULL);
    } else {
        consume("+", &token);
        node = primary(&token, token);
    }
    *rest = token;
    return node;
}

Node *mul(Token **rest, Token *token) {
    Node *node = unary(&token, token);
    for (;;) {
        if (consume("*", &token)) {
            node = new_node(ND_MUL, node, unary(&token, token));
        } else if (consume("/", &token)) {
            node = new_node(ND_DIV, node, unary(&token, token));
        } else {
            *rest = token;
            return node;
        }
    }
}

Node *add(Token **rest, Token *token) {
    Node *node = mul(&token, token);
    for (;;) {
        if (consume("+", &token)) {
            node = new_node(ND_ADD, node, mul(&token, token));
        } else if (consume("-", &token)) {
            node = new_node(ND_SUB, node, mul(&token, token));
        } else {
            *rest = token;
            return node;
        }
    }
}

Node *relational(Token **rest, Token *token) {
    Node *node = add(&token, token);
    for (;;) {
        if (consume("<", &token)) {
            node = new_node(ND_LT, node, add(&token, token));
        } else if (consume("<=", &token)) {
            node = new_node(ND_LE, node, add(&token, token));
        } else if (consume(">", &token)) {
            node = new_node(ND_LT, add(&token, token), node);
        } else if (consume(">=", &token)) {
            node = new_node(ND_LE, add(&token, token), node);
        } else {
            *rest = token;
            return node;
        }
    }
}

Node *equality(Token **rest, Token *token) {
    Node *node = relational(&token, token);
    for (;;) {
        if (consume("==", &token)) {
            node = new_node(ND_EQ, node, relational(&token, token));
        } else if (consume("!=", &token)) {
            node = new_node(ND_NE, node, relational(&token, token));
        } else {
            *rest = token;
            return node;
        }
    }
}

Node *assign(Token **rest, Token *token) {
    Node *node = equality(&token, token);
    if (consume("=", &token)) {
        node = new_node(ND_ASSIGN, node, assign(&token, token));
    }
    *rest = token;
    return node;
}

Node *expr(Token **rest, Token *token) {
    Node *node = assign(&token, token);
    *rest = token;
    return node;
}

Node *statement(Token** rest, Token *token) {
    Node *node;
    if (consume("{", &token)) {
        node = block(&token, token);
    } else if (consume("return", &token)) {
        node = new_node(ND_RETURN, expr(&token, token), NULL);
        expect(";", &token);
    } else if (consume("if", &token)) {
        expect("(", &token);
        Node *condition = expr(&token, token);
        expect(")", &token);
        Node *main_statement = statement(&token, token);
        Node *alt_statement = NULL;
        if (consume("else", &token)) {
            alt_statement = statement(&token, token);
        }
        node = new_node_if(condition, main_statement, alt_statement);
    } else if (consume("while", &token)) {
        expect("(", &token);
        Node *condition = expr(&token, token);
        expect(")", &token);
        Node *main_statement = statement(&token, token);
        node = new_node_while(condition, main_statement);
    } else if (consume("for", &token)) {
        expect("(", &token);
        Node *init=NULL, *condition=NULL, *loop=NULL;
        if (!consume(";", &token)) {
            init = expr(&token, token);
            expect(";", &token);
        }
        if (!consume(";", &token)) {
            condition = expr(&token, token);
            expect(";", &token);
        }
        if (!consume(")", &token)) {
            loop = expr(&token, token);
            expect(")", &token);
        }
        Node *main_statement = statement(&token, token);
        node = new_node_for(init, condition, loop, main_statement);
    } else {
        node = expr(&token, token);
        expect(";", &token);
    }
    *rest = token;
    return node;
}

// block = { statement* }
Node *block(Token **rest, Token *token) {
    Node *node;
    NodeList *head = calloc(1, sizeof(NodeList));
    NodeList *current_nl = head;
    for (int i = 0; !consume("}", &token); i++) {
        NodeList *new_nl = calloc(1, sizeof(NodeList));
        new_nl->node = statement(&token, token);
        current_nl->next = new_nl;
        current_nl = new_nl;
    }
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->statements = head->next;
    *rest = token;
    return node;
}

// function = type ident\((type arg)*\) block
Function *func(Token **rest, Token *token) {
    Function *function = calloc(1, sizeof(Function));
    consume_type(&token);
    Token *tok = consume_ident(&token);
    expect("(", &token);
    int nparams = 0;
    if (!consume(")", &token)) {
        VarList *params = NULL;
        while (true) {
            Token *param = consume_ident(&token);
            if (!param) {
                error("not identifier");
            }
            Var *var = new_var(param);
            VarList *varList = calloc(1, sizeof(VarList));
            varList->next = params;
            varList->var = var;
            params = varList;
            nparams++;
            if (consume(")", &token)) {
                break;
            }
            expect(",", &token);
        }
        function->params = params;
    }
    function->nparams = nparams;
    expect("{", &token);
    function->body = block(&token, token);
    function->name = strndup(tok->str, tok->len);
    *rest = token;
    return function;
}

Function *program(Token *token) {
    Function *head = calloc(1, sizeof(Function));
    Function *curr = head;
    while(!at_eof(token)) {
        Function *function = func(&token, token);
        function->locals = locals;
        curr->next = function;
        curr = curr->next;
        locals = NULL;
    }
    return head->next;
}
