#pragma once

#include "Graph/LookupTable.h"
#include "Trait/Condition.h"
#include "Util/StringUtil.h"
#include "Util/VectorUtil.h"

namespace PapyrusLibrary {
    using VM = RE::BSScript::IVirtualMachine;

    std::string randomScene(std::vector<RE::Actor*> actors, std::string furnitureType, std::function<bool(Graph::Node*)> condition) {
        if (Graph::Node* node = Graph::LookupTable::getRandomNode(Graph::LookupTable::getFurnitureType(furnitureType), Trait::ActorConditions::create(actors), condition)) {
            return node->scene_id;
        }
        return "";
    }

    std::string randomScene(std::vector<RE::Actor*> actors, std::function<bool(Graph::Node*)> condition) {
        return randomScene(actors, "", condition);
    }

    bool forAnyLoop(int max, std::function<bool(int)> condition) {
        for (int i = 0; i < max; i++) {
            if (condition(i)) {
                return true;
            }
        }
        return false;
    }

    bool forAllLoop(int max, std::function<bool(int)> condition) {
        for (int i = 0; i < max; i++) {
            if (!condition(i)) {
                return false;
            }
        }
        return true;
    }

    // *********************************************************
    // helper functions specifically for superload
    // *********************************************************
    bool hasAction(Graph::Node* node, std::string type, std::vector<int> actors, std::vector<int> targets, std::vector<int> performers, std::vector<int> matesAny, std::vector<int> matesAll, std::vector<int> participantsAny, std::vector<int> participantsAll) {
        for (Graph::Action* action : node->actions) {
            if (action->type == type &&
                (actors.empty() || VectorUtil::contains(actors, action->actor)) &&
                (targets.empty() || VectorUtil::contains(targets, action->target)) &&
                (performers.empty() || VectorUtil::contains(performers, action->performer)) &&
                (matesAny.empty() || VectorUtil::containsAny(matesAny, {action->actor, action->target})) &&
                (matesAll.empty() || VectorUtil::containsAll(matesAll, {action->actor, action->target})) &&
                (participantsAny.empty() || VectorUtil::containsAny(participantsAny, {action->actor, action->target, action->performer})) &&
                (participantsAll.empty() || VectorUtil::containsAll(participantsAll, {action->actor, action->target, action->performer}))) {
                return true;
            }
        }

        return false;
    }

    bool hasAnyAction(Graph::Node* node, std::vector<std::string> types, std::vector<std::vector<int>> actors, std::vector<std::vector<int>> targets, std::vector<std::vector<int>> performers, std::vector<std::vector<int>> matesAny, std::vector<std::vector<int>> matesAll, std::vector<std::vector<int>> participantsAny, std::vector<std::vector<int>> participantsAll) {
        for (int i = 0; i < types.size(); i++) {
            if (hasAction(node, types[i], VectorUtil::getElementOrEmpty(actors, i), VectorUtil::getElementOrEmpty(targets, i), VectorUtil::getElementOrEmpty(performers, i), VectorUtil::getElementOrEmpty(matesAny, i), VectorUtil::getElementOrEmpty(matesAll, i), VectorUtil::getElementOrEmpty(participantsAny, i), VectorUtil::getElementOrEmpty(participantsAll, i))) {
                return true;
            }
        }

        return false;
    }

    bool hasAllActions(Graph::Node* node, std::vector<std::string> types, std::vector<std::vector<int>> actors, std::vector<std::vector<int>> targets, std::vector<std::vector<int>> performers, std::vector<std::vector<int>> matesAny, std::vector<std::vector<int>> matesAll, std::vector<std::vector<int>> participantsAny, std::vector<std::vector<int>> participantsAll) {
        for (int i = 0; i < types.size(); i++) {
            if (!hasAction(node, types[i], VectorUtil::getElementOrEmpty(actors, i), VectorUtil::getElementOrEmpty(targets, i), VectorUtil::getElementOrEmpty(performers, i), VectorUtil::getElementOrEmpty(matesAny, i), VectorUtil::getElementOrEmpty(matesAll, i), VectorUtil::getElementOrEmpty(participantsAny, i), VectorUtil::getElementOrEmpty(participantsAll, i))) {
                return false;
            }
        }

        return true;
    }

    // *********************************************************
    // start of papyrus bound functions
    // *********************************************************

