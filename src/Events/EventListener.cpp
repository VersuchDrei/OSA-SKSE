#include "EventListener.h"

namespace Events {
    RE::BSEventNotifyControl EventListener::ProcessEvent(const RE::TESLoadGameEvent* a_event, RE::BSTEventSource<RE::TESLoadGameEvent>* a_eventSource) {
        logger::info("game loaded");

        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new CheckPapyrusUndressingCallbackFunctor());
            auto args = RE::MakeFunctionArguments();
            vm->DispatchStaticCall("OUndress", "UsePapyrusUndressing", args, callback);
        }

        return RE::BSEventNotifyControl::kContinue;
    }
}