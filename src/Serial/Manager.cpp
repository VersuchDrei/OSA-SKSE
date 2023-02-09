#include "Serial/Manager.h"

#include "OldThread.h"

#include "Core/ThreadManager.h"
#include "Game/Locker.h"
#include "Util/MCMTable.h"

namespace Serialization {
    inline const auto UndressingMaskRecord = _byteswap_ulong('UDRM');
    inline const auto OldThreadsRecord = _byteswap_ulong('OLTH');

    std::vector<OldThread> oldThreads;
    uint32_t deserializationErrors = 0;

    void closeOldThreads() {
        for (OldThread thread : oldThreads) {
            thread.close();
        }
        oldThreads.clear();
    }

    void Save(SKSE::SerializationInterface* serial) {
        std::unique_lock lock(_lock);
        logger::info("serializing data");

        if (!serial->OpenRecord(UndressingMaskRecord, 0)) {
            logger::error("Unable to open record to write cosave data.");
            return;
        }

        uint32_t undressingMask = MCM::MCMTable::getUndressingMask();
        serial->WriteRecordData(&undressingMask, sizeof(undressingMask));

        if (!serial->OpenRecord(OldThreadsRecord, 0)) {
            logger::error("Unable to open record to write cosave data.");
            return;
        }

        std::vector<OldThread> oldThreads = OStim::ThreadManager::GetSingleton()->serialize();
        size_t size = oldThreads.size();
        serial->WriteRecordData(&size, sizeof(size));
        for (OldThread oldThread : oldThreads) {
            oldThread.serialize(serial);
        }
    }

    void Load(SKSE::SerializationInterface* serial) {
        std::unique_lock lock(_lock);
        logger::info("deserializing data");

        std::uint32_t type;
        std::uint32_t size;
        std::uint32_t version;

        while (serial->GetNextRecordInfo(type, version, size)) {
            if (type == UndressingMaskRecord) {
                uint32_t undressingMask;
                serial->ReadRecordData(&undressingMask, sizeof(undressingMask));
                MCM::MCMTable::setUndressingMask(undressingMask);
            } else if (type == OldThreadsRecord){
                size_t size;
                serial->ReadRecordData(&size, sizeof(size));
                deserializationErrors = 0;
                for (int i = 0; i < size; i++) {
                    OldThread::deserialize(serial, oldThreads, deserializationErrors);
                }
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