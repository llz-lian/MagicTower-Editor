#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
#include<memory>
#include<filesystem>
#include"util.h"
#include"TileMap.h"

class Editor
{
public:
	Editor(sf::Vector2u tile_size, std::string asset_dir) :tile_size(tile_size),
		window(sf::VideoMode(unsigned int(512 * choose_scale + 500), unsigned int(1256 * choose_scale)), "editor", sf::Style::Default)
	{
		window.setFramerateLimit(60);


		for (size_t i = 0; i < max_length; i++)
		{
			__choose_map_tiles[i] = int(i);
		}
		for (size_t i = 0; i < map_x * map_y; i++)
		{
			__game_map_tiles[i] = 0;
		}

		__assets_background_paths= getFileByPath(asset_dir, true);
		__assets_foreground_paths = getFileByPath(asset_dir, false);

		if (!__loaded_textures.init(__assets_background_paths, __assets_foreground_paths))
		{
			std::cout << "Editor::Editor\n";
			std::cout << "Editor init error!\n";
		}
		__choose_map.init(&__loaded_textures,true);
		__choose_length = int(__assets_background_paths.size() + __assets_foreground_paths.size());
		__choose_map.setTexture(tile_size, 0);
		__choose_map.setTileGrid(__choose_map_tiles,
			{ __choose_map.getTextureDetail(0).back_texture_index_x, __choose_map.getTextureDetail(0).back_texture_index_y },0);
		__choose_map.setScale(choose_scale, choose_scale);

		__game_map.resize(game_map_max_level);
		__game_map[0].init(&__loaded_textures,true);
		__game_map[0].setBias(450, 300);
		__game_map[0].setTexture(tile_size, 0);
		__game_map[0].setTileGrid(__game_map_tiles, {map_x,map_y}, 0);
		__game_map[0].setScale(game_scale, game_scale);

		for (size_t i = 0; i < map_x * map_y; i++)
		{
			__game_map_tiles[i] = 42;
		}
		__game_map[1].init(&__loaded_textures,false);
		__game_map[1].setBias(450, 300);
		__game_map[1].setTexture(tile_size, 0);
		__game_map[1].setTileGrid(__game_map_tiles, { map_x,map_y }, 0);
		__game_map[1].setScale(game_scale, game_scale);

	};
	~Editor() {};

	void run();
	void ChooseMap2Next(int plus);

	inline void onButton()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
			ChooseMap2Next(1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
			ChooseMap2Next(-1);
	}
	inline void onClick()
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			auto&& pos = sf::Mouse::getPosition(window);
			if (bool in_choosemap = __posInChooseMap(pos.x, pos.y); in_choosemap)
			{
				auto [index_x, index_y] = __getChooseMapIndex(pos.x, pos.y);
				auto& gridcell = __choose_map.getGridCell(index_x, index_y);
				std::cout << gridcell.toString(__choose_map.is_background);
				__now_choose_grid = &gridcell;
				std::cout << "choose grid:\n"<<__now_choose_grid->toString(__choose_map.is_background);
			}
			else if (bool in_gamemap = __posInGameMap(pos.x, pos.y); in_gamemap)
			{
				
				auto [index_x, index_y] = __getGameMapIndex(pos.x, pos.y);
				if (__now_choose_grid != nullptr)
				{
					bool is_background = __now_choose_grid->tile.use_texture->detail.is_background;
					auto& game_map = is_background ? __game_map[0] : __game_map[1];
					auto& gridcell = game_map.getGridCell(index_x, index_y);
					std::cout << "set grid:\n" << gridcell.toString(is_background);
					game_map.setTileTexturePosition(
						gridcell.tile, 
						__now_choose_grid->tile.tile_pos_in_texutre,
						__now_choose_grid->tile.use_texture->detail.texture_id
					);
					
				}
				
			}
		}
	}

	float choose_scale = 0.7f;
	float game_scale = 1.0f;
	sf::Vector2u tile_size;
private:
	GridCell* __now_choose_grid;
	GridCell* __now_game_grid;


	TileMap __choose_map;
	size_t __now_choose = 0;
	int __choose_length;

	std::vector<TileMap> __game_map;

	LoadedTextures __loaded_textures;
	std::vector<std::string> __assets_background_paths;
	std::vector<std::string> __assets_foreground_paths;

	sf::RenderWindow window;
	int __choose_map_tiles[max_length];
	int __game_map_tiles[map_x * map_y]{ 0 };

	inline std::pair<int, int> __getChooseMapIndex(float pos_x, float pos_y)
	{
		pos_x -= __choose_map.bias_x;
		pos_y -= __choose_map.bias_y;
		auto tile_size_x = __choose_map.getGridCell(0, 0).tile.use_texture->detail.tile_size.x * choose_scale;
		auto tile_size_y = __choose_map.getGridCell(0, 0).tile.use_texture->detail.tile_size.y * choose_scale;

		int index_x = int(pos_x / tile_size_x);
		int index_y = int(pos_y / tile_size_y);
		return std::pair<int, int>(index_x, index_y);
	}
	inline std::pair<int, int> __getGameMapIndex(float pos_x, float pos_y)
	{
		pos_x -= __game_map[0].bias_x;
		pos_y -= __game_map[0].bias_y;
		auto tile_size_x = __game_map[0].getGridCell(0, 0).tile.use_texture->detail.tile_size.x * game_scale;
		auto tile_size_y = __game_map[0].getGridCell(0, 0).tile.use_texture->detail.tile_size.y * game_scale;

		int index_x = int(pos_x / tile_size_x);
		int index_y = int(pos_y / tile_size_y);
		return std::pair<int, int>(index_x, index_y);
	}
	inline bool __posInChooseMap(int x, int y)
	{
		float choosemap_top_left_x = __choose_map.bias_x;
		float choosemap_top_left_y = __choose_map.bias_y;
		auto [choosemap_w, choosemap_h] = __choose_map.getGridSize();
		float choosemap_right_buttom_x = choosemap_top_left_x + choosemap_w * __choose_map.getGridCell(0, 0).tile.use_texture->detail.tile_size.x * choose_scale;
		float choosemap_right_buttom_y = choosemap_top_left_y + choosemap_h * __choose_map.getGridCell(0, 0).tile.use_texture->detail.tile_size.y * choose_scale;
		return !(x <= choosemap_top_left_x || y <= choosemap_top_left_y || x >= choosemap_right_buttom_x || y >= choosemap_right_buttom_y);
	};
	inline bool __posInGameMap(int x, int y)
	{
		float choosemap_top_left_x = __game_map[0].bias_x;
		float choosemap_top_left_y = __game_map[0].bias_y;
		auto [choosemap_w, choosemap_h] = __game_map[0].getGridSize();

		auto& tile_size = __game_map[0].getGridCell(0, 0).tile.use_texture->detail.tile_size;
		float choosemap_right_buttom_x = choosemap_top_left_x + choosemap_w * tile_size.x * game_scale;
		float choosemap_right_buttom_y = choosemap_top_left_y + choosemap_h * tile_size.y * game_scale;
		return !(x <= choosemap_top_left_x || y <= choosemap_top_left_y || x >= choosemap_right_buttom_x || y >= choosemap_right_buttom_y);
	};
};



