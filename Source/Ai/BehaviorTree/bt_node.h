#pragma once

#include "bt_state.h"

class BTNode
{
public:
	virtual ~BTNode() = default;

	virtual BTState Tick(float elapsed_time) = 0;
};