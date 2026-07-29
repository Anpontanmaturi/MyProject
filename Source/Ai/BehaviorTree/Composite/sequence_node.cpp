#include "sequence_node.h"

BTState SequenceNode::Tick(BTContext& context, float elapsed_time)
{
	for (auto& child : children)
	{
		BTState result = child->Tick(context,elapsed_time);

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