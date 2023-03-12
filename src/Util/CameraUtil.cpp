#include "CameraUtil.h"

namespace CameraUtil {
    void toggleFlyCam() {
        const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
        const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
        if (script) {
            const auto selectedRef = RE::Console::GetSelectedRef();
            script->SetCommand("tfc");
            script->CompileAndRun(selectedRef.get());
            delete script;
        }
    }
}