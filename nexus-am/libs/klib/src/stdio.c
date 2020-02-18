#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define bool uint8_t

// convert number to character
void num2char(char *c, int s, int small) {
  assert(s >= 0 && s <= 15);
  if (s < 10) {
    *c = s + '0';
  } else if (small) {
    *c = s - 10 + 'a';
  } else {
    *c = s - 10 + 'A';
  }
}

void int2str(char *dst, int src, int radix, int small) {
  assert(radix == 10 || radix == 16);
  if (src == 0) {
    dst[0] = '0';
    dst[1] = '\0';
    return;
  }
  int n = src;
  int rem = 0;
  int i = 0;
  if (n < 0 && radix == 10) {
    dst[i++] = '-';
  }
  n = n<0?-n:n;
  int j = 0;
  char buf[20];
  while (n != 0) {
    if (radix == 10) {
      rem = n % 10;
      n = n / 10;
    } else if (radix == 16) {
      rem = n & 0xf;
      n >>= 4;
    }
    num2char(buf+j++, rem, small);
  }
  while (--j >= 0) {
    dst[i++] = buf[j];
  }
  dst[i] = '\0';
}

#define hex(dst, n, small) int2str(dst, n, 16, small)
#define decimal(dst, n) int2str(dst, n, 10, 0)


void print_str(const char *s) {
  assert(s != NULL);
  int i = 0;
  while (s[i] != '\0') {
    _putc(s[i++]);
  }
}

bool isDecimal(char c) {
  return c >= '0' && c <= '9';
}

int parse_num(const char *src, int st, int *en) {
	assert(src != NULL && isDecimal(src[st]));
	*en = st+1;
	int res = src[st] - '0';
	while (isDecimal(src[*en])) {
		res = res * 10 + (src[*en] - '0');
		(*en)++;
	}
	return res;
}

const char *zero = "0";

void fill_with_zero(char *dst, char *src, int placeholder, size_t *ch_num) {
	int len  = strlen(src);
	if (placeholder <= len) {
		if (dst != NULL)
			strcpy(dst, src);
		else
			print_str(src);
		return;
	}
	int dst_len = 0;
	for (int i = 0; i < placeholder-len; i++) {
		(*ch_num)++;
		if (dst != NULL)
			dst[dst_len++] = '0';
		else
			_putc('0');
	}
	if (dst != NULL)
		strcpy(dst+dst_len, src);
	else
		print_str(src);
}

int format_output(char *out, const char *fmt, va_list ap) {
  int len = strlen(fmt);
  size_t p_len = 0; // to record the printed character number
  int i = 0;
  char buf[40];
  int d;
  char *p = NULL;
  while (i < len) {
    if (fmt[i] != '%') {
      if (out) {
        out[p_len++] = fmt[i++];
      } else {
        _putc(fmt[i++]);
        p_len++;
      }
    } else {
      i++;
      int placeholder = 0;
      if (fmt[i] == '0') {
        placeholder = parse_num(fmt, i+1, &i);
      }
      char op = fmt[i];
      i++;
      // get info
      switch(op) {
        case 'd': // int
          d = va_arg(ap, int);
          decimal(buf, d);
          break;
        case 'x':
          d = va_arg(ap, int);
          hex(buf, d, 1);
          break;
        case 'X':
          d = va_arg(ap, int);
          hex(buf, d, 0);
          break;
        case 's': // str
          p = va_arg(ap, char *);
          break;
      }
      // print res
      switch(op) {
    		case 'd': // int
        case 'x':
        case 'X':
          fill_with_zero(out?out+p_len:NULL, buf, placeholder, &p_len);
          p_len += strlen(buf);
          break;
        case 's': // str
          if (out)
            strcpy(out+p_len, p);
          else
            print_str(p);
          p_len += strlen(p);
          break;
      }
    }
  }
  if (out)
    out[p_len] = '\0';
  return p_len;
}


int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = format_output(NULL, fmt, ap);
  va_end(ap);
  return n;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = format_output(out, fmt, ap);
  va_end(ap);
  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
