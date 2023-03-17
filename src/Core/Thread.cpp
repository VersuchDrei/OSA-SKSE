#include "Core/Thread.h"
#include "Furniture/Furniture.h"
#include "Graph/LookupTable.h"
#include "Graph/Node.h"
#include <Messaging/IMessages.h>
#include "Util/CameraUtil.h"
#include "Util/Constants.h"
#include "Util/MathUtil.h"
#include "Util/MCMTable.h"
#include "Util/StringUtil.h"
#include "Util.h"

namespace OStim {
    Thread::Thread(ThreadId id, RE::TESObjectREFR* furniture, std::vector<RE::Actor*> actors) : m_threadId{id}, furniture{furniture} {
        // --- setting up the vehicle --- //
        RE::TESObjectREFR* center = furniture ? furniture : actors[0];
        vehicle = center->PlaceObjectAtMe(Graph::LookupTable::OStimVehicle, false).get();

        if (furniture) {
            std::vector<float> offset = Furniture::getOffset(furniture);
            float angle = furniture->GetAngleZ();
            float sin = std::sin(angle);
            float cos = std::cos(angle);
            vehicle->data.angle.z = angle + offset[3]; // setting the angle does not directly rotate the object, but the call to SetPosition updates it
            vehicle->SetPosition(furniture->GetPositionX() + cos * offset[0] + sin * offset[1],
                                 furniture->GetPositionY() - sin * offset[0] + cos * offset[1],
                                 furniture->GetPositionZ() + offset[2]);
            
        } else {
            vehicle->MoveTo(center);
        }

        float angle = center->GetAngleZ();

        // --- locking the furniture --- //
        if (furniture) {
            if (!Furniture::isFurnitureInUse(furniture, false)) {
                Furniture::lockFurniture(furniture, actors[0]);
                furnitureLocked = true;
            }
        }

        RE::Actor* player = RE::PlayerCharacter::GetSingleton();
        for (int i = 0; i < actors.size(); i++) {
            RE::Actor* actor = actors[i];
            addActorInner(i, actor);
            isPlayerThread |= actor == player;
        }

        if (isPlayerThread) {
            RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
            if (MCM::MCMTable::useFreeCam()) {
                if (!camera->IsInFreeCameraMode()) {
                    camera->ForceThirdPerson();
                    std::thread camThread = std::thread([&] {
                        std::this_thread::sleep_for(std::chrono::milliseconds(250));
                        CameraUtil::toggleFlyCam();
                    });
                    camThread.detach();
                }
            }

            RE::INISettingCollection* ini = RE::INISettingCollection::GetSingleton();
            RE::Setting* speed = ini->GetSetting("fFreeCameraTranslationSpeed:Camera");
            if (speed) {
                freeCamSpeedBefore = speed->data.f;
                speed->data.f = MCM::MCMTable::freeCamSpeed();
            }

            worldFOVbefore = camera->worldFOV;
            camera->worldFOV = MCM::MCMTable::freeCamFOV();
        }
    }

    Thread::~Thread() {
        for (auto& actorIt : m_actors) {
            removeActorSink(actorIt.second.getActor());
        }
    }

