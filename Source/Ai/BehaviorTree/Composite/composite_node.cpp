#include "composite_node.h"

void CompositeNode::AddChild(std::unique_ptr<BTNode> child)
{
	children.emplace_back(std::move(child));
}