#include "MyWorld.h"
#include "core/runner.h"

int main(){
    MyWorld newWorld;
    return run(newWorld, "worldConfig.txt");
}