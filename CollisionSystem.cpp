#include "CollisionSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <vector>

#define PERCENT .95f
#define THRESH .01f

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

            collisionInfo res;
            bool collided = collisionTestTable[cc1.type][cc2.type](entity1ID, tc1, entity2ID, tc2, res, mWorld);

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
                //Just stop from penetrating
                else if (pc1.type == PhysicsType::KINEMATIC && pc2.type == PhysicsType::STATIC){
                    float maxDist = -FLT_MAX;
                    for (contactPoint cp: res.points){
                        if (cp.penetrationDist > maxDist){
                            maxDist = cp.penetrationDist;
                        }
                    }
                    tc1.position += PERCENT * maxDist * res.normal;

                }
                //Just stop from penetrating
                else if (pc1.type == PhysicsType::STATIC && pc2.type == PhysicsType::KINEMATIC){
                    float maxDist = -FLT_MAX;
                    for (contactPoint cp: res.points){
                        if (cp.penetrationDist > maxDist){
                            maxDist = cp.penetrationDist;
                        }
                    }
                    tc2.position -= PERCENT * maxDist * res.normal;
                }
                //Actually Do Collision Resolution
                else{
                    float maxDist = -FLT_MAX;
                    for (contactPoint cp: res.points){
                        if (cp.penetrationDist > maxDist){
                            maxDist = cp.penetrationDist;
                        }
                    }

                    float invMass1, invMass2;
                    glm::mat3 invInertia1, invInertia2;
                    if (pc1.type != PhysicsType::DYNAMIC){
                        invMass1 = 0; 
                        invInertia1 = glm::mat3(0);
                    }
                    else{
                        invMass1 = pc1.invMass;
                        invInertia1 = glm::mat3(6);
                        glm::mat3 rotation1 = tc1.getRotation();
                        invInertia1 = rotation1 * invInertia1 * glm::transpose(rotation1);
                    }
                    if (pc2.type != PhysicsType::DYNAMIC){
                        invMass2 = 0; 
                        invInertia2 = glm::mat3(0);
                    }
                    else{
                        invMass2 = pc1.invMass;
                        invInertia2 = glm::mat3(6);
                        glm::mat3 rotation2 = tc2.getRotation();
                        invInertia2 = rotation2 * invInertia2 * glm::transpose(rotation2);
                    }
                    //Positional Correction
                    glm::vec3 relativeVelocity = pc2.velocity - pc1.velocity;
                    float velocityAlongNormal = glm::dot(relativeVelocity, res.normal);
                    glm::vec3 correction = max(maxDist - THRESH, 0.0f) / (invMass1 + invMass2) * PERCENT * res.normal;
                    if (pc1.type != PhysicsType::STATIC)
                            tc1.position += invMass1 * correction;
                    if (pc2.type != PhysicsType::STATIC)
                            tc2.position -= invMass2 * correction;

                    if (velocityAlongNormal >= 0){
                        float ratio = 1.0f/res.points.size();
                        float e = min(pc1.restitutionCoefficient, pc2.restitutionCoefficient);
                        float j = -(1+e) * velocityAlongNormal;
                        j /= invMass1 + invMass2;
                        glm::vec3 impulse = j * res.normal;
                    
                        if (pc1.type != PhysicsType::STATIC){
                            pc1.velocity -= invMass1 * impulse;
                        }
                        if (pc2.type != PhysicsType::STATIC){
                            pc2.velocity += invMass2 * impulse;
                        }
                        glm::vec3 totalAngular1(0, 0, 0);
                        glm::vec3 totalAngular2(0, 0, 0);
                        for (contactPoint cp : res.points){
                            glm::vec3 r1 = cp.point - tc1.worldPosition;
                            glm::vec3 r2 = cp.point - tc2.worldPosition;
                            glm::vec3 crossPoint1 = invInertia1 * glm::cross(r1, res.normal);
                            glm::vec3 crossPoint2 = invInertia2 * glm::cross(r2, res.normal);
                            glm::vec3 relativeVelocityAngular = relativeVelocity + glm::cross(pc2.angularVelocity * .01745f, r2) - glm::cross(pc1.angularVelocity * .017453f, r1); 
                            float j = -(1+e) * glm::dot(res.normal, relativeVelocityAngular);
                            //Fudge factor for smoother simulation
                            if (j < -.0001){
                                j /= invMass1 + invMass2 + glm::dot(glm::cross(crossPoint1, r1) + glm::cross(crossPoint2, r2), res.normal);

                                if (pc1.type != PhysicsType::STATIC){
                                    totalAngular1 -= crossPoint1 * j;
                                }
                                if (pc2.type != PhysicsType::STATIC){
                                    totalAngular2 += crossPoint2 * j;
                                }
                            }
                        }
                        pc1.angularVelocity += totalAngular1 * 57.29f;
                        pc2.angularVelocity += totalAngular2 * 57.29f;
                    }
                }
            }
            else{
                set<int>::iterator historyEnter = cc1.collisionEnter.find(entityList[y]);
                set<int>::iterator historyStay = cc1.collisionStay.find(entityList[y]);
                set<int>::iterator historyExit = cc1.collisionExit.find(entityList[y]);
                if (historyEnter != cc1.collisionEnter.end()){
                    if (res.distBetweenObjects < .0005){
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
                    if (res.distBetweenObjects < .0005){
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

glm::vec3 computeContactPointEdges(glm::vec3 e1[2], glm::vec3 e2[2]){
	glm::vec3 d1 = e1[1] - e1[0];
	glm::vec3 d2 = e2[1] - e2[0];
	glm::vec3 r = e1[0] - e2[0];
	float a = glm::dot( d1, d1 );
	float e = glm::dot( d2, d2 );
	float f = glm::dot( d2, r );
	float c = glm::dot( d1, r );

	float b = glm::dot( d1, d2 );
	float denom = a * e - b * b;

	float TA = (b * f - c * e) / denom;
	float TB = (b * TA + f) / e;

    return ((e1[0] + d1 * TA) + (e2[0] + d2 * TB))*.5f;
}

int sign(float num){
    if (num >= 0)
        return 1;
    else
        return -1;
}

void getEdge(glm::mat4 orientation, OBB obb, glm::vec3 normal, glm::vec3 outPoints[2], int axisEdge){
    normal = glm::inverse(glm::mat3(orientation)) * normal;

    if (axisEdge == 0){
        glm::vec3 possiblePoints[] = {
            {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z},
            {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z},
            {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z},
            {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z}
        };
        float extremeEdge = -FLT_MAX;
        int extremeVert = 5;
        for (int x = 0; x < 4; x++){
            float temp = glm::dot(possiblePoints[x], normal);
            if (temp > extremeEdge){
                extremeEdge = temp;
                extremeVert = x;
            }
        }
        outPoints[0] = possiblePoints[extremeVert];
        outPoints[1] = possiblePoints[extremeVert];
        outPoints[1].x *= -1;
    }
    else if (axisEdge == 1){
        glm::vec3 possiblePoints[] = {
            {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z},
            {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z},
            {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z},
            {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z}
        };
        float extremeEdge = -FLT_MAX;
        int extremeVert = 5;
        for (int x = 0; x < 4; x++){
            float temp = glm::dot(possiblePoints[x], normal);
            if (temp > extremeEdge){
                extremeEdge = temp;
                extremeVert = x;
            }
        }
        outPoints[0] = possiblePoints[extremeVert];
        outPoints[1] = possiblePoints[extremeVert];
        outPoints[1].y *= -1;
    }
    else{
        glm::vec3 possiblePoints[] = {
            {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z},
            {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z},
            {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z},
            {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z}
        };
        float extremeEdge = -FLT_MAX;
        int extremeVert = 5;
        for (int x = 0; x < 4; x++){
            float temp = glm::dot(possiblePoints[x], normal);
            if (temp > extremeEdge){
                extremeEdge = temp;
                extremeVert = x;
            }
        }
        outPoints[0] = possiblePoints[extremeVert];
        outPoints[1] = possiblePoints[extremeVert];
        outPoints[1].z *= -1;
    }

    outPoints[0] = orientation * glm::vec4(outPoints[0], 1);
    outPoints[1] = orientation * glm::vec4(outPoints[1], 1);
}

glm::vec3 intersection(glm::vec3 start, glm::vec3 end, glm::vec3 point, glm::vec3 normal){
    glm::vec3 direction = end - start;
    float d = glm::dot(point - start, normal) / glm::dot(direction, normal);
    return d * direction + start;
}

std::vector<contactPoint> clipVertices(glm::vec3 rFaceVertices[4], glm::vec3 rFaceNormal, glm::vec3 iFaceVertices[4], glm::vec3 iFaceNormal){
    std::vector<glm::vec3> outList = std::vector<glm::vec3>(iFaceVertices, iFaceVertices + 4);
    for (int x = 0; x < 4; x++){
        glm::vec3 fStart = rFaceVertices[x];
        glm::vec3 fEnd = rFaceVertices[(x+1) % 4];
        glm::vec3 normal = glm::cross(fEnd - fStart, rFaceNormal);
        
        std::vector<glm::vec3> inList = outList;
        outList.clear();
        glm::vec3 S = inList.back();
        for(int y = 0; y < inList.size(); y++){
            glm::vec3 E = inList[y];
            if (glm::dot(E - fStart, normal) <= 0){
                if (glm::dot(S - fStart, normal) > 0){
                    outList.push_back(intersection(S, E, fStart, normal));
                }
                outList.push_back(E);
            }
            else if (glm::dot(S - fStart, normal) <= 0){
                outList.push_back(intersection(S, E, fStart, normal));
            }
            S = E;
        }
    }
    std::vector<contactPoint> result;
    for (int x = 0; x < outList.size(); x++){
        glm::vec3 relativeVec = outList[x] - rFaceVertices[0];
        float dist = glm::dot(relativeVec, rFaceNormal);
        if (dist < 0.01){
            contactPoint temp;
            temp.penetrationDist = abs(dist);
            temp.point = outList[x] - dist*rFaceNormal;
            result.push_back(temp);
        }
    }
    return result;
}

void getVerticesFromFaceNormal(glm::vec3 *outVertices, OBB obb, glm::mat4 orientation, int axis, bool opposite){
    if (axis == 0){
        if (!opposite){
            outVertices[0] = {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[1] = {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[2] = {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[3] = {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
        }
        else{
            outVertices[0] = {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[1] = {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[2] = {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[3] = {-obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
        }

    }
    else if (axis == 1){
        if (!opposite){
            outVertices[0] = {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[1] = {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[2] = {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[3] = {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
        }
        else{
            outVertices[0] = {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[1] = {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[2] = {-obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            outVertices[3] = {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
        }
    }
    else{
        if (!opposite){
            outVertices[0] = {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[1] = {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[2] = {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[3] = {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
        }
        else{
            outVertices[0] = {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            outVertices[1] = {-obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            outVertices[2] = {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[3] = {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z}; 
        }
    }
    for (int x = 0; x < 4; x++){
        outVertices[x] = orientation * glm::vec4(outVertices[x], 1);
    }
}

float separatingDistance(const glm::vec3& RPos, const glm::vec3& Plane, const OBB& obb1, const OBB&obb2){
    return (fabs(glm::dot(RPos,Plane))) -
           (fabs(glm::dot((obb1.AxisX*obb1.Half_size.x),Plane)) +
            fabs(glm::dot((obb1.AxisY*obb1.Half_size.y),Plane)) +
            fabs(glm::dot((obb1.AxisZ*obb1.Half_size.z),Plane)) +
            fabs(glm::dot((obb2.AxisX*obb2.Half_size.x),Plane)) + 
            fabs(glm::dot((obb2.AxisY*obb2.Half_size.y),Plane)) +
            fabs(glm::dot((obb2.AxisZ*obb2.Half_size.z),Plane)));
}

bool BoxVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
    BoxColliderComponent cc1 = mWorld->getComponent<BoxColliderComponent>(c1);
    BoxColliderComponent cc2 = mWorld->getComponent<BoxColliderComponent>(c2);
    glm::mat4 orientation1 = tc1.getOrientation();
    glm::mat4 orientation2 = tc2.getOrientation();
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
    bool collided = true;
    for (int x = 0; x < 6; x++){
        if (faceDistances[x] > 0){
            collided = false;
        }
        if (faceDistances[x] > minPenetrationFace){
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
            }
            else if (edgeDistances[x] > minPenetrationEdge){
                minPenetrationEdge = edgeDistances[x];
                edgeMin = x;
            }
        }
    }
    res.distBetweenObjects = minPenetrationEdge > minPenetrationFace ? minPenetrationEdge : minPenetrationFace;

    if (collided){
        if (minPenetrationEdge * .95 > minPenetrationFace + .01){
            //Edge Collision
            res.normal = edgePlanes[edgeMin];
            if (glm::dot(tc2.worldPosition - tc1.worldPosition, res.normal) < 0)
                res.normal *= -1;
            
            int axisEdge1 = edgeMin / 3;
            int axisEdge2 = edgeMin % 3;
            glm::vec3 edge1[2];
            glm::vec3 edge2[2];
            getEdge(orientation1, obb1, res.normal, edge1, axisEdge1);
            getEdge(orientation2, obb2, -res.normal, edge2, axisEdge2);
            contactPoint temp;
            temp.point = computeContactPointEdges(edge1, edge2);
            temp.penetrationDist = abs(minPenetrationEdge);
            res.points.push_back(temp);
            res.normal *= -1;
        }
        else{
            //Face Collision
            glm::vec3 referenceFaceNormal = facePlanes[faceMin];
            int referenceFaceOpposite = false;
            if (glm::dot(tc2.worldPosition - tc1.worldPosition, referenceFaceNormal) < 0){
                referenceFaceNormal *= -1;
                referenceFaceOpposite = true;
            }

            glm::vec3 incidentFaceNormal;
            glm::vec3 incidentFaceVertices[4];
            glm::vec3 referenceFaceVertices[4];

            if (faceMin < 3){
                float min = FLT_MAX;
                bool opposite = false;
                int axis = 0;
                for (int x = 0; x < 3; x++){
                    float dotRes = glm::dot(facePlanes[3+x], referenceFaceNormal);
                    if (dotRes < min){
                        min = dotRes;
                        incidentFaceNormal = facePlanes[3+x];
                        opposite = false;
                        axis = x;
                    }
                    if (-dotRes < min){
                        min = -dotRes;
                        incidentFaceNormal = -facePlanes[3+x];
                        opposite = true;
                        axis = x;
                    }
                }
                getVerticesFromFaceNormal(incidentFaceVertices, obb2, orientation2, axis, opposite);
                getVerticesFromFaceNormal(referenceFaceVertices, obb1, orientation1, faceMin, referenceFaceOpposite);
                res.points = clipVertices(referenceFaceVertices, referenceFaceNormal, incidentFaceVertices, incidentFaceNormal);
                res.normal = referenceFaceNormal;
            }
            else{
                referenceFaceNormal *= -1;
                referenceFaceOpposite = !referenceFaceOpposite;
                float min = FLT_MAX;
                bool opposite = false;
                int axis = 0;
                for (int x = 0; x < 3; x++){
                    float dotRes = glm::dot(facePlanes[x], referenceFaceNormal);
                    if (dotRes < min){
                        min = dotRes;
                        incidentFaceNormal = facePlanes[x];
                        opposite = false;
                        axis = x;
                    }
                    if (-dotRes < min){
                        min = -dotRes;
                        incidentFaceNormal = -facePlanes[x];
                        opposite = true;
                        axis = x;
                    }
                }
                getVerticesFromFaceNormal(incidentFaceVertices, obb1, orientation1, axis, opposite);
                getVerticesFromFaceNormal(referenceFaceVertices, obb2, orientation2, faceMin % 3, referenceFaceOpposite);
                res.points = clipVertices(referenceFaceVertices, referenceFaceNormal, incidentFaceVertices, incidentFaceNormal);
                res.normal = referenceFaceNormal * -1.0f;
            }
            res.normal *= -1.0f;
        }
        return true;
    }
    else{
        return false;
    }
}

bool SphereVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
    SphereColliderComponent cc1 = mWorld->getComponent<SphereColliderComponent>(c1);;
    SphereColliderComponent cc2 = mWorld->getComponent<SphereColliderComponent>(c2);;
    float totalR = cc1.radius + cc2.radius;
    float distance = glm::distance(tc1.worldPosition, tc2.worldPosition);
    float distBtwnObjects = distance - totalR;
    bool collided = distance < totalR;
    res.distBetweenObjects = distBtwnObjects;
    if (collided){
        contactPoint temp;
        temp.point = (tc2.worldPosition + tc1.worldPosition) * .5f;
        temp.penetrationDist = abs(distBtwnObjects);
        res.points.push_back(temp);
        res.normal = glm::normalize(tc1.worldPosition - tc2.worldPosition);
        return true;
    }
    else{
        return false;
    }
}

bool SphereVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
    SphereColliderComponent cc1 = mWorld->getComponent<SphereColliderComponent>(c1);
    BoxColliderComponent cc2 = mWorld->getComponent<BoxColliderComponent>(c2);
    glm::vec3 trueCenter = tc1.worldPosition;
    float trueRadius = cc1.radius;

    glm::mat4 boxOrientation = tc2.getOrientation();
    trueCenter = glm::inverse(boxOrientation) * glm::vec4(trueCenter, 1);

    glm::vec3 p = glm::clamp(trueCenter, cc2.boxMin, cc2.boxMax);
    float distance = glm::distance(p, trueCenter);
    res.distBetweenObjects = distance - trueRadius;
    if (distance < trueRadius){
        contactPoint temp;
        temp.point = boxOrientation * glm::vec4(p, 1);
        temp.penetrationDist = abs(distance - trueRadius);
        res.points.push_back(temp);
        res.normal = boxOrientation * glm::vec4(glm::normalize(trueCenter - p), 0);
        return true;
    }
    else{
        return false;
    }
}

bool BoxVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
    BoxColliderComponent cc1 = mWorld->getComponent<BoxColliderComponent>(c1);
    SphereColliderComponent cc2 = mWorld->getComponent<SphereColliderComponent>(c2);
    glm::vec3 trueCenter = tc2.worldPosition;
    float trueRadius = cc2.radius;

    glm::mat4 boxOrientation = tc1.getOrientation();
    trueCenter = glm::inverse(boxOrientation) * glm::vec4(trueCenter, 1);

    glm::vec3 p = glm::clamp(trueCenter, cc1.boxMin, cc1.boxMax);
    float distance = glm::distance(p, trueCenter);
    res.distBetweenObjects = distance - trueRadius;
    if (distance < trueRadius){
        contactPoint temp;
        temp.point = boxOrientation * glm::vec4(p, 1);
        temp.penetrationDist = abs(distance - trueRadius);
        res.points.push_back(temp);
        res.normal = -(boxOrientation * glm::vec4(glm::normalize(trueCenter - p), 0));
        return true;
    }
    else{
        return false;
    }
}