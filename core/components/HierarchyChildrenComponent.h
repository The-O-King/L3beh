#ifndef HIERARCHYCHILDRENCOMPONENT_H
#define HIERARCHYCHILDRENCOMPONENT_H

#include "Component.h"
#include <set>

struct HierarchyChildrenComponent : Serialization<HierarchyChildrenComponent> {
    std::set<int> children;

    static std::string getNameImpl() { return "HierarchyChildrenComponent"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HierarchyChildrenComponent, children)
};

#endif