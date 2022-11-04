#pragma once

namespace Trait {
    static const std::vector<int> eyelidModifierTypes = {0, 1, 12, 13};
    static const std::vector<int> eyebrowModifierTypes = {4, 5, 6, 7};
    static const std::vector<int> eyeballModifierTypes = {8, 9, 10, 11};

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
        std::unordered_map<int, FaceModifier> eyelidModifiers;
        std::unordered_map<int, FaceModifier> eyebrowModifiers;
        std::unordered_map<int, FaceModifier> eyeballModifiers;
        std::unordered_map<int, FaceModifier> phonemes;

        void apply(RE::Actor* actor, bool isEvent, float speed, float excitement, std::unordered_map<int, FaceModifier> eyeballModifierOverride, std::unordered_map<int, FaceModifier> phonemeOverride);
    };

    struct FacialExpression {
    public:
        GenderExpression female;
        GenderExpression male;

        void apply(RE::Actor* actor, bool isEvent, float speed, float excitement, std::unordered_map<int, FaceModifier> eyeballModifierOverride, PhonemeOverrideType phonemeOverride);
        float getDuration(RE::Actor* actor);
    };
}