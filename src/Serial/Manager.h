#pragma once

namespace Serialization {
    inline std::mutex _lock;

    void closeOldThreads();

    void Save(SKSE::SerializationInterface* a_intfc);
    void Load(SKSE::SerializationInterface* a_intfc);
    void Revert(SKSE::SerializationInterface* a_intfc);
}  // namespace Serialization