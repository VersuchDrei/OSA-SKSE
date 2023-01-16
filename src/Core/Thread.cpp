#include <Core/Thread.h>
#include <Graph/LookupTable.h>
#include <Graph/Node.h>
#include <Messaging/IMessages.h>
#include <Util/MCMTable.h>
#include <Util/StringUtil.h>

namespace OStim {
    Thread::Thread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
        m_threadId = a_id;
        for (int i = 0; i < a_actors.size(); i++) {
            addActorSink(a_actors[i]);
            m_actors.insert(std::make_pair(i, ThreadActor(a_id, a_actors[i])));
            ThreadActor* actor = GetActor(i);
            actor->initContinue();
            if (MCM::MCMTable::undressAtStart()) {
                actor->undress();
            }
            if (MCM::MCMTable::removeWeaponsAtStart()) {
                actor->removeWeapons();
            }
        }
    }

    Thread::~Thread() {
        for (auto& actorIt : m_actors) {
            removeActorSink(actorIt.second.getActor());
        }
    }

    void Thread::ChangeNode(Graph::Node* a_node) {
        std::unique_lock<std::shared_mutex> writeLock;
        m_currentNode = a_node;

        for (auto& actorIt : m_actors) {
            // --- excitement calculation --- //
            float excitementInc = 0;
            actorIt.second.maxExcitement = 0;
            std::vector<float> excitementVals;
            for (auto& action : m_currentNode->actions) {
                if (action->actor == actorIt.first && action->attributes->actor.stimulation != 0) {
                    excitementVals.push_back(action->attributes->actor.stimulation);
                    auto maxStim = action->attributes->actor.maxStimulation;
                    if (maxStim > actorIt.second.maxExcitement) {
                        actorIt.second.maxExcitement = maxStim;
                    }
                }
                if (action->target == actorIt.first && action->attributes->target.stimulation != 0) {
                    excitementVals.push_back(action->attributes->target.stimulation);
                    auto maxStim = action->attributes->target.maxStimulation;
                    if (maxStim > actorIt.second.maxExcitement) {
                        actorIt.second.maxExcitement = maxStim;
                    }
                }
                if (action->performer == actorIt.first && action->attributes->performer.stimulation != 0) {
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


            // --- undressing --- //
            if (MCM::MCMTable::undressMidScene() && m_currentNode->hasActionTag("sexual")) {
                actorIt.second.undress();
                actorIt.second.removeWeapons();
                // it is intended that the else fires if undressMidScene is checked but the action is not tagged as sexual
                // because some non sexual actions still have slots for partial stripping
                // for example kissing undresses helmets without being sexual
            } else if (MCM::MCMTable::partialUndressing()) {
                uint32_t slotMask = 0;
                for (auto& action : m_currentNode->actions) {
                    slotMask |= action->getStrippingMask(actorIt.first);
                }
                if (slotMask != 0) {
                    actorIt.second.undressPartial(slotMask);
                    if ((slotMask & MCM::MCMTable::removeWeaponsWithSlot()) != 0) {
                        actorIt.second.removeWeapons();
                    }
                }
            }

            // --- scaling / heel offsets --- //
            if (actorIt.first < m_currentNode->actors.size()) {
                actorIt.second.changeNode(m_currentNode->actors[actorIt.first]);
            }
        }

        auto messaging = SKSE::GetMessagingInterface();

        Messaging::AnimationChangedMessage msg;
        msg.newAnimation = a_node;
        logger::info("Sending animation changed event");
        Messaging::MessagingRegistry::GetSingleton()->SendMessageToListeners(msg);
    }

    Graph::Node* Thread::getCurrentNode() {
        return m_currentNode;
    }

    void Thread::AddThirdActor(RE::Actor* a_actor) {
        addActorSink(a_actor);
        m_actors.insert(std::make_pair(2, ThreadActor(m_threadId, a_actor)));
        ThreadActor* actor = GetActor(2);
        actor->initContinue();
        if (MCM::MCMTable::undressAtStart()) {
            actor->undress();
        }
        if (MCM::MCMTable::removeWeaponsAtStart()) {
            actor->removeWeapons();
        }
    }

    void Thread::RemoveThirdActor() {
        ThreadActor* actor = GetActor(2);
        removeActorSink(actor->getActor());
        actor->free();

        m_actors.erase(2);
    }

    void Thread::CalculateExcitement() {
        std::shared_lock<std::shared_mutex> readLock;
        // TODO: Can remove this when we start scenes in c++ with a starting node
        if (!m_currentNode) {
            return;
        }

        for (auto& actorIt : m_actors) {
            auto speedMod = (m_currentNodeSpeed - ceil((((m_currentNode->maxspeed - m_currentNode->minspeed) + 1) / 2))) * 0.2;
            auto& actorRef = actorIt.second;
            auto excitementInc = (actorIt.second.nodeExcitementTick + speedMod);
            auto finalExcitementInc = actorRef.baseExcitementMultiplier * excitementInc;
            if (finalExcitementInc <= 0) {  // Decay from previous scene with higher max
                auto excitementDecay = 0.5;
                if (actorRef.excitement - excitementDecay < 0) {
                    actorRef.excitement = 0;
                } else {
                    actorRef.excitement -= excitementDecay;
                }

            } else if (actorRef.excitement > actorRef.maxExcitement) {
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

    ThreadActor* Thread::GetActor(RE::Actor* a_actor) {
        for (auto& i : m_actors) {
            if (i.second.getActor() == a_actor) return &i.second;
        }
        return nullptr;
    }

    ThreadActor* Thread::GetActor(int a_position) {
        for (auto& i : m_actors) {
            if (i.first == a_position) return &i.second;
        }
        return nullptr;
    }

    int Thread::getActorPosition(RE::Actor* actor) {
        for (auto& i : m_actors) {
            if (i.second.getActor() == actor) return i.first;
        }
        return -1;
    }

    void Thread::free() {
        for (auto& actorIt : m_actors) {
            actorIt.second.free();
        }
    }

    void Thread::addActorSink(RE::Actor* a_actor) {
        RE::BSAnimationGraphManagerPtr graphManager;
        a_actor->GetAnimationGraphManager(graphManager);
        if (graphManager) {
		    for (const auto& animationGraph : graphManager->graphs) {
			    animationGraph->GetEventSource<RE::BSAnimationGraphEvent>()->AddEventSink(this);
		    }
	    }
    }

    void Thread::removeActorSink(RE::Actor* a_actor) {
        RE::BSAnimationGraphManagerPtr graphManager;
        a_actor->GetAnimationGraphManager(graphManager);
        if (graphManager) {
		    for (const auto& animationGraph : graphManager->graphs) {
			    animationGraph->GetEventSource<RE::BSAnimationGraphEvent>()->RemoveEventSink(this);
		    }
	    }
    }

    RE::BSEventNotifyControl Thread::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) {
        if (!a_event || !a_event->holder) {
            return RE::BSEventNotifyControl::kContinue;
        }
        auto actor = const_cast<RE::Actor*>(static_cast<const RE::Actor*>(a_event->holder));

        RE::BSAnimationGraphManagerPtr graphManager;
        actor->GetAnimationGraphManager(graphManager);
        if (!graphManager) {
            return RE::BSEventNotifyControl::kContinue;
        }

        uint32_t activeGraphIdx = graphManager->GetRuntimeData().activeGraph;

        if (graphManager->graphs[activeGraphIdx] && graphManager->graphs[activeGraphIdx].get() != a_eventSource) {
            return RE::BSEventNotifyControl::kContinue;
        }

        std::string tag = a_event->tag.c_str();

        if (tag == "OStimClimax") {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                auto args = RE::MakeFunctionArguments(std::move(actor));
                auto handle = skyrimVM->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(Graph::LookupTable::OSexIntegrationMainQuest->FORMTYPE), Graph::LookupTable::OSexIntegrationMainQuest);
                vm->DispatchMethodCall2(handle, "OSexIntegrationMain", "Climax", args, callback);
            }
        } else if (tag == "OStimSpank") {
            //TODO
        } else if (tag == "OStimUndress") {
            GetActor(actor)->undress();
        } else if (tag == "OStimRedress") {
            GetActor(actor)->redress();
        } else if (tag == "OStimUndressPartial") {
            std::string payload = a_event->payload.c_str();
            int mask = std::stoi(payload, nullptr, 16);
            GetActor(actor)->undressPartial(mask);
        } else if (tag == "OStimRedressPartial"){
            std::string payload = a_event->payload.c_str();
            int mask = std::stoi(payload, nullptr, 16);
            GetActor(actor)->redressPartial(mask);
        } else if (tag == "OStimRemoveWeapons") {
            GetActor(actor)->removeWeapons();
        } else if (tag == "OStimAddWeapons"){
            GetActor(actor)->addWeapons();
        } else if (tag == "OStimEvent") {
            std::string indexStr = a_event->payload.c_str();
            int index = std::stoi(indexStr);
            if (index >= 0 && index < m_currentNode->xmlEvents.size()) {
                Graph::XmlEvent* xmlEvent = m_currentNode->xmlEvents[index];
                RE::Actor* actor;
                RE::Actor* target;
                RE::Actor* performer;

                for (auto& i : m_actors) {
                    if (i.first == xmlEvent->actor) {
                        actor = i.second.getActor();
                    }
                    if (i.first == xmlEvent->target) {
                        target = i.second.getActor();
                    }
                    if (i.first == xmlEvent->performer) {
                        performer = i.second.getActor();
                    }
                }

                //TODO: send in papyrus
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

}  // namespace OStim