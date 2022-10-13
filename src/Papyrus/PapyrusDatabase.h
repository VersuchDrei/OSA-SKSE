#pragma once
#include "Graph/LookupTable.h"

namespace PapyrusDatabase {
    using VM = RE::BSScript::IVirtualMachine;

    std::set<std::string> aggressive_class{"Ro", "HhPJ", "HhBj", "HhPo", "SJ"};
    std::set<std::string> aggressive_mod{"BG", "BB"};

    json BuildJson(pugi::xml_document& xml_doc, const fs::path& mod_path, const fs::path& cls_path) {
        auto node = new Graph::Node();
        auto j_obj = json::object();

        std::vector<std::string> split_id;

        if (auto scene = xml_doc.child("scene")) {
            if (auto id = scene.attribute("id")) {
                auto id_val = id.value();
                j_obj["sceneid"] = id_val;
                node->scene_id = id_val;
                split_id = stl::string_split(id_val, '|');
            }

            int actorCount = 1;
            if (auto actors = scene.attribute("actors")) {
                actorCount = actors.as_int();
            }
            j_obj["NumActors"] = actorCount;

            if (auto info = scene.child("info")) {
                if (auto name = info.attribute("name")) {
                    j_obj["name"] = name.value();
                    node->scene_name = name.value();
                }
            }
                

            j_obj["OAanimids"] = json::array();

            int is_transitory = 0;
            int is_hub = 1;

            if (auto anim = scene.child("anim"))
                if (auto transitory = anim.attribute("t")) {
                    std::string trans_str{transitory.value()};
                    if (trans_str == "T"s) {
                        is_transitory = 1;
                        is_hub = 0;
                    }
                }

            j_obj["istransitory"] = is_transitory;
            node->isTransition = is_transitory;

            auto speed = scene.child("speed");
            if (speed && !is_transitory) {
                if (auto max = speed.attribute("max")) is_hub = 0;

                if (!is_hub) {
                    if (auto actor = speed.attribute("a")) j_obj["mainActor"] = actor.as_int();

                    if (auto min = speed.attribute("min")) {
                        j_obj["minspeed"] = min.as_int();
                        node->minspeed = min.as_int();
                    }

                    if (auto max = speed.attribute("max")) {
                        j_obj["maxspeed"] = max.as_int();
                        node->maxspeed = max.as_int();
                    }

                    j_obj["hasIdleSpeed"] = 0;
                    node->hasIdleSpeed = false;

                    for (auto& sp : speed.children("sp")) {
                        if (auto mtx = sp.attribute("mtx")) {
                            std::string mtx_str{mtx.value()};
                            if (mtx_str == "^idle"s) {
                                j_obj["hasIdleSpeed"] = 1;
                                node->hasIdleSpeed = true;
                            }
                        }

                        if (auto anim = sp.child("anim")) {
                            if (auto id = anim.attribute("id")) {
                                j_obj["OAanimids"].push_back(id.value());
                                node->anim_ids.push_back(id.value());
                            }
                        }
                            
                    }
                }
            }

            j_obj["ishub"] = is_hub;
            node->isHub = is_hub;

            if (j_obj["OAanimids"].empty()) {
                auto anim_children = scene.children("anim");
                auto anim_count = std::distance(anim_children.begin(), anim_children.end());
                bool anim_first = true;

                for (auto& anim : anim_children) {
                    if (anim_first && (anim_count > 1)) {
                        anim_first = false;
                        continue;
                    }

                    if (auto id = anim.attribute("id")) {
                        j_obj["OAanimids"].push_back(id.value());
                        node->anim_ids.push_back(id.value());
                    }
                }
            }

            // what the hell?
            if (auto animations = scene.child("animation")) {
                auto role_children = animations.children("animrole");
                auto role_count = std::distance(role_children.begin(), role_children.end());
                bool role_first = true;

                for (auto& role : role_children) {
                    if (role_first && (role_count > 1)) {
                        role_first = false;
                        continue;
                    }

                    if (auto anim = role.child("animplan").child("anim"))
                        if (auto id = anim.attribute("id")) {
                            j_obj["OAanimids"].push_back(id.value());
                            node->anim_ids.push_back(id.value());
                        }
                }
            }

            if (auto metadata = scene.child("metadata")) {
                if (auto tags = metadata.attribute("tags")) {
                    char delimiter = ',';
                    std::string tagStr = tags.as_string();
                    std::transform(tagStr.begin(), tagStr.end(), tagStr.begin(), ::tolower);

                    auto tag_split = stl::string_split(tagStr, delimiter);
                    for (std::string tag : tag_split) {
                        node->tags.push_back(tag);
                    }
                }
            }

            for (int i = 0; i < actorCount; i++) {
                node->actors.push_back(new Graph::Actor());
            }

            if (auto actors = scene.child("actors")) {
                for (auto& actor : actors.children("actor")) {

                    if (auto position = actor.attribute("position")) {
                        int pos = position.as_int();
                        if (pos >= 0 && pos < actorCount) {

                            if (auto penisAngle = actor.attribute("penisAngle")) {
                                node->actors[pos]->penisAngle = penisAngle.as_int();
                            }

                            if (auto scale = actor.attribute("scale")) {
                                node->actors[pos]->scale = scale.as_float();
                            }

                            if (auto tags = actor.attribute("tags")) {
                                char delimiter = ',';
                                std::string tagStr = tags.as_string();
                                std::transform(tagStr.begin(), tagStr.end(), tagStr.begin(), ::tolower);

                                auto tag_split = stl::string_split(tagStr, delimiter);
                                for (std::string tag : tag_split) {
                                    node->actors[pos]->tags.push_back(tag);
                                }
                            }
                        }
                    }
                }
            }

            if (auto actions = scene.child("actions")) {
                for (auto& action : actions.children("action")) {
                    auto type = action.attribute("type");
                    auto actor = action.attribute("actor");
                    if (!type || !actor) {
                        continue;
                    }

                    auto actionObj = new Graph::Action();

                    actionObj->type = type.as_string();
                    actionObj->actor = actor.as_int();

                    if (auto target = action.attribute("target")) {
                        actionObj->target = target.as_int();
                    } else {
                        actionObj->target = actor.as_int();
                    }

                    if (auto performer = action.attribute("performer")) {
                        actionObj->performer = performer.as_int();
                    } else {
                        actionObj->performer = actor.as_int();
                    }

                    node->actions.push_back(actionObj);
                }
            }
        }

        auto anim_class = cls_path.filename().string();
        auto source_mod = mod_path.filename().string();

        // ugh...
        if (anim_class == "PO") anim_class = "Po";

        int is_aggressive = 0;
        if (aggressive_class.find(anim_class) != aggressive_class.end())
            is_aggressive = 1;
        else if (aggressive_mod.find(source_mod) != aggressive_mod.end())
            is_aggressive = 1;
        else if (anim_class.find("Ag") != std::string::npos)
            is_aggressive = 1;

        j_obj["aggressive"] = is_aggressive;
        node->isAggresive = is_aggressive;
        j_obj["sourcemodule"] = source_mod;
        node->sourceModule = source_mod;
        j_obj["animclass"] = anim_class;
        node->animClass = anim_class;
        j_obj["positiondata"] = split_id[1];

        Graph::LookupTable::AddNode(node);

        return j_obj;
    }

