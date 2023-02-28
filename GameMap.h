#pragma once
#include<unordered_map>
#include"TileMap.h"
#include"Editor.h"
#include"util.h"
class Editor;
class GameMap
{
public:
	GameMap(float game_show_scale) {
		for (int j = 0; j < map_x * map_y; j++)
		{
			__map_tiles[1][j] = 42;
		}
		for (int i = 0; i < game_map_max_level; i++)
		{
			__game_map.push_back(ShowMap(game_show_scale, __map_tiles[i], i == 0,500,500));
		}
	};
	~GameMap() {};

	void init(LoadedTextures* textures)
	{
		for (int i = 0; i < game_map_max_level; i++)
		{
			__game_map[i].init(textures, 0, {map_x,map_y});
		}
	}
	void changeTileTexture(bool is_background,int index_x,int index_y, sf::Vector2f pos_in_texture ,int texture_id)
	{
		auto& game_map = is_background ? __game_map[0] : __game_map[1];
		auto& gridcell = game_map.getGridCell(index_x, index_y);
		std::cout << "set grid:\n" << gridcell.toString(is_background);
		game_map.setTileTexturePosition(
			gridcell.tile,
			pos_in_texture
		);

		if (!is_background)
		{
			//add ItemDetail

		}



	}
	friend Editor;
	float game_scale = 1.0f;

	void loadGameMap()
	{

	}
	void saveGameMap()
	{

	}

private:
	std::vector<ShowMap> __game_map;
	int __map_tiles[game_map_max_level][map_x * map_y];

	//put item detail
	//map<posx,map<posy,detail>>
	//std::unordered_map<int, std::unordered_map<int, ItemDetail>> __item_map;

};