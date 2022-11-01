#pragma once

namespace ActorUtil {
    inline void setScale(RE::Actor* actor, float scale) {
        using func_t = decltype(setScale);
        REL::Relocation<func_t> setScaleInner{RELOCATION_ID(19239, 19665)};
        setScaleInner(actor, scale);
    }
}