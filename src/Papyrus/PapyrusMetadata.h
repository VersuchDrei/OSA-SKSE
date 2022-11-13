#pragma once

#include "Graph/LookupTable.h"
#include "Util/StringUtil.h"
#include "Util/VectorUtil.h"
#include "Util.h"

namespace PapyrusMetadata {
    using VM = RE::BSScript::IVirtualMachine;

    int findAction(std::string id, std::function<bool(Graph::Action)> condition) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto action = node->actions[i];
                if (condition(*action)) {
                    return i;
                }
            }
        }
        return -1;
    }

    std::vector<int> findActions(std::string id, std::function<bool(Graph::Action)> condition) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            size_t size = node->actions.size();
            for (int i = 0; i < size; i++) {
                auto action = node->actions[i];
                if (condition(*action)) {
                    ret.push_back(i);
                }
            }
        }
        return ret;
    }

    // *********************************************************
    // start of papyrus bound functions
    // *********************************************************

    std::vector<std::string> GetTags(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->tags;
        }
        return std::vector<std::string>();
    }

    bool HasTag(RE::StaticFunctionTag*, std::string id, std::string tag) {
        StringUtil::toLower(&tag);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return VectorUtil::contains(node->tags, tag);
        }
        return false;
    }

    bool HasAnyTag(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return VectorUtil::containsAny(node->tags, tags);
        }
        return false;
    }

    bool HasAnyTagCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAnyTag(sft, id, StringUtil::toTagVector(tags));
    }

    bool HasAllTags(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return VectorUtil::containsAll(node->tags, tags);
        }
        return false;
    }

    bool HasAllTagsCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAllTags(sft, id, StringUtil::toTagVector(tags));
    }

    std::vector<std::string> GetActorTags(RE::StaticFunctionTag*, std::string id, int position) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (node->actors.size() > position) {
                return node->actors[position]->tags;
            }
        }
        return std::vector<std::string>();
    }

    bool HasActorTag(RE::StaticFunctionTag*, std::string id, int position, std::string tag) {
        StringUtil::toLower(&tag);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (node->actors.size() > position) {
                return node->hasActorTag(position, tag);
            }
        }
        return false;
    }

    bool HasAnyActorTag(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (node->actors.size() > position) {
                return VectorUtil::containsAny(node->actors[position]->tags, tags);
            }
        }
        return false;
    }

    bool HasAnyActorTagCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string tags) {
        return HasAnyActorTag(sft, id, position, StringUtil::toTagVector(tags));
    }

    bool HasAllActorTags(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (node->actors.size() > position) {
                return VectorUtil::containsAll(node->actors[position]->tags, tags);
            }
        }
        return false;
    }

    bool HasAllActorTagsCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string tags) {
        return HasAllActorTags(sft, id, position, StringUtil::toTagVector(tags));
    }

    bool HasActions(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return !node->actions.empty();
        }
        return false;
    }

    int FindAction(RE::StaticFunctionTag*, std::string id, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [type](Graph::Action action) { return action.type == type; });
    }

    int FindAnyAction(RE::StaticFunctionTag*, std::string id, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [types](Graph::Action action) { return VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionCSV(RE::StaticFunctionTag* sft, std::string id, std::string types) {
        return FindAnyAction(sft, id, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActions(RE::StaticFunctionTag*, std::string id, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [type](Graph::Action action) { return action.type == type; });
    }

    std::vector<int> FindAllActions(RE::StaticFunctionTag*, std::string id, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [types](Graph::Action action) { return VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsCSV(RE::StaticFunctionTag* sft, std::string id, std::string types) {
        return FindAllActions(sft, id, StringUtil::toTagVector(types));
    }

    int FindActionForActor(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [position, type](Graph::Action action) { return action.actor == position && action.type == type; });
    }

    int FindAnyActionForActor(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [position, types](Graph::Action action) { return action.actor == position && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForActorCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAnyActionForActor(sft, id, position, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForActor(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [position, type](Graph::Action action) { return action.actor == position && action.type == type; });
    }

    std::vector<int> FindAllActionsForActor(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [position, types](Graph::Action action) { return action.actor == position && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForActorCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAllActionsForActor(sft, id, position, StringUtil::toTagVector(types));
    }

    int FindActionForActors(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::contains(positions, action.actor) && action.type == type; });
    }

    int FindActionForActorsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForActors(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForActors(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::contains(positions, action.actor) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForActorsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForActors(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForActors(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::contains(positions, action.actor) && action.type == type; });
    }

    std::vector<int> FindActionsForActorsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForActors(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForActors(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::contains(positions, action.actor) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForActorsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForActors(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }


    int FindActionForTarget(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [position, type](Graph::Action action) { return action.target == position && action.type == type; });
    }

    int FindAnyActionForTarget(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [position, types](Graph::Action action) { return action.target == position && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForTargetCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAnyActionForTarget(sft, id, position, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForTarget(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [position, type](Graph::Action action) { return action.target == position && action.type == type; });
    }

    std::vector<int> FindAllActionsForTarget(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [position, types](Graph::Action action) { return action.target == position && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForTargetCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAllActionsForTarget(sft, id, position, StringUtil::toTagVector(types));
    }

    int FindActionForTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::contains(positions, action.target) && action.type == type; });
    }

    int FindActionForTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForTargets(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::contains(positions, action.target) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForTargets(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::contains(positions, action.target) && action.type == type; });
    }

    std::vector<int> FindActionsForTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForTargets(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::contains(positions, action.target) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForTargets(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }


    int FindActionForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [position, type](Graph::Action action) { return action.performer == position && action.type == type; });
    }

    int FindAnyActionForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [position, types](Graph::Action action) { return action.performer == position && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForPerformerCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAnyActionForPerformer(sft, id, position, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [position, type](Graph::Action action) { return action.performer == position && action.type == type; });
    }

    std::vector<int> FindAllActionsForPerformer(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [position, types](Graph::Action action) { return action.performer == position && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForPerformerCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAllActionsForPerformer(sft, id, position, StringUtil::toTagVector(types));
    }

    int FindActionForPerformers(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::contains(positions, action.performer) && action.type == type; });
    }

    int FindActionForPerformersCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForPerformers(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForPerformers(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::contains(positions, action.performer) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForPerformersCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForPerformers(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForPerformers(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::contains(positions, action.performer) && action.type == type; });
    }

    std::vector<int> FindActionsForPerformersCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForPerformers(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForPerformers(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::contains(positions, action.performer) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForPerformersCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForPerformers(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }


    int FindActionForMate(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [position, type](Graph::Action action) { return (action.actor == position || action.target == position) && action.type == type; });
    }

    int FindAnyActionForMate(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [position, types](Graph::Action action) { return (action.actor == position || action.target == position) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForMateCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAnyActionForMate(sft, id, position, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForMate(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [position, type](Graph::Action action) { return (action.actor == position || action.target == position) && action.type == type; });
    }

    std::vector<int> FindAllActionsForMate(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [position, types](Graph::Action action) { return (action.actor == position || action.target == position) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForMateCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAllActionsForMate(sft, id, position, StringUtil::toTagVector(types));
    }

    int FindActionForMatesAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target}) && action.type == type; });
    }

    int FindActionForMatesAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForMatesAny(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForMatesAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target}) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForMatesAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForMatesAny(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForMatesAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target}) && action.type == type; });
    }

    std::vector<int> FindActionsForMatesAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForMatesAny(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForMatesAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target}) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForMatesAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForMatesAny(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    int FindActionForMatesAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target}) && action.type == type; });
    }

    int FindActionForMatesAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForMatesAll(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForMatesAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target}) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForMatesAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForMatesAll(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForMatesAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target}) && action.type == type; });
    }

    std::vector<int> FindActionsForMatesAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForMatesAll(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForMatesAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target}) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForMatesAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForMatesAll(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }


    int FindActionForParticipant(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [position, type](Graph::Action action) { return (action.actor == position || action.target == position || action.performer == position) && action.type == type; });
    }

    int FindAnyActionForParticipant(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [position, types](Graph::Action action) { return (action.actor == position || action.target == position || action.performer == position) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForParticipantCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAnyActionForParticipant(sft, id, position, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForParticipant(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [position, type](Graph::Action action) { return (action.actor == position || action.target == position || action.performer == position) && action.type == type; });
    }

    std::vector<int> FindAllActionsForParticipant(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [position, types](Graph::Action action) { return (action.actor == position || action.target == position || action.performer == position) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForParticipantCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string types) {
        return FindAllActionsForParticipant(sft, id, position, StringUtil::toTagVector(types));
    }

    int FindActionForParticipantsAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target, action.performer}) && action.type == type; });
    }

    int FindActionForParticipantsAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForParticipantsAny(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForParticipantsAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target, action.performer}) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForParticipantsAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForParticipantsAny(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForParticipantsAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target, action.performer}) && action.type == type; });
    }

    std::vector<int> FindActionsForParticipantsAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForParticipantsAny(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForParticipantsAny(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::containsAny(positions, {action.actor, action.target, action.performer}) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForParticipantsAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForParticipantsAny(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    int FindActionForParticipantsAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [positions, type](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target, action.performer}) && action.type == type; });
    }

    int FindActionForParticipantsAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionForParticipantsAll(sft, id, VectorUtil::stoiv(positions), type);
    }

    int FindAnyActionForParticipantsAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [positions, types](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target, action.performer}) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForParticipantsAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAnyActionForParticipantsAll(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForParticipantsAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [positions, type](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target, action.performer}) && action.type == type; });
    }

    std::vector<int> FindActionsForParticipantsAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string type) {
        return FindActionsForParticipantsAll(sft, id, VectorUtil::stoiv(positions), type);
    }

    std::vector<int> FindAllActionsForParticipantsAll(RE::StaticFunctionTag*, std::string id, std::vector<int> positions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [positions, types](Graph::Action action) { return VectorUtil::containsAll(positions, {action.actor, action.target, action.performer}) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForParticipantsAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string positions, std::string types) {
        return FindAllActionsForParticipantsAll(sft, id, VectorUtil::stoiv(positions), StringUtil::toTagVector(types));
    }


    int FindActionSuperloadCSV(RE::StaticFunctionTag*, std::string id, std::string actorPositions, std::string targetPositions, std::string performerPositions, std::string matePositionsAny, std::string matePositionsAll, std::string participantPositionsAny, std::string participantPositionsAll, std::string types) {
        std::vector<int> actorPos = actorPositions.empty() ? VectorUtil::stoiv(actorPositions) : *(new std::vector<int>());
        std::vector<int> targetPos = targetPositions.empty() ? VectorUtil::stoiv(targetPositions) : *(new std::vector<int>());
        std::vector<int> performerPos = performerPositions.empty() ? VectorUtil::stoiv(performerPositions) : *(new std::vector<int>());
        std::vector<int> matePosAny = VectorUtil::stoiv(matePositionsAny);
        std::vector<int> matePosAll = VectorUtil::stoiv(matePositionsAll);
        std::vector<int> participantPosAny = VectorUtil::stoiv(participantPositionsAny);
        std::vector<int> participantPosAll = VectorUtil::stoiv(participantPositionsAll);
        std::vector<std::string> typeVector = types.empty() ? StringUtil::toTagVector(types) : *(new std::vector<std::string>());

        return findAction(id, [actorPos, targetPos, performerPos, matePosAny, matePosAll, participantPosAny, participantPosAll, typeVector](Graph::Action action) {
                return (actorPos.empty() || VectorUtil::contains(actorPos, action.actor)) &&
                    (targetPos.empty() || VectorUtil::contains(targetPos, action.target)) &&
                    (performerPos.empty() || VectorUtil::contains(performerPos, action.performer)) &&
                    (matePosAny.empty() || VectorUtil::containsAny(matePosAny, {action.actor, action.target})) &&
                    (matePosAll.empty() || VectorUtil::containsAll(matePosAll, {action.actor, action.target})) &&
                    (participantPosAny.empty() || VectorUtil::containsAny(participantPosAny, {action.actor, action.target, action.performer})) &&
                    (participantPosAll.empty() || VectorUtil::containsAll(participantPosAll, {action.actor, action.target, action.performer})) &&
                    (typeVector.empty() || VectorUtil::contains(typeVector, action.type));
            });
    }

    std::vector<int> FindActionsSuperloadCSV(RE::StaticFunctionTag*, std::string id, std::string actorPositions, std::string targetPositions, std::string performerPositions, std::string matePositionsAny, std::string matePositionsAll, std::string participantPositionsAny, std::string participantPositionsAll, std::string types) {
        std::vector<int> actorPos = VectorUtil::stoiv(actorPositions);
        std::vector<int> targetPos = VectorUtil::stoiv(targetPositions);
        std::vector<int> performerPos = VectorUtil::stoiv(performerPositions);
        std::vector<int> matePosAny = VectorUtil::stoiv(matePositionsAny);
        std::vector<int> matePosAll = VectorUtil::stoiv(matePositionsAll);
        std::vector<int> participantPosAny = VectorUtil::stoiv(participantPositionsAny);
        std::vector<int> participantPosAll = VectorUtil::stoiv(participantPositionsAll);
        std::vector<std::string> typeVector = StringUtil::toTagVector(types);

        return findActions(id, [actorPos, targetPos, performerPos, matePosAny, matePosAll, participantPosAny, participantPosAll, typeVector](Graph::Action action) {
                return (actorPos.empty() || VectorUtil::contains(actorPos, action.actor)) &&
                    (targetPos.empty() || VectorUtil::contains(targetPos, action.target)) &&
                    (performerPos.empty() || VectorUtil::contains(performerPos, action.performer)) &&
                    (matePosAny.empty() || VectorUtil::containsAny(matePosAny, {action.actor, action.target})) &&
                    (matePosAll.empty() || VectorUtil::containsAll(matePosAll, {action.actor, action.target})) &&
                    (participantPosAny.empty() || VectorUtil::containsAny(participantPosAny, {action.actor, action.target, action.performer})) &&
                    (participantPosAll.empty() || VectorUtil::containsAll(participantPosAll, {action.actor, action.target, action.performer})) &&
                    (typeVector.empty() || VectorUtil::contains(typeVector, action.type));
            });
    }


    std::vector<std::string> GetActionTypes(RE::StaticFunctionTag*, std::string id) {
        std::vector<std::string> ret;
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->type);
            }
        }
        return ret;
    }

    std::string GetActionType(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->type;
            }
        }
        return "";
    }

    std::vector<int> GetActionActors(RE::StaticFunctionTag*, std::string id) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->actor);
            }
        }
        return ret;
    }

    int GetActionActor(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->actor;
            }
        }
        return -1;
    }

    std::vector<int> GetActionTargets(RE::StaticFunctionTag*, std::string id) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->target);
            }
        }
        return ret;
    }

    int GetActionTarget(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->target;
            }
        }
        return -1;
    }

    std::vector<int> GetActionPerformers(RE::StaticFunctionTag*, std::string id) {
        std::vector<int> ret;
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (auto& action : node->actions) {
                ret.push_back(action->performer);
            }
        }
        return ret;
    }

    int GetActionPerformer(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->actor;
            }
        }
        return -1;
    }


	bool Bind(VM* a_vm) {
        const auto obj = "OMetadata"sv;

        BIND(GetTags);
        BIND(HasTag);
        BIND(HasAnyTag);
        BIND(HasAnyTagCSV);
        BIND(HasAllTags);
        BIND(HasAllTagsCSV);
        BIND(GetActorTags);
        BIND(HasActorTag);
        BIND(HasAnyActorTag);
        BIND(HasAnyActorTagCSV);
        BIND(HasAllActorTags);
        BIND(HasAllActorTagsCSV);

        BIND(HasActions);
        BIND(FindAction);
        BIND(FindAnyAction);
        BIND(FindAnyActionCSV);
        BIND(FindActions);
        BIND(FindAllActions);
        BIND(FindAllActionsCSV);

        BIND(FindActionForActor);
        BIND(FindAnyActionForActor);
        BIND(FindAnyActionForActorCSV);
        BIND(FindActionsForActor);
        BIND(FindAllActionsForActor);
        BIND(FindAllActionsForActorCSV);
        BIND(FindActionForActors);
        BIND(FindActionForActorsCSV);
        BIND(FindAnyActionForActors);
        BIND(FindAnyActionForActorsCSV);
        BIND(FindActionsForActors);
        BIND(FindActionsForActorsCSV);
        BIND(FindAllActionsForActors);
        BIND(FindAllActionsForActorsCSV);

        BIND(FindActionForTarget);
        BIND(FindAnyActionForTarget);
        BIND(FindAnyActionForTargetCSV);
        BIND(FindActionsForTarget);
        BIND(FindAllActionsForTarget);
        BIND(FindAllActionsForTargetCSV);
        BIND(FindActionForTargets);
        BIND(FindActionForTargetsCSV);
        BIND(FindAnyActionForTargets);
        BIND(FindAnyActionForTargetsCSV);
        BIND(FindActionsForTargets);
        BIND(FindActionsForTargetsCSV);
        BIND(FindAllActionsForTargets);
        BIND(FindAllActionsForTargetsCSV);

        BIND(FindActionForPerformer);
        BIND(FindAnyActionForPerformer);
        BIND(FindAnyActionForPerformerCSV);
        BIND(FindActionsForPerformer);
        BIND(FindAllActionsForPerformer);
        BIND(FindAllActionsForPerformerCSV);
        BIND(FindActionForPerformers);
        BIND(FindActionForPerformersCSV);
        BIND(FindAnyActionForPerformers);
        BIND(FindAnyActionForPerformersCSV);
        BIND(FindActionsForPerformers);
        BIND(FindActionsForPerformersCSV);
        BIND(FindAllActionsForPerformers);
        BIND(FindAllActionsForPerformersCSV);

        BIND(FindActionForMate);
        BIND(FindAnyActionForMate);
        BIND(FindAnyActionForMateCSV);
        BIND(FindActionsForMate);
        BIND(FindAllActionsForMate);
        BIND(FindAllActionsForMateCSV);
        BIND(FindActionForMatesAny);
        BIND(FindActionForMatesAnyCSV);
        BIND(FindAnyActionForMatesAny);
        BIND(FindAnyActionForMatesAnyCSV);
        BIND(FindActionsForMatesAny);
        BIND(FindActionsForMatesAnyCSV);
        BIND(FindAllActionsForMatesAny);
        BIND(FindAllActionsForMatesAnyCSV);
        BIND(FindActionForMatesAll);
        BIND(FindActionForMatesAllCSV);
        BIND(FindAnyActionForMatesAll);
        BIND(FindAnyActionForMatesAllCSV);
        BIND(FindActionsForMatesAll);
        BIND(FindActionsForMatesAllCSV);
        BIND(FindAllActionsForMatesAll);
        BIND(FindAllActionsForMatesAllCSV);

        BIND(FindActionForParticipant);
        BIND(FindAnyActionForParticipant);
        BIND(FindAnyActionForParticipantCSV);
        BIND(FindActionsForParticipant);
        BIND(FindAllActionsForParticipant);
        BIND(FindAllActionsForParticipantCSV);
        BIND(FindActionForParticipantsAny);
        BIND(FindActionForParticipantsAnyCSV);
        BIND(FindAnyActionForParticipantsAny);
        BIND(FindAnyActionForParticipantsAnyCSV);
        BIND(FindActionsForParticipantsAny);
        BIND(FindActionsForParticipantsAnyCSV);
        BIND(FindAllActionsForParticipantsAny);
        BIND(FindAllActionsForParticipantsAnyCSV);
        BIND(FindActionForParticipantsAll);
        BIND(FindActionForParticipantsAllCSV);
        BIND(FindAnyActionForParticipantsAll);
        BIND(FindAnyActionForParticipantsAllCSV);
        BIND(FindActionsForParticipantsAll);
        BIND(FindActionsForParticipantsAllCSV);
        BIND(FindAllActionsForParticipantsAll);
        BIND(FindAllActionsForParticipantsAllCSV);

        BIND(FindActionSuperloadCSV);
        BIND(FindActionsSuperloadCSV);

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
}