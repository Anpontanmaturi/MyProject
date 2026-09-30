#include "behavior_tree_fact.h"

#include "bt_node.h"

#include "Composite/sequence_node.h"
#include "Composite/selector_node.h"

#include "Action/wait_node.h"
#include "Action/move_to_node.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

std::unique_ptr<BTNode>

BehaviorTreeFact::CreateNode(int type, const json& node_data)
{
    switch (type)
    {
    case 1:
        return std::make_unique<SequenceNode>();

    case 2:
        return std::make_unique<SelectorNode>();

    case 3:
    {
        float wait_time = 1.0f;
        if (node_data.contains("properties"))
        {
            wait_time = node_data["properties"].value("wait_time", 1.0f);
        }
        return std::make_unique<WaitNode>(wait_time);
    }
    /*case 4:
        return std::make_unique<MoveToNode>();*/

    default:
        return nullptr;
    }
}