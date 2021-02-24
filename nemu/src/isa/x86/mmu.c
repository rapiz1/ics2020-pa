#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  if (GET_DIR(vaddr) != GET_DIR(vaddr+len)) return MEM_RET_CROSS_PAGE;
  if (GET_PAGE(vaddr) != GET_PAGE(vaddr+len)) return MEM_RET_CROSS_PAGE;
  paddr_t updir = cpu.cr3;
  int dir_index = GET_DIR(vaddr);
  paddr_t dir = updir + sizeof(PTE)*dir_index;
  assert(sizeof(PTE) == 4);
  PTE pde;
  pde.val = paddr_read(dir, sizeof(PTE));
  assert(pde.present);
  paddr_t page_table = pde.page_frame_address << 12;
  PTE pte;
  pte.val = paddr_read(page_table, sizeof(PTE));
  assert(pte.present);
  if (vaddr >= 0x100000 && vaddr <= 0x200000) {
    int ret = pte.page_frame_address == BITS(vaddr, 31, 12);
    if (ret == false) {
      printf("%d -> %d\n", vaddr, pte.page_frame_address);
      assert(0);
    }
  }
  return pte.page_frame_address << 12;
}
