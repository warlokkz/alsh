// Executables must have the following defined if the library contains
// doctest definitions. For builds with this disabled, e.g. code shipped to
// users, this can be left out.
#ifdef ENABLE_DOCTEST_IN_LIBRARY
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

const char *welcome = R"(
          _____                    _____            _____                    _____
         /\    \                  /\    \          /\    \                  /\    \
        /::\    \                /::\____\        /::\    \                /::\____\
       /::::\    \              /:::/    /       /::::\    \              /:::/    /
      /::::::\    \            /:::/    /       /::::::\    \            /:::/    /
     /:::/\:::\    \          /:::/    /       /:::/\:::\    \          /:::/    /
    /:::/__\:::\    \        /:::/    /       /:::/__\:::\    \        /:::/____/
   /::::\   \:::\    \      /:::/    /        \:::\   \:::\    \      /::::\    \
  /::::::\   \:::\    \    /:::/    /       ___\:::\   \:::\    \    /::::::\    \   _____
 /:::/\:::\   \:::\    \  /:::/    /       /\   \:::\   \:::\    \  /:::/\:::\    \ /\    \
/:::/  \:::\   \:::\____\/:::/____/       /::\   \:::\   \:::\____\/:::/  \:::\    /::\____\
\::/    \:::\  /:::/    /\:::\    \       \:::\   \:::\   \::/    /\::/    \:::\  /:::/    /
 \/____/ \:::\/:::/    /  \:::\    \       \:::\   \:::\   \/____/  \/____/ \:::\/:::/    /
          \::::::/    /    \:::\    \       \:::\   \:::\    \               \::::::/    /
           \::::/    /      \:::\    \       \:::\   \:::\____\               \::::/    /
           /:::/    /        \:::\    \       \:::\  /:::/    /               /:::/    /
          /:::/    /          \:::\    \       \:::\/:::/    /               /:::/    /
         /:::/    /            \:::\    \       \::::::/    /               /:::/    /
        /:::/    /              \:::\____\       \::::/    /               /:::/    /
        \::/    /                \::/    /        \::/    /                \::/    /
         \/____/                  \/____/          \/____/                  \/____/
)";


// TODO: -- move out of here --
class InputCommand {
public:
	InputCommand(std::string input);
	InputCommand(char *argv[]);

	std::string path;
	std::string cmd;
	std::vector<std::string> args;

	void print();
	int execute();
	bool is_builtin();
	int do_builtin(const char* argv[]);
};

InputCommand::InputCommand(char *argv[]) {
	cmd = std::string(argv[2]);
	path = std::string("/bin/") + cmd;
}

InputCommand::InputCommand(std::string input) {
	std::string token;
	std::string delimiter = std::string(" ");
	size_t pos_start = 0;
	size_t pos_end, delimiter_length = delimiter.length();

	while ((pos_end = input.find(delimiter, pos_start)) != std::string::npos) {
		token = input.substr(pos_start, pos_end - pos_start);

		if (cmd.empty()) {
			cmd = token;
			path = std::string("/bin/") + cmd;
		} else {
			args.push_back(token);
		}
		
		pos_start = pos_end + delimiter_length;
	}

	// ugh this is not pretty.... refactor later
	token = input.substr(pos_start);
	if (cmd.empty()) {
		cmd = token;
		path = std::string("/bin/") + cmd;
	} else {
		args.push_back(token);
	}

}

// uhhhhh
int cd(const char *argv[]) {
	return chdir(argv[1]);
}

// too tired to do an actual implementation.... must use if statements ....
bool InputCommand::is_builtin() {
	if (cmd == "cd") {
		return true;
	} else {
		return false;
	}
}

int InputCommand::do_builtin(const char *argv[]) {
	if (cmd == "cd") {
		return cd(argv);
	}

	return 0;
}

int InputCommand::execute() {
	// converts our vector of strings to a char* *const arg for execvp
	// is very ugly please dont look

	int exec_argc = args.size();
	const char **argv = new const char* [exec_argc + 2];
	const char *cmd_name = cmd.c_str();

	argv[0] = cmd_name;
	for (int i = 0; i < exec_argc; i++) {
		argv[i+1] = args[i].c_str();
	}
	argv[exec_argc + 1] = NULL;

	if (is_builtin()) {
		return do_builtin(argv);
	}

	int ret = execvp(
		cmd_name,
		(char **)argv
	);

	delete[] argv;

	return ret;
}

void InputCommand::print() {
	std::cout << cmd << std::endl;
	std::cout << path << std::endl;

	std::cout << args.size() << std::endl;

	for (auto arg : args) {
		std::cout << arg << std::endl;
	}
}

// TODO: -- move out of here --
class InputHandler {
public:
	InputCommand read_input();
	InputCommand read_argv(char *argv[]);
	void prompt();
private:
  	std::string input;
};

InputCommand InputHandler::read_input() {
	std::getline(std::cin, input);
	return InputCommand(input);
}

InputCommand InputHandler::read_argv(char *argv[]) {
	return InputCommand(argv);
}

void InputHandler::prompt() {
  	printf("😎-> ");
}

// ------------------------------------------


int main(int argc, char *argv[]) {
  InputHandler input_handler;

  if (argc > 2 && argv != NULL) {
	  InputCommand command = input_handler.read_argv(argv);
	  exit( command.execute() );
  }

  pid_t pid;
  int pid_status;

  std::cout << welcome << std::endl;

  while(!feof(stdin) && !ferror(stdin))
  {
	input_handler.prompt();
	InputCommand command = input_handler.read_input();

	if (command.is_builtin()) {
		command.execute();
		continue;
	}

	pid = fork();

     	if (pid == 0) {
		exit( command.execute() );
    	} else {
		if (waitpid(pid, &pid_status, 0) > 0) {
			if (WIFEXITED(pid_status) && !WEXITSTATUS(pid_status))
				printf("\n");
			else if (!feof(stdin))
				printf("🤚 invalid command: %s\n", command.cmd.c_str());
		}
	}

  }

  printf("\n----- 😎 Goodbye 😎 -----\n");
  return 0;
}
