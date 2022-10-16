#include "Graph/Node.h"

#include "Graph/LookupTable.h"
#include "SKEE.h"

namespace Graph {
    void Node::scaleActor(RE::Actor* actor, int position) {
        if (position < 0 || position >= actors.size()) {
            return;
        }

        float newScale = actors[position]->scale / (actor->GetScale() / (static_cast<float>(actor->GetReferenceRuntimeData().refScale) / 100.0f));
        if (actors[position]->feetOnGround) {
            // the NiTransformInterface has only been added to RaceMenu after the AE update
            // so for SE we have to invoke Papyrus here :^(
            if (REL::Module::GetRuntime() == REL::Module::Runtime::AE) {
                bool isFemale = actor->GetActorBase()->GetSex() == RE::SEX::kFemale;
                auto nioInterface = Graph::LookupTable::GetNiTransformInterface();
                if (nioInterface->HasNodeTransformPosition(actor, false, isFemale, "NPC", "internal")) {
                    float offset = nioInterface->GetNodeTransformPosition(actor, false, isFemale, "NPC", "internal").z;
                    newScale *= actors[position]->scaleHeight / (actors[position]->scaleHeight + offset);
                }
            } else {
                const auto skyrimVM = RE::SkyrimVM::GetSingleton();
                auto vm = skyrimVM ? skyrimVM->impl : nullptr;
                if (vm) {
                    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                    float height = actors[position]->scaleHeight;
                    auto args = RE::MakeFunctionArguments(std::move(actor), std::move(newScale), std::move(height));
                    vm->DispatchStaticCall("OSANative", "ScaleActorInner", args, callback);
                }
                return;
            }
        }

        const auto skyrimVM = RE::SkyrimVM::GetSingleton();
        auto vm = skyrimVM ? skyrimVM->impl : nullptr;
        if (vm) {
            RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
            auto args = RE::MakeFunctionArguments(std::move(newScale));
            auto handle = RE::SkyrimVM::GetSingleton()->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(actor->FORMTYPE), actor);
            vm->DispatchMethodCall2(handle, "Actor", "SetScale", args, callback);
        }
    }

    void Node::updateActors(std::vector<RE::Actor*> reActors, std::vector<float> offsets) {
        int count = std::min(actors.size(), reActors.size());
        for (int i = 0; i < count; i++) {
            // benis bending
            reActors[i]->NotifyAnimationGraph("SOSBend" + actors[i]->penisAngle);

            // scaling
            float newScale = actors[i]->scale / (reActors[i]->GetScale() / (static_cast<float>(reActors[i]->GetReferenceRuntimeData().refScale) / 100.0f));
            if (actors[i]->feetOnGround && offsets[i] != 0) {
                newScale *= actors[i]->scaleHeight / (actors[i]->scaleHeight + offsets[i]);
            }

            // TODO: RE Actor::SetScale
            const auto skyrimVM = RE::SkyrimVM::GetSingleton();
            auto vm = skyrimVM ? skyrimVM->impl : nullptr;
            if (vm) {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                auto args = RE::MakeFunctionArguments(std::move(newScale));
                auto handle = RE::SkyrimVM::GetSingleton()->handlePolicy.GetHandleForObject(static_cast<RE::VMTypeID>(reActors[i]->FORMTYPE), reActors[i]);
                vm->DispatchMethodCall2(handle, "Actor", "SetScale", args, callback);
            }

            // heel offsets
            if (offsets[i] != 0) {
                // the NiTransformInterface has only been added to RaceMenu after the AE update
                // so for SE we have to invoke Papyrus here :^(
                if (REL::Module::GetRuntime() == REL::Module::Runtime::AE) {
                    bool isFemale = reActors[i]->GetActorBase()->GetSex() == RE::SEX::kFemale;
                    auto nioInterface = Graph::LookupTable::GetNiTransformInterface();
                    bool hasOffset = nioInterface->HasNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal");
                    if (actors[i]->feetOnGround) {
                        if (!hasOffset) {
                            SKEE::INiTransformInterface::Position offset{};
                            offset.z = offsets[i];
                            nioInterface->AddNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal", offset);
                            nioInterface->UpdateNodeTransforms(reActors[i], false, isFemale, "NPC");
                        }
                    } else {
                        if (hasOffset) {
                            nioInterface->RemoveNodeTransformPosition(reActors[i], false, isFemale, "NPC", "internal");
                            nioInterface->UpdateNodeTransforms(reActors[i], false, isFemale, "NPC");
                        }
                    }
                } else {
                    if (vm) {
                        RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback;
                        float height = actors[i]->scaleHeight;
                        auto args = RE::MakeFunctionArguments(std::move(reActors[i]), std::move(actors[i]->feetOnGround), std::move(offsets[i]));
                        vm->DispatchStaticCall("OSANative", "CheckOffset", args, callback);
                    }
                }
            }
        }
    }
}