#pragma once

#include "../bt_node.h"

class IdleNode : public BTNode
{
public:
	BTState OnTick(
		BTContext& context,
		float elapsed_time
	)override;
};