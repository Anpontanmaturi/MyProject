#pragma once

#include <memory>
#include "bt_state.h"

class BTNode;

class BehaviorTree
{
public:
	void SetRoot(std::unique_ptr<BTNode> node);

	[[nodiscard]]
	BTState Tick(float elpased_time);

private:
	std::unique_ptr<BTNode> root;
};