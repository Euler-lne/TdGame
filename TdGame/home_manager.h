#ifndef _HOME_MANAGER_H_
#define _HOME_MANAGER_H_

#include "manager.h"
#include "config_manager.h"
#include "resources_manager.h"

class HomeManeger : public Manager<HomeManeger>
{
	friend class Manager<HomeManeger>;
public:
	double get_current_hp()
	{
		return num_hp;
	}

	void decrease_hp(double value)
	{
		num_hp -= value;
		if (num_hp < 0)
			num_hp = 0;

		static const ResourcesManager::SoundPool& sound_pool
			= ResourcesManager::instance()->get_sound_pool();

		Mix_PlayChannel(-1, sound_pool.find(ResID::Sound_HomeHurt)->second, 0);  // -1任意一个可用轨道， 0代表循环 0次 
	}

protected:
	HomeManeger() 
	{
		num_hp = ConfigManager::instance()->num_initial_hp;

	}
	~HomeManeger() = default;

private:
	double num_hp = 0;
};




#endif // !_HOME_MANAGER_H_

