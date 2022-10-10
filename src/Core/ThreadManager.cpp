#include "Core/ThreadManager.h"

namespace OStim {
    void ThreadManager::TrackThread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
        OstimThread* t = new OstimThread(a_id, a_actors);
        m_threadMap.insert(std::make_pair(a_id, t));
        auto log = RE::ConsoleLog::GetSingleton();
        if (log) {
            log->Print(("Tracking " + std::to_string(a_id)).c_str());
        }
    }

    OstimThread* ThreadManager::GetThread(ThreadId a_id)
    {
        auto it = m_threadMap.find(a_id);
        assert(it != m_threadMap.end());
        return it->second;
         
    }

    void ThreadManager::UnTrackThread(ThreadId a_id) {
        auto it = m_threadMap.find(a_id);
        if (it != m_threadMap.end()) {
            delete it->second;
            m_threadMap.erase(a_id);
            auto log = RE::ConsoleLog::GetSingleton();
            if (log) {
                log->Print(("Found scene: erasing " + std::to_string(a_id)).c_str());
            }
        }
    }
}  // namespace OStim