#pragma once

#include "Graph/Node.h"
#include "SKEE.h"

namespace Graph{
    class LookupTable {
    public:
        static void AddNode(Node* node);
        static Node* GetNodeById(std::string id);
        static Node* GetNodeByAnimation(std::string anim);

        static bool SetNiTransfromInterface(SKEE::INiTransformInterface* nioInterface);
        static SKEE::INiTransformInterface* GetNiTransformInterface();

        static void SetupActions();
        static ActionAttributes* GetActionAttributesByType(std::string type);
    private:
        inline static std::unordered_map<std::string, Node*> nodes;
        inline static std::unordered_map<std::string, Node*> animationNodeTable;
        inline static std::unordered_map<std::string, ActionAttributes> actions;
        
        inline static SKEE::INiTransformInterface* niTransformInterface;
    };
}