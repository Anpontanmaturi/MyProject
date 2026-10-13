#include "editor.h"

#include "../Node/editor_node.h"
#include "Json/behavior_tree_converter.h"// âºíuÇ´
#include <imgui_internal.h>

BehaviorTreeEditor::BehaviorTreeEditor()
{

}

void BehaviorTreeEditor::Update(float elapsed_time)
{
	
}

void BehaviorTreeEditor::Draw()
{
	ImGui::Begin("Behavior Tree", nullptr, ImGuiWindowFlags_NoMove);

	graph.Draw();

    if (ImGui::IsWindowHovered() &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("NodeContextMenu");
    }

    if (ImGui::BeginPopup("NodeContextMenu"))
    {
        ImVec2 mouse_pos = ImGui::GetMousePos();

        if (ImGui::BeginMenu("ÉmÅ[ÉhÇí«â¡"))
        {
            if (ImGui::MenuItem("Sequence"))
                graph.CreateNode(NodeType::Sequence, mouse_pos);

            if (ImGui::MenuItem("Selector"))
                graph.CreateNode(NodeType::Selector, mouse_pos);

            if (ImGui::MenuItem("Wait"))
                graph.CreateNode(NodeType::Wait, mouse_pos);

            if (ImGui::MenuItem("MoveTo"))
                graph.CreateNode(NodeType::MoveTo, mouse_pos);

            if (ImGui::MenuItem("Idle"))
                graph.CreateNode(NodeType::Idle, mouse_pos);

            if (ImGui::MenuItem("DistCondition"))
                graph.CreateNode(NodeType::DistCondition, mouse_pos);

            if (ImGui::MenuItem("Inverter"))
                graph.CreateNode(NodeType::Inverter, mouse_pos);

            if (ImGui::MenuItem("Repeater"))
                graph.CreateNode(NodeType::Repeater, mouse_pos);

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

	ImGui::End();

	ImGui::Begin("button", nullptr);

	ImGui::InputText("", save_file_name, sizeof(save_file_name));

	if (ImGui::Button("Save"))
	{
		BehaviorTreeConverter::Save(graph, save_file_name);
	}

	if (ImGui::Button("Load"))
	{
		BehaviorTreeConverter::Load(graph);
	}

	ImGui::End();

	ImGui::BringWindowToDisplayFront(
		ImGui::FindWindowByName("button"));
}