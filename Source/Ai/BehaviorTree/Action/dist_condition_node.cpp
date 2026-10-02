#include "dist_condition_node.h"

#include "../bt_context.h"
#include "Character/character.h"

#include <DirectXMath.h>

DistConditionNode::DistConditionNode(
	const BlackboardKey& key,
	DistanceCompare compare,
	float dist) :key(key), compare(compare), dist(dist)
{

}

BTState DistConditionNode::Tick(
	BTContext& context,
	float elapsed_time)
{
	if (!context.owner || !context.blackboard || !context.blackboard->HasValue(key))
	{
		return BTState::Failure;
	}

	DirectX::XMFLOAT3 target = context.blackboard->GetValue<DirectX::XMFLOAT3>(key);

	DirectX::XMFLOAT3 position = context.owner->GetPosition();

	float dx = target.x - position.x;
	float dz = target.z - position.z;

	float dist_sq = dx * dx + dz * dz;

	float compare_dist_sq = dist * dist;
	bool result = false;

	switch (compare)
	{
	case DistanceCompare::Less:
		result = dist_sq < compare_dist_sq;
		break;

	case DistanceCompare::LessEqual:
		result = dist_sq <= compare_dist_sq;
		break;

	case DistanceCompare::Greater:
		result = dist_sq > compare_dist_sq;
		break;

	case DistanceCompare::GreaterEqual:
		result = dist_sq >= compare_dist_sq;
		break;
	}

	return result ? BTState::Success : BTState::Failure;
}