#include "ActorUtil.h"

namespace ActorUtil {
    bool isMouthOpen(RE::Actor* actor) {
        return std::lround(actor->GetFaceGenAnimationData()->phenomeKeyFrame.values[0] * 100.0f) > 75;
    }

    void openMouth(RE::Actor* actor) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto args = RE::MakeFunctionArguments(std::move(actor), std::move(0), std::move(1), std::move(40));
            vm->DispatchStaticCall("MfgConsoleFunc", "SetPhonemeModifier", args, callback);
            args = RE::MakeFunctionArguments(std::move(actor), std::move(0), std::move(0), std::move(100));
            vm->DispatchStaticCall("MfgConsoleFunc", "SetPhonemeModifier", args, callback);
            args = RE::MakeFunctionArguments(std::move(actor), std::move(0), std::move(5), std::move(100));
            vm->DispatchStaticCall("MfgConsoleFunc", "SetPhonemeModifier", args, callback);
        }
        /*
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            int curr = std::lround(actor->GetFaceGenAnimationData()->phenomeKeyFrame.values[1] * 100.0f);
            auto args = RE::MakeFunctionArguments(std::move(actor), std::move(40), std::move(curr), std::move(0), std::move(1), std::move(3));
            vm->DispatchStaticCall("_oGlobal", "BlendPh", args, callback);
            curr = std::lround(actor->GetFaceGenAnimationData()->phenomeKeyFrame.values[0] * 100.0f);
            args = RE::MakeFunctionArguments(std::move(actor), std::move(100), std::move(curr), std::move(0), std::move(0), std::move(3));
            vm->DispatchStaticCall("_oGlobal", "BlendPh", args, callback);
            curr = std::lround(actor->GetFaceGenAnimationData()->phenomeKeyFrame.values[5] * 100.0f);
            args = RE::MakeFunctionArguments(std::move(actor), std::move(100), std::move(curr), std::move(0), std::move(5), std::move(3));
            vm->DispatchStaticCall("_oGlobal", "BlendPh", args, callback);
        }
        */
    }

    void closeMouth(RE::Actor* actor) {
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto args = RE::MakeFunctionArguments(std::move(actor), std::move(0), std::move(1), std::move(0));
            vm->DispatchStaticCall("MfgConsoleFunc", "SetPhonemeModifier", args, callback);
            args = RE::MakeFunctionArguments(std::move(actor), std::move(0), std::move(0), std::move(0));
            vm->DispatchStaticCall("MfgConsoleFunc", "SetPhonemeModifier", args, callback);
            args = RE::MakeFunctionArguments(std::move(actor), std::move(0), std::move(5), std::move(0));
            vm->DispatchStaticCall("MfgConsoleFunc", "SetPhonemeModifier", args, callback);
        }
    }
}