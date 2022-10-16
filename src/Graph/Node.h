#pragma once

namespace Graph {
    struct Action {
    public:
        std::string type;
        uint32_t actor;
        uint32_t target;
        uint32_t performer;
    };

    struct Actor {
    public:
        std::string position;
        int32_t penisAngle;
        float scale = 1.0;
        float scaleHeight = 120.748;
        bool feetOnGround;
        std::vector<std::string> tags;
    };

    struct Node {
    public:
        std::string scene_id;
        uint32_t numActors;
        std::string scene_name;
        std::vector<std::string> anim_ids;  // this probably needs to be reworked to contain speed info
        bool isTransition;
        bool isHub;
        bool isAggresive;
        uint32_t minspeed;
        uint32_t maxspeed;
        bool hasIdleSpeed;
        std::vector<std::string> tags;
        std::vector<Actor*> actors;
        std::vector<Action*> actions;

        // maybe remove this in a later iteration?
        std::string sourceModule;
        std::string animClass;

        void scaleActor(RE::Actor* actor, int position);
        void updateActors(std::vector<RE::Actor*> actors, std::vector<float> offsets);
    };
}