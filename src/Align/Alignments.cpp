#pragma once
#include "Align/Alignments.h"

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
            LoadAlignment(globalAlignments, json["global"]);
        }
    }

    std::tuple<Source, std::vector<AlignmentActorInfo>*> Alignments::GetAlignmentInfoForScene(Graph::Node* node,
                                                                                              GenderMap genderMap) {
        auto sceneIter = sceneAlignments.find(node->scene_id);
        if (sceneIter != sceneAlignments.end()) {
            auto& foundSceneAlignmentValues = sceneAlignments[node->scene_id];
            auto mapIter = foundSceneAlignmentValues.actorInfos.find(genderMap);
            if (mapIter != foundSceneAlignmentValues.actorInfos.end()) {
                return std::make_tuple(Individual, &foundSceneAlignmentValues.actorInfos[genderMap]);
            }
        }
        if (node->sourceModule != "") {
            auto packIter = packAlignments.find(node->sourceModule);
            if (packIter != packAlignments.end()) {
                auto& foundpackAlignmentValues = packAlignments[node->sourceModule];
                auto mapIter = foundpackAlignmentValues.actorInfos.find(genderMap);
                if (mapIter != foundpackAlignmentValues.actorInfos.end()) {
                    return std::make_tuple(Pack, &foundpackAlignmentValues.actorInfos[genderMap]);
                }
            }
        }
        auto mapIter = globalAlignments.actorInfos.find(genderMap);
        if (mapIter != globalAlignments.actorInfos.end()) {
            return std::make_tuple(Global, &globalAlignments.actorInfos[genderMap]);
        }
        return std::make_tuple(None, nullptr);
    }

    void Alignments::UpdateAndApplyAlignments(OStim::Thread* thread, OAlign::AlignmentActorInfo currentAlignment,
                                              Source source, Graph::Node* node, int selectedActor) {
        auto actors = thread->GetTESActors();
        auto map = GetGenderMap(&actors);
        NodeAlignmentInfo* nodeInfo;
        switch (source) {
            case Global:
                nodeInfo = &globalAlignments;
                break;
            case Pack:
                nodeInfo = &packAlignments[node->sourceModule];
                break;
            case Individual:
                nodeInfo = &sceneAlignments[node->scene_id];
                break;
            case None:
                return;
        }

        auto iter = nodeInfo->actorInfos.find(map);
        
        if (iter == nodeInfo->actorInfos.end()) {

            std::vector<AlignmentActorInfo> infos;
            for (int i = 0; i < actors.size(); i++) {
                if (i != selectedActor)
                    infos.push_back(OAlign::AlignmentActorInfo{});
                else
                    infos.push_back(currentAlignment);
            }

            nodeInfo->actorInfos.insert(std::make_pair(map, infos));

        } else {
            nodeInfo->actorInfos[map].at(selectedActor) = currentAlignment;
        }
        ApplyAlignments(thread, &nodeInfo->actorInfos[map]);
    }

    void Alignments::ApplyAlignments(OStim::Thread* thread, std::vector<AlignmentActorInfo>* info) {
        auto actors = thread->GetTESActors();

        for (int i = 0; i < actors.size(); i++) {
            RE::NiPoint3 pos{(float)info->at(i).offsetX, (float)info->at(i).offsetY, (float)info->at(i).offsetZ};
            actors[i]->SetPosition(pos, false);
        }
    }

    void Alignments::SerializeAlignments() {}
}  // namespace OAlign
