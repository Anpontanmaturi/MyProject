#pragma once

#include "composite_node.h"

class SequenceNode final : public CompositeNode
{
public:
	BTState Tick(float elpased_time) override;

};