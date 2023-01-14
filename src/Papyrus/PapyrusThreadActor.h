#pragma once

#include "Core/ThreadManager.h"

namespace PapyrusThreadActor {
    using VM = RE::BSScript::IVirtualMachine;

    void Undress(RE::StaticFunctionTag*, RE::Actor* actor) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->undress();
        }
    }

    void Redress(RE::StaticFunctionTag*, RE::Actor* actor) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->redress();
        }
    }

    void UndressPartial(RE::StaticFunctionTag*, RE::Actor* actor, int mask) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->undressPartial(mask);
        }
    }

    void RedressPartial(RE::StaticFunctionTag*, RE::Actor* actor, int mask) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->redressPartial(mask);
        }
    }

    void RemoveWeapons(RE::StaticFunctionTag*, RE::Actor* actor) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->removeWeapons();
        }
    }

    void AddWeapons(RE::StaticFunctionTag*, RE::Actor* actor) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->addWeapons();
        }
    }

    bool Bind(VM* a_vm) {
        const auto obj = "OActor"sv;

        BIND(Undress);
        BIND(Redress);
        BIND(UndressPartial);
        BIND(RedressPartial);
        BIND(RemoveWeapons);
        BIND(AddWeapons);

        return true;
    }
}  // namespace PapyrusThreadActor