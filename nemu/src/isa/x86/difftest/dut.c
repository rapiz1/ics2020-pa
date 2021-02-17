#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 8; i++) {
    if (cpu.gpr[i]._32 != ref_r->gpr[i]._32) {
      return false;
    }
  }
  if (cpu.eflags != ref_r->eflags) return false;
  return cpu.pc == ref_r->pc;
}

void isa_difftest_attach() {
}
