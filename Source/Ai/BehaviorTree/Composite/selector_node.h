#pragma once

#include "composite_node.h"

class SelectorNode final : public CompositeNode
{
public:
	BTState Tick(float elapsed_time) override;

};