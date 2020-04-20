#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};


typedef struct VarList VarList;
struct VarList {
  VarList *next;
  Var *var;
};


// AST node
typedef enum {
	      ND_ADD, // +
	      ND_SUB, // -
	      ND_MUL, // *
	      ND_DIV, // /
	      ND_EQ, // ==
	      ND_NE, // !=
	      ND_LT, // <
	      ND_LE, // <=
	      ND_ASSIGN, // =
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
  Token *tok;
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


long expect_number();
void expect(char *op);



static Node *primary();




void gen(Node *node);
Token *tokenize(void);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
char *expect_ident(void);
bool at_eof();
Function *program(void);
bool consume(char *op);
Token *consume_ident(void);
void codegen(Function *prog);
static bool is_alnum(char c);
static bool startswith(char *p, char *q);

static Node *new_unary(NodeKind kind, Node *expr, Token *tok);
static Node *new_node1(NodeKind kind, Token *tok);
static Node *read_expr_stmt(void);





extern char *user_input;
extern Token *token;
extern Node *code[100];
