#include "CollisionSystem.h"
#include "CustomComponents.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <vector>

#define PERCENT .2f
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

            collisionInfo res(entity1ID, entity2ID);
            collisionTestTable[cc1.type][cc2.type](entity1ID, tc1, entity2ID, tc2, res, mWorld);
            collisionInfoKey key(res);

            if (res.numContacts > 0){
                std::map<collisionInfoKey, collisionInfo>::iterator iter = persistentCollisionData.find(key);
                if (iter == persistentCollisionData.end()){
                    persistentCollisionData[key] = res;
                }
                else{
                    iter->second.update(res);
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

                // Pre Solving Computations
                float e = min(pc1.restitutionCoefficient, pc2.restitutionCoefficient);
                float mu = sqrt(pc1.friction * pc2.friction);
                for (int w = 0; w < res.numContacts; w++){
                    contactPoint cp = res.points[w];
                    res.points[w].r1 = cp.point - tc1.worldPosition;
                    res.points[w].r2 = cp.point - tc2.worldPosition;
                    glm::vec3 crossPoint1 = glm::cross(res.points[w].r1, res.normal);
                    glm::vec3 crossPoint2 = glm::cross(res.points[w].r2, res.normal);
                    float kNormal = invMass1 + invMass2;
                    kNormal += glm::dot(crossPoint1, invInertia1 * crossPoint1) + glm::dot(crossPoint2, invInertia2 * crossPoint2);
                    res.points[w].massNormal = 1.0f / kNormal;
                    res.points[w].bias = -PERCENT / deltaTime * max(0.0f, res.points[w].penetrationDist - THRESH);

                    for (int z = 0; z < 2; z++){
                        glm::vec3 crossPoint1T = glm::cross(res.points[w].r1, res.tangent[z]);
                        glm::vec3 crossPoint2T = glm::cross(res.points[w].r2, res.tangent[z]);
                        float kTangent = invMass1 + invMass2;
                        kTangent += glm::dot(crossPoint1T, invInertia1 * crossPoint1T) + glm::dot(crossPoint2T, invInertia2 * crossPoint2T);
                        res.points[w].massTangent[z] = 1.0f / kTangent;
                    }
                }

                for (int q = 0; q < 4; q++){
                    for (int w = 0; w < res.numContacts; w++){
                        // Normal Impulse
                        glm::vec3 relativeVelocity = pc2.velocity - pc1.velocity + glm::cross(pc2.angularVelocity, res.points[w].r2) - glm::cross(pc1.angularVelocity, res.points[w].r1);
                        float velocityAlongNormal = glm::dot(relativeVelocity, res.normal);
                        float j = (-velocityAlongNormal + res.points[w].bias) * res.points[w].massNormal;
                        
                        float temp = res.points[w].Pn;
                        res.points[w].Pn = min(temp + j, 0.0f);
                        j = res.points[w].Pn - temp;

                        glm::vec3 P = j * res.normal;
                        pc1.velocity -= invMass1 * P;
                        pc1.angularVelocity -= invInertia1 * glm::cross(res.points[w].r1, P);
                        pc2.velocity += invMass2 * P;
                        pc2.angularVelocity += invInertia2 * glm::cross(res.points[w].r2, P);

                        // Friction Impulse
                        relativeVelocity = pc2.velocity - pc1.velocity + glm::cross(pc2.angularVelocity, res.points[w].r2) - glm::cross(pc1.angularVelocity, res.points[w].r1);
                        for (int z = 0; z < 2; z++){
                            float velocityAlongTangent = glm::dot(relativeVelocity, res.tangent[z]); 
                            float j = -velocityAlongTangent * res.points[w].massTangent[z];
                            float maxJ = mu * res.points[w].Pn;

                            float temp = res.points[w].Pt[z];
                            res.points[w].Pt[z] = glm::clamp(temp + j, maxJ, -maxJ); // Negate the larger one because Pn is negative
                            j = res.points[w].Pt[z] - temp;

                            glm::vec3 P = j * res.tangent[z];
                            pc1.velocity -= invMass1 * P;
                            pc1.angularVelocity -= invInertia1 * glm::cross(res.points[w].r1, P);
                            pc2.velocity += invMass2 * P;
                            pc2.angularVelocity += invInertia2 * glm::cross(res.points[w].r2, P);
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

// Computes the orthonormal basis vectors given a normal
// Found at http://box2d.org/2014/02/computing-a-basis/
void computeBasis(const glm::vec3& a, glm::vec3* b, glm::vec3* c){
    if (abs(a.x) >= 0.57735027f){
        b->x =  a.y;
        b->y = -a.x;
        b->z = 0.0f;
    }
    else{
        b->x =  0.0f;
        b->y = a.z;
        b->z = -a.y;
    }

    *b = glm::normalize(*b);
    *c = glm::cross(a, *b);
}

// Function retrieved from https://pastebin.com/UQd1W0kg
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

// Computes the edge involved in a collision given an axis and collision normal by 
// Brute forcing through the 4 possible edges to find the best support edge
void getEdge(glm::mat4 orientation, OBB obb, glm::vec3 normal, glm::vec3 outPoints[2], int axisEdge, int &e){
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
        e = extremeVert;
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
        e = extremeVert;
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
        e = extremeVert;
    }

    outPoints[0] = orientation * glm::vec4(outPoints[0], 1);
    outPoints[1] = orientation * glm::vec4(outPoints[1], 1);
    e += axisEdge * 3;
}

// Line-Plane intersection with clip edge setting
clipVertex intersection(clipVertex start, clipVertex end, clipVertex point, glm::vec3 normal, bool back){
    glm::vec3 direction = end.v - start.v;
    float d = glm::dot(point.v - start.v, normal) / glm::dot(direction, normal);
    clipVertex res;
    res.v = d * direction + start.v;
    res.f.inI = point.f.inI;
    res.f.outI = point.f.outI;
    if (back){
        res.f.outR = point.f.outI;
        res.f.outI = 0;
}
    else{
        res.f.inR = point.f.outI;
        res.f.inI = 0;
    }
    return res;
}

// Sutherland-Hodgeman Clipping
void clipVertices(clipVertex rFaceVertices[4], glm::vec3 rFaceNormal, clipVertex iFaceVertices[4], glm::vec3 iFaceNormal, contactPoint* out, int& outCount){
    std::vector<clipVertex> outList = std::vector<clipVertex>(iFaceVertices, iFaceVertices + 4);
    for (int x = 0; x < 4; x++){
        clipVertex fStart = rFaceVertices[x];
        clipVertex fEnd = rFaceVertices[(x+1) % 4];
        glm::vec3 normal = glm::cross(fEnd.v - fStart.v, rFaceNormal);
        
        std::vector<clipVertex> inList = outList;
        outList.clear();
        clipVertex S = inList.back();
        for(int y = 0; y < inList.size(); y++){
            clipVertex E = inList[y];
            if (glm::dot(E.v - fStart.v, normal) <= 0){
                if (glm::dot(S.v - fStart.v, normal) > 0){
                    outList.push_back(intersection(S, E, fStart, normal, false));
                }
                outList.push_back(E);
            }
            else if (glm::dot(S.v - fStart.v, normal) <= 0){
                outList.push_back(intersection(S, E, fStart, normal, true));
            }
            S = E;
        }
    }
    int numContact = 0;
    for (int x = 0; x < outList.size(); x++){
        glm::vec3 relativeVec = outList[x].v - rFaceVertices[0].v;
        float dist = glm::dot(relativeVec, rFaceNormal);
        if (dist < 0.00){
            out[numContact].contactID = outList[x].f;
            out[numContact].contactID.key = 1;
            out[numContact].penetrationDist = abs(dist);
            out[numContact].point = outList[x].v - dist*rFaceNormal;
            numContact++;
        }
    }
    outCount = numContact;
}

// Inspired by q3ComputeIncidentFace found at https://github.com/RandyGaul/qu3e/blob/master/src/collision/q3Collide.cpp
void getVerticesFromFaceNormal(clipVertex *outVertices, OBB obb, glm::mat4 orientation, int axis, bool opposite){
    if (axis == 0){
        if (!opposite){
            outVertices[0].v = {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[1].v = {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[2].v = {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[3].v = {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};

            outVertices[0].f.inI = 9; outVertices[0].f.outI = 1;
            outVertices[1].f.inI = 1; outVertices[1].f.outI = 8;
            outVertices[2].f.inI = 8; outVertices[2].f.outI = 7;
            outVertices[3].f.inI = 7; outVertices[3].f.outI = 9;
        }
        else{
            outVertices[0].v = {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[1].v = {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[2].v = {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[3].v = {-obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            
            outVertices[0].f.inI = 5;  outVertices[0].f.outI = 11;
            outVertices[1].f.inI = 11; outVertices[1].f.outI = 3;
            outVertices[2].f.inI = 3;  outVertices[2].f.outI = 10;
            outVertices[3].f.inI = 10; outVertices[3].f.outI = 5;
        }

    }
    else if (axis == 1){
        if (!opposite){
            outVertices[0].v = {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[1].v = {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[2].v = {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[3].v = {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};

            outVertices[0].f.inI = 3; outVertices[0].f.outI = 0;
            outVertices[1].f.inI = 0; outVertices[1].f.outI = 1;
            outVertices[2].f.inI = 1; outVertices[2].f.outI = 2;
            outVertices[3].f.inI = 2; outVertices[3].f.outI = 3;
        }
        else{
            outVertices[0].v = {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[1].v = {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[2].v = {-obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            outVertices[3].v = {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            
            outVertices[0].f.inI = 7; outVertices[0].f.outI = 4;
            outVertices[1].f.inI = 4; outVertices[1].f.outI = 5;
            outVertices[2].f.inI = 5; outVertices[2].f.outI = 6;
            outVertices[3].f.inI = 6; outVertices[3].f.outI = 7;
        }
    }
    else{
        if (!opposite){
            outVertices[0].v = {-obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            outVertices[1].v = {-obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[2].v = {obb.Half_size.x, -obb.Half_size.y, obb.Half_size.z};
            outVertices[3].v = {obb.Half_size.x, obb.Half_size.y, obb.Half_size.z};
            
            outVertices[0].f.inI = 0;  outVertices[0].f.outI = 11;
            outVertices[1].f.inI = 11; outVertices[1].f.outI = 4;
            outVertices[2].f.inI = 4;  outVertices[2].f.outI = 8;
            outVertices[3].f.inI = 8;  outVertices[3].f.outI = 0;
        }
        else{
            outVertices[0].v = {obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            outVertices[1].v = {-obb.Half_size.x, -obb.Half_size.y, -obb.Half_size.z};
            outVertices[2].v = {-obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z};
            outVertices[3].v = {obb.Half_size.x, obb.Half_size.y, -obb.Half_size.z}; 
            
            outVertices[0].f.inI = 9;  outVertices[0].f.outI = 6;
            outVertices[1].f.inI = 6;  outVertices[1].f.outI = 10;
            outVertices[2].f.inI = 10; outVertices[2].f.outI = 2;
            outVertices[3].f.inI = 2;  outVertices[3].f.outI = 9;
        }
    }
    for (int x = 0; x < 4; x++){
        outVertices[x].v = orientation * glm::vec4(outVertices[x].v, 1);
    }
}

// https://stackoverflow.com/a/52010428/10383230
float separatingDistance(const glm::vec3& RPos, const glm::vec3& Plane, const OBB& obb1, const OBB&obb2){
    return (fabs(glm::dot(RPos,Plane))) -
           (fabs(glm::dot((obb1.AxisX*obb1.Half_size.x),Plane)) +
            fabs(glm::dot((obb1.AxisY*obb1.Half_size.y),Plane)) +
            fabs(glm::dot((obb1.AxisZ*obb1.Half_size.z),Plane)) +
            fabs(glm::dot((obb2.AxisX*obb2.Half_size.x),Plane)) + 
            fabs(glm::dot((obb2.AxisY*obb2.Half_size.y),Plane)) +
            fabs(glm::dot((obb2.AxisZ*obb2.Half_size.z),Plane)));
}

// Main Box vs Box collision detection function
void BoxVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
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
            getEdge(orientation1, obb1, res.normal, edge1, axisEdge1, res.points[0].contactID.inI);
            getEdge(orientation2, obb2, -res.normal, edge2, axisEdge2, res.points[0].contactID.inR);
            res.numContacts = 1;
            res.points[0].point = computeContactPointEdges(edge1, edge2);
            res.points[0].penetrationDist = abs(minPenetrationEdge);
            res.points[0].contactID.key = 2;
            res.normal *= -1;
            computeBasis(res.normal, res.tangent, res.tangent + 1);
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
            clipVertex incidentFaceVertices[4];
            clipVertex referenceFaceVertices[4];

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
                clipVertices(referenceFaceVertices, referenceFaceNormal, incidentFaceVertices, incidentFaceNormal, res.points, res.numContacts);
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
                clipVertices(referenceFaceVertices, referenceFaceNormal, incidentFaceVertices, incidentFaceNormal, res.points, res.numContacts);
                res.normal = referenceFaceNormal * -1.0f;
            }
            res.normal *= -1.0f;
            computeBasis(res.normal, res.tangent, res.tangent + 1);
        }
    }
}

void SphereVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
    SphereColliderComponent cc1 = mWorld->getComponent<SphereColliderComponent>(c1);
    SphereColliderComponent cc2 = mWorld->getComponent<SphereColliderComponent>(c2);
    float totalR = cc1.radius + cc2.radius;
    float distance = glm::distance(tc1.worldPosition, tc2.worldPosition);
    float distBtwnObjects = distance - totalR;
    bool collided = distance < totalR;
    res.distBetweenObjects = distBtwnObjects;
    if (collided){
        res.numContacts = 1;
        res.points[0].point = (tc2.worldPosition + tc1.worldPosition) * .5f;
        res.points[0].penetrationDist = abs(distBtwnObjects);
        res.points[0].contactID.key = 0;
        res.normal = glm::normalize(tc1.worldPosition - tc2.worldPosition);
        computeBasis(res.normal, res.tangent, res.tangent + 1);
    }
}

void SphereVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
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
        res.numContacts = 1;
        res.points[0].point = boxOrientation * glm::vec4(p, 1);
        res.points[0].penetrationDist = abs(distance - trueRadius);
        res.points[0].contactID.key = 0;
        res.normal = boxOrientation * glm::vec4(glm::normalize(trueCenter - p), 0);
        computeBasis(res.normal, res.tangent, res.tangent + 1);
    }
}

void BoxVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld){
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
        res.numContacts = 1;
        res.points[0].point = boxOrientation * glm::vec4(p, 1);
        res.points[0].penetrationDist = abs(distance - trueRadius);
        res.points[0].contactID.key = 0;
        res.normal = -(boxOrientation * glm::vec4(glm::normalize(trueCenter - p), 0));
        computeBasis(res.normal, res.tangent, res.tangent + 1);
    }
}