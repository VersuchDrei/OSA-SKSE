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

    bool IsTransition(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->isTransition;
        }
        return false;
    }

    int GetDefaultSpeed(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->defaultSpeed;
        }
        return 0;
    }

    int GetMaxSpeed(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->speeds.size() - 1;
        }
        return 0;
    }

    int GetActorCount(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->actors.size();
        }
        return 0;
    }

    std::string GetAnimationId(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (node->speeds.size() > index) {
                return node->speeds[index].animation;
            }
        }
        return "";
    }

    std::string GetAutoTransitionForActor(RE::StaticFunctionTag*, std::string id, int position, std::string type) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->getAutoTransitionForActor(position, type);
        }
        return "";
    }


    std::vector<std::string> GetSceneTags(RE::StaticFunctionTag*, std::string id) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return node->tags;
        }
        return std::vector<std::string>();
    }

    bool HasSceneTag(RE::StaticFunctionTag*, std::string id, std::string tag) {
        StringUtil::toLower(&tag);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return VectorUtil::contains(node->tags, tag);
        }
        return false;
    }

    bool HasAnySceneTag(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return VectorUtil::containsAny(node->tags, tags);
        }
        return false;
    }

    bool HasAnySceneTagCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAnySceneTag(sft, id, StringUtil::toTagVector(tags));
    }

    bool HasAllSceneTags(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            return VectorUtil::containsAll(node->tags, tags);
        }
        return false;
    }

    bool HasAllSceneTagsCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAllSceneTags(sft, id, StringUtil::toTagVector(tags));
    }

    std::vector<std::string> GetSceneTagOverlap(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        std::vector<std::string> ret;

        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (auto& tag : node->tags) {
                if (VectorUtil::contains(tags, tag)) {
                    ret.push_back(tag);
                }
            }
        }

        return ret;
    }

    std::string GetSceneTagOverlapCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return StringUtil::toTagCSV(GetSceneTagOverlap(sft, id, StringUtil::toTagVector(tags)));
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

    std::vector<std::string> GetActorTagOverlap(RE::StaticFunctionTag*, std::string id, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        std::vector<std::string> ret;

        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (node->actors.size() > position) {
                for (auto& tag : node->actors[position]->tags) {
                    if (VectorUtil::contains(tags, tag)) {
                        ret.push_back(tag);
                    }
                }
            }
            
        }

        return ret;
    }

    std::string GetActorTagOverlapCSV(RE::StaticFunctionTag* sft, std::string id, int position, std::string tags) {
        return StringUtil::toTagCSV(GetActorTagOverlap(sft, id, position, StringUtil::toTagVector(tags)));
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
    

    int FindActionForActorAndTarget(RE::StaticFunctionTag*, std::string id, int actorPosition, int targetPosition, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [actorPosition, targetPosition, type](Graph::Action action) { return action.actor == actorPosition && action.target == targetPosition && action.type == type; });
    }

    int FindAnyActionForActorAndTarget(RE::StaticFunctionTag*, std::string id, int actorPosition, int targetPosition, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [actorPosition, targetPosition, types](Graph::Action action) { return action.actor == actorPosition && action.target == targetPosition && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForActorAndTargetCSV(RE::StaticFunctionTag* sft, std::string id, int actorPosition, int targetPosition, std::string types) {
        return FindAnyActionForActorAndTarget(sft, id, actorPosition, targetPosition, StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForActorAndTarget(RE::StaticFunctionTag*, std::string id, int actorPosition, int targetPosition, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [actorPosition, targetPosition, type](Graph::Action action) { return action.actor == actorPosition && action.target == targetPosition && action.type == type; });
    }

    std::vector<int> FindAllActionsForActorAndTarget(RE::StaticFunctionTag*, std::string id, int actorPosition, int targetPosition, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [actorPosition, targetPosition, types](Graph::Action action) { return action.actor == actorPosition && action.target == targetPosition && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForActorAndTargetCSV(RE::StaticFunctionTag* sft, std::string id, int actorPosition, int targetPosition, std::string types) {
        return FindAllActionsForActorAndTarget(sft, id, actorPosition, targetPosition, StringUtil::toTagVector(types));
    }

    int FindActionForActorsAndTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> actorPositions, std::vector<int> targetPositions, std::string type) {
        StringUtil::toLower(&type);
        return findAction(id, [actorPositions, targetPositions, type](Graph::Action action) { return VectorUtil::contains(actorPositions, action.actor) && VectorUtil::contains(targetPositions, action.target) && action.type == type; });
    }

    int FindActionForActorsAndTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string actorPositions, std::string targetPositions, std::string type) {
        return FindActionForActorsAndTargets(sft, id, VectorUtil::stoiv(actorPositions), VectorUtil::stoiv(targetPositions), type);
    }

    int FindAnyActionForActorsAndTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> actorPositions, std::vector<int> targetPositions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findAction(id, [actorPositions, targetPositions, types](Graph::Action action) { return VectorUtil::contains(actorPositions, action.actor) && VectorUtil::contains(targetPositions, action.target) && VectorUtil::contains(types, action.type); });
    }

    int FindAnyActionForActorsAndTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string actorPositions, std::string targetPositions, std::string types) {
        return FindAnyActionForActorsAndTargets(sft, id, VectorUtil::stoiv(actorPositions), VectorUtil::stoiv(targetPositions), StringUtil::toTagVector(types));
    }

    std::vector<int> FindActionsForActorsAndTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> actorPositions, std::vector<int> targetPositions, std::string type) {
        StringUtil::toLower(&type);
        return findActions(id, [actorPositions, targetPositions, type](Graph::Action action) { return VectorUtil::contains(actorPositions, action.actor) && VectorUtil::contains(targetPositions, action.target) && action.type == type; });
    }

    std::vector<int> FindActionsForActorsAndTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string actorPositions, std::string targetPositions, std::string type) {
        return FindActionsForActorsAndTargets(sft, id, VectorUtil::stoiv(actorPositions), VectorUtil::stoiv(targetPositions), type);
    }

    std::vector<int> FindAllActionsForActorsAndTargets(RE::StaticFunctionTag*, std::string id, std::vector<int> actorPositions, std::vector<int> targetPositions, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return findActions(id, [actorPositions, targetPositions, types](Graph::Action action) { return VectorUtil::contains(actorPositions, action.actor) && VectorUtil::contains(targetPositions, action.target) && VectorUtil::contains(types, action.type); });
    }

    std::vector<int> FindAllActionsForActorsAndTargetsCSV(RE::StaticFunctionTag* sft, std::string id, std::string actorPositions, std::string targetPositions, std::string types) {
        return FindAllActionsForActorsAndTargets(sft, id, VectorUtil::stoiv(actorPositions), VectorUtil::stoiv(targetPositions), StringUtil::toTagVector(types));
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
        std::vector<int> actorPos = VectorUtil::stoiv(actorPositions);
        std::vector<int> targetPos = VectorUtil::stoiv(targetPositions);
        std::vector<int> performerPos = VectorUtil::stoiv(performerPositions);
        std::vector<int> matePosAny = VectorUtil::stoiv(matePositionsAny);
        std::vector<int> matePosAll = VectorUtil::stoiv(matePositionsAll);
        std::vector<int> participantPosAny = VectorUtil::stoiv(participantPositionsAny);
        std::vector<int> participantPosAll = VectorUtil::stoiv(participantPositionsAll);
        std::vector<std::string> typeVector = StringUtil::toTagVector(types);

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
    

    std::vector<std::string> GetActionTags(RE::StaticFunctionTag*, std::string id, int index) {
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return node->actions[index]->attributes->tags;
            }
        }
        return {};
    }

    std::vector<std::string> GetAllActionTags(RE::StaticFunctionTag*, std::string id) {
        std::set<std::string> tags;
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (Graph::Action* action : node->actions) {
                for (std::string tag : action->attributes->tags) {
                    tags.insert(tag);
                }
            }
        }
        return VectorUtil::toVector(tags);
    }

    bool HasActionTag(RE::StaticFunctionTag*, std::string id, int index, std::string tag) {
        StringUtil::toLower(&tag);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return VectorUtil::contains(node->actions[index]->attributes->tags, tag);
            }
        }
        return false;
    }

    bool HasActionTagOnAny(RE::StaticFunctionTag*, std::string id, std::string tag) {
        StringUtil::toLower(&tag);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (Graph::Action* action: node->actions) {
                if (VectorUtil::contains(action->attributes->tags, tag)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool HasAnyActionTag(RE::StaticFunctionTag*, std::string id, int index, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return VectorUtil::containsAny(node->actions[index]->attributes->tags, tags);
            }
        }
        return false;
    }

    bool HasAnyActionTagCSV(RE::StaticFunctionTag* sft, std::string id, int index, std::string tags) {
        return HasAnyActionTag(sft, id, index, StringUtil::toTagVector(tags));
    }

    bool HasAnyActionTagOnAny(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (Graph::Action* action : node->actions) {
                if (VectorUtil::containsAny(action->attributes->tags, tags)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool HasAnyActionTagOnAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAnyActionTagOnAny(sft, id, StringUtil::toTagVector(tags));
    }

    bool HasAllActionTags(RE::StaticFunctionTag*, std::string id, int index, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                return VectorUtil::containsAll(node->actions[index]->attributes->tags, tags);
            }
        }
        return false;
    }

    bool HasAllActionTagsCSV(RE::StaticFunctionTag* sft, std::string id, int index, std::string tags) {
        return HasAllActionTags(sft, id, index, StringUtil::toTagVector(tags));
    }

    bool HasAllActionTagsOnAny(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (Graph::Action* action : node->actions) {
                if (VectorUtil::containsAll(action->attributes->tags, tags)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool HasAllActionTagsOnAnyCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAllActionTagsOnAny(sft, id, StringUtil::toTagVector(tags));
    }

    bool HasAllActionTagsOverAll(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (Graph::Action* action : node->actions) {
                std::erase_if(tags, [action](std::string tag) {return VectorUtil::contains(action->attributes->tags, tag);});
                if (tags.empty()) {
                    return true;
                }
            }
        }
        return false;
    }

    bool HasAllActionTagsOverAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return HasAllActionTagsOverAll(sft, id, StringUtil::toTagVector(tags));
    }

    std::vector<std::string> GetActionTagOverlap(RE::StaticFunctionTag*, std::string id, int index, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        std::vector<std::string> ret;

        if (auto node = Graph::LookupTable::getNodeById(id)) {
            if (index < node->actions.size()) {
                for (auto& tag : node->actions[index]->attributes->tags) {
                    if (VectorUtil::contains(tags, tag)) {
                        ret.push_back(tag);
                    }
                }
            }
        }

        return ret;
    }

    std::string GetActionTagOverlapCSV(RE::StaticFunctionTag* sft, std::string id, int index, std::string tags) {
        return StringUtil::toTagCSV(GetActionTagOverlap(sft, id, index, StringUtil::toTagVector(tags)));
    }

    std::vector<std::string> GetActionTagOverlapOverAll(RE::StaticFunctionTag*, std::string id, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        std::set<std::string> ret;

        if (auto node = Graph::LookupTable::getNodeById(id)) {
            for (Graph::Action* action : node->actions) {
                for (auto& tag : action->attributes->tags) {
                    if (VectorUtil::contains(tags, tag)) {
                        ret.insert(tag);
                    }
                }
            }
        }

        return VectorUtil::toVector(ret);
    }

    std::string GetActionTagOverlapOverAllCSV(RE::StaticFunctionTag* sft, std::string id, std::string tags) {
        return StringUtil::toTagCSV(GetActionTagOverlapOverAll(sft, id, StringUtil::toTagVector(tags)));
    }


	bool Bind(VM* a_vm) {
        const auto obj = "OMetadata"sv;

        BIND(IsTransition);
        BIND(GetDefaultSpeed);
        BIND(GetMaxSpeed);
        BIND(GetActorCount);
        BIND(GetAnimationId);
        BIND(GetAutoTransitionForActor);

        BIND(GetSceneTags);
        BIND(HasSceneTag);
        BIND(HasAnySceneTag);
        BIND(HasAnySceneTagCSV);
        BIND(HasAllSceneTags);
        BIND(HasAllSceneTagsCSV);
        BIND(GetSceneTagOverlap);
        BIND(GetSceneTagOverlapCSV);

        BIND(GetActorTags);
        BIND(HasActorTag);
        BIND(HasAnyActorTag);
        BIND(HasAnyActorTagCSV);
        BIND(HasAllActorTags);
        BIND(HasAllActorTagsCSV);
        BIND(GetActorTagOverlap);
        BIND(GetActorTagOverlapCSV);

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

        BIND(FindActionForActorAndTarget);
        BIND(FindAnyActionForActorAndTarget);
        BIND(FindAnyActionForActorAndTargetCSV);
        BIND(FindActionsForActorAndTarget);
        BIND(FindAllActionsForActorAndTarget);
        BIND(FindAllActionsForActorAndTargetCSV);
        BIND(FindActionForActorsAndTargets);
        BIND(FindActionForActorsAndTargetsCSV);
        BIND(FindAnyActionForActorsAndTargets);
        BIND(FindAnyActionForActorsAndTargetsCSV);
        BIND(FindActionsForActorsAndTargets);
        BIND(FindActionsForActorsAndTargetsCSV);
        BIND(FindAllActionsForActorsAndTargets);
        BIND(FindAllActionsForActorsAndTargetsCSV);

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

        BIND(GetActionTags);
        BIND(GetAllActionTags);
        BIND(HasActionTag);
        BIND(HasActionTagOnAny);
        BIND(HasAnyActionTag);
        BIND(HasAnyActionTagCSV);
        BIND(HasAnyActionTagOnAny);
        BIND(HasAnyActionTagOnAnyCSV);
        BIND(HasAllActionTags);
        BIND(HasAllActionTagsCSV);
        BIND(HasAllActionTagsOnAny);
        BIND(HasAllActionTagsOnAnyCSV);
        BIND(HasAllActionTagsOverAll);
        BIND(HasAllActionTagsOverAllCSV);
        BIND(GetActionTagOverlap);
        BIND(GetActionTagOverlapCSV);
        BIND(GetActionTagOverlapOverAll);
        BIND(GetActionTagOverlapOverAllCSV);

        return true;
	}
}