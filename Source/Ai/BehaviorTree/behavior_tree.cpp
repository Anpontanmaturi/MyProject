#include "behavior_tree.h"
#include "bt_node.h"

void BehaviorTree::SetRoot(std::unique_ptr<BTNode> node)
{
	root = std::move(node);
}

BTState BehaviorTree::Tick(BTContext& context, float elapsed_time)
{
	if (!root) return BTState::Failure;

	debug_info.Clear();

	BTState state = root->Tick(context, elapsed_time);

	debug_info.SetState(root->GetNodeId(),
		state == BTState::Run ? BTDebugState::Run :
		state == BTState::Success ? BTDebugState::Success :
		BTDebugState::Failure);

	return state;
}