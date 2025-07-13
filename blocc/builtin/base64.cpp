/*
 *      Copyright (C) 2025 Jean-Luc Barriere
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "base64.h"

namespace bloc
{

static const char * B64chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const int B64index[256] =
{
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  62, 63, 62, 62, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63,
  0,  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

void b64encode(const void * data, size_t len, Literal& b64)
{
  size_t b64len = (len + 2) / 3 * 4;
  b64.assign(b64len, '=');

  char * p = (char*) data;
  size_t j = 0, pad = len % 3;
  const size_t last = len - pad;

  for (size_t i = 0; i < last; i += 3)
  {
    int n = int(p[i]) << 16 | int(p[i + 1]) << 8 | p[i + 2];
    b64[j++] = B64chars[n >> 18];
    b64[j++] = B64chars[n >> 12 & 0x3F];
    b64[j++] = B64chars[n >> 6 & 0x3F];
    b64[j++] = B64chars[n & 0x3F];
  }
  if (pad)  /// set padding
  {
    int n = --pad ? int(p[last]) << 8 | p[last + 1] : p[last];
    b64[j++] = B64chars[pad ? n >> 10 & 0x3F : n >> 2];
    b64[j++] = B64chars[pad ? n >> 4 & 0x03F : n << 4 & 0x3F];
    b64[j++] = pad ? B64chars[n << 2 & 0x3F] : '=';
  }
}

void b64decode(const void * b64, size_t len, TabChar& data)
{
  if (len == 0)
    return;

  unsigned char *p = (unsigned char*) b64;
  size_t j = 0,
      pad1 = len % 4 || p[len - 1] == '=',
      pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
  const size_t last = (len - pad1) / 4 << 2;
  size_t datalen = last / 4 * 3 + pad1 + pad2;
  data.assign(datalen, '\0');

  for (size_t i = 0; i < last; i += 4)
  {
    int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
    data[j++] = n >> 16;
    data[j++] = n >> 8 & 0xFF;
    data[j++] = n & 0xFF;
  }
  if (pad1)
  {
    int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
    data[j++] = n >> 16;
    if (pad2)
    {
      n |= B64index[p[last + 2]] << 6;
      data[j++] = n >> 8 & 0xFF;
    }
  }
}

}
