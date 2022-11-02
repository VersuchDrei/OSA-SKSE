#pragma once

#include <Graph/Node.h>

namespace Messaging {
	enum messageId : uint32_t { kAnimationChanged = 0x4e9673} ;

	struct AnimationChangedMessage {
		Graph::Node* newAnimation = nullptr;
	};
}