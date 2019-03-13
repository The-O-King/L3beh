#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"
#include "core/components.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include <vector>
#include <map>

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

    void update(collisionInfo &other){
        this->distBetweenObjects = other.distBetweenObjects;
        this->normal = other.normal;

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
                c->Pn = cOld->Pn;
                c->Pt[0] = cOld->Pt[0];
                c->Pt[1] = cOld->Pt[1];
            }
            else{
                merged[x] = other.points[x];
            }
        }
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

typedef void (*collisionFunction) (int, TransformComponent&, int, TransformComponent&, collisionInfo&, World*);
void BoxVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);
void BoxVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);
void SphereVsBox(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);
void SphereVsSphere(int c1, TransformComponent &tc1, int c2, TransformComponent &tc2, collisionInfo &res, World* mWorld);

class CollisionSystem : public System{
    private:
        std::map<collisionInfoKey, collisionInfo> persistentCollisionData;
    public:
        CollisionSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif