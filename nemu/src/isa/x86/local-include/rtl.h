#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //TODO();
  //Log("before stack push, esp is %x", reg_l(R_ESP));
  rtlreg_t tmp = *src1; // Handle src1 == esp
  rtl_subi(s, &reg_l(R_ESP), &reg_l(R_ESP), 4);
  rtl_sm(s, &reg_l(R_ESP), 0, &tmp, 4); 
  //Log("stack push %x", *src1);
}

static inline def_rtl(pusha) {
  //TODO();
  if (s->isa.is_operand_size_16) {
    TODO();
  } else {
    rtlreg_t esp = reg_l(R_ESP);
    rtl_push(s, &reg_l(R_EAX));
    rtl_push(s, &reg_l(R_ECX));
    rtl_push(s, &reg_l(R_EDX));
    rtl_push(s, &reg_l(R_EBX));
    rtl_push(s, &esp);
    rtl_push(s, &reg_l(R_EBP));
    rtl_push(s, &reg_l(R_ESI));
    rtl_push(s, &reg_l(R_EDI));
  }
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  //TODO();
  //Log("before stack pop, esp is %x", reg_l(R_ESP));
  if (dest == &reg_l(R_ESP))
    TODO();
  rtl_lm(s, dest, &reg_l(R_ESP), 0, 4);
  rtl_addi(s, &reg_l(R_ESP), &reg_l(R_ESP), 4);
  //Log("stack pop %x", *dest);
}

static inline def_rtl(popa) {
  //TODO();
  if (s->isa.is_operand_size_16) {
    TODO();
  } else {
    rtlreg_t esp;
    rtl_pop(s, &reg_l(R_EDI));
    rtl_pop(s, &reg_l(R_ESI));
    rtl_pop(s, &reg_l(R_EBP));
    rtl_pop(s, &esp);
    rtl_pop(s, &reg_l(R_EBX));
    rtl_pop(s, &reg_l(R_EDX));
    rtl_pop(s, &reg_l(R_ECX));
    rtl_pop(s, &reg_l(R_EAX));
    reg_l(R_ESP) = esp;
  }
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  //TODO();
  rtlreg_t t1 = *src1>>31;
  rtlreg_t t2 = *src2>>31;
  if (t1 == t2) {
    *dest = 0;
    return;
  }
  rtlreg_t t3 = *res>>31;
  *dest = t3 == t2;
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  //TODO();
  *dest = *src1 < *src2;
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  //TODO();
  rtlreg_t t1 = *src1>>31;
  rtlreg_t t2 = *src2>>31;
  if (t1 != t2) {
    *dest = 0;
    return;
  }
  rtlreg_t t3 = *res>>31;
  *dest = t3 != t1;
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  //TODO();
  *dest = *res < *src1;
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    /*TODO();*/cpu.f = *src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    /*TODO();*/*dest = cpu.f; \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  //TODO();
  rtlreg_t zf = (*result&BITMASK(width*8)) == 0;
  rtl_set_ZF(s, &zf);
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  //TODO();
  rtlreg_t sf = *result;
  sf >>= width*8-1;
  sf &= 1;
  rtl_set_SF(s, &sf);
}

static inline def_rtl(clearCFOF, const rtlreg_t* _not_used) {
  rtlreg_t zero = 0; // FIXME: There's a regsiter for zero. But I forgot its name.
  rtl_set_CF(s, &zero);
  rtl_set_OF(s, &zero);
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

static inline def_rtl(rotl, rtlreg_t *dest, const rtlreg_t *count) {
  const unsigned int mask = (8*sizeof(rtlreg_t) - 1);
  rtlreg_t c = *count, n = *dest;

  if (c == 0) {
    rtlreg_t cf = 1;
    rtl_set_OF(s, &cf);
  }

  c &= mask;

  *dest = (n<<c) | (n>>( (-c)&mask ));
}

static inline def_rtl(rotr, rtlreg_t *dest, const rtlreg_t *count) {
  const unsigned int mask = (8*sizeof(rtlreg_t) - 1);
  rtlreg_t c = *count, n = *dest;
  
  if (c == 0) {
    rtlreg_t cf = 1;
    rtl_set_OF(s, &cf);
  }

  c &= mask;

  *dest = (n>>c) | (n<<( (-c)&mask ));
}

#endif