    void BuildDB() {
        const auto timer_start = std::chrono::high_resolution_clock::now();
        const fs::path root_path("Data\\Meshes\\0SA\\mod\\0Sex\\scene");

        if (!fs::exists(root_path)) {
            logger::info("scene folder not found");
            return;
        }

        auto j_root = json::array();
        for (auto const& mod : fs::directory_iterator(root_path)) {
            if (!mod.is_directory()) continue;

            auto& mod_path = mod.path();
            for (auto& pos : fs::directory_iterator(mod_path)) {
                if (!pos.is_directory()) continue;

                auto& pos_path = pos.path();
                auto posName = pos_path.filename().string();
                if (posName == "_TOG") continue;

                for (auto& cls : fs::directory_iterator(pos_path)) {
                    if (!cls.is_directory()) continue;

                    auto& cls_path = cls.path();
                    for (auto& file : fs::directory_iterator(cls_path)) {
                        if (!file.exists() || !file.is_regular_file()) continue;

                        auto& file_path = file.path();

                        auto fileExt = file_path.extension().string();
                        if (fileExt != ".xml") continue;

                        auto file_path_str = file_path.string();

                        pugi::xml_document xml_doc;
                        auto xml_result = xml_doc.load_file(file_path_str.c_str());

                        if (!xml_result) {
                            logger::error("load failed: {} [{}]", file_path_str, xml_result.description());
                            continue;
                        }

                        auto j_build = BuildJson(xml_doc, mod_path, cls_path);
                        j_root.push_back(j_build);
                    }
                }
            }
        }

        const auto db_path = util::database_path();
        std::ofstream db_file(*db_path);
        db_file << std::setw(2) << j_root << std::endl;

        const auto timer_finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> timer_elapsed = timer_finish - timer_start;
        logger::info("Build time: {}s", timer_elapsed.count());
    }

