#pragma once
#include "Graph/Node.h"
#include "ThreadActor.h"
#include <shared_mutex>

namespace OStim {

    using ThreadId = int64_t;
    class Thread : public RE::BSTEventSink<RE::BSAnimationGraphEvent>{
    public:
        Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors, RE::TESObjectREFR* a_stageObject);

        ~Thread();
        
        void ChangeNode(Graph::Node* a_node);

        void AddThirdActor(RE::Actor* a_actor);

        void RemoveThirdActor();

        void CalculateExcitement();

        ThreadActor* GetActor(RE::Actor* a_actor);

        ThreadActor* GetActor(int a_position);

        void SetSpeed(int speed) { m_currentNodeSpeed = speed; }

        RE::TESObjectREFR* GetStageObject() { return stageObject; }

        std::vector<RE::Actor*> GetTESActors() { 
            std::vector<RE::Actor*> actors;
            for (auto& pair : m_actors) {                
                actors.push_back(pair.second.getActor());
            }
            return actors;
        }

    public:
        RE::TESObjectREFR* stageObject;

        virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

    private:
        ThreadId m_threadId;        
        std::map<int32_t, ThreadActor> m_actors;
        std::shared_mutex nodeLock;
        Graph::Node* m_currentNode = nullptr;
        int m_currentNodeSpeed = 0;        
        std::thread m_excitementThread;
        bool isPlayerThread = false;
        void addActorSink(RE::Actor* a_actor);
        void removeActorSink(RE::Actor* a_actor);
    };

}  // namespace OStim