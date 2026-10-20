#pragma once
#include "bt_debug_info.h"
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>
#include "bt_node_type.h"

void DrawBTDebugGui(
	const nlohmann::json& graph, const BTDebugInfo& debug_info);