#pragma once

namespace MCM {
    class MCMTable {
    public:
        static void setupForms();

        static bool isScalingDisabled();
        static bool isSchlongBendingDisabled();

        static float getMaleSexExcitementMult();
        static float getFemaleSexExcitementMult();

    private:
        inline static RE::TESGlobal* disableScalingSetting;
        inline static RE::TESGlobal* disableSchlongBendingSetting;

        inline static RE::TESGlobal* maleExcitementMultSetting;
        inline static RE::TESGlobal* femaleExcitementMultSetting;
    };
}