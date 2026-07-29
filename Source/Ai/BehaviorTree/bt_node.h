#pragma once

#include "bt_state.h"
#include "bt_context.h"

class BTNode
{
public:
	virtual ~BTNode() = default;

	virtual BTState Tick(BTContext& context, float elapsed_time) = 0;
};