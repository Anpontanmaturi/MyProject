#include "editor_node.h"

EditorNode::EditorNode(int id, const std::string& name, NodeType type) :id(id), name(name), type(type)
{
	switch (type)
	{
	case NodeType::DistCondition:
		size = { 180, 120 };break;

	default:
		size = { 180, 80 }; break;
	}

	if (type != NodeType::Root)
	{
		input_pins.emplace_back(this, "In", PinType::Input);
	}
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

	switch (type)
	{
	case NodeType::Wait: {
		ImGui::SetCursorScreenPos(
			ImVec2(
				p1.x + 10,
				p1.y + 40));

		ImGui::PushItemWidth(100.0f);

		ImGui::DragFloat(
			"##WaitTime",
			&wait_time,
			0.1f,
			0.0f,
			100.0f,
			"%.1f");

		ImGui::PopItemWidth();
		break;
	}

	case NodeType::DistCondition: {
		ImGui::SetCursorScreenPos(
			ImVec2(
				p1.x + 10,
				p1.y + 35));

		ImGui::PushItemWidth(100.0f);

		// key
		char key_buffer[128];
		strcpy_s(key_buffer, dist_key.c_str());
		if (ImGui::InputText("Key", key_buffer, sizeof(key_buffer)))
		{
			dist_key = key_buffer;
		}

		// Compare
		const char* compare_items[] =
		{
			"<", "<=", ">", ">="
		};
		ImGui::Combo("Compare", &dist_compare, compare_items, IM_ARRAYSIZE(compare_items));

		// distance
		ImGui::InputFloat("Distance", &dist);

		ImGui::PopItemWidth();
		break;
	}

	default:
		break;
	}

	for (auto& pin : input_pins)
	{
		pin.Draw();
	}

	for (auto& pin : output_pins)
	{
		pin.Draw();
	}
}

bool EditorNode::CanAddTo() const
{
	switch (type)
	{
	case NodeType::Sequence:
	case NodeType::Selector:
		return true;

	default:
		return false;
	}
}