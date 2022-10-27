#include "MCMTable.h"

namespace MCM {
    void MCMTable::setupForms() {
        disableScalingSetting = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x00000D94, "OStim.esp");
    }

    bool MCMTable::isScalingDisabled() {
        return disableScalingSetting->value != 0;
    }
}