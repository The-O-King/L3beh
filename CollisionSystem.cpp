#include "CollisionSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/glm/gtc/matrix_transform.hpp"
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
                glm::mat4 orientation1 = glm::rotate(glm::mat4(1), (float)(glm::radians(tc1.worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
                orientation1 = glm::rotate(orientation1, (float)(glm::radians(tc1.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
                orientation1 = glm::rotate(orientation1, (float)(glm::radians(tc1.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0)); 
                glm::mat4 orientation2 = glm::rotate(glm::mat4(1), (float)(glm::radians(tc2.worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
                orientation2 = glm::rotate(orientation2, (float)(glm::radians(tc2.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
                orientation2 = glm::rotate(orientation2, (float)(glm::radians(tc2.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));
                OBB obb1, obb2;
                obb1.AxisX = glm::vec3(orientation1[0]);
                obb1.AxisY = glm::vec3(orientation1[1]);
                obb1.AxisZ = glm::vec3(orientation1[2]);
                obb1.Half_size = (cc1.boxMax - cc1.boxMin) * .5f;
                obb2.AxisX = glm::vec3(orientation2[0]);
                obb2.AxisY = glm::vec3(orientation2[1]);
                obb2.AxisZ = glm::vec3(orientation2[2]);
                obb2.Half_size = (cc2.boxMax - cc2.boxMin) * .5f;

                glm::vec3 RPos = tc2.worldPosition - tc1.worldPosition;
                glm::vec3 facePlanes[6] = { obb1.AxisX, obb1.AxisY, obb1.AxisZ, obb2.AxisX, obb2.AxisY, obb2.AxisZ };
                glm::vec3 edgePlanes[9] = { 
                    glm::normalize(glm::cross(obb1.AxisX,obb2.AxisX)), 
                    glm::normalize(glm::cross(obb1.AxisX,obb2.AxisY)),
                    glm::normalize(glm::cross(obb1.AxisX,obb2.AxisZ)),
                    glm::normalize(glm::cross(obb1.AxisY,obb2.AxisX)), 
                    glm::normalize(glm::cross(obb1.AxisY,obb2.AxisY)), 
                    glm::normalize(glm::cross(obb1.AxisY,obb2.AxisZ)),
                    glm::normalize(glm::cross(obb1.AxisZ,obb2.AxisX)), 
                    glm::normalize(glm::cross(obb1.AxisZ,obb2.AxisY)), 
                    glm::normalize(glm::cross(obb1.AxisZ,obb2.AxisZ))
                };
                float faceDistances[6] = { 
                    separatingDistance(RPos, facePlanes[0], obb1, obb2), 
                    separatingDistance(RPos, facePlanes[1], obb1, obb2), 
                    separatingDistance(RPos, facePlanes[2], obb1, obb2),
                    separatingDistance(RPos, facePlanes[3], obb1, obb2),
                    separatingDistance(RPos, facePlanes[4], obb1, obb2),
                    separatingDistance(RPos, facePlanes[5], obb1, obb2) 
                };
                float edgeDistances[9] = {
                    separatingDistance(RPos, edgePlanes[0], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[1], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[2], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[3], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[4], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[5], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[6], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[7], obb1, obb2),
                    separatingDistance(RPos, edgePlanes[8], obb1, obb2)
                };

                float minPenetrationFace = -FLT_MAX;
                int faceMin = 7;
                collided = true;
                for (int x = 0; x < 6; x++){
                    if (faceDistances[x] > 0){
                        collided = false;
                        break;
                    }
                    else if (faceDistances[x] > minPenetrationFace){
                        collided = true;
                        minPenetrationFace = faceDistances[x];
                        faceMin = x;
                    }
                }

                float minPenetrationEdge = -FLT_MAX;
                int edgeMin = 10;
                if (collided){
                    for (int x = 0; x < 9; x++){
                        if (edgeDistances[x] > 0){
                            collided = false;
                            break;
                        }
                        else if (edgeDistances[x] > minPenetrationEdge){
                            collided = true;
                            minPenetrationEdge = edgeDistances[x];
                            edgeMin = x;
                        }
                    }
                }

                if (collided){
                    if (minPenetrationEdge > minPenetrationFace){
                        std::cout << "Edge Collision" << std::endl;
                        res.normal = edgePlanes[edgeMin];
                        res.penetrationDist = abs(minPenetrationEdge);
                    }
                    else{
                        std::cout << "Face Collision" << std::endl;
                        res.normal = facePlanes[faceMin];
                        res.penetrationDist = abs(minPenetrationFace);
                    }
                    if (glm::dot(tc2.worldPosition - tc1.worldPosition, res.normal) > 0)
                        res.normal *= -1;
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

                glm::mat4 orientation1 = glm::rotate(glm::mat4(1), (float)(glm::radians(tc1.worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
                orientation1 = glm::rotate(orientation1, (float)(glm::radians(tc1.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
                orientation1 = glm::rotate(orientation1, (float)(glm::radians(tc1.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0)); 
                trueCenter = glm::transpose(glm::mat3(orientation1)) * trueCenter;

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
                    float invMass1 = pc1.type != PhysicsType::DYNAMIC ? 0 : pc1.invMass;
                    float invMass2 = pc2.type != PhysicsType::DYNAMIC ? 0 : pc2.invMass;
                    
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

float CollisionSystem::separatingDistance(const glm::vec3& RPos, const glm::vec3& Plane, const OBB& obb1, const OBB&obb2){
    return (fabs(glm::dot(RPos,Plane))) -
           (fabs(glm::dot((obb1.AxisX*obb1.Half_size.x),Plane)) +
            fabs(glm::dot((obb1.AxisY*obb1.Half_size.y),Plane)) +
            fabs(glm::dot((obb1.AxisZ*obb1.Half_size.z),Plane)) +
            fabs(glm::dot((obb2.AxisX*obb2.Half_size.x),Plane)) + 
            fabs(glm::dot((obb2.AxisY*obb2.Half_size.y),Plane)) +
            fabs(glm::dot((obb2.AxisZ*obb2.Half_size.z),Plane)));
}