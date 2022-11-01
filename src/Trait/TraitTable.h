#pragma once

#include "FacialExpression.h"

namespace Trait {
    class TraitTable {
    public:
        inline static FacialExpression fallbackExpression{};

        inline static std::vector<FaceModifier> openMouthPhonemes;
        inline static std::vector<FaceModifier> kissingPhonemes;
        inline static std::vector<FaceModifier> lickingPhonemes;

        static void setup();
        static void setupForms();

        static FacialExpression* getExpressionForActionActor(std::string);
        static FacialExpression* getExpressionForActionTarget(std::string);
        static FacialExpression* getExpressionForEvent(std::string);

        static void setExcitement(RE::Actor* actor, float excitement);
        static float getExcitement(RE::Actor* actor);
        inline static bool areFacialExpressionsBlocked(RE::Actor* actor) {
            return actor->IsInFaction(noFacialExpressionsFaction);
        }

    private:
        static void parseGender(nlohmann::json json, GenderExpression* genderExpression);
        static FaceModifier parseModifier(nlohmann::json json);
        static void addToTable(std::unordered_map<std::string, std::vector<FacialExpression*>>* table, std::string key, FacialExpression* expression);
        static FacialExpression* getExpressionFromTable(std::unordered_map<std::string, std::vector<FacialExpression*>> table, std::string key);

        inline static std::unordered_map<std::string, std::vector<FacialExpression*>> expressionsByActionActors;
        inline static std::unordered_map<std::string, std::vector<FacialExpression*>> expressionsByActionTargets;
        inline static std::unordered_map<std::string, std::vector<FacialExpression*>> expressionsByEvents;

        inline static RE::TESFaction* excitementFaction;
        inline static RE::TESFaction* noFacialExpressionsFaction;
    };
}