    void Thread::initContinue() {
        // this stuff needs to happen after the thread has been put into the map in thread manager
        for (auto& [index, actor] : m_actors) {
            actor.getActor()->EvaluatePackage();
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

            actorIt.second.baseExcitementInc = excitementInc;
            if (excitementInc <= 0) {
                actorIt.second.maxExcitement = 0;
            }


            // --- undressing --- //
            if (!m_currentNode->hasActorTag(actorIt.first, "nostrip")) {
                if (MCM::MCMTable::undressMidScene() && m_currentNode->doFullStrip(actorIt.first)) {
                    actorIt.second.undress();
                    actorIt.second.removeWeapons();
                    // it is intended that the else fires if undressMidScene is checked but the action is not tagged as
                    // sexual because some non sexual actions still have slots for partial stripping for example kissing
                    // undresses helmets without being sexual
                } else if (MCM::MCMTable::partialUndressing()) {
                    uint32_t slotMask = m_currentNode->getStrippingMask(actorIt.first);
                    if (slotMask != 0) {
                        actorIt.second.undressPartial(slotMask);
                        if ((slotMask & MCM::MCMTable::removeWeaponsWithSlot()) != 0) {
                            actorIt.second.removeWeapons();
                        }
                    }
                }
            }

            // --- scaling / heel offsets / facial expressions --- //
            if (actorIt.first < m_currentNode->actors.size()) {
                actorIt.second.changeNode(m_currentNode->actors[actorIt.first], m_currentNode->getFacialExpressions(actorIt.first), m_currentNode->getOverrideExpressions(actorIt.first));
            }
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

    Graph::Node* Thread::getCurrentNode() {
        return m_currentNode;
    }

    void Thread::AddActor(RE::Actor* actor) {
        int index = m_actors.size();
        addActorInner(index, actor);
        actor->EvaluatePackage();
    }

    void Thread::RemoveActor() {
        int index = m_actors.size() - 1;
        ThreadActor* actor = GetActor(index);
        removeActorSink(actor->getActor());
        actor->free();

        m_actors.erase(index);
    }

    void Thread::addActorInner(int index, RE::Actor* actor) {
        ActorUtil::lockActor(actor);
        ActorUtil::setVehicle(actor, vehicle);
        addActorSink(actor);
        m_actors.insert(std::make_pair(index, ThreadActor(m_threadId, actor)));
        ThreadActor* threadActor = GetActor(index);
        threadActor->initContinue();
        if (MCM::MCMTable::undressAtStart()) {
            threadActor->undress();
        }
        if (MCM::MCMTable::removeWeaponsAtStart()) {
            threadActor->removeWeapons();
        }
        actor->MoveTo(vehicle);
        alignActor(actor, 0, 0, 0);
    }

    void Thread::alignActor(RE::Actor* actor, float x, float y, float z) {
        ObjectRefUtil::translateTo(actor, vehicle->data.location.x + x, vehicle->data.location.y + y, vehicle->data.location.z + z,MathUtil::toDegrees(vehicle->data.angle.x),
                                   MathUtil::toDegrees(vehicle->data.angle.y), MathUtil::toDegrees(vehicle->data.angle.z) + 1, 1000000, 0.0001);
    }

    void Thread::loop() {
        std::shared_lock<std::shared_mutex> readLock;
        // TODO: Can remove this when we start scenes in c++ with a starting node
        if (!m_currentNode) {
            return;
        }

        for (auto& actorIt : m_actors) {
            actorIt.second.loop();
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

    void Thread::SetSpeed(int speed) {
        m_currentNodeSpeed = speed;
        for (auto& actorIt : m_actors) {
            if (m_currentNode) {
                if (m_currentNode->speeds.size() > speed) {
                    RE::Actor* actor = actorIt.second.getActor();
                    actor->SetGraphVariableFloat("OStimSpeed", m_currentNode->speeds[speed].playbackSpeed);
                    actor->NotifyAnimationGraph(m_currentNode->speeds[speed].animation + "_" + std::to_string(actorIt.first));
                }

                float speedMod = 1 + speed / m_currentNode->speeds.size();
                actorIt.second.loopExcitementInc = actorIt.second.baseExcitementInc * actorIt.second.baseExcitementMultiplier * speedMod * Constants::LOOP_TIME_SECONDS;
            }

            actorIt.second.changeSpeed(speed);
        }
    }

    void Thread::close() {
        vehicle->Disable();
        vehicle->SetDelete(true);

        for (auto& actorIt : m_actors) {
            actorIt.second.free();
        }

        if (isPlayerThread) {
            RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
            
            if (camera->IsInFreeCameraMode()) {
                CameraUtil::toggleFlyCam();
            }

            RE::INISettingCollection* ini = RE::INISettingCollection::GetSingleton();
            RE::Setting* speed = ini->GetSetting("fFreeCameraTranslationSpeed:Camera");
            if (speed) {
                speed->data.f = freeCamSpeedBefore;
            }

            camera->worldFOV = worldFOVbefore;
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
        } else if (tag == "OStimAddWeapons") {
            GetActor(actor)->addWeapons();
        } else if (tag == "OStimEquipObject"){
            GetActor(actor)->equipObject(a_event->payload.c_str());
        } else if (tag == "OStimUnequipObject") {
            GetActor(actor)->unequipObject(a_event->payload.c_str());
        } else if (tag == "OStimSetLooking") {
            std::vector<std::string> payloadVec = stl::string_split(a_event->payload.c_str(), ',');
            std::unordered_map<int, Trait::FaceModifier> eyeballOverride;
            if (!payloadVec.empty()) {
                float value = std::stoi(payloadVec[0]);
                int type = 9;
                if (value < 0) {
                    value *= -1;
                    type = 10;
                }
                eyeballOverride.insert({type, {.type = type, .baseValue = value}});
            }
            if (payloadVec.size() == 2){
                float value = std::stoi(payloadVec[1]);
                int type = 11;
                if (value < 0) {
                    value *= -1;
                    type = 8;
                }
                eyeballOverride.insert({type, {.type = type, .baseValue = value}});
            }
            GetActor(actor)->setLooking(eyeballOverride);
        } else if (tag == "OStimUnsetLooking") {
            GetActor(actor)->unsetLooking();
        } else if (tag == "OStimResetLooking") {
            GetActor(actor)->resetLooking();
        } else if (tag == "OStimEvent") {
            
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    Serialization::OldThread Thread::serialize() {
        Serialization::OldThread oldThread;

        oldThread.vehicle = vehicle;
        if (furnitureLocked) {
            oldThread.furniture = furniture;
        }

        for (auto& actorIt : m_actors) {
            oldThread.actors.push_back(actorIt.second.serialize());
        }

        return oldThread;
    }

}  // namespace OStim