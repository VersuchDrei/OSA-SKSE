#pragma once

#include "Core/ThreadManager.h"

namespace PapyrusScenes {
    using VM = RE::BSScript::IVirtualMachine;

    void StartScene(RE::StaticFunctionTag*, int64_t a_id, std::vector<RE::Actor*> a_actors) {
        auto tm = OStim::ThreadManager::GetSingleton();
        tm->TrackThread(a_id, a_actors);
    }

    void EndScene(RE::StaticFunctionTag*, int64_t a_id) {
        auto tm = OStim::ThreadManager::GetSingleton();
        tm->UnTrackThread(a_id);
    }

    void ChangeAnimation(RE::StaticFunctionTag*, int64_t a_id, std::string a_nodeName) {
        auto tm = OStim::ThreadManager::GetSingleton();
        auto log = RE::ConsoleLog::GetSingleton();
        auto thread = tm->GetThread(a_id);
        if (thread) {
            auto node = Graph::LookupTable::GetNodeById(a_nodeName);
            if (node) {
                thread->ChangeNode(node);
                log->Print(("Changing Animation on thread: " + std::to_string(a_id) + " to " + a_nodeName).c_str());

            } else {
                log->Print(("Node " + a_nodeName + " not found").c_str());
            }

        } else {
            log->Print(("Thread "+ std::to_string(a_id) + "not found").c_str());
        }
    }

    void AddThirdActor(RE::StaticFunctionTag*, int64_t a_threadId, RE::Actor* a_actor) {
        auto tm = OStim::ThreadManager::GetSingleton();
        tm->GetThread(a_threadId)->AddThirdActor(a_actor);
    }

    void RemoveThirdActor(RE::StaticFunctionTag*, int64_t a_threadId) {
        auto tm = OStim::ThreadManager::GetSingleton();
        tm->GetThread(a_threadId)->RemoveThirdActor();
    }

    bool Bind(VM* a_vm) {
        const auto obj = "OSANative"sv;

        BIND(StartScene);
        BIND(EndScene);
        BIND(ChangeAnimation);
        BIND(AddThirdActor);
        BIND(RemoveThirdActor);

        return true;
    }
}  // namespace PapyrusScenes