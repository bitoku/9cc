#include <stdio.h>
#include "token.h"
#include "parse.h"
#include "codegen.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    char *user_input = argv[1];
    label_count = 0;
    Token *token = tokenize(user_input);
    Function *functions = program(token);
    prologue();
    for (Function *func = functions; func; func = func->next) {
        funcgen(func);
    }
    return 0;
}
