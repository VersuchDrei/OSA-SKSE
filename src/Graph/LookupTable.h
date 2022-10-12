#include "Graph/Node.h"

namespace Graph{
    class LookupTable {
    public:
        static void AddNode(Node* node) {
            nodes.insert({node->scene_id, node});
            for (std::string anim : node->anim_ids) {
                animationNodeTable.insert({anim, node});
            }
        }

        static Node* GetNodeById(std::string id) {
            auto iter = nodes.find(id);
            if (iter != nodes.end()) {
                return iter->second;
            }
            return nullptr;
        }

        static Node* GetNodeByAnimation(std::string anim) {
            auto iter = animationNodeTable.find(anim);
            if (iter != animationNodeTable.end()) {
                return iter->second;
            }
            return nullptr;
        }
    private:
        inline static std::unordered_map<std::string, Node*> nodes;
        inline static std::unordered_map<std::string, Node*> animationNodeTable;
    };
}