#pragma once

#include <string>
#include <imgui.h>
#include <vector>

#include "../Pin/editor_pin.h"

class EditorNode
{
public:
	EditorNode(int id, const std::string& name);

	virtual ~EditorNode() = default;

	virtual void Draw();

	int GetId() const { return id; }
	ImVec2 GetPosition() const { return position; }

	std::vector<EditorPin>& GetInputPins() { return input_pins; }
	std::vector<EditorPin>& GetOutputPins() { return output_pins; }

private:
	void WriteNode();

private:
	std::vector<EditorPin> input_pins;
	std::vector<EditorPin> output_pins;

protected:
	int id;
	std::string name;
	ImVec2 position{ 100, 100 };
	ImVec2 size{ 180, 80 };

};