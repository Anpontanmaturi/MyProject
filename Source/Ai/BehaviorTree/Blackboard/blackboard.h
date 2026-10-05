#pragma once

#include "blackboard_value.h"

#include <string>
#include <unordered_map>

enum class BlackboardValueType
{
	Int,
	Float,
	Bool,
	String,
	Character,
	Float3
};

struct BlackboardKey
{
	std::string name;
	BlackboardValueType type;
};

class Blackboard
{
public:
	template<class T>
	void SetValue(BlackboardKey key, const T& value)
	{
		values[key.name] = value;
	}

	template<class T>
	T GetValue(BlackboardKey key) const
	{
		return std::get<T>(values.at(key.name));
	}

	bool HasValue(BlackboardKey key) const
	{
		return values.find(key.name) != values.end();
	}

private:
	std::unordered_map<std::string, BlackboardValue> values;
};
