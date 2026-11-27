#include "set_bool_node.h"

SetBoolNode::SetBoolNode(const BlackboardKey& key, bool value) :key(key), value(value)
{

}

BTState SetBoolNode::OnTick(BTContext& context, float elapsed_time)
{
	if (!context.owner || !context.blackboard || !context.blackboard->HasValue(key))
	{
		return BTState::Failure;
	}

	context.blackboard->SetValue<bool>(key, value);

	return BTState::Success;
}