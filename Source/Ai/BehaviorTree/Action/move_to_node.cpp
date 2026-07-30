#include "move_to_node.h"

#include "../bt_context.h"
#include "Character/character.h"

#include <DirectXMath.h>

MoveToNode::MoveToNode(const BlackboardKey& key, float move_speed, float turn_speed, float arrive_dist)
	: key(key), move_speed(move_speed), turn_speed(turn_speed), arrive_dist(arrive_dist)
{

}

BTState MoveToNode::Tick(BTContext& context, float elapsed_time)
{
	if ((!context.owner)||(!context.blackboard)||(!context.blackboard->HasValue(key))) return BTState::Failure;

	DirectX::XMFLOAT3 target = context.blackboard->GetValue<DirectX::XMFLOAT3>(key);

	context.owner->MoveTo(target, move_speed, turn_speed, elapsed_time);

	DirectX::XMFLOAT3 pos = context.owner->GetPosition();

	float dx = target.x - pos.x;
	float dz = target.z - pos.z;
	float dist_sq = dx * dx + dz * dz;

	if (dist_sq <= arrive_dist * arrive_dist)
	{
		return BTState::Success;
	}

	return BTState::Run;
}
