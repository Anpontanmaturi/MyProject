#include "selector_node.h"

BTState SelectorNode::Tick(BTContext& context, float elapsed_time)
{
	for (auto& child : children)
	{
		BTState result = child->Tick(context, elapsed_time);

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