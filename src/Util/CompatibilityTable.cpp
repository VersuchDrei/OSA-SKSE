#include "CompatibilityTable.h"

#include "Graph/LookupTable.h"

namespace Compatibility {
    void CompatibilityTable::setupForms() {
        auto dataHandler = RE::TESDataHandler::GetSingleton();

        SexLabActive = dataHandler->LookupForm<RE::BGSKeyword>(0x012B24, "SexLab.esm");
        SexLabNoStrip = dataHandler->LookupForm<RE::BGSKeyword>(0x02F16E, "SexLab.esm");
        SexLabForbid = dataHandler->LookupForm<RE::BGSKeyword>(0x02F6D1, "SexLab.esm");

        if (SexLabNoStrip) {
            Graph::LookupTable::noStripKeywords.push_back(SexLabNoStrip);
        }
    }
}