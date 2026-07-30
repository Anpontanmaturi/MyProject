#pragma once

#include <memory>
#include <vector>

class EditorNode;

class EditorGraph
{
public:
	void Draw();

	void AddNode(std::unique_ptr<EditorNode> node);

private:
	std::vector<std::unique_ptr<EditorNode>> nodes;

};