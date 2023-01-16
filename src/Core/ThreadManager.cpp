#include "Core/ThreadManager.h"

namespace OStim {

    ThreadManager::ThreadManager() {
        m_excitementThread = std::thread([&]() {
            auto calendar = RE::Calendar::GetSingleton();
            auto previousTime = calendar->GetCurrentGameTime();
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                auto currentTime = calendar->GetCurrentGameTime(); 
                if (previousTime < currentTime) {
                    std::shared_lock<std::shared_mutex> lock(m_threadMapMtx);
                    for (auto& it : m_threadMap) {
                        it.second->CalculateExcitement();
                    }
                }
                previousTime = currentTime;
            }
        });
        m_excitementThread.detach();
    }

    void ThreadManager::TrackThread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
        Thread* t = new Thread(a_id, a_actors);
        std::unique_lock<std::shared_mutex> lock(m_threadMapMtx);
        m_threadMap.insert(std::make_pair(a_id, t));
        auto log = RE::ConsoleLog::GetSingleton();
        if (log) {
            log->Print(("Tracking " + std::to_string(a_id)).c_str());
        }
    }

    Thread* ThreadManager::GetThread(ThreadId a_id) {
        std::shared_lock<std::shared_mutex> lock(m_threadMapMtx);
        auto it = m_threadMap.find(a_id);
        if (it == m_threadMap.end()) {
            return nullptr;
        }
        return it->second;
    }

    void ThreadManager::UnTrackThread(ThreadId a_id) {
        std::unique_lock<std::shared_mutex> lock(m_threadMapMtx);
        auto it = m_threadMap.find(a_id);
        if (it != m_threadMap.end()) {
            it->second->free();
            delete it->second;
            m_threadMap.erase(a_id);
            auto log = RE::ConsoleLog::GetSingleton();
            if (log) {
                log->Print(("Found scene: erasing " + std::to_string(a_id)).c_str());
            }
        }
    }

    void ThreadManager::UntrackAllThreads() {
        // this is a force close due to the user loading another save
        // so no need to free actors etc. here
        std::unique_lock<std::shared_mutex> lock(m_threadMapMtx);
        for (auto& entry : m_threadMap) {
            delete entry.second;
        }
        m_threadMap.clear();
    }

    bool ThreadManager::AnySceneRunning() {
        return m_threadMap.size() > 0;
    }

    ThreadActor* ThreadManager::findActor(RE::Actor* actor) {
        for (auto&[id, thread] : m_threadMap) {
            ThreadActor* threadActor = thread->GetActor(actor);
            if (threadActor) {
                return threadActor;
            }
        }

        return nullptr;
    }
}  // namespace OStim