#pragma once

#include "action_node.h"
#include "../Blackboard/blackboard.h"

class StopNode :public ActionNode
{
public:
	BTState OnTick(BTContext& context, float elapsed_time)override;

};