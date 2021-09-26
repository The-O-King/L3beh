#include "core/runner.h"
#include "MyWorld.h"
#include <iostream>

int main(int argc, char *argv[]){
    if (argc < 2) {
        std::cout << "Pass in first world config txt file" << std::endl;
        return -1;
    }
    MyWorld newWorld;
    return run(newWorld, argv[1]);
}