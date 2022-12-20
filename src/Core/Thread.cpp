#include <Core/Thread.h>
#include <Graph/Node.h>
#include <Messaging/IMessages.h>
#include <Util/MCMTable.h>
#include <UI/Align/AlignMenu.h>

namespace OStim {
    Thread::Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
        m_threadId = a_id;
        for (int i = 0; i < a_actors.size(); i++) {
            m_actors.insert(std::make_pair(i, ThreadActor(a_actors[i])));
            if (a_actors[i]->IsPlayer() || a_actors[i]->IsPlayerRef()) {
                isPlayerThread = true;
            }
        }
    }

    void Thread::ChangeNode(Graph::Node* a_node) {
        std::unique_lock<std::shared_mutex> writeLock;
        m_currentNode = a_node;
        for (auto& actorIt : m_actors) {
            float excitementInc = 0;
            actorIt.second.maxExcitement = 0;
            std::vector<float> excitementVals;
            for (auto& action : m_currentNode->actions) {
                if (action->actor == actorIt.first) {
                    excitementVals.push_back(action->attributes->actor.stimulation);
                    auto maxStim = action->attributes->actor.maxStimulation;
                    if (maxStim > actorIt.second.maxExcitement) {
                        actorIt.second.maxExcitement = maxStim;
                    }
                }
                if (action->target == actorIt.first) {
                    excitementVals.push_back(action->attributes->target.stimulation);
                    auto maxStim = action->attributes->target.maxStimulation;
                    if (maxStim > actorIt.second.maxExcitement) {
                        actorIt.second.maxExcitement = maxStim;
                    }
                }
                if (action->performer == actorIt.first) {
                    excitementVals.push_back(action->attributes->performer.stimulation);
                }
            }

            if (actorIt.second.getActor()->GetActorBase()->GetSex() == RE::SEX::kMale) {
                actorIt.second.baseExcitementMultiplier = MCM::MCMTable::getMaleSexExcitementMult();
            } else {
                actorIt.second.baseExcitementMultiplier = MCM::MCMTable::getFemaleSexExcitementMult();
            }

            switch (excitementVals.size()) {
                case 0:
                    excitementInc = 0;
                    break;
                case 1:
                    excitementInc = excitementVals[0];
                    break;
                default: {
                    std::sort(excitementVals.begin(), excitementVals.end(), std::greater<float>());
                    excitementInc = excitementVals[0];
                    for (int i = 1; i < excitementVals.size(); i++) {
                        excitementInc += excitementVals[i] * 0.1;
                    }
                } break;
            }

            actorIt.second.nodeExcitementTick = excitementInc;
        }

        auto messaging = SKSE::GetMessagingInterface();

        Messaging::AnimationChangedMessage msg;
        msg.newAnimation = a_node;
        logger::info("Sending animation changed event");
        Messaging::MessagingRegistry::GetSingleton()->SendMessageToListeners(msg);

        if (isPlayerThread) {
            UI::Align::AlignMenu::SetThread(this);
            UI::Align::AlignMenu::SetNode(a_node);
            //TODO: Remove this
            UI::Align::AlignMenu::SetActor(this->GetTESActors()[0]);
        }        
    }

    void Thread::AddThirdActor(RE::Actor* a_actor) { m_actors.insert(std::make_pair(2, ThreadActor(a_actor))); }

    void Thread::RemoveThirdActor() { m_actors.erase(2); }

    void Thread::CalculateExcitement() {
        std::shared_lock<std::shared_mutex> readLock;
        // TODO: Can remove this when we start scenes in c++ with a starting node
        if (!m_currentNode) return;

        for (auto& actorIt : m_actors) {
            auto speedMod =
                (m_currentNodeSpeed - ceil((((m_currentNode->maxspeed - m_currentNode->minspeed) + 1) / 2))) * 0.2;
            auto& actorRef = actorIt.second;
            auto excitementInc = (actorIt.second.nodeExcitementTick + speedMod);
            auto finalExcitementInc = actorRef.baseExcitementMultiplier * excitementInc;
            if (finalExcitementInc > 0) {
                if (actorRef.excitement > actorRef.maxExcitement) {  // Decay from previous scene with higher max
                    auto excitementDecay = 0.5;
                    if (actorRef.excitement - excitementDecay < actorRef.maxExcitement) {
                        actorRef.excitement = actorRef.maxExcitement;
                    } else {
                        actorRef.excitement -= excitementDecay;
                    }

                } else { // increase excitement
                    if (finalExcitementInc + actorRef.excitement > actorRef.maxExcitement) {                          
                        actorRef.excitement = actorRef.maxExcitement;
                    } else {
                        actorRef.excitement += finalExcitementInc;
                    }
                }
            }
        }
    }

    ThreadActor* Thread::GetActor(RE::Actor* a_actor) {
        for (auto& i : m_actors) {
            if (i.second.getActor() == a_actor) return &i.second;
        }
        return nullptr;
    }

}  // namespace OStim