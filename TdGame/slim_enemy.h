#ifndef _SLIM_ENEMY_H_
#define _SLIM_ENEMY_H_

#include "enemy.h"
#include "config_manager.h"
#include "resources_manager.h"

class SlimEnemy : public Enemy
{
public:
	SlimEnemy()
	{
		static const ResourcesManager::TexturePool& texture_pool
			= ResourcesManager::instance()->get_texture_pool();
		static SDL_Texture* tex_normal = texture_pool.find(ResID::Tex_Slime)->second;
		static SDL_Texture* tex_sketch = texture_pool.find(ResID::Tex_SlimeSketch)->second;
		static ConfigManager::EnemyTemplate& enemy_template = ConfigManager::instance()->slim_template;

		static const std::vector<int> idx_list_up = { 6,7,8,9,10,11 };
		static const std::vector<int> idx_list_down = { 0,1,2,3,4,5 };
		static const std::vector<int> idx_list_left = { 18,19,20,21,22,23 };
		static const std::vector<int> idx_list_right = { 12,13,14,15,16,17 };

		set_anim(idx_list_up, idx_list_down, idx_list_left,
			idx_list_right, tex_normal, tex_sketch, 6, 4);

		set_data(enemy_template);

		size.x = 48, size.y = 48;

	}
	~SlimEnemy() = default;

private:

};


#endif // !_SLIM_ENEMY_H_

