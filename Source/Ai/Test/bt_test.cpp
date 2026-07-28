#include "bt_test.h"

#include <cassert>

#include "../BehaviorTree/behavior_tree.h"
#include "../BehaviorTree/bt_node.h"

#include "../BehaviorTree/Composite/selector_node.h"
#include "../BehaviorTree/Composite/sequence_node.h"


class TestSuccessNode : public BTNode
{
public:

    BTState Tick(float) override
    {
        return BTState::Success;
    }
};

class TestFailureNode : public BTNode
{
public:

    BTState Tick(float) override
    {
        return BTState::Failure;
    }
};


void BehaviorTreeTest()
{
    // Selector
    {
        auto root = std::make_unique<SelectorNode>();

        root->AddChild(std::make_unique<TestFailureNode>());
        root->AddChild(std::make_unique<TestSuccessNode>());

        BehaviorTree tree;
        tree.SetRoot(std::move(root));

        assert(tree.Tick(0.0f) == BTState::Success);
    }

    // Sequence
    {
        auto root = std::make_unique<SequenceNode>();

        root->AddChild(std::make_unique<TestSuccessNode>());
        root->AddChild(std::make_unique<TestFailureNode>());

        BehaviorTree tree;
        tree.SetRoot(std::move(root));

        assert(tree.Tick(0.0f) == BTState::Failure);
    }
}