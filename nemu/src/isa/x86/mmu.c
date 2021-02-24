#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  if (GET_DIR(vaddr) != GET_DIR(vaddr+len)) return MEM_RET_CROSS_PAGE;
  if (GET_PAGE(vaddr) != GET_PAGE(vaddr+len)) return MEM_RET_CROSS_PAGE;
  Log("translating 0x%08x, dir %d, page %d", vaddr, (int)GET_DIR(vaddr), (int)GET_PAGE(vaddr));
  paddr_t updir = cpu.cr3;
  paddr_t pde_addr = updir + sizeof(PTE)*GET_DIR(vaddr);
  PTE pde;
  pde.val = paddr_read(pde_addr, sizeof(PTE));
  assert(pde.present);
  paddr_t pt_addr = pde.page_frame_address << 12;
  PTE pte;
  pte.val = paddr_read(pt_addr + sizeof(PTE)*GET_PAGE(vaddr), sizeof(PTE));
  assert(pte.present);
  if (vaddr >= 0x100000 && vaddr <= 0x200000) {
    int ret = pte.page_frame_address == BITS(vaddr, 31, 12);
    if (ret == false) {
      Log("%x -> %x\n", vaddr, pte.page_frame_address);
      assert(0);
    }
  }
  return pte.page_frame_address << 12;
}
