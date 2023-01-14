#include "Action.h"

#include "Util/VectorUtil.h"

namespace Graph {
    bool ActionAttributes::hasTag(std::string tag) {
        return VectorUtil::contains(tags, tag);
    }

    uint32_t Action::getStrippingMask(int index) {
        return (actor == index ? attributes->actor.strippingMask : 0) |
               (target == index ? attributes->target.strippingMask : 0) |
               (performer == index ? attributes->performer.strippingMask : 0);
    }
}