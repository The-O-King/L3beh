#include "CollisionSystem.h"
#include "CollisionSystemUtils.hpp"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <vector>

collisionFunction collisionTestTable[2][2] = {
    {BoxVsBox, BoxVsSphere},
    {SphereVsBox, SphereVsSphere}
};

CollisionSystem::CollisionSystem(World* w){
    mWorld = w;
    componentSignature box;
    box[type_id<TransformComponent>()] = 1;
    box[type_id<BoxColliderComponent>()] = 1;
    box[type_id<PhysicsComponent>()] = 1;
    neededComponentSignatures.push_back(box);

    componentSignature sphere;
    sphere[type_id<TransformComponent>()] = 1;
    sphere[type_id<SphereColliderComponent>()] = 1;
    sphere[type_id<PhysicsComponent>()] = 1;
    neededComponentSignatures.push_back(sphere);
}

void CollisionSystem::init(){

}

void CollisionSystem::addEntity(int entityID, componentSignature sig){
    entities.emplace(entityID);
    // Box Collider
    if ((sig & neededComponentSignatures[0]) == sig){
        PhysicsComponent &pc1 = mWorld->getComponent<PhysicsComponent>(entityID);
        BoxColliderComponent &bc1 = mWorld->getComponent<BoxColliderComponent>(entityID);
        float constFactor = pc1.invMass * 12.0;
        pc1.invInertia[0][0] = constFactor / (bc1.halfSize.y * bc1.halfSize.y + bc1.halfSize.z * bc1.halfSize.z);
        pc1.invInertia[1][1] = constFactor / (bc1.halfSize.x * bc1.halfSize.x + bc1.halfSize.z * bc1.halfSize.z);
        pc1.invInertia[2][2] = constFactor / (bc1.halfSize.x * bc1.halfSize.x + bc1.halfSize.y * bc1.halfSize.y);
    }
    // Sphere Collider
    else{
        PhysicsComponent &pc1 = mWorld->getComponent<PhysicsComponent>(entityID);
        SphereColliderComponent &sc1 = mWorld->getComponent<SphereColliderComponent>(entityID);
        float constFactor = pc1.invMass * 5 / (2 * sc1.radius * sc1.radius);
        pc1.invInertia[0][0] = constFactor;
        pc1.invInertia[1][1] = constFactor;
        pc1.invInertia[2][2] = constFactor;
    }
}

