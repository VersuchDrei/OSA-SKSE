#include "Graph/Node.h"

#include "Graph/LookupTable.h"
#include "Trait/Condition.h"
#include "Trait/TraitTable.h"
#include "Util/ActorUtil.h"
#include "Util/MCMTable.h"
#include "Util/StringUtil.h"
#include "Util/VectorUtil.h"
#include "SKEE.h"

namespace Graph {
    bool Node::fulfilledBy(std::vector<Trait::ActorConditions> conditions) {
        int size = actors.size();
        if (size < conditions.size()) {
            return false;
        }

        for (int i = 0; i < size; i++) {
            if (!conditions[i].fulfills(actors[i]->conditions)) {
                return false;
            }
        }

        return true;
    }

    uint32_t Node::getStrippingMask(int position) {
        uint32_t mask = 0;
        for (auto& action : actions) {
            mask |= action->getStrippingMask(position);
        }
        return mask;
    }

    std::string Node::getAutoTransitionForActor(int position, std::string type) {
        if (position < 0 || position >= actors.size()) {
            return "";
        }

        StringUtil::toLower(&type);
        auto iter = actors[position]->autotransitions.find(type);
        if (iter != actors[position]->autotransitions.end()) {
            return iter->second;
        }
        return "";
    }

    bool Node::hasActorTag(int position, std::string tag) {
        if (position < 0 || position >= actors.size()) {
            return false;
        }
        return VectorUtil::contains(actors[position]->tags, tag);
    }

    bool Node::hasAnyActorTag(int position, std::vector<std::string> tags) {
        if (position < 0 || position >= actors.size()) {
            return false;
        }
        return VectorUtil::containsAny(actors[position]->tags, tags);
    }

    bool Node::hasAllActorTags(int position, std::vector<std::string> tags) {
        if (position < 0 || position >= actors.size()) {
            return false;
        }
        return VectorUtil::containsAll(actors[position]->tags, tags);
    }

    bool Node::hasOnlyListedActorTags(int position, std::vector<std::string> tags) {
        if (position < 0 || position >= actors.size()) {
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

    bool Node::hasActionTag(std::string tag) {
        for (auto& action : actions) {
            if (action->attributes->hasTag(tag)) {
                return true;
            }
        }
        return false;
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

    void Node::updateActors(std::vector<RE::Actor*> reActors, std::vector<float> rmheights, std::vector<float> offsets) {
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
        }
    }

    void Node::updateFacialExpressions(int position, RE::Actor* actor) {
        if (Trait::TraitTable::areFacialExpressionsBlocked(actor)) {
            return;
        }

        if (position > actors.size()) {
            if (actors[position]->expressionAction != -1 && actors[position]->expressionAction < actions.size()) {
                auto& action = actions[actors[position]->expressionAction];
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
        if (hasActorTag(position, "openmouth") || findAnyActionForActor(position, {"blowjob", "cunnilingus", "suckingnipples"}) != -1) {
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