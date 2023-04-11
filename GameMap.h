#pragma once
#include<unordered_map>
#include<fstream>
#include"TileMap.h"
#include"Editor.h"
#include"util.h"
#include"Json.h"
class Editor;
class GameMap:public CanJson
{
public:
	GameMap(float game_show_scale) {
		__map_tiles.resize(game_map_max_level);
		for (int i = 0; i < game_map_max_level; i++)
		{
			__map_tiles[i].resize(map_x * map_y,i==0?0:42);
			__game_map.push_back(ShowMap(game_show_scale, __map_tiles[i], i == 0,500,500));
		}
	};
	~GameMap() {};

	void init(LoadedTextures* textures)
	{
		if (loadGameMap(textures))
		{
			std::cout << "load map from:game_map.json\n" << std::endl;
		}
		else
		{
			for (int i = 0; i < game_map_max_level; i++)
			{
				__game_map[i].init(textures, 0, { map_x,map_y });
			}
		}
	}
	void changeTileTexture(int cell_index_x, int cell_index_y, const GridCell& target)
	{
		bool is_background = target.tile.use_texture->detail.is_background;

		auto& game_map = is_background ? __game_map[0] : __game_map[1];
		game_map.changeGridCellTile(cell_index_x, cell_index_y, target);
	}
	friend Editor;
	float game_scale = 1.0f;


	Json toJson() const override
	{
		JsonArray json_array;
		auto __map_tiles_v = genValue(__map_tiles);
		json_array.insert(JsonNode("__map_tiles",__map_tiles_v));

		return Json(std::move(json_array));
	}

	bool loadGameMap(LoadedTextures* textures)
	{
		std::ifstream f("game_map.json");
		if (!f.good())
			return false;
		Json load_map;
		load_map.read("game_map.json");
		auto [map_begin,map_end] = load_map["__map_tiles"].iterArrayValue();

		for (int i = 0; map_begin!=map_end; i++,map_begin++)
		{
			auto [sub_begin, sub_end] = map_begin->iterArrayValue();
			for (int j = 0; sub_begin!=sub_end; j++,sub_begin++)
			{
				__map_tiles[i][j] = *sub_begin;
			}
		}
		for (int i = 0; i < game_map_max_level; i++)
		{
			__game_map[i].init(textures, 0, { map_x,map_y });
		}
		return true;
	}

	void saveGameMap()
	{
		toJson().save("game_map.json");
	}
private:
	std::vector<ShowMap> __game_map;
	//int __map_tiles[game_map_max_level][map_x * map_y];
	std::vector<std::vector<int>> __map_tiles;
};