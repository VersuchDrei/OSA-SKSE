#include "FacialExpression.h"

#include "TraitTable.h"

namespace Trait {
    void GenderExpression::apply(RE::Actor* actor, bool isEvent, float speed, float excitement, std::unordered_map<int, FaceModifier> eyeballModifierOverride, std::unordered_map<int, FaceModifier> phonemeOverride) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;

            auto faceData = actor->GetFaceGenAnimationData();

            // expression
            if (auto value = expression.calculate(speed, excitement)) {
                faceData->exprOverride = false;
                faceData->SetExpressionOverride(expression.type, static_cast<float>(value) / 100.0f);
                faceData->exprOverride = true;
            }
            
            // modifiers
            if (!isEvent || !eyelidModifiers.empty()) {
                for (int i : eyelidModifierTypes) {
                    int current = faceData->modifierKeyFrame.values[i] * 100;
                    int goal = 0;
                    auto iter = eyelidModifiers.find(i);
                    if (iter != eyelidModifiers.end()) {
                        goal = iter->second.calculate(speed, excitement);
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3));
                        vm->DispatchStaticCall("_oGlobal", "BlendMo", args, callback);
                    }
                }
            }

            if (!isEvent || !eyebrowModifiers.empty()) {
                for (int i : eyebrowModifierTypes) {
                    int current = faceData->modifierKeyFrame.values[i] * 100;
                    int goal = 0;
                    auto iter = eyebrowModifiers.find(i);
                    if (iter != eyebrowModifiers.end()) {
                        goal = iter->second.calculate(speed, excitement);
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3));
                        vm->DispatchStaticCall("_oGlobal", "BlendMo", args, callback);
                    }
                }
            }

            auto& eyeballModifiersToUse = (isEvent && !eyeballModifiers.empty()) || eyeballModifierOverride.empty() ? eyeballModifiers : eyeballModifierOverride;
            if (!isEvent || !eyeballModifiersToUse.empty()) {
                for (int i : eyeballModifierTypes) {
                    int current = faceData->modifierKeyFrame.values[i] * 100;
                    int goal = 0;
                    auto iter = eyeballModifiersToUse.find(i);
                    if (iter != eyeballModifiersToUse.end()) {
                        goal = iter->second.calculate(speed, excitement);
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3));
                        vm->DispatchStaticCall("_oGlobal", "BlendMo", args, callback);
                    }
                }
            }

            // phonemes
            auto& phonemesToUse = phonemeOverride.empty() ? phonemes : phonemeOverride;
            if (!isEvent || !phonemesToUse.empty()) {
                for (int i = 0; i < 14; i++) {
                    int current = faceData->phenomeKeyFrame.values[i] * 100;
                    int goal = 0;
                    auto iter = phonemesToUse.find(i);
                    if (iter != phonemesToUse.end()) {
                        goal = iter->second.calculate(speed, excitement);
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3));
                        vm->DispatchStaticCall("_oGlobal", "BlendPh", args, callback);
                    }
                }
            }
        }
    }

    void FacialExpression::apply(RE::Actor* actor, bool isEvent, float speed, float excitement, std::unordered_map<int, FaceModifier> eyeballModifierOverride, PhonemeOverrideType phonemeOverride) {
        std::unordered_map<int, FaceModifier> phonemeOverrideMap;
        switch (phonemeOverride) {
        case OpenMouth:
            phonemeOverrideMap = TraitTable::openMouthPhonemes;
            break;
        case Licking:
            phonemeOverrideMap = TraitTable::lickingPhonemes;
            break;
        default:
            break;
        }

        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            female.apply(actor, isEvent, speed, excitement, eyeballModifierOverride, phonemeOverrideMap);
        } else {
            male.apply(actor, isEvent, speed, excitement, eyeballModifierOverride, phonemeOverrideMap);
        }
    }

    float FacialExpression::getDuration(RE::Actor* actor) {
        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            return female.duration;
        } else {
            return male.duration;
        }
    }
}