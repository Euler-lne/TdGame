#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include"manager.h"
#include"config_manager.h"
#include"resources_manager.h"

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

		ConfigManager* config = ConfigManager::instance();
		init_assert(config->map.load("map.csv"), u8"加载游戏地图失败");
		init_assert(config->load_game_config("config.json"), u8"加载游戏配置失败");
		init_assert(config->load_level_config("level.json"), u8"加载关卡配置失败");

		window = SDL_CreateWindow(config->basic_template.window_tile.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			config->basic_template.window_width, config->basic_template.window_height, SDL_WINDOW_SHOWN);
		init_assert(window, u8"创建游戏窗口失败！");
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		init_assert(renderer, u8"创建渲染器失败！");

		init_assert(ResourcesManager::instance()->load_from_file(renderer), u8"加载游戏资源失败");

		init_assert(generate_tile_map_texture(), u8"生成瓦片地图纹理失败");
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
	SDL_Texture* tex_tile_map = nullptr;

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
		static ConfigManager* instance = ConfigManager::instance();
		static SDL_Rect& rect_dst = instance->rect_tile_map;
		SDL_RenderCopy(renderer, tex_tile_map, nullptr, &rect_dst);
	}

	bool generate_tile_map_texture()
	{
		// 整个渲染一个图片，在每一帧直接绘图
		// 创建一个新的画布，用画笔renderer在上面绘制，之后再绘制到 window （再on_render时候）
		const Map& map = ConfigManager::instance()->map;
		const TileMap& tile_map = map.get_tile_map();
		SDL_Rect& rect_tile_map = ConfigManager::instance()->rect_tile_map;  // 这个变量还没有赋值
		SDL_Texture* tex_tile_set = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Tileset)->second;

		int width_tex_tile_set, height_tex_tile_set; // 这获取的是源图片的set
		SDL_QueryTexture(tex_tile_set, nullptr, nullptr, &width_tex_tile_set, &height_tex_tile_set);
		int num_tile_single_line = (int)std::ceil((double)width_tex_tile_set / SIZE_TILE);

		int width_tex_tile_map, height_tex_tile_map;
		width_tex_tile_map = (int)map.get_width() * SIZE_TILE;
		height_tex_tile_map = (int)map.get_height() * SIZE_TILE;

		tex_tile_map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
			SDL_TEXTUREACCESS_TARGET, width_tex_tile_map, height_tex_tile_map);
		if (!tex_tile_map)return false;
		
		// 绘制居中
		ConfigManager *config = ConfigManager::instance();
		rect_tile_map.x = (config->basic_template.window_width - width_tex_tile_map) / 2;
		rect_tile_map.y = (config->basic_template.window_height - height_tex_tile_map) / 2;
		rect_tile_map.w = width_tex_tile_map;
		rect_tile_map.h = height_tex_tile_map;

		SDL_SetTextureBlendMode(tex_tile_map, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, tex_tile_map);
		for (int y = 0; y < map.get_height(); y++)
		{
			for (int x = 0; x < map.get_width(); x++)
			{
				const Tile& tile = tile_map[y][x];

				SDL_Rect  rect_dst = {
					x * SIZE_TILE, y * SIZE_TILE,
					SIZE_TILE, SIZE_TILE
				};
				SDL_Rect rect_src = { // 图片原位值，素材图片
					(tile.terrian % num_tile_single_line) * SIZE_TILE,
					(tile.terrian / num_tile_single_line) * SIZE_TILE,
					SIZE_TILE, SIZE_TILE
				};
				SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);

				if (tile.decoration >= 0)
				{
					rect_src =
					{
						(tile.decoration % num_tile_single_line) * SIZE_TILE,
						(tile.decoration / num_tile_single_line) * SIZE_TILE,
						SIZE_TILE, SIZE_TILE
					};
					SDL_RenderCopy(renderer, tex_tile_set, &rect_src, &rect_dst);
				}
			}
		}
		// 绘制房间
		const SDL_Point& idx_home = map.get_idx_home();
		SDL_Rect  rect_dst = {
			idx_home.x * SIZE_TILE, idx_home.y * SIZE_TILE,
			SIZE_TILE, SIZE_TILE
		};
		SDL_RenderCopy(renderer,
			ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_Home)->second, nullptr,
			&rect_dst);

		SDL_SetRenderTarget(renderer, nullptr);

		return true;
	}
};

#endif // !_GAME_MANAGER_H_
