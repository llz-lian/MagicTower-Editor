#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
#include<memory>
#include<filesystem>
#include"util.h"
#include"TileMap.h"
#include"GameMap.h"
struct ItemDetail
{
	int hp = 0;
	int attack = 0;
	int special_damage = 0;
	int defence = 0;
	int drop_yellow_keys = 0;
	int drop_blue_keys = 0;
	int drop_red_keys = 0;
	int has_coins = 0;
	int exp = 0;
};
class Editor
{
public:
	Editor(sf::Vector2u tile_size, std::string asset_dir, float choose_scale,float game_show_scale,int * __choose_map_tiles)
		:tile_size(tile_size),__loaded_textures(tile_size), __choose_map_tiles(__choose_map_tiles),__game_map(game_show_scale),
		__choose_map(choose_scale, __choose_map_tiles, true,0,0),
		
		window(sf::VideoMode(unsigned int(800 * choose_scale + 500), unsigned int(1400 * choose_scale)), 
			"editor", sf::Style::Default)
	{
		window.setFramerateLimit(60);
		__assets_background_paths= getFileByPath(asset_dir, true);
		__assets_foreground_paths = getFileByPath(asset_dir, false);

		if (!__loaded_textures.init(__assets_background_paths, __assets_foreground_paths))
		{
			std::cout << "Editor::Editor\n";
			std::cout << "Editor init error!\n";
		}
		auto&& texture_detail = __loaded_textures.getTexturesDetail(true, 0);
		__choose_map.init(&__loaded_textures, 0, {texture_detail.back_texture_index_x,texture_detail.back_texture_index_y});
		__choose_length = int(__assets_background_paths.size() + __assets_foreground_paths.size());

		__game_map.init(&__loaded_textures);

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
			if (bool in_choosemap = __choose_map.posInMap(pos.x, pos.y); in_choosemap)
			{
				auto [index_x, index_y] = __choose_map.getMapIndex(pos.x, pos.y);
				auto& gridcell = __choose_map.getGridCell(index_x, index_y);
				std::cout << gridcell.toString(__choose_map.is_background);
				__now_choose_grid = &gridcell;
				std::cout << "choose grid:\n"<<__now_choose_grid->toString(__choose_map.is_background);
			}
			else if (bool in_gamemap = __game_map.__game_map[0].posInMap(pos.x, pos.y); in_gamemap)
			{
				
				auto [index_x, index_y] = __game_map.__game_map[0].getMapIndex(pos.x, pos.y);
				if (__now_choose_grid != nullptr)
				{
					bool is_background = __now_choose_grid->tile.use_texture->detail.is_background;

					__game_map.changeTileTexture(is_background, index_x, index_y,
						__now_choose_grid->tile.tile_pos_in_texutre, __now_choose_grid->tile.use_texture->detail.texture_id);
					
				}
				
			}
		}
	}


	inline void drawAllMap()
	{
		__choose_map.drawMap(window);
		for (auto&& game_map:__game_map.__game_map)
		{
			game_map.drawMap(window);
		}
	}

	//float choose_scale = 0.7f;
	/*float game_scale = 1.0f;*/
	sf::Vector2u tile_size;
private:
	GridCell* __now_choose_grid;
	GridCell* __now_game_grid;


	ShowMap __choose_map;
	GameMap __game_map;

	int __now_choose = 0;
	int __choose_length;

	LoadedTextures __loaded_textures;
	std::vector<std::string> __assets_background_paths;
	std::vector<std::string> __assets_foreground_paths;

	sf::RenderWindow window;
	int * __choose_map_tiles;
};



