#pragma once
#include "Core/Singleton.h"
#include "Core/Thread.h"

namespace OStim {    

    class ThreadManager final : public ISingleton<ThreadManager> {
    public:
        void TrackThread(ThreadId a_id, std::vector<RE::Actor*> a_actors);
        Thread* GetThread(ThreadId a_id);
        void UnTrackThread(ThreadId a_id);

    private:
        using ThreadMap = std::unordered_map<ThreadId, Thread*>;

        ThreadMap m_threadMap;
    };
}  // namespace OStim