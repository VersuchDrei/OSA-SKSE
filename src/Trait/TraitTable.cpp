#include "TraitTable.h"

#include "FacialExpression.h"
#include "Util/ActorUtil.h"
#include "Util/VectorUtil.h"
#include "Util/JsonFileLoader.h"
#include "Util/JsonUtil.h"

namespace Trait {
    const char* EXPRESSION_FILE_PATH{"Data/SKSE/Plugins/OStim/facial expressions"};
    const char* EQUIP_OBJECT_FILE_PATH{"Data/SKSE/Plugins/OStim/equip objects"};

    void TraitTable::setup() {
        openMouthPhonemes.insert({0, {.type = 0, .baseValue = 100}});
        openMouthPhonemes.insert({1, {.type = 1, .baseValue = 40}});
        openMouthPhonemes.insert({5, {.type = 5, .baseValue = 100}});

        lickingPhonemes.insert({0, {.type = 0, .baseValue = 100}});

        std::srand((unsigned)time(NULL));

        Util::JsonFileLoader::LoadFilesInFolder(EXPRESSION_FILE_PATH,[&](std::string,std::string,json json) {
            FacialExpression* expression = new FacialExpression();
            if (json.contains("female")) {
                parseGender(json["female"], &expression->female);
            }

            if (json.contains("male")) {
                parseGender(json["male"], &expression->male);
            }

            if (json.contains("sets")) {
                for (auto& group : json["sets"]) {
                    addToTable(&expressionsBySets, group, expression);
                }
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

        auto iter = expressionsBySets.find("default");
        if (iter == expressionsBySets.end()) {
            std::vector<FacialExpression*>* expressions = new std::vector<FacialExpression*>();
            expressions->push_back({});
            expressionsBySets.insert({"default", expressions});
        }

        Util::JsonFileLoader::LoadFilesInFolder(EQUIP_OBJECT_FILE_PATH, [&](std::string path, std::string, json json) {
            if (!json.contains("id")) {
                logger::info("file {} does not have field 'id' defined", path);
                return;
            }
            if (!json.contains("type")) {
                logger::info("file {} does not have field 'type' defined", path);
                return;
            }
            if (!json.contains("name")) {
                logger::info("file {} does not have field 'name' defined", path);
                return;
            }

            RE::TESObjectARMO* item = JsonUtil::getForm<RE::TESObjectARMO>(path, json);
            if (!item) {
                return;
            }

            EquipObject* object = new EquipObject();
            object->name = json["name"];
            object->item = item;

            if (json.contains("variants")) {
                for (auto& [key, val] : json["variants"].items()) {
                    RE::TESObjectARMO* variant = JsonUtil::getForm<RE::TESObjectARMO>(path, val);
                    if (variant) {
                        object->variants.emplace(key, variant);
                    }
                }
            }

            std::string id = json["id"];
            std::string type = json["type"];
            auto iter = equipObjects.find(type);
            if (iter != equipObjects.end()) {
                if (iter->second.contains(id)) {
                    logger::info("duplicate equip object id {} for type {}", id, type);
                } else {
                    iter->second.emplace(id, object);
                }
            } else {
                equipObjects.emplace(type, std::unordered_map<std::string, EquipObject*>{{id, object}});
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
            genderExpression->typeMask |= ExpressionType::EXPRESSION;
        }

        if (json.contains("modifiers")) {
            for (auto& modifier : json["modifiers"]) {
                auto mod = parseModifier(modifier);
                if (VectorUtil::contains(eyelidModifierTypes, mod.type)) {
                    genderExpression->eyelidModifiers.insert({mod.type, mod});
                    genderExpression->typeMask |= ExpressionType::LID_MODIFIER;
                } else if (VectorUtil::contains(eyebrowModifierTypes, mod.type)) {
                    genderExpression->eyebrowModifiers.insert({mod.type, mod});
                    genderExpression->typeMask |= ExpressionType::BROW_MODIFIER;
                } else if (VectorUtil::contains(eyeballModifierTypes, mod.type)) {
                    genderExpression->eyeballModifiers.insert({mod.type, mod});
                    genderExpression->typeMask |= ExpressionType::BALL_MODIFIER;
                }
            }
        }

        if (json.contains("phonemes")) {
            for (auto& phoneme : json["phonemes"]) {
                auto mod = parseModifier(phoneme);
                genderExpression->phonemes.insert({mod.type, mod});
            }
            genderExpression->typeMask |= ExpressionType::PHONEME;
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

    void TraitTable::addToTable(std::unordered_map<std::string, std::vector<FacialExpression*>*>* table, std::string key, FacialExpression* expression) {
        auto iter = table->find(key);
        if (iter != table->end()) {
            iter->second->push_back(expression);
        } else {
            std::vector<FacialExpression*>* expressions = new std::vector<FacialExpression*>();
            expressions->push_back(expression);
            table->insert({key, expressions});
        }
    }

    std::vector<FacialExpression*>* TraitTable::getExpressionsForActionActor(std::string action) {
        return getExpressionsFromTable(expressionsByActionActors, action);
    }

    std::vector<FacialExpression*>* TraitTable::getExpressionsForActionTarget(std::string action) {
        return getExpressionsFromTable(expressionsByActionTargets, action);
    }

    std::vector<FacialExpression*>* TraitTable::getExpressionsForEvent(std::string strEvent) {
        return getExpressionsFromTable(expressionsByEvents, strEvent);
    }

    std::vector<FacialExpression*>* TraitTable::getExpressionsForSet(std::string set) {
        return getExpressionsFromTable(expressionsBySets, set);
    }

    std::vector<FacialExpression*>* TraitTable::getExpressionsFromTable(std::unordered_map<std::string, std::vector<FacialExpression*>*> table, std::string key) {
        auto iter = table.find(key);
        if (iter != table.end()) {
            return iter->second;
        }
        return nullptr;
    }

    void TraitTable::addToExcitementFaction(RE::Actor* actor) {
        //actor->AddToFaction(excitementFaction, 0);
    }

    void TraitTable::removeFromExcitementFaction(RE::Actor* actor) {
        ActorUtil::removeFromFaction(actor, excitementFaction);
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