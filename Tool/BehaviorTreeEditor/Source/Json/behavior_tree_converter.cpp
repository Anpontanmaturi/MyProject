#include "behavior_tree_converter.h"

#include "../Graph/graph.h"
#include "../Node/editor_node.h"
#include "../Link/editor_link.h"

#include "nlohmann/json.hpp"

#include <fstream>

using json = nlohmann::json;

bool BehaviorTreeConverter::Save(
	const EditorGraph& graph,
	const char* filename)
{
	json data;

	data["nodes"] = json::array();
	data["links"] = json::array();

	// ノード保存
	for (const auto& node : graph.GetNodes())
	{
		json node_data;

		node_data["id"] = node->GetId();
		node_data["type"] = static_cast<int>(node->GetNodeType());
		node_data["position"]["x"] = node->GetPosition().x;
		node_data["position"]["y"] = node->GetPosition().y;
		switch (node->GetNodeType())
		{
		case NodeType::Wait:
			node_data["properties"]["wait_time"] = node->GetWaitTime();
			break;

		case NodeType::DistCondition:
			node_data["properties"]["key"] = node->GetDistKey();
			node_data["properties"]["compare"] = node->GetDistCompare();
			node_data["properties"]["distance"] = node->GetDist();
			break;

		default:
			break;
		}

		data["nodes"].push_back(node_data);
	}

	// リンク保存
	for (const auto& link : graph.GetLinks())
	{
		if (link.from == nullptr || link.to == nullptr)
		{
			continue;
		}

		json link_data;

		link_data["from"] = link.from->owner->GetId();
		link_data["to"] = link.to->owner->GetId();
		data["links"].push_back(link_data);
	}

	std::ofstream file(filename); // 書き出し

	if (!file)
	{
		return false;
	}

	file << data.dump(4);

	return true;
}

bool BehaviorTreeConverter::Load(
	EditorGraph& graph,
	const char* filename)
{
	std::ifstream file(filename);

	if (!file)
	{
		return false;
	}

	json data;
	file >> data;

	// ノードを消す
	graph.Clear();

	for (const auto& node_data : data["nodes"])
	{
		int id = node_data["id"];

		NodeType type = static_cast<NodeType>(node_data["type"]);

		std::string name;

		switch (type)
		{
		case NodeType::Root:
			name = "Root";
			break;

		case NodeType::Sequence:
			name = "Sequence";
			break;

		case NodeType::Selector:
			name = "Selector";
			break;

		case NodeType::Wait:
			name = "Wait";
			break;

		case NodeType::MoveTo:
			name = "MoveTo";
			break;

		case NodeType::Idle:
			name = "Idle";
			break;

		case  NodeType::Inverter:
			name = "Inverter";
			break;

		case NodeType::Repeater:
			name = "Repeater";
			break;
		}

		auto node = std::make_unique<EditorNode>(
			id,
			name,
			type);

		if (type == NodeType::Wait)
		{
			float wait_time = node_data["properties"].value("wait_time", 1.0f);
			node->SetWaitTime(wait_time);
		}

		node->SetPosition({ node_data["position"]["x"],node_data["position"]["y"] });

		// ノード追加
		graph.AddNode(std::move(node));
	}

	for (const auto& link_data : data["links"])
	{
		int from_id = link_data["from"];
		int to_id = link_data["to"];

		EditorNode* from_node = graph.FinedNodeById(from_id);

		EditorNode* to_node = graph.FinedNodeById(to_id);

		if (from_node == nullptr || to_node == nullptr)
		{
			continue;
		}

		auto& outputs = from_node->GetOutputPins();

		auto& inputs = to_node->GetInputPins();

		if (outputs.empty() || inputs.empty())
		{
			continue;
		}

		graph.AddLink(&outputs[0], &inputs[0]);
	}

	return true;
}