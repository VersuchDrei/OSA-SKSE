#pragma once

namespace MCM {
    class MCMTable {
    public:
        static void setupForms();
        static void resetDefaults();

        static bool isScalingDisabled();
        static bool isSchlongBendingDisabled();

        static bool undressAtStart();
        static bool removeWeaponsAtStart();
        static bool undressMidScene();
        static bool partialUndressing();
        static uint32_t removeWeaponsWithSlot();
        static bool animateRedress();
        static bool usePapyrusUndressing();
        static uint32_t getUndressingMask();
        static void setUndressingMask(uint32_t mask);

        static float getMaleSexExcitementMult();
        static float getFemaleSexExcitementMult();

    private:
        inline static RE::TESGlobal* disableScalingSetting;
        inline static RE::TESGlobal* disableSchlongBendingSetting;

        inline static RE::TESGlobal* OStimUndressAtStart;
        inline static RE::TESGlobal* OStimRemoveWeaponsAtStart;
        inline static RE::TESGlobal* OStimUndressMidScene;
        inline static RE::TESGlobal* OStimPartialUndressing;
        inline static RE::TESGlobal* OStimRemoveWeaponsWithSlot;
        inline static RE::TESGlobal* OStimAnimateRedress;
        inline static RE::TESGlobal* OStimUsePapyrusUndressing;
        inline static uint32_t undressingMask = 0x3D8BC39D;

        inline static RE::TESGlobal* maleExcitementMultSetting;
        inline static RE::TESGlobal* femaleExcitementMultSetting;
    };
}