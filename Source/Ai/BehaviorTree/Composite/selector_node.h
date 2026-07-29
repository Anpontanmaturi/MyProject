#pragma once

#include "composite_node.h"

class SelectorNode final : public CompositeNode
{
public:
	BTState Tick(BTContext& context, float elapsed_time) override;

};