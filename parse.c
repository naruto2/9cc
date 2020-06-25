#include "9cc.h"

// All local variable instances created during parsing are
// accumulated to this list.
 VarList *locals;

// Likewise, global variables are accumulated to this list.
 VarList *globals;

// C has two block scopes; one is for variables/typedefs and
// the other is for struct/union/enum tags.
 VarScope *var_scope;
 TagScope *tag_scope;
 int scope_depth;

// Points to a node representing a switch if we are parsing
// a switch statement. Otherwise, NULL.
 Node *current_switch;


bool is_function(void);
Function *function(void);
Type *basetype(StorageClass *sclass);
Type *declarator(Type *ty, char **name);
 Type *abstract_declarator(Type *ty);
 Type *type_suffix(Type *ty);
 Type *type_name(void);
 Type *struct_decl(void);
 Type *enum_specifier(void);
 Member *struct_member(void);
void global_var(void);
 Node *declaration(void);
 bool is_typename(void);
 Node *stmt(void);
 Node *stmt2(void);
 Node *expr(void);
 long eval(Node *node);
 long eval2(Node *node, Var **var);
 long const_expr(void);
 Node *assign(void);
 Node *conditional(void);
 Node *logor(void);
 Node *logand(void);
 Node *bitand(void);
 Node *bitor(void);
 Node *bitxor(void);
 Node *equality(void);
 Node *relational(void);
 Node *shift(void);
 Node *new_add(Node *lhs, Node *rhs, Token *tok);
 Node *add(void);
 Node *mul(void);
 Node *cast(void);
 Node *unary(void);
 Node *postfix(void);
 Node *compound_literal(void);
Node *primary(void);
void push_tag_scope(Token *tok, Type *ty);
bool consume_end(void);
bool peek_end(void);
void expect_end(void);
VarList *read_func_param(void);      
void read_func_params(Function *fn);
void skip_excess_elements2(void);
void skip_excess_elements(void);




// basetype = builtin-type | struct-decl | typedef-name | enum-specifier
//
// builtin-type = "void" | "_Bool" | "char" | "short" | "int"
//              | "long" | "long" "long"
//
// Note that "typedef" and "static" can appear anywhere in a basetype.
// "int" can appear anywhere if type is short, long or long long.
Type *basetype(StorageClass *sclass) {
  if (!is_typename())
    error_tok(token, "typename expected");

  enum {
    VOID  = 1 << 0,
    BOOL  = 1 << 2,
    CHAR  = 1 << 4,
    SHORT = 1 << 6,
    INT   = 1 << 8,
    LONG  = 1 << 10,
    OTHER = 1 << 12,
  };

  Type *ty = int_type;
  int counter = 0;

  if (sclass)
    *sclass = 0;

  while (is_typename()) {
    Token *tok = token;

    // Handle storage class specifiers.
    if (peek("typedef") || peek("static") || peek("extern")) {
      if (!sclass)
        error_tok(tok, "storage class specifier is not allowed");

      if (consume("typedef"))
        *sclass |= TYPEDEF;
      else if (consume("static"))
        *sclass |= STATIC;
      else if (consume("extern"))
	*sclass |= EXTERN;
      
      if (*sclass & (*sclass - 1))
        error_tok(tok, "typedef, static and extern may not be used together");
      continue;
    }

    // Handle user-defined types.
    if (!peek("void") && !peek("_Bool") && !peek("char") &&
        !peek("short") && !peek("int") && !peek("long")) {
      if (counter)
        break;

      if (peek("struct")) {
        ty = struct_decl();
      } else if (peek("enum")) {
        ty = enum_specifier();
      } else {
        ty = find_typedef(token);
        assert(ty);
        token = token->next;
      }

      counter |= OTHER;
      continue;
    }

    // Handle built-in types.
    if (consume("void"))
      counter += VOID;
    else if (consume("_Bool"))
      counter += BOOL;
    else if (consume("char"))
      counter += CHAR;
    else if (consume("short"))
      counter += SHORT;
    else if (consume("int"))
      counter += INT;
    else if (consume("long"))
      counter += LONG;

    switch (counter) {
    case VOID:
      ty = void_type;
      break;
    case BOOL:
      ty = bool_type;
      break;
    case CHAR:
      ty = char_type;
      break;
    case SHORT:
    case SHORT + INT:
      ty = short_type;
      break;
    case INT:
      ty = int_type;
      break;
    case LONG:
    case LONG + INT:
    case LONG + LONG:
    case LONG + LONG + INT:
      ty = long_type;
      break;
    default:
      error_tok(tok, "invalid type");
    }
  }

  return ty;
}

