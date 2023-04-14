//================================================================================
//08.04.2023
//================================================================================

#include "alpha.h"
#include  <cstring>

//================================================================================

#define PIXEL 4

const char  I = 255;

const int _1 = 255;

const char  ShuffleZERO = 0x80u;

const __m256i   ZeroVector  = _mm256_set1_epi8 (0);
const __m256i  IdentVector  = _mm256_set1_epi8 (I);

//================================================================================

void extend_array (sf::Uint8** array, int* width, int* height, int multiple)
{
    int new_width = *width;

    if (new_width % multiple == 0)
    {
        return;
    }

    while (new_width % multiple != 0)
    {
        new_width++;
    }

    new_width = new_width * PIXEL;

    int old_width = *width * PIXEL;

    int size = *height * new_width;

    sf::Uint8* res_array = new sf::Uint8[size];

    int old_counter = 0;

    int new_counter = 0;
    int inline_cnt  = 0;

    while (new_counter < size)
    {
        printf ("new %d/%d old %d inline %d\n", new_counter, size, old_counter, inline_cnt);

        if (inline_cnt >= old_width)
        {
            res_array[new_counter] = 0;
        }
        else
        {
            res_array[new_counter] = (*array)[old_counter];

            old_counter++;
        }

        inline_cnt++;
        inline_cnt %= new_width;

        new_counter++;
    }

    *array = res_array;

    *width = new_width / PIXEL;

    return;
}

void blender (SConfig* config, sf::Image* res_image, const sf::Uint8* front_array, const sf::Uint8* back_array)
{
    sf::Uint8* array = (sf::Uint8*) malloc (config->back.x * config->back.y * 4);

    std::memcpy (array, back_array,         config->back.x * config->back.y * 4);

    blend_them_avx (config, array, front_array);

    res_image->create (config->back.x, config->back.y, array);

    return;
}

void blend_them_no_avx (SConfig* config, sf::Uint8* array, const sf::Uint8* front_array)
{
    unsigned int R_pxl = 0;
    unsigned int G_pxl = 0;
    unsigned int B_pxl = 0;
    unsigned int A_pxl = 0;

    int background_pxl = 0;
    int foreground_pxl = 0;

    for (int y = 0; y < config->front.y; y++)
    {
        for (int x = 0; x < config->front.x; x++)
        {
            background_pxl  = PIXEL * ((300 + y) * config->back.x + 300 + x);
            foreground_pxl  = PIXEL * (y * config->front.x + x);

            R_pxl = array[background_pxl]     * (_1 - front_array[foreground_pxl + 3]);
            G_pxl = array[background_pxl + 1] * (_1 - front_array[foreground_pxl + 3]);
            B_pxl = array[background_pxl + 2] * (_1 - front_array[foreground_pxl + 3]);
            A_pxl = array[background_pxl + 3] * (_1 - front_array[foreground_pxl + 3]);

            R_pxl += front_array[foreground_pxl]     * front_array[foreground_pxl + 3];
            G_pxl += front_array[foreground_pxl + 1] * front_array[foreground_pxl + 3];
            B_pxl += front_array[foreground_pxl + 2] * front_array[foreground_pxl + 3];
            A_pxl += front_array[foreground_pxl + 3] * front_array[foreground_pxl + 3];

            R_pxl = R_pxl >> 8;
            G_pxl = G_pxl >> 8;
            B_pxl = B_pxl >> 8;
            A_pxl = A_pxl >> 8;

            array[background_pxl]     = (unsigned char) R_pxl;
            array[background_pxl + 1] = (unsigned char) G_pxl;
            array[background_pxl + 2] = (unsigned char) B_pxl;
            array[background_pxl + 3] = (unsigned char) A_pxl;

        }
    }

    return;
}

void blend_them_avx (SConfig* config, sf::Uint8* array, const sf::Uint8* front_array)
{
    int y_0 = 300;
    int x_0 = 300;

    unsigned int R_pxl = 0;
    unsigned int G_pxl = 0;
    unsigned int B_pxl = 0;
    unsigned int A_pxl = 0;

    int background_pxl = 0;
    int foreground_pxl = 0;

    for (int y = 0; y < config->front.y; y++)
    {
        for (int x = 0; x < config->front.x; x++)
        {
            background_pxl  = PIXEL * ((y_0 + y) * config->back.x + x_0 + x);
            foreground_pxl  = PIXEL * (y * config->front.x + x);

            printf ("x %d y %d bac_px %d/%d fr_px %d/%d\n", x, y, background_pxl, config->back.x * config->back.y, foreground_pxl, config->front.x * config->front.y);

            R_pxl = array[background_pxl]     * (_1 - front_array[foreground_pxl + 3]);
            G_pxl = array[background_pxl + 1] * (_1 - front_array[foreground_pxl + 3]);
            B_pxl = array[background_pxl + 2] * (_1 - front_array[foreground_pxl + 3]);
            A_pxl = array[background_pxl + 3] * (_1 - front_array[foreground_pxl + 3]);

            R_pxl += front_array[foreground_pxl]     * front_array[foreground_pxl + 3];
            G_pxl += front_array[foreground_pxl + 1] * front_array[foreground_pxl + 3];
            B_pxl += front_array[foreground_pxl + 2] * front_array[foreground_pxl + 3];
            A_pxl += front_array[foreground_pxl + 3] * front_array[foreground_pxl + 3];

            R_pxl = R_pxl >> 8;
            G_pxl = G_pxl >> 8;
            B_pxl = B_pxl >> 8;
            A_pxl = A_pxl >> 8;

            array[background_pxl]     = (unsigned char) R_pxl;
            array[background_pxl + 1] = (unsigned char) G_pxl;
            array[background_pxl + 2] = (unsigned char) B_pxl;
            array[background_pxl + 3] = (unsigned char) A_pxl;

        }
    }

    return;
}

//================================================================================

void start_fps_count (sf::Clock* clock, STimer* timer)
{
    if (timer->frames_cnt == 0)
    {
        clock->restart();
    }

    return;
}

double get_fps_count (sf::Clock* clock, STimer* timer)
{
    if (timer->frames_cnt == timer->frames_amnt)
    {
        timer->fps =  1 / ((double) clock->getElapsedTime().asSeconds() / (double) timer->frames_amnt);

        printf("avg_FPS:    %g\n", timer->fps);

        timer->frames_cnt = 0;

        return timer->fps;
    }
    else
    {
        timer->frames_cnt++;
    }

    return 0;
}

//================================================================================
