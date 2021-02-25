#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
  if (GET_DIR(vaddr) != GET_DIR(vaddr+len-1)) return MEM_RET_CROSS_PAGE;
  if (GET_PAGE(vaddr) != GET_PAGE(vaddr+len-1)) return MEM_RET_CROSS_PAGE;
  paddr_t updir = cpu.cr3;
  paddr_t pde_addr = updir + sizeof(PTE)*GET_DIR(vaddr);
  PTE pde;
  pde.val = paddr_read(pde_addr, sizeof(PTE));
  if (!pde.present) goto failed;
  paddr_t pt_addr = pde.page_frame_address << 12;
  PTE pte;
  pte.val = paddr_read(pt_addr + sizeof(PTE)*GET_PAGE(vaddr), sizeof(PTE));
  if (!pte.present) goto failed;
  if (vaddr <= 0x8000000) {
    int ret = pte.page_frame_address == BITS(vaddr, 31, 12);
    if (ret == false) {
      Log("0x%x -> 0x%x\n", vaddr, (uint32_t)((pte.page_frame_address<<12)|GET_OFFSET(vaddr)));
      assert(0);
    }
  }
  return pte.page_frame_address << 12;
  failed:
  isa_reg_display();
  Log("translating 0x%08x", vaddr);
  assert(0);
}
