#include "FacialExpression.h"

#include "TraitTable.h"

namespace Trait {
    void GenderExpression::apply(RE::Actor* actor, int mask, float speed, float excitement) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;

            auto faceData = actor->GetFaceGenAnimationData();

            // expression
            if ((mask & ExpressionType::EXPRESSION) == ExpressionType::EXPRESSION) {
                if (auto value = expression.calculate(speed, excitement)) {
                    faceData->exprOverride = false;
                    faceData->SetExpressionOverride(expression.type, static_cast<float>(value) / 100.0f);
                    faceData->exprOverride = true;
                }
            }
            
            
            // modifiers
            if ((mask & ExpressionType::LID_MODIFIER) == ExpressionType::LID_MODIFIER) {
                for (int i : eyelidModifierTypes) {
                    int current = faceData->modifierKeyFrame.values[i] * 100;
                    int goal = 0;
                    float delay = 0;
                    auto iter = eyelidModifiers.find(i);
                    if (iter != eyelidModifiers.end()) {
                        goal = iter->second.calculate(speed, excitement);
                        delay = iter->second.randomizeDelay();
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3), std::move(delay));
                        vm->DispatchStaticCall("OExpression", "BlendMo", args, callback);
                    }
                }
            }

            if ((mask & ExpressionType::BROW_MODIFIER) == ExpressionType::BROW_MODIFIER) {
                for (int i : eyebrowModifierTypes) {
                    int current = faceData->modifierKeyFrame.values[i] * 100;
                    int goal = 0;
                    float delay = 0;
                    auto iter = eyebrowModifiers.find(i);
                    if (iter != eyebrowModifiers.end()) {
                        goal = iter->second.calculate(speed, excitement);
                        delay = iter->second.randomizeDelay();
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3), std::move(delay));
                        vm->DispatchStaticCall("OExpression", "BlendMo", args, callback);
                    }
                }
            }

            if ((mask & ExpressionType::BALL_MODIFIER) == ExpressionType::BALL_MODIFIER) {
                for (int i : eyeballModifierTypes) {
                    int current = faceData->modifierKeyFrame.values[i] * 100;
                    int goal = 0;
                    float delay = 0;
                    auto iter = eyeballModifiers.find(i);
                    if (iter != eyeballModifiers.end()) {
                        goal = iter->second.calculate(speed, excitement);
                        delay = iter->second.randomizeDelay();
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3), std::move(delay));
                        vm->DispatchStaticCall("OExpression", "BlendMo", args, callback);
                    }
                }
            }

            // phonemes
            if ((mask & ExpressionType::PHONEME) == ExpressionType::PHONEME) {
                for (int i = 0; i < 14; i++) {
                    int current = faceData->phenomeKeyFrame.values[i] * 100;
                    int goal = 0;
                    float delay = 0;
                    auto iter = phonemes.find(i);
                    if (iter != phonemes.end()) {
                        goal = iter->second.calculate(speed, excitement);
                        delay = iter->second.randomizeDelay();
                    }

                    if (current != goal) {
                        auto args = RE::MakeFunctionArguments(std::move(actor), std::move(goal), std::move(current), std::move(i), std::move(3), std::move(delay));
                        vm->DispatchStaticCall("OExpression", "BlendPh", args, callback);
                    }
                }
            }
        }
    }

    void FacialExpression::apply(RE::Actor* actor, int mask, float speed, float excitement) {
        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            female.apply(actor, mask, speed, excitement);
        } else {
            male.apply(actor, mask, speed, excitement);
        }
    }

    float FacialExpression::getDuration(RE::Actor* actor) {
        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            return female.duration;
        } else {
            return male.duration;
        }
    }

    int FacialExpression::getTypeMask(RE::Actor* actor) {
        if (actor->GetActorBase()->GetSex() == RE::SEX::kFemale) {
            return female.typeMask;
        } else {
            return male.typeMask;
        }
    }
}