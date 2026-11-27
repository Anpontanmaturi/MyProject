#include "decorator_node.h"

void DecoratorNode::SetChild(std::unique_ptr<BTNode> node)
{
	child = std::move(node);
}

void DecoratorNode::OnBreak(BTContext& context)
{
	if (child)
	{
		child->Break(context);
	}
}