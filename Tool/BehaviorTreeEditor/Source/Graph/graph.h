#pragma once

#include <memory>
#include <vector>

#include "../Node/editor_node.h" 

class EditorGraph
{
public:
	void Draw();

	void AddNode(std::unique_ptr<EditorNode> node);

private:
	std::vector<std::unique_ptr<EditorNode>> nodes;

	// 接続確認用
	EditorPin* drag_pin = nullptr;

	float line_slender = 3.0f;// 接続線用
};