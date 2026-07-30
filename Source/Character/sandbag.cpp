#include "sandbag.h"
#include "Collision/collision_manager.h"
#include "Graphics/graphics.h"

#include "Ai/BehaviorTree/Composite/sequence_node.h"
#include <Ai/BehaviorTree/Action/wait_node.h>

Sandbag::~Sandbag() = default;

Sandbag::Sandbag(ID3D11Device* device)
{
	const char* filename = ".\\Data\\Model\\Jammo\\Jammo.fbx";
	mesh = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	animator = std::make_unique<Animator>(mesh.get());

	BuildBehaviorTree();

	context.owner = this;
	context.blackboard = &black_board;

	// ステート生成
	states[static_cast<size_t>(StateId::Idle)] = std::make_unique<IdleState>(this);
	states[static_cast<size_t>(StateId::Damage)] = std::make_unique<DamageState>(this);

	SetState(StateId::Idle);
}

void Sandbag::Update(float elapsed_time)
{
	if (use_behavior_tree)
	{
		behavior_tree.Tick(context, elapsed_time);
	}
	else
	{
		UpdateStateMachine(elapsed_time);
	}

	UpdateVelocity(elapsed_time);

	UpdateInvincibleTimer(elapsed_time);

	animator->Update(elapsed_time);

	UpdateTransform();
}

void Sandbag::Render(ID3D11DeviceContext* device_context)
{
	mesh->Render(device_context, transform, color, animator->GetCurrentKeyframe());
}

void Sandbag::DebugRenderGui()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(1, 0, 0, 1));

	ImGui::Begin("Enemy(sandbag)");
	{
		ImGui::InputFloat3("position", &position.x);
		ImGui::InputFloat3("rotation", &rotation.x);
		ImGui::InputFloat4("color", &static_cast<float>(color.x));
		ImGui::Checkbox("use_behavior_tree", &use_behavior_tree);
	}
	ImGui::End();
}

void Sandbag::OnDamaged()
{
	this->SetState(StateId::Damage);
}

void Sandbag::UpdateStateMachine(float elapsed_time)
{
	// ステートの切り替え
	if (next_state != StateId::None)
	{
		if (current_state != StateId::None)
		{
			states[static_cast<size_t>(current_state)]->OnExit();
		}
		current_state = next_state;
		next_state = StateId::None;
		if (current_state != StateId::None)
		{
			states[static_cast<size_t>(current_state)]->OnEnter();
		}
	}
	// ステートの更新
	if (current_state != StateId::None)
	{
		states[static_cast<size_t>(current_state)]->OnUpdate(elapsed_time);
	}
}

void Sandbag::SetState(StateId state_id)
{
	next_state = state_id;
}


void Sandbag::IdleState::OnEnter()
{
	owner->animator->Play("Idle", true);
}

void Sandbag::IdleState::OnUpdate(float elapsed_time)
{
	
}

void Sandbag::DamageState::OnEnter()
{
	owner->animator->Play("GetHit1", false);
}

void Sandbag::DamageState::OnUpdate(float elapsed_time)
{
	if (owner->animator->IsFinished())
	{
		owner->SetState(StateId::Idle);
	}
}

void Sandbag::BuildBehaviorTree()
{
	auto root = std::make_unique<SequenceNode>();

	root->AddChild(std::make_unique<WaitNode>(2.0f));

	behavior_tree.SetRoot(std::move(root));
}

