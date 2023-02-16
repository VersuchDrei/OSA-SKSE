#pragma once

#include <shared_mutex>

#include "ThreadActor.h"

#include "Graph/Node.h"
#include "Serial/OldThread.h"

namespace OStim {

    using ThreadId = int64_t;
    class Thread : public RE::BSTEventSink<RE::BSAnimationGraphEvent>{
    public:
        Thread(ThreadId id, RE::TESObjectREFR* furniture, std::vector<RE::Actor*> actors);

        ~Thread();
        
        void ChangeNode(Graph::Node* a_node);
        Graph::Node* getCurrentNode();

        void AddActor(RE::Actor* a_actor);
        void RemoveActor();

        void loop();

        ThreadActor* GetActor(RE::Actor* a_actor);
        ThreadActor* GetActor(int a_position);
        int getActorPosition(RE::Actor* actor);

        void SetSpeed(int speed);

        void close();

        virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

        Serialization::OldThread serialize();

    private:
        ThreadId m_threadId;        
        RE::TESObjectREFR* furniture;
        bool furnitureLocked = false;
        RE::TESObjectREFR* vehicle;
        std::map<int32_t, ThreadActor> m_actors;
        std::shared_mutex nodeLock;
        Graph::Node* m_currentNode = nullptr;
        int m_currentNodeSpeed = 0;        
        std::thread m_excitementThread;

        void addActorSink(RE::Actor* a_actor);
        void removeActorSink(RE::Actor* a_actor);
    };

}  // namespace OStim