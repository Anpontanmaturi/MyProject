#pragma once

#include "blackboard_value.h"

#include <string>
#include <unordered_map>

struct BlackboardKey
{
	uint32_t id;
	std::string name;
	BlackboardValue value;
};

class Blackboard
{
public:
	template<class T>
	void SetValue(BlackboardKey key, const T& value)
	{
		values[key.id] = value;
	}

	template<class T>
	T GetValue(BlackboardKey key) const
	{
		return std::get<T>(values.at(key.id));
	}

	bool HasValue(BlackboardKey key) const
	{
		return values.find(key.id) != values.end();
	}

private:
	std::unordered_map<uint32_t, BlackboardValue> values;
};
