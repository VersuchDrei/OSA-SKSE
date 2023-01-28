#pragma once

#include "Core/ThreadManager.h"
#include "Trait/TraitTable.h"

namespace PapyrusThreadActor {
    using VM = RE::BSScript::IVirtualMachine;

    float PlayExpression(RE::StaticFunctionTag*, RE::Actor* actor, std::string expression) {
        return 0.5;
        std::vector<Trait::FacialExpression*>* expressions = Trait::TraitTable::getExpressionsForEvent(expression);
        if (!expressions) {
            return -1;
        }

        Trait::FacialExpression* eventExpression = expressions->at(std::rand() % expressions->size());

        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->setEventExpression(eventExpression);
        }

        return eventExpression->getDuration(actor);
    }
    
    void ClearExpression(RE::StaticFunctionTag*, RE::Actor* actor) {
        OStim::ThreadActor* threadActor = OStim::ThreadManager::GetSingleton()->findActor(actor);
        if (threadActor) {
            threadActor->clearEventExpression();
        }
    }

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

        BIND(PlayExpression);
        BIND(ClearExpression);
        BIND(Undress);
        BIND(Redress);
        BIND(UndressPartial);
        BIND(RedressPartial);
        BIND(RemoveWeapons);
        BIND(AddWeapons);

        return true;
    }
}  // namespace PapyrusThreadActor