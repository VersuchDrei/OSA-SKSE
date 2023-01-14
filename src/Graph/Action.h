#pragma once

namespace Graph {
	struct ActionActor{
	public:
        float stimulation = 0.0;
        float maxStimulation = 100.0;
		std::vector<std::string> requirements;
		uint32_t strippingMask = 0;
		std::unordered_map<std::string, int> ints;
		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, std::string> strings;
	};

	struct ActionAttributes {
	public:
		ActionActor actor;
		ActionActor target;
		ActionActor performer;
        std::vector<std::string> tags;

		bool hasTag(std::string tag);
    };

    struct Action {
    public:
        std::string type;
        ActionAttributes* attributes;
        int actor;
        int target;
        int performer;

		uint32_t getStrippingMask(int index);
    };
}