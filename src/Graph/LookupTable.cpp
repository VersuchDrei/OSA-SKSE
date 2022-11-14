#include "Graph/LookupTable.h"

#include "Graph/Node.h"
#include "Trait/Condition.h"
#include "Util/StringUtil.h"
#include "SKEE.h"

namespace Graph {
    FurnitureType LookupTable::getFurnitureType(std::string type) {
        auto iter = furnitures.find(type);
        if (iter != furnitures.end()) {
            return iter->second;
        }

        return FurnitureType::NONE;
    }

    void LookupTable::addNode(Node* node) {
        nodes.insert({node->scene_id, node});
        for (std::string anim : node->anim_ids) {
            animationNodeTable.insert({anim, node});
        }

        std::unordered_map<int, std::vector<Node*>*>* innerMap;
        std::vector<Node*>* innerList;
        int count = node->actors.size();
        auto iter = nodeList.find(node->furnitureType);
        if (iter != nodeList.end()) {
            innerMap = iter->second;
        } else {
            innerMap = new std::unordered_map<int, std::vector<Node*>*>();
            nodeList.insert({node->furnitureType, innerMap});
        }

        auto iter2 = innerMap->find(count);
        if (iter2 != innerMap->end()) {
            innerList = iter2->second;
        } else {
            innerList = new std::vector<Node*>();
            innerMap->insert({count, innerList});
        }
        
        innerList->push_back(node);
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

    Node* LookupTable::getRandomNode(FurnitureType furnitureType, std::vector<Trait::ActorConditions> actorConditions, std::function<bool(Node*)> nodeCondition) {
        auto iter = nodeList.find(furnitureType);
        if (iter == nodeList.end()) {
            return nullptr;
        }
        auto innerMap = iter->second;

        auto iter2 = innerMap->find(actorConditions.size());
        if (iter2 == innerMap->end()) {
            return nullptr;
        }
        // the copy is to prevent race conditions if several scripts try to call this at once
        std::vector<Node*> copy = *iter2->second;

        std::shuffle(std::begin(copy), std::end(copy), rng);

        for (auto& node : copy) {
            if (!node->isTransition && node->fulfilledBy(actorConditions) && nodeCondition(node)) {
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