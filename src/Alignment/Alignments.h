#pragma once

#include <Graph/Node.h>

#include "Core/Singleton.h"
#include <Core/Thread.h>

namespace OAlign {

    using GenderMap = std::string;
    using GenderVal = std::string;

    inline static GenderVal GetGenderVal(RE::Actor* actor) {
        switch (actor->GetActorBase()->GetSex()) {
            case RE::SEX::kMale:
                return "M";
            case RE::SEX::kFemale:
                return "F";
        }
        return "";
    }
    inline static GenderMap GetGenderMap(std::vector<RE::Actor*>* actors) {
        if (actors == nullptr) {
            return "";
        }
        std::string ret;
        for (auto g : *actors) {
            ret += GetGenderVal(g);
        }
        return ret;
    }

    // TODO: Think about default values (I.E when we don't want to change from whats defined in scene, important for
    // scales and angles)
    // TODO: Add back in scaling
    struct AlignmentActorInfo {
        float offsetX = 0;
        float offsetY = 0;
        float offsetZ = 0;
        float scale = 1;
        float rotation = 0;
        float sosBend = 0;
    };
    
    struct NodeAlignmentInfo {
        std::unordered_map<GenderMap, std::vector<AlignmentActorInfo>> actorInfos;
    };

    class Alignments : public OStim::ISingleton<Alignments> {
    public:
        void LoadAlignments();        

        std::vector<AlignmentActorInfo>* GetAlignmentInfoForScene(Graph::Node* node, GenderMap genderMap);

        void UpdateAndApplyAlignments(OStim::Thread* thread, OAlign::AlignmentActorInfo currentAlignment, Graph::Node* node, int selectedActor);
        void ApplyAlignments(OStim::Thread* thread, std::vector<AlignmentActorInfo>* info);
        
        void SerializeAlignments();

    private:
        std::unordered_map<std::string, NodeAlignmentInfo> sceneAlignments;
    };
}  // namespace OAlign