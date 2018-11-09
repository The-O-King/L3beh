#include "CollisionSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include <iostream>

CollisionSystem::CollisionSystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<TransformComponent>()] = 1;
    main[type_id<ColliderComponent>()] = 1;
    neededComponentSignatures.push_back(main);
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
            PhysicsComponent &pc2 = mWorld->getComponent<PhysicsComponent>(entityList[y]);
            if (pc1.type == PhysicsType::STATIC && pc2.type == PhysicsType::STATIC)
                continue;

            bool collided = false;
            float distBtwnObjects = FLT_MAX;
            collisionInfo res;
            if (cc1.boxMin != cc1.boxMax && cc2.boxMin != cc2.boxMax){
                glm::vec3 trueMin1 = tc2.worldPosition - cc1.boxMin;
                glm::vec3 trueMax1 = tc2.worldPosition + cc1.boxMax;
                glm::vec3 trueMin2 = tc2.worldPosition - cc2.boxMin;
                glm::vec3 trueMax2 = tc2.worldPosition + cc2.boxMax;

                if ((trueMin1.x <= trueMax2.x && trueMax1.x >= trueMin2.x) &&
                    (trueMin1.y <= trueMax2.y && trueMax1.y >= trueMin2.y) &&
                    (trueMin1.z <= trueMax2.z && trueMax1.z >= trueMin2.z)){
                        // collision occurred
                        // get penetration dist and collision normal
                    }
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
                glm::vec3 trueMin;
                glm::vec3 trueMax;
                glm::vec3 trueCenter;
                float trueRadius;
                float swap = 1;
                if (cc1.sphereRadius != 0){
                    trueMin = tc2.worldPosition + cc2.boxMin; trueMax = tc2.worldPosition + cc2.boxMax;
                    trueCenter = tc1.worldPosition;
                    trueRadius = cc1.sphereRadius;
                }
                else {
                    trueMin = tc1.worldPosition + cc1.boxMin; trueMax = tc1.worldPosition + cc1.boxMax;
                    trueCenter = tc2.worldPosition;
                    trueRadius = cc2.sphereRadius;
                    swap = -1;
                }

                glm::vec3 p = glm::clamp(trueCenter, trueMin, trueMax);
                float distance = glm::distance(p, trueCenter);
                if (distance < trueRadius){
                    collided = true;
                    res.normal = swap * glm::normalize(trueCenter - p);
                    res.penetrationDist = abs(distance - trueRadius);
                }
            }

            if (collided){  
                set<int>::iterator historyEnter = cc1.collisionEnter.find(entityList[y]);
                set<int>::iterator historyStay = cc1.collisionStay.find(entityList[y]);
                if (historyEnter != cc1.collisionEnter.end()){
                    cc1.collisionStay.insert(entityList[y]);
                    cc2.collisionStay.insert(entityList[x]);
                    cc1.collisionEnter.erase(historyEnter);
                    cc2.collisionEnter.erase(cc2.collisionEnter.find(entityList[x]));
                    //std::cout << "Enter to Stay" << std::endl;
                }
                else if (historyStay != cc1.collisionStay.end()){
                    //std::cout << "Stay to Stay" << std::endl;
                }
                else{
                    cc1.collisionEnter.insert(entityList[y]);
                    cc2.collisionEnter.insert(entityList[x]);
                    //std::cout << "Enter" << std::endl;
                }

                if (cc1.isTrigger || cc2.isTrigger || (pc1.type == PhysicsType::KINEMATIC && pc2.type == PhysicsType::KINEMATIC))
                    continue;
                else if (pc1.type == PhysicsType::KINEMATIC && pc2.type == PhysicsType::STATIC){
                    tc1.position += .8f * res.penetrationDist * res.normal;

                }
                else if (pc1.type == PhysicsType::STATIC && pc2.type == PhysicsType::KINEMATIC){
                    tc2.position -= .8f * res.penetrationDist * res.normal;
                }
                else{
                    float invMass1 = pc1.type == PhysicsType::STATIC || pc1.type == PhysicsType::KINEMATIC ? 0 : pc1.invMass;
                    float invMass2 = pc2.type == PhysicsType::STATIC || pc2.type == PhysicsType::KINEMATIC ? 0 : pc2.invMass;
                    
                    glm::vec3 relativeVelocity = pc2.velocity - pc1.velocity;
                    float velocityAlongNormal = glm::dot(relativeVelocity, res.normal);
                    if (velocityAlongNormal < 0){
                        float percent = 0.8;
                        float thresh = 0.01; 
                        glm::vec3 correction = max(res.penetrationDist - thresh, 0.0f) / (invMass1 + invMass2) * percent * res.normal;
                        if (pc1.type != PhysicsType::STATIC){
                            tc1.position += invMass1 * correction;
                        }
                        if (pc2.type != PhysicsType::STATIC){
                            tc2.position -= invMass2 * correction;
                        }
                        continue;                                              
                    }
                    float e = min(pc1.restitutionCoefficient, pc2.restitutionCoefficient);

                    float j = -(1+e) * velocityAlongNormal;
                    j /= invMass1 + invMass2;
                    glm::vec3 impulse = j * res.normal;

                    float percent = 0.8;
                    float thresh = 0.01; 
                    glm::vec3 correction = max(res.penetrationDist - thresh, 0.0f) / (invMass1 + invMass2) * percent * res.normal;
                    
                    if (pc1.type != PhysicsType::STATIC){
                        pc1.velocity -= invMass1 * impulse;
                        tc1.position += invMass1 * correction;
                    }
                    if (pc2.type != PhysicsType::STATIC){
                        pc2.velocity += invMass2 * impulse;
                        tc2.position -= invMass2 * correction;
                    }
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
                        //std::cout << "Enter to Stay" << std::endl;
                    }
                    else{
                        cc1.collisionExit.insert(entityList[y]);
                        cc2.collisionExit.insert(entityList[x]);
                        cc1.collisionEnter.erase(historyEnter);
                        cc2.collisionEnter.erase(cc2.collisionEnter.find(entityList[x]));
                        //std::cout << "Enter to Exit" << std::endl;   
                    }
                }
                else if (historyStay != cc1.collisionStay.end()){
                    if (distBtwnObjects < .00005){
                        //std::cout << "Stay to Stay" << std::endl;
                    }
                    else{
                        cc1.collisionExit.insert(entityList[y]);
                        cc2.collisionExit.insert(entityList[x]);
                        cc1.collisionStay.erase(historyStay);
                        cc2.collisionStay.erase(cc2.collisionStay.find(entityList[x])); 
                        //std::cout << "Stay to Exit" << std::endl;  
                    }
                }
                else if (historyExit != cc1.collisionExit.end()){
                    cc1.collisionExit.erase(historyExit);
                    cc2.collisionExit.erase(cc2.collisionExit.find(entityList[x])); 
                    //std::cout << "Post Exit" << std::endl;                 
                }
            }
        }
    }
}