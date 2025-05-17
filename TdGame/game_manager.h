#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "manager.h"

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
#include<SDL_net.h>
#include<SDL2_gfxPrimitives.h>

#include<iostream>

class GameManager : public Manager<GameManager>
{
	friend class Manager<GameManager>; 
	// 使得父类可以使用子类的构造函数，protected之下父类不能使用子类中的protected的函数 
	// 父类的instance方法调用的是子类的构造方法，然而这个子类构造方法是protected的所以父类调用不了子类的构造方法
public:
	int run(int argc, char** argv)
	{
		Uint64 last_counter = SDL_GetPerformanceCounter();
		const Uint64 counter_freq = SDL_GetPerformanceFrequency();

		while (!is_quit)
		{
			while (SDL_PollEvent(&event))
			{
				on_input();
			}

			// 更新数据
			Uint64 current_counter = SDL_GetPerformanceCounter();
			double delta = (double)(current_counter - last_counter) / counter_freq;
			last_counter = current_counter;
			if (delta < 1.0 / 60)
				SDL_Delay(1000 * (1.0 / 60 - delta));
			on_update(delta);

			// 绘制画面
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer); // 清空画面

			on_render();

			SDL_RenderPresent(renderer); // 刷新
		}
		return 0;
	}

protected:
	GameManager()
	{
		init_assert(!SDL_Init(SDL_INIT_EVERYTHING),u8"SDL2 初始化失败！");
		init_assert(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG), u8"SDL_image 初始化失败！"); // Mix_Init IMG_Init 返回0 失败
		init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer 初始化失败！");
		init_assert(!TTF_Init(), u8"SDL_mixer 初始化失败！");
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1"); //向SDL提供建议，希望显示IME（候选测列表），设置为"1"就显示

		window = SDL_CreateWindow(u8"村庄保卫战", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
		init_assert(window, u8"创建游戏窗口失败！");
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		init_assert(renderer, u8"创建渲染器失败！");
	}
	~GameManager()
	{
		// 从下往上析构释放
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		TTF_Quit();
		Mix_Quit();
		IMG_Quit();
		SDL_Quit();
	}

private:
	SDL_Event event;
	bool is_quit = false;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

private:
	void init_assert(bool flag, const char* err_msg)
	{
		if (flag) return;

		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"游戏启动失败", err_msg, window);
		exit(-1);
	}

	void on_input()
	{

	}

	void on_update(double delta)
	{

	}

	void on_render()
	{

	}
};

#endif // !_GAME_MANAGER_H_
