#include "idle_node.h"
#include "Character/character.h"

BTState IdleNode::OnTick(
	BTContext& context,
	float elapsed_time)
{
	context.owner->MoveStop();
	return BTState::Success;
}