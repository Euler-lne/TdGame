#ifndef _ROUTE_H_
#define _ROUTE_H_

#include"tile.h"

#include<SDL.h>

class Route
{
public:
	typedef std::vector<SDL_Point> IdxList;
public:
	Route() = default;
	Route(TileMap& map,const SDL_Point& idx_oringe)
	{
		size_t width_map = map[0].size(), height_map = map.size();
		SDL_Point idx_next = idx_oringe;
		while (true) {
			if (idx_next.x >= width_map || idx_next.x < 0 || idx_next.y >= height_map || idx_next.y < 0)
				break;
			if (check_duplicate_idx(idx_next))
				break;
			else
				idx_list.push_back(idx_next);

			bool is_next_dir_exist = true;
			const Tile& tile = map[idx_next.y][idx_next.x];
			if (tile.special_flag == 0) break; // 遇到房屋

			switch (tile.direction) {
			case Tile::Direction::Up:
				idx_next.y--;
				break;
			case Tile::Direction::Down:
				idx_next.y++;
				break;
			case Tile::Direction::Left:
				idx_next.x--;
				break;
			case Tile::Direction::Right:
				idx_next.x++;
				break;
			default:
				is_next_dir_exist = false;
				break;
			}
			if (is_next_dir_exist) // 走到头了，并且没有找到房屋
				break;
		}
	}
	~Route() = default;

	const IdxList& get_idx_list() const
	{
		return idx_list; // 返回常量引用
	}

private:
	IdxList idx_list;

private:
	bool check_duplicate_idx(const SDL_Point& target_idx) {
		for (const SDL_Point& idx : idx_list) {
			if (idx.x == target_idx.x && idx.y == target_idx.y)
				return true;
		}
		return false;
	}
};

#endif // !_ROUTE_H_

