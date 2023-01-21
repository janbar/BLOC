#include <stdint.h>

static uint_fast32_t hashvalue(uint_fast32_t maxsize, const char * buf, unsigned len)
{
  /*
   * DJB Hash Function
   */
  uint_fast32_t h = 5381;
  const char * end = buf + len;

  while (buf < end)
  {
    h = ((h << 5) + h) + *buf++;
  }
  return h % maxsize;
}
