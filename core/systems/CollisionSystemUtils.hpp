#ifndef COLLISIONSYSTEMUTILS_H
#define COLLISIONSYSTEMUTILS_H

#include <core/components/PhysicsComponent.h>
#include <core/components/TransformComponent.h>
#include <core/world.h>
#include <cfloat>

#define PERCENT .2f
#define THRESH .01f

struct featurePair{
    int key = -1;  // Represents BoxEdge, BoxFace, SphereAny
    int inR = -1, outR = -1;
    int inI = -1, outI = -1;

    bool operator== (featurePair &rhs){
        return key == rhs.key && 
               inR == rhs.inR && outR == rhs.outR &&
               inI == rhs.inI && outI == rhs.outI;
    }
};

struct clipVertex{
    glm::vec3 v;
    featurePair f;
};

struct contactPoint{
    float penetrationDist = 0;
    glm::vec3 point;
    featurePair contactID;
    float Pn = 0;
    float Pt[2] = {0, 0};
    glm::vec3 r1 = glm::vec3(0);
    glm::vec3 r2 = glm::vec3(0);
    float massNormal = 0;
    float massTangent[2] = {0, 0};
    float bias = 0;
};

struct collisionInfo{
    int entity1, entity2;
    float distBetweenObjects;
    glm::vec3 normal;
    glm::vec3 tangent[2];
    contactPoint points[8];
    int numContacts = 0;

    collisionInfo(){
        entity1 = -2;
        entity2 = -1;
    }

    collisionInfo(int entity1, int entity2){
        if (entity1 < entity2){
            this->entity1 = entity1;
            this->entity2 = entity2;
        }
        else{
            this->entity1 = entity1;
            this->entity2 = entity2;
        }
    }

    collisionInfo* update(collisionInfo &other){
        distBetweenObjects = other.distBetweenObjects;
        normal = other.normal;
        tangent[0] = other.tangent[0];
        tangent[1] = other.tangent[1];

        contactPoint merged[8];
        for (int x = 0; x < other.numContacts; x++){
            contactPoint* newContact = other.points + x;
            int equal = -1;
            for (int y = 0; y < numContacts; y++){
                contactPoint* oldContact = points + y;
                if (newContact->contactID == oldContact->contactID){
                    equal = y;
                    break;
                }
            }

            if (equal != -1){
                contactPoint* c = merged + x;
                contactPoint* cOld = points + equal;
                *c = *newContact;
                // Warm Starting
                c->Pn = cOld->Pn;
                c->Pt[0] = cOld->Pt[0];
                c->Pt[1] = cOld->Pt[1];
            }
            else{
                merged[x] = other.points[x];
            }
        }

        numContacts = other.numContacts;
        for (int x = 0; x < numContacts; x++){
            points[x] = merged[x];
        }
        return this;
    }
};

struct collisionInfoKey{
    int entity1, entity2;
    collisionInfoKey(collisionInfo& other){
        if (other.entity1 < other.entity2){
            entity1 = other.entity1;
            entity2 = other.entity2;
        }
        else{
            entity1 = other.entity2;
            entity2 = other.entity1;
        }
    }

    bool operator<( const collisionInfoKey& other ) const{
        if (entity1 < other.entity1)
            return true;
    
        if (entity1 == other.entity1 && entity2 < other.entity2)
            return true;
    
        return false;
    }
};

struct OBB{
    glm::vec3 AxisX, AxisY, AxisZ, Half_size;
};

