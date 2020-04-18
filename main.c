#include "9cc.h"



int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(user_input);
  Node *node = program();

  codegen(node);
  return 0;
  
  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");


  // 抽象構文木を下りながらコード生成
  for (Node *n = node; n; n = n->next) {
    gen(n);
    //printf("  pop rax\n");
  }
  
  return 0;
}

