#include <am.h>
#include <nemu.h>
#include <stdio.h>

static uint64_t start_time;

static uint64_t __am_read_timer() {
  uint64_t us = inl(0x48);
  uint64_t sec = inl(0x4c);
  return sec*1000000 + us;
}

void __am_timer_init() {
  start_time = __am_read_timer();
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = __am_read_timer() - start_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
