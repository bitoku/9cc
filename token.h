//
// Created by 徳備彩人 on 2020/06/07.
//

#ifndef INC_9CC_TOKEN_H
#define INC_9CC_TOKEN_H

#include <string.h>
#include <zconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum {
    TK_RESERVED,
    TK_INT,  // integer
    TK_IDENT,  // identifier
    TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;  // type of token
    Token *next;  // next token
    int val;  // token value;
    char *str;  // token string;
    size_t len; // length of str;
};

Token *token;
char *user_input;
void error(char*, ...);
void error_at(char*, char*, ...);
void expect(char*);
int expect_number();
bool at_eof();
Token *tokenize(char*);
bool consume(char*);
Token *consume_ident();


#endif //INC_9CC_TOKEN_H
