#pragma once
#include "Graph/Node.h"
#include "ThreadActor.h"
#include <shared_mutex>

namespace OStim {

    using ThreadId = int64_t;
    class Thread : public RE::BSTEventSink<RE::BSAnimationGraphEvent>{
    public:
        Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors);

        ~Thread();
        
        void ChangeNode(Graph::Node* a_node);
        Graph::Node* getCurrentNode();

        void AddThirdActor(RE::Actor* a_actor);
        void RemoveThirdActor();

        void loop();

        ThreadActor* GetActor(RE::Actor* a_actor);
        ThreadActor* GetActor(int a_position);
        int getActorPosition(RE::Actor* actor);

        void SetSpeed(int speed);

        void free();

        virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

    private:
        ThreadId m_threadId;        
        std::map<int32_t, ThreadActor> m_actors;
        std::shared_mutex nodeLock;
        Graph::Node* m_currentNode = nullptr;
        int m_currentNodeSpeed = 0;        
        std::thread m_excitementThread;

        void addActorSink(RE::Actor* a_actor);
        void removeActorSink(RE::Actor* a_actor);
    };

}  // namespace OStim