#pragma once

namespace Trait {
    enum PhonemeOverrideType {
        NoOveride,
        OpenMouth,
        Kissing,
        Licking
    };

    struct FaceModifier {
    public:
        int type;
        float baseValue;
        float speedMultiplier;
        float excitementMultiplier;

        inline int calculate(float speed, float excitement) {
            return baseValue + (int)(speedMultiplier * speed + excitementMultiplier * excitement);
        }
    };

    struct GenderExpression {
    public:
        float duration;
        FaceModifier expression;
        std::unordered_map<int, FaceModifier> phonemes;
        std::unordered_map<int, FaceModifier> modifiers;

        void apply(RE::Actor* actor, float speed, float excitement, std::unordered_map<int, FaceModifier> phonemeOverride);
    };

    struct FacialExpression {
    public:
        GenderExpression female;
        GenderExpression male;

        void apply(RE::Actor* actor, float speed, float excitement, PhonemeOverrideType phonemeOverride);
        float getDuration(RE::Actor* actor);
    };
}