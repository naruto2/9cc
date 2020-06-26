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

