#pragma once
#include "Core/Singleton.h"
#include "Core/Thread.h"
#include <shared_mutex>

namespace OStim {    

    class ThreadManager final : public ISingleton<ThreadManager> {
        
    public:
        ThreadManager();
        void TrackThread(ThreadId a_id, std::vector<RE::Actor*> a_actors);
        Thread* GetThread(ThreadId a_id);
        void UnTrackThread(ThreadId a_id);

    private:
        using ThreadMap = std::unordered_map<ThreadId, Thread*>;
        std::shared_mutex m_threadMapMtx;
        ThreadMap m_threadMap;
        std::thread m_excitementThread;
    };
}  // namespace OStim