#include "graph.h"
#include "../Node/editor_node.h"

void EditorGraph::AddNode(std::unique_ptr<EditorNode> node)
{
	nodes.push_back(std::move(node));
}

void EditorGraph::Draw()
{
	for (auto& node : nodes)
	{
		node->Draw();
	}
}