//
// Created by 徳備彩人 on 2020/06/09.
//

#ifndef INC_9CC_CODEGEN_H
#define INC_9CC_CODEGEN_H

#include "parse.h"

void codegen();
void gen(const Node*);

int label_count;

#endif //INC_9CC_CODEGEN_H
