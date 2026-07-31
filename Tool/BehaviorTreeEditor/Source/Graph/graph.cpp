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

	ImDrawList* draw = ImGui::GetWindowDrawList();

	ImVec2 mouse = ImGui::GetMousePos();
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		for (auto& node : nodes)
		{
			for (auto& pin : node->GetOutputPins())
			{
				if (pin.HitTest(mouse))
				{
					drag_pin = &pin;
					return;
				}
				else
				{
					drag_pin = nullptr;
					return;
				}
			}
		}
	}

	if (drag_pin)
	{
		draw->AddBezierCurve(
			drag_pin->GetPosition(),
			ImVec2(drag_pin->GetPosition().x, drag_pin->GetPosition().y),
			ImVec2(mouse.x, mouse.y),
			mouse,
			IM_COL32_WHITE,
			line_slender
		);
	}
}