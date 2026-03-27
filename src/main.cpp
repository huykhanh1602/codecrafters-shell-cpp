#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
std::string builtin[] = {"echo", "type", "exit"};

std::string is_executable(const fs::path &path) {
    char *env_path = std::getenv("PATH");
    if (env_path == nullptr)
        return "";
    std::string_view path_env(env_path);
    size_t start = 0;
    size_t end;
    while ((end = path_env.find(':', start)) != std::string_view::npos) {
        fs::path full_path = fs::path(path_env.substr(start, end - start)) / path;
        if (fs::exists(full_path) && fs::is_regular_file(full_path) && ((fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none)) {
            return full_path.string();
        }
        start = end + 1;
    }
    return "";
}

void type(std::string input) {
    bool found = false;
    for (const auto &cmd : builtin) {
        if (input == cmd) {
            found = true;
            break;
        }
    }
    if (found) {
        std::cout << input << " is a shell builtin" << std::endl;
    } else if (const auto &exec_path = is_executable(input); !exec_path.empty()) {
        std::cout << input << " is " << exec_path << std::endl;
    } else {
        std::cout << input << ": not found" << std::endl;
    }
}

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";

        std::string command;
        std::getline(std::cin, command);

        std::string path = std::getenv("PATH");

        if (command == "exit") {
            break;
        } else if (command.substr(0, 5) == "echo ") {
            std::cout << command.substr(5) << std::endl;
        } else if (command.substr(0, 5) == "type ") {
            type(command.substr(5));
        } else {
            std::cout << command << ": command not found" << std::endl;
        }
    }
}