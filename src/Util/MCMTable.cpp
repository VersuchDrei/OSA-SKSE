#include "MCMTable.h"

namespace MCM {
    void MCMTable::setupForms() {
        auto dataHandler = RE::TESDataHandler::GetSingleton();

        disableScalingSetting = dataHandler->LookupForm<RE::TESGlobal>(0xD94, "OStim.esp");
        disableSchlongBendingSetting = dataHandler->LookupForm<RE::TESGlobal>(0xD97, "OStim.esp");

        OStimUndressAtStart = dataHandler->LookupForm<RE::TESGlobal>(0xDAA, "OStim.esp");
        OStimRemoveWeaponsAtStart = dataHandler->LookupForm<RE::TESGlobal>(0xDAB, "OStim.esp");
        OStimUndressMidScene = dataHandler->LookupForm<RE::TESGlobal>(0xDAC, "OStim.esp");
        OStimPartialUndressing = dataHandler->LookupForm<RE::TESGlobal>(0xDAD, "OStim.esp");
        OStimRemoveWeaponsWithSlot = dataHandler->LookupForm<RE::TESGlobal>(0xDAE, "OStim.esp");
        OStimAnimateRedress = dataHandler->LookupForm<RE::TESGlobal>(0xDAF, "OStim.esp");
        OStimUsePapyrusUndressing = dataHandler->LookupForm<RE::TESGlobal>(0xDB0, "OStim.esp");

        maleExcitementMultSetting = dataHandler->LookupForm<RE::TESGlobal>(0xDA2, "OStim.esp");
        femaleExcitementMultSetting = dataHandler->LookupForm<RE::TESGlobal>(0xDA3, "OStim.esp");
    }

    void MCMTable::resetDefaults() {
        undressingMask = 0x8D;
    }


    bool MCMTable::isScalingDisabled() {
        return disableScalingSetting->value != 0;
    }

    bool MCMTable::isSchlongBendingDisabled() {
        return disableSchlongBendingSetting->value != 0;
    }


    bool MCMTable::undressAtStart() {
        return OStimUndressAtStart->value != 0;
    }

    bool MCMTable::removeWeaponsAtStart() {
        return OStimRemoveWeaponsAtStart->value != 0;
    }

    bool MCMTable::undressMidScene() {
        return OStimUndressMidScene->value != 0;
    }

    bool MCMTable::partialUndressing() {
        return OStimPartialUndressing->value != 0;
    }

    uint32_t MCMTable::removeWeaponsWithSlot() {
        // the stripping function will treat this as a slotmask
        // so if we ever want to support multiple slots here in the future we just need to change the return value of this function
        return 1 << static_cast<int>(OStimRemoveWeaponsWithSlot->value - 30);
    }

    bool MCMTable::animateRedress() {
        return OStimAnimateRedress->value != 0;
    }

    bool MCMTable::usePapyrusUndressing() {
        return OStimUsePapyrusUndressing->value != 0;
    }

    uint32_t MCMTable::getUndressingMask() {
        return undressingMask;
    }

    void MCMTable::setUndressingMask(uint32_t mask) {
        undressingMask = mask;
    }


    float MCMTable::getMaleSexExcitementMult() {
        return maleExcitementMultSetting->value;
    }


    float MCMTable::getFemaleSexExcitementMult() {
        return femaleExcitementMultSetting->value;
    }
}