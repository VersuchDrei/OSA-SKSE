#include "FacialExpression.h"

#include "TraitTable.h"

namespace Trait {
    void GenderExpression::apply(RE::Actor* actor, float speed, float excitement, std::unordered_map<int, FaceModifier> phonemeOverride) {
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
            for (int i = 0; i < 14; i++) {
                int current = faceData->modifierKeyFrame.values[i] * 100;
                int goal = 0;
                auto iter = modifiers.find(i);
                if (iter != modifiers.end()) {
                    goal = iter->second.calculate(speed, excitement);
                }

                if (current != goal) {
                    auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3));
                    vm->DispatchStaticCall("_oGlobal", "BlendMo", args, callback);
                }
            }

            // phonemes
            auto& phonemesToUse = phonemeOverride.empty() ? phonemes : phonemeOverride;
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

    void FacialExpression::apply(RE::Actor* actor, float speed, float excitement, PhonemeOverrideType phonemeOverride) {
        std::unordered_map<int, FaceModifier> phonemeOverrideMap;
        switch (phonemeOverride) {
        case OpenMouth:
            phonemeOverrideMap = TraitTable::openMouthPhonemes;
            break;
        case Kissing:
            phonemeOverrideMap = TraitTable::kissingPhonemes;
            break;
        case Licking:
            phonemeOverrideMap = TraitTable::lickingPhonemes;
            break;
        default:
            break;
        }

        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            female.apply(actor, speed, excitement, phonemeOverrideMap);
        } else {
            male.apply(actor, speed, excitement, phonemeOverrideMap);
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