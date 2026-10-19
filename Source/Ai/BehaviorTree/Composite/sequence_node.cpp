#include "sequence_node.h"
#include "../bt_debug_info.h"

BTState SequenceNode::Tick(BTContext& context, float elapsed_time)
{
	for (auto& child : children)
	{
		BTState result = child->Tick(context,elapsed_time);

		if (context.debug_info)
		{
			context.debug_info->SetState(child->GetNodeId(), BTDebug::ToDebugState(result));
		}

		switch (result)
		{
		case BTState::Success:
			continue;

		case BTState::Run:
			return BTState::Run;

		case BTState::Failure:
			return BTState::Failure;
		}
	}

	return BTState::Success;
}