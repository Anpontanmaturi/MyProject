#include "selector_node.h"
#include "../bt_debug_info.h"

BTState SelectorNode::Tick(BTContext& context, float elapsed_time)
{
	for (auto& child : children)
	{
		BTState result = child->Tick(context, elapsed_time);

		if (context.debug_info)
		{
			context.debug_info->SetState(child->GetNodeId(), BTDebug::ToDebugState(result));
		}

		switch (result)
		{
		case BTState::Success:
			return BTState::Success;

		case BTState::Run:
			return BTState::Run;

		case BTState::Failure:
			continue;
		}
	}

	return BTState::Failure;
}