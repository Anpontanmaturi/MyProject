#pragma once

#include <string>

class EditorGraph;

class BehaviorTreeConverter
{
public:
	static bool Save(
		const EditorGraph& graph,
		const std::string& file_path);

	static bool SaveAs(
		const EditorGraph& graph,
		const char* file_name,
		std::string& save_path);

	static bool Load(
		EditorGraph& graph,
		std::string& load_path);

	static std::string SelectJsonSavePath();
	static std::string SelectJsonLoadPath();
};