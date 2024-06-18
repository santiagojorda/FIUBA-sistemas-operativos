// Forkless spin; pid 0 spins, pid 1 yields.

#include <inc/lib.h>
#define TICK (1U << 15)

void
umain(int argc, char **argv)
{
	envid_t me = sys_getenvid();
	unsigned n = 0;
	bool yield = me & 1;

	while (n++ < 5 || !yield) {
		unsigned i = TICK;
		while (i--)
			;
		if (yield) {
			cprintf("I am %08x and I like my interrupt #%u\n", me, n);
			sys_yield();
		} else {
			cprintf("I am %08x and my spin will go on #%u\n", me, n);
		}
	}
}
