#include "projectile_straight.h"

ProjectileStraight::ProjectileStraight(ProjectileManager* manager, ID3D11Device* device) :Projectile(manager)
{
	const char* filename = ".\\.\\Data\\Model\\Ball\\Ball.fbx";
	model = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	this->scale = { 0.003f, 0.003f ,0.005f };
}

ProjectileStraight::~ProjectileStraight()
{

}

void ProjectileStraight::Update(float elapsed_time)
{
	// 寿命処理
	life_timer -= elapsed_time;
	if (life_timer < 0.0f)
	{
		// 自身を削除
		Destroy();
	}
	// 移動
	float speed = this->speed * elapsed_time;
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;

	// 行列の更新
	UpdateTransform();
}

void ProjectileStraight::Render(ID3D11DeviceContext* device_context)
{
	model->Render(device_context, transform, color);
}

// 発射
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}