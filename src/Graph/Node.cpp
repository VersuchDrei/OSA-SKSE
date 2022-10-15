#include "Graph/Node.h"

#include "Graph/LookupTable.h"
#include "SKEE.h"

namespace Graph {
    void Node::scaleActor(RE::Actor* actor, int position) {
        if (position < 0 || position >= actors.size()) {
            return;
        }

        float newScale = actors[position]->scale / (actor->GetScale() / (static_cast<float>(actor->GetReferenceRuntimeData().refScale) / 100.0f));
        if (actors[position]->heelScaling) {
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
}