#include "editor.h"

#include "../Node/editor_node.h"
#include "Json/behavior_tree_converter.h"// ‰¼’u‚«
#include <imgui_internal.h>

BehaviorTreeEditor::BehaviorTreeEditor()
{
	graph.AddNode(std::make_unique<EditorNode>(0, "Sequence", NodeType::Sequence));

	graph.AddNode(std::make_unique<EditorNode>(1, "Wait", NodeType::Wait));

	graph.AddNode(std::make_unique<EditorNode>(2, "MoveTo", NodeType::MoveTo));

	graph.AddNode(std::make_unique<EditorNode>(3, "DistCondition", NodeType::DistCondition));

}

void BehaviorTreeEditor::Update(float elapsed_time)
{
	
}

void BehaviorTreeEditor::Draw()
{
	ImGui::Begin("Behavior Tree", nullptr, ImGuiWindowFlags_NoMove);

	graph.Draw();

	ImGui::End();

	ImGui::Begin("button", nullptr);

	ImGui::InputText("", save_file_name, sizeof(save_file_name));

	if (ImGui::Button("Save"))
	{
		BehaviorTreeConverter::Save(graph, save_file_name);
	}

	if (ImGui::Button("Load"))
	{
		BehaviorTreeConverter::Load(graph, "test.json");
	}

	ImGui::End();

	ImGui::BringWindowToDisplayFront(
		ImGui::FindWindowByName("button"));
}