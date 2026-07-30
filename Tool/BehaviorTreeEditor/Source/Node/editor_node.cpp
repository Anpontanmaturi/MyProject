#include "editor_node.h"

EditorNode::EditorNode(int id, const std::string& title) :id(id), title(title)
{
}

void EditorNode::Draw()
{
	ImGui::SetCursorScreenPos(position);

	ImGui::BeginGroup();

	ImGui::Button(title.c_str(), size);

	ImGui::EndGroup();
}