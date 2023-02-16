#include "EquipObjectHandler.h"

#include "Util/ActorUtil.h"
#include "Util/Constants.h"
#include "Util/ObjectRefUtil.h"

namespace OStim {
    void EquipObjectHandler::loop() {
        if (variantDuration > 0) {
            variantDuration -= Constants::LOOP_TIME_MILLISECONDS;

            if (variantDuration <= 0) {
                unsetVariant();
            }
        }
    }

    void EquipObjectHandler::equip() {
        if (equipped) {
            return;
        }

        equipInner();
    }

    void EquipObjectHandler::unequip() {
        if (!equipped) {
            return;
        }

        unequipInner();
    }

    void EquipObjectHandler::setVariant(std::string variant, int duration) {
        if (this->variant == variant) {
            if (duration == 0) {
                variantDuration = 0;
            } else if (variantDuration != 0 && duration > variantDuration) {
                variantDuration = duration;
            }
            return;
        }

        if (!object->variants.contains(variant)) {
            return;
        }

        this->variant = variant;
        variantDuration = duration;

        if (equipped) {
            unequipInner();
            equipInner();
        }
    }

    void EquipObjectHandler::unsetVariant() {
        if (variant.empty()) {
            return;
        }

        variant = "";
        variantDuration = 0;

        if (equipped) {
            unequipInner();
            equipInner();
        }
    }

    void EquipObjectHandler::equipInner() {
        equipped = !variant.empty() ? object->variants[variant] : object->item;
        
        ObjectRefUtil::addItem(actor, equipped);
        ActorUtil::equipItem(actor, equipped);

        // if we don't add a slight delay here we end up with a race condition where the tongue sometimes doesn't appear
        RE::Actor* temp = actor;
        std::thread ninodeThread = std::thread([temp] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ActorUtil::queueNiNodeUpdate(temp);
        });
        ninodeThread.detach();
    }

    void EquipObjectHandler::unequipInner() {
        ActorUtil::unequipItem(actor, equipped);
        ObjectRefUtil::removeItem(actor, equipped, 1, true, nullptr);

        equipped = nullptr;

        ActorUtil::queueNiNodeUpdate(actor);
    }
}