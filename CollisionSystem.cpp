#include "CollisionSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include <iostream>

CollisionSystem::CollisionSystem(World* w){
    mWorld = w;
    neededComponents[type_id<TransformComponent>()] = 1;
    neededComponents[type_id<ColliderComponent>()] = 1;
}

void CollisionSystem::init(){

}

void CollisionSystem::update(float deltaTime){
    vector<int> entityList(entities.begin(), entities.end());
    for (int x = 0; x < entityList.size(); x++){
        TransformComponent &tc1 = mWorld->getComponent<TransformComponent>(entityList[x]);
        ColliderComponent &cc1 = mWorld->getComponent<ColliderComponent>(entityList[x]);
        PhysicsComponent &pc1 = mWorld->getComponent<PhysicsComponent>(entityList[x]);
        for (int y = x+1; y < entityList.size(); y++){
            TransformComponent &tc2 = mWorld->getComponent<TransformComponent>(entityList[y]);
            ColliderComponent &cc2 = mWorld->getComponent<ColliderComponent>(entityList[y]);

            bool collided = false;
            float distBtwnObjects = FLT_MAX;
            collisionInfo res;
            if (cc1.boxMin != cc1.boxMax && cc2.boxMin != cc2.boxMax){
                //AABB va AABB
            }
            else if(cc1.sphereRadius != 0 && cc2.sphereRadius != 0){
                float totalR = cc1.sphereRadius + cc2.sphereRadius;
                float distance = glm::distance(tc1.worldPosition, tc2.worldPosition);
                distBtwnObjects = distance - totalR;
                collided = distance < totalR;
                if (collided){
                    res.penetrationDist = abs(distBtwnObjects);
                    res.normal = glm::normalize(tc1.worldPosition - tc2.worldPosition);
                }
            }
            else {
                // Sphere vs AABB collision check
            }

            if (collided){  
                set<int>::iterator historyEnter = cc1.collisionEnter.find(entityList[y]);
                set<int>::iterator historyStay = cc1.collisionStay.find(entityList[y]);
                if (historyEnter != cc1.collisionEnter.end()){
                    cc1.collisionStay.insert(entityList[y]);
                    cc2.collisionStay.insert(entityList[x]);
                    cc1.collisionEnter.erase(historyEnter);
                    cc2.collisionEnter.erase(cc2.collisionEnter.find(entityList[x]));
                    std::cout << "Enter to Stay" << std::endl;
                }
                else if (historyStay != cc1.collisionStay.end()){
                    std::cout << "Stay to Stay" << std::endl;
                }
                else{
                    cc1.collisionEnter.insert(entityList[y]);
                    cc2.collisionEnter.insert(entityList[x]);
                    std::cout << "Enter" << std::endl;
                }

                if (cc1.isTrigger || cc2.isTrigger)
                    continue;
                else{
                    PhysicsComponent &pc2 = mWorld->getComponent<PhysicsComponent>(entityList[y]);
                    float invMass1 = pc1.isKinematic ? 1.0/FLT_MAX : pc1.invMass;
                    float invMass2 = pc2.isKinematic ? 1.0/FLT_MAX : pc2.invMass;

                    glm::vec3 relativeVelocity = pc2.velocity - pc1.velocity;
                    float velocityAlongNormal = glm::dot(relativeVelocity, res.normal);
                    if (velocityAlongNormal < 0)
                        continue;
                    float e = min(pc1.restitutionCoefficient, pc2.restitutionCoefficient);

                    float j = -(1+e) * velocityAlongNormal;
                    j /= invMass1 + invMass2;
                    
                    glm::vec3 impulse = j * res.normal;
                    pc1.velocity -= invMass1 * impulse;
                    pc2.velocity += invMass2 * impulse;

                    float percent = 0.2;
                    float thresh = 0.01; 
                    glm::vec3 correction = max(res.penetrationDist - thresh, 0.0f) / (invMass1 + invMass2) * percent * res.normal;
                    tc1.position += invMass1 * correction;
                    tc2.position -= invMass2 * correction;
                }
            }
            else{
                set<int>::iterator historyEnter = cc1.collisionEnter.find(entityList[y]);
                set<int>::iterator historyStay = cc1.collisionStay.find(entityList[y]);
                set<int>::iterator historyExit = cc1.collisionExit.find(entityList[y]);
                if (historyEnter != cc1.collisionEnter.end()){
                    if (distBtwnObjects < .00005){
                        cc1.collisionStay.insert(entityList[y]);
                        cc2.collisionStay.insert(entityList[x]);
                        cc1.collisionEnter.erase(historyEnter);
                        cc2.collisionEnter.erase(cc2.collisionEnter.find(entityList[x]));
                        std::cout << "Enter to Stay" << std::endl;
                    }
                    else{
                        cc1.collisionExit.insert(entityList[y]);
                        cc2.collisionExit.insert(entityList[x]);
                        cc1.collisionEnter.erase(historyEnter);
                        cc2.collisionEnter.erase(cc2.collisionEnter.find(entityList[x]));
                        std::cout << "Enter to Exit" << std::endl;   
                    }
                }
                else if (historyStay != cc1.collisionStay.end()){
                    if (distBtwnObjects < .00005){
                        std::cout << "Stay to Stay" << std::endl;
                    }
                    else{
                        cc1.collisionExit.insert(entityList[y]);
                        cc2.collisionExit.insert(entityList[x]);
                        cc1.collisionStay.erase(historyStay);
                        cc2.collisionStay.erase(cc2.collisionStay.find(entityList[x])); 
                        std::cout << "Stay to Exit" << std::endl;  
                    }
                }
                else if (historyExit != cc1.collisionExit.end()){
                    cc1.collisionExit.erase(historyExit);
                    cc2.collisionExit.erase(cc2.collisionExit.find(entityList[x])); 
                    std::cout << "Post Exit" << std::endl;                 
                }
            }
        }
    }
}