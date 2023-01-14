#include "ThreadActor.h"

#include "Graph/LookupTable.h"
#include "Util/ActorUtil.h"
#include "Util/FormUtil.h"
#include "Util/MCMTable.h"
#include "Util/VectorUtil.h"

namespace OStim {
    ThreadActor::ThreadActor(int threadId, RE::Actor* actor) : threadId{threadId}, actor{actor} {
        scaleBefore = actor->GetReferenceRuntimeData().refScale / 100.0;
        isFemale = actor->GetActorBase()->GetSex() == RE::SEX::kFemale;
        isPlayer = actor == RE::PlayerCharacter::GetSingleton();
    }

    void ThreadActor::initContinue() {
        // for some reason the "this" keyword is not properly giving a pointer to this object when used in the constructor
        // so we do everything that uses pointers in this initContinue method
        if (REL::Module::GetRuntime() == REL::Module::Runtime::AE) {
            auto nioInterface = Graph::LookupTable::getNiTransformInterface();
            if (nioInterface->HasNodeTransformScale(actor, false, isFemale, "NPC", "RSMPlugin")) {
                rmHeight = nioInterface->GetNodeTransformScale(actor, false, isFemale, "NPC", "RSMPlugin");
            }
        } else {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new GetRmHeightCallbackFunctor(this));
                auto args = RE::MakeFunctionArguments(std::move(actor), std::move(isFemale));
                vm->DispatchStaticCall("OSKSE", "GetRmScale", args, callback);
            }
        }

        heelOffset = ActorUtil::getHeelOffset(actor, &heelArmor);
    }

    void ThreadActor::undress() {
        if (MCM::MCMTable::usePapyrusUndressing()) {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new PapyrusUndressCallbackFunctor(this, false));
                auto args = RE::MakeFunctionArguments(std::move(threadId), std::move(actor));
                vm->DispatchStaticCall("OUndress", "Undress", args, callback);
            }
            return;
        }

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

            if (armor == heelArmor) {
                updateHeelArmor(true);
            }
        }

        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto args = RE::MakeFunctionArguments();
            auto handle = skyrimVM->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(actor->FORMTYPE), actor);
            vm->DispatchMethodCall2(handle, "Actor", "QueueNiNodeUpdate", args, callback);
        }

        undressed = true;
    }

    void ThreadActor::undressPartial(uint32_t mask) {
        if (MCM::MCMTable::usePapyrusUndressing()) {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new PapyrusUndressCallbackFunctor(this, false));
                auto args = RE::MakeFunctionArguments(std::move(threadId), std::move(actor), std::move(mask));
                vm->DispatchStaticCall("OUndress", "UndressPartial", args, callback);
            }
            return;
        }

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

            if (armor == heelArmor) {
                updateHeelArmor(true);
            }
        }

        if ((undressedMask & filteredMask) != 0) {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                auto args = RE::MakeFunctionArguments();
                auto handle = skyrimVM->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(actor->FORMTYPE), actor);
                vm->DispatchMethodCall2(handle, "Actor", "QueueNiNodeUpdate", args, callback);
            }
        }

        // some slots might not have any items equipped in them
        // so to not check them over and over again we add those to the undressedMask
        undressedMask |= mask;
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
        if (MCM::MCMTable::usePapyrusUndressing()) {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new PapyrusUndressCallbackFunctor(this, true));
                auto args = RE::MakeFunctionArguments(std::move(threadId), std::move(actor), std::move(undressedItems));
                vm->DispatchStaticCall("OUndress", "Redress", args, callback);
            }
            return;
        }

        if (undressedMask == 0) {
            return;
        }

        for (auto& armor : undressedItems) {
            ActorUtil::equipItemEx(actor, armor);

            if (armor == heelArmor) {
                updateHeelArmor(false);
            }
        }
        undressedItems.clear();
        undressedMask = 0;

        undressed = false;
    }

    void ThreadActor::redressPartial(uint32_t mask) {
        if (MCM::MCMTable::usePapyrusUndressing()) {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback(new PapyrusUndressCallbackFunctor(this, true));
                auto args = RE::MakeFunctionArguments(std::move(threadId), std::move(actor), std::move(undressedItems), std::move(mask));
                vm->DispatchStaticCall("OUndress", "RedressPartial", args, callback);
            }
            return;
        }

        uint32_t filteredMask = undressedMask & mask;
        if (filteredMask == 0) {
            return;
        }

        std::vector<RE::TESObjectARMO*>::iterator it = undressedItems.begin();
        while (it != undressedItems.end()) {
            RE::TESObjectARMO* armor = *it;
            uint32_t armorMask = static_cast<uint32_t>(armor->GetSlotMask());
            if ((armorMask & filteredMask) == 0) {
                ++it;
            } else {
                undressedMask &= ~armorMask;
                ActorUtil::equipItemEx(actor, armor);

                if (armor == heelArmor) {
                    updateHeelArmor(false);
                }

                undressedItems.erase(it);
            }
        }

        if ((undressedMask & filteredMask) != filteredMask) {
            undressed = false;
        }
        undressedMask &= ~mask;
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

    void ThreadActor::changeNode(Graph::Actor* graphActor) {
        this->graphActor = graphActor;

        checkHeelOffset();
        scale();
    }

    void ThreadActor::scale() {
        if (MCM::MCMTable::isScalingDisabled()) {
            return;
        }

        float newScale = graphActor->scale / (actor->GetActorBase()->GetHeight() * rmHeight);
        if (!heelOffsetRemoved && heelOffset != 0) {
            newScale *= graphActor->scaleHeight / (graphActor->scaleHeight + heelOffset);
        }

        // setscale resets 3BA physics, so we don't do it if the actor already has the desired scale
        if (static_cast<int>(newScale * 100) != actor->GetReferenceRuntimeData().refScale) {
            ActorUtil::setScale(actor, newScale);
        }
    }

    void ThreadActor::checkHeelOffset() {
        if (heelOffset == 0 || heelArmorRemoved || !graphActor || graphActor->feetOnGround != heelOffsetRemoved) {
            return;
        }

        updateHeelOffset(!graphActor->feetOnGround);
    }

    void ThreadActor::updateHeelOffset(bool remove) {
        if (heelOffsetRemoved == remove) {
            return;
        }

        // the NiTransformInterface has only been added to RaceMenu after the AE update
        // so for SE we have to invoke Papyrus here :^(
        if (REL::Module::GetRuntime() == REL::Module::Runtime::AE) {
            auto nioInterface = Graph::LookupTable::getNiTransformInterface();
            if (remove) {
                // we are adding a second node transform with a different key to counter out the existing one, thereby "removing" the heel offset
                SKEE::INiTransformInterface::Position offset{};
                offset.z = -heelOffset;
                nioInterface->AddNodeTransformPosition(actor, false, isFemale, "NPC", "OStim", offset);
            } else {
                nioInterface->RemoveNodeTransformPosition(actor, false, isFemale, "NPC", "OStim");
            }
            nioInterface->UpdateNodeTransforms(actor, false, isFemale, "NPC");
        } else {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                auto args = RE::MakeFunctionArguments(std::move(actor), std::move(heelOffset), std::move(remove), std::move(isFemale));
                vm->DispatchStaticCall("OSKSE", "UpdateHeelOffset", args, callback);
            }
        }

        heelOffsetRemoved = remove;
    }

    void ThreadActor::updateHeelArmor(bool remove) {
        if (heelArmorRemoved == remove) {
            return;
        }

        heelArmorRemoved = remove;

        if (remove) {
            if (heelOffsetRemoved) {
                updateHeelOffset(false);
            } else {
                scale();
            }
        } else {
            if (graphActor->feetOnGround) {
                scale();
            } else {
                updateHeelOffset(remove);
            }
        }
    }

    void ThreadActor::free() {
        if (MCM::MCMTable::animateRedress() && !isPlayer) {
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                std::vector<RE::TESForm*> weapons = {rightHand, leftHand, ammo};
                auto args = RE::MakeFunctionArguments(std::move(actor), std::move(isFemale), std::move(undressedItems), std::move(weapons));
                vm->DispatchStaticCall("OUndress", "AnimateRedress", args, callback);
            }
        } else {
            if (MCM::MCMTable::usePapyrusUndressing()) {
                // this object will be destroyed before papyrus redressing is done
                // so for this case we need to invoke Redress without a callback here
                const auto skyrimVM = RE::SkyrimVM::GetSingleton();
                auto vm = skyrimVM ? skyrimVM->impl : nullptr;
                if (vm) {
                    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                    auto args = RE::MakeFunctionArguments(std::move(threadId), std::move(actor), std::move(undressedItems));
                    vm->DispatchStaticCall("OUndress", "Redress", args, callback);
                }
            } else {
                redress();
            }
            addWeapons();
        }
        updateHeelOffset(false);
        ActorUtil::setScale(actor, scaleBefore);
    }

    void ThreadActor::papyrusUndressCallback(std::vector<RE::TESObjectARMO*> items) {
        for (RE::TESObjectARMO* item : items) {
            if (VectorUtil::contains(undressedItems, item)) {
                continue;
            }

            undressedItems.push_back(item);
            if (item == heelArmor) {
                updateHeelArmor(true);
            }
        }
    }

    void ThreadActor::papyrusRedressCallback(std::vector<RE::TESObjectARMO*> items) {
        if (VectorUtil::contains(items, heelArmor) && VectorUtil::contains(undressedItems, heelArmor)) {
            updateHeelArmor(false);
        }

        std::erase_if(undressedItems, [items](RE::TESObjectARMO* item){return VectorUtil::contains(items, item);});
    }

    void ThreadActor::GetRmHeightCallbackFunctor::setRmHeight(float height) {
        threadActor->rmHeight = height;
        float currentScale = threadActor->actor->GetReferenceRuntimeData().refScale / 100.0;
        ActorUtil::setScale(threadActor->actor, currentScale / height);
    }
}