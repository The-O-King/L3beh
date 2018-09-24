#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "system.h"
#include "world.h"

System::System(){
    neededComponents.set();
}

void System::update(double deltaTime){

}

RenderSystem::RenderSystem(World* w){
    mWorld = w;
}

void RenderSystem::update(double deltaTime){
    for (int e : entities){
        RenderComponent rc = mWorld->getComponent<RenderComponent>(e);
        TransformComponent tc = mWorld->getComponent<TransformComponent>(e); 
    }
}