#include "bt_test.h"

#include <cassert>

#include "../BehaviorTree/behavior_tree.h"
#include "../BehaviorTree/bt_node.h"

#include "../BehaviorTree/Composite/selector_node.h"
#include "../BehaviorTree/Composite/sequence_node.h"
#include "../BehaviorTree/Action/wait_node.h"


class TestSuccessNode : public BTNode
{
public:

    BTState Tick(BTContext&,float) override
    {
        return BTState::Success;
    }
};

class TestFailureNode : public BTNode
{
public:

    BTState Tick(BTContext&,float) override
    {
        return BTState::Failure;
    }
};


void BehaviorTreeTest()
{
    BTContext context;

    // Selector
    {
        auto root = std::make_unique<SelectorNode>();

        root->AddChild(std::make_unique<TestFailureNode>());
        root->AddChild(std::make_unique<TestSuccessNode>());

        BehaviorTree tree;
        tree.SetRoot(std::move(root));

        assert(tree.Tick(context, 0.0f) == BTState::Success);
    }

    // Sequence
    {
        auto root = std::make_unique<SequenceNode>();

        root->AddChild(std::make_unique<TestSuccessNode>());
        root->AddChild(std::make_unique<TestFailureNode>());

        BehaviorTree tree;
        tree.SetRoot(std::move(root));

        assert(tree.Tick(context, 0.0f) == BTState::Failure);
    }

    // WaitNode
    {
        BehaviorTree tree;

        auto root = std::make_unique<WaitNode>(2.0f);

        tree.SetRoot(std::move(root));

        BTContext context;

        assert(tree.Tick(context, 1.0f) == BTState::Run);

        assert(tree.Tick(context, 0.5f) == BTState::Run);

        assert(tree.Tick(context, 0.5f) == BTState::Success);

        // ê¨å˜å„ÇÕÉäÉZÉbÉgÇ≥ÇÍÇƒÇ¢ÇÈ
        assert(tree.Tick(context, 1.0f) == BTState::Run);
    }
}