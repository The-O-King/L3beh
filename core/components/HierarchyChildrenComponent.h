#ifndef HIERARCHYCHILDRENCOMPONENT_H
#define HIERARCHYCHILDRENCOMPONENT_H

#include "Component.h"
#include <set>

struct HierarchyChildrenComponent : public Component {
    std::set<int> children;
};

#endif