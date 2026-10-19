#pragma once

class Blackboard;
class Character;
class BTDebugInfo;

struct BTContext
{
	Blackboard* blackboard = nullptr;
	Character* owner = nullptr;

	BTDebugInfo* debug_info = nullptr;

	float move_speed{};
	float turn_speed{};
};
