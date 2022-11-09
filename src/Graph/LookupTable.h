#pragma once

#include "Graph/Node.h"
#include "SKEE.h"

namespace Graph{
    class LookupTable {
    public:
        static void addNode(Node* node);
        static Node* getNodeById(std::string id);
        static Node* getNodeByAnimation(std::string anim);

        static Node* getRandomNode(std::function<bool(Node)> condition);

        static bool setNiTransfromInterface(SKEE::INiTransformInterface* nioInterface);
        static SKEE::INiTransformInterface* getNiTransformInterface();
    private:
        inline static std::unordered_map<std::string, Node*> nodes;
        inline static std::unordered_map<std::string, Node*> animationNodeTable;
        inline static std::vector<Node*> nodeList;
        
        inline static auto rng = std::default_random_engine{};

        inline static SKEE::INiTransformInterface* niTransformInterface;
    };
}