#ifndef HIERARCHYPARENTCOMPONENT_H
#define HIERARCHYPARENTCOMPONENT_H

#include "Component.h"

struct HierarchyParentComponent : Serialization<HierarchyParentComponent> {
    int parent;

    static std::string getNameImpl() { return "HierarchyParentComponent"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(HierarchyParentComponent, parent)
};

#endif