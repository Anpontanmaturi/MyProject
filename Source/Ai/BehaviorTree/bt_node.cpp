#include "bt_node.h"

BTState BTNode::Tick(BTContext& context, float elapsed_time)
{
	if (!once)
	{
		OnEnter(context);
		once = true;
	}

	BTState result = OnTick(context, elapsed_time);

	if (result != BTState::Run)
	{
		OnExit(context, result);
		once = false;
	}

	return result;
}

// ‰¼’u‚«
void BTNode::Break(BTContext& context)
{
	if (once)
	{
		OnBreak(context);
		once = false;
	}
}