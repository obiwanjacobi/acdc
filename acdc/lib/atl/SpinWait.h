#pragma once
#include <stdint.h>

static void SpinWait(uint16_t spinDelay)
{
    while (spinDelay-- > 0)
    {
        __asm__ __volatile__("nop");
    }
}
