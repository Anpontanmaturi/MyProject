#include "behavior_tree_loader.h"

#include "../behavior_tree.h"
#include "../behavior_tree_fact.h"
#include "../bt_node.h"
#include "../Composite/composite_node.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>

using json = nlohmann::json;

std::unique_ptr<BehaviorTree>
BehaviorTreeLoader::Load(const char* filename)
{
	std::ifstream file(filename);

	if (!file)return nullptr;

	json data;
	file >> data;

	auto tree = std::make_unique<BehaviorTree>();

	std::unordered_map<int, std::unique_ptr<BTNode>>nodes;

	int root_id;

	// ÉmÅ[ÉhÇÃê∂ê¨
	for (const auto& node_data : data["nodes"])
	{
		int id = node_data["id"];
		int type = node_data["type"];

		auto node = BehaviorTreeFact::CreateNode(type, node_data);

		if (!node)continue;

		nodes[id] = std::move(node);

		if (type == 0) { root_id = id; }
	}

	// êeéqä÷åWÇÃç\íz
	for (const auto& link_data : data["links"])
	{
		int from_id = link_data["from"];
		int to_id = link_data["to"];

		auto from_it = nodes.find(from_id);
		auto to_it = nodes.find(to_id);

		if (from_it == nodes.end() || to_it == nodes.end()) continue;

		auto* parent = dynamic_cast<CompositeNode*>(from_it->second.get());

		if (!parent) continue;

		parent->AddChild(std::move(to_it->second));
	}

	if (root_id < 0)return nullptr;

	auto root_it = nodes.find(root_id);

	if (root_it == nodes.end())return nullptr;

	tree->SetRoot(std::move(root_it->second));

	return tree;
}