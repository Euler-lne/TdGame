#ifndef _MAP_H_
#define _MAP_H_

#include"tile.h"
#include"route.h"

#include<SDL.h>
#include<string>
#include<fstream>
#include<sstream>
#include<unordered_map>

class Map
{
public:
	typedef std::unordered_map<int, Route> SpawnerRoutePool;
public:
	Map() = default;
	~Map() = default;
	/// <summary>
	/// 加载地图
	/// </summary>
	/// <param name="path">地图路径</param>
	/// <returns>加载成功true 失败false</returns>
	bool load(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.good()) return false;

		TileMap tile_map_temp;
		int idx_x = -1, idx_y = -1;
		std::string str_line;
		while (std::getline(file, str_line))
		{
			str_line = trim_str(str_line);
			if (str_line.empty()) // 出现一行是空行
				continue; 

			idx_x = -1, idx_y++;
			tile_map_temp.emplace_back(); // 这里必须要加，不然报空
			// 更具逗号分割这一行的单元格

			std::string str_tile;
			std::stringstream str_stream(str_line); // 制作分割字符的stringstream
			while (std::getline(str_stream, str_tile, ',')) {
				idx_x++;

				tile_map_temp[idx_y].emplace_back(); // 填充单元格中的元素，因为这是一个二维vector
				Tile& tile = tile_map_temp[idx_y].back(); // 需要定义一个引用类型，不然tile不能改变数组中的值
				load_tile_from_string(tile, str_tile);
			}
		}
		file.close();
		if (tile_map_temp.empty() || tile_map_temp[0].empty())
			return false;
		tile_map = tile_map_temp;
		generate_map_cache();
		return true;
	}

	size_t get_width() const  // 加const说明不会对类内进行修改
	{
		if (tile_map.empty())
			return 0;
		return tile_map[0].size();
	}
	size_t get_height() const
	{
		return tile_map.size();
	}

	const TileMap& get_tile_map() const
	{
		return tile_map;
	}

	const SDL_Point& get_idx_home() const
	{
		return idx_home;
	}

	const SpawnerRoutePool& get_idx_spawner_pool() const
	{
		return spawner_route_pool;
	}

	void place_tower(const SDL_Point& idx_tile)
	{
		tile_map[idx_tile.y][idx_tile.x].has_tower = true;
	}

private:
	TileMap tile_map; // 这个保存的是scv文件内容
	SDL_Point idx_home = { 0 }; // 保存防守位点对应的下标
	SpawnerRoutePool spawner_route_pool;

private:
	/// <summary>
	/// 截取首位空白字符
	/// </summary>
	/// <param name="str">原来字符串</param>
	/// <returns>删除首位空白字符后的字符串</returns>
	std::string trim_str(const std::string& str)
	{
		size_t begin_idx = str.find_first_not_of(" \t");
		if (begin_idx == std::string::npos)
			return "";
		size_t end_idx = str.find_last_not_of(" \t");
		size_t idx_range = end_idx - begin_idx + 1;

		return str.substr(begin_idx, idx_range);
	}
	/// <summary>
	/// 将瓦片字符串转换为瓦片数据
	/// </summary>
	/// <param name="tile">瓦片引用</param>
	/// <param name="str">字符串</param>
	void load_tile_from_string(Tile& tile, const std::string& str)
	{
		std::string str_tidy = trim_str(str);

		std::string str_value;
		std::vector<int> values;
		std::stringstream str_stream(str_tidy);
		while (std::getline(str_stream, str_value, '\\'))
		{
			int value;
			try
			{
				value = std::stoi(str_value); // 字符串转换为数值
			}
			catch (const std::invalid_argument) // 如果出现不能转换的情况，比如 "aaa"
			{
				value = -1;
			}
			values.push_back(value);
		}
		tile.terrian = (values.size() < 1 || values[0] < 0) ? 0 : values[0];
		tile.decoration = (values.size() < 2) ? -1 : values[1];
		tile.direction = (Tile::Direction)((values.size() < 3 || values[2] < 0 || values[2] > 4) ? 0 : values[2]);
		tile.special_flag = (values.size() < 4) ? -1 : values[3];
	}
	/// <summary>
	/// 生成地图缓存
	/// </summary>
	void generate_map_cache()
	{
		for (int y = 0; y < get_height(); y++)
		{
			for (int x = 0; x < get_width(); x++)
			{
				const Tile& tile = tile_map[y][x];
				if (tile.special_flag < 0) continue; // 没有特殊标志
				if (tile.special_flag == 0) { // 房屋
					idx_home.x = x;
					idx_home.y = y;
				}
				else 
					spawner_route_pool[tile.special_flag] = Route(tile_map, { x,y });
			}
		}
	}
};


#endif // !_MAP_H_

