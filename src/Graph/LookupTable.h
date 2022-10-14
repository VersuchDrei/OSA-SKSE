#pragma once

#include "Graph/Node.h"

namespace Graph{
    class LookupTable {
    public:
        static void AddNode(Node* node);
        static Node* GetNodeById(std::string id);
        static Node* GetNodeByAnimation(std::string anim);
    private:
        inline static std::unordered_map<std::string, Node*> nodes;
        inline static std::unordered_map<std::string, Node*> animationNodeTable;
    };
}