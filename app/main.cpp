// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char input[256];

int main() {
  printf("---- Al Pal Shell 😎 ----\n");

  while(!feof(stdin) && !ferror(stdin))
  {
     // prompt
     printf("😎-> ");

     // request input
     fgets(input, 256, stdin);

     // hmmm
     printf("invalid command: %s", input);

     // clear the contents of input
     memset(input, 0, sizeof(input));
  }

  printf("\n----- Goodbye. -----\n");
  return 0;
}
