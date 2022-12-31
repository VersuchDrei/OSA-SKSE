#pragma once

namespace ObjectRefUtil {
    int getMotionType(RE::TESObjectREFR* object);
    bool isInBBLS(RE::TESObjectREFR* object);
    void stopTranslation(RE::TESObjectREFR* object);
    void translateTo(RE::TESObjectREFR* object, float x, float y, float z, float angleX, float angleY, float angleZ, float speed, float maxRotationSpeed);

    inline void StopTranslation(RE::BSScript::IVirtualMachine* vm, RE::VMStackID stackID, RE::TESObjectREFR* object) {
        using func_t = decltype(StopTranslation);
        REL::Relocation<func_t> func{RELOCATION_ID(55712, 56243)};
        func(vm, stackID, object);
    }

    inline void TranslateTo(RE::BSScript::IVirtualMachine* vm, RE::VMStackID stackID, RE::TESObjectREFR* object, float afX, float afY, float afZ, float afAngleX, float afAngleY, float afAngleZ, float afSpeed, float afMaxRotationSpeed) {
        using func_t = decltype(TranslateTo);
        REL::Relocation<func_t> func{RELOCATION_ID(55706, 56237)};
        func(vm, stackID, object, afX, afY, afZ, afAngleX, afAngleY, afAngleZ, afSpeed, afMaxRotationSpeed);
    }

}