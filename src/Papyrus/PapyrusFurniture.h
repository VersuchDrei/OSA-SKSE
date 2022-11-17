#pragma once

#include "Furniture/Furniture.h"
#include "Furniture/FurnitureTable.h"

namespace PapyrusFurniture {
    using VM = RE::BSScript::IVirtualMachine;

    int GetFurnitureType(RE::StaticFunctionTag*, RE::TESObjectREFR* furnitureRef) {
        return Furniture::getFurnitureType(furnitureRef);
    }

    int ToFurnitureTypeInt(RE::StaticFunctionTag*, std::string furnitureType) {
        return Furniture::FurnitureTable::getFurnitureType(furnitureType);
    }

    std::vector<RE::TESObjectREFR*> FindFurniture(RE::StaticFunctionTag*, int actorCount, RE::TESObjectREFR* centerRef, float radius, float sameFloor) {
        return Furniture::findFurniture(actorCount, centerRef, radius, sameFloor);
    }

    bool Bind(VM* a_vm) {
        const auto obj = "OFurniture"sv;

        BIND(GetFurnitureType);
        BIND(ToFurnitureTypeInt);
        BIND(FindFurniture);

        return true;
    }
}