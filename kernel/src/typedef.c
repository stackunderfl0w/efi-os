#include "typedef.h"
void* memset (void *dest, int val, size_t len){
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}
void* memcpy (void *dest, const void *src, size_t len){
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}
wchar_t* wmemset (wchar_t *s, wchar_t c, size_t n){
  wchar_t *wp = s;
  while (n >= 4)
    {
      wp[0] = c;
      wp[1] = c;
      wp[2] = c;
      wp[3] = c;
      wp += 4;
      n -= 4;
    }
  if (n > 0)
    {
      wp[0] = c;
      if (n > 1)
        {
          wp[1] = c;
          if (n > 2)
            wp[2] = c;
        }
    }
  return s;
}
void* intset(uint32_t *s, const uint32_t c, size_t n){
  uint32_t *wp = s;
  while (n>=2){
    wp[0]=c;
    wp[1]=c;
    wp+=2;
    n-=2;
  }
  if (n > 0){
    wp[0] = c;
    }
    //while(n>0){
    //  s[n--]=c;
    //}


}

char* strstr(char* string, char* substring){
    char *a, *b;


    b = substring;
    if (*b == 0) {
  return string;
    }
    for ( ; *string != 0; string += 1) {
  if (*string != *b) {
      continue;
  }
  a = string;
  while (1) {
      if (*b == 0) {
    return string;
      }
      if (*a++ != *b++) {
    break;
      }
  }
  b = substring;
    }
    return NULL;
}
char* strrstr(const char* haystack, const char* needle){
    if (*needle == '\0')
        return (char *) haystack;

    char *result = NULL;
    for (;;) {
        char *p = (char*)(strstr(haystack, needle));
        if (p == NULL)
            break;
        result = p;
        haystack = p + 1;
    }
    return result;
}