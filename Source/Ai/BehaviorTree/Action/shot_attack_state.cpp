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


}