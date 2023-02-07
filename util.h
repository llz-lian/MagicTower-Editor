#pragma once
#include<filesystem>
#include<vector>
#include<string>
static const unsigned int map_x = 11;
static const unsigned int map_y = 11;
static const unsigned int max_length = 1500;
static const unsigned int background_texture_num = 1;
static const unsigned int foreground_texture_num = 1;
static const unsigned int game_map_max_level = 2;

std::vector<std::string> getFileByPath(const std::string file_path, bool is_background = true);
