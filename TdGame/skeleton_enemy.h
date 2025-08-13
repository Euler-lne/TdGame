#ifndef _SKELETON_ENEMY_H_
#define _SKELETON_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

class SkeletonEnemy : public Enemy
{
public:
	SkeletonEnemy()
	{
		static const ResourcesManager::TexturePool& texture_pool
			= ResourcesManager::instance()->get_texture_pool();
		static SDL_Texture* tex_normal = texture_pool.find(ResID::Tex_Skeleton)->second;
		static SDL_Texture* tex_sketch = texture_pool.find(ResID::Tex_SkeletonSketch)->second;
		static ConfigManager::EnemyTemplate& enemy_template = ConfigManager::instance()->skeleton_template;

		static const std::vector<int> idx_list_up = { 5,6,7,8,9 };
		static const std::vector<int> idx_list_down = { 0,1,2,3,4 };
		static const std::vector<int> idx_list_left = { 15,16,17,18,19 };
		static const std::vector<int> idx_list_right = { 10,11,12,13,14 };

		set_anim(idx_list_up, idx_list_down, idx_list_left,
			idx_list_right, tex_normal, tex_sketch, 5, 4);

		set_data(enemy_template);

		size.x = 48, size.y = 48;

	}
	~SkeletonEnemy() = default;

private:

};


#endif // !_SKELETON_ENEMY_H_

