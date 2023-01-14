#pragma once

#include "Furniture/Furniture.h"
#include "Trait/Condition.h"
#include "Action.h"
#include "Trait/FacialExpression.h"

namespace Graph {
    struct XmlEvent {
    public:
        std::string type;
        int actor;
        int target;
        int performer;
    };

    struct Actor {
    public:
        std::string position;
        int penisAngle;
        float scale = 1.0;
        float scaleHeight = 120.748;
        bool feetOnGround;
        int expressionAction = -1;
        std::unordered_map<int, Trait::FaceModifier> eyeballModifierOverride;
        std::vector<std::string> tags;
        Trait::ActorConditions conditions;
        std::unordered_map<std::string, std::string> autotransitions;
    };

    struct Node {
    public:
        std::string scene_id;
        std::string lowercase_id;
        uint32_t numActors;
        std::string scene_name;
        std::vector<std::string> anim_ids;  // this probably needs to be reworked to contain speed info        
        bool isTransition = false;
        bool isHub = false;
        bool isAggresive = false;
        uint32_t minspeed = 0;
        uint32_t maxspeed = 0;
        bool hasIdleSpeed = false;
        bool noRandomSelection = false;
        Furniture::FurnitureType furnitureType = Furniture::FurnitureType::NONE;
        std::vector<std::string> tags;
        std::vector<Actor*> actors;
        std::vector<Action*> actions;
        std::vector<XmlEvent*> xmlEvents;

        // maybe remove this in a later iteration?
        std::string sourceModule;
        std::string animClass;

        bool fulfilledBy(std::vector<Trait::ActorConditions> conditions);

        void updateActors(std::vector<RE::Actor*> actors, std::vector<float> rmheights, std::vector<float> offsets);
        void updateFacialExpressions(int position, RE::Actor* actor);
        float playExpressionEvent(int position, RE::Actor* actor, std::string eventName);

        uint32_t getStrippingMask(int index);

        std::string getAutoTransitionForActor(int position, std::string type);

        bool hasActorTag(int position, std::string tag);
        bool hasAnyActorTag(int position, std::vector<std::string> tags);
        bool hasAllActorTags(int position, std::vector<std::string> tags);
        bool hasOnlyListedActorTags(int position, std::vector<std::string> tags);

        int findAction(std::function<bool(Action*)> condition);
        std::vector<int> findActions(std::function<bool(Action*)> condition);

        bool hasActionTag(std::string tag);

        int findAction(std::string type);
        int findAnyAction(std::vector<std::string> types);
        int findActionForActor(int position, std::string type);
        int findAnyActionForActor(int position, std::vector<std::string> types);
        int findActionForTarget(int position, std::string type);
        int findAnyActionForTarget(int position, std::vector<std::string> types);
        int findActionForActorAndTarget(int actorPosition, int targetPosition, std::string type);
        int findAnyActionForActorAndTarget(int actorPosition, int targetPosition, std::vector<std::string> types);

    private:
        Trait::PhonemeOverrideType getOverrideType(int position);
        std::unordered_map<int, Trait::FaceModifier> getEyeballModifierOverride(int position);
    };
}