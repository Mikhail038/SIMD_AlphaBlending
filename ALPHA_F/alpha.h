#pragma once

//================================================================================

#include <SFML/Graphics.hpp>

#include <immintrin.h>
#include <x86intrin.h>

//================================================================================

typedef struct
{
    int     frames_amnt;
    double  fps;
    int     frames_cnt;
}STimer;

typedef struct
{
    sf::Vector2<int> front;
    sf::Vector2<int> back;
}SConfig;

//================================================================================

void extend_array (sf::Uint8* array, int* width, int* height, int multiple);

void blender (SConfig* config, sf::Image* res_image, const sf::Uint8* front_array, const sf::Uint8* back_array);

void blend_them_no_avx (SConfig* config,sf::Uint8* array, const sf::Uint8* front_array);

void blend_them_avx (SConfig* config, sf::Uint8* array, const sf::Uint8* front_array);

//================================================================================

void start_fps_count (sf::Clock* clock, STimer* timer);

double get_fps_count (sf::Clock* clock, STimer* timer);

//================================================================================
