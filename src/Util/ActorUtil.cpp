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

    float getHeelOffset(RE::Actor* actor, RE::TESObjectARMO** heelArmor) {
        auto& weightModel = actor->GetBiped(0);
        if (weightModel) {
            std::set<RE::NiAVObject*> touched;

            for (int i = 0; i < 42; ++i) {
                auto& data = weightModel->objects[i];
                if (data.partClone) {
                    RE::TESForm* bipedArmor = data.item;

                    // some, but not all, weapon meshes CTD if you check them for "HH_OFFSET", so we skip those
                    if (bipedArmor->formType == RE::TESObjectWEAP::FORMTYPE || bipedArmor->formType == RE::TESAmmo::FORMTYPE){
                        continue;
                    }

                    RE::NiAVObject* object = data.partClone.get();
                    
                    if (!touched.count(object)) {
                        RE::NiNode* node = data.partClone.get()->AsNode();

                        if (node) {
                            for (auto& child : node->GetChildren()) {
                                if (child->HasExtraData("HH_OFFSET")) {
                                    auto hh_offset = child->GetExtraData<RE::NiFloatExtraData>("HH_OFFSET");
                                    if (hh_offset) {
                                        if (bipedArmor->formType == RE::TESObjectARMO::FORMTYPE) {
                                            RE::TESObjectARMO* armor = bipedArmor->As<RE::TESObjectARMO>();
                                            *heelArmor = armor;
                                        }
                                        return hh_offset->value;
                                    }
                                } else if (child->HasExtraData("SDTA")) {
                                    auto sdta = child->GetExtraData<RE::NiStringExtraData>("SDTA");
                                    if (sdta) {
                                        json json = json::parse(sdta->value, nullptr, false);

                                        if (!json.is_discarded()) {
                                            for (auto& element : json) {
                                                if (element.contains("name") && element["name"] == "NPC" && element.contains("pos")) {
                                                    if (bipedArmor->formType == RE::TESObjectARMO::FORMTYPE) {
                                                        RE::TESObjectARMO* armor = bipedArmor->As<RE::TESObjectARMO>();
                                                        *heelArmor = armor;
                                                    }
                                                    return element["pos"][2];
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        touched.emplace(object);
                    }
                }
            }
        }

        return 0;
    }
}