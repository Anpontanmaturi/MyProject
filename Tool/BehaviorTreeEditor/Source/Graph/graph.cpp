#include "graph.h"
#include "../Node/editor_node.h"

void EditorGraph::AddNode(std::unique_ptr<EditorNode> node)
{
	if (node->GetNodeType() == NodeType::Root)
	{
		if (HasRoot())
		{
			return;
		}
	}

	nodes.push_back(std::move(node));
}

void EditorGraph::RemoveNode(int id)
{
	// ノード探し
	auto it = std::find_if(nodes.begin(), nodes.end(), [id](const std::unique_ptr<EditorNode>& node)
		{
			return node->GetId() == id;
		}
	);

	if (it == nodes.end()) { return; }

	EditorNode* target = it->get();

	// 削除対象か
	auto isTagpin = [target](EditorPin* pin)
		{
			if (pin == nullptr) { return false; }

			for (auto& input : target->GetInputPins())
			{
				if (pin == &input) { return true; }
			}

			for (auto& output : target->GetOutputPins())
			{
				if (pin == &output) { return true; }
			}

			return false;
		};

	// リンク削除
	links.erase(
		std::remove_if(links.begin(), links.end(), [&isTagpin](const EditorLink& link)
			{
				return isTagpin(link.from) || isTagpin(link.to);
			}
		),
		links.end()
	);

	if (isTagpin(drag_pin)) { drag_pin = nullptr; }

	// ノード本体を削除
	nodes.erase(it);
}

void EditorGraph::AddLink(EditorPin* from, EditorPin* to)
{
	if (from == nullptr || to == nullptr)
	{
		return;
	}

	EditorLink link;
	link.from = from;
	link.to = to;

	links.push_back(link);
}

bool EditorGraph::HasRoot() const
{
	for (const auto& node : nodes)
	{
		if (node->GetNodeType() == NodeType::Root)
		{
			return true;
		}
	}
}

void EditorGraph::Draw()
{
	DrawLines();

	DrawDragLine();

	DrawNodes();

	HandleConnect();
}

void EditorGraph::DrawNodes()
{
	std::vector<int> delete_id;

	for (auto& node : nodes)
	{
		node->Draw();

		if (node->WantDelete())
		{
			delete_id.push_back(node->GetId());
		}
	}

	for (int id : delete_id)
	{
		RemoveNode(id);
	}
}

void EditorGraph::DrawLines()
{
	ImDrawList* draw = ImGui::GetWindowDrawList();

	for (auto& link : links)
	{
		ImVec2 start = link.from->GetPosition();
		ImVec2 end = link.to->GetPosition();

		float offset = std::max(cp_offset, std::abs(end.x - start.x) * 0.5f);

		ImVec2 cp1{ start.x + offset, start.y };
		ImVec2 cp2{ end.x - offset, end.y };

		draw->AddBezierCurve(
			start,
			cp1,
			cp2,
			end,
			IM_COL32_WHITE,
			line_slender
		);
	}
}

void EditorGraph::DrawDragLine()
{
	if (!drag_pin)return;

	ImDrawList* draw = ImGui::GetWindowDrawList();

	ImVec2 mouse = ImGui::GetMousePos();

	ImVec2 start = drag_pin->GetPosition();
	ImVec2 end = mouse;

	float offset = std::max(cp_offset, std::abs(end.x - start.x) * 0.5f);

	ImVec2 cp1{ start.x + offset, start.y };
	ImVec2 cp2{ end.x - offset, end.y };

	draw->AddBezierCurve(
		start,
		cp1,
		cp2,
		end,
		IM_COL32_WHITE,
		line_slender
	);
}

void EditorGraph::HandleConnect()
{

	ImVec2 mouse = ImGui::GetMousePos();

	// クリック時
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		drag_pin = nullptr;

		for (auto& node : nodes)
		{
			for (auto& pin : node->GetOutputPins())
			{
				if (pin.HitTest(mouse))
				{
					drag_pin = &pin;
					return;
				}
			}
		}
	}

	// リリース時
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		if (drag_pin == nullptr)
			return;

		for (auto& node : nodes)
		{
			for (auto& pin : node->GetInputPins())
			{
				if (pin.HitTest(mouse))
				{
					if (!drag_pin->owner->CanAddTo())
						break;

					// 自身に接続しない
					if (pin.owner != drag_pin->owner)
					{
						EditorLink link;
						link.from = drag_pin;
						link.to = &pin;

						links.push_back(link);
					}

					drag_pin = nullptr;
					return;
				}
			}
		}

		drag_pin = nullptr;
	}
}

void EditorGraph::Clear()
{
	links.clear();
	nodes.clear();

	drag_pin = nullptr;
}

EditorNode* EditorGraph::FinedNodeById(int id)
{
	for (auto& node : nodes)
	{
		if (node->GetId() == id)
		{
			return node.get();
		}
	}

	return nullptr;
}

void EditorGraph::CreateNode(NodeType type, const ImVec2& position)
{
	int id = 0;

	for (const auto& node : nodes)
	{
		if (node->GetId() >= id)
		{
			id = node->GetId() + 1;
		}
	}

	std::string name;

	switch (type)
	{
	case NodeType::Root:          name = "Root"; break;
	case NodeType::Sequence:      name = "Sequence"; break;
	case NodeType::Selector:      name = "Selector"; break;
	case NodeType::Wait:          name = "Wait"; break;
	case NodeType::MoveTo:        name = "MoveTo"; break;
	case NodeType::Idle:          name = "Idle"; break;
	case NodeType::DistCondition: name = "DistCondition"; break;
	case NodeType::Inverter:      name = "Inverter"; break;
	case NodeType::Repeater:      name = "Repeater"; break;
	default: return;
	}

	auto node = std::make_unique<EditorNode>(id, name, type);
	node->SetPosition(position);

	AddNode(std::move(node));
}