//
// Created by 徳備彩人 on 2020/06/07.
//

#include "token.h"

typedef enum {
    TK_RESERVED,
    TK_INT,  // integer
    TK_EOF,
} TokenKind;

struct Token {
    TokenKind kind;  // type of token
    Token *next;  // next token
    int val;  // token kind;
    char *str;  // token string;
};

// current token


void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    unsigned long pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", (int)pos, "  ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void move_next() {
    token = token->next;
}

bool consume(char expected) {
    if (token->kind != TK_RESERVED || token->str[0] != expected) {
        return false;
    }
    move_next();
    return true;
}

void expect(char expected) {
    if (!consume(expected)) {
        error_at(token->str, "'%c'ではありません", expected);
    }
}

int expect_number() {
    if (token->kind != TK_INT) error_at(token->str, "数ではありません");
    int val = token->val;
    move_next();
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_INT, cur, p);
            cur->val = (int)strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}