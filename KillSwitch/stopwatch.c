#include "stopwatch.h"

#include <pmb887x.h>

static uint32_t ticks_per_s;
static uint32_t ticks_per_ms;
static uint32_t ticks_per_us;

void stopwatch_init() {
	uint32_t clock = (STM_CLC & MOD_CLC_RMC) >> MOD_CLC_RMC_SHIFT;
	
	ticks_per_s = 26000000 / clock;
	ticks_per_ms = ticks_per_s / 1000;
	ticks_per_us = ticks_per_s / 1000000;
}

uint64_t stopwatch_get() {
	return ((uint64_t) STM_TIM6 << 32) | (uint64_t) STM_TIM0;
}

uint64_t stopwatch_elapsed(uint64_t start) {
	return stopwatch_get() - start;
}

uint32_t stopwatch_elapsed_us(uint64_t start) {
	return stopwatch_elapsed(start) / ticks_per_us;
}

uint32_t stopwatch_elapsed_ms(uint64_t start) {
	return stopwatch_elapsed(start) / ticks_per_ms;
}

uint32_t stopwatch_elapsed_s(uint64_t start) {
	return stopwatch_elapsed(start) / ticks_per_s;
}