typedef void (*collisionFunction) (int, TransformComponent&, ColliderComponent &, int, TransformComponent&, ColliderComponent&, collisionInfo&, World*);

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
void BoxVsBox(int c1, TransformComponent &tc1, ColliderComponent &cc1, int c2, TransformComponent &tc2, ColliderComponent &cc2, collisionInfo &res, World* mWorld){
    BoxColliderComponent bc1 = mWorld->getComponent<BoxColliderComponent>(c1);
    BoxColliderComponent bc2 = mWorld->getComponent<BoxColliderComponent>(c2);
    glm::mat4 orientation1 = tc1.getWorldOrientation(mWorld);
    glm::mat4 orientation2 = tc2.getWorldOrientation(mWorld);
    OBB obb1, obb2;
    obb1.AxisX = glm::vec3(orientation1[0]);
    obb1.AxisY = glm::vec3(orientation1[1]);
    obb1.AxisZ = glm::vec3(orientation1[2]);
    obb1.Half_size = bc1.halfSize;
    obb2.AxisX = glm::vec3(orientation2[0]);
    obb2.AxisY = glm::vec3(orientation2[1]);
    obb2.AxisZ = glm::vec3(orientation2[2]);
    obb2.Half_size = bc2.halfSize;

    glm::vec3 RPos = (tc2.getWorldPosition(mWorld) + cc2.offset) - (tc1.getWorldPosition(mWorld) + cc1.offset);
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
            if (glm::dot(RPos, res.normal) < 0)
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
            if (glm::dot(RPos, referenceFaceNormal) < 0){
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

void SphereVsSphere(int c1, TransformComponent &tc1, ColliderComponent &cc1, int c2, TransformComponent &tc2, ColliderComponent &cc2, collisionInfo &res, World* mWorld){
    SphereColliderComponent sc1 = mWorld->getComponent<SphereColliderComponent>(c1);
    SphereColliderComponent sc2 = mWorld->getComponent<SphereColliderComponent>(c2);
    glm::vec3 trueCenter1 = tc1.getWorldPosition(mWorld) + cc1.offset;
    glm::vec3 trueCenter2 = tc2.getWorldPosition(mWorld) + cc2.offset;
    float totalR = sc1.radius + sc2.radius;
    float distance = glm::distance(trueCenter1, trueCenter2);
    float distBtwnObjects = distance - totalR;
    bool collided = distance < totalR;
    res.distBetweenObjects = distBtwnObjects;
    if (collided){
        res.numContacts = 1;
        res.points[0].point = (trueCenter1 + trueCenter2) * .5f;
        res.points[0].penetrationDist = abs(distBtwnObjects);
        res.points[0].contactID.key = 0;
        res.normal = glm::normalize(trueCenter1 - trueCenter2);
        computeBasis(res.normal, res.tangent, res.tangent + 1);
    }
}

void SphereVsBox(int c1, TransformComponent &tc1, ColliderComponent &cc1, int c2, TransformComponent &tc2, ColliderComponent &cc2, collisionInfo &res, World* mWorld){
    SphereColliderComponent sc1 = mWorld->getComponent<SphereColliderComponent>(c1);
    BoxColliderComponent bc2 = mWorld->getComponent<BoxColliderComponent>(c2);
    glm::vec3 trueCenter1 = tc1.getWorldPosition(mWorld) + cc1.offset;
    float trueRadius = sc1.radius;

    glm::mat4 boxOrientation = tc2.getWorldOrientation(mWorld);
    trueCenter1 = glm::inverse(boxOrientation) * glm::vec4(trueCenter1, 1);

    glm::vec3 p = glm::clamp(trueCenter1, -bc2.halfSize, bc2.halfSize);
    float distance = glm::distance(p, trueCenter1);
    res.distBetweenObjects = distance - trueRadius;
    if (distance < trueRadius){
        res.numContacts = 1;
        res.points[0].point = boxOrientation * glm::vec4(p, 1);
        res.points[0].penetrationDist = abs(distance - trueRadius);
        res.points[0].contactID.key = 0;
        res.normal = boxOrientation * glm::vec4(glm::normalize(trueCenter1 - p), 0);
        computeBasis(res.normal, res.tangent, res.tangent + 1);
    }
}

void BoxVsSphere(int c1, TransformComponent &tc1, ColliderComponent &cc1, int c2, TransformComponent &tc2, ColliderComponent &cc2, collisionInfo &res, World* mWorld){
    BoxColliderComponent bc1 = mWorld->getComponent<BoxColliderComponent>(c1);
    SphereColliderComponent sc2 = mWorld->getComponent<SphereColliderComponent>(c2);
    glm::vec3 trueCenter2 = tc2.getWorldPosition(mWorld) + cc2.offset;        // Sphere true position
    float trueRadius = sc2.radius;

    glm::mat4 boxOrientation = tc1.getWorldOrientation(mWorld);
    trueCenter2 = glm::inverse(boxOrientation) * glm::vec4(trueCenter2, 1);

    glm::vec3 p = glm::clamp(trueCenter2, -bc1.halfSize, bc1.halfSize);
    float distance = glm::distance(p, trueCenter2);
    res.distBetweenObjects = distance - trueRadius;
    if (distance < trueRadius){
        res.numContacts = 1;
        res.points[0].point = boxOrientation * glm::vec4(p, 1);
        res.points[0].penetrationDist = abs(distance - trueRadius);
        res.points[0].contactID.key = 0;
        res.normal = -(boxOrientation * glm::vec4(glm::normalize(trueCenter2 - p), 0));
        computeBasis(res.normal, res.tangent, res.tangent + 1);
    }
}

#endif