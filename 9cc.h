#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Type Type;
typedef struct Member Member;


// Token
typedef enum {
	      TK_RESERVED, // 記号
	      TK_IDENT,    // 識別子
	      TK_STR,      // String literals
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
  
  char *contents; // String literal contents includeing terminating '\0'
  char  cont_len; // String literal length
};


// Variable
typedef struct Var Var;
struct Var {
  char *name;    // Variable name
  Type *ty;      // Type
  bool is_local; // local or global

  // Local variable
  int offset;    // Offset from RBP

  // Global variable
  char *contents;
  int   cont_len;
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
	      ND_MEMBER, // . (struct member access)
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
	      ND_STMT_EXPR, // Statement expression
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

  // Block or statement expression
  Node *body;
  
  // Struct member access
  Member *member;

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

typedef enum {
	      TY_CHAR,
	      TY_SHORT,
	      TY_INT,
	      TY_LONG,
	      TY_PTR,
	      TY_ARRAY,
	      TY_STRUCT,
	      TY_FUNC,
} TypeKind;

struct Type {
  TypeKind kind;
  int size;        // sizeof() value
  int align;       // alignment
  Type *base;      // pointer or array
  int array_len;   // array
  Member *members; // struct
  Type *return_ty; // function
};


// Struct member
struct Member {
  Member *next;
  Type *ty;
  char *name;
  int offset;
};


typedef struct {
  VarList *globals;
  Function *fns;
} Program;


Program *program(void);
Token *consume(char *op);
Token *consume_ident(void);
Token *tokenize(void);
Token *peek(char *s);
char *expect_ident(void);
long expect_number(void);
bool is_integer(Type *ty);
bool at_eof(void);
void add_type(Node *node);
void codegen(Program *prog);
void error_tok(Token *tok, char *fmt, ...);
void warn_tok(Token *tok, char *fmt, ...);
void expect(char *op);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int size);
Type *func_type(Type *return_ty);
int align_to(int n, int align);


extern char *user_input;
extern Token *token;
extern Node *code[100];
extern Type *char_type;
extern Type *short_type;
extern Type *int_type;
extern Type *long_type;
extern char *filename;
