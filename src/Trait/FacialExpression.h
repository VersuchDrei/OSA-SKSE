#pragma once

namespace Trait {
    static const std::vector<int> eyelidModifierTypes = {0, 1, 12, 13};
    static const std::vector<int> eyebrowModifierTypes = {2, 3, 4, 5, 6, 7};
    static const std::vector<int> eyeballModifierTypes = {8, 9, 10, 11};

    enum ExpressionType {
        EXPRESSION = 1 << 0,
        PHONEME = 1 << 1,
        LID_MODIFIER = 1 << 2,
        BROW_MODIFIER = 1 << 3,
        BALL_MODIFIER = 1 << 4
    };

    struct FaceModifier {
    public:
        int type = 0;
        float baseValue = 0;
        float speedMultiplier = 0;
        float excitementMultiplier = 0;
        float delay = 0;
        float delayVariance = 0;

        inline static auto rng = std::default_random_engine{};

        inline int calculate(float speed, float excitement) {
            return (int)(baseValue + speedMultiplier * speed + excitementMultiplier * excitement);
        }

        inline float randomizeDelay() {
            return delay + (delayVariance == 0 ? 0 : std::uniform_real_distribution<float>(0, delayVariance)(rng));
        }
    };

    struct GenderExpression {
    public:
        float duration = 0;
        FaceModifier expression;
        std::unordered_map<int, FaceModifier> eyelidModifiers;
        std::unordered_map<int, FaceModifier> eyebrowModifiers;
        std::unordered_map<int, FaceModifier> eyeballModifiers;
        std::unordered_map<int, FaceModifier> phonemes;
        int typeMask = 0;

        void apply(RE::Actor* actor, int mask, float speed, float excitement);
    };

    struct FacialExpression {
    public:
        GenderExpression female;
        GenderExpression male;

        void apply(RE::Actor* actor, int mask, float speed, float excitement);
        float getDuration(RE::Actor* actor);
        int getTypeMask(RE::Actor* actor);
    };
}