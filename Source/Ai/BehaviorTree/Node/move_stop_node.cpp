#include "move_stop_node.h"
#include "Character/character.h"

BTState StopNode::OnTick(
	BTContext& context,
	float elapsed_time)
{
	if (!executed)
	{
		context.owner->MoveStop();

		executed = true;
	}

	return BTState::Success;
}

void StopNode::OnExit(BTContext& context, BTState result)
{
	executed = false;
}