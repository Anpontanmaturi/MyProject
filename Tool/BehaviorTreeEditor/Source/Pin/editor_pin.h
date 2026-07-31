#pragma once

#include <string>
#include <imgui.h>

class EditorNode;

enum class PinType
{
	Input,
	Output
};

class EditorPin
{
public:
	EditorPin(EditorNode* owner, const std::string& name, PinType type);

	void Draw();

	bool HitTest(ImVec2 mouse_pos) const;

	ImVec2 GetPosition() const;
	void SetLocalPosition(const ImVec2& pos) { local_position = pos; }

public:
	EditorNode* owner = nullptr;

	std::string name;

	PinType type;

private:
	ImVec2 local_position{};
	float pin_radius = 6.0f;
};