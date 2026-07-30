#pragma once

#include <string>
#include <imgui.h>

class EditorNode
{
public:
	EditorNode(int id, const std::string& title);

	virtual ~EditorNode() = default;

	virtual void Draw();

	int GetId() const { return id; }

protected:
	int id;
	std::string title;
	ImVec2 position{ 100, 100 };
	ImVec2 size{ 180, 80 };

};