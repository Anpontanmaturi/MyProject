#pragma once

#include "action_node.h"
#include "../Blackboard/blackboard.h"

#define EPSILON 1e-6f

class EscapeNode : public ActionNode
{
public:
	EscapeNode(const BlackboardKey& key, float dist, float jump_scale);
	void OnEnter(BTContext& context)override;
	BTState OnTick(BTContext& context,	float elapsed_time) override;

private:
	BlackboardKey key;

	enum class Phase
	{
		Ready,
		Active,
		Recovery
	};

	Phase phase = Phase::Ready;

	float base_dist; // ‚Ç‚ê‚¾‚¯—£‚ê‚é‚©
	float jump_scale; // ƒWƒƒƒ“ƒv‚Ì‹­‚³

	DirectX::XMFLOAT3 target_pos{};
};