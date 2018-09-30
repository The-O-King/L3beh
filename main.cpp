#include "core/world.h"
#include "core/runner.h"

int main(){
    World newWorld;
    return run(newWorld, "worldConfig.txt");
}