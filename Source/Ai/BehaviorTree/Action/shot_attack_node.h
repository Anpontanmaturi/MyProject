#pragma once

#include "action_node.h"
#include "../Blackboard/blackboard.h"

class ShotAttackNode :public ActionNode
{
public:
	ShotAttackNode(const BlackboardKey& key, float shot_interval, float attack_time);

	void OnEnter(BTContext& context)override;
	BTState OnTick(BTContext& context, float elapsed_time)override;

private:
	BlackboardKey key;

	float timer{};
	float shot_timer{};
	float shot_interval = 0.1f;
	float shot_speed = 30.0f;
	float shot_scale = 5.0f;

	float attack_time;

	DirectX::XMFLOAT3 target{};
};