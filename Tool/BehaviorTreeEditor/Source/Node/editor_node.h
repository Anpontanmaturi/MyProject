#pragma once

#include <string>
#include <imgui.h>
#include <vector>

#include "../Pin/editor_pin.h"

enum class NodeType
{
	Root,

	Sequence, // è„Ç©ÇÁèáÇ…
	Selector, // íTÇ∑

	Wait,
	MoveTo,
	Idle,
	DistCondition,

	Inverter, // îΩì]
	Repeater, // åJÇËï‘Çµ
};

class EditorNode
{
public:
	EditorNode(int id, const std::string& name, NodeType type);

	virtual ~EditorNode() = default;

	virtual void Draw();

	int GetId() const { return id; }
	ImVec2 GetPosition() const { return position; }
	void SetPosition(const ImVec2& position) { this->position = position; }

	std::vector<EditorPin>& GetInputPins() { return input_pins; }
	std::vector<EditorPin>& GetOutputPins() { return output_pins; }

	NodeType GetNodeType() const { return type; }

	bool CanAddTo() const;// ê⁄ë±êÊçÏê¨Ç≈Ç´ÇÈÇ©

	float GetWaitTime() const { return wait_time; }
	void SetWaitTime(float value) { wait_time = value; }

	const std::string& GetDistKey() const { return dist_key; }
	void SetDistKey(const std::string& value) { dist_key = value; }
	int GetDistCompare()const { return dist_compare; }
	void SetDistCompare(int value) { dist_compare = value; }
	float GetDist()const { return dist; }
	void SetDist(float value) { dist = value; }

private:
	void WriteNode();

private:
	std::vector<EditorPin> input_pins;
	std::vector<EditorPin> output_pins;

	NodeType type;

	float wait_time = 1.0f;

	// DistConditionóp
	std::string dist_key = "PlayerPosition";
	int dist_compare = 2;
	float dist = 5.0f;

protected:
	int id;
	std::string name;
	ImVec2 position{ 100, 100 };
	ImVec2 size{};

};