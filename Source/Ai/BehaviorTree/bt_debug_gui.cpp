#include "bt_debug_info.h"
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

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
			const int type = node["type"].get<int>();

			const BTDebugState state = debug_info.GetState(id);
			const char* node_name = "";

			switch (type)
			{
			case 0: node_name = "Root"; break;
			case 1: node_name = "Sequence"; break;
			case 2: node_name = "Selector"; break;
			case 3: node_name = "Wait"; break;
			case 4: node_name = "MoveTo"; break;
			case 5: node_name = "Idle"; break;
			case 6: node_name = "DistCondition"; break;
			case 7: node_name = "Inverter"; break;
			case 8: node_name = "Repeater"; break;
			}


		}
	}
}