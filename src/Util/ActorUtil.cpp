#include "ActorUtil.h"

namespace ActorUtil {
    void unequipItem(RE::Actor* actor, RE::TESForm* item, bool preventEquip, bool silent) {
        UnequipItem(nullptr, 0, actor, item, preventEquip, silent);
    }

    void unequipItem(RE::Actor* actor, RE::TESForm* item) {
        UnequipItem(nullptr, 0, actor, item, false, false);
    }

    void equipItemEx(RE::Actor* actor, RE::TESForm* item, int slotId, bool preventUnequip, bool equipSound) {
        //TODO: reimplement SKSE function here
        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto handle = skyrimVM->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(actor->FORMTYPE), actor);
            auto args = RE::MakeFunctionArguments(std::move(item), std::move(slotId), std::move(preventUnequip), std::move(equipSound));
            vm->DispatchMethodCall2(handle, "Actor", "EquipItemEx", args, callback);
        }
    }

    void equipItemEx(RE::Actor* actor, RE::TESForm* item, int slotId) {
        equipItemEx(actor, item, slotId, false, true);
    }

    void equipItemEx(RE::Actor* actor, RE::TESForm* item) {
        equipItemEx(actor, item, 0, false, true);
    }
}