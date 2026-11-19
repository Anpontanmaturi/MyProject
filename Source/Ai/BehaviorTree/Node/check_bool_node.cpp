#include "check_bool_node.h"

BTState CheckBoolNode::OnTick(
	BTContext& context,
	float elapsed_time)
{
	bool value = context.blackboard->GetValue<bool>(key);

	return value ? BTState::Success : BTState::Failure;
}