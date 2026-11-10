#pragma once

#include <string>
#include <imgui.h>
#include <vector>
#include <variant>

#include "../Pin/editor_pin.h"
#include "Blackboard/blackboard_keys.h"

enum class NodeType
{
	// ƒm[ƒh’Ç‰Á•ÏX‚·‚é
	Root,

	Sequence, // ã‚©‚ç‡‚É
	Selector, // ’T‚·

	Wait,		// s“®Œn
	MoveTo,
	Idle,
	ShotStreight,

	DistCondition, // ğŒŒn

	Inverter, // ”½“]
	Repeater, // ŒJ‚è•Ô‚µ
};

struct WaitData
{
	float wait_time = 1.0f;
};

struct DistConditionData
{
	std::string key = {};
	int compare = 2;
	float dist = 5.0f;
};

struct MoveToData
{
	std::string key = {};
};

struct ShotAttackData
{
	std::string key = {};
	float shot_interval = 0.1f;
	float attack_time = 1.0f;
};

using EditorNodeData = std::variant<
	// ƒm[ƒh’Ç‰Á•ÏX
	std::monostate,
	WaitData,
	DistConditionData,
	MoveToData,
	ShotAttackData
>;

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

	bool CanAddTo() const;// Ú‘±æì¬‚Å‚«‚é‚©

	bool WantDelete() const { return delete_request; }

	EditorNodeData& GetData() { return data; }

private:
	void WriteNode();

	void DrawBlackboardKeyCombo(
		const char* label,
		std::string& select_key,
		const std::vector<BlackboardValueType>& required_type);

private:
	std::vector<EditorPin> input_pins;
	std::vector<EditorPin> output_pins;

	NodeType type;
	EditorNodeData data;

	bool delete_request = false;

protected:
	int id;
	std::string name;
	ImVec2 position{ 100, 100 };
	ImVec2 size{};

};