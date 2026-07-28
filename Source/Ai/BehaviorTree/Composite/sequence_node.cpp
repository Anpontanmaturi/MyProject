#include "sequence_node.h"

BTState SequenceNode::Tick(float elapsed_time)
{
	for (auto& child : children)
	{
		BTState result = child->Tick(elapsed_time);

		switch (result)
		{
		case BTState::Success:
			return BTState::Success;

		case BTState::Run:
			return BTState::Run;

		case BTState::Failure:
			break;
		}
	}

	return BTState::Failure;
}