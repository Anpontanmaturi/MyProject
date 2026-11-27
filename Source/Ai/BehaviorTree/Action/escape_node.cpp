#include "escape_node.h"
#include "Character/character.h"

EscapeNode::EscapeNode(const BlackboardKey& key, float dist, float jump_scale) :key(key), base_dist(dist), jump_scale(jump_scale)
{

}

void EscapeNode::OnEnter(BTContext& context)
{
	phase = Phase::Ready;
	context.owner->SetInvincible(true);
}

BTState EscapeNode::OnTick(BTContext& context, float elapsed_time)
{
	if (!context.owner || !context.blackboard || !context.blackboard->HasValue(key))
	{
		return BTState::Failure;
	}

	Character* owner = context.owner;

	switch (phase)
	{
	case Phase::Ready: {
		DirectX::XMFLOAT3 center = {};
		DirectX::XMFLOAT3 pos = owner->GetPosition();
		DirectX::XMFLOAT3 player_pos = context.blackboard->GetValue<DirectX::XMFLOAT3>(key);

		// 中心→敵
		float dx = pos.x - center.x;
		float dz = pos.z - center.z;
		float len = sqrtf(dx * dx + dz * dz);
		if (len < EPSILON)
		{
			target_pos = { center.x, pos.y, center.z + base_dist };
		}
		else
		{
			// 中心→プレイヤー
			dx = player_pos.x - center.x;
			dz = player_pos.z - center.z;
			len = sqrtf(dx * dx + dz * dz);

			// 離れる
			dx /= len;
			dz /= len;

			target_pos = { center.x - dx * base_dist, pos.y, center.z - dz * base_dist };
		}

		phase = Phase::Active;

		owner->ExecuteJump(owner->GetJumpSpeed() * jump_scale);

		return BTState::Run;
	}
	case Phase::Active: {
		DirectX::XMFLOAT3 pos = owner->GetPosition();

		owner->MoveTo(target_pos, owner->GetMoveSpeed() * base_dist, owner->GetTurnSpeed(), elapsed_time);

		float dx = target_pos.x - pos.x;
		float dz = target_pos.z - pos.z;
		float distsq = dx * dx + dz * dz;

		if (distsq <= 1.0f)
		{
			owner->MoveStop();
			if (owner->IsGround())
			{
				phase = Phase::Recovery;
			}
		}

		return BTState::Run;
	}
	case Phase::Recovery: {
		owner->SetInvincible(false);
		context.blackboard->SetValue<bool>(BlackboardKeys::IsEscape, false);
		return BTState::Success;
	}
	}

	return BTState::Failure;
}