// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>


int main() {
  printf("---- 😎 Al Pal Shell 😎 ----\n");

  std::string input;
  pid_t pid;
  int status;

  while(!feof(stdin) && !ferror(stdin))
  {
  	printf("😎-> ");
	std::cin >> input;

	pid = fork();

     	if (pid == 0) {
		std::string command = std::string("/bin/") + input;
		execlp(command.c_str(), input.c_str(), NULL);
		exit(0);
    	} else {
		if (waitpid(pid, &status, 0) > 0) {
			if (WIFEXITED(status) && !WEXITSTATUS(status))
				printf("\n");
		}
	}

  }

  printf("\n----- 😎 Goodbye 😎 -----\n");
  return 0;
}
