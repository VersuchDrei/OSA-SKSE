#pragma once

#include "Trait/EquipObject.h"

namespace OStim {
    struct EquipObjectHandler {
    public:
        RE::Actor* actor;
        Trait::EquipObject* object;
        RE::TESObjectARMO* equipped = nullptr;
        int variantDuration = 0;
        std::string variant = "";

        void loop();
        void equip();
        void unequip();
        void setVariant(std::string variant, int duration);
        void unsetVariant();

        inline void setVariant(std::string variant) {
            setVariant(variant, 0);
        }

    private:
        void equipInner();
        void unequipInner();
    };
}