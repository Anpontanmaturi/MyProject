#pragma once

class EditorGraph;

class BehaviorTreeConverter
{
public:
	static bool Save(
		const EditorGraph& graph,
		const char* filename);

	static bool Load(
		EditorGraph& graph,
		const char* filename);
};