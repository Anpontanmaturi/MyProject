#include "behavior_tree_converter.h"

#include "../Graph/graph.h"
#include "../Node/editor_node.h"
#include "../Link/editor_link.h"

#include "nlohmann/json.hpp"

#include <fstream>
#include <Windows.h>
#include <ShlObj.h>
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

using json = nlohmann::json;

std::string GetNodeName(NodeType type)// ラベルの名前設定
{
	switch (type)
	{
	case NodeType::Root:
		return "Root";

	case NodeType::Sequence:
		return "Sequence";

	case NodeType::Selector:
		return "Selector";

	case NodeType::Wait:
		return "Wait";

	case NodeType::MoveTo:
		return "MoveTo";

	case NodeType::Idle:
		return "Idle";

	case NodeType::DistCondition:
		return "DistCondition";

	case NodeType::Inverter:
		return "Inverter";

	case NodeType::Repeater:
		return "Repeater";

	default:
		return "Unknown";
	}
}

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

		case NodeType::MoveTo:
			node_data["properties"]["key"] = node->GetMoveToKey();
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

	std::string path = SelectJsonSavePath();

	if (path.empty()) { return false; }

	std::string file_path = path + "\\" + filename;

	if (file_path.size() < 5 || file_path.substr(file_path.size() - 5) != ".json")
	{
		file_path += ".json";
	}

	std::ofstream file(file_path); // 書き出し

	if (!file)
	{
		return false;
	}

	file << data.dump(4);

	return true;
}

bool BehaviorTreeConverter::Load(
	EditorGraph& graph)
{
	std::string path = SelectJsonLoadPath();

	if (path.empty()) { return false; }

	std::ifstream file(path);

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

		auto node = std::make_unique<EditorNode>(
			id,
			GetNodeName(type),
			type);

		// その他
		switch (type)
		{
		case NodeType::Wait:
			node->SetWaitTime(node_data["properties"].value("wait_time", 1.0f));
			break;

		case NodeType::MoveTo:
			node->SetMoveToKey(node_data["properties"].value("key", std::string("PlayerPosition")));
			break;

		case NodeType::DistCondition:
			node->SetDistKey(node_data["properties"].value("key", std::string("PlayerPosition")));
			node->SetDistCompare(node_data["properties"].value("compare", 2));
			node->SetDist(node_data["properties"].value("distance", 5.0f));
			break;

		default:
			break;

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

std::string BehaviorTreeConverter::SelectJsonSavePath()
{
	BROWSEINFOA sf{};
	sf.lpszTitle = "JSONの保存先を選択";
	sf.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST item_list = SHBrowseForFolderA(&sf);

	if (item_list == nullptr) { return{}; }

	char folder[MAX_PATH]{};

	bool success = SHGetPathFromIDListA(item_list, folder);

	CoTaskMemFree(item_list);

	if (!success) { return {}; }

	return folder;
}

std::string BehaviorTreeConverter::SelectJsonLoadPath()
{
	char file_path[MAX_PATH]{};

	OPENFILENAMEA lf{};
	lf.lStructSize = sizeof(lf);
	lf.lpstrFile = file_path;
	lf.nMaxFile = MAX_PATH;
	lf.lpstrFilter =
		"JSONファイル (*.json)\0*.json\0"
		"すべてのファイル (*.*)\0*.*\0";
	lf.nFilterIndex = 1;
	lf.lpstrTitle = "読み込むJSONファイルを選択";
	lf.Flags = OFN_FILEMUSTEXIST
		| OFN_PATHMUSTEXIST
		| OFN_NOCHANGEDIR;

	char initial_dir[MAX_PATH]{};

	// 読み込み先のファイル指定
	GetModuleFileNameA(nullptr, initial_dir, MAX_PATH);
	PathRemoveFileSpecA(initial_dir);
	PathRemoveFileSpecA(initial_dir);
	PathRemoveFileSpecA(initial_dir);
	PathAppendA(initial_dir, "Data\\BehaviorTree");
	lf.lpstrInitialDir = initial_dir;

	if (!GetOpenFileNameA(&lf)){return {};}

	return file_path;
}