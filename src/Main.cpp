#include <stddef.h>

#include "Game/Patch.h"
#include "Graph/LookupTable.h"
#include "InterfaceSpec/IPluginInterface.h"
#include "InterfaceSpec/PluginInterface.h"
#include "Messaging/IMessages.h"
#include "Papyrus/Papyrus.h"
#include "SKEE.h"
#include "Serial/Manager.h"
#include "Trait/TraitTable.h"
#include "Util/MCMTable.h"

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace {
    void InitializeLogging() {
        auto path = log_directory();
        if (!path) {
            report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= PluginDeclaration::GetSingleton()->GetName();
        *path += L".log";

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    }

    void UnspecificedSenderMessageHandler(SKSE::MessagingInterface::Message* a_msg) {        
        switch (a_msg->type) {
            case OSAInterfaceExchangeMessage::kMessage_ExchangeInterface: {
                OSAInterfaceExchangeMessage* exchangeMessage = (OSAInterfaceExchangeMessage*)a_msg->data;
                exchangeMessage->interfaceMap = InterfaceMap::GetSingleton();
            } break;
        }
    }

    void MessageHandler(SKSE::MessagingInterface::Message* a_msg) {        
        switch (a_msg->type) {
            case SKSE::MessagingInterface::kPostLoad: {
                auto message = SKSE::GetMessagingInterface();
                if (message) {
                    message->RegisterListener(nullptr, UnspecificedSenderMessageHandler);
                }
            } break;
            case SKSE::MessagingInterface::kDataLoaded: {
                Trait::TraitTable::setupForms();
                MCM::MCMTable::setupForms();
            } break;
            case SKSE::MessagingInterface::kPostPostLoad: {
                SKEE::InterfaceExchangeMessage msg;
                auto intfc = SKSE::GetMessagingInterface();
                intfc->Dispatch(SKEE::InterfaceExchangeMessage::kExchangeInterface, (void*)&msg,
                                sizeof(SKEE::InterfaceExchangeMessage*), "skee");
                if (!msg.interfaceMap) {
                    logger::critical("Couldn't get interface map!");
                    return;
                }

                auto nioInterface =
                    static_cast<SKEE::INiTransformInterface*>(msg.interfaceMap->QueryInterface("NiTransform"));
                if (!nioInterface) {
                    logger::critical("Couldn't get serialization NiTransformInterface!");
                }

                logger::info("NiTransform version {}", nioInterface->GetVersion());
                if (!Graph::LookupTable::setNiTransfromInterface(nioInterface)) {
                    logger::info("NiTransformInterface not provided.");
                }
            } break;
        }
    }
}  // namespace

SKSEPluginLoad(const LoadInterface* skse) {
    InitializeLogging();

    auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    log::info("{} {} is loading...", plugin->GetName(), version);

    Init(skse);

    InterfaceMap::GetSingleton()->AddInterface("Messaging", Messaging::MessagingRegistry::GetSingleton());

    auto message = SKSE::GetMessagingInterface();
    if (!message->RegisterListener(MessageHandler)) {
        return false;
    }

    Patch::Install();
    Papyrus::Bind();
    Papyrus::Build();
    Trait::TraitTable::setup();

    const auto serial = SKSE::GetSerializationInterface();
    serial->SetUniqueID(Serialization::kOSA);
    serial->SetSaveCallback(Serialization::Save);
    serial->SetLoadCallback(Serialization::Load);
    serial->SetRevertCallback(Serialization::Revert);

    log::info("{} has finished loading.", plugin->GetName());
    return true;
}
