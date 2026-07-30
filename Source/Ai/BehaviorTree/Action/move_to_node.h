#pragma once

#include "action_node.h"
#include "../Blackboard/blackboard.h"

class MoveToNode : public ActionNode
{
public:
	MoveToNode(
		const BlackboardKey& key,
		float move_speed,
		float turn_speed,
		float arrive_dist = 0.2f);

	BTState Tick(BTContext& context, float elapsed_time) override;

private:
	BlackboardKey key;

	float move_speed;
	float turn_speed;
	float arrive_dist;
};