#include "print_node.h"

#include <iostream>

PrintNode::PrintNode(const std::string& text) :text(text)
{

}

BTState PrintNode::OnTick(BTContext& context, float elapsed_time)
{
	std::cout << text << std::endl;

	return BTState::Success;
}