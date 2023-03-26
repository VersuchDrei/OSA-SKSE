#pragma once

#include <shared_mutex>

#include "ThreadActor.h"

#include "Alignment/ThreadKey.h"
#include "Graph/Node.h"
#include "Serial/OldThread.h"

namespace OStim {

    using ThreadId = int64_t;
    class Thread : public RE::BSTEventSink<RE::BSAnimationGraphEvent>{
    public:
        Thread(ThreadId id, RE::TESObjectREFR* furniture, std::vector<RE::Actor*> actors);

        ~Thread();

        void initContinue();

        Alignment::ThreadKey getAlignmentKey();
        
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

        RE::TESObjectREFR* GetStageObject() { return vehicle; }

        std::vector<RE::Actor*> GetTESActors() { 
            std::vector<RE::Actor*> actors;
            for (auto& pair : m_actors) {                
                actors.push_back(pair.second.getActor());
            }
            return actors;
        }

        void alignActor(RE::Actor* actor, float x, float y, float z, float rotation);

    public:
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

        Serialization::OldThread serialize();

    private:
        ThreadId m_threadId;        
        bool isPlayerThread = false;
        RE::TESObjectREFR* furniture;
        bool furnitureLocked = false;
        RE::TESObjectREFR* vehicle;
        std::map<int32_t, ThreadActor> m_actors;
        std::shared_mutex nodeLock;
        Graph::Node* m_currentNode = nullptr;
        int m_currentNodeSpeed = 0;

        float freeCamSpeedBefore = 0;
        float worldFOVbefore = 0;

        void addActorInner(int index, RE::Actor* actor);
        void addActorSink(RE::Actor* a_actor);
        void removeActorSink(RE::Actor* a_actor);
    };

}  // namespace OStim