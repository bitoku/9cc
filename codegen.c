//
// Created by 徳備彩人 on 2020/06/09.
//

#include "codegen.h"

void gen_lval(const Node *node) {
    if (node->kind != ND_LVAR) error("左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(const Node *node) {
    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->left);
        gen(node->right);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    gen(node->left);
    gen(node->right);

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

void codegen() {
    printf(".intel_syntax noprefix\n");
    printf(".globl _main\n");
    printf("_main:\n");

    // prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for (int i = 0; code[i]; i++) {
        gen(code[i]);
        printf("  pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}