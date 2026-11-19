#include "inverter_node.h"

BTState InverterNode::OnTick(BTContext& context, float elapsed_time)
{
	if (child == nullptr)
	{
		return BTState::Failure;
	}

	// 子ノードのTickを通常の仮想関数呼び出しで呼ぶ
	BTState state = child->Tick(context, elapsed_time);

	switch (state)
	{
	case BTState::Success:
		return BTState::Failure;
	case BTState::Failure:
		return BTState::Success;
	case BTState::Run:
		return BTState::Run;
	}

	return BTState::Failure;
}