//
// Created by 徳備彩人 on 2020/06/09.
//

#include "codegen.h"

static char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

static int assign_var(Function *function) {
    int offset = 0;
    for (VarList *varList = function->locals; varList; varList = varList->next) {
        Var *var = varList->var;
        offset += 8;
        var->offset = offset;
    }
    return offset;
}

void gen_var(const Node *node) {
    if (node->kind != ND_LVAR) error("左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->var->offset);
    printf("  push rax\n");
}

void gen(const Node *node, Function *function) {
    int label = label_count++;

    switch (node->kind) {
    case ND_BLOCK:
        for (NodeList *cur = node->statements; cur; cur = cur->next) {
            gen(cur->node, function);
        }
        return;
    case ND_IF:
        gen(node->condition, function);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (node->alt_statement) {
            printf("  je  .Lelse%d\n", label);
            gen(node->main_statement, function);
            printf("  jmp .Lend%d\n", label);
            printf(".Lelse%d:\n", label);
            gen(node->alt_statement, function);
        } else {
            printf("  je  .Lend%d\n", label);
            gen(node->main_statement, function);
        }
        printf(".Lend%d:\n", label);
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", label);
        gen(node->condition, function);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", label);
        gen(node->main_statement, function);
        printf("  jmp .Lbegin%d\n", label);
        printf(".Lend%d:\n", label);
        return;
    case ND_FOR:
        if (node->init) gen(node->init, function);
        printf(".Lbegin%d:\n", label);
        if (node->condition) {
            gen(node->condition, function);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lend%d\n", label);
        }
        gen(node->main_statement, function);
        if (node->loop) gen(node->loop, function);
        printf("  jmp .Lbegin%d\n", label);
        printf(".Lend%d:\n", label);
        return;
    case ND_RETURN:
        gen(node->left, function);
        printf("  pop rax\n");
        printf("  jmp .Lreturn.%s\n", function->name);
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_var(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_var(node->left);
        gen(node->right, function);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_FUNCCALL: {
        int nargs = 0;
        for (NodeList *arg = node->args; arg; arg = arg->next) {
            gen(arg->node, function);
            nargs++;
        }

        for (int i = nargs-1; i >= 0; i--) {
            printf("  pop %s\n", argreg[i]);
        }
        // We need to align RSP to a 16 byte boundary before
        // calling a function because it is an ABI requirement.
        // RAX is set to 0 for variadic function.
        printf("  mov rax, rsp\n");
        printf("  and rax, 15\n");
        printf("  jnz .Lcall%d\n", label);
        printf("  mov rax, 0\n");
        printf("  call _%s\n", node->funcname);
        printf("  jmp .Lend%d\n", label);
        printf(".Lcall%d:\n", label);
        printf("  sub rsp, 8\n");
        printf("  mov rax, 0\n");
        printf("  call _%s\n", node->funcname);
        printf("  add rsp, 8\n");
        printf(".Lend%d:\n", label);
        printf("  push rax\n");
        return;
    }
    }

    gen(node->left, function);
    gen(node->right, function);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzx rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzx rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzx rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzx rax, al\n");
        break;
    }
    printf("  push rax\n");
}

void funcgen(Function *function) {
    printf("_%s:\n", function->name);

    int offset = assign_var(function);

    // prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", offset);

    int i = 0;
    for (VarList *varList = function->params; varList; varList = varList->next) {
        Var *var = varList->var;
        int j = function->nparams - 1 - (i++);
        printf("  mov [rbp-%d], %s\n", var->offset, argreg[j]);
    }
    gen(function->body, function);

    printf(".Lreturn.%s:\n", function->name);
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

void prologue() {
    printf(".intel_syntax noprefix\n");
    printf(".globl _main\n");
}