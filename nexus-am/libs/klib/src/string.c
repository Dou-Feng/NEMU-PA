#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_STR __UINT32_MAX__
size_t strlen(const char *s) {
  // assert(s != NULL);
  size_t len;
  for (len = 0; len < MAX_STR && s[len] != '\0'; len++);
  return len;
}

char *strcpy(char* dst,const char* src) {
  // assert((dst != NULL) && (src != NULL));
  char *r = dst;
  while ((*r++ = *src++) != '\0') {
    // _putc(*(r-1));
  }
  // _putc('\n');
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  // assert((dst != NULL) && (src != NULL));
  char *r = dst;
  int t = 0;
  while (t++ < n && ((*r++ = *src++) != '\0'));
  return dst;
}

char* strcat(char* dst, const char* src) {
  // assert((dst != NULL) && (src != NULL));
  char *r = dst;
  while (*r != '\0') {
    r++;
  }
  while ((*r++ = *src++) != '\0');
  // _putc('c'); _putc('a');_putc('t');_putc(':');_putc(' ');
  int i = 0;
  while (dst[i] != '\0') {
    _putc(dst[i]);
    i++;
  }
  // _putc('\n');
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  // assert((s1 != NULL) && (s2 != NULL));
  // if (s1 == NULL && s2 == NULL) return 0;
  // if (s2 == NULL) return 1;
  // else if (s1 == NULL) return -1;
  while (*s1 != '\0' && *s2 != '\0') {
    if (*s1 > *s2) {
      // _putc('c'); _putc('m');_putc('p');_putc(':');_putc('G');_putc('\n');
      return 1;
    } else if (*s1 < *s2) {
      // _putc('c'); _putc('m');_putc('p');_putc(':');_putc('L'); _putc('\n');
      return -1;
    }
    s1++; s2++;
  }
  if (*s1 != '\0') {
    // _putc('c'); _putc('m');_putc('p');_putc(':');_putc('G');_putc('\n');
    return 1;
  } else if (*s2 != '\0') {
    // _putc('c'); _putc('m');_putc('p');_putc(':');_putc('L');_putc('\n');
    return -1;
  } else {
    // _putc('c'); _putc('m');_putc('p');_putc(':');_putc('E');_putc('\n');
    return 0;
  }
}

int strncmp(const char* s1, const char* s2, size_t n) {
  // assert((s1 != NULL) && (s2 != NULL));
  // if (s1 == NULL && s2 == NULL) return 0;
  // if (s2 == NULL) return 1;
  // else if (s1 == NULL) return -1;
  int t = 0;
  while (t < n && *s1 != '\0' && *s2 != '\0') {
    if (*s1 > *s2) {
      return 1;
    } else if (*s1 < *s2) {
      return -1;
    }
    s1++; s2++; t++;
  }
  if (t == n) {
    return 0;
  }
  if (*s1 != '\0') {
    return 1;
  } else if (*s2 != '\0') {
    return -1;
  } else {
    return 0;
  }
}

void* memset(void* v,int c,size_t n) {
  assert(v != NULL);
  uint8_t byte = (uint8_t) c;
  for (int i = 0; i < n; i++) {
    ((uint8_t*)v)[i] = byte;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert((out != NULL) && (in != NULL));

  for (int i = 0; i < n; i++) {
    ((uint8_t*)out)[i] = ((uint8_t*)in)[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert((s1 != NULL) && (s2 != NULL));
  unsigned char c1, c2;
  for (int i = 0; i < n; i++) {
    c1 = ((unsigned char*)s1)[i];
    c2 = ((unsigned char*)s2)[i];
    if (c1 > c2) {
      return 1;
    } else if (c1 < c2) {
      return -1;
    }
  }
  return 0;
}

#endif
