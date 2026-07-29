#pragma once

#include <memory>
#include "bt_state.h"
#include "bt_context.h"

class BTNode;

class BehaviorTree
{
public:
	void SetRoot(std::unique_ptr<BTNode> node);

	[[nodiscard]]
	BTState Tick(BTContext& context, float elapsed_time);

private:
	std::unique_ptr<BTNode> root;
};