#ifndef _ENEMY_MANAGER_H_
#define _ENEMY_MANAGER_H_

#include "enemy.h"
#include "manager.h"
#include "config_manager.h"
#include "vector2.h"
#include "home_manager.h"
#include "slim_enemy.h"
#include "king_slim_enemy.h"
#include "skeleton_enemy.h"
#include "goblin_enemy.h"
#include "goblin_priest_enemy.h"

#include <vector>
#include<SDL.h>

class EnemyManager : public Manager<EnemyManager>
{
	friend class Manager<EnemyManager>;

public:
	typedef std::vector<Enemy*> EnemyList;
	
public:
	void on_update(double delta)
	{
		move_invalid_enemy();    // 先移除失效的敌人
		for (Enemy* enemy : enemy_list)
			enemy->on_update(delta);

		// 碰撞体积
		process_bullet_collision();
		process_home_collision();
	}

	void on_render(SDL_Renderer* renderer)
	{
		for (Enemy* enemy : enemy_list)
			enemy->on_render(renderer);
	}

	void spawn_enemy(EnemyType type, int idx_spawn_point)
	{
		static Vector2 position;
		static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		static const Map::SpawnerRoutePool& spawner_route_pool
			= ConfigManager::instance()->map.get_idx_spawner_pool();

		const auto& itor = spawner_route_pool.find(idx_spawn_point);
		//for (SDL_Point point : itor->second.get_idx_list())
			//std::cout << point.x << " " << point.y << std::endl;
		//std::cout << "End Rout Print" << std::endl;
		if (itor == spawner_route_pool.end()) return; // 没有找到存在的生成点

		Enemy* enemy = nullptr;
		switch (type)
		{
		case EnemyType::Slim:
			enemy = new SlimEnemy();
			break;
		case EnemyType::KingSlim:
			enemy = new KingSlimEnemy();
			break;
		case EnemyType::Skeleton:
			enemy = new SkeletonEnemy();
			break;
		case EnemyType::Goblin:
			enemy = new GoblinEnemy();
			break;
		case EnemyType::GoblinPriest:
			enemy = new GoblinPriestEnemy();
			break;
		default:
			enemy = new SlimEnemy();
			break;
		}

		enemy->set_on_skill_released(
			[&](Enemy* enemy_src)
			{
				double recover_radius = enemy_src->get_recover_radius();
				if (recover_radius < 0) return;

				const Vector2 pos_src = enemy_src->get_position();
				for (Enemy* enemy_dst : enemy_list)
				{
					const Vector2& pos_dst = enemy_dst->get_position();
					double distance = (pos_dst - pos_src).length();
					if (distance < recover_radius)
						enemy_dst->increase_hp(enemy_src->get_recover_intensity());
				}
			}
		);

		const Route::IdxList& idx_list = itor->second.get_idx_list();
		position.x = rect_tile_map.x + idx_list[0].x * SIZE_TILE + SIZE_TILE / 2;
		position.y = rect_tile_map.y + idx_list[0].y * SIZE_TILE + SIZE_TILE / 2;

		enemy->set_position(position);
		enemy->set_route(&itor->second);

		enemy_list.push_back(enemy);

	}

	bool check_cleared()
	{
		return enemy_list.empty();
	}

protected:
	EnemyManager() = default;
	~EnemyManager()
	{
		for (Enemy* enemy : enemy_list)
			delete enemy;
	}

private:
	EnemyList enemy_list;

private:
	void process_home_collision()
	{
		static const SDL_Point& idx_home = ConfigManager::instance()->map.get_idx_home();
		static const SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;
		static const Vector2 position_home_tile =
		{
			(double)rect_tile_map.x + idx_home.x * SIZE_TILE,
			(double)rect_tile_map.y + idx_home.y * SIZE_TILE
		};

		for (Enemy* enemy : enemy_list)
		{
			if (enemy->can_remove()) continue;  // 首先判断是否无效
			const Vector2& position = enemy->get_position();
			if (position.x >= position_home_tile.x    // position_home_tile 是左上角 position 是敌人中心
				&& position.y >= position_home_tile.y
				&& position.x <= position_home_tile.x + SIZE_TILE
				&& position.y <= position_home_tile.y + SIZE_TILE)
			{
				enemy->make_invalid();
				// std::cout << "Touch Home" << std::endl;
				// std::cout << "Home Pos x: " << position_home_tile.x << " y: " << position_home_tile.y << std::endl;
				// std::cout << "Enemy Pos x: " << position.x << " y: " << position.y << std::endl;
				HomeManeger::instance()->decrease_hp(enemy->get_damage());
			}
		}
	}
	void process_bullet_collision()
	{

	}

	void move_invalid_enemy()
	{
		enemy_list.erase(std::remove_if(enemy_list.begin(), enemy_list.end(),
			[](const Enemy* enemy)
			{
				bool deletable = enemy->can_remove();
				if (deletable) delete enemy;   // 防止内存泄露，需要delete
				return deletable;
			}), enemy_list.end());
	}
};



#endif // !_ENEMY_MANAGER_H_

