#pragma once

namespace OStim {
	class ThreadActor {
	public:
		ThreadActor(RE::Actor* actor);

		float excitement = 0;
		float nodeExcitementTick = 0;
		float baseExcitementMultiplier = 1.0;
		float maxExcitement = 0;
		RE::Actor* getActor() { return actor; }
	private:
		RE::Actor* actor;
	};	
}