#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "9cc.h"

static char *intelprob(char *str) {
  static char *reg[] = {"rax", "rsp", "rbp", "rdi", "ah", "al", "a0", "ch", "cl"};

  for (int i = 0; i < sizeof(reg) / sizeof(*reg); i++) {
    int len = strlen(reg[i]);
    
    static char buf[999];
    if (!strncmp(reg[i],str, len) ) {
      snprintf(buf,900,"_%s",reg[i]);
      return buf;
    }
  }
  return str;
}


int iprintf(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  char *str = (char*)format;
  int ret=0;
  
  while(1) {

    if (str[0] == '%' ) {
      
      switch(str[1]) {
      case 'd': ret += printf("%d",va_arg(argptr, int));              break;
      case 's': ret += printf("%s",intelprob(va_arg(argptr, char*))); break;
      default:                                                        break;
      }
      str+=2;
    } else {
      if (str[0] == '\0') break;
      putchar(str[0]);
      ret++;
      str++;
    }
  }
  return ret;
}
