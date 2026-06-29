#include "projectile_manager.h"

static ProjectileManager* instance = nullptr;

ProjectileManager& ProjectileManager::Instance()
{
	static ProjectileManager instance;
	return instance;
}

// コンストラクタ
ProjectileManager::ProjectileManager()
{
}

// デストラクタ
ProjectileManager::~ProjectileManager()
{
	Clear();
}

void ProjectileManager::Update(float elapsed_time)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsed_time);
	}

	for (Projectile* projectile : removes)
	{
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		delete projectile;
	}
	removes.clear();
}

void ProjectileManager::Render(ID3D11DeviceContext* device_context)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(device_context);
	}
}

// 弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

// 全削除
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

// 弾丸削除
void ProjectileManager::Remove(Projectile* projectile)
{
	removes.emplace_back(projectile);
}