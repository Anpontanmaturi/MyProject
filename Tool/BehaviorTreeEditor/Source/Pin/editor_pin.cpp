#include "editor_pin.h"
#include "../Node/editor_node.h"

EditorPin::EditorPin(EditorNode* owner, const std::string& name, PinType type):owner(owner), name(name), type(type)
{ }

void EditorPin::Draw()
{
	ImDrawList* draw = ImGui::GetWindowDrawList();

	ImVec2 pos = GetPosition();

	ImU32 color = (type == PinType::Input) ? IM_COL32(220, 0, 0, 255) : IM_COL32(0, 255, 0, 255);

	draw->AddCircleFilled(pos, 6.0f, color);
}

bool EditorPin::HitTest(ImVec2 mouse_pos) const
{
	ImVec2 pos = GetPosition();

	float dx = mouse_pos.x - pos.x;
	float dy = mouse_pos.y - pos.y;

	return dx * dx + dy * dy <= pin_radius * pin_radius;
}

ImVec2 EditorPin::GetPosition() const
{
	return
	{
		owner->GetPosition().x + local_position.x,
		owner->GetPosition().y + local_position.y
	};
}