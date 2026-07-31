#include "editor.h"

#include "../Node/editor_node.h"

BehaviorTreeEditor::BehaviorTreeEditor()
{
	graph.AddNode(std::make_unique<EditorNode>(0, "Sequence"));

	graph.AddNode(std::make_unique<EditorNode>(1, "Wait"));

	graph.AddNode(std::make_unique<EditorNode>(2, "MoveTo"));

}

void BehaviorTreeEditor::Update(float elapsed_time)
{
	
}

void BehaviorTreeEditor::Draw()
{
	ImGui::Begin("Behavior Tree", nullptr, ImGuiWindowFlags_NoMove);

	graph.Draw();

	ImGui::End();
}