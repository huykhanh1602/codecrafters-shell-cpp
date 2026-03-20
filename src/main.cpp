#include <iostream>
#include <string>

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";
        std::string command;
        std::getline(std::cin, command);

        std::string builtin[] = {"echo", "type", "exit"};
        if (command == "exit") {
            break;
        } else if (command.substr(0, 5) == "echo ") {
            std::cout << command.substr(5) << std::endl;
        } else if (command.substr(0, 5) == "type ") {
            bool found = false;
            for (const auto &cmd : builtin) {
                if (command.substr(5) == cmd) {
                    std::cout << command.substr(5) << " is a shell builtin" << std::endl;
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << command.substr(5) << ": not found" << std::endl;
            }
        } else {
            std::cout << command << ": command not found" << std::endl;
        }
    }
}