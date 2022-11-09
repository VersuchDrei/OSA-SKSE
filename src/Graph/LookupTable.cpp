#include "Graph/LookupTable.h"

#include "Graph/Node.h"
#include "Util/StringUtil.h"
#include "SKEE.h"

namespace Graph {
    void LookupTable::addNode(Node* node) {
        nodes.insert({node->scene_id, node});
        for (std::string anim : node->anim_ids) {
            animationNodeTable.insert({anim, node});
        }
        nodeList.push_back(node);
    }

    Node* LookupTable::getNodeById(std::string id) {
        StringUtil::toLower(&id);
        auto iter = nodes.find(id);
        if (iter != nodes.end()) {
            return iter->second;
        }
        return nullptr;
    }

    Node* LookupTable::getNodeByAnimation(std::string anim) {
        auto iter = animationNodeTable.find(anim);
        if (iter != animationNodeTable.end()) {
            return iter->second;
        }
        return nullptr;
    }

    Node* LookupTable::getRandomNode(std::function<bool(Node)> condition) {
        auto copy = nodeList;
        std::shuffle(std::begin(copy), std::end(copy), rng);

        for (auto& node : copy) {
            if (condition(*node)) {
                return node;
            }
        }

        return nullptr;
    }

    bool LookupTable::setNiTransfromInterface(SKEE::INiTransformInterface* nioInterface) {
        return nioInterface->GetVersion() ? niTransformInterface = nioInterface : false;
    }

    SKEE::INiTransformInterface* LookupTable::getNiTransformInterface() {
        return niTransformInterface;
    }
}