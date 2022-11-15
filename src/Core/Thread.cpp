#include <Core/Thread.h>
#include <Graph/Node.h>
#include <Messaging/IMessages.h>

namespace OStim {
    Thread::Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
        m_threadId = a_id;
        for (int i = 0; i < a_actors.size(); i++) {
            m_actors.insert(std::make_pair(i, ThreadActor(a_actors[i])));
        }
    }

    void Thread::ChangeNode(Graph::Node* a_node)
    {
        std::unique_lock<std::shared_mutex> writeLock;
        m_currentNode = a_node;

        for (auto& actorIt : m_actors) {
            float excitementInc = 0;
            for (auto& action : m_currentNode->actions) {
                if (action->actor == actorIt.first) {
                    excitementInc += action->attributes->actor.stimulation;
                }
                if (action->target == actorIt.first) {
                    excitementInc += action->attributes->target.stimulation;
                }
                if (action->performer == actorIt.first) {
                    excitementInc += action->attributes->performer.stimulation;
                }
            }
            actorIt.second.nodeExcitementTick = excitementInc;
        }

        auto messaging = SKSE::GetMessagingInterface();


        Messaging::AnimationChangedMessage msg;
        msg.newAnimation = a_node;        
        logger::info("Sending animation changed event");
        Messaging::MessagingRegistry::GetSingleton()->SendMessageToListeners(msg);        
    }

    void Thread::AddThirdActor(RE::Actor* a_actor) { m_actors.insert(std::make_pair(2, ThreadActor(a_actor))); }

    void Thread::RemoveThirdActor() { m_actors.erase(2); }

    void Thread::CalculateExcitement()
    {
        std::shared_lock<std::shared_mutex> readLock;
        //TODO: Can remove this when we start scenes in c++ with a starting node
        if (!m_currentNode) return;

        for (auto& actorIt : m_actors) {            
            //TODO: Clamping
            actorIt.second.excitement += actorIt.second.nodeExcitementTick;
            logger::info("{} excitement {}", actorIt.second.getActor()->GetDisplayFullName(), actorIt.second.excitement);
        }
    }

    ThreadActor* Thread::GetActor(RE::Actor* a_actor)
    {
        for (auto &i : m_actors) {
            if (i.second.getActor() == a_actor)
                return &i.second;
        }
        return nullptr;
    }

}  // namespace OStim