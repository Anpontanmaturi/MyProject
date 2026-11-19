#include "behavior_tree_fact.h"

#include "behavior_tree.h"
#include "bt_node.h"

#include "Composite/sequence_node.h"
#include "Composite/selector_node.h"

#include "Action/wait_node.h"
#include "Action/move_to_node.h"
#include "Action/move_stop_node.h"
#include "Action/idle_node.h"
#include "Action/shot_attack_node.h"
#include "Action/escape_node.h"

#include "Condition/dist_condition_node.h"
#include "Condition/check_bool_node.h"

#include "Decorator/inverter_node.h"

#include "Blackboard/blackboard_keys.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <windows.h>
#include <string>

std::unique_ptr<BTNode>
BehaviorTreeFact::CreateNode(BTNodeType type, const json& node_data)
{
    // ÉmÅ[Éhí«â¡éûïœçXÇ∑ÇÈ
    switch (type)
    {
    case BTNodeType::Sequence:
        return std::make_unique<SequenceNode>();

    case BTNodeType::Selector:
        return std::make_unique<SelectorNode>();

    case BTNodeType::Wait:{
        float wait_time = 1.0f;
        if (node_data.contains("properties"))
        {
            wait_time = node_data["properties"]["wait_time"];
        }
        return std::make_unique<WaitNode>(wait_time);
    }

    case BTNodeType::MoveTo:{
        BlackboardKey key
        {
            node_data["properties"]["key"],BlackboardValueType::Float3
        };
        return std::make_unique<MoveToNode>(key);
    }

    case BTNodeType::MoveStop:
        return std::make_unique<StopNode>();

    case BTNodeType::Idle:
        return std::make_unique<IdleNode>();

    case BTNodeType::ShotStreight: {
        const auto& properties = node_data["properties"];

        BlackboardKey key
        {
            properties["key"],BlackboardValueType::Float3
        };

        float shot_interval = properties["shot_interval"];
        float attack_time = properties["attack_time"];

        return std::make_unique<ShotAttackNode>(key, shot_interval, attack_time);
    }

    case BTNodeType::Escape: {
        const auto& properties = node_data["properties"];

        BlackboardKey key
        {
            properties["key"],BlackboardValueType::Float3
        };

        float jump_scale = properties["jump_scale"];
        float base_dist = properties["base_dist"];

        return std::make_unique<EscapeNode>(key, base_dist, jump_scale);
    }


    case BTNodeType::DistCondition: {
        const auto& properties = node_data["properties"];

        BlackboardKey key
        {
            properties["key"],BlackboardValueType::Float3
        };

        DistanceCompare compare = DistanceCompare::Greater;

        float dist = properties["distance"];
        int compare_type = properties["compare"];

        switch (compare_type)
        {
        case 0:
            compare = DistanceCompare::Less;
            break;

        case 1:
            compare = DistanceCompare::LessEqual;
            break;

        case 2:
            compare = DistanceCompare::Greater;
            break;

        case 3:
            compare = DistanceCompare::GreaterEqual;
            break;
        }

        return std::make_unique<DistConditionNode>(key, compare, dist);
    }

    case BTNodeType::CheckBool: {
        BlackboardKey key
        {
            node_data["properties"]["key"],BlackboardValueType::Bool
        };
        return std::make_unique<CheckBoolNode>(key);
    }


    case BTNodeType::Inverter:
        return std::make_unique<InverterNode>();


    default:
        return nullptr;
    }
}

std::unique_ptr<BehaviorTree>
BehaviorTreeFact::CreateTree(const json& data)
{
    auto tree = std::make_unique<BehaviorTree>();

    std::unordered_map<int, const json*> node_data_map;

    for (const auto& node_data : data["nodes"])
    {
        int id = node_data["id"];
        node_data_map[id] = &node_data;
    }


    std::unordered_map<int, std::vector<int>> children_map;

    std::unordered_set<int> has_parent;

    for (const auto& link : data["links"])
    {
        int parent_id = link["from"];
        int child_id = link["to"];
        children_map[parent_id].push_back(child_id);
        has_parent.insert(child_id);
    }


    int root_id = -1;
    for (const auto& [id, node_data] : node_data_map)
    {
        if (has_parent.find(id) == has_parent.end())
        {
            root_id = id;
            break;
        }
    }
    _ASSERT(root_id >= 0, L"BehaviorTree root node was not found.");


    std::function<std::unique_ptr<BTNode>(int)> build_node;

    build_node = [&](int id)->std::unique_ptr<BTNode>
        {
            const json& node_data = *node_data_map.at(id);
            BTNodeType type = static_cast<BTNodeType>(node_data["type"]);

            auto node = CreateNode(type, node_data);

            if (!node) { return nullptr; }

            node->SetNodeId(node_data["id"].get<int>());

            auto* composite = dynamic_cast<CompositeNode*>(node.get());
            auto* decorator = dynamic_cast<DecoratorNode*>(node.get());

            auto it = children_map.find(id);

            if (it != children_map.end())
            {
                for (int child_id : it->second)
                {
                    auto child = build_node(child_id);

                    if (!child) { return nullptr; }

                    if (composite)
                    {
                        composite->AddChild(std::move(child));
                    }
                    else if (decorator)
                    {
                        // éqÉmÅ[ÉhÇÕÇPÇ±
                        if (decorator->GetChild()) return nullptr;

                        decorator->SetChild(std::move(child));
                    }
                }
            }

            return node;
        };

    auto root_node = build_node(root_id);

    if (!root_node) { return nullptr; }

    tree->SetRoot(std::move(root_node));

    return tree;
}