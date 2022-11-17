#include "Furniture.h"

#include "FurnitureTable.h"
#include "Graph/LookupTable.h"
#include "Util.h"

namespace Furniture {
    FurnitureType getFurnitureType(RE::TESObjectREFR* object) {
        if (!object) {
            return FurnitureType::NONE;
        }

        if (object->GetBaseObject()->Is(RE::FormType::Furniture)) {
            // TODO: RE IsFurnitureInUse

            if (object->HasKeyword(FurnitureTable::WICraftingAlchemy) || object->HasKeyword(FurnitureTable::WICraftingEnchanting) || object->HasKeyword(FurnitureTable::isLeanTable)) {
                return FurnitureType::TABLE;
            }

            if (object->HasKeyword(FurnitureTable::CraftingCookPot)) {
                return FurnitureType::COOKING_POT;
            }

            if (object->HasKeyword(FurnitureTable::IsTable) || object->HasKeyword(FurnitureTable::isWritingChair) || object->HasKeyword(FurnitureTable::FurnitureCounterLeanMarker)) {
                // chairs that are part of a table, or not a chair at all?!
                return FurnitureType::NONE;
            }

            if (object->GetBaseObject() == FurnitureTable::WallLeanMarker) {
                return FurnitureType::WALL;
            }

            auto root = object->Get3D();
            if (!root) {
                return FurnitureType::NONE;
            }

            auto extra = root->GetExtraData("FRN");
            if (!extra) {
                return FurnitureType::NONE;
            }

            auto node = netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra);
            if (!node) {
                return FurnitureType::NONE;
            }

            if (node->markers.empty()) {
                return FurnitureType::NONE;
            }

            int chairMarkers = 0;

            for (auto& marker : node->markers) {
                if (marker.animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep)) {
                    return FurnitureType::BED;
                } else if (marker.animationType.all(RE::BSFurnitureMarker::AnimationType::kSit)) {
                    chairMarkers++;
                }
            }

            if (chairMarkers == 1) {
                return FurnitureType::CHAIR;
            } else if (chairMarkers > 1) {
                return FurnitureType::BENCH;
            }
        } else {
            // TODO: check formlists
        }

        return FurnitureType::NONE;
    }

    std::vector<RE::TESObjectREFR*> findFurniture(int actorCount, RE::TESObjectREFR* centerRef, float radius, float sameFloor) {
        std::vector<RE::TESObjectREFR*> ret(7);
        if (!centerRef) {
            return ret;
        }

        auto centerPos = centerRef->GetPosition();

        util::iterate_attached_cells(centerPos, radius, [&](RE::TESObjectREFR& ref) {
            auto refPos = ref.GetPosition();



            if (sameFloor == 0.0 || std::fabs(centerPos.z - refPos.z) <= sameFloor) {
                FurnitureType type = getFurnitureType(&ref);
                if (type == FurnitureType::NONE || type != FurnitureType::BED && !Graph::LookupTable::hasNodes(type, actorCount)) {
                    return RE::BSContainer::ForEachResult::kContinue;
                }

                int index = type - 1;
                if (!ret[index] || centerPos.GetDistance(refPos) < centerPos.GetDistance(ret[index]->GetPosition())) {
                    ret[index] = &ref;
                }
            }

            return RE::BSContainer::ForEachResult::kContinue;
        });

        return ret;
    }
}