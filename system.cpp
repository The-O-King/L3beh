#include "system.h"
#include "world.h"

void System::update(double deltaTime){

}

RenderSystem::RenderSystem(World* w){
    mWorld = w;
}

void RenderSystem::update(double deltaTime){
    for (int e : entities){
        RenderComponent rc = mWorld->RenderData.getComponent(e);
        TransformComponent tc = mWorld->TransformData.getComponent(e);   
    }
}