#pragma once
#include "Graph/Node.h"
#include "ThreadActor.h"
#include <shared_mutex>

namespace OStim {

    using ThreadId = int64_t;
    class Thread {
    public:
        Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors);
        
        void ChangeNode(Graph::Node* a_node);

        void AddThirdActor(RE::Actor* a_actor);

        void RemoveThirdActor();

        void CalculateExcitement();

        ThreadActor* GetActor(RE::Actor* a_actor);

        void SetSpeed(int speed) { m_currentNodeSpeed = speed; }

    private:
        ThreadId m_threadId;        
        std::map<int32_t, ThreadActor> m_actors;
        std::shared_mutex nodeLock;
        Graph::Node* m_currentNode = nullptr;
        int m_currentNodeSpeed = 0;
        std::thread m_excitementThread;
    };

}  // namespace OStim