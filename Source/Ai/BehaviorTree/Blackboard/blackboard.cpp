#include "blackboard.h"

void Blackboard::RegisterKey(const BlackboardKey& key)
{
	keys[key.name] = key;
}

bool Blackboard::HasKey(const std::string& name) const
{
	return keys.find(name) != keys.end();
}

BlackboardKey Blackboard::GetKey(const std::string& name) const
{
	return keys.at(name);
}