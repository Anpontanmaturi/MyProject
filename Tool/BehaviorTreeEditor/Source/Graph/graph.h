#pragma once

#include <memory>
#include <vector>

#include "../Node/editor_node.h" 
#include "../Link/editor_link.h"

class EditorGraph
{
public:
	void Draw();

	void AddNode(std::unique_ptr<EditorNode> node);

private:
	// 描画用
	void DrawNodes();
	void DrawLines();
	void DrawDragLine();

	// 入力
	void HandleConnect();

private:
	std::vector<std::unique_ptr<EditorNode>> nodes;

	std::vector<EditorLink> links;

	// 接続確認用
	EditorPin* drag_pin = nullptr;

	float line_slender = 3.0f;// 接続線用
	float cp_offset = 10.0f;
};