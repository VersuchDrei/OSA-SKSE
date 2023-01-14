#pragma once

#include "Furniture/Furniture.h"
#include "Graph/Node.h"
#include "Trait/Condition.h"
#include "SKEE.h"

namespace Graph{
    class LookupTable {
    public:
        inline static RE::TESQuest* OSexIntegrationMainQuest;
        inline static RE::BGSKeyword* OStimNoStrip;

        inline static std::vector<RE::BGSKeyword*> noStripKeywords;

        inline static auto rng = std::default_random_engine{};

        static void setupForms();

        static void addNode(Node* node);
        static Node* getNodeById(std::string id);
        static Node* getNodeByAnimation(std::string anim);

        static bool hasNodes(Furniture::FurnitureType furnitureType, int actorCount);
        static Node* getRandomNode(Furniture::FurnitureType furnitureType, std::vector<Trait::ActorConditions> actorConditions, std::function<bool(Node*)> nodeCondition);

        static bool setNiTransfromInterface(SKEE::INiTransformInterface* nioInterface);
        static SKEE::INiTransformInterface* getNiTransformInterface();

        static void SetupActions();
        static ActionAttributes* GetActionAttributesByType(std::string type);
    private:

        inline static std::unordered_map<std::string, Node*> nodes;
        inline static std::unordered_map<std::string, Node*> animationNodeTable;
        inline static std::unordered_map<Furniture::FurnitureType, std::unordered_map<int, std::vector<Node*>*>*> nodeList;
        inline static std::unordered_map<std::string, ActionAttributes> actions;

        inline static SKEE::INiTransformInterface* niTransformInterface;
    };
}