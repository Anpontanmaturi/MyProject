#pragma once

#include <variant>
#include <string>

class Character;

using BlackboardValue = std::variant<
	int,
	float,
	bool,
	std::string,
	Character*
>;