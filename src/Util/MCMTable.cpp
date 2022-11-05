#include "MCMTable.h"

namespace MCM {
    void MCMTable::setupForms() {
        disableScalingSetting = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x00000D94, "OStim.esp");
        disableSchlongBendingSetting = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x00000D97, "OStim.esp");
    }

    bool MCMTable::isScalingDisabled() {
        return disableScalingSetting->value != 0;
    }

    bool MCMTable::isSchlongBendingDisabled() {
        return disableSchlongBendingSetting->value != 0;
    }
}