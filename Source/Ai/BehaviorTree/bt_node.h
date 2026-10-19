#pragma once

#include "bt_state.h"
#include "bt_context.h"

class BTNode
{
public:
	virtual ~BTNode() = default;

	virtual BTState Tick(BTContext& context, float elapsed_time) = 0;

	virtual const char* GetTypeName() const = 0;

	void SetNodeId(int id) { node_id = id; }
	int GetNodeId()const { return node_id; }

private:
	int node_id;
};