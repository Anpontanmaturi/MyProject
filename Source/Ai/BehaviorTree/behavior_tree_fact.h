#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "bt_node_type.h"

class BTNode;
class BehaviorTree;

using json = nlohmann::json;

class BehaviorTreeFact
{
public:
	static std::unique_ptr<BTNode> CreateNode(BTNodeType type, const nlohmann::json& node_data);

	static std::unique_ptr<BehaviorTree> CreateTree(const json& data);
};