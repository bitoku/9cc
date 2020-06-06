//
// Created by 徳備彩人 on 2020/06/07.
//

#ifndef INC_9CC_TOKEN_H
#define INC_9CC_TOKEN_H

#include <zconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>


typedef struct Token Token;
Token *token;
char *user_input;
void error_at(char *loc, char *fmt, ...);
void expect(char expected);
int expect_number();
bool at_eof();
Token *tokenize(char *p);
bool consume(char expected);


#endif //INC_9CC_TOKEN_H
