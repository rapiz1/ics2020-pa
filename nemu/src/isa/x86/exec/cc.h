#ifndef __CC_H__
#define __CC_H__

#include "../local-include/rtl.h"

enum {
  CC_O, CC_NO, CC_B,  CC_NB,
  CC_E, CC_NE, CC_BE, CC_NBE,
  CC_S, CC_NS, CC_P,  CC_NP,
  CC_L, CC_NL, CC_LE, CC_NLE
};


/* Condition Code */

static inline const char* get_cc_name(int subcode) {
  static const char *cc_name[] = {
    "o", "no", "b", "nb",
    "e", "ne", "be", "nbe",
    "s", "ns", "p", "np",
    "l", "nl", "le", "nle"
  };
  return cc_name[subcode];
}

static inline void rtl_setcc(DecodeExecState *s, rtlreg_t* dest, uint32_t subcode) {
  uint32_t invert = subcode & 0x1;

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  rtlreg_t t1, t2;
  switch (subcode & 0xe) {
    case CC_O:
      rtl_get_OF(s, dest);
      break;
    case CC_B:
      rtl_get_CF(s, dest);
      break;
    case CC_E:
      rtl_get_ZF(s, dest);
      break;
    case CC_BE:
      rtl_get_CF(s, &t1);
      rtl_get_ZF(s, &t2);
      *dest = t1 || t2;
      break;
    case CC_S:
      rtl_get_SF(s, dest);
      break;
    case CC_L:
      rtl_get_SF(s, &t1);
      rtl_get_OF(s, &t2);
      *dest = t1 != t2;
      break;
    case CC_LE:
      rtl_get_SF(s, &t1);
      rtl_get_OF(s, &t2);
      *dest = t1 != t2;
      rtl_get_ZF(s, &t1);
      *dest = *dest || t1;
      break;
    default: panic("should not reach here");
    case CC_P: panic("PF is not supported");
  }

  if (invert) {
    rtl_xori(s, dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}

#endif
