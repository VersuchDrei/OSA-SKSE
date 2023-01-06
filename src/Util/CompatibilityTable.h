#pragma once

namespace Compatibility {
    class CompatibilityTable {
    public:
        inline static RE::BGSKeyword* SexLabActive;
        inline static RE::BGSKeyword* SexLabNoStrip;
        inline static RE::BGSKeyword* SexLabForbid;

        static void setupForms();
    };
}