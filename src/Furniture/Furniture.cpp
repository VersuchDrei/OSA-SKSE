#include "Furniture.h"

#include "FurnitureTable.h"
#include "Graph/LookupTable.h"
#include "Util.h"

namespace Furniture {
    FurnitureType getFurnitureType(RE::TESObjectREFR* object) {
        if (!object || object->IsDisabled()) {
            return FurnitureType::NONE;
        }

        if (object->GetBaseObject()->Is(RE::FormType::Furniture)) {
            if (isFurnitureInUse(object, false)) {
                return FurnitureType::NONE;
            }

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

            auto markers = getMarkers(object);

            if (markers.empty()) {
                return FurnitureType::NONE;
            }

            int chairMarkers = 0;

            for (auto& marker : markers) {
                if (marker.animationType.all(RE::BSFurnitureMarker::AnimationType::kSleep)) {
                    return FurnitureType::BED;
                } else if (marker.animationType.all(RE::BSFurnitureMarker::AnimationType::kSit) && std::abs(marker.offset.z - 34) < 1) {
                    chairMarkers++;
                }
            }

            if (chairMarkers == 1) {
                return FurnitureType::CHAIR;
            } else if (chairMarkers > 1) {
                return FurnitureType::BENCH;
            }
        } else {
            if (FurnitureTable::OStimShelfList->HasForm(object->GetBaseObject()->formID)) {
                return FurnitureType::SHELF;
            }
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

    std::vector<float> getOffset(RE::TESObjectREFR* object) {
        std::vector<float> ret = {0,0,0,0};

        if (!object) {
            return ret;
        }

        if (object->GetBaseObject()->Is(RE::FormType::Furniture)) {
            auto markers = getMarkers(object);

            if (markers.empty()) {
                return ret;
            }

            ret[0] = markers[0].offset.x;
            ret[1] = markers[0].offset.y;
            ret[2] = markers[0].offset.z;
            ret[3] = markers[0].heading;

            switch (getFurnitureType(object)) {
                case BED:
                    if (!object->HasKeyword(FurnitureTable::FurnitureBedRoll)) {
                        ret[1] += 50;
                        ret[2] += 8;
                    }
                    ret[0] = 0;
                    ret[3] += 2 * std::acos(0); // this is pi
                    break;
                case BENCH:
                    ret[0] = 0;
                case CHAIR:
                    ret[2] = 0;
                    break;
                default:
                    break;
            }
        }

        return ret;
    }

    RE::BSTArray<RE::BSFurnitureMarker> Furniture::getMarkers(RE::TESObjectREFR* object) {
        auto root = object->Get3D();
        if (!root) {
            return {};
        }

        auto extra = root->GetExtraData("FRN");
        if (!extra) {
            return {};
        }

        auto node = netimmerse_cast<RE::BSFurnitureMarkerNode*>(extra);
        if (!node) {
            return {};
        }

        return node->markers;
    }

    bool Furniture::isFurnitureInUse(RE::TESObjectREFR* object, bool ignoreReserved) {
        return IsFurnitureInUse(nullptr, 0, object, ignoreReserved);
    }
}