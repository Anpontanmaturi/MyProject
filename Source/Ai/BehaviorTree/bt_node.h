#pragma once

#include "bt_state.h"
#include "bt_context.h"

class BTNode
{
public:
	virtual ~BTNode() = default;

	virtual BTState Tick(BTContext& context, float elapsed_time);
	void Break(BTContext& context);

	void SetNodeId(int id) { node_id = id; }
	int GetNodeId()const { return node_id; }

protected:
	virtual void OnEnter(BTContext& context) {}
	virtual BTState OnTick(BTContext& context, float elapsed_time) = 0;
	virtual void OnExit(BTContext& context, BTState result) {}
	virtual void OnBreak(BTContext& context){}

private:
	int node_id;
	bool once = false;
};