#include "editor_node.h"

namespace
{
	const BlackboardKey* BlackboardKeyList[] =
	{
		&BlackboardKeys::PlayerPosition,
		&BlackboardKeys::Player,
		&BlackboardKeys::Enemy
	};
}

EditorNode::EditorNode(int id, const std::string& name, NodeType type) :id(id), name(name), type(type)
{
	size = { 180, 80 };

	// ノード追加時変更する
	switch (type)
	{
	case NodeType::Wait:
		data = WaitData{};
		break;

	case NodeType::DistCondition:
		data = DistConditionData{};
		size = { 180, 140 };
		break;

	case NodeType::MoveTo:
		data = MoveToData{};
		break;

	case NodeType::ShotStreight:
		data = ShotAttackData{};
		size = { 220, 140 };
		break;

	default:
		data = std::monostate{};
		break;
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

	std::string popup_id = "NodePopup##" + std::to_string(id);
	if (ImGui::BeginPopupContextItem(popup_id.c_str()))
	{
		if (ImGui::MenuItem("delete"))
		{
			delete_request = true;
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		position.x += ImGui::GetIO().MouseDelta.x;
		position.y += ImGui::GetIO().MouseDelta.y;
	}

	ImGui::EndGroup();
}

void EditorNode::WriteNode()
{
	ImGui::PushID(id);

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

	// ノード追加時変更する
	switch (type)
	{
	case NodeType::Wait: {
		ImGui::SetCursorScreenPos(
			ImVec2(
				p1.x + 10,
				p1.y + 40));

		ImGui::PushItemWidth(100.0f);

		if (auto* wait = std::get_if<WaitData>(&data))
		{
			ImGui::DragFloat(
				"WaitTime",
				&wait->wait_time,
				0.1f,
				0.0f,
				100.0f,
				"%.1f");
		}

		ImGui::PopItemWidth();
		break;
	}

	case NodeType::DistCondition: {

		ImGui::SetCursorScreenPos(
			ImVec2(
				p1.x + 10,
				p1.y + 35));

		ImGui::BeginGroup();

		ImGui::PushItemWidth(100.0f);

		if (auto* dist_condition = std::get_if<DistConditionData>(&data))
		{
			// key
			DrawBlackboardKeyCombo(
				"Key",
				dist_condition->key,
				{ BlackboardValueType::Float3 }
			);

			// Compare
			const char* compare_items[] =
			{
				"<", "<=", ">", ">="
			};
			ImGui::Combo("Compare", &dist_condition->compare, compare_items, IM_ARRAYSIZE(compare_items));

			// distance
			ImGui::InputFloat("Distance", &dist_condition->dist);
		}

		ImGui::PopItemWidth();

		ImGui::EndGroup();

		break;
	}

	case NodeType::MoveTo: {

		ImGui::SetCursorScreenPos(
			ImVec2(p1.x + 10, p1.y + 35));

		ImGui::BeginGroup();

		if (auto* move_to = std::get_if<MoveToData>(&data))
		{
			DrawBlackboardKeyCombo(
				"Target Position",
				move_to->key,
				{ BlackboardValueType::Float3,
				  BlackboardValueType::Character }
			);
		}

		ImGui::EndGroup();

		break;
	}

	case NodeType::ShotStreight: {

		ImGui::SetCursorScreenPos(
			ImVec2(p1.x + 10, p1.y + 35));

		ImGui::BeginGroup();

		ImGui::PushItemWidth(100.0f);
		
		if (auto* shot = std::get_if<ShotAttackData>(&data))
		{
			DrawBlackboardKeyCombo(
				"Target",
				shot->key,
				{ BlackboardValueType::Float3 }
			);

			ImGui::InputFloat("Shot Interval", &shot->shot_interval);
			ImGui::InputFloat("Attack Time", &shot->attack_time);

		}
		ImGui::PopItemWidth();

		ImGui::EndGroup();

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

	ImGui::PopID();
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

void EditorNode::DrawBlackboardKeyCombo(
	const char* label,
	std::string& select_key,
	const std::vector<BlackboardValueType>& required_type)
{
	ImGui::Text("%s", label);

	ImGui::PushItemWidth(140.0f);

	if (ImGui::BeginCombo((std::string("##") + label).c_str(), select_key.c_str()))
	{
		for (const BlackboardKey* key : BlackboardKeyList)
		{
			bool parmission = std::find(
				required_type.begin(),
				required_type.end(),
				key->type
			) != required_type.end();

			if (!parmission) continue;

			bool selected = (select_key == key->name);

			if (ImGui::Selectable(key->name.c_str(), selected))
			{
				select_key = key->name;
			}
			if (selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}