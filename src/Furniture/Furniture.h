#pragma once

namespace Furniture {
    enum FurnitureType {
        NONE = 0,
        BED = 1,
        BENCH = 2,
        CHAIR = 3,
        TABLE = 4,
        SHELF = 5,
        WALL = 6,
        COOKING_POT = 7,
    };

    FurnitureType getFurnitureType(RE::TESObjectREFR* object);
    std::vector<RE::TESObjectREFR*> findFurniture(int actorCount, RE::TESObjectREFR* centerRef, float radius, float sameFloor);
}