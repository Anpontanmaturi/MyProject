#include "editor.h"

#include "../Node/editor_node.h"

BehaviorTreeEditor::BehaviorTreeEditor()
{
	graph.AddNode(std::make_unique<EditorNode>(0, "Sequence"));

	graph.AddNode(std::make_unique<EditorNode>(1, "Wait"));

	graph.AddNode(std::make_unique<EditorNode>(2, "MoveTo"));

}

void BehaviorTreeEditor::Update()
{
	ImGui::Begin("Behavior Tree");

	graph.Draw();

	ImGui::End();
}