#include "Graph/LookupTable.h"

#include "Graph/Node.h"
#include "Util/JsonFileLoader.h"
#include "SKEE.h"


namespace Graph {

    const char* ACTION_FILE_PATH{ "Data/SKSE/Plugins/OStim/actions" };

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

    void parseActor(json& json, ActionActor& actor) {
        if (json.contains("stimulation")) {
            actor.stimulation = json["stimulation"];
        }
        else {
            actor.stimulation = 0.0;
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