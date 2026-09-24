#pragma once

#include <string>
#include <imgui.h>
#include <vector>

#include "../Pin/editor_pin.h"

enum class NodeType
{
	Root,

	Sequence,
	Selector,

	Wait,
	MoveTo,
	Idle,

	Inverter,
	Repeater,
};

class EditorNode
{
public:
	EditorNode(int id, const std::string& name, NodeType type);

	virtual ~EditorNode() = default;

	virtual void Draw();

	int GetId() const { return id; }
	ImVec2 GetPosition() const { return position; }

	std::vector<EditorPin>& GetInputPins() { return input_pins; }
	std::vector<EditorPin>& GetOutputPins() { return output_pins; }

	NodeType GetNodeType() const { return type; }

	bool CanAddTo() const;// ê⁄ë±êÊçÏê¨Ç≈Ç´ÇÈÇ©

private:
	void WriteNode();

private:
	std::vector<EditorPin> input_pins;
	std::vector<EditorPin> output_pins;

	NodeType type;

protected:
	int id;
	std::string name;
	ImVec2 position{ 100, 100 };
	ImVec2 size{ 180, 80 };

};