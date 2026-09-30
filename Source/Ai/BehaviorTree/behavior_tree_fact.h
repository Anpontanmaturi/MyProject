#pragma once

#include <memory>
#include <nlohmann/json.hpp>

class BTNode;

using json = nlohmann::json;

class BehaviorTreeFact
{
public:
	static std::unique_ptr<BTNode> CreateNode(int type, const nlohmann::json& node_data);
};