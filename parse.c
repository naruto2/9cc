#include "9cc.h"
void basetype_6_7(Token *tok);
void basetype_6_5(Token *tok, StorageClass *sclass){
  if ( *sclass & (*sclass -1)) basetype_6_7(tok);
}
