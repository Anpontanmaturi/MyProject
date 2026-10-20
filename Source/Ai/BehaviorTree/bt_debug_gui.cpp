#include "bt_debug_gui.h"

void DrawBTDebugGui(
	const nlohmann::json& graph, const BTDebugInfo& debug_info)
{
	ImGui::Begin("Behavior Tree Debug");

	bool running = false;

	if (graph.contains("nodes") && graph["nodes"].is_array())
	{
		for (const auto& node : graph["nodes"])
		{
			if (!node.contains("id") || !node.contains("type")) { continue; }

			const int id = node["id"].get<int>();
			const BTNodeType type = node["type"].get<BTNodeType>();

			const BTDebugState state = debug_info.GetState(id);
			const char* node_name = "";

			// 仮置き
			// ノード追加時変更する
			switch (type)
			{
			case BTNodeType::Root: node_name = "Root"; break;
			case BTNodeType::Sequence: node_name = "Sequence"; break;
			case BTNodeType::Selector: node_name = "Selector"; break;
			case BTNodeType::Wait: node_name = "Wait"; break;
			case BTNodeType::MoveTo: node_name = "MoveTo"; break;
			case BTNodeType::Idle: node_name = "Idle"; break;
			case BTNodeType::DistCondition: node_name = "DistCondition"; break;
			case BTNodeType::Inverter: node_name = "Inverter"; break;
			case BTNodeType::Repeater: node_name = "Repeater"; break;
			}

			if (state == BTDebugState::Run)
			{
				running = true;

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.3f, 1.0f));
				ImGui::Text("Now Running : %s (ID: %d)", node_name, id);
				ImGui::PopStyleColor();
			}
		}
	}

	if (!running)
	{
		ImGui::TextUnformatted("Not running node.");
	}

	ImGui::End();
}