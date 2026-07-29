#pragma once

class Blackboard;
class Character;

struct BTContext
{
	Blackboard* blackboard = nullptr;

	Character* owner = nullptr;
};
