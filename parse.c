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

LVar *find_lvar(Token *tok) {
    for (LVar *lvar = locals; lvar; lvar = lvar->next) {
        if (lvar->len == tok->len && !memcmp(tok->str, lvar->name, lvar->len)){
            return lvar;
        }
    }
    return NULL;
}

// primary = "(" expr ")" | ident args? | num
// args = "(" (arg ("," arg)*)? ")"
Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        // function call
        if (consume("(")) {
            Node *node = new_node_call(strndup(tok->str, tok->len));
            if (consume(")")) {
                return node;
            }
            NodeList *head = calloc(1, sizeof(NodeList));
            NodeList *current = head;
            while (true) {
                NodeList *nodelist = calloc(1, sizeof(NodeList));
                nodelist->node = expr();
                current->next = nodelist;
                current = nodelist;
                if (consume(")")) {
                    break;
                }
                expect(",");
            }
            node->args = head->next;
            return node;
        }
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals ? locals->offset + 8 : 0;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }
    return new_node_num(expect_number());
}

Node *unary() {
    if (consume("+")) return primary();
    if (consume("-")) return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *mul() {
    Node *node = unary();
    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

Node *add() {
    Node *node = mul();
    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = add();
    for (;;) {
        if (consume("<")) {
            node = new_node(ND_LT, node, add());
        } else if (consume("<=")) {
            node = new_node(ND_LE, node, add());
        } else if (consume(">")) {
            node = new_node(ND_LT, add(), node);
        } else if (consume(">=")) {
            node = new_node(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

Node *equality() {
    Node *node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node *assign() {
    Node *node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node *expr() {
    return assign();
}

Node *statement() {
    Node *node;
    if (consume("{")) {
        NodeList *head = calloc(1, sizeof(NodeList));
        NodeList *current_nl = head;
        for (int i = 0; i < CODE_LENGTH && !consume("}"); i++) {
            NodeList *new_nl = calloc(1, sizeof(NodeList));
            new_nl->node = statement();
            current_nl->next = new_nl;
            current_nl = new_nl;
        }
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->statements = head->next;
    } else if (consume("return")) {
        node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
    } else if (consume("if")) {
        expect("(");
        Node *condition = expr();
        expect(")");
        Node *main_statement = statement();
        Node *alt_statement = NULL;
        if (consume("else")) {
            alt_statement = statement();
        }
        node = new_node_if(condition, main_statement, alt_statement);
    } else if (consume("while")) {
        expect("(");
        Node *condition = expr();
        expect(")");
        Node *main_statement = statement();
        node = new_node_while(condition, main_statement);
    } else if (consume("for")) {
        expect("(");
        Node *init=NULL, *condition=NULL, *loop=NULL;
        if (!consume(";")) {
            init = expr();
            expect(";");
        }
        if (!consume(";")) {
            condition = expr();
            expect(";");
        }
        if (!consume(")")) {
            loop = expr();
            expect(")");
        }
        Node *main_statement = statement();
        node = new_node_for(init, condition, loop, main_statement);
    } else {
        node = expr();
        expect(";");
    }
    return node;
}

void program() {
    for (int i = 0; i < CODE_LENGTH && !at_eof(); i++) {
        code[i] = statement();
    }
    code[CODE_LENGTH] = NULL;
}