#include "editor_node.h"

EditorNode::EditorNode(int id, const std::string& name) :id(id), name(name)
{
	input_pins.emplace_back(this, "In", PinType::Input);
	output_pins.emplace_back(this, "Out", PinType::Output);

	input_pins[0].SetLocalPosition({ 0, size.y * 0.5f });
	output_pins[0].SetLocalPosition({ size.x, size.y * 0.5f });
}

void EditorNode::Draw()
{
	ImGui::BeginGroup();

	WriteNode();

	ImGui::SetCursorScreenPos(position);

	ImGui::InvisibleButton(("Node" + std::to_string(id)).c_str(), size);

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		position.x += ImGui::GetIO().MouseDelta.x;
		position.y += ImGui::GetIO().MouseDelta.y;
	}

	ImGui::EndGroup();
}

void EditorNode::WriteNode()
{
	ImDrawList* draw = ImGui::GetWindowDrawList();

	ImVec2 p1 = position;
	ImVec2 p2 =
	{
		position.x + size.x,
		position.y + size.y
	};

	draw->AddRectFilled(p1, p2, IM_COL32(55, 55, 60, 255), 6.0f);

	draw->AddRect(p1, p2, IM_COL32(180, 180, 180, 255), 6.0f);

	draw->AddRectFilled(p1, ImVec2(p2.x, p1.y +25), IM_COL32(80, 110, 180, 255), 6.0f);

	draw->AddText(ImVec2(p1.x + 8, p1.y +5), IM_COL32_WHITE, name.c_str());

	for (auto& pin : input_pins)
	{
		pin.Draw();
	}

	for (auto& pin : output_pins)
	{
		pin.Draw();
	}
}