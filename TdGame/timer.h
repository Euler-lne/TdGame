﻿#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void restart()
	{
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(double value)
	{
		wait_time = value;
	}

	void set_one_shot(bool flag)
	{
		one_shot = flag;
	}

	void set_on_timeout(std::function<void()> on_timeout)
	{
		this->on_timeout = on_timeout;
	}

	void pause()
	{
		paused = true;
	}

	void resume()
	{
		paused = false;
	}

	void on_update(double delta)
	{
		if (paused) return;
		pass_time += delta;
		if (pass_time >= wait_time)
		{
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;
			if (can_shot && on_timeout)
				on_timeout();
			pass_time -= wait_time;
		}
	}

private:
	double pass_time = 0;
	double wait_time = 0;
	bool paused = false;	// 是否暂停
	bool shotted = false;	// 是否触发过
	bool one_shot = false;	// 是否只触发一次
	std::function<void()> on_timeout;


};

#endif // !_TIMER_H_

