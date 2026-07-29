#pragma once

#include "action_node.h"

#include <string>

class PrintNode : public ActionNode
{
public:
	explicit PrintNode(const std::string& text);

	BTState Tick(BTContext& context, float elapsed_time) override;

private:
	std::string text;

};