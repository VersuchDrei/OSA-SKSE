#pragma once
#include "Core/Singleton.h"

namespace OStim {

    using ThreadId = int64_t;
    class OstimThread {
    public:
        OstimThread(ThreadId a_id, std::vector<RE::Actor*> a_actors) {
            m_threadId = a_id;
            for (int i = 0; i < a_actors.size(); i++) {
                m_actors.insert(std::make_pair(i, a_actors[i]));
            }
        }

        void AddThirdActor(RE::Actor* a_actor) { m_actors.insert(std::make_pair(2, a_actor)); }

        void RemoveThirdActor() { m_actors.erase(2); }

    private:
        ThreadId m_threadId;
        std::map<int32_t, RE::Actor*> m_actors;
    };

    class ThreadManager final : public ISingleton<ThreadManager> {
    public:
        void TrackThread(ThreadId a_id, std::vector<RE::Actor*> a_actors);
        OstimThread* GetThread(ThreadId a_id);
        void UnTrackThread(ThreadId a_id);

    private:
        using ThreadMap = std::unordered_map<ThreadId, OstimThread*>;

        ThreadMap m_threadMap;
    };
}  // namespace OStim