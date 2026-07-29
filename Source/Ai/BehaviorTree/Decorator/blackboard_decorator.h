#pragma once

#include "decorator_node.h"
#include "../Blackboard/blackboard.h"

class BlackboardDecorator : public DecoratorNode
{
public:
	BlackboardDecorator(
		Blackboard* blackboard,
		const BlackboardKey& key
	);

	BTState Tick(BTContext& context, float elapsed_time) override;

private:
	Blackboard* blackboard;

	BlackboardKey key;
};
