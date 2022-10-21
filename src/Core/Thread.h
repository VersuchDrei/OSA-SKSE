#pragma once
#include "Graph/Node.h"

namespace OStim {

    using ThreadId = int64_t;
    class Thread {
    public:
        Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors);

        void ChangeNode(Graph::Node* a_node);

        void AddThirdActor(RE::Actor* a_actor);

        void RemoveThirdActor();

    private:
        ThreadId m_threadId;
        std::map<int32_t, RE::Actor*> m_actors;
        Graph::Node* m_currentNode;
    };

}  // namespace OStim