VarList *read_func_param(void);
void read_func_params(Function *fn);
Initializer *new_init_val(Initializer *cur, int sz, int val);
Initializer *new_init_label(Initializer *cur, char *label, long addend);
Initializer *new_init_zero(Initializer *cur, int nbytes);
Initializer *gvar_init_string(char *p, int len);
Initializer *emit_struct_padding(Initializer *cur, Type *parent, Member *mem);
Initializer *gvar_initializer2(Initializer *cur, Type *ty);
Initializer *gvar_initializer(Type *ty);
void global_var(void);


/* ------------ 2020-6-26 -------------- parse.c -> mini.c ここまで */


typedef struct Designator Designator;
struct Designator {
  Designator *next;
  int idx;     // array
  Member *mem; // struct
};


Node *new_desg_node(Var *var, Designator *desg, Node *rhs);
Node *lvar_init_zero(Node *cur, Var *var, Type *ty, Designator *desg);
Node *lvar_initializer2(Node *cur, Var *var, Type *ty, Designator *desg);
Node *lvar_initializer(Var *var, Token *tok);
Node *declaration(void);
Node *read_expr_stmt(void);
bool is_typename(void);
Node *stmt(void);
Node *stmt2(void);
Node *expr(void);
long eval(Node *node);
long eval2(Node *node, Var **var);
long const_expr(void);
Node *assign(void);
Node *conditional(void);
Node *logor(void);
Node *logand(void);
Node *bitor(void);
Node *bitxor(void);
Node *bitand(void);
Node *equality(void);
Node *relational(void);
Node *shift(void) ;
Node *new_add(Node *lhs, Node *rhs, Token *tok);
Node *new_sub(Node *lhs, Node *rhs, Token *tok);
Node *add(void);
Node *mul(void);
Node *cast(void);
Node *unary(void);
Member *find_member(Type *ty, char *name);
Node *struct_ref(Node *lhs);
Node *postfix(void);
Node *compound_literal(void);
Node *stmt_expr(Token *tok);
Node *func_args(void);
Node *primary(void);


// primary = "(" "{" stmt-expr-tail
//         | "(" expr ")"
//         | "sizeof" "(" type-name ")"
//         | "sizeof" unary
//         | "_Alignof" "(" type-name ")"
//         | ident func-args?
//         | str
//         | num
Node *primary(void) {
  Token *tok;

  if ((tok = consume("("))) {
    if (consume("{"))
      return stmt_expr(tok);

    Node *node = expr();
    expect(")");
    return node;
  }

  if ((tok = consume("sizeof"))) {
    if (consume("(")) {
      if (is_typename()) {
        Type *ty = type_name();
        if (ty->is_incomplete)
          error_tok(tok, "incomplete type");
        expect(")");
        return new_num(ty->size, tok);
      }
      token = tok->next;
    }

    Node *node = unary();
    add_type(node);
    if (node->ty->is_incomplete)
      error_tok(node->tok, "incomplete type");
    return new_num(node->ty->size, tok);
  }

  if ((tok = consume("_Alignof"))) {
    expect("(");
    Type *ty = type_name();
    expect(")");
    return new_num(ty->align, tok);
  }
  
  if ((tok = consume_ident())) {
    // Function call
    if (consume("(")) {
      Node *node = new_node(ND_FUNCALL, tok);
      node->funcname = strndup(tok->str, tok->len);
      node->args = func_args();
      add_type(node);

      VarScope *sc = find_var(tok);
      if (sc) {
        if (!sc->var || sc->var->ty->kind != TY_FUNC)
          error_tok(tok, "not a function");
        node->ty = sc->var->ty->return_ty;
      } else if (!strcmp(node->funcname, "__builtin_va_start")) {
	node->ty = void_type;
      } else {
        warn_tok(node->tok, "implicit declaration of a function");
        node->ty = int_type;
      }
      return node;
    }

    // Variable or enum constant
    VarScope *sc = find_var(tok);
    if (sc) {
      if (sc->var)
        return new_var_node(sc->var, tok);
      if (sc->enum_ty)
        return new_num(sc->enum_val, tok);
    }
    error_tok(tok, "undefined variable");
  }

  tok = token;
  if (tok->kind == TK_STR) {
    token = token->next;

    Type *ty = array_of(char_type, tok->cont_len);
    Var *var = new_gvar(new_label(), ty, true, true);
    
    var->initializer = gvar_init_string(tok->contents, tok->cont_len);
    return new_var_node(var, tok);
  }

  if (tok->kind != TK_NUM)
    error_tok(tok, "expected expression");
  return new_num(expect_number(), tok);
}
