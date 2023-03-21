#pragma once

#include "ActorKey.h"

namespace Alignment {
    struct SceneKey {
        std::vector<ActorKey> keys;

        inline bool operator==(SceneKey other) {
            if (keys.size() != other.keys.size()) {
                return false;
            }

            for (int i = 0; i < keys.size(); i++) {
                if (keys[i] != other.keys[i]) {
                    return false;
                }
            }

            return true;
        }

        inline bool operator!=(SceneKey other) {
            if (keys.size() != other.keys.size()) {
                return true;
            }

            for (int i = 0; i < keys.size(); i++) {
                if (keys[i] != other.keys[i]) {
                    return true;
                }
            }

            return false;
        }
    };
}