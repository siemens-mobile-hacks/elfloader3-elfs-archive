#include <swilib.h>

void stopwatch_init();
uint64_t stopwatch_get();
uint64_t stopwatch_elapsed(uint64_t start);
uint32_t stopwatch_elapsed_us(uint64_t start);
uint32_t stopwatch_elapsed_ms(uint64_t start);
uint32_t stopwatch_elapsed_s(uint64_t start);
