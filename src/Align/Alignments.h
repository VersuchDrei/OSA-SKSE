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

    enum Source { None, Global, Pack, Individual };

    inline static std::string GetString(Source src) {
        switch (src) {
            case None:
                return "None";
            case Global:
                return "Global";
            case Pack:
                return "Pack";
            case Individual:
                return "Scene";
        }
        return "None";
    }

    // TODO: Think about default values (I.E when we don't want to change from whats defined in scene, important for
    // scales and angles)
    // TODO: Add back in scaling
    struct AlignmentActorInfo {
        double offsetX = 0;
        double offsetY = 0;
        double offsetZ = 0;
    };
    
    struct NodeAlignmentInfo {
        std::unordered_map<GenderMap, std::vector<AlignmentActorInfo>> actorInfos;
    };

    class Alignments : public OStim::ISingleton<Alignments> {
    public:
        void LoadAlignments();        

        std::tuple<Source, std::vector<AlignmentActorInfo>*> GetAlignmentInfoForScene(Graph::Node* node,
                                                                                      GenderMap genderMap);

        void UpdateAndApplyAlignments(OStim::Thread* thread, OAlign::AlignmentActorInfo currentAlignment, Source source, Graph::Node* node, int selectedActor);
        void ApplyAlignments(OStim::Thread* thread, std::vector<AlignmentActorInfo>* info);
        
        void SerializeAlignments();

    private:
        NodeAlignmentInfo globalAlignments;
        std::unordered_map<std::string, NodeAlignmentInfo> packAlignments;
        std::unordered_map<std::string, NodeAlignmentInfo> sceneAlignments;
    };
}  // namespace OAlign