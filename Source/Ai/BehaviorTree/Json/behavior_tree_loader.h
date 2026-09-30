#pragma once

#include <memory>

class BehaviorTree;

class BehaviorTreeLoader
{
public:
	static std::unique_ptr<BehaviorTree> Load(const char* filename);
};