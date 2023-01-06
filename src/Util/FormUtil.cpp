#include "FormUtil.h"

#include "Graph/LookupTable.h"

namespace FormUtil {
    bool isWig(RE::Actor* actor, RE::TESObjectARMO* armor) {
        if (!armor || !armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair)) {
            return false;
        }

        auto& weightModel = actor->GetBiped(0);
        if (weightModel) {
            std::set<RE::NiAVObject*> touched;

            for (int i = 0; i < 42; ++i) {
                auto& data = weightModel->objects[i];
                if (data.partClone) {
                    RE::TESForm* bipedArmor = data.item;
                    RE::TESForm* addon = data.addon;
                    RE::NiAVObject* object = data.partClone.get();

                    bool isSame = bipedArmor->formType == RE::TESObjectARMO::FORMTYPE && bipedArmor->formID == armor->formID;
                    if (isSame && !touched.count(object)) {
                        if (data.partClone.get()->HasShaderType(RE::BSShaderMaterial::Feature::kHairTint)) {
                            return true;
                        } else {
                            touched.emplace(object);
                        }
                    }
                }
            }
        }
        
        return false;
    }

    bool canUndress(RE::TESForm* form) {
        return !form->HasKeywordInArray(Graph::LookupTable::noStripKeywords, false);
    }
}