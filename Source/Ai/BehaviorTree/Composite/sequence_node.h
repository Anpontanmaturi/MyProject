#pragma once

#include "composite_node.h"

class SequenceNode final : public CompositeNode
{
public:
	BTState OnTick(BTContext& context, float elpased_time) override;
};