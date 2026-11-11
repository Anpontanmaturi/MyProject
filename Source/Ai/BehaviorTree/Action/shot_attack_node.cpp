#include "shot_attack_node.h"

#include "Character/character.h"

ShotAttackNode::ShotAttackNode(const BlackboardKey& key, float shot_interval, float attack_time) :
	key(key), shot_interval(shot_interval),attack_time(attack_time)
{

}

void ShotAttackNode::OnEnter(BTContext& context)
{
	timer = 0.0f;
	shot_timer = 0.0f;
}

BTState ShotAttackNode::OnTick(BTContext& context, float elapsed_time)
{
	if (!context.owner || !context.blackboard || !context.blackboard->HasValue(key))
	{
		return BTState::Failure;
	}

	timer += elapsed_time;
	shot_timer += elapsed_time;

	if (timer >= attack_time)
	{
		return BTState::Success;
	}

	DirectX::XMFLOAT3 pos = context.owner->GetPosition();
	target = context.blackboard->GetValue<DirectX::XMFLOAT3>(key);

	if (shot_timer >= shot_interval)
	{
		shot_timer -= shot_interval;

		DirectX::XMFLOAT3 shot_pos =
		{
			pos.x, pos.y + (context.owner->GetModelHeight() * context.owner->GetScale().y) * 0.15f, pos.z
		};

		target.y += 1.0f;

		// Œü‚©‚¤•ûŒü
		DirectX::XMFLOAT3 d = {
			target.x - shot_pos.x,
			target.y - shot_pos.y,
			target.z - shot_pos.z
		};
		float len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);

		if (len >= EPSILON)
		{
			DirectX::XMFLOAT3 direction =
			{
				d.x / len,
				d.y / len,
				d.z / len
			};

			context.owner->ShotProjectileSt(direction, shot_pos);
		}
	}

	return BTState::Run;
}