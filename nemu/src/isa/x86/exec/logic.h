#include "cc.h"

static inline def_EHelper(test) {
  //TODO();
  rtlreg_t tmp;
  rtl_and(s, &tmp, s->dest.preg, s->src1.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, &tmp, s->dest.width);
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  //TODO();
  rtl_and(s, s->dest.preg, s->dest.preg, s->src1.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  //TODO();
  rtl_xor(s, s->dest.preg, s->dest.preg, s->src1.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  //TODO();
  rtl_or(s, s->dest.preg, s->dest.preg, s->src1.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  print_asm_template2(or);
}

static inline def_EHelper(not) {
  //TODO();
  rtl_not(s, s->dest.preg, s->dest.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  print_asm_template1(not);
}

static inline def_EHelper(sar) {
  //TODO();
  rtl_sar(s, s->dest.preg, s->dest.preg, s->src1.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  //TODO();
  rtl_shl(s, s->dest.preg, s->dest.preg, s->src1.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  //TODO();
  rtl_shr(s, s->dest.preg, s->dest.preg, s->src1.preg);
  operand_write(s, &s->dest, s->dest.preg);
  rtl_clearCFOF(s, NULL);
  rtl_update_ZFSF(s, s->dest.preg, s->dest.width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shr);
}


static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(bsr) {
  //TODO();
  Log("bsr %x width %d, dest %x", *id_src1->preg, id_src1->width, *id_dest->preg);
  rtlreg_t zf = 0;
  if (id_src1->val == 0) {
    zf = 1;
  } else {
    rtlreg_t temp = id_src1->width-1;
    zf = 0;
    while (temp != 0 && BIT(id_src1->val, temp) == 0) {
      temp--;
      *id_dest->preg = temp;
    }
    operand_write(s, id_dest, id_dest->preg);
  }
  rtl_set_ZF(s, &zf);
  print_asm_template2(bsr);
}
