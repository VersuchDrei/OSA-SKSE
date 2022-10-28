#pragma once

namespace MCM {
    class MCMTable {
    public:
        static void setupForms();

        static bool isScalingDisabled();

    private:
        inline static RE::TESGlobal* disableScalingSetting;
    };
}