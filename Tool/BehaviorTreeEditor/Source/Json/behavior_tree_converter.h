#pragma once

#include <string>

class EditorGraph;

class BehaviorTreeConverter
{
public:
	static bool Save(
		const EditorGraph& graph,
		const char* filename);

	static bool Load(
		EditorGraph& graph);

	static std::string SelectJsonSavePath();
	static std::string SelectJsonLoadPath();
};