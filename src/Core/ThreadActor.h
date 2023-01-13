#pragma once

#include "Graph/Node.h"

namespace OStim {
	class ThreadActor {
	public:
		ThreadActor(RE::Actor* actor);
        void initContinue();

		float excitement = 0;
		float nodeExcitementTick = 0;
		float baseExcitementMultiplier = 1.0;
		float maxExcitement = 0;

		inline RE::Actor* getActor() { return actor; }

        void undress();
        void undressPartial(uint32_t mask);
        void removeWeapons();
        void redress();
        void redressPartial(uint32_t mask);
        void addWeapons();

        void changeNode(Graph::Actor* graphActor);

        void free();

    private:
        class GetRmHeightCallbackFunctor : public RE::BSScript::IStackCallbackFunctor {
        public:
            GetRmHeightCallbackFunctor(ThreadActor* threadActor) : threadActor{threadActor} {}

            void setRmHeight(float height);

            virtual inline void operator()(RE::BSScript::Variable a_result) override {
                if (a_result.IsNoneObject()) {
                    logger::info("result is none");
                } else if (a_result.IsFloat()) {
                    setRmHeight(a_result.GetFloat());
                } else {
                    logger::info("result is not a float");
                }
            }

            virtual inline void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>& a_object){};

        private:
            ThreadActor* threadActor;
        };

		RE::Actor* actor;
        float scaleBefore = 1;
        bool isPlayer;
        bool isFemale;

        Graph::Actor* graphActor;

        bool undressed = false;
        uint32_t undressedMask = 0;
        std::vector<RE::TESObjectARMO*> undressedItems;
        bool weaponsRemoved = false;
        RE::TESForm* rightHand;
        RE::TESForm* leftHand;
        RE::TESAmmo* ammo;

        float rmHeight = 1;
        float heelOffset = 0;
        bool heelOffsetRemoved = false;
        RE::TESObjectARMO* heelArmor;
        bool heelArmorRemoved = false;

        void scale();
        void checkHeelOffset();
        void updateHeelOffset(bool remove);
        void updateHeelArmor(bool remove);
	};	
}