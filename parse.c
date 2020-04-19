#include "9cc.h"

// All local variable instances created during parseing are
// acculated to this list.
Var *locals;

// 現在着目しているトークン
Token *token;


// 入力プログラム
char *user_input;


Node *code[100];


static Node *new_var_node(Var *var) {
  Node *node = new_node1(ND_VAR,token);
  node->var = var;
  return node;
}



static Var *new_lvar(char *name) {
  Var *var = calloc(1, sizeof(Var));
  var->next = locals;
  var->name = name;
  locals = var;
  return var;
}


Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}


static Node *new_node1(NodeKind kind, Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}


Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}


Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}


static char *starts_with_reserved(char *p) {
  // Keyword
  static char *kw[] = {"return", "if", "else", "while", "for"};

  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
    int len = strlen(kw[i]);
    if (startswith(p, kw[i]) && !is_alnum(p[len]))
      return kw[i];
  }

  // Multi-letter punctuator
  static char *ops[] = {"==", "!=", "<=", ">="};

  for (int i = 0; i < sizeof(ops) / sizeof(*ops); i++)
    if (startswith(p, ops[i]))
      return ops[i];

  return NULL;
}



static Node *expr() {
  return assign();
}


// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      |  "{" stmt* "}"
//      | expr ";"
static Node *stmt(void) {
  if (consume("return")) {
    Node *node = new_unary(ND_RETURN, expr(),token);
    expect(";");
    return node;
  }
  if (consume("if")) {
    Node *node = new_node1(ND_IF,token);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else"))
      node->els = stmt();
    return node;
  }
  if (consume("while")) {
    Node *node = new_node1(ND_WHILE,token);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }
  if (consume("for")) {
    Node *node = new_node1(ND_FOR,token);
    expect("(");
    if (!consume(";")) {
      node->init = read_expr_stmt();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = read_expr_stmt();
      expect(")");
    }
    node->then = stmt();
    return node;
  }
  if (consume("{")) {
    Node head = {};
    Node *cur = &head;

    while (!consume("}")) {
      cur->next = stmt();
      cur = cur->next;
    }

    Node *node = new_node1(ND_BLOCK,token);
    node->body = head.next;
    return node;
  }
  Node *node = read_expr_stmt();
  expect(";");
  return node;
}
  
Function *program(void) {
  locals = NULL;

  Node head = {};
  Node *cur = &head;

  while (!at_eof()) {
    cur->next = stmt();
    cur = cur->next;
  }

  Function *prog = calloc(1, sizeof(Function));
  prog->node = head.next;
  prog->locals = locals;
  return prog;
}


#if 0
Node *program(void) {
  Node head = {};
  Node *cur = &head;

  while (!at_eof()) {
    cur->next = stmt();
    cur = cur->next;
  }
  return head.next;
}
#endif

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}


// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}


// Find a local variable by name.
static Var *find_var(Token *tok) {
  for (Var *var = locals; var; var = var->next)
    if (strlen(var->name) == tok->len && !strncmp(tok->str, var->name, tok->len))
      return var;
  return NULL;
}


// func-args = "(" (assign ("," assign)*)? ")"
static Node *func_args(void) {
  if (consume(")"))
    return NULL;

  Node *head = assign();
  Node *cur = head;
  while (consume(",")) {
    cur->next = assign();
    cur = cur->next;
  }
  expect(")");
  return head;
}

// primary = "(" expr ")" | ident func-args? | num
static Node *primary() {

  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    // Function call
    if (consume("(")) {
      Node *node = new_node1(ND_FUNCALL,token);
      node->funcname = strndup(tok->str, tok->len);
      node->args = func_args();
      return node;
    }
    
    // Variable
    Var *var = find_var(tok);
    if (!var)
      var = new_lvar(strndup(tok->str, tok->len));
    return new_var_node(var);
  }
  
  // そうでなければ数値のはず
  return new_node_num(expect_number());
}


Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}


Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}



static Node *new_unary(NodeKind kind, Node *expr, Token *tok) {
  Node *node = new_node1(kind, tok);
  node->lhs = expr;
  return node;
}



static Node *read_expr_stmt(void) {
  return new_unary(ND_EXPR_STMT, expr(), token);
}


Node *unary() {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node
      (ND_SUB, new_node_num(0), unary());
  return primary();
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}


bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}


static bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}



