// test sys_page_alloc()

#include <inc/lib.h>

void
write(uint16_t *addr, uint16_t value)
{
	int r;

	if ((r = sys_page_alloc(0, addr, PTE_P | PTE_U | PTE_W)) < 0)
		panic("sys_page_alloc: %e", r);
	addr[0] = value;
	if ((r = sys_page_unmap(0, addr)) < 0)
		panic("sys_page_unmap: %e", r);
}

void
check(uint16_t *addr)
{
	int r;

	if ((r = sys_page_alloc(0, addr, PTE_P | PTE_U)) < 0)
		panic("sys_page_alloc: %e", r);
	if (addr[0] != '\0')
		panic("The allocated memory is not initialized to zero");
	if ((r = sys_page_unmap(0, addr)) < 0)
		panic("sys_page_unmap: %e", r);
}

void
umain(int argc, char **argv)
{
	write(UTEMP, 0x7508);
	check(UTEMP);
	cprintf("The allocated memory is initialized to zero\n");
}
