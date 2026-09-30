#pragma once

#include <memory>

class BTNode;

class BehaviorTreeFact
{
public:
	static std::unique_ptr<BTNode> CreateNode(int type, const json& node_data);
};