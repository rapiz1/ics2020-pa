#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keycode = inl(0x60);
  kbd->keydown = kbd->keycode & KEYDOWN_MASK;
  kbd->keycode = kbd->keycode & (~KEYDOWN_MASK);
}
