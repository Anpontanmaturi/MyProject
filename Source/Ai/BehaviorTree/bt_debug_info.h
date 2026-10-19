#pragma once

#include <unordered_map>
#include "bt_state.h"

enum class BTDebugState
{
	NotExecuted,
	Run,
	Success,
	Failure
};

class BTDebugInfo
{
public:
	void SetState(int node_id, BTDebugState state)
	{
		node_states[node_id] = state;
	}

	BTDebugState GetState(int node_id)const
	{
		auto it = node_states.find(node_id);

		if (it == node_states.end())
		{
			return BTDebugState::NotExecuted;
		}
		return it->second;
	}

	void Clear()
	{
		node_states.clear();
	}

private:
	std::unordered_map<int, BTDebugState> node_states;
};

namespace BTDebug
{
	inline BTDebugState ToDebugState(BTState state)
	{
		switch (state)
		{
		case BTState::Success:
			return BTDebugState::Success;

		case BTState::Run:
			return BTDebugState::Run;

		case BTState::Failure:
			return BTDebugState::Failure;

		default:
			return BTDebugState::NotExecuted;
		}
	}
}
