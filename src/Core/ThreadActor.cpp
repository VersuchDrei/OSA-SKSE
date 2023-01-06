#include "ThreadActor.h"

#include "Util/ActorUtil.h"
#include "Util/FormUtil.h"

namespace OStim {
    ThreadActor::ThreadActor(RE::Actor* actor) : actor{actor} {}

    void ThreadActor::undress() {
        if (undressed) {
            return;
        }

        auto inventory = actor->GetInventory();
        for (const auto& [obj, data] : inventory) {
            auto& [count, entry] = data;

            if (!entry->IsWorn() || !obj->IsArmor() || !FormUtil::canUndress(obj)) {
                continue;
            }

            auto armor = obj->As<RE::TESObjectARMO>();
            if (FormUtil::isWig(actor, armor)) {
                continue;
            }

            undressedMask |= static_cast<uint32_t>(armor->GetSlotMask());

            undressedItems.push_back(armor);
            ActorUtil::unequipItem(actor, obj);
        }

        undressed = true;
    }

    void ThreadActor::undressPartial(uint32_t mask) {
        uint32_t filteredMask = (~undressedMask) & mask;
        if (filteredMask == 0) {
            return;
        }

        auto inventory = actor->GetInventory();
        for (const auto& [obj, data] : inventory) {
            auto& [count, entry] = data;

            if (!entry->IsWorn() || !obj->IsArmor() || !FormUtil::canUndress(obj)) {
                continue;
            }

            auto armor = obj->As<RE::TESObjectARMO>();
            uint32_t armorMask = static_cast<uint32_t>(armor->GetSlotMask());
            if ((filteredMask & armorMask) == 0 || FormUtil::isWig(actor, armor)) {
                continue;
            }

            undressedMask |= armorMask;
            undressedItems.push_back(armor);
            ActorUtil::unequipItem(actor, obj);
        }

        // some slots might not have any items equipped in them
        // so to not check them over and over again we add those to the undressedMask
        undressedMask |= mask;

        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto args = RE::MakeFunctionArguments();
            auto handle = skyrimVM->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(actor->FORMTYPE), actor);
            vm->DispatchMethodCall2(handle, "Actor", "QueueNiNodeUpdate", args, callback);
        }
    }

    void ThreadActor::removeWeapons() {
        if (weaponsRemoved) {
            return;
        }

        rightHand = actor->GetEquippedObject(false);
        leftHand = actor->GetEquippedObject(true);
        ammo = actor->GetCurrentAmmo();

        if (rightHand) {
            ActorUtil::unequipItem(actor, rightHand);
        }
        if (leftHand) {
            ActorUtil::unequipItem(actor, leftHand);
        }
        if (ammo) {
            ActorUtil::unequipItem(actor, ammo);
        }

        weaponsRemoved = true;
    }

    void ThreadActor::redress() {
        if (undressedMask == 0) {
            return;
        }

        for (auto& armor : undressedItems) {
            ActorUtil::equipItemEx(actor, armor);
        }
        undressedItems.clear();
        undressedMask = 0;

        undressed = false;
    }

    void ThreadActor::redressPartial(uint32_t mask) {
        uint32_t filteredMask = undressedMask & mask;
        if (filteredMask == 0) {
            return;
        }

        std::vector<RE::TESObjectARMO*>::iterator it = undressedItems.begin();
        while (it != undressedItems.end()) {
            uint32_t armorMask = static_cast<uint32_t>((*it)->GetSlotMask());
            if ((armorMask & filteredMask) == 0) {
                ++it;
            } else {
                undressedMask &= ~armorMask;
                ActorUtil::equipItemEx(actor, (*it));
                undressedItems.erase(it);
            }
        }

        undressedMask &= ~mask;

        undressed = false;
    }

    void ThreadActor::addWeapons() {
        if (!weaponsRemoved) {
            return;
        }

        if (rightHand) {
            ActorUtil::equipItemEx(actor, rightHand, 1);
        }
        if (leftHand) {
            ActorUtil::equipItemEx(actor, leftHand, 2);
        }
        if (ammo) {
            ActorUtil::equipItemEx(actor, ammo);
        }

        weaponsRemoved = false;
    }
}