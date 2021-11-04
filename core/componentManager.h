#ifndef COMPONENT_H
#define COMPONENT_H
#include <unordered_map>
#include <vector>
#include <bitset>
#include <any>

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
        virtual bool addComponent(int entity) = 0;
        virtual bool addComponent(int entity, std::any initialComp) = 0;
        virtual bool setComponent(int entity, std::any initialComp) = 0;
        virtual bool removeComponent(int entity) = 0;
        virtual std::any getComponent(int entity) = 0;
};

template<class T> 
class ComponentList : public ComponentListInterface {
    private:
        std::vector<T> data;
        std::unordered_map<int, int> entityToIndex;

    public:
        ComponentList() { data.reserve(200); }

        bool addComponent(int entity, std::any initialComp) {
            if (entityToIndex.find(entity) == entityToIndex.end()){
                data.push_back(std::any_cast<T>(initialComp));
                entityToIndex[entity] = data.size() - 1;
                return true;
            }
            return false;
        }

        bool addComponent(int entity) {
            return addComponent(entity, T());
        }

        bool setComponent(int entity, std::any initialComp) {
            if (entityToIndex.find(entity) != entityToIndex.end()){
                data[entityToIndex[entity]] = std::any_cast<T>(initialComp);
                return true;
            }
            return false;
        }

        bool removeComponent(int entity) {
            if (entityToIndex.find(entity) != entityToIndex.end()){
                int entityIndex = entityToIndex[entity];
                for(auto& p : entityToIndex) {
                    if (p.second > entityIndex)
                        p.second--;
                }
                entityToIndex.erase(entity);
                data.erase(data.begin() + entityIndex);
                return true;
            }
            return false;
        }

        std::any getComponent(int entity) {
            return &data[entityToIndex[entity]];
        }
};

class ComponentManager{
    private:
        std::vector<ComponentListInterface*> componentHolder;

    public:
        ComponentManager();
        bool destroyEntity(int entityID, componentSignature toDestroy);

        template <class T>
        int registerComponent();
        template <class T>
        bool addComponent(int entity);
        template <class T>
        bool addComponent(int entity, T initialComp);
        template <class T>
        bool setComponent(int entityID, T initialComp);
        template <class T>
        bool removeComponent(int entity);
        template <class T>
        T* getComponent(int entity);
};

template <class T>
bool ComponentManager::addComponent(int entityID){
    int compID = type_id<T>();
    return componentHolder[compID]->addComponent(entityID);
}

template <class T>
bool ComponentManager::addComponent(int entityID, T initialComp){
    int compID = type_id<T>();
    return componentHolder[compID]->addComponent(entityID, initialComp);
}

template <class T>
bool ComponentManager::setComponent(int entityID, T initialComp){
    int compID = type_id<T>();
    return componentHolder[compID]->addComponent(entityID, initialComp);
}

template <class T>
bool ComponentManager::removeComponent(int entityID){
    int compID = type_id<T>();
    return componentHolder[compID]->removeComponent(entityID);
}

template <class T>
T* ComponentManager::getComponent(int entityID){
    int compID = type_id<T>();
    return std::any_cast<T*>(componentHolder[compID]->getComponent(entityID));
}

template <class T>
int ComponentManager::registerComponent(){
    int compID = type_id<T>();
    componentHolder[compID] = new ComponentList<T>;
    return compID;
}

#endif