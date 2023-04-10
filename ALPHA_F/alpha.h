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

//================================================================================

void start_fps_count (sf::Clock* clock, STimer* timer);

double get_fps_count (sf::Clock* clock, STimer* timer);

//================================================================================
