#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H
#include <core/components/Component.h>
#include <unordered_map>
#include <vector>
#include <stack>
#include <bitset>
#include <any>
#include <stdexcept>

#define MAX_COMPONENT 100
typedef std::bitset<MAX_COMPONENT> componentSignature;

inline int generate_type_id() {
    static int value = 0;
    return value++;
}

template<class T>
int type_id() {
    static int value = generate_type_id();
    return value;
}

class ComponentListInterface {
    public:
        virtual std::any addComponent(int entity) = 0;
        virtual void removeComponent(int entity) = 0;
        virtual std::any getComponent(int entity) = 0;
        virtual Component* getComponentAsComponent(int entity) = 0;
        virtual Component* addComponentAsComponent(int entity) = 0;
        virtual std::string getName() = 0;
};

template<class T> 
class ComponentList : public ComponentListInterface {
static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    private:
        std::vector<T> data;
        std::unordered_map<int, int> entityToIndex;
        std::stack<int> freeIndices;

    public:
        ComponentList() { data.reserve(200); }

        std::any addComponent(int entity) {
            if (entityToIndex.find(entity) != entityToIndex.end()){
                throw std::runtime_error("Entity " + std::to_string(entity) + " already has Component " + T::getName());
            }

            int index = -1;
            if (!freeIndices.empty()) {
                index = freeIndices.top(); freeIndices.pop();
                data[index] = T();
            }
            else {
                data.emplace_back();
                index = data.size() - 1;
            }
            data[index].owner = entity;
            entityToIndex[entity] = index;
            return &data[index];
        }

        void removeComponent(int entity) {
            if (entityToIndex.find(entity) == entityToIndex.end()){
                throw std::runtime_error("Entity " + std::to_string(entity) + " does not have Component " + T::getName());
            }
            
            freeIndices.push(entityToIndex[entity]);
            entityToIndex.erase(entity);
        }

        std::any getComponent(int entity) {
            return &data[entityToIndex[entity]];
        }

        Component* getComponentAsComponent(int entity) {
            return &data[entityToIndex[entity]];
        }

        Component* addComponentAsComponent(int entity) {
            if (entityToIndex.find(entity) != entityToIndex.end()){
                throw std::runtime_error("Entity " + std::to_string(entity) + " already has Component " + T::getName());
            }

            int index = -1;
            if (!freeIndices.empty()) {
                index = freeIndices.top(); freeIndices.pop();
                data[index] = T();
            }
            else {
                data.emplace_back();
                index = data.size() - 1;
            }
            data[index].owner = entity;
            entityToIndex[entity] = index;
            return &data[index];
        }

        std::string getName() {
            return T::getName();
        }
};

#endif