    std::vector<std::string> GetTags(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            return node->tags;
        }
        return std::vector<std::string>();
    }

    bool HasTag(RE::StaticFunctionTag*, std::string id, std::string tag) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            return std::find(node->tags.begin(), node->tags.end(), tag) != node->tags.end();
        }
        return false;
    }

    bool HasAnyTag(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            for (auto& tag : tags) {
                if (std::find(node->tags.begin(), node->tags.end(), tag) != node->tags.end()) {
                    return true;
                }
            }
        }
        return false;
    }

    bool HasAllTags(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            for (auto& tag : tags) {
                if (std::find(node->tags.begin(), node->tags.end(), tag) == node->tags.end()) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    std::vector<std::string> GetActorTags(RE::StaticFunctionTag*, std::string id, int position) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (node->actors.size() > position) {
                return node->actors[position]->tags;
            }
        }
        return std::vector<std::string>();
    }

    bool HasActorTag(RE::StaticFunctionTag*, std::string id, int position, std::string tag) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (node->actors.size() > position) {
                return std::find(node->actors[position]->tags.begin(), node->actors[position]->tags.end(), tag) !=
                       node->actors[position]->tags.end();
            }
        }
        return false;
    }

    bool HasAnyActorTag(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> tags) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (node->actors.size() > position) {
                for (auto& tag : tags) {
                    if( std::find(node->actors[position]->tags.begin(), node->actors[position]->tags.end(), tag) !=
                        node->actors[position]->tags.end()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool HasAllActorTags(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> tags) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (node->actors.size() > position) {
                for (auto& tag : tags) {
                    if (std::find(node->actors[position]->tags.begin(), node->actors[position]->tags.end(), tag) ==
                        node->actors[position]->tags.end()) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool HasActions(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            return !node->actions.empty();
        }
        return false;
    }

    int FindAction(RE::StaticFunctionTag*, std::string id, std::string type) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->type == type) {
                    return i;
                }
            }
        }
        return -1;
    }

    int FindAnyAction(RE::StaticFunctionTag*, std::string id, std::vector<std::string> types) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                for (auto& type : types) {
                    if (act->type == type) {
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    std::vector<int> FindActions(RE::StaticFunctionTag*, std::string id, std::string type) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->type == type) {
                    ret.push_back(i);
                }
            }
        }
        return ret;
    }

    std::vector<int> FindAllActions(RE::StaticFunctionTag*, std::string id, std::vector<std::string> types) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                for (auto& type : types) {
                    if (act->type == type) {
                        ret.push_back(i);
                    }
                }  
            }
        }
        return ret;
    }

    int FindActionForActor(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->actor == position && act->type == type) {
                    return i;
                }
            }
        }
        return -1;
    }

    int FindAnyActionForActor(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->actor == position) {
                    for (auto& type : types) {
                        if (act->type == type) {
                            return i;
                        }
                    }
                }
            }
        }
        return -1;
    }

    std::vector<int> FindActionsForActor(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->actor == position && act->type == type) {
                    ret.push_back(i);
                }
            }
        }
        return ret;
    }

    std::vector<int> FindAllActionsForActor(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->actor == position) {
                    for (auto& type : types) {
                        if (act->type == type) {
                            ret.push_back(i);
                        }
                    }
                }
            }
        }
        return ret;
    }

    int FindActionForTarget(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->target == position && act->type == type) {
                    return i;
                }
            }
        }
        return -1;
    }

    int FindAnyActionForTarget(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->target == position) {
                    for (auto& type : types) {
                        if (act->type == type) {
                            return i;
                        }
                    }
                }
            }
        }
        return -1;
    }

    std::vector<int> FindActionsForTarget(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->target == position && act->type == type) {
                    ret.push_back(i);
                }
            }
        }
        return ret;
    }

    std::vector<int> FindAllActionsForTarget(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->target == position) {
                    for (auto& type : types) {
                        if (act->type == type) {
                            ret.push_back(i);
                        }
                    }
                }
            }
        }
        return ret;
    }

    int FindActionForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->performer == position && act->type == type) {
                    return i;
                }
            }
        }
        return -1;
    }

    int FindAnyActionForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->performer == position) {
                    for (auto& type : types) {
                        if (act->type == type) {
                            return i;
                        }
                    }
                }
            }
        }
        return -1;
    }

    std::vector<int> FindActionsForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->performer == position && act->type == type) {
                    ret.push_back(i);
                }
            }
        }
        return ret;
    }

    std::vector<int> FindAllActionsForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto act = node->actions[i];
                if (act->performer == position) {
                    for (auto& type : types) {
                        if (act->type == type) {
                            ret.push_back(i);
                        }
                    }
                }
            }
        }
        return ret;
    }

    std::vector<std::string> GetActionTypes(RE::StaticFunctionTag*, std::string id) {
        std::vector<std::string> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->type);
            }
        }
        return ret;
    }

    std::string GetActionType(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->type;
            }
        }
        return "";
    }

    std::vector<int> GetActionActors(RE::StaticFunctionTag*, std::string id) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->actor);
            }
        }
        return ret;
    }

    int GetActionActor(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->actor;
            }
        }
        return -1;
    }

    std::vector<int> GetActionTargets(RE::StaticFunctionTag*, std::string id) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->target);
            }
        }
        return ret;
    }

    int GetActionTarget(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->target;
            }
        }
        return -1;
    }

    std::vector<int> GetActionPerformers(RE::StaticFunctionTag*, std::string id) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->performer);
            }
        }
        return ret;
    }

    int GetActionPerformer(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::GetNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->actor;
            }
        }
        return -1;
    }

    bool Bind(VM* a_vm) {
        const auto obj = "OData"sv;

        BIND(GetTags);
        BIND(HasTag);
        BIND(HasAnyTag);
        BIND(HasAllTags);
        BIND(GetActorTags);
        BIND(HasActorTag);
        BIND(HasAnyActorTag);
        BIND(HasAllActorTags);

        BIND(HasActions);
        BIND(FindAction);
        BIND(FindAnyAction);
        BIND(FindActions);
        BIND(FindAllActions);
        BIND(FindActionForActor);
        BIND(FindAnyActionForActor);
        BIND(FindActionsForActor);
        BIND(FindAllActionsForActor);
        BIND(FindActionForTarget);
        BIND(FindAnyActionForTarget);
        BIND(FindActionsForTarget);
        BIND(FindAllActionsForTarget);
        BIND(FindActionForPerformer);
        BIND(FindAnyActionForPerformer);
        BIND(FindActionsForPerformer);
        BIND(FindAllActionsForPerformer);
        BIND(GetActionTypes);
        BIND(GetActionType);
        BIND(GetActionActors);
        BIND(GetActionActor);
        BIND(GetActionTargets);
        BIND(GetActionTarget);
        BIND(GetActionPerformers);
        BIND(GetActionPerformer);

        return true;
    }
}  // namespace PapyrusDatabase