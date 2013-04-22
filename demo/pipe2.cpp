#include <iostream>
#include <string>

int main(int argc, char **argv) {
    while(1) {
        std::string message;
        getline(std::cin, message);
        std::cout << "Prog 2: Prog 1 gave me: " << message << std::endl;
    }
    return 0;
}
