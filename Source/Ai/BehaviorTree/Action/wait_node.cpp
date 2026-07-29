#include "wait_node.h"

WaitNode::WaitNode(float wait_time) :wait_time(wait_time)
{

}

BTState WaitNode::Tick(BTContext& context, float elapsed_time)
{
	current_time += elapsed_time;

	if (current_time >= wait_time)
	{
		current_time = 0.0f;
		return BTState::Success;
	}

	return BTState::Run;
}