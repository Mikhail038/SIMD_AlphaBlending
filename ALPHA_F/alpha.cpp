//================================================================================
//08.04.2023
//================================================================================

#include "alpha.h"

//================================================================================

const char  I = 0xFFu;

const char  ShuffleZERO = 0x80u;

const __m256i   ZeroVector  = _mm256_set1_epi8 (0);
const __m256i  IdentVector  = _mm256_set1_epi8 (I);

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

        printf("avg_FPS: %g\n", timer->fps);

        timer->frames_cnt = 0;

        return timer->fps;
    }
    else
    {
        timer->frames_cnt++;
    }

    return 0;
}
