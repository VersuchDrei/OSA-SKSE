#pragma once

#include "Core/Singleton.h"
#include "Util/MCMTable.h"

namespace Events {
    class EventListener final : public OStim::ISingleton<EventListener>, public RE::BSTEventSink<RE::TESLoadGameEvent> {
    public:
        virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESLoadGameEvent* a_event, RE::BSTEventSource<RE::TESLoadGameEvent>* a_eventSource) override;

    private:
        class CheckPapyrusUndressingCallbackFunctor : public RE::BSScript::IStackCallbackFunctor {
            virtual inline void operator()(RE::BSScript::Variable a_result) override {
                if (a_result.IsNoneObject()) {
                    logger::info("result is none");
                } else if (a_result.IsBool()) {
                    MCM::MCMTable::setPapyusUndressing(a_result.GetBool());
                } else {
                    logger::info("result is not a bool");
                }
            }

            virtual inline void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>& a_object){};
        };
    };
}