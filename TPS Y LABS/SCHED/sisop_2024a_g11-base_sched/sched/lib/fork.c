// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW 0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	pte_t fault_pte = uvpt[((uint32_t) addr) >> PGSHIFT];
	if ((err & FEC_WR) & !(fault_pte & PTE_COW))
		panic("pgfault: write fault on a non COW page");

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	int perm = PTE_P | PTE_U | PTE_W;
	if ((r = sys_page_alloc(0, PFTEMP, perm)) < 0)
		panic("pgfault: sys_page_alloc: %e", r);

	addr = ROUNDDOWN(addr, PGSIZE);
	memmove(PFTEMP, addr, PGSIZE);
	if ((r = sys_page_map(0, PFTEMP, 0, addr, perm)) < 0)
		panic("pgfault: sys_page_map: %e", r);

	if ((r = sys_page_unmap(0, PFTEMP)) < 0)
		panic("pgfault: sys_page_unmap: %e", r);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	void *addr = (void *) (pn << PGSHIFT);

	pte_t pte = uvpt[pn];

	if (!(pte & PTE_P) || !(pte & PTE_U))
		panic("duppage: copy a non-present or non-user page");

	int perm = PTE_U | PTE_P;
	// Verifico permisos para páginas de IO
	if (pte & (PTE_PCD | PTE_PWT))
		perm |= PTE_PCD | PTE_PWT;


	// Si es de escritura o copy-on-write y NO es de IO
	if ((pte & PTE_W) || (pte & PTE_COW)) {
		// Mappeo en el hijo la página
		if ((r = sys_page_map(0, addr, envid, addr, perm | PTE_COW)) < 0)
			panic("duppage: sys_page_map on childern COW: %e", r);

		// Cambio los permisos del padre
		if ((r = sys_page_map(0, addr, 0, addr, perm | PTE_COW)) < 0)
			panic("duppage: sys_page_map on parent COW: %e", r);
	} else {
		// Solo mappeo la página de solo lectura
		if ((r = sys_page_map(0, addr, envid, addr, perm)) < 0)
			panic("duppage: sys_page_map on childern RO: %e", r);
	}

	// panic("duppage not implemented");
	return 0;
}

/*
 * Parte 2: fork_v0
 * Mappea la pagina nro pnum en el espacio de direcciones
 * envid->env_pgdir con permisos perm.
 *
 * Si la pagina era de solo lectura, la comparte incluso
 * aunque (perm & PTE_W).
 * Si la pagina estaba marcada con PTE_W, se copia en el
 * environment envid.
 *
 * Hace uso de uvpd y uvpt para chequear permisos.
 */
static void
dup_or_share(envid_t envid, uint32_t pnum, int perm)
{
	int r;
	void *addr = (void *) (pnum << PGSHIFT);

	pte_t pte = uvpt[pnum];

	if (!(pte & PTE_P))
		return;

	// Filtro permisos
	perm &= (pte & PTE_W);
	perm &= PTE_SYSCALL;


	if (pte & (PTE_PCD | PTE_PWT))
		perm |= PTE_PCD | PTE_PWT;

	// Si tengo que copiar
	if (!(pte & PTE_W) || (pte & (PTE_PCD | PTE_PWT))) {
		if ((r = sys_page_map(0, addr, envid, addr, perm)) < 0)
			panic("dup_or_share: sys_page_map: %e", r);
	} else {
		// Pido una nueva pagina
		if ((r = sys_page_alloc(envid, addr, perm)) < 0)
			panic("dup_or_share: sys_page_alloc: %e", r);
		if ((r = sys_page_map(envid, addr, 0, UTEMP, perm)) < 0)
			panic("dup_or_share: sys_page_map: %e", r);
		memmove(UTEMP, addr, PGSIZE);
		if ((r = sys_page_unmap(0, UTEMP)) < 0)
			panic("sys_page_unmap: %e", r);
	}
}

/*
 *  Parte 2: fork_v0
 *  Implementacion de fork intermedia entre el dumbfork de userspace
 *  y la versión final copy-on-write de fork()
 *
 *  Crea un nuevo environment mediante sys_exofork()
 *  Mappea en el hijo las mismas paginas que el padre
 *  	-- Las paginas marcadas con PTE_W son copiadas
 *  	-- Las paginas de solo lecturas son compartidas
 *  	-- Para ello se utiliza la funcion dup_or_share()
 *  	-- Se copian todas las paginas desde 0 hasta UTOP
 */
envid_t
fork_v0(void)
{
	envid_t envid = sys_exofork();
	if (envid < 0)
		panic("sys_exofork: %e", envid);
	if (envid == 0) {
		// Child
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// Parent
	uint32_t pnum = 0;
	uint32_t pnum_end = (UTOP >> PGSHIFT);
	int perm = PTE_P | PTE_U | PTE_W;

	for (pnum = 0; pnum < pnum_end; pnum++) {
		pde_t pde = uvpd[pnum >> 10];
		if (!(pde & PTE_P) || !(pde & PTE_U))
			continue;
		dup_or_share(envid, pnum, perm);
	}


	int r;
	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);

	return envid;
}


//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	set_pgfault_handler(pgfault);

	envid_t envid = sys_exofork();
	if (envid < 0)
		panic("sys_exofork: %e", envid);
	if (envid == 0) {
		// Child
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// Parent
	uint32_t pnum = 0;
	uint32_t pnum_end = (UTOP >> PGSHIFT);

	// Handle all pages below UTOP
	for (pnum = 0; pnum < pnum_end; pnum++) {
		if (pnum == ((UXSTACKTOP >> PGSHIFT) - 1))
			continue;

		pde_t pde = uvpd[pnum >> 10];
		if (!(pde & PTE_P) || !(pde & PTE_U))
			continue;

		pte_t pte = uvpt[pnum];
		if (!(pte & PTE_P) || !(pte & PTE_U))
			continue;
		duppage(envid, pnum);
	}

	uint32_t exstk = (UXSTACKTOP - PGSIZE);
	int r = sys_page_alloc(envid, (void *) exstk, PTE_U | PTE_P | PTE_W);
	if (r < 0)
		panic("fork: sys_page_alloc of exception stk: %e", r);
	r = sys_env_set_pgfault_upcall(envid, thisenv->env_pgfault_upcall);
	if (r < 0)
		panic("fork: sys_env_set_pgfault_upcall on childern: %e", r);

	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);

	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
