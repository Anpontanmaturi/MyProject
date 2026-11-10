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
	// ノード追加時変更する
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

	case NodeType::ShotStreight:
		return "ShotStreight";

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
	const std::string& file_path)
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
		case NodeType::Wait: {
			const auto& data = std::get<WaitData>(node->GetData()); // 取り出し
			node_data["properties"]["wait_time"] = data.wait_time;
			break;
		}

		case NodeType::MoveTo: {
			const auto& data = std::get<MoveToData>(node->GetData());
			node_data["properties"]["key"] = data.key;
			break;
		}

		case NodeType::DistCondition: {
			const auto& data = std::get<DistConditionData>(node->GetData());
			node_data["properties"]["key"] = data.key;
			node_data["properties"]["compare"] = data.compare;
			node_data["properties"]["distance"] = data.dist;
			break;
		}

		case NodeType::ShotStreight: {
			const auto& data = std::get<ShotAttackData>(node->GetData());
			node_data["properties"]["key"] = data.key;
			node_data["properties"]["shot_interval"] = data.shot_interval;
			node_data["properties"]["attack_time"] = data.attack_time;
			break;
		}

		default:
			break;
		}

		data["nodes"].push_back(node_data); // 書き込み
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

	//std::string path = SelectJsonSavePath();

	//if (path.empty()) { return false; }

	/*std::string file_path = path + "\\" + filename;

	if (file_path.size() < 5 || file_path.substr(file_path.size() - 5) != ".json")
	{
		file_path += ".json";
	}*/

	if (file_path.empty()) { return false; }

	std::ofstream file(file_path); // 書き出し

	if (!file)
	{
		return false;
	}

	file << data.dump(4);

	return true;
}

bool BehaviorTreeConverter::SaveAs(
	const EditorGraph& graph,
	const char* file_name,
	std::string& save_path)
{
	std::string path = SelectJsonSavePath();

	std::string file_path = path + "\\" + file_name;

	if (file_path.size() < 5 || file_path.substr(file_path.size() - 5) != ".json")
	{
		file_path += ".json";
	}

	if (Save(graph, file_path))
	{
		save_path = file_path;
	}
	else
	{
		assert("couldn't save jsonfile!!");
	}

	return true;
}

bool BehaviorTreeConverter::Load(
	EditorGraph& graph,
	std::string& load_path)
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
		// ノード追加時変更する
		switch (type)
		{
		case NodeType::Wait: {
			auto& data = std::get<WaitData>(node->GetData());
			data.wait_time = node_data["properties"].value("wait_time", 1.0f);
			break;
		}

		case NodeType::MoveTo: {
			auto& data = std::get<MoveToData>(node->GetData());
			data.key = node_data["properties"].value("key", std::string("PlayerPosition"));
			break;
		}

		case NodeType::ShotStreight: {
			auto& data = std::get<ShotAttackData>(node->GetData());
			data.key = node_data["properties"].value("key", std::string(""));
			data.shot_interval = node_data["properties"].value("shot_interval", 0.1f);
			data.attack_time = node_data["properties"].value("attack_time", 1.0f);
			break;
		}

		case NodeType::DistCondition: {
			auto& data = std::get<DistConditionData>(node->GetData());
			data.key = node_data["properties"].value("key", std::string("PlayerPosition"));
			data.compare = node_data["properties"].value("compare", 2);
			data.dist = node_data["properties"].value("distance", 5.0f);
			break;
		}

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

	load_path = path;

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