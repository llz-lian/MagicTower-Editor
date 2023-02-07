#pragma once
#include<SFML/Graphics.hpp>
#include<utility>
#include<string>
#include<iostream>
#include"util.h"
class MyTexture : public sf::Texture
{
public:
	struct Detail
	{
		unsigned int back_texture_index_x = 0;
		unsigned int back_texture_index_y = 0;
		unsigned int total_texture_index = 0;
		//std::string path;
		size_t texture_id = 0;
		sf::Vector2u tile_size;
		bool is_background = true;
	};
	Detail detail;
};
class Tile
{
public:
	Tile()
	{
		use_texture = nullptr;
		quad = nullptr;
		tile_pos_in_texutre.x = tile_pos_in_texutre.y = -1;
	};
	void setTile(const Tile& tile)
	{
		use_texture = tile.use_texture;
		quad = tile.quad;
		tile_pos_in_texutre = tile.tile_pos_in_texutre;
	}
	Tile&& operator= (const Tile& tile)
	{
		return Tile(tile);
	}
	const MyTexture* use_texture;
	sf::Vertex* quad;
	inline std::string toString() const
	{
		std::string ret = "\ttexture id:" + std::to_string(use_texture->detail.texture_id) + "\n\t"
			+ "position in texture:" + "[" + std::to_string(tile_pos_in_texutre.x) + "," + std::to_string(tile_pos_in_texutre.y) + "]\n";
		return ret;
	};
	//1.file path 2.pos in TileMap([x,y] rel) 3.pos in texture (top_left abs)
	sf::Vector2f tile_pos_in_texutre; //pixel [a,b]
};
class GridCell
{
public:
	GridCell()
	{
		tile_index_in_map.x = tile_index_in_map.y = 0;
	}
	inline std::string toString(bool is_background)
	{
		std::string ret;
		ret = "\tposition in TileMap: [" + std::to_string(tile_index_in_map.x) + "," + std::to_string(tile_index_in_map.y) + "]\n";
		ret += is_background?"\tbackground tiles detail:\n": "\tforeground tiles detail:\n\t";
		ret += tile.toString();
		return ret;
	};
	Tile tile;
	sf::Vector2u tile_index_in_map;//index [x,y]
};
class Grid
{
public:
	Grid(int level = 0):grid_level(level) {};
	std::vector<std::vector<GridCell>> grid;
	unsigned int grid_level = 0;//0,1,2,3,4...

	bool operator<(const Grid&& a)
	{
		return a.grid_level < grid_level;
	}

};






class TileMap;

class LoadedTextures
{
public:
	LoadedTextures() {
		__background_textures = new MyTexture[background_texture_num];
		__foreground_textures = new MyTexture[foreground_texture_num];
	};
	bool init(const std::vector<std::string>& background_paths, const std::vector<std::string>& foreground_paths)
	{
		if (!loadAllTextures(background_paths, true))
		{
			std::cout << "LoadedTextures::LoadedTextures fail!\n";
			std::cout << "load background textures fail!\n";
			return false;
		}
		if (!loadAllTextures(foreground_paths, false))
		{
			std::cout << "LoadedTextures::LoadedTextures fail!\n";
			std::cout << "load foreground textures fail!\n";
			return false;
		}
		return true;
	}
	~LoadedTextures()
	{
		delete[] __background_textures; delete[] __foreground_textures;
	};
	bool loadAllTextures(const std::vector<std::string>& tileset_paths, bool is_background)
	{
		MyTexture* textures = is_background ? __background_textures : __foreground_textures;
		size_t length = is_background ? background_texture_num : foreground_texture_num;
		for (size_t texture_index = 0; texture_index < length; texture_index++)
		{
			if (!textures[texture_index].loadFromFile(tileset_paths[texture_index]))
			{
				std::cout << "fail @ TileMap::setTexture" << std::endl;
				return false;
			}
			textures[texture_index].detail.texture_id = texture_index;
			textures[texture_index].detail.is_background = is_background;
			std::cout << "load file:" << tileset_paths[texture_index] << std::endl;
		}
		std::cout << (is_background ? "background " : "foreground ") << "textures loaded\n";
		return true;
	}
	friend TileMap;
private:
	MyTexture* __background_textures;
	MyTexture* __foreground_textures;
};



