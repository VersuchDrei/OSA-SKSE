#include "FacialExpression.h"

#include "TraitTable.h"

namespace Trait {
    void GenderExpression::apply(RE::Actor* actor, float speed, float excitement, std::vector<FaceModifier> phonemeOverride) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;

            auto faceData = actor->GetFaceGenAnimationData();
            faceData->Reset(0, true, true, true, false);

            // expression
            if (auto value = expression.calculate(speed, excitement)) {
                faceData->exprOverride = false;
                faceData->SetExpressionOverride(expression.type, value);
                faceData->exprOverride = true;
            }
            
            // modifiers
            for (auto& modifier : modifiers) {
                auto args = RE::MakeFunctionArguments(std::move(actor), std::move(modifier.type), std::move(modifier.calculate(speed, excitement)));
                vm->DispatchStaticCall("MfgConsoleFunc", "SetModifier", args, callback);
            }

            // phonemes
            if (phonemeOverride.empty()) {
                for (auto& phoneme : phonemes) {
                    auto args = RE::MakeFunctionArguments(std::move(actor), std::move(phoneme.type), std::move(phoneme.calculate(speed, excitement)));
                    vm->DispatchStaticCall("MfgConsoleFunc", "SetPhoneme", args, callback);
                }
            } else {
                for (auto& phoneme : phonemeOverride) {
                    auto args = RE::MakeFunctionArguments(std::move(actor), std::move(phoneme.type), std::move(phoneme.calculate(speed, excitement)));
                    vm->DispatchStaticCall("MfgConsoleFunc", "SetPhoneme", args, callback);
                }
            }
            
        }
    }

    void FacialExpression::apply(RE::Actor* actor, float speed, float excitement, PhonemeOverrideType phonemeOverride) {
        std::vector<FaceModifier> phonemeOverrideVector;
        switch (phonemeOverride) {
        case OpenMouth:
            phonemeOverrideVector = TraitTable::openMouthPhonemes;
            break;
        case Kissing:
            phonemeOverrideVector = TraitTable::kissingPhonemes;
            break;
        default:
            break;
        }

        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            female.apply(actor, speed, excitement, phonemeOverrideVector);
        } else {
            male.apply(actor, speed, excitement, phonemeOverrideVector);
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