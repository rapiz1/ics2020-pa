#include <am.h>
#include <nemu.h>
#include <stdio.h>

static uint64_t __am_start_secs;

void __am_timer_init() {
  __am_start_secs = inl(0x4c);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t secs = inl(0x4c);
  uint64_t usecs = inl(0x48);
  uptime->us = (secs - __am_start_secs)*1000000 + usecs;
  //printf("%d\n", (uint32_t)usecs);
  //uptime->us = secs*1000000;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
