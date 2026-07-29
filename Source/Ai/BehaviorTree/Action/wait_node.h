#pragma once

#include "action_node.h"

class WaitNode : public ActionNode
{
public:
	explicit WaitNode(float wait_node);

	BTState Tick(
		BTContext& context, float elapsed_time) override;

private:
	float wait_time = 0.0f;
	float current_time = 0.0f;
};
