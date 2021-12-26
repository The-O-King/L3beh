#ifndef HIERARCHYPARENTCOMPONENT_H
#define HIERARCHYPARENTCOMPONENT_H

#include "Component.h"

struct HierarchyParentComponent : public Component {
    int parent;
};

#endif