    std::string GetRandomScene(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors) {
        return randomScene(actors, [](Graph::Node* node) { return true; });
    }

    std::string GetRandomFurnitureScene(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType) {
        return randomScene(actors, furnitureType, [](Graph::Node* node) { return true; });
    }


    std::string GetRandomSceneWithTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string tag) {
        StringUtil::toLower(&tag);
        return randomScene(actors, [tag](Graph::Node* node) { return VectorUtil::contains(node->tags, tag); });
    }

    std::string GetRandomSceneWithAnyTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, [tags](Graph::Node* node) { return VectorUtil::containsAny(node->tags, tags); });
    }

    std::string GetRandomSceneWithAnyTagCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string tags) {
        return GetRandomSceneWithAnyTag(sft, actors, StringUtil::toTagVector(tags));
    }

    std::string GetRandomSceneWithAllTags(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, [tags](Graph::Node* node) { return VectorUtil::containsAll(node->tags, tags); });
    }

    std::string GetRandomSceneWithAllTagsCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string tags) {
        return GetRandomSceneWithAllTags(sft, actors, StringUtil::toTagVector(tags));
    }


    std::string GetRandomFurnitureSceneWithTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string tag) {
        StringUtil::toLower(&tag);
        return randomScene(actors, furnitureType, [tag](Graph::Node* node) { return VectorUtil::contains(node->tags, tag); });
    }

    std::string GetRandomFurnitureSceneWithAnyTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, furnitureType, [tags](Graph::Node* node) { return VectorUtil::containsAny(node->tags, tags); });
    }

    std::string GetRandomFurnitureSceneWithAnyTagCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, std::string tags) {
        return GetRandomFurnitureSceneWithAnyTag(sft, actors, furnitureType, StringUtil::toTagVector(tags));
    }

    std::string GetRandomFurnitureSceneWithAllTags(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, furnitureType, [tags](Graph::Node* node) { return VectorUtil::containsAll(node->tags, tags); });
    }

    std::string GetRandomFurnitureSceneWithAllTagsCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, std::string tags) {
        return GetRandomFurnitureSceneWithAllTags(sft, actors, furnitureType, StringUtil::toTagVector(tags));
    }
    

    std::string GetRandomSceneWithSingleActorTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::string tag) {
        StringUtil::toLower(&tag);
        return randomScene(actors, [position, tag](Graph::Node* node) { return node->hasActorTag(position, tag); });
    }

    std::string GetRandomSceneWithAnySingleActorTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, [position, tags](Graph::Node* node) { return node->hasAnyActorTag(position, tags); });
    }

    std::string GetRandomSceneWithAnySingleActorTagCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, int position, std::string tags) {
        return GetRandomSceneWithAnySingleActorTag(sft, actors, position, StringUtil::toTagVector(tags));
    }

    std::string GetRandomSceneWithAllSingleActorTags(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, [position, tags](Graph::Node* node) { return node->hasAllActorTags(position, tags); });
    }

    std::string GetRandomSceneWithAllSingleActorTagsCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, int position, std::string tags) {
        return GetRandomSceneWithAllSingleActorTags(sft, actors, position, StringUtil::toTagVector(tags));
    }
    

    std::string GetRandomFurnitureSceneWithSingleActorTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string tag) {
        StringUtil::toLower(&tag);
        return randomScene(actors, furnitureType, [position, tag](Graph::Node* node) { return node->hasActorTag(position, tag); });
    }

    std::string GetRandomFurnitureSceneWithAnySingleActorTag(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, furnitureType, [position, tags](Graph::Node* node) { return node->hasAnyActorTag(position, tags); });
    }

    std::string GetRandomFurnitureSceneWithAnySingleActorTagCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string tags) {
        return GetRandomFurnitureSceneWithAnySingleActorTag(sft, actors, furnitureType, position, StringUtil::toTagVector(tags));
    }

    std::string GetRandomFurnitureSceneWithAllSingleActorTags(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, furnitureType, [position, tags](Graph::Node* node) { return node->hasAllActorTags(position, tags); });
    }

    std::string GetRandomFurnitureSceneWithAllSingleActorTagsCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string tags) {
        return GetRandomFurnitureSceneWithAllSingleActorTags(sft, actors, furnitureType, position, StringUtil::toTagVector(tags));
    }


    std::string GetRandomSceneWithMultiActorTagForAny(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, [tags](Graph::Node* node) { return forAnyLoop(tags.size(), [node, tags](int i) {return node->hasActorTag(i, tags[i]);}); });
    }

    std::string GetRandomSceneWithMultiActorTagForAnyCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string tags) {
        return GetRandomSceneWithMultiActorTagForAny(sft, actors, StringUtil::toTagVector(tags));
    }

    std::string GetRandomSceneWithMultiActorTagForAll(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, [tags](Graph::Node* node) { return forAllLoop(tags.size(), [node, tags](int i) {return node->hasActorTag(i, tags[i]);}); });
    }

    std::string GetRandomSceneWithMultiActorTagForAllCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string tags) {
        return GetRandomSceneWithMultiActorTagForAll(sft, actors, StringUtil::toTagVector(tags));
    }

    std::string GetRandomSceneWithAnyMultiActorTagForAnyCSV(RE::StaticFunctionTag*, std::vector < RE::Actor*> actors, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, [matrix](Graph::Node* node) { return forAnyLoop(matrix.size(), [node, matrix](int i) {return node->hasAnyActorTag(i, matrix[i]);}); });
    }

    std::string GetRandomSceneWithAnyMultiActorTagForAllCSV(RE::StaticFunctionTag*, std::vector < RE::Actor*> actors, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, [matrix](Graph::Node* node) { return forAllLoop(matrix.size(), [node, matrix](int i) {return node->hasAnyActorTag(i, matrix[i]);}); });
    }

    std::string GetRandomSceneWithAllMultiActorTagsForAnyCSV(RE::StaticFunctionTag*, std::vector < RE::Actor*> actors, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, [matrix](Graph::Node* node) { return forAnyLoop(matrix.size(), [node, matrix](int i) {return node->hasAllActorTags(i, matrix[i]);}); });
    }

    std::string GetRandomSceneWithAllMultiActorTagsForAllCSV(RE::StaticFunctionTag*, std::vector < RE::Actor*> actors, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, [matrix](Graph::Node* node) { return forAllLoop(matrix.size(), [node, matrix](int i) {return node->hasAllActorTags(i, matrix[i]);}); });
    }


    std::string GetRandomFurnitureSceneWithMultiActorTagForAny(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, furnitureType, [tags](Graph::Node* node) { return forAnyLoop(tags.size(), [node, tags](int i) {return node->hasActorTag(i, tags[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithMultiActorTagForAnyCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, std::string tags) {
        return GetRandomFurnitureSceneWithMultiActorTagForAny(sft, actors, furnitureType, StringUtil::toTagVector(tags));
    }

    std::string GetRandomFurnitureSceneWithMultiActorTagForAll(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::vector<std::string> tags) {
        StringUtil::toLower(&tags);
        return randomScene(actors, furnitureType, [tags](Graph::Node* node) { return forAllLoop(tags.size(), [node, tags](int i) {return node->hasActorTag(i, tags[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithMultiActorTagForAllCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, std::string tags) {
        return GetRandomFurnitureSceneWithMultiActorTagForAll(sft, actors, furnitureType, StringUtil::toTagVector(tags));
    }

    std::string GetRandomFurnitureSceneWithAnyMultiActorTagForAnyCSV(RE::StaticFunctionTag*, std::vector <RE::Actor*> actors, std::string furnitureType, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, furnitureType, [matrix](Graph::Node* node) { return forAnyLoop(matrix.size(), [node, matrix](int i) {return node->hasAnyActorTag(i, matrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAnyMultiActorTagForAllCSV(RE::StaticFunctionTag*, std::vector <RE::Actor*> actors, std::string furnitureType, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, furnitureType, [matrix](Graph::Node* node) { return forAllLoop(matrix.size(), [node, matrix](int i) {return node->hasAnyActorTag(i, matrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAllMultiActorTagsForAnyCSV(RE::StaticFunctionTag*, std::vector <RE::Actor*> actors, std::string furnitureType, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, furnitureType, [matrix](Graph::Node* node) { return forAnyLoop(matrix.size(), [node, matrix](int i) {return node->hasAllActorTags(i, matrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAllMultiActorTagsForAllCSV(RE::StaticFunctionTag*, std::vector <RE::Actor*> actors, std::string furnitureType, std::string tags) {
        auto matrix = StringUtil::toTagMatrix(tags);
        return randomScene(actors, furnitureType, [matrix](Graph::Node* node) { return forAllLoop(matrix.size(), [node, matrix](int i) {return node->hasAllActorTags(i, matrix[i]);}); });
    }


    std::string GetRandomSceneWithAnySceneTagAndAnyMultiActorTagForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAllSceneTagsAndAnyMultiActorTagForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAnySceneTagAndAnyMultiActorTagForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAllSceneTagsAndAnyMultiActorTagForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAnySceneTagAndAllMultiActorTagsForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAllSceneTagsAndAllMultiActorTagsForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAnySceneTagAndAllMultiActorTagsForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }

    std::string GetRandomSceneWithAllSceneTagsAndAllMultiActorTagsForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }


    std::string GetRandomFurnitureSceneWithAnySceneTagAndAnyMultiActorTagForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAllSceneTagsAndAnyMultiActorTagForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAnySceneTagAndAnyMultiActorTagForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAllSceneTagsAndAnyMultiActorTagForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAnyActorTag(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAnySceneTagAndAllMultiActorTagsForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAllSceneTagsAndAllMultiActorTagsForAnyCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAnyLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAnySceneTagAndAllMultiActorTagsForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAny(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }

    std::string GetRandomFurnitureSceneWithAllSceneTagsAndAllMultiActorTagsForAllCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string sceneTags, std::string actorTags) {
        auto sceneVector = StringUtil::toTagVector(sceneTags);
        auto actorMatrix = StringUtil::toTagMatrix(actorTags);
        return randomScene(actors, furnitureType, [sceneVector, actorMatrix](Graph::Node* node) { return VectorUtil::containsAll(node->tags, sceneVector) && forAllLoop(actorMatrix.size(), [node, actorMatrix](int i){return node->hasAllActorTags(i, actorMatrix[i]);}); });
    }


    std::string GetRandomSceneWithAction(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string type) {
        StringUtil::toLower(&type);
        return randomScene(actors, [type](Graph::Node* node) { return node->findAction(type) != -1; });
    }

    std::string GetRandomSceneWithAnyAction(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, [types](Graph::Node* node) { return node->findAnyAction(types) != -1; });
    }

    std::string GetRandomSceneWithAnyActionCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string types) {
        return GetRandomSceneWithAnyAction(sft, actors, StringUtil::toTagVector(types));
    }

    std::string GetRandomSceneWithAllActions(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, [types](Graph::Node* node) { return forAllLoop(types.size(), [node, types](int i){return node->findAction(types[i]) != -1;}); });
    }

    std::string GetRandomSceneWithAllActionsCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string tags) {
        return GetRandomSceneWithAllActions(sft, actors, StringUtil::toTagVector(tags));
    }


    std::string GetRandomFurnitureSceneWithAction(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string type) {
        StringUtil::toLower(&type);
        return randomScene(actors, furnitureType, [type](Graph::Node* node) { return node->findAction(type) != -1; });
    }

    std::string GetRandomFurnitureSceneWithAnyAction(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, furnitureType, [types](Graph::Node* node) { return node->findAnyAction(types) != -1; });
    }

    std::string GetRandomFurnitureSceneWithAnyActionCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, std::string types) {
        return GetRandomFurnitureSceneWithAnyAction(sft, actors, furnitureType, StringUtil::toTagVector(types));
    }

    std::string GetRandomFurnitureSceneWithAllActions(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, furnitureType, [types](Graph::Node* node) { return forAllLoop(types.size(), [node, types](int i){return node->findAction(types[i]) != -1;}); });
    }

    std::string GetRandomFurnitureSceneWithAllActionsCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, std::string tags) {
        return GetRandomFurnitureSceneWithAllActions(sft, actors, furnitureType, StringUtil::toTagVector(tags));
    }


    std::string GetRandomSceneWithActionForActor(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::string type) {
        StringUtil::toLower(&type);
        return randomScene(actors, [position, type](Graph::Node* node) { return node->findActionForActor(position, type) != -1; });
    }

    std::string GetRandomSceneWithAnyActionForActor(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, [position, types](Graph::Node* node) { return node->findAnyActionForActor(position, types) != -1; });
    }

    std::string GetRandomSceneWithAnyActionForActorCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, int position, std::string types) {
        return GetRandomSceneWithAnyActionForActor(sft, actors, position, StringUtil::toTagVector(types));
    }

    std::string GetRandomSceneWithAllActionsForActor(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, [position, types](Graph::Node* node) { return forAllLoop(types.size(), [node, position, types](int i){return node->findActionForActor(position, types[i]) != -1;}); });
    }

    std::string GetRandomSceneWithAllActionsForActorCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, int position, std::string tags) {
        return GetRandomSceneWithAllActionsForActor(sft, actors, position, StringUtil::toTagVector(tags));
    }


    std::string GetRandomFurnitureSceneWithActionForActor(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string type) {
        StringUtil::toLower(&type);
        return randomScene(actors, furnitureType, [position, type](Graph::Node* node) { return node->findActionForActor(position, type) != -1; });
    }

    std::string GetRandomFurnitureSceneWithAnyActionForActor(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, furnitureType, [position, types](Graph::Node* node) { return node->findAnyActionForActor(position, types) != -1; });
    }

    std::string GetRandomFurnitureSceneWithAnyActionForActorCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string types) {
        return GetRandomFurnitureSceneWithAnyActionForActor(sft, actors, furnitureType, position, StringUtil::toTagVector(types));
    }

    std::string GetRandomFurnitureSceneWithAllActionsForActor(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, furnitureType, [position, types](Graph::Node* node) { return forAllLoop(types.size(), [node, position, types](int i){return node->findActionForActor(position, types[i]) != -1;}); });
    }

    std::string GetRandomFurnitureSceneWithAllActionsForActorCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string tags) {
        return GetRandomFurnitureSceneWithAllActionsForActor(sft, actors, furnitureType, position, StringUtil::toTagVector(tags));
    }


    std::string GetRandomSceneWithActionForTarget(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::string type) {
        StringUtil::toLower(&type);
        return randomScene(actors, [position, type](Graph::Node* node) { return node->findActionForTarget(position, type) != -1; });
    }

    std::string GetRandomSceneWithAnyActionForTarget(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, [position, types](Graph::Node* node) { return node->findAnyActionForTarget(position, types) != -1; });
    }

    std::string GetRandomSceneWithAnyActionForTargetCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, int position, std::string types) {
        return GetRandomSceneWithAnyActionForTarget(sft, actors, position, StringUtil::toTagVector(types));
    }

    std::string GetRandomSceneWithAllActionsForTarget(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, [position, types](Graph::Node* node) { return forAllLoop(types.size(), [node, position, types](int i){return node->findActionForTarget(position, types[i]) != -1;}); });
    }

    std::string GetRandomSceneWithAllActionsForTargetCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, int position, std::string tags) {
        return GetRandomSceneWithAllActionsForTarget(sft, actors, position, StringUtil::toTagVector(tags));
    }


    std::string GetRandomFurnitureSceneWithActionForTarget(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string type) {
        StringUtil::toLower(&type);
        return randomScene(actors, furnitureType, [position, type](Graph::Node* node) { return node->findActionForTarget(position, type) != -1; });
    }

    std::string GetRandomFurnitureSceneWithAnyActionForTarget(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, furnitureType, [position, types](Graph::Node* node) { return node->findAnyActionForTarget(position, types) != -1; });
    }

    std::string GetRandomFurnitureSceneWithAnyActionForTargetCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string types) {
        return GetRandomFurnitureSceneWithAnyActionForTarget(sft, actors, furnitureType, position, StringUtil::toTagVector(types));
    }

    std::string GetRandomFurnitureSceneWithAllActionsForTarget(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::vector<std::string> types) {
        StringUtil::toLower(&types);
        return randomScene(actors, furnitureType, [position, types](Graph::Node* node) { return forAllLoop(types.size(), [node, position, types](int i){return node->findActionForTarget(position, types[i]) != -1;}); });
    }

    std::string GetRandomFurnitureSceneWithAllActionsForTargetCSV(RE::StaticFunctionTag* sft, std::vector<RE::Actor*> actors, std::string furnitureType, int position, std::string tags) {
        return GetRandomFurnitureSceneWithAllActionsForTarget(sft, actors, furnitureType, position, StringUtil::toTagVector(tags));
    }


    std::string GetRandomSceneSuperloadCSV(RE::StaticFunctionTag*, std::vector<RE::Actor*> actors, std::string furnitureType, std::string anySceneTag, std::string allSceneTags, std::string anyActorTagForAny, std::string anyActorTagForAll, std::string allActorTagsForAny, std::string allActorTagsForAll, std::string anyActionType, std::string anyActionActor, std::string anyActionTarget, std::string anyActionPerformer, std::string anyActionMateAny, std::string anyActionMateAll, std::string anyActionParticipantAny, std::string anyActionParticipantAll, std::string allActionTypes, std::string allActionActors, std::string allActionTargets, std::string allActionPerformers, std::string allActionMatesAny, std::string allActionMatesAll, std::string allActionParticipantsAny, std::string allActionParticipantsAll) {
        std::vector<std::string> anySceneTagVector = StringUtil::toTagVector(anySceneTag);
        std::vector<std::string> allSceneTagsVector = StringUtil::toTagVector(allSceneTags);
        
        std::vector<std::vector<std::string>> anyActorTagForAnyMatrix = StringUtil::toTagMatrix(anyActorTagForAny);
        std::vector<std::vector<std::string>> anyActorTagForAllMatrix = StringUtil::toTagMatrix(anyActorTagForAll);
        std::vector<std::vector<std::string>> allActorTagsForAnyMatrix = StringUtil::toTagMatrix(allActorTagsForAny);
        std::vector<std::vector<std::string>> allActorTagsForAllMatrix = StringUtil::toTagMatrix(allActorTagsForAll);

        std::vector<std::string> anyActionTypeVector = StringUtil::toTagVector(anyActionType);
        std::vector<std::vector<int>> anyActionActorMatrix = VectorUtil::stoim(anyActionActor);
        std::vector<std::vector<int>> anyActionTargetMatrix = VectorUtil::stoim(anyActionTarget);
        std::vector<std::vector<int>> anyActionPerformerMatrix = VectorUtil::stoim(anyActionPerformer);
        std::vector<std::vector<int>> anyActionMateAnyMatrix = VectorUtil::stoim(anyActionMateAny);
        std::vector<std::vector<int>> anyActionMateAllMatrix = VectorUtil::stoim(anyActionMateAll);
        std::vector<std::vector<int>> anyActionParticipantAnyMatrix = VectorUtil::stoim(anyActionParticipantAny);
        std::vector<std::vector<int>> anyActionParticipantAllMatrix = VectorUtil::stoim(anyActionParticipantAll);

        std::vector<std::string> allActionTypesVector = StringUtil::toTagVector(allActionTypes);
        std::vector<std::vector<int>> allActionActorsMatrix = VectorUtil::stoim(allActionActors);
        std::vector<std::vector<int>> allActionTargetsMatrix = VectorUtil::stoim(allActionTargets);
        std::vector<std::vector<int>> allActionPerformersMatrix = VectorUtil::stoim(allActionPerformers);
        std::vector<std::vector<int>> allActionMatesAnyMatrix = VectorUtil::stoim(allActionMatesAny);
        std::vector<std::vector<int>> allActionMatesAllMatrix = VectorUtil::stoim(allActionMatesAll);
        std::vector<std::vector<int>> allActionParticipantsAnyMatrix = VectorUtil::stoim(allActionParticipantsAny);
        std::vector<std::vector<int>> allActionParticipantsAllMatrix = VectorUtil::stoim(allActionParticipantsAll);

        return randomScene(actors, furnitureType, [anySceneTagVector, allSceneTagsVector, anyActorTagForAnyMatrix, anyActorTagForAllMatrix, allActorTagsForAnyMatrix, allActorTagsForAllMatrix, anyActionTypeVector, anyActionActorMatrix, anyActionTargetMatrix, anyActionPerformerMatrix, anyActionMateAnyMatrix, anyActionMateAllMatrix, anyActionParticipantAnyMatrix, anyActionParticipantAllMatrix, allActionTypesVector, allActionActorsMatrix, allActionTargetsMatrix, allActionPerformersMatrix, allActionMatesAnyMatrix, allActionMatesAllMatrix, allActionParticipantsAnyMatrix, allActionParticipantsAllMatrix](Graph::Node* node) {
                return (anySceneTagVector.empty() || VectorUtil::containsAny(node->tags, anySceneTagVector)) &&
                    (allSceneTagsVector.empty() || VectorUtil::containsAll(node->tags, allSceneTagsVector)) &&
                    (anyActorTagForAnyMatrix.empty() || forAnyLoop(anyActorTagForAnyMatrix.size(), [node, anyActorTagForAnyMatrix](int i){return node->hasAnyActorTag(i, anyActorTagForAnyMatrix[i]);})) &&
                    (anyActorTagForAllMatrix.empty() || forAllLoop(anyActorTagForAllMatrix.size(), [node, anyActorTagForAllMatrix](int i){return node->hasAnyActorTag(i, anyActorTagForAllMatrix[i]);})) &&
                    (allActorTagsForAnyMatrix.empty() || forAnyLoop(allActorTagsForAnyMatrix.size(), [node, allActorTagsForAnyMatrix](int i){return node->hasAllActorTags(i, allActorTagsForAnyMatrix[i]);})) &&
                    (allActorTagsForAllMatrix.empty() || forAllLoop(allActorTagsForAllMatrix.size(), [node, allActorTagsForAllMatrix](int i){return node->hasAllActorTags(i, allActorTagsForAllMatrix[i]);})) &&
                    (anyActionTypeVector.empty() || hasAnyAction(node, anyActionTypeVector, anyActionActorMatrix, anyActionTargetMatrix, anyActionPerformerMatrix, anyActionMateAnyMatrix, anyActionMateAllMatrix, anyActionParticipantAnyMatrix, anyActionParticipantAllMatrix)) &&
                    (allActionTypesVector.empty() || hasAllActions(node, allActionTypesVector, allActionActorsMatrix, allActionTargetsMatrix, allActionPerformersMatrix, allActionMatesAnyMatrix, allActionMatesAllMatrix, allActionParticipantsAnyMatrix, allActionParticipantsAllMatrix));
            });
    }

    bool Bind(VM* a_vm) {
        const auto obj = "OLibrary"sv;

        BIND(GetRandomScene);
        BIND(GetRandomFurnitureScene);

        BIND(GetRandomSceneWithTag);
        BIND(GetRandomSceneWithAnyTag);
        BIND(GetRandomSceneWithAnyTagCSV);
        BIND(GetRandomSceneWithAllTags);
        BIND(GetRandomSceneWithAllTagsCSV);

        BIND(GetRandomFurnitureSceneWithTag);
        BIND(GetRandomFurnitureSceneWithAnyTag);
        BIND(GetRandomFurnitureSceneWithAnyTagCSV);
        BIND(GetRandomFurnitureSceneWithAllTags);
        BIND(GetRandomFurnitureSceneWithAllTagsCSV);

        BIND(GetRandomSceneWithSingleActorTag);
        BIND(GetRandomSceneWithAnySingleActorTag);
        BIND(GetRandomSceneWithAnySingleActorTagCSV);
        BIND(GetRandomSceneWithAllSingleActorTags);
        BIND(GetRandomSceneWithAllSingleActorTagsCSV);

        BIND(GetRandomFurnitureSceneWithSingleActorTag);
        BIND(GetRandomFurnitureSceneWithAnySingleActorTag);
        BIND(GetRandomFurnitureSceneWithAnySingleActorTagCSV);
        BIND(GetRandomFurnitureSceneWithAllSingleActorTags);
        BIND(GetRandomFurnitureSceneWithAllSingleActorTagsCSV);

        BIND(GetRandomSceneWithMultiActorTagForAny);
        BIND(GetRandomSceneWithMultiActorTagForAnyCSV);
        BIND(GetRandomSceneWithMultiActorTagForAll);
        BIND(GetRandomSceneWithMultiActorTagForAllCSV);
        BIND(GetRandomSceneWithAnyMultiActorTagForAnyCSV);
        BIND(GetRandomSceneWithAnyMultiActorTagForAllCSV);
        BIND(GetRandomSceneWithAllMultiActorTagsForAnyCSV);
        BIND(GetRandomSceneWithAllMultiActorTagsForAllCSV);

        BIND(GetRandomFurnitureSceneWithMultiActorTagForAny);
        BIND(GetRandomFurnitureSceneWithMultiActorTagForAnyCSV);
        BIND(GetRandomFurnitureSceneWithMultiActorTagForAll);
        BIND(GetRandomFurnitureSceneWithMultiActorTagForAllCSV);
        BIND(GetRandomFurnitureSceneWithAnyMultiActorTagForAnyCSV);
        BIND(GetRandomFurnitureSceneWithAnyMultiActorTagForAllCSV);
        BIND(GetRandomFurnitureSceneWithAllMultiActorTagsForAnyCSV);
        BIND(GetRandomFurnitureSceneWithAllMultiActorTagsForAllCSV);

        BIND(GetRandomSceneWithAnySceneTagAndAnyMultiActorTagForAnyCSV);
        BIND(GetRandomSceneWithAllSceneTagsAndAnyMultiActorTagForAnyCSV);
        BIND(GetRandomSceneWithAnySceneTagAndAnyMultiActorTagForAllCSV);
        BIND(GetRandomSceneWithAllSceneTagsAndAnyMultiActorTagForAllCSV);
        BIND(GetRandomSceneWithAnySceneTagAndAllMultiActorTagsForAnyCSV);
        BIND(GetRandomSceneWithAllSceneTagsAndAllMultiActorTagsForAnyCSV);
        BIND(GetRandomSceneWithAnySceneTagAndAllMultiActorTagsForAllCSV);
        BIND(GetRandomSceneWithAllSceneTagsAndAllMultiActorTagsForAllCSV);

        BIND(GetRandomFurnitureSceneWithAnySceneTagAndAnyMultiActorTagForAnyCSV);
        BIND(GetRandomFurnitureSceneWithAllSceneTagsAndAnyMultiActorTagForAnyCSV);
        BIND(GetRandomFurnitureSceneWithAnySceneTagAndAnyMultiActorTagForAllCSV);
        BIND(GetRandomFurnitureSceneWithAllSceneTagsAndAnyMultiActorTagForAllCSV);
        BIND(GetRandomFurnitureSceneWithAnySceneTagAndAllMultiActorTagsForAnyCSV);
        BIND(GetRandomFurnitureSceneWithAllSceneTagsAndAllMultiActorTagsForAnyCSV);
        BIND(GetRandomFurnitureSceneWithAnySceneTagAndAllMultiActorTagsForAllCSV);
        BIND(GetRandomFurnitureSceneWithAllSceneTagsAndAllMultiActorTagsForAllCSV);

        BIND(GetRandomSceneWithAction);
        BIND(GetRandomSceneWithAnyAction);
        BIND(GetRandomSceneWithAnyActionCSV);
        BIND(GetRandomSceneWithAllActions);
        BIND(GetRandomSceneWithAllActionsCSV);

        BIND(GetRandomFurnitureSceneWithAction);
        BIND(GetRandomFurnitureSceneWithAnyAction);
        BIND(GetRandomFurnitureSceneWithAnyActionCSV);
        BIND(GetRandomFurnitureSceneWithAllActions);
        BIND(GetRandomFurnitureSceneWithAllActionsCSV);

        BIND(GetRandomSceneWithActionForActor);
        BIND(GetRandomSceneWithAnyActionForActor);
        BIND(GetRandomSceneWithAnyActionForActorCSV);
        BIND(GetRandomSceneWithAllActionsForActor);
        BIND(GetRandomSceneWithAllActionsForActorCSV);

        BIND(GetRandomFurnitureSceneWithActionForActor);
        BIND(GetRandomFurnitureSceneWithAnyActionForActor);
        BIND(GetRandomFurnitureSceneWithAnyActionForActorCSV);
        BIND(GetRandomFurnitureSceneWithAllActionsForActor);
        BIND(GetRandomFurnitureSceneWithAllActionsForActorCSV);

        BIND(GetRandomSceneWithActionForTarget);
        BIND(GetRandomSceneWithAnyActionForTarget);
        BIND(GetRandomSceneWithAnyActionForTargetCSV);
        BIND(GetRandomSceneWithAllActionsForTarget);
        BIND(GetRandomSceneWithAllActionsForTargetCSV);

        BIND(GetRandomFurnitureSceneWithActionForTarget);
        BIND(GetRandomFurnitureSceneWithAnyActionForTarget);
        BIND(GetRandomFurnitureSceneWithAnyActionForTargetCSV);
        BIND(GetRandomFurnitureSceneWithAllActionsForTarget);
        BIND(GetRandomFurnitureSceneWithAllActionsForTargetCSV);

        BIND(GetRandomSceneSuperloadCSV);

        return true;
    }
}