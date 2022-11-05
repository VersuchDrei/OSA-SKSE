#pragma once

namespace MCM {
    class MCMTable {
    public:
        static void setupForms();

        static bool isScalingDisabled();
        static bool isSchlongAnglingDisabled();

    private:
        inline static RE::TESGlobal* disableScalingSetting;
        inline static RE::TESGlobal* disableSchlongAnglingSetting;
    };
}