#include "behavior_tree_fact.h"

#include "bt_node.h"

#include "Composite/sequence_node.h"
#include "Composite/selector_node.h"

#include "Action/wait_node.h"
#include "Action/move_to_node.h"
#include "Action/dist_condition_node.h"

std::unique_ptr<BTNode>
BehaviorTreeFact::CreateNode(BTNodeType type, const json& node_data)
{
    switch (type)
    {
    case BTNodeType::Sequence:
        return std::make_unique<SequenceNode>();

    case BTNodeType::Selector:
        return std::make_unique<SelectorNode>();

    case BTNodeType::Wait:
    {
        float wait_time = 1.0f;
        if (node_data.contains("properties"))
        {
            wait_time = node_data["properties"].value("wait_time", 1.0f);
        }
        return std::make_unique<WaitNode>(wait_time);
    }

    case BTNodeType::MoveTo:
        //return std::make_unique<MoveToNode>();
        break;

    case BTNodeType::DistCondition:
    {
        // ‰¼’u‚«
        BlackboardKey key
        {
            node_data["properties"].value("key",std::string{}),BlackboardValueType::Float3
        };
        float dist;

        DistanceCompare compare = DistanceCompare::Greater;

        if (node_data.contains("properties"))
        {
            const auto& properties = node_data["properties"];

            dist = properties.value("distance", 5.0f);
            int compare_type = properties.value("compare", 2);

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
        }

        return std::make_unique<DistConditionNode>(key, compare, dist);
    }

    default:
        return nullptr;
    }
}