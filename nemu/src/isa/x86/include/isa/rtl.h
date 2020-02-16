#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  cpu.esp -= 4;
  Assert(cpu.esp >= 0, "cpu.esp < 0");
  paddr_write(cpu.esp, *src1, 4); // default width is 4
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  *dest = paddr_read(cpu.esp, 4);
  cpu.esp += 4;
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  // It comes to src1 + src2 = res, overflow means src1 and src2 have the same sign, 
  // and res's sign is opposite to src1.
  // src1 - src2 = res can convert to src1 + (-src2) = res
  *dest = (*src1 & (0x80 << ((width-1)*8))) ^ ((-*src2) & (0x80 << ((width-1)*8)));
  if (!*dest) { // if signs of src1 and (-src2) are the same
    // if signs of res and src1 are opposite, it indicates overflow
    *dest = ((*src1 & (0x80 << ((width-1)*8))) ^ (*res & (0x80 << ((width-1)*8))))?1:0;
  } else {
    *dest = 0;
  }
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  *dest = *res>*src1?1:0;
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  // It comes to src1 + src2 = res, overflow means src1 and src2 have the same sign, 
  // and res's sign is opposite to src1.
  *dest = (*src1 & (0x80 << ((width-1)*8))) ^ (*src2 & (0x80 << ((width-1)*8)));
  if (!*dest) { // if signs of src1 and src2 are the same
    // if signs of res and src1 are opposite, it indicates overflow
    *dest = ((*src1 & (0x80 << ((width-1)*8))) ^ (*res & (0x80 << ((width-1)*8))))?1:0;
  } else {
    *dest = 0;
  }
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  *dest = *res<*src1?1:0;
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    EFLAGS(f) = *src?1:0; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = EFLAGS(f)?1:0; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  if (width == 4) {
    cpu.ZF = *result?0:1;
  } else {
    cpu.ZF = (*result & (~(0xffffffff << (width*8))))?0:1;
  }
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  cpu.SF = (*result & (0x80 << ((width-1)*8)))?1:0;
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
