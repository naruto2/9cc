#include "9cc.h"

Type *basetype(StorageClass *sclass);
Type *declarator(Type *ty, char **name);

// Determine whether the next top-level item is a function
// or a global variable by looking ahead input tokens.
bool is_function(void) {
  Token *tok = token;
  bool isfunc = false;

  StorageClass sclass;
  Type *ty = basetype(&sclass);

  if (!consume(";")) {
    char *name = NULL;
    declarator(ty, &name);
    isfunc = name && consume("(");
  }

  token = tok;
  return isfunc;
}
