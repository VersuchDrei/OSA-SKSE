#include "Graph/Node.h"

#include "Graph/LookupTable.h"
#include "Trait/Condition.h"
#include "Trait/TraitTable.h"
#include "Util/ActorUtil.h"
#include "Util/MCMTable.h"
#include "Util/VectorUtil.h"
#include "SKEE.h"

namespace Graph {
    bool Node::fulfilledBy(std::vector<Trait::ActorConditions> conditions) {
        int size = actors.size();
        if (size != conditions.size()) {
            return false;
        }

        for (int i = 0; i < size; i++) {
            if (!conditions[i].fulfills(actors[i]->conditions)) {
                return false;
            }
        }

        return true;
    }

    bool Node::hasActorTag(int position, std::string tag) {
        if (position >= actors.size()) {
            return false;
        }
        return VectorUtil::contains(actors[position]->tags, tag);
    }

    bool Node::hasAnyActorTag(int position, std::vector<std::string> tags) {
        if (position >= actors.size()) {
            return false;
        }
        return VectorUtil::containsAny(actors[position]->tags, tags);
    }

    bool Node::hasAllActorTags(int position, std::vector<std::string> tags) {
        if (position >= actors.size()) {
            return false;
        }
        return VectorUtil::containsAll(actors[position]->tags, tags);
    }

    bool Node::hasOnlyListedActorTags(int position, std::vector<std::string> tags) {
        if (position >= actors.size()) {
            return true;
        }
        return VectorUtil::containsAll(tags, actors[position]->tags);
    }

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

    int Node::findAction(std::string type) {
        return findAction([type](Action* action) { return action->type == type; });
    }

    int Node::findAnyAction(std::vector<std::string> types) {
        return findAction([types](Action* action) { return VectorUtil::contains(types, action->type); });
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

    int Node::findAnyActionForTarget(int position, std::vector<std::string> types) {
        return findAction([position, types](Action* action) {
            return action->target == position && VectorUtil::contains(types, action->type);
        });
    }

    int Node::findActionForActorAndTarget(int actorPosition, int targetPosition, std::string type) {
        return findAction([actorPosition, targetPosition, type](Action* action) {return action->actor == actorPosition && action->target == targetPosition && action->type == type;});
    }

    int Node::findAnyActionForActorAndTarget(int actorPosition, int targetPosition, std::vector<std::string> types) {
        return findAction([actorPosition, targetPosition, types](Action* action) {return action->actor == actorPosition && action->target == targetPosition && VectorUtil::contains(types, action->type);});
    }

    void Node::updateActors(std::vector<RE::Actor*> reActors, std::vector<float> offsets) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;

        int count = std::min(actors.size(), reActors.size());
        for (int i = 0; i < count; i++) {
            // penis bending
            if (!MCM::MCMTable::isSchlongBendingDisabled()) {
                reActors[i]->NotifyAnimationGraph("SOSBend" + std::to_string(actors[i]->penisAngle));
            }

            // expressions
            if (!Trait::TraitTable::areFacialExpressionsBlocked(reActors[i])) {
                updateFacialExpressions(i, reActors[i]);
            }

            // scaling
            if (!MCM::MCMTable::isScalingDisabled()) {
                float newScale = actors[i]->scale / reActors[i]->GetActorBase()->GetHeight();
                if (actors[i]->feetOnGround && offsets[i] != 0) {
                    newScale *= actors[i]->scaleHeight / (actors[i]->scaleHeight + offsets[i]);
                }

                // setscale resets 3BA physics, so we don't do it if the actor already has the desired scale
                if (static_cast<int>(newScale * 100) != reActors[i]->GetReferenceRuntimeData().refScale) {
                    ActorUtil::setScale(reActors[i], newScale);
                }
            }
            

            // heel offsets
            if (offsets[i] != 0) {
                // the NiTransformInterface has only been added to RaceMenu after the AE update
                // so for SE we have to invoke Papyrus here :^(
                if (REL::Module::GetRuntime() == REL::Module::Runtime::AE) {
                    bool isFemale = reActors[i]->GetActorBase()->GetSex() == RE::SEX::kFemale;
                    auto nioInterface = Graph::LookupTable::getNiTransformInterface();
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

        if (position > actors.size()) {
            if (actors[position]->expressionAction != -1 && actors[position]->expressionAction < actions.size()) {
                if (auto expression = Trait::TraitTable::getExpressionForActionActor(actions[actors[position]->expressionAction]->type)) {
                    expression->apply(actor, false, 0, Trait::TraitTable::getExcitement(actor), getEyeballModifierOverride(position), getOverrideType(position));
                    return;
                }
            }
        }

        for (auto& action : actions) {
            if (action->target == position) {
                if (auto expression = Trait::TraitTable::getExpressionForActionTarget(action->type)) {
                    expression->apply(actor, false, 0, Trait::TraitTable::getExcitement(actor), getEyeballModifierOverride(position), getOverrideType(position));
                    return;
                }
            } else if (action->actor == position) {
                if (auto expression = Trait::TraitTable::getExpressionForActionActor(action->type)) {
                    expression->apply(actor, false, 0, Trait::TraitTable::getExcitement(actor), getEyeballModifierOverride(position), getOverrideType(position));
                    return;
                }
            }
        }

        if (auto expression = Trait::TraitTable::getExpressionForEvent("default")) {
            expression->apply(actor, false, 0, Trait::TraitTable::getExcitement(actor), getEyeballModifierOverride(position), getOverrideType(position));
            return;
        }

        Trait::TraitTable::fallbackExpression.apply(actor, false, 0, Trait::TraitTable::getExcitement(actor), getEyeballModifierOverride(position), getOverrideType(position));
    }

    float Node::playExpressionEvent(int position, RE::Actor* actor, std::string eventName) {
        if (auto expression = Trait::TraitTable::getExpressionForEvent(eventName)) {
            expression->apply(actor, true, 0, Trait::TraitTable::getExcitement(actor), {}, Trait::PhonemeOverrideType::NoOveride);
            return expression->getDuration(actor);
        }
        return -1;
    }

    Trait::PhonemeOverrideType Node::getOverrideType(int position) {
        if (hasActorTag(position, "openmouth") || findAnyActionForActor(position, {"blowjob", "cunnilingus"}) != -1) {
            return Trait::PhonemeOverrideType::OpenMouth;
        } else if (hasActorTag(position, "licking") || findAnyActionForActor(position, {"lickingnipples", "lickingpenis", "lickingtesticles", "lickingvagina", "rimjob"}) != -1) {
            return Trait::PhonemeOverrideType::Licking;
        }
        return Trait::PhonemeOverrideType::NoOveride;
    }

    std::unordered_map<int, Trait::FaceModifier> Node::getEyeballModifierOverride(int position) {
        if (actors.size() > position) {
            return actors[position]->eyeballModifierOverride;
        }
        return {};
    }
}