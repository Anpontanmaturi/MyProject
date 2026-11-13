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
	case Phase::Ready:
		DirectX::XMFLOAT3 center = {};
		DirectX::XMFLOAT3 pos = owner->GetPosition();

		// ’†S->“G
		float dx = pos.x - center.x;
		float dz = pos.z - center.z;
		float len = sqrtf(dx * dx + dz * dz);

		if (len < EPSILON)
		{
			target_pos = { center.x, pos.y, center.z + base_dist };
		}
		else
		{
			// ”½‘Î‚Ö
			dx = -dx / len;
			dz = -dz / len;

			target_pos = { center.x + dx * base_dist, pos.y, center.z + dz * base_dist };
		}

		phase = Phase::Active;

		owner->Jump(owner->jump_speed * jump_scale);
	}
}