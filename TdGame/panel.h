#ifndef _PANEL_H_
#define _PANEL_H_

#include <SDL.h>
#include <string>
#include "resources_manager.h"
#include "tile.h"

class Panel
{
public:
	Panel()
	{
		tex_select_cursor = ResourcesManager::instance()->get_texture_pool().find(ResID::Tex_UISelectCursor)->second;
	}
	~Panel()
	{
		SDL_DestroyTexture(tex_text_background);
		SDL_DestroyTexture(tex_text_foreground);
	}

	void show()
	{
		visible = true;
	}

	void set_idx_tile(const SDL_Point& pos)
	{
		idx_tile_selected = pos;
	}

	void set_center_pos(const SDL_Point& pos)
	{
		center_pos = pos;
	}

	void on_input(const SDL_Event& event)
	{
		if (!visible) return;

		switch (event.type)
		{
		case SDL_MOUSEMOTION:  // 鼠标移动事件
		{
			SDL_Point pos_cursor = { event.motion.x,event.motion.y };
			SDL_Rect rect_target = { 0,0,size_button,size_button };
			//std::cout << "Mouse " << pos_cursor.x << " " << pos_cursor.y << std::endl;

			rect_target.x = center_pos.x - width / 2 + offset_top.x;
			rect_target.y = center_pos.y - width / 2 + offset_top.y;
			//std::cout << "Rect Top " << rect_target.x << " " << rect_target.y << std::endl;
			if (SDL_PointInRect(&pos_cursor, &rect_target))
			{
				hovered_target = HoveredTarget::Top;
				return;
			}
			rect_target.x = center_pos.x - width / 2 + offset_left.x;
			rect_target.y = center_pos.y - width / 2 + offset_left.y;
			//std::cout << "Rect Left " << rect_target.x << " " << rect_target.y << std::endl;
			if (SDL_PointInRect(&pos_cursor, &rect_target))
			{
				hovered_target = HoveredTarget::Left;
				return;
			}
			rect_target.x = center_pos.x - width / 2 + offset_right.x;
			rect_target.y = center_pos.y - width / 2 + offset_right.y;
			//std::cout << "Rect Right " << rect_target.x << " " << rect_target.y << std::endl;
			if (SDL_PointInRect(&pos_cursor, &rect_target))
			{
				hovered_target = HoveredTarget::Right;
				return;
			}
			hovered_target = HoveredTarget::None;
		}
			break;
		case SDL_MOUSEBUTTONUP:
		{
			switch (hovered_target)
			{
			case Panel::HoveredTarget::None:
				break;
			case Panel::HoveredTarget::Top:
				on_click_top_area();
				break;
			case Panel::HoveredTarget::Left:
				on_click_left_area();
				break;
			case Panel::HoveredTarget::Right:
				on_click_right_area();
				break;
			default:
				break;
			}
			visible = false;
		}
			break;
		default:
			break;
		}
	}

	virtual void on_update(SDL_Renderer* renderer)
	{
		static TTF_Font* font = ResourcesManager::instance()->get_font_pool().find(ResID::Font_Main)->second;

		if (hovered_target == HoveredTarget::None) return;

		int val = 0;
		switch (hovered_target)
		{
		case Panel::HoveredTarget::Top:
			val = val_top;
			break;
		case Panel::HoveredTarget::Left:
			val = val_left;
			break;
		case Panel::HoveredTarget::Right:
			val = val_right;
			break;
		}

		SDL_DestroyTexture(tex_text_background);
		tex_text_background = nullptr;
		SDL_DestroyTexture(tex_text_foreground);
		tex_text_foreground = nullptr;

		std::string str_val = val < 0 ? "MAX" : std::to_string(val);
		SDL_Surface* suf_text_background = TTF_RenderText_Blended(font, str_val.c_str(), color_text_background);
		SDL_Surface* suf_text_foreground = TTF_RenderText_Blended(font, str_val.c_str(), color_text_foreground);


		width_text = suf_text_background->w, height_text = suf_text_background->h;

		tex_text_background = SDL_CreateTextureFromSurface(renderer, suf_text_background);
		tex_text_foreground = SDL_CreateTextureFromSurface(renderer, suf_text_foreground);

		SDL_FreeSurface(suf_text_background);
		SDL_FreeSurface(suf_text_foreground);
	}

	virtual void on_render(SDL_Renderer* renderer)
	{
		if (!visible) return;

		SDL_Rect rect_dst_cursor =
		{
			center_pos.x - SIZE_TILE / 2,
			center_pos.y - SIZE_TILE / 2,
			SIZE_TILE,
			SIZE_TILE
		};
		SDL_RenderCopy(renderer, tex_select_cursor, nullptr, &rect_dst_cursor);

		SDL_Rect rect_dst_panel =
		{
			center_pos.x - width / 2,
			center_pos.y - height / 2,
			width, height
		};
		SDL_Texture* tex_panel = nullptr;
		switch (hovered_target)
		{
		case Panel::HoveredTarget::None:
			tex_panel = tex_idle;
			break;
		case Panel::HoveredTarget::Top:
			tex_panel = tex_hovered_top;
			break;
		case Panel::HoveredTarget::Left:
			tex_panel = tex_hovered_left;
			break;
		case Panel::HoveredTarget::Right:
			tex_panel = tex_hovered_right;
			break;
		}

		SDL_RenderCopy(renderer, tex_panel, nullptr, &rect_dst_panel);

		if (hovered_target == HoveredTarget::None) return;
		SDL_Rect rect_dst_text;
		rect_dst_text.x = center_pos.x - width_text / 2 + offset_show.x;
		rect_dst_text.y = center_pos.y + height / 2 + offset_show.y;  // 加上面板高度除以2 使得位于下方
		rect_dst_text.w = width_text;
		rect_dst_text.h = height_text;
		SDL_RenderCopy(renderer, tex_text_background, nullptr, &rect_dst_text);

		rect_dst_text.x -= offset_show.x;
		rect_dst_text.y -= offset_show.y;
		SDL_RenderCopy(renderer, tex_text_foreground, nullptr, &rect_dst_text);
	}

protected:
	enum class HoveredTarget
	{
		None, Top, Left, Right
	};

protected:
	bool visible = false;  // 面板一直存在 只是可不可见
	SDL_Point idx_tile_selected;
	SDL_Point center_pos = { 0 };
	SDL_Texture* tex_idle = nullptr;
	SDL_Texture* tex_hovered_top = nullptr;
	SDL_Texture* tex_hovered_right = nullptr;
	SDL_Texture* tex_hovered_left = nullptr;
	SDL_Texture* tex_select_cursor = nullptr;
	int val_top = 0, val_right = 0, val_left = 0;
	HoveredTarget hovered_target = HoveredTarget::None;

protected:
	virtual void on_click_top_area() = 0;  // = 0 代表子类必须实现，不然不可以初始化
	virtual void on_click_left_area() = 0;
	virtual void on_click_right_area() = 0;



private:
	const int size_button = 48;
	const int width = 144, height = 144;
	const SDL_Point offset_top = { 48,6 };
	const SDL_Point offset_left = { 8,80 };
	const SDL_Point offset_right = { 90,80 };
	const SDL_Point offset_show = { 3,3 };
	const SDL_Color color_text_background = { 175,175,175,255 };
	const SDL_Color color_text_foreground = { 255,255,255,255 };

	int width_text = 0, height_text = 0;
	SDL_Texture* tex_text_background = nullptr;
	SDL_Texture* tex_text_foreground = nullptr;

};



#endif // !_PANEL_H_

