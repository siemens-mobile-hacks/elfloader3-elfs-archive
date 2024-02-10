#include <swilib.h>

static inline void disable_irq() {
	volatile uint32_t cpsr;
	__asm__ volatile("MRS %0, cpsr" : "=r" (cpsr) : );
	cpsr |= 0xC0;
	__asm__ volatile("MSR  CPSR_c, %0" :  : "r" (cpsr));
}

static inline void enable_irq() {
	volatile uint32_t cpsr;
	__asm__ volatile("MRS %0, cpsr" : "=r" (cpsr) : );
	cpsr &= ~0xC0;
	__asm__ volatile("MSR  CPSR_c, %0" :  : "r" (cpsr));
}

static inline void system_mode() {
	#ifdef NEWSGOLD
	__asm__ volatile("SWI 4");
	#else
	__asm__ volatile("SWI 0");
	#endif
}

static inline uint32_t get_domain_access() {
	system_mode();
	volatile uint32_t domains;
	__asm__ volatile("mrc p15, 0, %0, c3, c0, 0" : "=r" (domains));
	return domains;
}

static inline uint32_t *get_mmu_table() {
	uint32_t *addr;
	__asm__ volatile("mrc p15, 0, %0, c2, c0, 0" : "=r" (addr));
	return addr;
}

static inline uint32_t set_domain_access(volatile uint32_t domains) {
	system_mode();
	disable_irq();
	volatile uint32_t old_domains;
	__asm__ volatile("mrc p15, 0, %0, c3, c0, 0" : "=r" (old_domains));
	__asm__ volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (domains));
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	__asm__ volatile("NOP");
	enable_irq();
	return old_domains;
}

static inline uint32_t safe_read_reg(volatile uint32_t *reg) {
	system_mode();
	
	uint32_t value;
	
	disable_irq();
	uint32_t pll_mmci_sleep = PLL_CON3 & 1;
	PLL_CON3 = PLL_CON3 & ~1;
	value = MMIO32((uint32_t) reg);
	PLL_CON3 = PLL_CON3 | pll_mmci_sleep;
	enable_irq();
	
	return value;
}

static inline void safe_write_reg(volatile uint32_t *reg, uint32_t value) {
	system_mode();
	
	disable_irq();
	uint32_t pll_mmci_sleep = PLL_CON3 & 1;
	PLL_CON3 = PLL_CON3 & ~1;
	MMIO32((uint32_t) reg) = value;
	PLL_CON3 = PLL_CON3 | pll_mmci_sleep;
	enable_irq();
}
