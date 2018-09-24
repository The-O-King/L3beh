#ifndef COMPONENT_H
#define COMPONENT_H
#include <unordered_map>
#include <vector>
#include <bitset>
#define MAX_COMPONENT 100

typedef std::bitset<MAX_COMPONENT> componentSignature;

enum componentID{
    TRANSFORM,
    RENDER,
    MOVEMENT,
    NUM_COMPONENT
};


struct TransformComponent{
    double x, y, z;
};

struct RenderComponent{
    double t, u, v;
};

struct MovementComponent{
    double velocity;
    double acceleration;
};


template <class T>
class ComponentManager{
    private:
        std::unordered_map<int, int> entityDict;
        std::vector<T> componentHolder;

    public:
        bool addComponent(int entity);
        bool addComponent(int entity, T initialComp);
        bool setComponent(int entityID, T initialComp);
        bool removeComponent(int entity);
        T getComponent(int entity);
};

template class ComponentManager<TransformComponent>;
template class ComponentManager<RenderComponent>;
template class ComponentManager<MovementComponent>;

#endif