#include "Serial/Manager.h"

#include "Core/ThreadManager.h"
#include "Game/Locker.h"
#include "Util/MCMTable.h"

namespace Serialization {
    inline const auto UndressingMaskRecord = _byteswap_ulong('UDRM');

    constexpr std::size_t SIZE = sizeof(uint32_t);

    std::string DecodeType(uint32_t a_typeCode) {
        std::string sig;
        sig.resize(SIZE);
        char* iter = reinterpret_cast<char*>(&a_typeCode);
        for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
            sig[j] = iter[i];
        }

        return sig;
    }

    void Save(SKSE::SerializationInterface* serial) {
        std::unique_lock lock(_lock);

        if (!serial->OpenRecord(UndressingMaskRecord, 0)) {
            logger::error("Unable to open record to write cosave data.");
            return;
        }

        uint32_t undressingMask = MCM::MCMTable::getUndressingMask();
        serial->WriteRecordData(&undressingMask, sizeof(undressingMask));
    }

    void Load(SKSE::SerializationInterface* serial) {
        std::unique_lock lock(_lock);

        std::uint32_t type;
        std::uint32_t size;
        std::uint32_t version;

        while (serial->GetNextRecordInfo(type, version, size)) {
            if (type == UndressingMaskRecord) {
                uint32_t undressingMask;
                serial->ReadRecordData(&undressingMask, sizeof(undressingMask));
                MCM::MCMTable::setUndressingMask(undressingMask);
            } else {
                logger::warn("Unknown record type in cosave.");
            }
        }
    }

    void Revert(SKSE::SerializationInterface* serial) {
        std::unique_lock lock(_lock);
        auto locker = Locker::GetSingleton();
        locker->Revert(serial);
        OStim::ThreadManager::GetSingleton()->UntrackAllThreads();
        MCM::MCMTable::resetDefaults();
    }
}  // namespace Serialization