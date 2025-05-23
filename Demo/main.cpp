﻿#define SDL_MAIN_HANDLED

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
#include<SDL_net.h>
#include<SDL2_gfxPrimitives.h>

#include<cJSON.h>

void test_json()
{
	std::ifstream file("test.json");
	if (!file.good())
	{
		std::cout << "无法打开文件" << std::endl;
		return;
	}

	std::stringstream str_stream;
	str_stream << file.rdbuf(); // 这里这几全部读入
	file.close();

	cJSON* json_root = cJSON_Parse(str_stream.str().c_str());

	cJSON* json_name = cJSON_GetObjectItem(json_root, "name");
	cJSON* json_age = cJSON_GetObjectItem(json_root, "age");
	cJSON* json_pets = cJSON_GetObjectItem(json_root, "pets");


	std::cout << json_name->string << ": " << json_name->valuestring << std::endl;
	std::cout << json_age->string << ": " << json_age->valueint << std::endl;
	std::cout << json_pets->string << ": " << std::endl;

	cJSON* json_item = nullptr;
	cJSON_ArrayForEach(json_item, json_pets) // 遍历
	{
		std::cout << "\t" << json_item->valuestring << std::endl;
	}

}

void test_csv()
{
	std::ifstream file("test.csv");
	if (!file.good())
	{
		std::cout << "无法打开文件" << std::endl;
		return;
	}

	std::string str_line; // csv只能一行一行的读
	while (std::getline(file, str_line))
	{
		std::string str_grid;
		std::stringstream str_stream(str_line); // 制作分割字符的stringstream
		while (std::getline(str_stream, str_grid, ',')) // 切分 最后一个必须是','而不是","
		{
			std::cout << str_grid << " ";
		}
		std::cout << std::endl;
	}
	file.close();
}

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);
	TTF_Init();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	SDL_Window* window = SDL_CreateWindow(u8"你好，世界", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	// SDL_RENDERER_ACCELERATED 硬件加速
	// SDL_RENDERER_PRESENTVSYNC 垂直同步，防止画面撕裂（发生在视频帧率和显示器刷新率不同步时）
	// SDL_RENDERER_TARGETTEXTURE 渲染目标设置为TEXTURE，之后再渲染到屏幕上，用于瓦片地图


	SDL_Surface* suf_img = IMG_Load("avatar.jpg"); // 图片保存到内存中，也就是从硬盘上加载图片
	SDL_Texture* tex_img = SDL_CreateTextureFromSurface(renderer, suf_img);  // 内存中的图片上传到GPU

	TTF_Font* font = TTF_OpenFont("ipix.ttf", 32);
	SDL_Color color = { 255,255,255,255 };
	SDL_Surface* suf_text = TTF_RenderUTF8_Blended(font, u8"你好，世界", color);
	SDL_Texture* tex_text = SDL_CreateTextureFromSurface(renderer, suf_text);

	Mix_Music* music = Mix_LoadMUS("music.mp3");
	Mix_FadeInMusic(music, -1, 1500);
	
	int fps = 60;
	bool is_quit = false;

	SDL_Event event;
	SDL_Point pos_cursor = { 0,0 };
	SDL_Rect rect_img, rect_text;
	// 动态延时
	Uint64 last_counter = SDL_GetPerformanceCounter(); // 计数总数
	Uint64 counter_freq = SDL_GetPerformanceFrequency(); // 频率，频率单位 s分之一

	rect_img.w = suf_img->w;
	rect_img.h = suf_img->h;

	rect_text.w = suf_text->w;
	rect_text.h = suf_text->h;

	test_json();
	test_csv();

	while (!is_quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				is_quit = true;
			else if (event.type == SDL_MOUSEMOTION) {
				pos_cursor.x = event.motion.x;
				pos_cursor.y = event.motion.y;
			}
		}
		Uint64 current_counter = SDL_GetPerformanceCounter();
		double delta = (double)(current_counter - last_counter) / counter_freq; // 这里的单位为s
		last_counter = current_counter;
		if (delta < 1.0 / fps) {
			SDL_Delay((Uint32)(1000 * (1.0 / fps - delta)));
			// std::cout << 1000 * (1.0 / fps - delta) << " ms" << std::endl;
		}

		// 处理数据
		rect_img.x = pos_cursor.x;
		rect_img.y = pos_cursor.y;

		rect_text.x = pos_cursor.x;
		rect_text.y = pos_cursor.y;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer); // 这两行是清除屏幕的操作

		// 渲染绘图
		SDL_RenderCopy(renderer, tex_img, nullptr, &rect_img);
		filledCircleRGBA(renderer, pos_cursor.x, pos_cursor.y, 50, 255, 0 ,0 ,125 );
		SDL_RenderCopy(renderer, tex_text, nullptr, &rect_text);
		SDL_RenderPresent(renderer); // 让渲染器绘制的内容跟新到窗口上，将像素数据实际展示到窗口的缓冲区上
	}
	return 0;
}