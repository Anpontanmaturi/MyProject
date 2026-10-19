#pragma once

#include <memory>
#include "bt_state.h"
#include "bt_context.h"
#include "bt_debug_info.h"

class BTNode;

class BehaviorTree
{
public:
	void SetRoot(std::unique_ptr<BTNode> node);

	[[nodiscard]]
	BTState Tick(BTContext& context, float elapsed_time);

	BTDebugInfo& GetDebugInfo()
	{
		return debug_info;
	}

private:
	std::unique_ptr<BTNode> root;
	BTDebugInfo debug_info;
};