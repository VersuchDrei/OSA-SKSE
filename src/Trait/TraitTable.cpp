#include "TraitTable.h"

#include "FacialExpression.h"
#include "Serial/Manager.h"
#include "Util/ActorUtil.h"
#include "Util/MapUtil.h"
#include "Util/VectorUtil.h"
#include "Util/JsonFileLoader.h"
#include "Util/JsonUtil.h"
#include "Util/StringUtil.h"

namespace Trait {
    const char* EXPRESSION_FILE_PATH{"Data/SKSE/Plugins/OStim/facial expressions"};
    const char* EQUIP_OBJECT_FILE_PATH{"Data/SKSE/Plugins/OStim/equip objects"};

    void TraitTable::setup() {
        openMouthPhonemes.insert({0, {.type = 0, .baseValue = 100}});
        openMouthPhonemes.insert({1, {.type = 1, .baseValue = 40}});
        openMouthPhonemes.insert({5, {.type = 5, .baseValue = 100}});

        lickingPhonemes.insert({0, {.type = 0, .baseValue = 100}});

        std::srand((unsigned)time(NULL));

        Util::JsonFileLoader::LoadFilesInFolder(EXPRESSION_FILE_PATH,[&](std::string,std::string filename,json json) {
            FacialExpression* expression = new FacialExpression();
            if (json.contains("female")) {
                parseGender(json["female"], &expression->female);
            }

            if (json.contains("male")) {
                parseGender(json["male"], &expression->male);
            }

            if (json.contains("sets")) {
                for (auto& set : json["sets"]) {
                    addToTable(&expressionsBySets, set, expression);
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
            logger::warn("no default expression defined, generating empty");
            std::vector<FacialExpression*>* expressions = new std::vector<FacialExpression*>();
            expressions->push_back(new FacialExpression());
            expressionsBySets.insert({"default", expressions});
        }
    }

    void TraitTable::setupForms() {
        RE::TESDataHandler* handler = RE::TESDataHandler::GetSingleton();
        excitementFaction = handler->LookupForm<RE::TESFaction>(0xD93, "OStim.esp");
        noFacialExpressionsFaction = handler->LookupForm<RE::TESFaction>(0xD92, "OStim.esp");

        if (handler->GetModIndex("Schlongs of Skyrim.esp")) {
            SOS_SchlongifiedFaction = handler->LookupForm<RE::TESFaction>(0x00Aff8, "Schlongs of Skyrim.esp");
        } else {
            logger::info("SoS full is not installed.");
        }

        // this needs to go in setupForms because it requires the kDataLoaded event
        Util::JsonFileLoader::LoadFilesInFolder(EQUIP_OBJECT_FILE_PATH, [&](std::string path, std::string filename, json json) {
            std::string id = filename;
            StringUtil::toLower(&id);

            if (id == "default" || id == "random") {
                logger::info("illegal equip object id: {}, this id is reserved by OStim", id);
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

            std::string type = json["type"];
            StringUtil::toLower(&type);
            auto iter = equipObjects.find(type);
            if (iter != equipObjects.end()) {
                if (iter->second.contains(id)) {
                    logger::info("duplicate equip object {} for type {}", id, type);
                } else {
                    iter->second.emplace(id, object);
                }
            } else {
                equipObjects.emplace(type, std::unordered_map<std::string, EquipObject*>{{id, object}});
            }
        });
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

        if (json.contains("phonemeObjects")) {
            for (auto& object : json["phonemeObjects"]) {
                genderExpression->phonemeObjects.push_back(object);
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

    void TraitTable::addToTable(std::unordered_map<std::string, std::vector<FacialExpression*>*>* table, std::string key, FacialExpression* expression) {
        StringUtil::toLower(&key);
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

    EquipObject* TraitTable::getRandomEquipObject(std::string type) {
        auto iter = equipObjects.find(type);
        if (iter != equipObjects.end()) {
            return MapUtil::randomValue(iter->second);
        }
        return nullptr;
    }

    EquipObject* TraitTable::getEquipObject(RE::Actor* actor, std::string type) {
        auto iter = equipObjects.find(type);
        if (iter == equipObjects.end()) {
            return nullptr;
        }

        RE::TESNPC* base = actor->GetActorBase();
        std::string id = Serialization::getEquipObject(base->GetFormID(), type);
        if (id == "random") {
            return MapUtil::randomValue(iter->second);
        } else if (id != "" && id != "default") {
            auto iter2 = iter->second.find(id);
            if (iter2 != iter->second.end()) {
                return iter2->second;
            }
        }

        id = Serialization::getEquipObject(base->GetSex() == RE::SEX::kMale ? 0x0 : 0x1, type);
        if (id != "" && id != "random") {
            auto iter2 = iter->second.find(id);
            if (iter2 != iter->second.end()) {
                return iter2->second;
            }
        }

        return MapUtil::randomValue(iter->second);
    }

    std::vector<std::string> TraitTable::getEquipObjectPairs(RE::FormID formID, std::string type) {
        std::vector<std::string> ret;
        if (formID > 1) {
            ret.push_back("default");
            ret.push_back("default");
        }
        ret.push_back("random");
        ret.push_back("random");

        auto iter = equipObjects.find(type);
        if (iter != equipObjects.end()) {
            for (auto& [id, object] : iter->second) {
                ret.push_back(id);
                ret.push_back(object->name);
            }
        }

        return ret;
    }

    std::string TraitTable::getEquipObjectName(RE::FormID formID, std::string type) {
        std::string id = Serialization::getEquipObject(formID, type);
        if (id == "") {
            return formID >= 1 ? "default" : "random";
        } else if (id == "default") {
            return "default";
        } else if (id == "random") {
            return "random";
        }

        auto iter = equipObjects.find(type);
        if (iter == equipObjects.end()) {
            return "-invalid-";
        }

        auto iter2 = iter->second.find(id);
        if (iter2 != iter->second.end()) {
            return iter2->second->name;
        }

        return "-invalid-";
    }

    void TraitTable::setEquipObjectID(RE::FormID formID, std::string type, std::string id) {
        Serialization::setEquipObject(formID, type, id);
    }

    bool TraitTable::hasSchlong(RE::Actor* actor) {
        if (SOS_SchlongifiedFaction) {
            return actor->IsInFaction(SOS_SchlongifiedFaction);
        } else {
            return actor->GetActorBase()->GetSex() == RE::SEX::kMale;
        }
    }
}