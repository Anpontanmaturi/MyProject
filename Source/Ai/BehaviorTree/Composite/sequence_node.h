#pragma once

#include "composite_node.h"

class SequenceNode final : public CompositeNode
{
public:
	BTState Tick(BTContext& context, float elpased_time) override;

};