#include "MCMTable.h"

namespace MCM {
    void MCMTable::setupForms() {
        auto dataHandler = RE::TESDataHandler::GetSingleton();
        disableScalingSetting = dataHandler->LookupForm<RE::TESGlobal>(0xD94, "OStim.esp");
        disableSchlongBendingSetting = dataHandler->LookupForm<RE::TESGlobal>(0xD97, "OStim.esp");
        maleExcitementMultSetting = dataHandler->LookupForm<RE::TESGlobal>(0xDA2, "OStim.esp");
        femaleExcitementMultSetting = dataHandler->LookupForm<RE::TESGlobal>(0xDA3, "OStim.esp");
    }

    bool MCMTable::isScalingDisabled() {
        return disableScalingSetting->value != 0;
    }

    bool MCMTable::isSchlongBendingDisabled() {
        return disableSchlongBendingSetting->value != 0;
    }


    float MCMTable::getMaleSexExcitementMult() {
        return maleExcitementMultSetting->value;
    }


    float MCMTable::getFemaleSexExcitementMult() {
        return femaleExcitementMultSetting->value;
    }
}