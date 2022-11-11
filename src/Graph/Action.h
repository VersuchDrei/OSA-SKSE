#pragma once

namespace Graph {	
	struct ActionActor{
	public:
		float stimulation;
		std::vector<std::string> requirements;
		std::vector<int> strippingSlots;
	};
	struct ActionPerformer {
	public:
		std::unordered_map<std::string, float> floats;
	};

	struct ActionAttributes {
	public:
		ActionActor actor;
		ActionActor target;
		ActionPerformer performer;
	};
}