class TileMap :
	public sf::Drawable, public sf::Transformable
{
public:
	TileMap() :__grid(){
		__textures = nullptr; 
		__loaded_textures = nullptr;
	}
	void init(LoadedTextures* loaded_textuers,bool is_background)
	{
		this->is_background = is_background;
		__loaded_textures = loaded_textuers;
		__textures = is_background ? loaded_textuers->__background_textures : loaded_textuers->__foreground_textures;
	}
	bool setTexture(sf::Vector2u tile_size, int __textures_index)
	{
		__textures[__textures_index].detail.back_texture_index_x = __textures[__textures_index].getSize().x / tile_size.x;
		__textures[__textures_index].detail.back_texture_index_y = __textures[__textures_index].getSize().y / tile_size.y;
		__textures[__textures_index].detail.total_texture_index = __textures[__textures_index].detail.back_texture_index_x *
																	__textures[__textures_index].detail.back_texture_index_y;
		__textures[__textures_index].detail.tile_size = tile_size;
		return true;
	}
	void setTilePosition(Tile& tile, float pos_top_left_x, float pos_top_left_y, int __textures_index)
	{
		auto& quad = tile.quad;
		quad[0].position = sf::Vector2f(pos_top_left_x, pos_top_left_y);
		quad[1].position = sf::Vector2f(pos_top_left_x + __textures[__textures_index].detail.tile_size.x, pos_top_left_y);
		quad[3].position = sf::Vector2f(pos_top_left_x, pos_top_left_y + __textures[__textures_index].detail.tile_size.y);
		quad[2].position = sf::Vector2f(pos_top_left_x + __textures[__textures_index].detail.tile_size.x, pos_top_left_y + __textures[__textures_index].detail.tile_size.y);
	}
	void setTileTexturePosition(Tile& tile, int texture_index, int __textures_index)
	{

		auto& quad = tile.quad;
		int texture_index_top_left_x = texture_index % __textures[__textures_index].detail.back_texture_index_x;
		int texture_index_top_left_y = texture_index / __textures[__textures_index].detail.back_texture_index_x;

		//pos in tileset
		unsigned int top_left_texture_pos_x = texture_index_top_left_x * __textures[__textures_index].detail.tile_size.x;
		unsigned int top_left_texture_pos_y = texture_index_top_left_y * __textures[__textures_index].detail.tile_size.y;

		tile.tile_pos_in_texutre = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y);

		quad[0].texCoords = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y);
		quad[1].texCoords = sf::Vector2f(top_left_texture_pos_x + __textures[__textures_index].detail.tile_size.x, top_left_texture_pos_y);
		quad[3].texCoords = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y + __textures[__textures_index].detail.tile_size.y);
		quad[2].texCoords = sf::Vector2f(top_left_texture_pos_x + __textures[__textures_index].detail.tile_size.x, top_left_texture_pos_y + __textures[__textures_index].detail.tile_size.y);
	}

	void setTileTexturePosition(Tile& tile, sf::Vector2f& pos,int __textures_index)
	{
		auto& quad = tile.quad;
		unsigned int top_left_texture_pos_x = pos.x;
		unsigned int top_left_texture_pos_y = pos.y;
		tile.tile_pos_in_texutre = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y);
		tile.use_texture = &__textures[__textures_index];

		quad[0].texCoords = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y);
		quad[1].texCoords = sf::Vector2f(top_left_texture_pos_x + __textures[__textures_index].detail.tile_size.x, top_left_texture_pos_y);
		quad[3].texCoords = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y + __textures[__textures_index].detail.tile_size.y);
		quad[2].texCoords = sf::Vector2f(top_left_texture_pos_x + __textures[__textures_index].detail.tile_size.x, top_left_texture_pos_y + __textures[__textures_index].detail.tile_size.y);
	}



	bool setTileGrid(const int* tiles, std::pair<size_t, size_t> grid_size, int __textures_index)
		/*
			11*11
			tileset: img path
			tiles: map
			size: map size
		*/
	{
		auto [width, height] = grid_size;


		__draw_array.setPrimitiveType(sf::Quads);
		__draw_array.resize(width * height * 4);//4 points per quad
		__grid.grid.clear();
		__grid.grid.resize(width);//__grid[width][height]

		for (size_t i = 0; i < width; i++)
		{
			__grid.grid[i].clear();
			for (size_t j = 0; j < height; j++)
			{
				__grid.grid[i].resize(height);
				__grid.grid[i][j].tile_index_in_map = sf::Vector2u(i, j);
				auto& grid_tile =__grid.grid[i][j].tile;
				Tile tile;
				sf::Vertex* quad = &__draw_array[(i + j * width) * 4];
				tile.quad = quad;
				tile.use_texture = &__textures[__textures_index];
				// 0  1
				// 3  2
				float pos_top_left_x = i * __textures[__textures_index].detail.tile_size.x + bias_x;
				float pos_top_left_y = j * __textures[__textures_index].detail.tile_size.y + bias_y;
				setTilePosition(tile, pos_top_left_x, pos_top_left_y, __textures_index);

				//index in tileset
				int texture_index = tiles[j * width + i];//0,1,2,3
				//tile.tile_pos_in_texutre;
				setTileTexturePosition(tile, texture_index,__textures_index);

				grid_tile.setTile(tile);
			}
		}
		return true;
	}
	void setBias(float x, float y)
	{
		bias_x = x;
		bias_y = y;
	}
	void changeTexCoords(int texture_index, int tile_x, int tile_y, int __textures_index)
	{
		setTileTexturePosition(__grid.grid[tile_x][tile_y].tile, texture_index, __textures_index);
	}

	float bias_x = 0.f;
	float bias_y = 0.f;
	bool is_background = true;

	MyTexture::Detail getTextureDetail(int __textures_index)
	{
		return __textures[__textures_index].detail;
	}

	GridCell& getGridCell(int tile_x, int tile_y)
	{
		return __grid.grid[tile_x][tile_y];
	}
	auto&& getGridSize()
	{
		return std::pair<int, int>(__grid.grid.size(), __grid.grid[0].size());
	}
private:
	//draw background
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		//big problem
		states.texture = __textures;
		target.draw(__draw_array, states);
	}
	sf::VertexArray __draw_array;

	Grid __grid;
	MyTexture* __textures;
	LoadedTextures* __loaded_textures;
};

