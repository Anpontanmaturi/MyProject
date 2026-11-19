#pragma once

#include "condition_node.h"
#include "../Blackboard/blackboard.h"

class CheckBoolNode : public ConditionNode
{
public:
	CheckBoolNode(BlackboardKey key):key(key){}

	BTState OnTick(
		BTContext& context,
		float elapsed_time) override;

private:
	BlackboardKey key;
};