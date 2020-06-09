//
// Created by 徳備彩人 on 2020/06/07.
//

#include "token.h"

bool starts_with(const char *p, const char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}


void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    unsigned long pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", (int)pos, "");
    fprintf(stderr, "^ ");
    error(fmt, ap);
}

void move_next() {
    token = token->next;
}

bool consume(char* expected) {
    if (token->kind != TK_RESERVED ||
        strlen(expected) != token->len ||
        !starts_with(token->str, expected)) {
        return false;
    }
    move_next();
    return true;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT) return NULL;
    Token *tok = token;
    move_next();
    return tok;
}

void expect(char* expected) {
    if (!consume(expected)) {
        error_at(token->str, "'%s'ではありません", expected);
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

Token *new_token(TokenKind kind, Token *cur, char *str, size_t len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

size_t isreserved(char *p) {
    const char *reserved[] = {"==", "!=", "<=", ">=", "<", ">", "+", "-", "*", "/", "(", ")", "=", ";"};
    for (int i = 0; i < sizeof(reserved)/sizeof(char*); i++) {
        if (!starts_with(p, reserved[i])) continue;
        return strlen(reserved[i]);
    }
    return 0;
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

        size_t reserved_len = isreserved(p);

        if (reserved_len > 0) {
            cur = new_token(TK_RESERVED, cur, p, reserved_len);
            p += reserved_len;
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_INT, cur, p, 0);
            char *q = p;
            cur->val = (int)strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            cur = new_token(TK_IDENT, cur, p, 1);
            p++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
