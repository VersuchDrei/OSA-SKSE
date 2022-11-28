#include "FurnitureTable.h"

#include "Util/StringUtil.h"

namespace Furniture {
    void FurnitureTable::setupForms() {
        auto handler = RE::TESDataHandler::GetSingleton();
        CraftingCookPot = handler->LookupForm<RE::BGSKeyword>(0x000a5cb3, "Skyrim.esm");
        FurnitureCounterLeanMarker = handler->LookupForm<RE::BGSKeyword>(0x00088106, "Skyrim.esm");
        FurnitureBedRoll = handler->LookupForm<RE::BGSKeyword>(0x000e4ad6, "Skyrim.esm");
        isLeanTable = handler->LookupForm<RE::BGSKeyword>(0x000c4ef2, "Skyrim.esm");
        IsTable = handler->LookupForm<RE::BGSKeyword>(0x0009b9a9, "Skyrim.esm");
        isWritingChair = handler->LookupForm<RE::BGSKeyword>(0x000eaa4b, "Skyrim.esm");
        RaceToScale = handler->LookupForm<RE::BGSKeyword>(0x000FD0E1, "Skyrim.esm");
        WICraftingAlchemy = handler->LookupForm<RE::BGSKeyword>(0x0004f6e6, "Skyrim.esm");
        WICraftingEnchanting = handler->LookupForm<RE::BGSKeyword>(0x0004f6dd, "Skyrim.esm");

        WallLeanMarker = handler->LookupForm<RE::TESFurniture>(0x00052ff5, "Skyrim.esm");

        OStimShelfList = handler->LookupForm<RE::BGSListForm>(0x00000da0, "OStim.esp");
    }

    FurnitureType FurnitureTable::getFurnitureType(std::string type) {
        StringUtil::toLower(&type);

        auto iter = furnitures.find(type);
        if (iter != furnitures.end()) {
            return iter->second;
        }

        return FurnitureType::NONE;
    }
}