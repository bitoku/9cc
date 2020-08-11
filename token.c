//
// Created by 徳備彩人 on 2020/06/07.
//

#include "token.h"

bool starts_with(const char *p, const char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

bool strcmp_with_literal(const char *p, const char *q, size_t len) {
    return strlen(q) == len && starts_with(p, q);
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
    if (strlen(expected) != token->len ||
        !starts_with(token->str, expected)) {
        return false;
    }
    move_next();
    return true;
}

bool peek(char *expected) {
    int x = strlen(expected);
    return token->next && strncmp(token->next->str, expected, strlen(expected)) == 0;
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

size_t issymbol(char *p) {
    const char *reserved[] = {"==", "!=", "<=", ">=", "<", ">", "+", "-", "*", "/", "(", ")", "=", ";"};
    for (int i = 0; i < sizeof(reserved)/sizeof(char*); i++) {
        if (!starts_with(p, reserved[i])) continue;
        return strlen(reserved[i]);
    }
    return 0;
}

Token *alnumtoken(char *p, Token *cur, size_t len) {
    if (strcmp_with_literal(p, "return", len)) {
        return new_token(TK_RETURN, cur, p, 6);
    } else if (strcmp_with_literal(p, "if", len)) {
        return new_token(TK_IF, cur, p, 2);
    } else if (strcmp_with_literal(p, "else", len)) {
        return new_token(TK_ELSE, cur, p, 4);
    } else if (strcmp_with_literal(p, "while", len)) {
        return new_token(TK_WHILE, cur, p, 5);
    } else if (strcmp_with_literal(p, "for", len)) {
        return new_token(TK_FOR, cur, p, 3);
    }
    return new_token(TK_IDENT, cur, p, len);
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

char *find_ident(char *p) {
    if (!(('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || *p == '_')) return NULL;
    p++;
    while (is_alnum(*p)) {
        p++;
    }
    return p;
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

        size_t reserved_len = issymbol(p);

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

        char *ident_end = find_ident(p);
        if (ident_end) {
            cur = alnumtoken(p, cur, ident_end - p);
            p = ident_end;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
