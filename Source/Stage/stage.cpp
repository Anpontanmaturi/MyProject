#include "stage.h"

Stage::Stage(ID3D11Device* device)
{
	const char* filename = ".\\Data\\Model\\ExampleStage\\ExampleStage.fbx";
	model = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	collision_mesh.mesh = model.get();
	collision_mesh.transform = &transform;
	CollisionManager::Instance().Register(&collision_mesh);
}

Stage::~Stage()
{
	CollisionManager::Instance().Unregister(&collision_mesh);
}

void Stage::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);
}

void Stage::Render(ID3D11DeviceContext* device_context)
{
	model->Render(device_context, transform, color, nullptr);
}