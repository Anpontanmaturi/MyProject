#pragma once

#include "../Graph/graph.h"

class BehaviorTreeEditor
{
public:
	BehaviorTreeEditor();

	void Update(float elapsed_time);
	void Draw();

private:
	EditorGraph graph;

};