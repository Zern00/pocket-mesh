#include <iostream>
#include <string_view>

int main(const int argc, const char* const argv[]) {
    if (argc == 2 && std::string_view{argv[1]} == "--version") {
        std::cout << "pocketscene-server 0.1.0\n";
        return 0;
    }

    std::cout << "PocketScene backend skeleton. HTTP transport is not implemented yet.\n";
    return 0;
}
