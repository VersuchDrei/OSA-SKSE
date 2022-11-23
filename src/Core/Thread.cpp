#include <Core/Thread.h>
#include <Graph/Node.h>
#include <Messaging/IMessages.h>
#include <Util/MCMTable.h>

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
            std::vector<float> excitementVals;
            for (auto& action : m_currentNode->actions) {
                if (action->actor == actorIt.first) {
                    excitementVals.push_back(action->attributes->actor.stimulation);                    
                }
                if (action->target == actorIt.first) {
                    excitementVals.push_back(action->attributes->target.stimulation);                    
                }
                if (action->performer == actorIt.first) {
                    excitementVals.push_back(action->attributes->performer.stimulation);                    
                }
            }
            std::sort(excitementVals.begin(), excitementVals.end(), std::greater<float>());
            for (int i = 0; i < 4 || i < excitementVals.size(); i++) {
                excitementInc += ((5 - i) * 0.2 * excitementVals[i]);
            }

            if (actorIt.second.getActor()->GetActorBase()->GetSex() == RE::SEX::kMale) {
                actorIt.second.baseExcitementMultiplier = MCM::MCMTable::getMaleSexExcitementMult();
            }
            else {
                actorIt.second.baseExcitementMultiplier = MCM::MCMTable::getFemaleSexExcitementMult();
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
            auto speedMod = (m_currentNodeSpeed - ceil((((m_currentNode->maxspeed - m_currentNode->minspeed) + 1) / 2))) * 0.2;
            auto excitementInc = (actorIt.second.nodeExcitementTick + speedMod);
            if (excitementInc > 0) {
                logger::info("Adding {} base {} speed {}", excitementInc, actorIt.second.nodeExcitementTick, speedMod);
                actorIt.second.excitement += actorIt.second.baseExcitementMultiplier * excitementInc;
            }
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