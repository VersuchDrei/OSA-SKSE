#include "TraitTable.h"

#include "FacialExpression.h"
#include "Util/VectorUtil.h"
#include "Util/JsonFileLoader.h"

namespace Trait {
    const char* EXPRESSION_FILE_PATH{"Data/SKSE/Plugins/OStim/facial expressions"};

    void TraitTable::setup() {
        openMouthPhonemes.insert({0, {.type = 0, .baseValue = 100}});
        openMouthPhonemes.insert({1, {.type = 1, .baseValue = 40}});
        openMouthPhonemes.insert({5, {.type = 5, .baseValue = 100}});

        lickingPhonemes.insert({0, {.type = 0, .baseValue = 100}});

        std::srand((unsigned)time(NULL));

        Util::JsonFileLoader::LoadFilesInFolder(EXPRESSION_FILE_PATH,[&](std::string,json json) {
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
        });        
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

        if (json.contains("modifiers")) {
            for (auto& modifier : json["modifiers"]) {
                auto mod = parseModifier(modifier);
                if (VectorUtil::contains(eyelidModifierTypes, mod.type)) {
                    genderExpression->eyelidModifiers.insert({mod.type, mod});
                } else if (VectorUtil::contains(eyebrowModifierTypes, mod.type)) {
                    genderExpression->eyebrowModifiers.insert({mod.type, mod});
                } else if (VectorUtil::contains(eyeballModifierTypes, mod.type)) {
                    genderExpression->eyeballModifiers.insert({mod.type, mod});
                }
            }
        }

        if (json.contains("phonemes")) {
            for (auto& phoneme : json["phonemes"]) {
                auto mod = parseModifier(phoneme);
                genderExpression->phonemes.insert({mod.type, mod});
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
        if (json.contains("delay")) {
            modifier.delay = json["delay"];
        }
        if (json.contains("delayVariance")) {
            modifier.delayVariance = json["delayVariance"];
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