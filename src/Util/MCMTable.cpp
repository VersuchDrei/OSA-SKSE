#include "MCMTable.h"

namespace MCM {
    void MCMTable::setupForms() {
        disableScalingSetting = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x00000D94, "OStim.esp");
        disableSchlongAnglingSetting = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x00000D97, "OStim.esp");
    }

    bool MCMTable::isScalingDisabled() {
        return disableScalingSetting->value != 0;
    }

    bool MCMTable::isSchlongAnglingDisabled() {
        return disableSchlongAnglingSetting->value != 0;
    }
}