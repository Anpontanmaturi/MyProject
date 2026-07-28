#pragma once

#include "../bt_node.h"

#include <memory>
#include <vector>

class CompositeNode : public BTNode
{
public:
	virtual ~CompositeNode() = default;

	void AddChild(std::unique_ptr<BTNode> child);

protected:
	std::vector<std::unique_ptr<BTNode>> children;
};