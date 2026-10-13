#pragma once

#include "../bt_node.h"

class IdleNode : public BTNode
{
public:
	BTState Tick(
		BTContext& context,
		float elapsed_time
	)override;
};