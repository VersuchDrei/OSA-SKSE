#include "Alignments.h"

namespace OAlign {
    const char* ALIGNMENT_DATA_FILE_PATH{"Data/SKSE/Plugins/OStim/alignment.json"};

    void LoadAlignmentInfo(AlignmentActorInfo& actorInfo, json jData) {
        if (jData.contains("x")) {
            actorInfo.offsetX = jData["x"].get<double>();
        }
        if (jData.contains("y")) {
            actorInfo.offsetY = jData["y"].get<double>();
        }
        if (jData.contains("z")) {
            actorInfo.offsetZ = jData["z"].get<double>();
        }
        /* if (jData.contains("scale")) {
             actorInfo.scale = jData["scale"].get<float>();
         }
         if (jData.contains("penisScale")) {
             actorInfo.penisScale = jData["penisScale"].get<float>();
         }
         if (jData.contains("penisAngle")) {
             actorInfo.penisAngle = jData["penisAngle"].get<int>();
         }*/
    }

    void LoadAlignment(NodeAlignmentInfo& info, json jData) {
        if (jData.is_object()) {
            for (auto& genderMap : jData.items()) {
                auto& jActor = genderMap.value();
                if (jActor.is_array() || jActor.size() != genderMap.key().size()) {
                    std::vector<AlignmentActorInfo> actorInfos;
                    for (auto& actorJData : jActor) {
                        AlignmentActorInfo actorInfo;
                        LoadAlignmentInfo(actorInfo, actorJData);
                        actorInfos.push_back(actorInfo);
                    }
                    info.actorInfos.emplace(genderMap.key(), actorInfos);
                } else {
                    logger::warn("Invalid Alignment info - {}", genderMap.key());
                }
            }
        }
    }

    void Alignments::LoadAlignments() {
        fs::path rootPath{ALIGNMENT_DATA_FILE_PATH};
        if (!fs::exists(rootPath)) {
            logger::warn("file path ({}) does not exist", ALIGNMENT_DATA_FILE_PATH);
            return;
        }
        std::ifstream ifs(ALIGNMENT_DATA_FILE_PATH);
        json json = json::parse(ifs, nullptr, false);
        if (json.contains("global")) {
            //LoadAlignment(globalAlignments, json["global"]);
        }
    }

    std::vector<AlignmentActorInfo>* Alignments::GetAlignmentInfoForScene(Graph::Node* node, GenderMap genderMap) {
        auto sceneIter = sceneAlignments.find(node->scene_id);
        if (sceneIter != sceneAlignments.end()) {
            auto& foundSceneAlignmentValues = sceneAlignments[node->scene_id];
            auto mapIter = foundSceneAlignmentValues.actorInfos.find(genderMap);
            if (mapIter != foundSceneAlignmentValues.actorInfos.end()) {
                return &foundSceneAlignmentValues.actorInfos[genderMap];
            }
        }
        return nullptr;
    }

    void Alignments::UpdateAndApplyAlignments(OStim::Thread* thread, OAlign::AlignmentActorInfo currentAlignment, Graph::Node* node, int selectedActor) {
        auto actors = thread->GetTESActors();
        auto map = GetGenderMap(&actors);

        std::vector<AlignmentActorInfo> infos;
        for (int i = 0; i < actors.size(); i++) {
            if (i != selectedActor)
                infos.push_back(OAlign::AlignmentActorInfo{});
            else
                infos.push_back(currentAlignment);
        }

        ApplyAlignments(thread, &infos);
    }

    void Alignments::ApplyAlignments(OStim::Thread* thread, std::vector<AlignmentActorInfo>* info) {
        auto actors = thread->GetTESActors();
        
        auto stage = thread->GetStageObject();
        
        for (int i = 0; i < actors.size(); i++) {
            auto& actorInfo = info->at(i);
            thread->alignActor(actors[i], info->at(i).offsetX, info->at(i).offsetY, info->at(i).offsetZ);
        }
    }

    void Alignments::SerializeAlignments() {}
}  // namespace OAlign
