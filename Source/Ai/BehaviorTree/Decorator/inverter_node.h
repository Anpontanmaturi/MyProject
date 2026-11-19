#pragma once

#include "decorator_node.h"

class InverterNode : public DecoratorNode
{
public:
	BTState OnTick(BTContext& context, float elapsed_time)override;
};