void CollisionSystem::update(float deltaTime){
    vector<int> entityList(entities.begin(), entities.end());
    for (int x = 0; x < entityList.size(); x++){
        int entity1ID = entityList[x];
        TransformComponent &tc1 = mWorld->getComponent<TransformComponent>(entity1ID);
        ColliderComponent &cc1 = mWorld->getComponent<ColliderComponent>(entity1ID);
        PhysicsComponent &pc1 = mWorld->getComponent<PhysicsComponent>(entity1ID);
        for (int y = x+1; y < entityList.size(); y++){
            int entity2ID = entityList[y];
            TransformComponent &tc2 = mWorld->getComponent<TransformComponent>(entity2ID);
            ColliderComponent &cc2 = mWorld->getComponent<ColliderComponent>(entity2ID);
            PhysicsComponent &pc2 = mWorld->getComponent<PhysicsComponent>(entity2ID);
            if (pc1.type == PhysicsType::STATIC && pc2.type == PhysicsType::STATIC)
                continue;

            collisionInfo resNew(entity1ID, entity2ID);
            collisionTestTable[cc1.type][cc2.type](entity1ID, tc1, entity2ID, tc2, resNew, mWorld);
            collisionInfoKey key(resNew);

            if (resNew.numContacts > 0){
                std::map<collisionInfoKey, collisionInfo>::iterator iter = persistentCollisionData.find(key);
                collisionInfo* res;
                if (iter == persistentCollisionData.end()){
                    persistentCollisionData[key] = resNew;
                    res = &persistentCollisionData[key];
                }
                else{
                    res = iter->second.update(resNew);
                }
                
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

                float invMass1 = pc1.invMass, invMass2 = pc2.invMass;
                glm::mat3 invInertia1 = pc1.invInertia, invInertia2 = pc2.invInertia;

                // Pre Solving Computations
                float e = min(pc1.restitutionCoefficient, pc2.restitutionCoefficient);
                float mu = sqrt(pc1.friction * pc2.friction);
                for (int w = 0; w < res->numContacts; w++){
                    contactPoint cp = res->points[w];
                    res->points[w].r1 = cp.point - tc1.worldPosition;
                    res->points[w].r2 = cp.point - tc2.worldPosition;
                    glm::vec3 crossPoint1 = glm::cross(res->points[w].r1, res->normal);
                    glm::vec3 crossPoint2 = glm::cross(res->points[w].r2, res->normal);
                    float kNormal = invMass1 + invMass2;
                    kNormal += glm::dot(crossPoint1, invInertia1 * crossPoint1) + glm::dot(crossPoint2, invInertia2 * crossPoint2);
                    res->points[w].massNormal = 1.0f / kNormal;
                    res->points[w].bias = -PERCENT / deltaTime * max(0.0f, res->points[w].penetrationDist - THRESH);

                    for (int z = 0; z < 2; z++){
                        glm::vec3 crossPoint1T = glm::cross(res->points[w].r1, res->tangent[z]);
                        glm::vec3 crossPoint2T = glm::cross(res->points[w].r2, res->tangent[z]);
                        float kTangent = invMass1 + invMass2;
                        kTangent += glm::dot(crossPoint1T, invInertia1 * crossPoint1T) + glm::dot(crossPoint2T, invInertia2 * crossPoint2T);
                        res->points[w].massTangent[z] = 1.0f / kTangent;
                    }

                    //Accumulated Impulses Warm Starting
                    glm::vec3 P = res->points[w].Pn * res->normal + res->points[w].Pt[0] * res->tangent[0] + res->points[w].Pt[1] * res->tangent[1];
                    pc1.velocity -= invMass1 * P;
                    pc1.angularVelocity -= invInertia1 * glm::cross(res->points[w].r1, P);
                    pc2.velocity += invMass2 * P;
                    pc2.angularVelocity += invInertia2 * glm::cross(res->points[w].r2, P);
                }

                for (int q = 0; q < 4; q++){
                    for (int w = 0; w < res->numContacts; w++){
                        // Normal Impulse
                        glm::vec3 relativeVelocity = pc2.velocity - pc1.velocity + glm::cross(pc2.angularVelocity, res->points[w].r2) - glm::cross(pc1.angularVelocity, res->points[w].r1);
                        float velocityAlongNormal = glm::dot(relativeVelocity, res->normal);
                        float j = (-velocityAlongNormal + res->points[w].bias) * res->points[w].massNormal;
                        
                        float temp = res->points[w].Pn;
                        res->points[w].Pn = min(temp + j, 0.0f);
                        j = res->points[w].Pn - temp;

                        glm::vec3 P = j * res->normal;
                        pc1.velocity -= invMass1 * P;
                        pc1.angularVelocity -= invInertia1 * glm::cross(res->points[w].r1, P);
                        pc2.velocity += invMass2 * P;
                        pc2.angularVelocity += invInertia2 * glm::cross(res->points[w].r2, P);

                        // Friction Impulse
                        relativeVelocity = pc2.velocity - pc1.velocity + glm::cross(pc2.angularVelocity, res->points[w].r2) - glm::cross(pc1.angularVelocity, res->points[w].r1);
                        for (int z = 0; z < 2; z++){
                            float velocityAlongTangent = glm::dot(relativeVelocity, res->tangent[z]); 
                            float j = -velocityAlongTangent * res->points[w].massTangent[z];
                            float maxJ = mu * res->points[w].Pn;

                            float temp = res->points[w].Pt[z];
                            res->points[w].Pt[z] = glm::clamp(temp + j, maxJ, -maxJ); // Negate the larger one because Pn is negative
                            j = res->points[w].Pt[z] - temp;

                            glm::vec3 P = j * res->tangent[z];
                            pc1.velocity -= invMass1 * P;
                            pc1.angularVelocity -= invInertia1 * glm::cross(res->points[w].r1, P);
                            pc2.velocity += invMass2 * P;
                            pc2.angularVelocity += invInertia2 * glm::cross(res->points[w].r2, P);
                        }
                    }
                }
            }
            else{
                persistentCollisionData.erase(key);

                set<int>::iterator historyEnter = cc1.collisionEnter.find(entityList[y]);
                set<int>::iterator historyStay = cc1.collisionStay.find(entityList[y]);
                set<int>::iterator historyExit = cc1.collisionExit.find(entityList[y]);
                if (historyEnter != cc1.collisionEnter.end()){
                    if (resNew.distBetweenObjects < .0005){
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
                    if (resNew.distBetweenObjects < .0005){
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