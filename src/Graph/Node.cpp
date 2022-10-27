#include "Graph/Node.h"

#include "Graph/LookupTable.h"
#include "Trait/TraitTable.h"
#include "Util/ActorUtil.h"
#include "Util/VectorUtil.h"
#include "SKEE.h"

namespace Graph {
    bool Node::hasActorTag(int position, std::string tag) {
        return VectorUtil::contains(actors[position]->tags, tag); }

    int Node::findAction(std::function<bool(Action*)> condition) {
        size_t size = actions.size();
        for (int i = 0; i < size; i++) {
            if (condition(actions[i])) {
                return i;
            }
        }
        return -1;
    }

    std::vector<int> Node::findActions(std::function<bool(Action*)> condition) {
        std::vector<int> ret;
        size_t size = actions.size();
        for (int i = 0; i < size; i++) {
            if (condition(actions[i])) {
                ret.push_back(i);
            }
        }
        return ret;
    }

    int Node::findActionForActor(int position, std::string type) {
        return findAction([position, type](Action* action) {return action->actor == position && action->type == type;});
    }

    int Node::findAnyActionForActor(int position, std::vector<std::string> types) {
        return findAction([position, types](Action* action) {return action->actor == position && VectorUtil::contains(types, action->type);});
    }

    int Node::findActionForTarget(int position, std::string type) {
        return findAction([position, type](Action* action) {return action->target == position && action->type == type;});
    }

    void Node::updateActors(std::vector<RE::Actor*> reActors, std::vector<float> offsets) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;

        int count = std::min(actors.size(), reActors.size());
        for (int i = 0; i < count; i++) {
            // penis bending
            reActors[i]->NotifyAnimationGraph("SOSBend" + std::to_string(actors[i]->penisAngle));

            // mouth controls
            updateFacialExpressions(i, reActors[i]);

            // scaling
            float newScale = actors[i]->scale / reActors[i]->GetActorBase()->GetHeight();
            if (actors[i]->feetOnGround && offsets[i] != 0) {
                newScale *= actors[i]->scaleHeight / (actors[i]->scaleHeight + offsets[i]);
            }

            // TODO: RE Actor::SetScale
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                auto args = RE::MakeFunctionArguments(std::move(newScale));
                auto handle = RE::SkyrimVM::GetSingleton()->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(reActors[i]->FORMTYPE), reActors[i]);
                vm->DispatchMethodCall2(handle, "Actor", "SetScale", args, callback);
            }

            // heel offsets
            if (offsets[i] != 0) {
                // the NiTransformInterface has only been added to RaceMenu after the AE update
                // so for SE we have to invoke Papyrus here :^(
                if (REL::Module::GetRuntime() == REL::Module::Runtime::AE) {
                    bool isFemale = reActors[i]->GetActorBase()->GetSex() == RE::SEX::kFemale;
                    auto nioInterface = Graph::LookupTable::GetNiTransformInterface();
                    bool hasOffset = nioInterface->HasNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal");
                    if (actors[i]->feetOnGround) {
                        if (hasOffset) {
                            auto offset = nioInterface->GetNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal");
                            if (offset.z == 0) {
                                offset.z = offsets[i];
                            nioInterface->AddNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal", offset);
                            nioInterface->UpdateNodeTransforms(reActors[i], false, isFemale, "NPC");
                            }
                        } else {
                            SKEE::INiTransformInterface::Position offset{};
                            offset.z = offsets[i];
                            nioInterface->AddNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal", offset);
                            nioInterface->UpdateNodeTransforms(reActors[i], false, isFemale, "NPC");
                        }
                    } else {
                        if (hasOffset) {
                            nioInterface->RemoveNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal");
                            nioInterface->UpdateNodeTransforms(reActors[i], false, isFemale, "NPC");
                        }
                    }
                } else {
                    if (vm) {
                        RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                        float height = actors[i]->scaleHeight;
                        auto args = RE::MakeFunctionArguments(std::move(reActors[i]), std::move(actors[i]->feetOnGround), std::move(offsets[i]));
                        vm->DispatchStaticCall("OSANative", "CheckOffset", args, callback);
                    }
                }
            }
        }
    }

    void Node::updateFacialExpressions(int position, RE::Actor* actor) {
        if (Trait::TraitTable::areFacialExpressionsBlocked(actor)) {
            return;
        }

        for (auto& action : actions) {
            if (action->target == position) {
                if (auto expression = Trait::TraitTable::getExpressionForActionTarget(action->type)) {
                    expression->apply(actor, 0, Trait::TraitTable::getExcitement(actor), getOverrideType(position));
                    return;
                }
            } else if (action->actor == position) {
                if (auto expression = Trait::TraitTable::getExpressionForActionActor(action->type)) {
                    expression->apply(actor, 0, Trait::TraitTable::getExcitement(actor), getOverrideType(position));
                    return;
                }
            }
        }

        if (auto expression = Trait::TraitTable::getExpressionForEvent("default")) {
            expression->apply(actor, 0, Trait::TraitTable::getExcitement(actor), getOverrideType(position));
            return;
        }

        Trait::TraitTable::fallbackExpression.apply(actor, 0, Trait::TraitTable::getExcitement(actor), getOverrideType(position));
    }

    float Node::playExpressionEvent(int position, RE::Actor* actor, std::string eventName) {
        if (auto expression = Trait::TraitTable::getExpressionForEvent(eventName)) {
            expression->apply(actor, 0, Trait::TraitTable::getExcitement(actor), getOverrideType(position));
            return expression->getDuration(actor);
        }
        return -1;
    }

    Trait::PhonemeOverrideType Node::getOverrideType(int position) {
        if (hasActorTag(position, "openmouth") || findAnyActionForActor(position, {"blowjob", "cunnilingus"}) != -1) {
            return Trait::PhonemeOverrideType::OpenMouth;
        } else if (hasActorTag(position, "kissing") || findActionForActor(position, "kissing") != -1 || findActionForTarget(position, "kissing") != -1) {
            return Trait::PhonemeOverrideType::Kissing;
        }
        return Trait::PhonemeOverrideType::NoOveride;
    }
}