#pragma once

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

        return true;
    }
}