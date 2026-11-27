#pragma once

#include "set_node.h"
#include "../Blackboard/blackboard.h"

class SetBoolNode : public SetNode
{
public:
	SetBoolNode(
		const BlackboardKey& key,
		bool value
	);

	BTState OnTick(
		BTContext& context,
		float elapsed_time)override;

private:
	BlackboardKey key;
	bool value;
};