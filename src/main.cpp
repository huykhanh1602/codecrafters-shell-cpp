#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;
std::string builtin[] = {"echo", "type", "exit"};

std::string is_executable(const fs::path &path) {
    char *env_path = std::getenv("PATH");
    std::vector<std::string> dirs;
    if (env_path == nullptr)
        return "";
    std::string_view path_env(env_path);
    size_t start = 0;
    while (true) {
        size_t end = path_env.find(':', start);
        if (end == std::string_view::npos) {
            dirs.push_back(std::string(path_env.substr(start)));
            break;
        }
        dirs.push_back(std::string(path_env.substr(start, end - start)));
        start = end + 1;
    }
    // std::cout << "Checking for " << path << " in PATH directories:" << std::endl;
    for (const auto &dir : dirs) {
        fs::path full_path = fs::path(dir) / path;
        // std::cout << "  " << full_path << std::endl;
        if (fs::exists(full_path) && fs::is_regular_file(full_path) && (fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none) {
            std::string(full_path);
            return dir + "/" + path.filename().string();
            // std::cout << "Found executable: " << full_path << std::endl;
        }
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