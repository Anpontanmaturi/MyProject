#pragma once

#include "condition_node.h"
#include "../Blackboard/blackboard.h"

enum class DistanceCompare
{
	Less,			// ÅÉ
	LessEqual,		// ÅÖ
	Greater,		// ÅÑ
	GreaterEqual,	// ÅÜ
};

class DistConditionNode : public ConditionNode
{
public:
	DistConditionNode(
		const BlackboardKey& key,
		DistanceCompare compare,
		float dist);

	BTState Tick(
		BTContext& context,
		float elapsed_time) override;

private:
	BlackboardKey key;
	DistanceCompare compare;
	float dist = 0.0f;
};
