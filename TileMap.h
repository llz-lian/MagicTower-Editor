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
		//Detail() = delete;
		Detail(sf::Vector2u tile_size) :tile_size(tile_size) {};
	};
	MyTexture(sf::Vector2u tile_size):detail(tile_size)
	{
	}
	void initDetail(int texture_id,bool is_background)
	{
		this->detail.texture_id = texture_id;
		this->detail.is_background = is_background;
		detail.back_texture_index_x = getSize().x / detail.tile_size.x;
		detail.back_texture_index_y = getSize().y / detail.tile_size.y;
		detail.total_texture_index = detail.back_texture_index_x * detail.back_texture_index_y;
	}
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
	void setFromTile(const Tile& tile)
	{
		use_texture = tile.use_texture;
		quad = tile.quad;
		tile_pos_in_texutre = tile.tile_pos_in_texutre;
	}
	void setTileTexturePosition(const int pos_in_texture,const MyTexture & target_texture)
	{
		int texture_index_top_left_x = pos_in_texture % target_texture.detail.back_texture_index_x;
		int texture_index_top_left_y = pos_in_texture / target_texture.detail.back_texture_index_x;
		setTileTexturePosition(texture_index_top_left_x, texture_index_top_left_y, target_texture);
	}
	void setTileTexturePosition(const sf::Vector2f& pos, const MyTexture& target_texture)
	{
		unsigned int top_left_texture_pos_x = pos.x;
		unsigned int top_left_texture_pos_y = pos.y;
		tile_pos_in_texutre = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y);
		use_texture = &target_texture;
		setQuadtexCoords(top_left_texture_pos_x, top_left_texture_pos_y, target_texture);
	}
	void setTileTexturePosition(const int texture_index_top_left_x,const int texture_index_top_left_y, const MyTexture& target_texture)
	{
		//pos in tileset
		unsigned int top_left_texture_pos_x = texture_index_top_left_x * target_texture.detail.tile_size.x;
		unsigned int top_left_texture_pos_y = texture_index_top_left_y * target_texture.detail.tile_size.y;

		tile_pos_in_texutre = sf::Vector2f(top_left_texture_pos_x, top_left_texture_pos_y);
		use_texture = &target_texture;
		setQuadtexCoords(top_left_texture_pos_x, top_left_texture_pos_y, target_texture);
	}



	void setQuadtexCoords(unsigned int pos_top_left_x, unsigned int pos_top_left_y, const MyTexture& target_texture)
	{
		quad[0].texCoords = sf::Vector2f(pos_top_left_x, pos_top_left_y);
		quad[1].texCoords = sf::Vector2f(pos_top_left_x + target_texture.detail.tile_size.x, pos_top_left_y);
		quad[3].texCoords = sf::Vector2f(pos_top_left_x, pos_top_left_y + target_texture.detail.tile_size.y);
		quad[2].texCoords = sf::Vector2f(pos_top_left_x + target_texture.detail.tile_size.x, pos_top_left_y + target_texture.detail.tile_size.y);
	}
	void setQuadPosition(unsigned int pos_top_left_x, unsigned int pos_top_left_y, const MyTexture& target_texture)
	{
		quad[0].position = sf::Vector2f(pos_top_left_x, pos_top_left_y);
		quad[1].position = sf::Vector2f(pos_top_left_x + target_texture.detail.tile_size.x, pos_top_left_y);
		quad[3].position = sf::Vector2f(pos_top_left_x, pos_top_left_y + target_texture.detail.tile_size.y);
		quad[2].position = sf::Vector2f(pos_top_left_x + target_texture.detail.tile_size.x, pos_top_left_y + target_texture.detail.tile_size.y);
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
	LoadedTextures(sf::Vector2u tile_size) {

		__background_textures = new MyTexture[background_texture_num]{ tile_size };
		__foreground_textures = new MyTexture[foreground_texture_num]{ tile_size };
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
			textures[texture_index].initDetail(texture_index, is_background);
			//textures[texture_index].detail.texture_id = texture_index;
			//textures[texture_index].detail.is_background = is_background;
			std::cout << "load file:" << tileset_paths[texture_index] << std::endl;
		}
		std::cout << (is_background ? "background " : "foreground ") << "textures loaded\n";
		return true;
	}
	MyTexture::Detail getTexturesDetail(bool is_background,int texture_index)
	{
		return is_background ? __background_textures[texture_index].detail : __foreground_textures[texture_index].detail;
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
	TileMap(float bias_x,float bias_y) :__grid(),bias_x(bias_x),bias_y(bias_y){
		__textures = nullptr; 
		//__loaded_textures = nullptr;
		__textures_index = 0;
	}
	void init(LoadedTextures* loaded_textuers,bool is_background,int texture_index)
	{
		this->is_background = is_background;
		//__loaded_textures = loaded_textuers;
		__textures = is_background ? loaded_textuers->__background_textures : loaded_textuers->__foreground_textures;
		__textures_index = texture_index;
	}

	bool setTileGrid(const int* tiles, std::pair<size_t, size_t> grid_size)
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
				tile.setQuadPosition( pos_top_left_x, pos_top_left_y, __textures[__textures_index]);

				//index in tileset
				int texture_index = tiles[j * width + i];//0,1,2,3
				//tile.tile_pos_in_texutre;
				tile.setTileTexturePosition(texture_index, __textures[__textures_index]);
				//setTileTexturePosition(tile, texture_index);
				grid_tile.setFromTile(tile);
			}
		}
		return true;
	}
	void setBias(float x, float y)
	{
		bias_x = x;
		bias_y = y;
	}
	void changeTexCoords(int pos_in_texture, int tile_x, int tile_y)
	{
		__grid.grid[tile_x][tile_y].tile.setTileTexturePosition(pos_in_texture, __textures[__textures_index]);
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
	int getNowTexturesIndex()
	{
		return __textures_index;
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
	int __textures_index;
	Grid __grid;
	MyTexture* __textures;
	//LoadedTextures* __loaded_textures;
};

#include"Json.h"

class ShowMap
{
public:
	ShowMap(float show_scale, std::vector<int> & map_tiles , bool is_background,float bias_x,float bias_y)
		:show_scale(show_scale), is_background(is_background),__map(bias_x,bias_y), __map_tiles(map_tiles)
	{

	};
	void init(LoadedTextures * textures,int textures_index, std::pair<size_t, size_t> grid_size)
	{
		__map.init(textures,is_background, textures_index);
		__map.setTileGrid(__map_tiles.data(), grid_size);
		__map.setScale(show_scale, show_scale);
		this->grid_size = grid_size;
	};

	~ShowMap() {};
	float show_scale = 1.0f;
	bool is_background = true;

	//std::pair<size_t, size_t> grid_size{map_x,map_y};

	std::pair<size_t, size_t> getTextureGridSize()
	{
		auto&& detail = __map.getTextureDetail(__map.getNowTexturesIndex());
		return { detail.back_texture_index_x,detail.back_texture_index_y };
	}


	inline std::pair<int, int> getMapIndex(float pos_x, float pos_y)
	{
		pos_x -= __map.bias_x;
		pos_y -= __map.bias_y;
		auto tile_size_x = __map.getGridCell(0, 0).tile.use_texture->detail.tile_size.x * show_scale;
		auto tile_size_y = __map.getGridCell(0, 0).tile.use_texture->detail.tile_size.y * show_scale;

		int index_x = int(pos_x / tile_size_x);
		int index_y = int(pos_y / tile_size_y);
		return std::pair<int, int>(index_x, index_y);
	}
	inline bool posInMap(int x, int y)
	{
		float choosemap_top_left_x = __map.bias_x;
		float choosemap_top_left_y = __map.bias_y;
		auto [choosemap_w, choosemap_h] = __map.getGridSize();
		auto& tile_size = __map.getGridCell(0, 0).tile.use_texture->detail.tile_size;
		float choosemap_right_buttom_x = choosemap_top_left_x + choosemap_w * tile_size.x * show_scale;
		float choosemap_right_buttom_y = choosemap_top_left_y + choosemap_h * tile_size.y * show_scale;
		return !(x <= choosemap_top_left_x || y <= choosemap_top_left_y || x >= choosemap_right_buttom_x || y >= choosemap_right_buttom_y);
	};


	void drawMap(sf::RenderWindow  & window)
	{
		window.draw(__map);
	}

	void changeGridCellTile(int cell_index_x, int cell_index_y, const GridCell& target)
	{
		auto& src_cell = __map.getGridCell(cell_index_x, cell_index_y);
		const MyTexture& target_texture = *target.tile.use_texture;
		src_cell.tile.setTileTexturePosition(target.tile.tile_pos_in_texutre, target_texture);
		__map_tiles[cell_index_x + cell_index_y * grid_size.first] = target.tile_index_in_map.x + target.tile_index_in_map.y * target.tile.use_texture->detail.back_texture_index_x;	
	}
	GridCell & getGridCell(int x, int y)
	{
		return __map.getGridCell(x, y);
	}

private:
	std::vector<int> & __map_tiles;
	TileMap __map;
	std::pair<size_t, size_t> grid_size;
	void showMaptiles()
	{
		for (int i = 0; i < grid_size.first; i++)
		{
			for (int j = 0; j < grid_size.second; j++)
			{
				std::cout << __map_tiles[j + i * grid_size.second]<<",";
			}
			std::cout << std::endl;
		}
	}
};


