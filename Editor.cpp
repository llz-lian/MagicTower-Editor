#include"Editor.h"
#include"util.h"
std::vector<std::string> getFileByPath(const std::string file_path, bool is_background)
{
    std::vector<std::string> ret;

    std::string head_str = is_background ? "\\background\\" : "\\foreground\\";

    auto path = std::filesystem::path(file_path + head_str);

    std::filesystem::directory_iterator itr(path);
    for (auto&& it : itr)
    {
        ret.push_back("assets" + head_str + it.path().filename().string());
    }
    return ret;
}

void Editor::run()
{
    sf::Event event;
    sf::Clock button_clock;
    sf::Clock click_clock;
    while (window.isOpen())
    {
        // handle events
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (button_clock.getElapsedTime() > sf::seconds(0.1f))
        {
            onButton();
            button_clock.restart();
        }if (click_clock.getElapsedTime() > sf::seconds(0.1f))
        {
            onClick();
            click_clock.restart();
        }

        // draw the map
        window.clear();
        //window.draw(__choose_map);
        //for (auto&& game_map : __game_map)
        //{
        //    window.draw(game_map);
        //}

        drawAllMap();

        window.display();
    }
}

void Editor::ChooseMap2Next(int plus)
{
    __now_choose += plus;
    if (__now_choose < 0)
        __now_choose = __choose_length - 1;
    __now_choose = __now_choose % __choose_length;

    bool is_background = __now_choose < background_texture_num ? true : false;
    int texture_index = is_background ? __now_choose : __now_choose - background_texture_num;


    __choose_map.is_background = is_background;
    auto && texture_detail = __loaded_textures.getTexturesDetail(is_background, texture_index);
    __choose_map.init(&__loaded_textures, texture_index, {texture_detail.back_texture_index_x,texture_detail.back_texture_index_y});
    /*__choose_map.setTexture(tile_size, texture_index);
    __choose_map.setTileGrid(__choose_map_tiles,
        { __choose_map.getTextureDetail(texture_index).back_texture_index_x,__choose_map.getTextureDetail(texture_index).back_texture_index_y },
        texture_index
    );*/
    __now_choose_grid = nullptr;
}



