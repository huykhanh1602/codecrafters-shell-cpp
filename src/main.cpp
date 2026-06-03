#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
const char PATH_SEPARATOR = ';';
#else
const char PATH_SEPARATOR = ':';
#endif

namespace fs = std::filesystem;
std::string builtin[] = {"echo", "type", "exit"};

/**
 * @brief Checks if a file is executable
 * @param path The path to check
 * @return The full path if the file is executable, otherwise an empty string
 */
std::string is_executable(const fs::path &path) {
    // Get the PATH environment variable
    char *env_path = std::getenv("PATH");
    if (env_path == nullptr)
        return "";
    std::string_view path_env(env_path);

    // Split the PATH variable and check each directory for the executable
    size_t start = 0;
    size_t end;
#ifdef _WIN32
    // on Windows, checking for .exe extension if not provided
    std::cout << "win exe check for " << path << std::endl;
    if (path.extension() != ".exe") {
        return is_executable(path.string() + ".exe");
    }
#endif
    // Iterate through each directory in PATH
    while ((end = path_env.find(PATH_SEPARATOR, start)) != std::string_view::npos) {
        fs::path full_path = fs::path(path_env.substr(start, end - start)) / path;
        std::cout << "Checking " << full_path << std::endl;
        if (fs::exists(full_path) && fs::is_regular_file(full_path) && ((fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none)) {
            return full_path.string();
        }
        start = end + 1;
    }
    return "";
}

/**
 * @brief Checks if a command is a builtin or an executable and prints its type
 * @param input The command to check
 */
void type(std::string input) {
    bool found = false;

    // check if the command is a builtin
    for (const auto &cmd : builtin) {
        if (input == cmd) {
            found = true;
            break;
        }
    }

    // check if the command is an executable in PATH
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
        } else if (command == "") {
            // Do nothing for empty commands
        } else if (std::string exec_path = is_executable(command); !exec_path.empty()) {
            std::cout << command << " is " << exec_path << std::endl;
        } else {
            std::cout << command << ": command not found" << std::endl;
        }
    }
}