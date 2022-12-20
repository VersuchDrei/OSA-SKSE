#include "AlignMenu.h"

#include "RE/G/GFxValue.h"
#include "RE/G/GPtr.h"

namespace UI::Align {

    inline RE::GFxValue GetRoot() {
        RE::GFxValue root;
        RE::GPtr<RE::IMenu> alignMenu = RE::UI::GetSingleton()->GetMenu(AlignMenu::MENU_NAME);
        if (!alignMenu || !alignMenu->uiMovie) {
            return root;
        }
        alignMenu->uiMovie->GetVariable(&root, "_root.rootObj");
        return root;
    }

    AlignMenu::AlignMenu() : Super() {
        auto scaleformManager = RE::BSScaleformManager::GetSingleton();

        inputContext = Context::kNone;
        depthPriority = 0;

        auto menu = static_cast<Super*>(this);
        menu->depthPriority = SORT_PRIORITY;

        menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
        menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);

        if (uiMovie) {
            uiMovie->SetMouseCursorCount(0);  // disable input
        }

        scaleformManager->LoadMovieEx(this, MENU_PATH, [](RE::GFxMovieDef* a_def) -> void {
            a_def->SetState(RE::GFxState::StateType::kLog, RE::make_gptr<Logger>().get());
        });
    }

    void AlignMenu::Register() {
        auto ui = RE::UI::GetSingleton();
        if (ui) {
            ui->Register(MENU_NAME, Creator);

            AlignMenu::Show();
        }
    }

    void AlignMenu::Show() {
        auto msgQ = RE::UIMessageQueue::GetSingleton();
        if (msgQ) {
            msgQ->AddMessage(AlignMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
        }
        if (currentActor == nullptr) {
            if (currentThread != nullptr) {
                SetActor(0);
                SelectField(0);
            }
        }
    }

    void AlignMenu::SetThread(OStim::Thread* thread) {
        currentThread = thread;
        UpdateSceneInfo();
    }

    void AlignMenu::SetNode(Graph::Node* node) {
        currentNode = node;
        UpdateSceneInfo();
    }

    void AlignMenu::SetActor(int actor) {
        selectedSlot = actor;
        currentActor = currentThread->GetTESActors()[selectedSlot];
        LoadCurrentAlignment();
        UpdateActorInfo();
    }

    void AlignMenu::SetActor(RE::Actor* newActor) {
        currentActor = newActor;
        auto actors = currentThread->GetTESActors();
        for (int i = 0; i < actors.size(); i++) {
            if (actors[i] == newActor) {
                selectedSlot = i;
            }
        }
        LoadCurrentAlignment();
        UpdateActorInfo();
    }

    void AlignMenu::LoadCurrentAlignment() {
        auto actors = currentThread->GetTESActors();
        auto genderMap = OAlign::GetGenderMap(&actors);

        auto alignments = OAlign::Alignments::GetSingleton()->GetAlignmentInfoForScene(currentNode, genderMap);
        loadedSource = std::get<0>(alignments);
        auto alignmentInfos = std::get<1>(alignments);
        if (loadedSource != OAlign::Source::None) {
            currentActorInfo.offsetX = alignmentInfos->at(selectedSlot).offsetX;
            currentActorInfo.offsetY = alignmentInfos->at(selectedSlot).offsetY;
            currentActorInfo.offsetZ = alignmentInfos->at(selectedSlot).offsetZ;
        } else {
            currentActorInfo.offsetX = 0;
            currentActorInfo.offsetY = 0;
            currentActorInfo.offsetZ = 0;
        }
    }

    void AlignMenu::HandleThreadRemoved(OStim::Thread* thread) {
        if (thread == currentThread) {
            currentThread = nullptr;
        }
    }

    void AlignMenu::Hide() {
        auto msgQ = RE::UIMessageQueue::GetSingleton();
        if (msgQ) {
            msgQ->AddMessage(AlignMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
        }
    }

    void AlignMenu::UpdateSceneInfo() {
        if (currentThread == nullptr || currentNode == nullptr) return;
        auto root = GetRoot();

        RE::GFxValue sceneInfo;
        root.GetMember("sceneInfo", &sceneInfo);

        const RE::GFxValue sceneInfoPack = RE::GFxValue{currentNode->sourceModule.c_str()};
        const RE::GFxValue sceneInfoAnimName = RE::GFxValue{currentNode->scene_id.c_str()};
        auto actors = currentThread->GetTESActors();
        auto genderMap = OAlign::GetGenderMap(&actors);
        const RE::GFxValue genderMapValue = RE::GFxValue{genderMap.c_str()};

        RE::GFxValue infoArray[3]{sceneInfoPack, sceneInfoAnimName, genderMapValue};
        sceneInfo.Invoke("updateInfo", nullptr, infoArray, 3);
    }

    void AlignMenu::UpdateActorInfo() {
        if (currentActor == nullptr) return;
        auto root = GetRoot();

        RE::GFxValue alignmentInfo;
        root.GetMember("alignmentInfo", &alignmentInfo);

        const RE::GFxValue actorName = RE::GFxValue{currentActor->GetDisplayFullName()};
        const RE::GFxValue actorSlot = selectedSlot;

        auto gender = OAlign::GetGenderVal(currentActor);

        const RE::GFxValue actorGender = RE::GFxValue{gender};

        auto source = OAlign::GetString((OAlign::Source)loadedSource);
        const RE::GFxValue sourceData{source};
        RE::GFxValue infoArray[8]{actorName,
                                  actorSlot,
                                  actorGender,
                                  sourceData,
                                  incrementvalue,
                                  currentActorInfo.offsetX,
                                  currentActorInfo.offsetY,
                                  currentActorInfo.offsetZ};

        alignmentInfo.Invoke("updateInfo", nullptr, infoArray, 8);
    }

    void AlignMenu::Update() {}

    void AlignMenu::Handle(UI::Controls control) {
        switch (control) {
            case Up: {
                ScrollSelectedField(-1);
            } break;
            case Down: {
                ScrollSelectedField(1);
            } break;
            case Toggle: {
                ToggleActor();
            } break;
            case Left: {
                Increment(false);
            } break;
            case Right: {
                Increment(true);
            } break;
            case No: {
                CycleIncrement();
            } break;
        }
    }

    void AlignMenu::SelectField(int field) {
        auto root = GetRoot();

        RE::GFxValue alignmentInfo;
        root.GetMember("alignmentInfo", &alignmentInfo);
        RE::GFxValue fieldValue[1]{field};
        alignmentInfo.Invoke("selectField", nullptr, fieldValue, 1);
    }

    void AlignMenu::ScrollSelectedField(int field) {
        if (selectedField + field < 0) {
            selectedField = 2;
        } else if (selectedField + field > 2) {
            selectedField = 0;
        } else {
            selectedField += field;
        }
        SelectField(selectedField);
    }

    void AlignMenu::ToggleActor() {
        auto actors = currentThread->GetTESActors();
        selectedSlot++;
        if (selectedSlot > actors.size() - 1) {
            selectedSlot = 0;
        }
        SetActor(selectedSlot);

        UpdateActorInfo();
    }

    void AlignMenu::Increment(bool up) {
        auto root = GetRoot();

        double* currentVal;
        switch (selectedField) {
            case 0:
                currentVal = &currentActorInfo.offsetX;
                break;
            case 1:
                currentVal = &currentActorInfo.offsetY;
                break;
            case 2:
                currentVal = &currentActorInfo.offsetZ;
                break;
            default:
                return;
        }
        if (up)
            *currentVal += incrementvalue;
        else
            *currentVal -= incrementvalue;

        RE::GFxValue alignmentInfo;
        root.GetMember("alignmentInfo", &alignmentInfo);
        RE::GFxValue values[2]{*currentVal, up};
        alignmentInfo.Invoke("updateDoubleField", nullptr, values, 2);

        OAlign::Alignments::GetSingleton()->UpdateAndApplyAlignments(currentThread, currentActorInfo,
                                                                     (OAlign::Source)loadedSource, currentNode, selectedSlot);
    }

    void AlignMenu::CycleIncrement() {
        if (incrementvalue == 1.0) {
            incrementvalue = 0.1;
        } else if (incrementvalue == 0.1) {
            incrementvalue = 0.01;
        } else if (incrementvalue == 0.01) {
            incrementvalue = 0.001;
        } else {
            incrementvalue = 1.0;
        }
        auto root = GetRoot();
        RE::GFxValue alignmentInfo;
        root.GetMember("alignmentInfo", &alignmentInfo);
        RE::GFxValue values[1]{incrementvalue};
        alignmentInfo.Invoke("updateIncrement", nullptr, values, 1);
    }

    void AlignMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime) {
        AlignMenu::Update();
        RE::IMenu::AdvanceMovie(a_interval, a_currentTime);
    }
}  // namespace UI::Align