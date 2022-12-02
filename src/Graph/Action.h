#pragma once

namespace Graph {	
	struct ActionActor{
	public:
		float stimulation;
		float maxStimulation;
		std::vector<std::string> requirements;
		std::vector<int> strippingSlots;
		std::unordered_map<std::string, int> ints;
		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, std::string> strings;
	};

	struct ActionAttributes {
	public:
		ActionActor actor;
		ActionActor target;
		ActionActor performer;
	};
}