#pragma once

#include "../bt_node.h"

#include <memory>

class DecoratorNode : public BTNode
{
public:
	virtual ~DecoratorNode() = default;

	void SetChild(std::unique_ptr<BTNode> node);

protected:
	std::unique_ptr<BTNode> child;
};