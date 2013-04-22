#include <iostream>
#include <unistd.h>

int main(int argc, char **argv) {
    while(1) {
        std::cout << "Prog 1: I am awesome" << std::endl;
        sleep(1);
    }
    return 0;
}
