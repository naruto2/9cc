#include "9cc.h"
bool is_typename(void);
Type *struct_decl(void);
Type *enum_specifier(void);


// basetype = builtin-type | struct-decl | typedef-name | enum-specifier
//
// builtin-type = "void" | "_Bool" | "char" | "short" | "int"
//              | "long" | "long" "long"
//
// Note that "typedef" and "static" can appear anywhere in a basetype.
// "int" can appear anywhere if type is short, long or long long.
Type *basetype_7(void);
int basetype_8(int counter);
Type *basetype_9(Token *tok, int counter);
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

      if (*sclass & (*sclass -1))
        error_tok(tok, "typedef, static and extern may not be used together");
      continue;
    }

    // Handle user-defined types.
    if (!peek("void") && !peek("_Bool") && !peek("char") &&
        !peek("short") && !peek("int") && !peek("long")) {
      if (counter) break;
      ty = basetype_7();
      counter |= OTHER;
      continue;
    }
    counter = basetype_8(counter);
    ty = basetype_9(tok, counter);
  }
  return ty;
}
