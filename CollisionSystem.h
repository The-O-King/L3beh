#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include <vector>

struct contactPoint{
    float penetrationDist;
    glm::vec3 point;
};

struct collisionInfo{
    glm::vec3 normal;
    std::vector<contactPoint> points;
};

struct OBB{
    glm::vec3 AxisX, AxisY, AxisZ, Half_size;
};


class CollisionSystem : public System{
    private:
        float separatingDistance(const glm::vec3& RPos, const glm::vec3& Plane, const OBB& box1, const OBB&box2);
        void computeIncidentFace(glm::vec3 *facePlanes, glm::vec3 normal, glm::vec3 &incidentFaceNormal, glm::vec3 *incidentFaceVertices);
        void getVerticesFromFaceNormal(glm::vec3 *outVertices, OBB obb, glm::mat4 orientation, int axis, bool opposite);
        std::vector<contactPoint> clipVertices(glm::vec3 rFaceVertices[4], glm::vec3 rFaceNormal, glm::vec3 iFaceVertices[4], glm::vec3 iFaceNormal);
        void getEdge(glm::mat4 orientation, OBB obb, glm::vec3 normal, glm::vec3 outPoints[2], int axisEdge);
        glm::vec3 computeContactPointEdges(glm::vec3 e1[2], glm::vec3 e2[2]);

    public:
        CollisionSystem(World* w);
        void init() override;
        void update(float deltaTime) override;
};

#endif