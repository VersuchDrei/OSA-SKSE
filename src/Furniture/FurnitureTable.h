#pragma once

#include "Furniture.h"

namespace Furniture {
    class FurnitureTable {
    public:
        inline static RE::BGSKeyword* CraftingCookPot;
        inline static RE::BGSKeyword* FurnitureCounterLeanMarker;
        inline static RE::BGSKeyword* isLeanTable;
        inline static RE::BGSKeyword* IsTable;
        inline static RE::BGSKeyword* isWritingChair;
        inline static RE::BGSKeyword* RaceToScale;
        inline static RE::BGSKeyword* WICraftingAlchemy;
        inline static RE::BGSKeyword* WICraftingEnchanting;

        inline static RE::TESFurniture* WallLeanMarker;

        static void setupForms();
        static FurnitureType getFurnitureType(std::string type);

    private:
        inline static std::unordered_map<std::string, FurnitureType> furnitures{
            {"", FurnitureType::NONE},
            {"bed", FurnitureType::BED},
            {"chair", FurnitureType::CHAIR},
            {"bench", FurnitureType::BENCH},
            {"shelf", FurnitureType::SHELF},
            {"table", FurnitureType::TABLE},
            {"cookingpot", FurnitureType::COOKING_POT}};
    };
}