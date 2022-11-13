#pragma once

#include "Graph/Node.h"
#include "Trait/Condition.h"
#include "SKEE.h"

namespace Graph{
    class LookupTable {
    public:
        static FurnitureType getFurnitureType(std::string type);

        static void addNode(Node* node);
        static Node* getNodeById(std::string id);
        static Node* getNodeByAnimation(std::string anim);

        static Node* getRandomNode(FurnitureType furnitureType, std::vector<Trait::ActorConditions> actorConditions, std::function<bool(Node*)> nodeCondition);

        static bool setNiTransfromInterface(SKEE::INiTransformInterface* nioInterface);
        static SKEE::INiTransformInterface* getNiTransformInterface();
    private:
        inline static std::unordered_map<std::string, FurnitureType> furnitures{
            {"", FurnitureType::NONE},
            {"chair", FurnitureType::CHAIR},
            {"bench", FurnitureType::BENCH},
            {"shelf", FurnitureType::SHELF},
            {"craftingtable", FurnitureType::CRAFTING_TABLE},
            {"cookingpot", FurnitureType::COOKING_POT}
        };

        inline static std::unordered_map<std::string, Node*> nodes;
        inline static std::unordered_map<std::string, Node*> animationNodeTable;
        inline static std::unordered_map<FurnitureType, std::unordered_map<int, std::vector<Node*>*>*> nodeList;
        
        inline static auto rng = std::default_random_engine{};

        inline static SKEE::INiTransformInterface* niTransformInterface;
    };
}