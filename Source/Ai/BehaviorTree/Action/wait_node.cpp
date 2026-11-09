#include "wait_node.h"

WaitNode::WaitNode(float wait_time) :wait_time(wait_time)
{

}

void WaitNode::OnEnter(BTContext& context)
{
	current_time = 0.0f;
}

BTState WaitNode::OnTick(BTContext& context, float elapsed_time)
{
	current_time += elapsed_time;

	if (current_time >= wait_time)
	{
		return BTState::Success;
	}

	return BTState::Run;
}