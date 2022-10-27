#include "TraitTable.h"

#include "FacialExpression.h"

namespace Trait {
    const char* EXPRESSION_FILE_PATH{"Data/SKSE/Plugins/OStim/facial expressions"};
    const char* EXPRESSION_FILE_SUFFIX{".json"};

    void TraitTable::setup() {
        openMouthPhonemes.push_back(*(new FaceModifier()));
        openMouthPhonemes.push_back(*(new FaceModifier()));
        openMouthPhonemes.push_back(*(new FaceModifier()));

        openMouthPhonemes[0].type = 0;
        openMouthPhonemes[0].baseValue = 100;
        openMouthPhonemes[1].type = 1;
        openMouthPhonemes[1].baseValue = 40;
        openMouthPhonemes[2].type = 5;
        openMouthPhonemes[2].baseValue = 100;

        kissingPhonemes.push_back(*(new FaceModifier()));
        kissingPhonemes[0].type = 15;
        kissingPhonemes[0].baseValue = 100;

        std::srand((unsigned)time(NULL));

        fs::path rootPath{EXPRESSION_FILE_PATH};
        if (!fs::exists(rootPath)) {
            logger::warn("expression path ({}) does not exist", EXPRESSION_FILE_PATH);
            return;
        }

        for (auto& file : fs::directory_iterator{rootPath}) {
            auto& path = file.path();
            auto pathStr = path.string();


            if (pathStr.ends_with(EXPRESSION_FILE_SUFFIX)) {
                logger::info("parsing file {}", pathStr);
                std::ifstream ifs(pathStr);
                json json = json::parse(ifs);

                FacialExpression* expression = new FacialExpression();
                if (json.contains("female")) {
                    parseGender(json["female"], &expression->female);
                }

                if (json.contains("male")) {
                    parseGender(json["male"], &expression->male);
                }

                if (json.contains("events")) {
                    for (auto& eventStr : json["events"]) {
                        addToTable(&expressionsByEvents, eventStr, expression);
                    }
                }

                if (json.contains("actionActors")) {
                    for (auto& action : json["actionActors"]) {
                        addToTable(&expressionsByActionActors, action, expression);
                    }
                }

                if (json.contains("actionTargets")) {
                    for (auto& action : json["actionTargets"]) {
                        addToTable(&expressionsByActionTargets, action, expression);
                    }
                }
                logger::info("parsed file {}", pathStr);
            }

        }
    }

    void TraitTable::setupForms() {
        excitementFaction = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x00000D93, "OStim.esp");
        noFacialExpressionsFaction = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESFaction>(0x00000D92, "OStim.esp");
    }

    void TraitTable::parseGender(nlohmann::json json, GenderExpression* genderExpression) {
        if (json.contains("duration")) {
            genderExpression->duration = json["duration"];
        }
        if (json.contains("expression")) {
            genderExpression->expression = parseModifier(json["expression"]);
        }
        if (json.contains("phonemes")) {
            for (auto& phoneme : json["phonemes"]) {
                genderExpression->phonemes.push_back(parseModifier(phoneme));
            }
        }
        if (json.contains("modifiers")) {
            for (auto& modifier : json["modifiers"]) {
                genderExpression->modifiers.push_back(parseModifier(modifier));
            }
        }
    }

    FaceModifier TraitTable::parseModifier(nlohmann::json json) {
        FaceModifier modifier{};
        if (json.contains("type")) {
            modifier.type = json["type"];
        }
        if (json.contains("baseValue")) {
            modifier.baseValue = json["baseValue"];
        }
        if (json.contains("speedMultiplier")) {
            modifier.speedMultiplier = json["speedMultiplier"];
        }
        if (json.contains("excitementMultiplier")) {
            modifier.excitementMultiplier = json["excitementMultiplier"];
        }
        return modifier;
    }

    void TraitTable::addToTable(std::unordered_map<std::string, std::vector<FacialExpression*>>* table, std::string key, FacialExpression* expression) {
        auto iter = table->find(key);
        if (iter != table->end()) {
            iter->second.push_back(expression);
        } else {
            std::vector<FacialExpression*> expressions;
            expressions.push_back(expression);
            table->insert({key, expressions});
        }
    }

    FacialExpression* TraitTable::getExpressionForActionActor(std::string action) {
        return getExpressionFromTable(expressionsByActionActors, action);
    }

    FacialExpression* TraitTable::getExpressionForActionTarget(std::string action) {
        return getExpressionFromTable(expressionsByActionTargets, action);
    }

    FacialExpression* TraitTable::getExpressionForEvent(std::string strEvent) {
        return getExpressionFromTable(expressionsByEvents, strEvent);
    }

    FacialExpression* TraitTable::getExpressionFromTable(std::unordered_map<std::string, std::vector<FacialExpression*>> table, std::string key) {
        auto iter = table.find(key);
        if (iter != table.end()) {
            auto& expressions = iter->second;
            auto size = expressions.size();
            if (size != 0) {
                return expressions[std::rand() % size];
            }
        }
        return nullptr;
    }

    void TraitTable::setExcitement(RE::Actor* actor, float excitement) {
        for (auto& factionInfo : actor->GetActorBase()->factions) {
            if (factionInfo.faction == excitementFaction) {
                factionInfo.rank = (int) excitement;
            }
        }
    }

    float TraitTable::getExcitement(RE::Actor* actor) {
        for (auto& factionInfo : actor->GetActorBase()->factions) {
            if (factionInfo.faction == excitementFaction) {
                return factionInfo.rank;
            }
        }
        return 0;
    }
}