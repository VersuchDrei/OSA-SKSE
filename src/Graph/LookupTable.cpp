#include "Graph/LookupTable.h"

#include "Graph/Node.h"
#include "SKEE.h"

namespace Graph {
    void LookupTable::AddNode(Node* node) {
        nodes.insert({node->scene_id, node});
        for (std::string anim : node->anim_ids) {
            animationNodeTable.insert({anim, node});
        }
    }

    Node* LookupTable::GetNodeById(std::string id) {
        auto iter = nodes.find(id);
        if (iter != nodes.end()) {
            return iter->second;
        }
        return nullptr;
    }

    Node* LookupTable::GetNodeByAnimation(std::string anim) {
        auto iter = animationNodeTable.find(anim);
        if (iter != animationNodeTable.end()) {
            return iter->second;
        }
        return nullptr;
    }

    bool LookupTable::SetNiTransfromInterface(SKEE::INiTransformInterface* nioInterface) {
        return nioInterface->GetVersion() ? niTransformInterface = nioInterface : false;
    }

    SKEE::INiTransformInterface* LookupTable::GetNiTransformInterface() {
        return niTransformInterface;
    }
}