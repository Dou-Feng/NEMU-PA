#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void int2str(char *dst, int src) {
  if (src == 0) {
    dst[0] = '0';
    dst[1] = '\0';
    return;
  }
  int n = src;
  int rem = 0;
  int i = 0;
  if (n < 0) {
    dst[i++] = '-';
    n = -n;
  }
  int j = 0;
  char buf[20];
  while (n != 0) {
    rem = n % 10;
    n = n / 10;
    buf[j++] = rem + '0';
  }
  while (--j >= 0) {
    dst[i++] = buf[j];
  }
  dst[i] = '\0';
}

// void printStr(const char *s) {
//   assert(s != NULL);
//   int i = 0;
//   while (s[i] != '\0') {
//     putchar(s[i++]);
//   }
// }

int printf(const char *fmt, ...) {
  // int len = strlen(fmt);
  // size_t p_len = 0; // to record the printed character number
  // int i = 0;
  // char buf[40];
  // int d;
  // char *p;
  // va_list ap;
  // va_start(ap, fmt);
  // while (i < len) {
  //   if (fmt[i] != '%') {
  //     putchar(fmt[i]);
  //     p_len++;
  //   } else {
  //     i++;
  //     switch(fmt[i]) {
  //       case 'd': // int
  //         d = va_arg(ap, int);
  //         int2str(buf, d);
  //         p_len += strlen(buf);
  //         printStr(buf);
  //         break;
  //       case 's': // str
  //         p = va_arg(ap, char *);
  //         printStr(p);
  //         p_len += strlen(p);
  //         break;
  //     }
  //     i++;
  //   }
  // }
  // va_end(ap);
  // return p_len;
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  int len = strlen(fmt);
  size_t p_len = 0; // to record the printed character number
  int i = 0;
  char buf[40];
  int d;
  char *p;
  va_list ap;
  va_start(ap, fmt);
  while (i < len) {
    if (fmt[i] != '%') {
      out[p_len++] = fmt[i++];
    } else {
      i++;
      switch(fmt[i]) {
        case 'd': // int
          d = va_arg(ap, int);
          int2str(buf, d);
          strcpy(out+p_len, buf);
          p_len += strlen(buf);
          break;
        case 's': // str
          p = va_arg(ap, char *);
          strcat(out, p);
          strcpy(out+p_len, p);
          p_len += strlen(p);
          break;
      }
      i++;
    }
  }
  va_end(ap);
  out[p_len] = '\0';
  return p_len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
