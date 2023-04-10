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

const int H = 800;
const int W = 600;

//================================================================================

int main ()
{
    file_to_array ("MEDIA/AskhatCat.bmp", );

    file_to_array ("MEDIA/Table.bmp");

    RenderWindow window(VideoMode(W, H), "Alpha blending example");

	Image front_image;
    front_image.create(W, H);
	Image back_image;
    back_image.create(W, H);

	Texture front_texture;
	Texture back_texture;

	Sprite foreground;
	Sprite background;

    blend_8_pixels ();

    front_texture.loadFromImage(front_image);
    background.setTexture(back_texture);

    back_texture.loadFromImage(back_image);
    background.setTexture(back_texture);

    window.draw(sprite);

    return 0;
}


//
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
