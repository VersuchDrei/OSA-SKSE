#pragma once

namespace OStim {
	class ThreadActor {
	public:
		ThreadActor(RE::Actor* actor);

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
	private:
		RE::Actor* actor;
        bool undressed = false;
        uint32_t undressedMask = 0;
        std::vector<RE::TESObjectARMO*> undressedItems;
        bool weaponsRemoved = false;
        RE::TESForm* rightHand;
        RE::TESForm* leftHand;
        RE::TESAmmo* ammo;
	};	
}