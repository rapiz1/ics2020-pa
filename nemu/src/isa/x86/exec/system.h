#include <monitor/difftest.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

static inline def_EHelper(lidt) {
  rtlreg_t limit = 0, base = 0;
  vaddr_t addr = *s->isa.mbase + s->isa.moff;
  rtl_lm(s, &limit, &addr, 0, 2);
  rtl_lm(s, &base, &addr, 2, 4);
  cpu.idtr.limit = limit;
  cpu.idtr.base = base;
  Log("idtr base:limit=%x:%x at mem %x", base, limit, addr);
  print_asm_template1(lidt);
}

static inline def_EHelper(mov_r2cr) {
  TODO();
  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  TODO();
  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
  //TODO();
  raise_intr(s, s->dest.imm, s->seq_pc);
  print_asm("int %s", s->dest.str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
  //TODO();
  if (s->isa.is_operand_size_16) TODO();
  rtl_pop(s, &reg_l(R_ESP));
  rtl_pop(s, &cpu.cs);
  rtl_pop(s, &cpu.eflags);
  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
  //TODO();
  switch (id_dest->width) {
  case 1:
    *id_dest->preg = pio_read_b(*id_src1->preg);
    break;
  case 2:
    *id_dest->preg = pio_read_w(*id_src1->preg);
    break;
  case 4: 
    *id_dest->preg = pio_read_l(*id_src1->preg);
    break;
  default:
    TODO();
    break;
  };
  print_asm_template2(in);
}

static inline def_EHelper(out) {
  //TODO();
  uint32_t val = *id_src1->preg;
  switch (id_src1->width) {
  case 1:
    pio_write_b(*id_dest->preg, val);
    break;
  case 2:
    pio_write_w(*id_dest->preg, val);
    break;
  case 4: 
    pio_write_l(*id_dest->preg, val);
    break;
  default:
    TODO();
    break;
  };
  print_asm_template2(out);
}
