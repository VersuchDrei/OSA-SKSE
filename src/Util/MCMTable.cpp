#include "MCMTable.h"

namespace MCM {
    void MCMTable::setupForms() {
        auto dataHandler = RE::TESDataHandler::GetSingleton();

        maleExcitementMultSetting = dataHandler->LookupForm<RE::TESGlobal>(0xDA2, "OStim.esp");
        femaleExcitementMultSetting = dataHandler->LookupForm<RE::TESGlobal>(0xDA3, "OStim.esp");
        OStimExcitementDecayRate = dataHandler->LookupForm<RE::TESGlobal>(0xDB5, "OStim.esp");
        OStimExcitementDecayGracePeriod = dataHandler->LookupForm<RE::TESGlobal>(0xDB4, "OStim.esp");

        disableScalingSetting = dataHandler->LookupForm<RE::TESGlobal>(0xD94, "OStim.esp");
        disableSchlongBendingSetting = dataHandler->LookupForm<RE::TESGlobal>(0xD97, "OStim.esp");

        OStimUndressAtStart = dataHandler->LookupForm<RE::TESGlobal>(0xDAA, "OStim.esp");
        OStimRemoveWeaponsAtStart = dataHandler->LookupForm<RE::TESGlobal>(0xDAB, "OStim.esp");
        OStimUndressMidScene = dataHandler->LookupForm<RE::TESGlobal>(0xDAC, "OStim.esp");
        OStimPartialUndressing = dataHandler->LookupForm<RE::TESGlobal>(0xDAD, "OStim.esp");
        OStimRemoveWeaponsWithSlot = dataHandler->LookupForm<RE::TESGlobal>(0xDAE, "OStim.esp");
        OStimAnimateRedress = dataHandler->LookupForm<RE::TESGlobal>(0xDAF, "OStim.esp");
        OStimUsePapyrusUndressing = dataHandler->LookupForm<RE::TESGlobal>(0xDB0, "OStim.esp");

        OStimExpressionDurationMin = dataHandler->LookupForm<RE::TESGlobal>(0xDB2, "OStim.esp");
        OStimExpressionDurationMax = dataHandler->LookupForm<RE::TESGlobal>(0xDB3, "OStim.esp");
    }

    void MCMTable::resetDefaults() {
        undressingMask = 0x3D8BC39D;
        doPapyrusUndressing = false;
    }

    float MCMTable::getMaleSexExcitementMult() {
        return maleExcitementMultSetting->value;
    }


    float MCMTable::getFemaleSexExcitementMult() {
        return femaleExcitementMultSetting->value;
    }

    float MCMTable::getExcitementDecayRate() {
        return OStimExcitementDecayRate->value;
    }

    int MCMTable::getExcitementDecayGracePeriod() {
        return OStimExcitementDecayGracePeriod->value;
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

    uint32_t MCMTable::getUndressingMask() {
        return undressingMask;
    }

    void MCMTable::setUndressingMask(uint32_t mask) {
        undressingMask = mask;
    }

    void MCMTable::setPapyusUndressing(bool doPapyrus) {
        doPapyrusUndressing = doPapyrus;
        OStimUsePapyrusUndressing->value = doPapyrus ? 1 : 0;
        logger::info("papyrus undressing is {}", doPapyrus ? "enabled" : "disabled");
    }

    bool MCMTable::usePapyrusUndressing() {
        return doPapyrusUndressing;
    }


    int MCMTable::getExpressionDurationMin() {
        return OStimExpressionDurationMin->value;
    }

    int MCMTable::getExpressionDurationMax() {
        return OStimExpressionDurationMax->value;
    }
}