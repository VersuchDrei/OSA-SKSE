#pragma once

namespace ActorUtil {
    inline void setScale(RE::Actor* actor, float scale) {
        using func_t = decltype(setScale);
        REL::Relocation<func_t> func{RELOCATION_ID(19239, 19665)};
        func(actor, scale);
    }

    inline void UnequipItem(RE::BSScript::IVirtualMachine* vm, RE::VMStackID stackID, RE::Actor* actor, RE::TESForm* akItem, bool abPreventEquip, bool abSilent) {
        using func_t = decltype(UnequipItem);
        REL::Relocation<func_t> func{RELOCATION_ID(53950, 54774)};
        func(vm, stackID, actor, akItem, abPreventEquip, abSilent);
    }

    void unequipItem(RE::Actor* actor, RE::TESForm* item, bool preventEquip, bool silent);
    void unequipItem(RE::Actor* actor, RE::TESForm* item);

    void equipItemEx(RE::Actor* actor, RE::TESForm* item, int slotId, bool preventUnequip, bool equipSound);
    void equipItemEx(RE::Actor* actor, RE::TESForm* item, int slotId);
    void equipItemEx(RE::Actor* actor, RE::TESForm* item);

    float getHeelOffset(RE::Actor* actor, RE::TESObjectARMO** heelArmor);
}