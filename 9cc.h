#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Type Type;

// Token
typedef enum {
	      TK_RESERVED, // 記号
	      TK_IDENT,    // 識別子
	      TK_NUM,      // 整数トークン
	      TK_EOF,      // 入力の終わりを表すトークン
	      TK_SCOLON,   // セミコロン
} TokenKind;


typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};


// Local variable
typedef struct Var Var;
struct Var {
  char *name; // Variable name
  int offset; // Offset from RBP
  Type *ty;   // Type
};


typedef struct VarList VarList;
struct VarList {
  VarList *next;
  Var *var;
};


// AST node
typedef enum {
	      ND_ADD, // +
	      ND_PTR_ADD, // ptr + num or num + ptr
	      ND_SUB, // -
	      ND_PTR_SUB, // ptr - num
	      ND_PTR_DIFF,// ptr - ptr
	      ND_MUL, // *
	      ND_DIV, // /
	      ND_EQ, // ==
	      ND_NE, // !=
	      ND_LT, // <
	      ND_LE, // <=
	      ND_ASSIGN, // =
	      ND_ADDR,   // unary &
	      ND_DEREF,  // unary *
	      ND_LVAR, // ローカル変数
	      ND_RETURN, // "return"
	      ND_IF,     // "if"
	      ND_WHILE,  // "while"
	      ND_FOR,    // "for"
	      ND_BLOCK,  // { ... }
	      ND_FUNCALL,// Function call
	      ND_EXPR_STMT, // 式ステートメント
	      ND_VAR,  // Variable
	      ND_NUM,  // 整数
	      ND_NULL, // Empty statement
} NodeKind;


typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *next;    // Next node

  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  char name;     // Used if kind == ND_VAR

  // "if", "while" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // Block
  Node *body;
  
  // Function call
  char *funcname;
  Node *args;
  
  Var *var;      // Used if kind == ND=VAR
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
  Type *ty;      // Type, e.g. int or pointer to int
  Token *tok;    // Representative token
};


typedef struct Function Function;
struct Function {
  Function *next;
  char *name;
  VarList *params;
  Node *node;
  VarList *locals;
  int stack_size;
};


//
// typing.c
//

typedef enum { TY_INT, TY_PTR } TypeKind;

struct Type {
  TypeKind kind;
  Type *base;
};

bool is_integer(Type *ty);
void add_type(Node *node);




Function *program(void);
Token *consume(char *op);
Token *consume_ident(void);
Token *tokenize(void);
Token *peek(char *s);
char *expect_ident(void);
long expect_number(void);
bool at_eof(void);
void codegen(Function *prog);
void error_tok(Token *tok, char *fmt, ...);
void expect(char *op);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool is_integer(Type *ty);
Type *pointer_to(Type *base);
void add_type(Node *node);


extern char *user_input;
extern Token *token;
extern Node *code[100];
extern Type *int_type;
