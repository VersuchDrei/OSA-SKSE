#include "Graph/LookupTable.h"

#include "Graph/Node.h"
#include "Trait/Condition.h"
#include "Util/StringUtil.h"
#include "Util/JsonFileLoader.h"
#include "SKEE.h"


namespace Graph {
    const char* ACTION_FILE_PATH{ "Data/SKSE/Plugins/OStim/actions" };

    void LookupTable::setupForms() {
        auto handler = RE::TESDataHandler::GetSingleton();
        OSexIntegrationMainQuest = handler->LookupForm<RE::TESQuest>(0x801, "OStim.esp");
    }

    void LookupTable::addNode(Node* node) {
        nodes.insert({node->lowercase_id, node});
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

    bool LookupTable::hasNodes(Furniture::FurnitureType furnitureType, int actorCount) {
        if (furnitureType == Furniture::FurnitureType::BED) {
            furnitureType = Furniture::FurnitureType::NONE;
        }

        auto iter = nodeList.find(furnitureType);
        if (iter == nodeList.end()) {
            return false;
        }

        auto iter2 = iter->second->find(actorCount);
        return iter2 != iter->second->end();
    }

    Node* LookupTable::getRandomNode(Furniture::FurnitureType furnitureType, std::vector<Trait::ActorConditions> actorConditions, std::function<bool(Node*)> nodeCondition) {
        if (furnitureType == Furniture::FurnitureType::BED) {
            furnitureType = Furniture::FurnitureType::NONE;
        }

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
            if (!node->isTransition && !node->noRandomSelection && node->fulfilledBy(actorConditions) && nodeCondition(node)) {
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

    void parseActor(json& json, ActionActor& actor) {
        if (json.contains("stimulation")) {
            actor.stimulation = json["stimulation"];
        }
        else {
            actor.stimulation = 0.0;
        }

        if (json.contains("maxStimulation")) {
            actor.maxStimulation = json["maxStimulation"];
        }
        else {
            actor.maxStimulation = 100.0;
        }

        if (json.contains("requirements")) {
            for (auto& req : json["requirements"]) {
                actor.requirements.push_back(req.get<std::string>());
            }
        }

        if (json.contains("strippingSlots")) {
            for (auto& slot : json["strippingSlots"]) {
                actor.strippingSlots.push_back(slot.get<int>());
            }
        }

        if (json.contains("floats")) {
            auto& floats = json["floats"];
            for (json::iterator it = floats.begin(); it != floats.end(); it++) {
                actor.floats.insert(std::make_pair(it.key(), it.value().get<float>()));
            }
        }

        if (json.contains("ints")) {
            auto& floats = json["ints"];
            for (json::iterator it = floats.begin(); it != floats.end(); it++) {
                actor.ints.insert(std::make_pair(it.key(), it.value().get<int>()));
            }
        }

        if (json.contains("strings")) {
            auto& floats = json["strings"];
            for (json::iterator it = floats.begin(); it != floats.end(); it++) {
                actor.strings.insert(std::make_pair(it.key(), it.value().get<std::string>()));
            }
        }
    };

    void LookupTable::SetupActions(){        
    
        Util::JsonFileLoader::LoadFilesInFolder(ACTION_FILE_PATH, [&](std::string filename, json json) {
            Graph::ActionAttributes attr;
            if(json.contains("actor")){
                Graph::ActionActor actor;
                parseActor(json["actor"], actor);
                attr.actor = actor;
            }
            if (json.contains("target")) {
                Graph::ActionActor target;
                parseActor(json["target"], target);
                attr.target = target;
            }
            if(json.contains("performer")){
                Graph::ActionActor performer;
                parseActor(json["performer"], performer);
                attr.performer = performer;
            }
            actions.insert(std::make_pair(filename, attr));
        });
    }

    ActionAttributes* LookupTable::GetActionAttributesByType(std::string type)
    {
        if (auto it = actions.find(type); it != actions.end()) {
            return &actions.at(type);
        }
        else {
            logger::warn("No action found for {} using default", type);
            return &actions.at("default");
        }
    }
}