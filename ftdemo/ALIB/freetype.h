#pragma once

#include <de/freetype.h>

ft_font *_ft_open(const char *font, uint16_t size, int load);
void _ft_close(ft_font *f);
