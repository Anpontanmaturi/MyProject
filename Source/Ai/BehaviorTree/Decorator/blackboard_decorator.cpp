#include "blackboard_decorator.h"

BlackboardDecorator::BlackboardDecorator(
	Blackboard* blackboard, const BlackboardKey& key) : blackboard(blackboard), key(key)
{

}

BTState BlackboardDecorator::Tick(BTContext& context, float elapsed_time)
{
	if (!blackboard->GetValue<bool>(key))
	{
		return BTState::Failure;
	}

	if (!child)
	{
		return BTState::Success;
	}

	return child->Tick(context, elapsed_time);
}