#pragma once

#include <variant>
#include <string>
#include <DirectXMath.h>

class Character;

using BlackboardValue = std::variant<
	int,
	float,
	bool,
	std::string,
	Character*,
	DirectX::XMFLOAT3
>;