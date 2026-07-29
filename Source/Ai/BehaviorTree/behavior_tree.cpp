#include "behavior_tree.h"
#include "bt_node.h"

void BehaviorTree::SetRoot(std::unique_ptr<BTNode> node)
{
	root = std::move(node);
}

BTState BehaviorTree::Tick(BTContext& context, float elapsed_time)
{
	if (!root) return BTState::Failure;

	return root->Tick(context,elapsed_time);
}