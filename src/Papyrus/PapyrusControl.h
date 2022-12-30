#pragma once

#include "Core/ThreadManager.h"

namespace PapyrusControl {
    using VM = RE::BSScript::IVirtualMachine;

    RE::GPtr<RE::IMenu> GetMenu() {
        auto ui = RE::UI::GetSingleton();
        return ui->GetMenu("HUD Menu");
    }
    RE::GFxValue GetControlUIRoot(RE::GPtr<RE::IMenu> a_menu, int64_t a_glyph) {
        std::string container = "_root.WidgetContainer." + std::to_string(a_glyph) + ".widget.ctr";
        RE::GFxValue root;
        if (a_menu) {
            a_menu->uiMovie->GetVariable(&root, container.c_str());
        }
        return root;
    }
    enum Controls { Up, Down, Left, Right, Toggle, Yes, No, Menu };

    std::string GetControlString(Controls a_control) {
        switch (a_control) {
            case Controls::Up:
                return "UP";
            case Controls::Down:
                return "DOWN";
            case Controls::Left:
                return "LEFT";
            case Controls::Right:
                return "RIGHT";
            case Controls::Toggle:
                return "TOG";
            case Controls::Yes:
                return "YES";
            case Controls::No:
                return "NO";
            case Controls::Menu:
                return "MENU";
            default:
                return "";
        }
    }

    void Control(RE::StaticFunctionTag*, Controls a_direction, int64_t a_glyph) {
        if (!OStim::ThreadManager::GetSingleton()->AnySceneRunning()) {
            return;
        }
        auto menu = GetMenu();
        if (menu) {
            auto ui = GetControlUIRoot(menu, a_glyph);
            auto direction = GetControlString(a_direction);
            if (direction != "") {
                ui.Invoke(direction.c_str());
            }
        }
    }

    void EndPlayerDialogue(RE::StaticFunctionTag*) {
        RE::PlayerCharacter::GetSingleton()->EndDialogue();
    }

    bool Bind(VM* a_vm) {
        const auto obj = "OSANative"sv;

        BIND(Control);
        BIND(EndPlayerDialogue);

        return true;
    }
}  // namespace PapyrusControl