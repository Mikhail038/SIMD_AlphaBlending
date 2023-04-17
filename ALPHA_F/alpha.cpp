//================================================================================
//08.04.2023
//================================================================================

#include "alpha.h"
#include  <cstring>

//================================================================================

#define PIXEL 4

const int _1 = 255;

const char  Z = 0x80u;

const __m256i alpha_mask = _mm256_set_epi8
(
        Z, 14, Z, 14, Z, 14, Z, 14,
        Z,  6, Z,  6, Z,  6, Z,  6,
        Z, 14, Z, 14, Z, 14, Z, 14,
        Z,  6, Z,  6, Z,  6, Z,  6);

    // 31, Z, 31, Z, 31, Z, 31, Z,
    // 22, Z, 22, Z, 22, Z, 22, Z,
    // 14, Z, 14, Z, 14, Z, 14, Z,
    // 6,  Z,  6, Z,  6, Z,  6, Z); wrong way

// ...| 15  14  13  12  11  10   9   8 |  7   6   5   4   3   2  1  0 |
// ...|  0   a   0   r   0   g   0   b |  0   a   0   r   0   g  0  b |

const __m256i sum_mask = _mm256_set_epi8
(
        15, 13, 11, 9, 7, 5, 3, 1,
        Z, Z, Z, Z, Z, Z, Z, Z,
        Z, Z, Z, Z, Z, Z, Z, Z,
        15, 13, 11, 9, 7, 5, 3, 1);

    // Z,  Z,  Z,  Z,  Z,  Z,  Z,  Z,
    // Z,  Z,  Z,  Z,  Z,  Z,  Z,  Z,
    // 31, 29, 27, 25, 23, 21, 19, 17,
    // 15, 13, 11,  9,  7,  5,  3,  1); wrong way

// ...| 15  14  13  12  11  10   9   8 |  7   6   5   4   3   2  1  0 |
// ...|  A   a   R   r   G   g   B   b |  A   a   R   r   G   g  B  b |

const __m256i   ZeroVector  = _mm256_set1_epi8 (0);
const __m256i  IdentVector  = _mm256_set1_epi16 (0xff);

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
        //printf ("new %d/%d old %d inline %d\n", new_counter, size, old_counter, inline_cnt);

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

    // int extra_rounds = 0;
    // while (extra_rounds < 100)
    // {
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

    //     extra_rounds++;
    // }

    return;
}

void blend_them_avx (SConfig* config, sf::Uint8* array, const sf::Uint8* front_array)
{
    sf::Color unpacked_color[8];

    int y_0 = 300;
    int x_0 = 300;

    int background_pxl = 0;
    int foreground_pxl = 0;

    // int extra_rounds = 0;
    // while (extra_rounds < 100)
    // {
        for (int y = 0; y < config->front.y; y++)
        {
            for (int x = 0; x < config->front.x; x += 8)
            {
                background_pxl  = PIXEL * ((y_0 + y) * config->back.x + x_0 + x);
                foreground_pxl  = PIXEL * (y * config->front.x + x);

                //printf ("x %d y %d bac_px %d/%d fr_px %d/%d\n", x, y, background_pxl, config->back.x * config->back.y, foreground_pxl, config->front.x * config->front.y);

                __m256i low_back    = _mm256_loadu_si256 (( __m256i*) &array[background_pxl]);
                __m256i low_front   = _mm256_loadu_si256 (( __m256i*) &front_array[foreground_pxl]);

                __m256i up_back     = _mm256_cvtepu8_epi16 (_mm256_extractf128_si256(low_back, 1));  //1 - UP
                __m256i up_front    = _mm256_cvtepu8_epi16 (_mm256_extractf128_si256(low_front, 1));

                low_back    = _mm256_cvtepu8_epi16 (_mm256_extractf128_si256 (low_back, 0)); //0 - LOW
                low_front   = _mm256_cvtepu8_epi16 (_mm256_extractf128_si256 (low_front, 0));

                __m256i low_alpha   = _mm256_shuffle_epi8 (low_front, alpha_mask);
                __m256i up_alpha    = _mm256_shuffle_epi8 (up_front, alpha_mask);

                low_front   = _mm256_mullo_epi16 (low_front, low_alpha);
                up_front    = _mm256_mullo_epi16 (up_front, up_alpha);

                low_back    = _mm256_mullo_epi16 (low_back, _mm256_sub_epi16 (IdentVector, low_alpha));
                up_back     = _mm256_mullo_epi16 (up_back, _mm256_sub_epi16 (IdentVector, up_alpha));

                __m256i low_sum = _mm256_add_epi16 (low_front, low_back);
                __m256i up_sum  = _mm256_add_epi16 (up_front, up_back);  //every second byte is low part of number now

                low_sum = _mm256_shuffle_epi8 (low_sum, sum_mask);   //so we will shuffle them (div 256 <=> take only upper part of each 2 byte number)
                up_sum  = _mm256_shuffle_epi8 (up_sum, sum_mask);

                __m256i color = _mm256_setr_m128i
                (
                        _mm_add_epi64 (_mm256_extracti128_si256 (low_sum, 1), _mm256_extracti128_si256 (low_sum, 0)),
                        _mm_add_epi64 (_mm256_extracti128_si256 (up_sum, 1), _mm256_extracti128_si256 (up_sum, 0))
                );

                _mm256_store_si256((__m256i*) unpacked_color, color);

                for (int i = 0; i < 8; i++)
                {
                    array[background_pxl + i * PIXEL]       = unpacked_color[i].r;
                    array[background_pxl + i * PIXEL + 1]   = unpacked_color[i].g;
                    array[background_pxl + i * PIXEL + 2]   = unpacked_color[i].b;
                    array[background_pxl + i * PIXEL + 3]   = unpacked_color[i].a;
                }
            }
        }

    //     extra_rounds++;
    // }

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
