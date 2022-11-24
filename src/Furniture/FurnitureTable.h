#pragma once

#include "Furniture.h"

namespace Furniture {
    class FurnitureTable {
    public:
        inline static RE::BGSKeyword* CraftingCookPot;
        inline static RE::BGSKeyword* FurnitureCounterLeanMarker;
        inline static RE::BGSKeyword* FurnitureBedRoll;
        inline static RE::BGSKeyword* isLeanTable;
        inline static RE::BGSKeyword* IsTable;
        inline static RE::BGSKeyword* isWritingChair;
        inline static RE::BGSKeyword* RaceToScale;
        inline static RE::BGSKeyword* WICraftingAlchemy;
        inline static RE::BGSKeyword* WICraftingEnchanting;

        inline static RE::TESFurniture* WallLeanMarker;
        inline static RE::BGSListForm* OStimShelfList;

        static void setupForms();
        static FurnitureType getFurnitureType(std::string type);

    private:
        inline static std::unordered_map<std::string, FurnitureType> furnitures{
            {"", FurnitureType::NONE},
            {"bed", FurnitureType::BED},
            {"bench", FurnitureType::BENCH},
            {"chair", FurnitureType::CHAIR},
            {"table", FurnitureType::TABLE},
            {"shelf", FurnitureType::SHELF},
            {"wall", FurnitureType::WALL},
            {"cookingpot", FurnitureType::COOKING_POT}};
    };
}