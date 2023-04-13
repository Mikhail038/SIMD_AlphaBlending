//================================================================================
//08.04.2023
//================================================================================

#include <stdio.h>
#include <math.h>
#include <SFML/Graphics.hpp>

#include "alpha.h"

//================================================================================

using namespace sf;

//================================================================================

int main ()
{
    SConfig config;

	sf::Image res_image;

	sf::Image front_image;
	sf::Image back_image;
    front_image.loadFromFile ("MEDIA/AskhatCat.bmp");
    back_image.loadFromFile ("MEDIA/Table.bmp");

    sf::Uint8* front_array    = (sf::Uint8*) front_image.getPixelsPtr();
    sf::Uint8* back_array     = (sf::Uint8*) back_image.getPixelsPtr();

    config.back.x   = back_image.getSize().x;
    config.back.y   = back_image.getSize().y;
    config.front.x  = front_image.getSize().x;
    config.front.y  = front_image.getSize().y;

    printf ("back: %dx%d front: %dx%d\n", config.back.x, config.back.y, config.front.x, config.front.y);

    sf::Texture texture;
    sf::Sprite sprite;

    sf::Clock clock;

    STimer timer;
    timer.frames_amnt   = 100;
    timer.frames_cnt    = 0;
    timer.fps           = 0;

    int pixels_in_vector = 8;

    extend_array (front_array, &config.front.x, &config.front.y, pixels_in_vector);

    RenderWindow window(VideoMode(config.back.x, config.back.y), "Alpha blending example");

	while (window.isOpen())
	{
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed || event.key.code == Keyboard::Escape)
            {
                window.close();
            }
        }

        start_fps_count (&clock, &timer);

        blender (&config, &res_image, front_array, back_array);

        int fps = get_fps_count (&clock, &timer);

        texture.loadFromImage (res_image);
        sprite.setTexture (texture);

        window.draw (sprite);

        window.display ();
    }

    return 0;
}


// int main ()
// {
//     SRender render = {};
//
//     render.draw_permission = true;
//     render.SIMD_mode = true;
//     render.color = black_n_white;
//     render_init (&render);
//
// 	RenderWindow window(VideoMode(render.W, render.H), "Mandelbrot-Menge");
//
// 	Image image;
//     image.create(render.W, render.H);
//
// 	Texture texture;
// 	Sprite sprite;
//
// 	Font font;
//     font.loadFromFile("arial.ttf");
//
// 	Text text;
// 	text.setFont(font);
// 	text.setCharacterSize(24);
// 	text.setFillColor(Color::Green);
//     text.setOutlineThickness(4);
//
//     Clock clock;
//
//     STimer timer;
//     timer.frames_amnt   = 10;
//     timer.frames_cnt    = 0;
//     timer.fps           = 0;
//
// 	while (window.isOpen())
// 	{
//         start_fps_count (&clock, &timer);
//
//         Event event;
//         while (window.pollEvent(event))
//         {
//             user_interruption (P_ARGUMENTS, &window);
//         }
//
//         count_Mandelbrot (&render, &image);
//
//
//         if (render.draw_permission == true)
//         {
//             texture.loadFromImage(image);
//             sprite.setTexture(texture);
//             window.draw(sprite);
//         }
//
//         int fps = get_fps_count (&clock, &timer);
//
//         if (fps > 1e-5)
//         {
//             user_set_display (&text, &render, fps);
//         }
//
//         window.draw(text);
//         window.display();
//     }
//
//     return 0;
// }

//================================================================================
