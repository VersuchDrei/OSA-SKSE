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
    bool isFurnitureInUse(RE::TESObjectREFR* object, bool ignoreReserved);

    inline bool IsFurnitureInUse(RE::BSScript::IVirtualMachine* vm, RE::VMStackID stackID, RE::TESObjectREFR* self, bool ignoreReserved) {
        using func_t = decltype(IsFurnitureInUse);
        REL::Relocation<func_t> funct{RELOCATION_ID(55661, 56192)};
        return funct(vm, stackID, self, ignoreReserved);
    }
}