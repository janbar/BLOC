/*
  Copyright (C) 2021  Jean-Luc Barriere

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include "utf8helper_charmap.h"

namespace utf8helper
{

const character* pagemap_16[32] = {
  /* c0 */
  nullptr, nullptr, charmap_c2, charmap_c3, charmap_c4, charmap_c5, charmap_c6, charmap_c7,
  charmap_c8, charmap_c9, charmap_ca, charmap_cb, charmap_cc, charmap_cd, charmap_ce, charmap_cf,
  charmap_d0, charmap_d1, charmap_d2, charmap_d3, charmap_d4, charmap_d5, charmap_d6, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
};

const character charmap_us7ascii[128] = {
  { 0x00 , 0x00 , 0x00 , None , "" },
  { 0x01 , 0x01 , 0x01 , IsControl , "" },
  { 0x02 , 0x02 , 0x02 , IsControl , "" },
  { 0x03 , 0x03 , 0x03 , IsControl , "" },
  { 0x04 , 0x04 , 0x04 , IsControl , "" },
  { 0x05 , 0x05 , 0x05 , IsControl , "" },
  { 0x06 , 0x06 , 0x06 , IsControl , "" },
  { 0x07 , 0x07 , 0x07 , IsControl , "" },
  { 0x08 , 0x08 , 0x08 , IsControl , "" },
  { 0x09 , 0x09 , 0x09 , IsSpace | IsBreaker , " " },
  { 0x0a , 0x0a , 0x0a , IsBreaker , "\x0a" },
  { 0x0b , 0x0b , 0x0b , IsControl , "" },
  { 0x0c , 0x0c , 0x0c , IsControl , "" },
  { 0x0d , 0x0d , 0x0d , IsControl , "" },
  { 0x0e , 0x0e , 0x0e , IsControl , "" },
  { 0x0f , 0x0f , 0x0f , IsControl , "" },
  { 0x10 , 0x10 , 0x10 , IsControl , "" },
  { 0x11 , 0x11 , 0x11 , IsControl , "" },
  { 0x12 , 0x12 , 0x12 , IsControl , "" },
  { 0x13 , 0x13 , 0x13 , IsControl , "" },
  { 0x14 , 0x14 , 0x14 , IsControl , "" },
  { 0x15 , 0x15 , 0x15 , IsControl , "" },
  { 0x16 , 0x16 , 0x16 , IsControl , "" },
  { 0x17 , 0x17 , 0x17 , IsControl , "" },
  { 0x18 , 0x18 , 0x18 , IsControl , "" },
  { 0x19 , 0x19 , 0x19 , IsControl , "" },
  { 0x1a , 0x1a , 0x1a , IsControl , "" },
  { 0x1b , 0x1b , 0x1b , IsControl , "" },
  { 0x1c , 0x1c , 0x1c , IsControl , "" },
  { 0x1d , 0x1d , 0x1d , IsControl , "" },
  { 0x1e , 0x1e , 0x1e , IsControl , "" },
  { 0x1f , 0x1f , 0x1f , IsControl , "" },
  { 0x20 , 0x20 , 0x20 , IsSpace | IsBreaker , " " },
  { 0x21 , 0x21 , 0x21 , IsPunctuation , "\x21" }, /* ! */
  { 0x22 , 0x22 , 0x22 , IsPunctuation , "\x22" }, /* " */
  { 0x23 , 0x23 , 0x23 , None , "\x23" }, /* # */
  { 0x24 , 0x24 , 0x24 , None , "\x24" }, /* $ */
  { 0x25 , 0x25 , 0x25 , None , "\x25" }, /* % */
  { 0x26 , 0x26 , 0x26 , None , "\x26" }, /* & */
  { 0x27 , 0x27 , 0x27 , IsPunctuation , "\x27" }, /* APO */
  { 0x28 , 0x28 , 0x28 , IsPunctuation , "\x28" }, /* ( */
  { 0x29 , 0x29 , 0x29 , IsPunctuation , "\x29" }, /* ) */
  { 0x2a , 0x2a , 0x2a , None , "\x2a" }, /* * */
  { 0x2b , 0x2b , 0x2b , None , "\x2b" }, /* + */
  { 0x2c , 0x2c , 0x2c , IsPunctuation , "\x2c" }, /* COMMA */
  { 0x2d , 0x2d , 0x2d , IsPunctuation , "\x2d" }, /* - */
  { 0x2e , 0x2e , 0x2e , IsPunctuation , "\x2e" }, /* . */
  { 0x2f , 0x2f , 0x2f , None , "\x2f" }, /* / */
  { 0x30 , 0x30 , 0x30 , None , "\x30" }, /* 0 */
  { 0x31 , 0x31 , 0x31 , None , "\x31" }, /* 1 */
  { 0x32 , 0x32 , 0x32 , None , "\x32" }, /* 2 */
  { 0x33 , 0x33 , 0x33 , None , "\x33" }, /* 3 */
  { 0x34 , 0x34 , 0x34 , None , "\x34" }, /* 4 */
  { 0x35 , 0x35 , 0x35 , None , "\x35" }, /* 5 */
  { 0x36 , 0x36 , 0x36 , None , "\x36" }, /* 6 */
  { 0x37 , 0x37 , 0x37 , None , "\x37" }, /* 7 */
  { 0x38 , 0x38 , 0x38 , None , "\x38" }, /* 8 */
  { 0x39 , 0x39 , 0x39 , None , "\x39" }, /* 9 */
  { 0x3a , 0x3a , 0x3a , IsPunctuation , "\x3a" }, /* : */
  { 0x3b , 0x3b , 0x3b , IsPunctuation , "\x3b" }, /* SEMICOLON */
  { 0x3c , 0x3c , 0x3c , IsPunctuation , "\x3c" }, /* < */
  { 0x3d , 0x3d , 0x3d , None , "\x3d" }, /* = */
  { 0x3e , 0x3e , 0x3e , IsPunctuation , "\x3e" }, /* > */
  { 0x3f , 0x3f , 0x3f , IsPunctuation , "\x3f" }, /* ? */
  { 0x40 , 0x40 , 0x40 , None , "\x40" }, /* @ */
  { 0x41 , 0x41 , 0x61 , None , "\x41" }, /* A : up=A : lo=a */
  { 0x42 , 0x42 , 0x62 , None , "\x42" }, /* B : up=B : lo=b */
  { 0x43 , 0x43 , 0x63 , None , "\x43" }, /* C : up=C : lo=c */
  { 0x44 , 0x44 , 0x64 , None , "\x44" }, /* D : up=D : lo=d */
  { 0x45 , 0x45 , 0x65 , None , "\x45" }, /* E : up=E : lo=e */
  { 0x46 , 0x46 , 0x66 , None , "\x46" }, /* F : up=F : lo=f */
  { 0x47 , 0x47 , 0x67 , None , "\x47" }, /* G : up=G : lo=g */
  { 0x48 , 0x48 , 0x68 , None , "\x48" }, /* H : up=H : lo=h */
  { 0x49 , 0x49 , 0x69 , None , "\x49" }, /* I : up=I : lo=i */
  { 0x4a , 0x4a , 0x6a , None , "\x4a" }, /* J : up=J : lo=j */
  { 0x4b , 0x4b , 0x6b , None , "\x4b" }, /* K : up=K : lo=k */
  { 0x4c , 0x4c , 0x6c , None , "\x4c" }, /* L : up=L : lo=l */
  { 0x4d , 0x4d , 0x6d , None , "\x4d" }, /* M : up=M : lo=m */
  { 0x4e , 0x4e , 0x6e , None , "\x4e" }, /* N : up=N : lo=n */
  { 0x4f , 0x4f , 0x6f , None , "\x4f" }, /* O : up=O : lo=o */
  { 0x50 , 0x50 , 0x70 , None , "\x50" }, /* P : up=P : lo=p */
  { 0x51 , 0x51 , 0x71 , None , "\x51" }, /* Q : up=Q : lo=q */
  { 0x52 , 0x52 , 0x72 , None , "\x52" }, /* R : up=R : lo=r */
  { 0x53 , 0x53 , 0x73 , None , "\x53" }, /* S : up=S : lo=s */
  { 0x54 , 0x54 , 0x74 , None , "\x54" }, /* T : up=T : lo=t */
  { 0x55 , 0x55 , 0x75 , None , "\x55" }, /* U : up=U : lo=u */
  { 0x56 , 0x56 , 0x76 , None , "\x56" }, /* V : up=V : lo=v */
  { 0x57 , 0x57 , 0x77 , None , "\x57" }, /* W : up=W : lo=w */
  { 0x58 , 0x58 , 0x78 , None , "\x58" }, /* X : up=X : lo=x */
  { 0x59 , 0x59 , 0x79 , None , "\x59" }, /* Y : up=Y : lo=y */
  { 0x5a , 0x5a , 0x7a , None , "\x5a" }, /* Z : up=Z : lo=z */
  { 0x5b , 0x5b , 0x5b , IsPunctuation , "\x5b" }, /* [ */
  { 0x5c , 0x5c , 0x5c , None , "\x5c" }, /* BACKSLASH */
  { 0x5d , 0x5d , 0x5d , IsPunctuation , "\x5d" }, /* ] */
  { 0x5e , 0x5e , 0x5e , IsDiacritic , "\x5e" }, /* ^ */
  { 0x5f , 0x5f , 0x5f , None , "\x5f" }, /* _ */
  { 0x60 , 0x60 , 0x60 , IsDiacritic , "\x60" }, /* ` */
  { 0x61 , 0x41 , 0x61 , None , "\x61" }, /* a : up=A : lo=a */
  { 0x62 , 0x42 , 0x62 , None , "\x62" }, /* b : up=B : lo=b */
  { 0x63 , 0x43 , 0x63 , None , "\x63" }, /* c : up=C : lo=c */
  { 0x64 , 0x44 , 0x64 , None , "\x64" }, /* d : up=D : lo=d */
  { 0x65 , 0x45 , 0x65 , None , "\x65" }, /* e : up=E : lo=e */
  { 0x66 , 0x46 , 0x66 , None , "\x66" }, /* f : up=F : lo=f */
  { 0x67 , 0x47 , 0x67 , None , "\x67" }, /* g : up=G : lo=g */
  { 0x68 , 0x48 , 0x68 , None , "\x68" }, /* h : up=H : lo=h */
  { 0x69 , 0x49 , 0x69 , None , "\x69" }, /* i : up=I : lo=i */
  { 0x6a , 0x4a , 0x6a , None , "\x6a" }, /* j : up=J : lo=j */
  { 0x6b , 0x4b , 0x6b , None , "\x6b" }, /* k : up=K : lo=k */
  { 0x6c , 0x4c , 0x6c , None , "\x6c" }, /* l : up=L : lo=l */
  { 0x6d , 0x4d , 0x6d , None , "\x6d" }, /* m : up=M : lo=m */
  { 0x6e , 0x4e , 0x6e , None , "\x6e" }, /* n : up=N : lo=n */
  { 0x6f , 0x4f , 0x6f , None , "\x6f" }, /* o : up=O : lo=o */
  { 0x70 , 0x50 , 0x70 , None , "\x70" }, /* p : up=P : lo=p */
  { 0x71 , 0x51 , 0x71 , None , "\x71" }, /* q : up=Q : lo=q */
  { 0x72 , 0x52 , 0x72 , None , "\x72" }, /* r : up=R : lo=r */
  { 0x73 , 0x53 , 0x73 , None , "\x73" }, /* s : up=S : lo=s */
  { 0x74 , 0x54 , 0x74 , None , "\x74" }, /* t : up=T : lo=t */
  { 0x75 , 0x55 , 0x75 , None , "\x75" }, /* u : up=U : lo=u */
  { 0x76 , 0x56 , 0x76 , None , "\x76" }, /* v : up=V : lo=v */
  { 0x77 , 0x57 , 0x77 , None , "\x77" }, /* w : up=W : lo=w */
  { 0x78 , 0x58 , 0x78 , None , "\x78" }, /* x : up=X : lo=x */
  { 0x79 , 0x59 , 0x79 , None , "\x79" }, /* y : up=Y : lo=y */
  { 0x7a , 0x5a , 0x7a , None , "\x7a" }, /* z : up=Z : lo=z */
  { 0x7b , 0x7b , 0x7b , IsPunctuation , "\x7b" }, /* { */
  { 0x7c , 0x7c , 0x7c , IsBreaker , "\x7c" }, /* | */
  { 0x7d , 0x7d , 0x7d , IsPunctuation , "\x7d" }, /* } */
  { 0x7e , 0x7e , 0x7e , IsPunctuation , "\x7e" }, /* ~ */
  { 0x7f , 0x7f , 0x7f , IsControl , "" },
};

const character charmap_c2[64] = {
  { 0xc280 , 0xc280 , 0xc280 , IsControl , "" },
  { 0xc281 , 0xc281 , 0xc281 , IsControl , "" },
  { 0xc282 , 0xc282 , 0xc282 , IsControl , "" },
  { 0xc283 , 0xc283 , 0xc283 , IsControl , "" },
  { 0xc284 , 0xc284 , 0xc284 , IsControl , "" },
  { 0xc285 , 0xc285 , 0xc285 , IsControl , "" },
  { 0xc286 , 0xc286 , 0xc286 , IsControl , "" },
  { 0xc287 , 0xc287 , 0xc287 , IsControl , "" },
  { 0xc288 , 0xc288 , 0xc288 , IsControl , "" },
  { 0xc289 , 0xc289 , 0xc289 , IsControl , "" },
  { 0xc28a , 0xc28a , 0xc28a , IsControl , "" },
  { 0xc28b , 0xc28b , 0xc28b , IsControl , "" },
  { 0xc28c , 0xc28c , 0xc28c , IsControl , "" },
  { 0xc28d , 0xc28d , 0xc28d , IsControl , "" },
  { 0xc28e , 0xc28e , 0xc28e , IsControl , "" },
  { 0xc28f , 0xc28f , 0xc28f , IsControl , "" },
  { 0xc290 , 0xc290 , 0xc290 , IsControl , "" },
  { 0xc291 , 0xc291 , 0xc291 , IsControl , "" },
  { 0xc292 , 0xc292 , 0xc292 , IsControl , "" },
  { 0xc293 , 0xc293 , 0xc293 , IsControl , "" },
  { 0xc294 , 0xc294 , 0xc294 , IsControl , "" },
  { 0xc295 , 0xc295 , 0xc295 , IsControl , "" },
  { 0xc296 , 0xc296 , 0xc296 , IsControl , "" },
  { 0xc297 , 0xc297 , 0xc297 , IsControl , "" },
  { 0xc298 , 0xc298 , 0xc298 , IsControl , "" },
  { 0xc299 , 0xc299 , 0xc299 , IsControl , "" },
  { 0xc29a , 0xc29a , 0xc29a , IsControl , "" },
  { 0xc29b , 0xc29b , 0xc29b , IsControl , "" },
  { 0xc29c , 0xc29c , 0xc29c , IsControl , "" },
  { 0xc29d , 0xc29d , 0xc29d , IsControl , "" },
  { 0xc29e , 0xc29e , 0xc29e , IsControl , "" },
  { 0xc29f , 0xc29f , 0xc29f , IsControl , "" },
  { 0xc2a0 , 0xc2a0 , 0xc2a0 , IsSpace , " " },
  { 0xc2a1 , 0xc2a1 , 0xc2a1 , None , "!" }, /* ?? : up=?? : lo=?? */
  { 0xc2a2 , 0xc2a2 , 0xc2a2 , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc2a3 , 0xc2a3 , 0xc2a3 , None , "GBP" }, /* ?? : up=?? : lo=?? */
  { 0xc2a4 , 0xc2a4 , 0xc2a4 , None , "" }, /* ?? : up=?? : lo=?? */
  { 0xc2a5 , 0xc2a5 , 0xc2a5 , None , "JPY" }, /* ?? : up=?? : lo=?? */
  { 0xc2a6 , 0xc2a6 , 0xc2a6 , IsBreaker , "|" }, /* ?? : up=?? : lo=?? */
  { 0xc2a7 , 0xc2a7 , 0xc2a7 , None , "" }, /* ?? : up=?? : lo=?? */
  { 0xc2a8 , 0xc2a8 , 0xc2a8 , None , "" }, /* ?? : up=?? : lo=?? */
  { 0xc2a9 , 0xc2a9 , 0xc2a9 , None , "(C)" }, /* ?? : up=?? : lo=?? */
  { 0xc2aa , 0xc2aa , 0xc2aa , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc2ab , 0xc2ab , 0xc2ab , IsPunctuation , "\"" }, /* ?? : up=?? : lo=?? */
  { 0xc2ac , 0xc2ac , 0xc2ac , None , "!" }, /* ?? : up=?? : lo=?? */
  { 0xc2ad , 0xc2ad , 0xc2ad , IsControl , "" }, /* Conditional hyphen (SHY) */
  { 0xc2ae , 0xc2ae , 0xc2ae , None , "(R)" }, /* ?? : up=?? : lo=?? */
  { 0xc2af , 0xc2af , 0xc2af , None , "" }, /* ?? : up=?? : lo=?? */
  { 0xc2b0 , 0xc2b0 , 0xc2b0 , None , "" }, /* ?? : up=?? : lo=?? */
  { 0xc2b1 , 0xc2b1 , 0xc2b1 , None , "+-" }, /* ?? : up=?? : lo=?? */
  { 0xc2b2 , 0xc2b2 , 0xc2b2 , None , "2" }, /* ?? : up=?? : lo=?? */
  { 0xc2b3 , 0xc2b3 , 0xc2b3 , None , "3" }, /* ?? : up=?? : lo=?? */
  { 0xc2b4 , 0xc2b4 , 0xc2b4 , IsDiacritic , "\x27" }, /* ?? : up=?? : lo=?? */
  { 0xc2b5 , 0xce9c , 0xc2b5 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc2b6 , 0xc2b6 , 0xc2b6 , None , "" }, /* ?? : up=?? : lo=?? */
  { 0xc2b7 , 0xc2b7 , 0xc2b7 , IsPunctuation , "." }, /* ?? : up=?? : lo=?? */
  { 0xc2b8 , 0xc2b8 , 0xc2b8 , IsPunctuation , "\x2c" }, /* ?? : up=?? : lo=?? */
  { 0xc2b9 , 0xc2b9 , 0xc2b9 , None , "1" }, /* ?? : up=?? : lo=?? */
  { 0xc2ba , 0xc2ba , 0xc2ba , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc2bb , 0xc2bb , 0xc2bb , IsPunctuation , "\"" }, /* ?? : up=?? : lo=?? */
  { 0xc2bc , 0xc2bc , 0xc2bc , None , "1/4" }, /* ?? : up=?? : lo=?? */
  { 0xc2bd , 0xc2bd , 0xc2bd , None , "1/2" }, /* ?? : up=?? : lo=?? */
  { 0xc2be , 0xc2be , 0xc2be , None , "3/4" }, /* ?? : up=?? : lo=?? */
  { 0xc2bf , 0xc2bf , 0xc2bf , IsPunctuation , "?" }, /* ?? : up=?? : lo=?? */
};

const character charmap_c3[64] = {
  { 0xc380 , 0xc380 , 0xc3a0 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc381 , 0xc381 , 0xc3a1 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc382 , 0xc382 , 0xc3a2 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc383 , 0xc383 , 0xc3a3 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc384 , 0xc384 , 0xc3a4 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc385 , 0xc385 , 0xc3a5 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc386 , 0xc386 , 0xc3a6 , None , "AE" }, /* ?? : up=?? : lo=?? */
  { 0xc387 , 0xc387 , 0xc3a7 , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc388 , 0xc388 , 0xc3a8 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc389 , 0xc389 , 0xc3a9 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc38a , 0xc38a , 0xc3aa , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc38b , 0xc38b , 0xc3ab , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc38c , 0xc38c , 0xc3ac , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc38d , 0xc38d , 0xc3ad , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc38e , 0xc38e , 0xc3ae , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc38f , 0xc38f , 0xc3af , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc390 , 0xc390 , 0xc3b0 , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc391 , 0xc391 , 0xc3b1 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc392 , 0xc392 , 0xc3b2 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc393 , 0xc393 , 0xc3b3 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc394 , 0xc394 , 0xc3b4 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc395 , 0xc395 , 0xc3b5 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc396 , 0xc396 , 0xc3b6 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc397 , 0xc397 , 0xc397 , None , "x" }, /* ?? : up=?? : lo=?? */
  { 0xc398 , 0xc398 , 0xc3b8 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc399 , 0xc399 , 0xc3b9 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc39a , 0xc39a , 0xc3ba , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc39b , 0xc39b , 0xc3bb , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc39c , 0xc39c , 0xc3bc , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc39d , 0xc39d , 0xc3bd , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xc39e , 0xc39e , 0xc3be , None , "TH" }, /* ?? : up=?? : lo=?? */
  { 0xc39f , 0xc39f , 0xc39f , None , "ss" }, /* ?? : up=?? : lo=?? */
  { 0xc3a0 , 0xc380 , 0xc3a0 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc3a1 , 0xc381 , 0xc3a1 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc3a2 , 0xc382 , 0xc3a2 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc3a3 , 0xc383 , 0xc3a3 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc3a4 , 0xc384 , 0xc3a4 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc3a5 , 0xc385 , 0xc3a5 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc3a6 , 0xc386 , 0xc3a6 , None , "ae" }, /* ?? : up=?? : lo=?? */
  { 0xc3a7 , 0xc387 , 0xc3a7 , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc3a8 , 0xc388 , 0xc3a8 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc3a9 , 0xc389 , 0xc3a9 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc3aa , 0xc38a , 0xc3aa , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc3ab , 0xc38b , 0xc3ab , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc3ac , 0xc38c , 0xc3ac , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc3ad , 0xc38d , 0xc3ad , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc3ae , 0xc38e , 0xc3ae , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc3af , 0xc38f , 0xc3af , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc3b0 , 0xc390 , 0xc3b0 , None , "d" }, /* ?? : up=?? : lo=?? */
  { 0xc3b1 , 0xc391 , 0xc3b1 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc3b2 , 0xc392 , 0xc3b2 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc3b3 , 0xc393 , 0xc3b3 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc3b4 , 0xc394 , 0xc3b4 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc3b5 , 0xc395 , 0xc3b5 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc3b6 , 0xc396 , 0xc3b6 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc3b7 , 0xc3b7 , 0xc3b7 , None , "\xc3\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xc3b8 , 0xc398 , 0xc3b8 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc3b9 , 0xc399 , 0xc3b9 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc3ba , 0xc39a , 0xc3ba , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc3bb , 0xc39b , 0xc3bb , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc3bc , 0xc39c , 0xc3bc , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc3bd , 0xc39d , 0xc3bd , None , "y" }, /* ?? : up=?? : lo=?? */
  { 0xc3be , 0xc39e , 0xc3be , None , "th" }, /* ?? : up=?? : lo=?? */
  { 0xc3bf , 0xc5b8 , 0xc3bf , None , "y" }, /* ?? : up=?? : lo=?? */
};

const character charmap_c4[64] = {
  { 0xc480 , 0xc480 , 0xc481 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc481 , 0xc480 , 0xc481 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc482 , 0xc482 , 0xc483 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc483 , 0xc482 , 0xc483 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc484 , 0xc484 , 0xc485 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc485 , 0xc484 , 0xc485 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc486 , 0xc486 , 0xc487 , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc487 , 0xc486 , 0xc487 , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc488 , 0xc488 , 0xc489 , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc489 , 0xc488 , 0xc489 , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc48a , 0xc48a , 0xc48b , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc48b , 0xc48a , 0xc48b , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc48c , 0xc48c , 0xc48d , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc48d , 0xc48c , 0xc48d , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc48e , 0xc48e , 0xc48f , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc48f , 0xc48e , 0xc48f , None , "d" }, /* ?? : up=?? : lo=?? */
  { 0xc490 , 0xc490 , 0xc491 , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc491 , 0xc490 , 0xc491 , None , "d" }, /* ?? : up=?? : lo=?? */
  { 0xc492 , 0xc492 , 0xc493 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc493 , 0xc492 , 0xc493 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc494 , 0xc494 , 0xc495 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc495 , 0xc494 , 0xc495 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc496 , 0xc496 , 0xc497 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc497 , 0xc496 , 0xc497 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc498 , 0xc498 , 0xc499 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc499 , 0xc498 , 0xc499 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc49a , 0xc49a , 0xc49b , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc49b , 0xc49a , 0xc49b , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc49c , 0xc49c , 0xc49d , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc49d , 0xc49c , 0xc49d , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc49e , 0xc49e , 0xc49f , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc49f , 0xc49e , 0xc49f , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc4a0 , 0xc4a0 , 0xc4a1 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc4a1 , 0xc4a0 , 0xc4a1 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc4a2 , 0xc4a2 , 0xc4a3 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc4a3 , 0xc4a2 , 0xc4a3 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc4a4 , 0xc4a4 , 0xc4a5 , None , "H" }, /* ?? : up=?? : lo=?? */
  { 0xc4a5 , 0xc4a4 , 0xc4a5 , None , "h" }, /* ?? : up=?? : lo=?? */
  { 0xc4a6 , 0xc4a6 , 0xc4a7 , None , "H" }, /* ?? : up=?? : lo=?? */
  { 0xc4a7 , 0xc4a6 , 0xc4a7 , None , "h" }, /* ?? : up=?? : lo=?? */
  { 0xc4a8 , 0xc4a8 , 0xc4a9 , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc4a9 , 0xc4a8 , 0xc4a9 , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc4aa , 0xc4aa , 0xc4ab , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc4ab , 0xc4aa , 0xc4ab , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc4ac , 0xc4ac , 0xc4ad , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc4ad , 0xc4ac , 0xc4ad , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc4ae , 0xc4ae , 0xc4af , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc4af , 0xc4ae , 0xc4af , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc4b0 , 0xc4b0 , 0x69 , None , "I" }, /* ?? : up=?? : lo=i */
  { 0xc4b1 , 0x49 , 0xc4b1 , None , "i" }, /* ?? : up=I : lo=?? */
  { 0xc4b2 , 0xc4b2 , 0xc4b3 , None , "IJ" }, /* ?? : up=?? : lo=?? */
  { 0xc4b3 , 0xc4b2 , 0xc4b3 , None , "ij" }, /* ?? : up=?? : lo=?? */
  { 0xc4b4 , 0xc4b4 , 0xc4b5 , None , "J" }, /* ?? : up=?? : lo=?? */
  { 0xc4b5 , 0xc4b4 , 0xc4b5 , None , "j" }, /* ?? : up=?? : lo=?? */
  { 0xc4b6 , 0xc4b6 , 0xc4b7 , None , "K" }, /* ?? : up=?? : lo=?? */
  { 0xc4b7 , 0xc4b6 , 0xc4b7 , None , "k" }, /* ?? : up=?? : lo=?? */
  { 0xc4b8 , 0xc4b8 , 0xc4b8 , None , "q" }, /* ?? : up=?? : lo=?? */
  { 0xc4b9 , 0xc4b9 , 0xc4ba , None , "L" }, /* ?? : up=?? : lo=?? */
  { 0xc4ba , 0xc4b9 , 0xc4ba , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc4bb , 0xc4bb , 0xc4bc , None , "L" }, /* ?? : up=?? : lo=?? */
  { 0xc4bc , 0xc4bb , 0xc4bc , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc4bd , 0xc4bd , 0xc4be , None , "L" }, /* ?? : up=?? : lo=?? */
  { 0xc4be , 0xc4bd , 0xc4be , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc4bf , 0xc4bf , 0xc580 , None , "L" }, /* ?? : up=?? : lo=?? */
};

const character charmap_c5[64] = {
  { 0xc580 , 0xc4bf , 0xc580 , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc581 , 0xc581 , 0xc582 , None , "L" }, /* ?? : up=?? : lo=?? */
  { 0xc582 , 0xc581 , 0xc582 , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc583 , 0xc583 , 0xc584 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc584 , 0xc583 , 0xc584 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc585 , 0xc585 , 0xc586 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc586 , 0xc585 , 0xc586 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc587 , 0xc587 , 0xc588 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc588 , 0xc587 , 0xc588 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc589 , 0xc589 , 0xc589 , None , "\x27\x6e" }, /* ?? : up=?? : lo=?? */
  { 0xc58a , 0xc58a , 0xc58b , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc58b , 0xc58a , 0xc58b , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc58c , 0xc58c , 0xc58d , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc58d , 0xc58c , 0xc58d , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc58e , 0xc58e , 0xc58f , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc58f , 0xc58e , 0xc58f , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc590 , 0xc590 , 0xc591 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc591 , 0xc590 , 0xc591 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc592 , 0xc592 , 0xc593 , None , "OE" }, /* ?? : up=?? : lo=?? */
  { 0xc593 , 0xc592 , 0xc593 , None , "oe" }, /* ?? : up=?? : lo=?? */
  { 0xc594 , 0xc594 , 0xc595 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc595 , 0xc594 , 0xc595 , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc596 , 0xc596 , 0xc597 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc597 , 0xc596 , 0xc597 , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc598 , 0xc598 , 0xc599 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc599 , 0xc598 , 0xc599 , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc59a , 0xc59a , 0xc59b , None , "S" }, /* ?? : up=?? : lo=?? */
  { 0xc59b , 0xc59a , 0xc59b , None , "s" }, /* ?? : up=?? : lo=?? */
  { 0xc59c , 0xc59c , 0xc59d , None , "S" }, /* ?? : up=?? : lo=?? */
  { 0xc59d , 0xc59c , 0xc59d , None , "s" }, /* ?? : up=?? : lo=?? */
  { 0xc59e , 0xc59e , 0xc59f , None , "S" }, /* ?? : up=?? : lo=?? */
  { 0xc59f , 0xc59e , 0xc59f , None , "s" }, /* ?? : up=?? : lo=?? */
  { 0xc5a0 , 0xc5a0 , 0xc5a1 , None , "S" }, /* ?? : up=?? : lo=?? */
  { 0xc5a1 , 0xc5a0 , 0xc5a1 , None , "s" }, /* ?? : up=?? : lo=?? */
  { 0xc5a2 , 0xc5a2 , 0xc5a3 , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc5a3 , 0xc5a2 , 0xc5a3 , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc5a4 , 0xc5a4 , 0xc5a5 , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc5a5 , 0xc5a4 , 0xc5a5 , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc5a6 , 0xc5a6 , 0xc5a7 , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc5a7 , 0xc5a6 , 0xc5a7 , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc5a8 , 0xc5a8 , 0xc5a9 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc5a9 , 0xc5a8 , 0xc5a9 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc5aa , 0xc5aa , 0xc5ab , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc5ab , 0xc5aa , 0xc5ab , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc5ac , 0xc5ac , 0xc5ad , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc5ad , 0xc5ac , 0xc5ad , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc5ae , 0xc5ae , 0xc5af , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc5af , 0xc5ae , 0xc5af , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc5b0 , 0xc5b0 , 0xc5b1 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc5b1 , 0xc5b0 , 0xc5b1 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc5b2 , 0xc5b2 , 0xc5b3 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc5b3 , 0xc5b2 , 0xc5b3 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc5b4 , 0xc5b4 , 0xc5b5 , None , "W" }, /* ?? : up=?? : lo=?? */
  { 0xc5b5 , 0xc5b4 , 0xc5b5 , None , "w" }, /* ?? : up=?? : lo=?? */
  { 0xc5b6 , 0xc5b6 , 0xc5b7 , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xc5b7 , 0xc5b6 , 0xc5b7 , None , "y" }, /* ?? : up=?? : lo=?? */
  { 0xc5b8 , 0xc5b8 , 0xc3bf , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xc5b9 , 0xc5b9 , 0xc5ba , None , "Z" }, /* ?? : up=?? : lo=?? */
  { 0xc5ba , 0xc5b9 , 0xc5ba , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xc5bb , 0xc5bb , 0xc5bc , None , "Z" }, /* ?? : up=?? : lo=?? */
  { 0xc5bc , 0xc5bb , 0xc5bc , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xc5bd , 0xc5bd , 0xc5be , None , "Z" }, /* ?? : up=?? : lo=?? */
  { 0xc5be , 0xc5bd , 0xc5be , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xc5bf , 0x53 , 0xc5bf , None , "s" }, /* ?? : up=S : lo=?? */
};

const character charmap_c6[64] = {
  { 0xc680 , 0xc983 , 0xc680 , None , "b" }, /* ?? : up=?? : lo=?? */
  { 0xc681 , 0xc681 , 0xc993 , None , "B" }, /* ?? : up=?? : lo=?? */
  { 0xc682 , 0xc682 , 0xc683 , None , "B" }, /* ?? : up=?? : lo=?? */
  { 0xc683 , 0xc682 , 0xc683 , None , "b" }, /* ?? : up=?? : lo=?? */
  { 0xc684 , 0xc684 , 0xc685 , None , "b" }, /* ?? : up=?? : lo=?? */
  { 0xc685 , 0xc684 , 0xc685 , None , "B" }, /* ?? : up=?? : lo=?? */
  { 0xc686 , 0xc686 , 0xc994 , None , "\xc6\x86" }, /* ?? : up=?? : lo=?? */
  { 0xc687 , 0xc687 , 0xc688 , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc688 , 0xc687 , 0xc688 , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc689 , 0xc689 , 0xc996 , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc68a , 0xc68a , 0xc997 , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc68b , 0xc68b , 0xc68c , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc68c , 0xc68b , 0xc68c , None , "d" }, /* ?? : up=?? : lo=?? */
  { 0xc68d , 0xc68d , 0xc68d , None , "\xc6\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xc68e , 0xc68e , 0xc79d , None , "\xc6\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xc68f , 0xc68f , 0xc999 , None , "\xc6\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xc690 , 0xc690 , 0xc99b , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc691 , 0xc691 , 0xc692 , None , "F" }, /* ?? : up=?? : lo=?? */
  { 0xc692 , 0xc691 , 0xc692 , None , "f" }, /* ?? : up=?? : lo=?? */
  { 0xc693 , 0xc693 , 0xc9a0 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc694 , 0xc694 , 0xc9a3 , None , "\xc6\x94" }, /* ?? : up=?? : lo=?? */
  { 0xc695 , 0xc7b6 , 0xc695 , None , "hv" }, /* ?? : up=?? : lo=?? */
  { 0xc696 , 0xc696 , 0xc9a9 , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc697 , 0xc697 , 0xc9a8 , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc698 , 0xc698 , 0xc699 , None , "K" }, /* ?? : up=?? : lo=?? */
  { 0xc699 , 0xc698 , 0xc699 , None , "k" }, /* ?? : up=?? : lo=?? */
  { 0xc69a , 0xc8bd , 0xc69a , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc69b , 0xc69b , 0xc69b , None , "\xc6\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xc69c , 0xc69c , 0xc9af , None , "\xc6\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xc69d , 0xc69d , 0xc9b2 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc69e , 0xc8a0 , 0xc69e , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc69f , 0xc69f , 0xc9b5 , None , "\xc6\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xc6a0 , 0xc6a0 , 0xc6a1 , None , "\xc6\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xc6a1 , 0xc6a0 , 0xc6a1 , None , "\xc6\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xc6a2 , 0xc6a2 , 0xc6a3 , None , "OI" }, /* ?? : up=?? : lo=?? */
  { 0xc6a3 , 0xc6a2 , 0xc6a3 , None , "oi" }, /* ?? : up=?? : lo=?? */
  { 0xc6a4 , 0xc6a4 , 0xc6a5 , None , "P" }, /* ?? : up=?? : lo=?? */
  { 0xc6a5 , 0xc6a4 , 0xc6a5 , None , "p" }, /* ?? : up=?? : lo=?? */
  { 0xc6a6 , 0xc6a6 , 0xca80 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc6a7 , 0xc6a7 , 0xc6a8 , None , "\xc6\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xc6a8 , 0xc6a7 , 0xc6a8 , None , "\xc6\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xc6a9 , 0xc6a9 , 0xca83 , None , "\xc6\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xc6aa , 0xc6aa , 0xc6aa , None , "\xc6\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xc6ab , 0xc6ab , 0xc6ab , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc6ac , 0xc6ac , 0xc6ad , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc6ad , 0xc6ac , 0xc6ad , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc6ae , 0xc6ae , 0xca88 , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc6af , 0xc6af , 0xc6b0 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc6b0 , 0xc6af , 0xc6b0 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc6b1 , 0xc6b1 , 0xca8a , None , "\xc6\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xc6b2 , 0xc6b2 , 0xca8b , None , "V" }, /* ?? : up=?? : lo=?? */
  { 0xc6b3 , 0xc6b3 , 0xc6b4 , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xc6b4 , 0xc6b3 , 0xc6b4 , None , "y" }, /* ?? : up=?? : lo=?? */
  { 0xc6b5 , 0xc6b5 , 0xc6b6 , None , "Z" }, /* ?? : up=?? : lo=?? */
  { 0xc6b6 , 0xc6b5 , 0xc6b6 , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xc6b7 , 0xc6b7 , 0xca92 , None , "\xc6\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xc6b8 , 0xc6b8 , 0xc6b9 , None , "\xc6\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xc6b9 , 0xc6b8 , 0xc6b9 , None , "\xc6\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xc6ba , 0xc6ba , 0xc6ba , None , "\xc6\xba" }, /* ?? : up=?? : lo=?? */
  { 0xc6bb , 0xc6bb , 0xc6bb , None , "\xc6\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xc6bc , 0xc6bc , 0xc6bd , None , "\xc6\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xc6bd , 0xc6bc , 0xc6bd , None , "\xc6\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xc6be , 0xc6be , 0xc6be , None , "\xc6\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xc6bf , 0xc7b7 , 0xc6bf , None , "\xc6\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_c7[64] = {
  { 0xc780 , 0xc780 , 0xc780 , None , "\xc7\x80" }, /* ?? : up=?? : lo=?? */
  { 0xc781 , 0xc781 , 0xc781 , None , "\xc7\x81" }, /* ?? : up=?? : lo=?? */
  { 0xc782 , 0xc782 , 0xc782 , None , "\xc7\x82" }, /* ?? : up=?? : lo=?? */
  { 0xc783 , 0xc783 , 0xc783 , None , "\xc7\x83" }, /* ?? : up=?? : lo=?? */
  { 0xc784 , 0xc784 , 0xc786 , None , "\xc7\x84" }, /* ?? : up=?? : lo=?? */
  { 0xc785 , 0xc784 , 0xc786 , None , "\xc7\x85" }, /* ?? : up=?? : lo=?? */
  { 0xc786 , 0xc784 , 0xc786 , None , "\xc7\x86" }, /* ?? : up=?? : lo=?? */
  { 0xc787 , 0xc787 , 0xc789 , None , "LJ" }, /* ?? : up=?? : lo=?? */
  { 0xc788 , 0xc787 , 0xc789 , None , "Lj" }, /* ?? : up=?? : lo=?? */
  { 0xc789 , 0xc787 , 0xc789 , None , "lj" }, /* ?? : up=?? : lo=?? */
  { 0xc78a , 0xc78a , 0xc78c , None , "NJ" }, /* ?? : up=?? : lo=?? */
  { 0xc78b , 0xc78a , 0xc78c , None , "Nj" }, /* ?? : up=?? : lo=?? */
  { 0xc78c , 0xc78a , 0xc78c , None , "nj" }, /* ?? : up=?? : lo=?? */
  { 0xc78d , 0xc78d , 0xc78e , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc78e , 0xc78d , 0xc78e , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc78f , 0xc78f , 0xc790 , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc790 , 0xc78f , 0xc790 , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc791 , 0xc791 , 0xc792 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc792 , 0xc791 , 0xc792 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc793 , 0xc793 , 0xc794 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc794 , 0xc793 , 0xc794 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc795 , 0xc795 , 0xc796 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc796 , 0xc795 , 0xc796 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc797 , 0xc797 , 0xc798 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc798 , 0xc797 , 0xc798 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc799 , 0xc799 , 0xc79a , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc79a , 0xc799 , 0xc79a , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc79b , 0xc79b , 0xc79c , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc79c , 0xc79b , 0xc79c , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc79d , 0xc68e , 0xc79d , None , "\xc7\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xc79e , 0xc79e , 0xc79f , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc79f , 0xc79e , 0xc79f , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc7a0 , 0xc7a0 , 0xc7a1 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc7a1 , 0xc7a0 , 0xc7a1 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc7a2 , 0xc7a2 , 0xc7a3 , None , "AE" }, /* ?? : up=?? : lo=?? */
  { 0xc7a3 , 0xc7a2 , 0xc7a3 , None , "ae" }, /* ?? : up=?? : lo=?? */
  { 0xc7a4 , 0xc7a4 , 0xc7a5 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc7a5 , 0xc7a4 , 0xc7a5 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc7a6 , 0xc7a6 , 0xc7a7 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc7a7 , 0xc7a6 , 0xc7a7 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc7a8 , 0xc7a8 , 0xc7a9 , None , "K" }, /* ?? : up=?? : lo=?? */
  { 0xc7a9 , 0xc7a8 , 0xc7a9 , None , "k" }, /* ?? : up=?? : lo=?? */
  { 0xc7aa , 0xc7aa , 0xc7ab , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc7ab , 0xc7aa , 0xc7ab , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc7ac , 0xc7ac , 0xc7ad , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc7ad , 0xc7ac , 0xc7ad , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc7ae , 0xc7ae , 0xc7af , None , "\xc7\xae" }, /* ?? : up=?? : lo=?? */
  { 0xc7af , 0xc7ae , 0xc7af , None , "\xc7\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xc7b0 , 0xc7b0 , 0xc7b0 , None , "J" }, /* ?? : up=?? : lo=?? */
  { 0xc7b1 , 0xc7b1 , 0xc7b3 , None , "DZ" }, /* ?? : up=?? : lo=?? */
  { 0xc7b2 , 0xc7b1 , 0xc7b3 , None , "Dz" }, /* ?? : up=?? : lo=?? */
  { 0xc7b3 , 0xc7b1 , 0xc7b3 , None , "dz" }, /* ?? : up=?? : lo=?? */
  { 0xc7b4 , 0xc7b4 , 0xc7b5 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc7b5 , 0xc7b4 , 0xc7b5 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc7b6 , 0xc7b6 , 0xc695 , None , "HV" }, /* ?? : up=?? : lo=?? */
  { 0xc7b7 , 0xc7b7 , 0xc6bf , None , "\xc7\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xc7b8 , 0xc7b8 , 0xc7b9 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc7b9 , 0xc7b8 , 0xc7b9 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc7ba , 0xc7ba , 0xc7bb , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc7bb , 0xc7ba , 0xc7bb , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc7bc , 0xc7bc , 0xc7bd , None , "AE" }, /* ?? : up=?? : lo=?? */
  { 0xc7bd , 0xc7bc , 0xc7bd , None , "ae" }, /* ?? : up=?? : lo=?? */
  { 0xc7be , 0xc7be , 0xc7bf , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc7bf , 0xc7be , 0xc7bf , None , "o" }, /* ?? : up=?? : lo=?? */
};

const character charmap_c8[64] = {
  { 0xc880 , 0xc880 , 0xc881 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc881 , 0xc880 , 0xc881 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc882 , 0xc882 , 0xc883 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc883 , 0xc882 , 0xc883 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc884 , 0xc884 , 0xc885 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc885 , 0xc884 , 0xc885 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc886 , 0xc886 , 0xc887 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc887 , 0xc886 , 0xc887 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc888 , 0xc888 , 0xc889 , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc889 , 0xc888 , 0xc889 , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc88a , 0xc88a , 0xc88b , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc88b , 0xc88a , 0xc88b , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc88c , 0xc88c , 0xc88d , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc88d , 0xc88c , 0xc88d , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc88e , 0xc88e , 0xc88f , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc88f , 0xc88e , 0xc88f , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc890 , 0xc890 , 0xc891 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc891 , 0xc890 , 0xc891 , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc892 , 0xc892 , 0xc893 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc893 , 0xc892 , 0xc893 , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc894 , 0xc894 , 0xc895 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc895 , 0xc894 , 0xc895 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc896 , 0xc896 , 0xc897 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc897 , 0xc896 , 0xc897 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xc898 , 0xc898 , 0xc899 , None , "S" }, /* ?? : up=?? : lo=?? */
  { 0xc899 , 0xc898 , 0xc899 , None , "s" }, /* ?? : up=?? : lo=?? */
  { 0xc89a , 0xc89a , 0xc89b , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc89b , 0xc89a , 0xc89b , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc89c , 0xc89c , 0xc89d , None , "\xc8\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xc89d , 0xc89c , 0xc89d , None , "\xc8\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xc89e , 0xc89e , 0xc89f , None , "H" }, /* ?? : up=?? : lo=?? */
  { 0xc89f , 0xc89e , 0xc89f , None , "h" }, /* ?? : up=?? : lo=?? */
  { 0xc8a0 , 0xc8a0 , 0xc69e , None , "\xc8\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xc8a1 , 0xc8a1 , 0xc8a1 , None , "d" }, /* ?? : up=?? : lo=?? */
  { 0xc8a2 , 0xc8a2 , 0xc8a3 , None , "\xc8\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xc8a3 , 0xc8a2 , 0xc8a3 , None , "\xc8\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xc8a4 , 0xc8a4 , 0xc8a5 , None , "Z" }, /* ?? : up=?? : lo=?? */
  { 0xc8a5 , 0xc8a4 , 0xc8a5 , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xc8a6 , 0xc8a6 , 0xc8a7 , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc8a7 , 0xc8a6 , 0xc8a7 , None , "a" }, /* ?? : up=?? : lo=?? */
  { 0xc8a8 , 0xc8a8 , 0xc8a9 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc8a9 , 0xc8a8 , 0xc8a9 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc8aa , 0xc8aa , 0xc8ab , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc8ab , 0xc8aa , 0xc8ab , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc8ac , 0xc8ac , 0xc8ad , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc8ad , 0xc8ac , 0xc8ad , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc8ae , 0xc8ae , 0xc8af , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc8af , 0xc8ae , 0xc8af , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc8b0 , 0xc8b0 , 0xc8b1 , None , "O" }, /* ?? : up=?? : lo=?? */
  { 0xc8b1 , 0xc8b0 , 0xc8b1 , None , "o" }, /* ?? : up=?? : lo=?? */
  { 0xc8b2 , 0xc8b2 , 0xc8b3 , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xc8b3 , 0xc8b2 , 0xc8b3 , None , "y" }, /* ?? : up=?? : lo=?? */
  { 0xc8b4 , 0xc8b4 , 0xc8b4 , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc8b5 , 0xc8b5 , 0xc8b5 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc8b6 , 0xc8b6 , 0xc8b6 , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xc8b7 , 0xc8b7 , 0xc8b7 , None , "j" }, /* ?? : up=?? : lo=?? */
  { 0xc8b8 , 0xc8b8 , 0xc8b8 , None , "db" }, /* ?? : up=?? : lo=?? */
  { 0xc8b9 , 0xc8b9 , 0xc8b9 , None , "qp" }, /* ?? : up=?? : lo=?? */
  { 0xc8ba , 0xc8ba , 0xc8ba , None , "A" }, /* ?? : up=?? : lo=?? */
  { 0xc8bb , 0xc8bb , 0xc8bc , None , "C" }, /* ?? : up=?? : lo=?? */
  { 0xc8bc , 0xc8bb , 0xc8bc , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc8bd , 0xc8bd , 0xc69a , None , "L" }, /* ?? : up=?? : lo=?? */
  { 0xc8be , 0xc8be , 0xc8be , None , "T" }, /* ?? : up=?? : lo=?? */
  { 0xc8bf , 0xc8bf , 0xc8bf , None , "s" }, /* ?? : up=?? : lo=?? */
};

const character charmap_c9[64] = {
  { 0xc980 , 0xc980 , 0xc980 , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xc981 , 0xc981 , 0xc982 , None , "\xc9\x81" }, /* ?? : up=?? : lo=?? */
  { 0xc982 , 0xc981 , 0xc982 , None , "\xc9\x82" }, /* ?? : up=?? : lo=?? */
  { 0xc983 , 0xc983 , 0xc680 , None , "B" }, /* ?? : up=?? : lo=?? */
  { 0xc984 , 0xc984 , 0xca89 , None , "U" }, /* ?? : up=?? : lo=?? */
  { 0xc985 , 0xc985 , 0xca8c , None , "\xc9\x85" }, /* ?? : up=?? : lo=?? */
  { 0xc986 , 0xc986 , 0xc987 , None , "E" }, /* ?? : up=?? : lo=?? */
  { 0xc987 , 0xc986 , 0xc987 , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc988 , 0xc988 , 0xc989 , None , "J" }, /* ?? : up=?? : lo=?? */
  { 0xc989 , 0xc988 , 0xc989 , None , "j" }, /* ?? : up=?? : lo=?? */
  { 0xc98a , 0xc98a , 0xc98b , None , "\xc9\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xc98b , 0xc98a , 0xc98b , None , "\xc9\x90" }, /* ?? : up=?? : lo=?? */
  { 0xc98c , 0xc98c , 0xc98d , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xc98d , 0xc98c , 0xc98d , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc98e , 0xc98e , 0xc98f , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xc98f , 0xc98e , 0xc98f , None , "y" }, /* ?? : up=?? : lo=?? */
  { 0xc990 , 0xc990 , 0xc990 , None , "\xc9\x90" }, /* ?? : up=?? : lo=?? */
  { 0xc991 , 0xc991 , 0xc991 , None , "\xc9\x91" }, /* ?? : up=?? : lo=?? */
  { 0xc992 , 0xc992 , 0xc992 , None , "\xc9\x92" }, /* ?? : up=?? : lo=?? */
  { 0xc993 , 0xc681 , 0xc993 , None , "b" }, /* ?? : up=?? : lo=?? */
  { 0xc994 , 0xc686 , 0xc994 , None , "\xc9\x94" }, /* ?? : up=?? : lo=?? */
  { 0xc995 , 0xc995 , 0xc995 , None , "c" }, /* ?? : up=?? : lo=?? */
  { 0xc996 , 0xc689 , 0xc996 , None , "d" }, /* ?? : up=?? : lo=?? */
  { 0xc997 , 0xc68a , 0xc997 , None , "D" }, /* ?? : up=?? : lo=?? */
  { 0xc998 , 0xc998 , 0xc998 , None , "\xc9\x98" }, /* ?? : up=?? : lo=?? */
  { 0xc999 , 0xc68f , 0xc999 , None , "\xc9\x99" }, /* ?? : up=?? : lo=?? */
  { 0xc99a , 0xc99a , 0xc99a , None , "\xc9\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xc99b , 0xc690 , 0xc99b , None , "e" }, /* ?? : up=?? : lo=?? */
  { 0xc99c , 0xc99c , 0xc99c , None , "\xc9\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xc99d , 0xc99d , 0xc99d , None , "\xc9\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xc99e , 0xc99e , 0xc99e , None , "\xc9\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xc99f , 0xc99f , 0xc99f , None , "j" }, /* ?? : up=?? : lo=?? */
  { 0xc9a0 , 0xc693 , 0xc9a0 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc9a1 , 0xc9a1 , 0xc9a1 , None , "g" }, /* ?? : up=?? : lo=?? */
  { 0xc9a2 , 0xc9a2 , 0xc9a2 , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xc9a3 , 0xc694 , 0xc9a3 , None , "\xc9\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xc9a4 , 0xc9a4 , 0xc9a4 , None , "\xc9\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xc9a5 , 0xc9a5 , 0xc9a5 , None , "\xc9\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xc9a6 , 0xc9a6 , 0xc9a6 , None , "h" }, /* ?? : up=?? : lo=?? */
  { 0xc9a7 , 0xc9a7 , 0xc9a7 , None , "h" }, /* ?? : up=?? : lo=?? */
  { 0xc9a8 , 0xc697 , 0xc9a8 , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc9a9 , 0xc696 , 0xc9a9 , None , "i" }, /* ?? : up=?? : lo=?? */
  { 0xc9aa , 0xc9aa , 0xc9aa , None , "I" }, /* ?? : up=?? : lo=?? */
  { 0xc9ab , 0xc9ab , 0xc9ab , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc9ac , 0xc9ac , 0xc9ac , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc9ad , 0xc9ad , 0xc9ad , None , "l" }, /* ?? : up=?? : lo=?? */
  { 0xc9ae , 0xc9ae , 0xc9ae , None , "\xc9\xae" }, /* ?? : up=?? : lo=?? */
  { 0xc9af , 0xc69c , 0xc9af , None , "\xc9\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xc9b0 , 0xc9b0 , 0xc9b0 , None , "\xc9\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xc9b1 , 0xc9b1 , 0xc9b1 , None , "\xc9\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xc9b2 , 0xc69d , 0xc9b2 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc9b3 , 0xc9b3 , 0xc9b3 , None , "n" }, /* ?? : up=?? : lo=?? */
  { 0xc9b4 , 0xc9b4 , 0xc9b4 , None , "N" }, /* ?? : up=?? : lo=?? */
  { 0xc9b5 , 0xc69f , 0xc9b5 , None , "\xc9\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xc9b6 , 0xc9b6 , 0xc9b6 , None , "OE" }, /* ?? : up=?? : lo=?? */
  { 0xc9b7 , 0xc9b7 , 0xc9b7 , None , "\xc9\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xc9b8 , 0xc9b8 , 0xc9b8 , None , "\xc9\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xc9b9 , 0xc9b9 , 0xc9b9 , None , "\xc9\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xc9ba , 0xc9ba , 0xc9ba , None , "\xc9\xba" }, /* ?? : up=?? : lo=?? */
  { 0xc9bb , 0xc9bb , 0xc9bb , None , "\xc9\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xc9bc , 0xc9bc , 0xc9bc , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc9bd , 0xc9bd , 0xc9bd , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc9be , 0xc9be , 0xc9be , None , "r" }, /* ?? : up=?? : lo=?? */
  { 0xc9bf , 0xc9bf , 0xc9bf , None , "\xc9\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_ca[64] = {
  { 0xca80 , 0xc6a6 , 0xca80 , None , "R" }, /* ?? : up=?? : lo=?? */
  { 0xca81 , 0xca81 , 0xca81 , None , "\xca\x81" }, /* ?? : up=?? : lo=?? */
  { 0xca82 , 0xca82 , 0xca82 , None , "s" }, /* ?? : up=?? : lo=?? */
  { 0xca83 , 0xc6a9 , 0xca83 , None , "\xca\x83" }, /* ?? : up=?? : lo=?? */
  { 0xca84 , 0xca84 , 0xca84 , None , "\xca\x84" }, /* ?? : up=?? : lo=?? */
  { 0xca85 , 0xca85 , 0xca85 , None , "\xca\x85" }, /* ?? : up=?? : lo=?? */
  { 0xca86 , 0xca86 , 0xca86 , None , "\xca\x86" }, /* ?? : up=?? : lo=?? */
  { 0xca87 , 0xca87 , 0xca87 , None , "\xca\x87" }, /* ?? : up=?? : lo=?? */
  { 0xca88 , 0xc6ae , 0xca88 , None , "t" }, /* ?? : up=?? : lo=?? */
  { 0xca89 , 0xc984 , 0xca89 , None , "u" }, /* ?? : up=?? : lo=?? */
  { 0xca8a , 0xc6b1 , 0xca8a , None , "\xca\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xca8b , 0xc6b2 , 0xca8b , None , "v" }, /* ?? : up=?? : lo=?? */
  { 0xca8c , 0xc985 , 0xca8c , None , "\xca\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xca8d , 0xca8d , 0xca8d , None , "\xca\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xca8e , 0xca8e , 0xca8e , None , "\xca\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xca8f , 0xca8f , 0xca8f , None , "Y" }, /* ?? : up=?? : lo=?? */
  { 0xca90 , 0xca90 , 0xca90 , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xca91 , 0xca91 , 0xca91 , None , "z" }, /* ?? : up=?? : lo=?? */
  { 0xca92 , 0xc6b7 , 0xca92 , None , "\xca\x92" }, /* ?? : up=?? : lo=?? */
  { 0xca93 , 0xca93 , 0xca93 , None , "\xca\x93" }, /* ?? : up=?? : lo=?? */
  { 0xca94 , 0xca94 , 0xca94 , None , "\xca\x94" }, /* ?? : up=?? : lo=?? */
  { 0xca95 , 0xca95 , 0xca95 , None , "\xca\x95" }, /* ?? : up=?? : lo=?? */
  { 0xca96 , 0xca96 , 0xca96 , None , "\xca\x96" }, /* ?? : up=?? : lo=?? */
  { 0xca97 , 0xca97 , 0xca97 , None , "\xca\x97" }, /* ?? : up=?? : lo=?? */
  { 0xca98 , 0xca98 , 0xca98 , None , "\xca\x98" }, /* ?? : up=?? : lo=?? */
  { 0xca99 , 0xca99 , 0xca99 , None , "B" }, /* ?? : up=?? : lo=?? */
  { 0xca9a , 0xca9a , 0xca9a , None , "\xca\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xca9b , 0xca9b , 0xca9b , None , "G" }, /* ?? : up=?? : lo=?? */
  { 0xca9c , 0xca9c , 0xca9c , None , "H" }, /* ?? : up=?? : lo=?? */
  { 0xca9d , 0xca9d , 0xca9d , None , "j" }, /* ?? : up=?? : lo=?? */
  { 0xca9e , 0xca9e , 0xca9e , None , "\xca\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xca9f , 0xca9f , 0xca9f , None , "L" }, /* ?? : up=?? : lo=?? */
  { 0xcaa0 , 0xcaa0 , 0xcaa0 , None , "q" }, /* ?? : up=?? : lo=?? */
  { 0xcaa1 , 0xcaa1 , 0xcaa1 , None , "\xca\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xcaa2 , 0xcaa2 , 0xcaa2 , None , "\xca\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xcaa3 , 0xcaa3 , 0xcaa3 , None , "dz" }, /* ?? : up=?? : lo=?? */
  { 0xcaa4 , 0xcaa4 , 0xcaa4 , None , "\xca\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xcaa5 , 0xcaa5 , 0xcaa5 , None , "dz" }, /* ?? : up=?? : lo=?? */
  { 0xcaa6 , 0xcaa6 , 0xcaa6 , None , "ts" }, /* ?? : up=?? : lo=?? */
  { 0xcaa7 , 0xcaa7 , 0xcaa7 , None , "\xca\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xcaa8 , 0xcaa8 , 0xcaa8 , None , "\xca\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xcaa9 , 0xcaa9 , 0xcaa9 , None , "\xca\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xcaaa , 0xcaaa , 0xcaaa , None , "ls" }, /* ?? : up=?? : lo=?? */
  { 0xcaab , 0xcaab , 0xcaab , None , "lz" }, /* ?? : up=?? : lo=?? */
  { 0xcaac , 0xcaac , 0xcaac , None , "\xca\xac" }, /* ?? : up=?? : lo=?? */
  { 0xcaad , 0xcaad , 0xcaad , None , "\xca\xad" }, /* ?? : up=?? : lo=?? */
  { 0xcaae , 0xcaae , 0xcaae , None , "\xca\xae" }, /* ?? : up=?? : lo=?? */
  { 0xcaaf , 0xcaaf , 0xcaaf , None , "\xca\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xcab0 , 0xcab0 , 0xcab0 , IsModifier , "h" }, /* ?? : up=?? : lo=?? */
  { 0xcab1 , 0xcab1 , 0xcab1 , IsModifier , "h" }, /* ?? : up=?? : lo=?? */
  { 0xcab2 , 0xcab2 , 0xcab2 , IsModifier , "j" }, /* ?? : up=?? : lo=?? */
  { 0xcab3 , 0xcab3 , 0xcab3 , IsModifier , "r" }, /* ?? : up=?? : lo=?? */
  { 0xcab4 , 0xcab4 , 0xcab4 , IsModifier , "\xca\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xcab5 , 0xcab5 , 0xcab5 , IsModifier , "\xca\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xcab6 , 0xcab6 , 0xcab6 , IsModifier , "\xca\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xcab7 , 0xcab7 , 0xcab7 , IsModifier , "w" }, /* ?? : up=?? : lo=?? */
  { 0xcab8 , 0xcab8 , 0xcab8 , IsModifier , "y" }, /* ?? : up=?? : lo=?? */
  { 0xcab9 , 0xcab9 , 0xcab9 , IsModifier , "r" }, /* ?? : up=?? : lo=?? */
  { 0xcaba , 0xcaba , 0xcaba , IsModifier , "\xca\xba" }, /* ?? : up=?? : lo=?? */
  { 0xcabb , 0xcabb , 0xcabb , IsModifier , "\xca\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xcabc , 0xcabc , 0xcabc , IsModifier , "\xca\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xcabd , 0xcabd , 0xcabd , IsModifier , "\xca\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xcabe , 0xcabe , 0xcabe , IsModifier , "\xca\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xcabf , 0xcabf , 0xcabf , IsModifier , "\xca\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_cb[64] = {
  { 0xcb80 , 0xcb80 , 0xcb80 , IsModifier , "\xcb\x80" }, /* ?? : up=?? : lo=?? */
  { 0xcb81 , 0xcb81 , 0xcb81 , IsModifier , "\xcb\x81" }, /* ?? : up=?? : lo=?? */
  { 0xcb82 , 0xcb82 , 0xcb82 , IsModifier , "\xcb\x82" }, /* ?? : up=?? : lo=?? */
  { 0xcb83 , 0xcb83 , 0xcb83 , IsModifier , "\xcb\x83" }, /* ?? : up=?? : lo=?? */
  { 0xcb84 , 0xcb84 , 0xcb84 , IsModifier , "\xcb\x84" }, /* ?? : up=?? : lo=?? */
  { 0xcb85 , 0xcb85 , 0xcb85 , IsModifier , "\xcb\x85" }, /* ?? : up=?? : lo=?? */
  { 0xcb86 , 0xcb86 , 0xcb86 , IsModifier , "\xcb\x86" }, /* ?? : up=?? : lo=?? */
  { 0xcb87 , 0xcb87 , 0xcb87 , IsModifier , "\xcb\x87" }, /* ?? : up=?? : lo=?? */
  { 0xcb88 , 0xcb88 , 0xcb88 , IsModifier , "\xcb\x88" }, /* ?? : up=?? : lo=?? */
  { 0xcb89 , 0xcb89 , 0xcb89 , IsModifier , "\xcb\x89" }, /* ?? : up=?? : lo=?? */
  { 0xcb8a , 0xcb8a , 0xcb8a , IsModifier , "\xcb\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xcb8b , 0xcb8b , 0xcb8b , IsModifier , "\xcb\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xcb8c , 0xcb8c , 0xcb8c , IsModifier , "\xcb\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xcb8d , 0xcb8d , 0xcb8d , IsModifier , "\xcb\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xcb8e , 0xcb8e , 0xcb8e , IsModifier , "\xcb\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xcb8f , 0xcb8f , 0xcb8f , IsModifier , "\xcb\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xcb90 , 0xcb90 , 0xcb90 , IsModifier , "\xcb\x90" }, /* ?? : up=?? : lo=?? */
  { 0xcb91 , 0xcb91 , 0xcb91 , IsModifier , "\xcb\x91" }, /* ?? : up=?? : lo=?? */
  { 0xcb92 , 0xcb92 , 0xcb92 , IsModifier , "\xcb\x92" }, /* ?? : up=?? : lo=?? */
  { 0xcb93 , 0xcb93 , 0xcb93 , IsModifier , "\xcb\x93" }, /* ?? : up=?? : lo=?? */
  { 0xcb94 , 0xcb94 , 0xcb94 , IsModifier , "\xcb\x94" }, /* ?? : up=?? : lo=?? */
  { 0xcb95 , 0xcb95 , 0xcb95 , IsModifier , "\xcb\x95" }, /* ?? : up=?? : lo=?? */
  { 0xcb96 , 0xcb96 , 0xcb96 , IsModifier , "\xcb\x96" }, /* ?? : up=?? : lo=?? */
  { 0xcb97 , 0xcb97 , 0xcb97 , IsModifier , "\xcb\x97" }, /* ?? : up=?? : lo=?? */
  { 0xcb98 , 0xcb98 , 0xcb98 , IsModifier , "\xcb\x98" }, /* ?? : up=?? : lo=?? */
  { 0xcb99 , 0xcb99 , 0xcb99 , IsModifier , "\xcb\x99" }, /* ?? : up=?? : lo=?? */
  { 0xcb9a , 0xcb9a , 0xcb9a , IsModifier , "\xcb\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xcb9b , 0xcb9b , 0xcb9b , IsModifier , "\xcb\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xcb9c , 0xcb9c , 0xcb9c , IsModifier , "\xcb\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xcb9d , 0xcb9d , 0xcb9d , IsModifier , "\xcb\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xcb9e , 0xcb9e , 0xcb9e , IsModifier , "\xcb\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xcb9f , 0xcb9f , 0xcb9f , IsModifier , "\xcb\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xcba0 , 0xcba0 , 0xcba0 , IsModifier , "\xcb\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xcba1 , 0xcba1 , 0xcba1 , IsModifier , "l" }, /* ?? : up=?? : lo=?? */
  { 0xcba2 , 0xcba2 , 0xcba2 , IsModifier , "s" }, /* ?? : up=?? : lo=?? */
  { 0xcba3 , 0xcba3 , 0xcba3 , IsModifier , "x" }, /* ?? : up=?? : lo=?? */
  { 0xcba4 , 0xcba4 , 0xcba4 , IsModifier , "\xcb\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xcba5 , 0xcba5 , 0xcba5 , IsModifier , "\xcb\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xcba6 , 0xcba6 , 0xcba6 , IsModifier , "\xcb\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xcba7 , 0xcba7 , 0xcba7 , IsModifier , "\xcb\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xcba8 , 0xcba8 , 0xcba8 , IsModifier , "\xcb\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xcba9 , 0xcba9 , 0xcba9 , IsModifier , "\xcb\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xcbaa , 0xcbaa , 0xcbaa , IsModifier , "\xcb\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xcbab , 0xcbab , 0xcbab , IsModifier , "\xcb\xab" }, /* ?? : up=?? : lo=?? */
  { 0xcbac , 0xcbac , 0xcbac , IsModifier , "\xcb\xac" }, /* ?? : up=?? : lo=?? */
  { 0xcbad , 0xcbad , 0xcbad , IsModifier , "\xcb\xad" }, /* ?? : up=?? : lo=?? */
  { 0xcbae , 0xcbae , 0xcbae , IsModifier , "\xcb\xae" }, /* ?? : up=?? : lo=?? */
  { 0xcbaf , 0xcbaf , 0xcbaf , IsModifier , "\xcb\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xcbb0 , 0xcbb0 , 0xcbb0 , IsModifier , "\xcb\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xcbb1 , 0xcbb1 , 0xcbb1 , IsModifier , "\xcb\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xcbb2 , 0xcbb2 , 0xcbb2 , IsModifier , "\xcb\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xcbb3 , 0xcbb3 , 0xcbb3 , IsModifier , "\xcb\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xcbb4 , 0xcbb4 , 0xcbb4 , IsModifier , "\xcb\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xcbb5 , 0xcbb5 , 0xcbb5 , IsModifier , "\xcb\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xcbb6 , 0xcbb6 , 0xcbb6 , IsModifier , "\xcb\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xcbb7 , 0xcbb7 , 0xcbb7 , IsModifier , "\xcb\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xcbb8 , 0xcbb8 , 0xcbb8 , IsModifier , "\xcb\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xcbb9 , 0xcbb9 , 0xcbb9 , IsModifier , "\xcb\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xcbba , 0xcbba , 0xcbba , IsModifier , "\xcb\xba" }, /* ?? : up=?? : lo=?? */
  { 0xcbbb , 0xcbbb , 0xcbbb , IsModifier , "\xcb\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xcbbc , 0xcbbc , 0xcbbc , IsModifier , "\xcb\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xcbbd , 0xcbbd , 0xcbbd , IsModifier , "\xcb\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xcbbe , 0xcbbe , 0xcbbe , IsModifier , "\xcb\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xcbbf , 0xcbbf , 0xcbbf , IsModifier , "\xcb\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_cc[64] = {
  { 0xcc80 , 0xcc80 , 0xcc80 , IsDiacritic , "\xcc\x80" },
  { 0xcc81 , 0xcc81 , 0xcc81 , IsDiacritic , "\xcc\x81" },
  { 0xcc82 , 0xcc82 , 0xcc82 , IsDiacritic , "\xcc\x82" },
  { 0xcc83 , 0xcc83 , 0xcc83 , IsDiacritic , "\xcc\x83" },
  { 0xcc84 , 0xcc84 , 0xcc84 , IsDiacritic , "\xcc\x84" },
  { 0xcc85 , 0xcc85 , 0xcc85 , IsDiacritic , "\xcc\x85" },
  { 0xcc86 , 0xcc86 , 0xcc86 , IsDiacritic , "\xcc\x86" },
  { 0xcc87 , 0xcc87 , 0xcc87 , IsDiacritic , "\xcc\x87" },
  { 0xcc88 , 0xcc88 , 0xcc88 , IsDiacritic , "\xcc\x88" },
  { 0xcc89 , 0xcc89 , 0xcc89 , IsDiacritic , "\xcc\x89" },
  { 0xcc8a , 0xcc8a , 0xcc8a , IsDiacritic , "\xcc\x8a" },
  { 0xcc8b , 0xcc8b , 0xcc8b , IsDiacritic , "\xcc\x8b" },
  { 0xcc8c , 0xcc8c , 0xcc8c , IsDiacritic , "\xcc\x8c" },
  { 0xcc8d , 0xcc8d , 0xcc8d , IsDiacritic , "\xcc\x8d" },
  { 0xcc8e , 0xcc8e , 0xcc8e , IsDiacritic , "\xcc\x8e" },
  { 0xcc8f , 0xcc8f , 0xcc8f , IsDiacritic , "\xcc\x8f" },
  { 0xcc90 , 0xcc90 , 0xcc90 , IsDiacritic , "\xcc\x90" },
  { 0xcc91 , 0xcc91 , 0xcc91 , IsDiacritic , "\xcc\x91" },
  { 0xcc92 , 0xcc92 , 0xcc92 , IsDiacritic , "\xcc\x92" },
  { 0xcc93 , 0xcc93 , 0xcc93 , IsDiacritic , "\xcc\x93" },
  { 0xcc94 , 0xcc94 , 0xcc94 , IsDiacritic , "\xcc\x94" },
  { 0xcc95 , 0xcc95 , 0xcc95 , IsDiacritic , "\xcc\x95" },
  { 0xcc96 , 0xcc96 , 0xcc96 , IsDiacritic , "\xcc\x96" },
  { 0xcc97 , 0xcc97 , 0xcc97 , IsDiacritic , "\xcc\x97" },
  { 0xcc98 , 0xcc98 , 0xcc98 , IsDiacritic , "\xcc\x98" },
  { 0xcc99 , 0xcc99 , 0xcc99 , IsDiacritic , "\xcc\x99" },
  { 0xcc9a , 0xcc9a , 0xcc9a , IsDiacritic , "\xcc\x9a" },
  { 0xcc9b , 0xcc9b , 0xcc9b , IsDiacritic , "\xcc\x9b" },
  { 0xcc9c , 0xcc9c , 0xcc9c , IsDiacritic , "\xcc\x9c" },
  { 0xcc9d , 0xcc9d , 0xcc9d , IsDiacritic , "\xcc\x9d" },
  { 0xcc9e , 0xcc9e , 0xcc9e , IsDiacritic , "\xcc\x9e" },
  { 0xcc9f , 0xcc9f , 0xcc9f , IsDiacritic , "\xcc\x9f" },
  { 0xcca0 , 0xcca0 , 0xcca0 , IsDiacritic , "\xcc\xa0" },
  { 0xcca1 , 0xcca1 , 0xcca1 , IsDiacritic , "\xcc\xa1" },
  { 0xcca2 , 0xcca2 , 0xcca2 , IsDiacritic , "\xcc\xa2" },
  { 0xcca3 , 0xcca3 , 0xcca3 , IsDiacritic , "\xcc\xa3" },
  { 0xcca4 , 0xcca4 , 0xcca4 , IsDiacritic , "\xcc\xa4" },
  { 0xcca5 , 0xcca5 , 0xcca5 , IsDiacritic , "\xcc\xa5" },
  { 0xcca6 , 0xcca6 , 0xcca6 , IsDiacritic , "\xcc\xa6" },
  { 0xcca7 , 0xcca7 , 0xcca7 , IsDiacritic , "\xcc\xa7" },
  { 0xcca8 , 0xcca8 , 0xcca8 , IsDiacritic , "\xcc\xa8" },
  { 0xcca9 , 0xcca9 , 0xcca9 , IsDiacritic , "\xcc\xa9" },
  { 0xccaa , 0xccaa , 0xccaa , IsDiacritic , "\xcc\xaa" },
  { 0xccab , 0xccab , 0xccab , IsDiacritic , "\xcc\xab" },
  { 0xccac , 0xccac , 0xccac , IsDiacritic , "\xcc\xac" },
  { 0xccad , 0xccad , 0xccad , IsDiacritic , "\xcc\xad" },
  { 0xccae , 0xccae , 0xccae , IsDiacritic , "\xcc\xae" },
  { 0xccaf , 0xccaf , 0xccaf , IsDiacritic , "\xcc\xaf" },
  { 0xccb0 , 0xccb0 , 0xccb0 , IsDiacritic , "\xcc\xb0" },
  { 0xccb1 , 0xccb1 , 0xccb1 , IsDiacritic , "\xcc\xb1" },
  { 0xccb2 , 0xccb2 , 0xccb2 , IsDiacritic , "\xcc\xb2" },
  { 0xccb3 , 0xccb3 , 0xccb3 , IsDiacritic , "\xcc\xb3" },
  { 0xccb4 , 0xccb4 , 0xccb4 , IsDiacritic , "\xcc\xb4" },
  { 0xccb5 , 0xccb5 , 0xccb5 , IsDiacritic , "\xcc\xb5" },
  { 0xccb6 , 0xccb6 , 0xccb6 , IsDiacritic , "\xcc\xb6" },
  { 0xccb7 , 0xccb7 , 0xccb7 , IsDiacritic , "\xcc\xb7" },
  { 0xccb8 , 0xccb8 , 0xccb8 , IsDiacritic , "\xcc\xb8" },
  { 0xccb9 , 0xccb9 , 0xccb9 , IsDiacritic , "\xcc\xb9" },
  { 0xccba , 0xccba , 0xccba , IsDiacritic , "\xcc\xba" },
  { 0xccbb , 0xccbb , 0xccbb , IsDiacritic , "\xcc\xbb" },
  { 0xccbc , 0xccbc , 0xccbc , IsDiacritic , "\xcc\xbc" },
  { 0xccbd , 0xccbd , 0xccbd , IsDiacritic , "\xcc\xbd" },
  { 0xccbe , 0xccbe , 0xccbe , IsDiacritic , "\xcc\xbe" },
  { 0xccbf , 0xccbf , 0xccbf , IsDiacritic , "\xcc\xbf" },
};

const character charmap_cd[64] = {
  { 0xcd80 , 0xcd80 , 0xcd80 , IsDiacritic , "\xcd\x80" },
  { 0xcd81 , 0xcd81 , 0xcd81 , IsDiacritic , "\xcd\x81" },
  { 0xcd82 , 0xcd82 , 0xcd82 , IsDiacritic , "\xcd\x82" },
  { 0xcd83 , 0xcd83 , 0xcd83 , IsDiacritic , "\xcd\x83" },
  { 0xcd84 , 0xcd84 , 0xcd84 , IsDiacritic , "\xcd\x84" },
  { 0xcd85 , 0xcd85 , 0xcd85 , IsDiacritic , "\xcd\x85" },
  { 0xcd86 , 0xcd86 , 0xcd86 , IsDiacritic , "\xcd\x86" },
  { 0xcd87 , 0xcd87 , 0xcd87 , IsDiacritic , "\xcd\x87" },
  { 0xcd88 , 0xcd88 , 0xcd88 , IsDiacritic , "\xcd\x88" },
  { 0xcd89 , 0xcd89 , 0xcd89 , IsDiacritic , "\xcd\x89" },
  { 0xcd8a , 0xcd8a , 0xcd8a , IsDiacritic , "\xcd\x8a" },
  { 0xcd8b , 0xcd8b , 0xcd8b , IsDiacritic , "\xcd\x8b" },
  { 0xcd8c , 0xcd8c , 0xcd8c , IsDiacritic , "\xcd\x8c" },
  { 0xcd8d , 0xcd8d , 0xcd8d , IsDiacritic , "\xcd\x8d" },
  { 0xcd8e , 0xcd8e , 0xcd8e , IsDiacritic , "\xcd\x8e" },
  { 0xcd8f , 0xcd8f , 0xcd8f , IsDiacritic , "\xcd\x8f" },
  { 0xcd90 , 0xcd90 , 0xcd90 , IsDiacritic , "\xcd\x90" },
  { 0xcd91 , 0xcd91 , 0xcd91 , IsDiacritic , "\xcd\x91" },
  { 0xcd92 , 0xcd92 , 0xcd92 , IsDiacritic , "\xcd\x92" },
  { 0xcd93 , 0xcd93 , 0xcd93 , IsDiacritic , "\xcd\x93" },
  { 0xcd94 , 0xcd94 , 0xcd94 , IsDiacritic , "\xcd\x94" },
  { 0xcd95 , 0xcd95 , 0xcd95 , IsDiacritic , "\xcd\x95" },
  { 0xcd96 , 0xcd96 , 0xcd96 , IsDiacritic , "\xcd\x96" },
  { 0xcd97 , 0xcd97 , 0xcd97 , IsDiacritic , "\xcd\x97" },
  { 0xcd98 , 0xcd98 , 0xcd98 , IsDiacritic , "\xcd\x98" },
  { 0xcd99 , 0xcd99 , 0xcd99 , IsDiacritic , "\xcd\x99" },
  { 0xcd9a , 0xcd9a , 0xcd9a , IsDiacritic , "\xcd\x9a" },
  { 0xcd9b , 0xcd9b , 0xcd9b , IsDiacritic , "\xcd\x9b" },
  { 0xcd9c , 0xcd9c , 0xcd9c , IsDiacritic , "\xcd\x9c" },
  { 0xcd9d , 0xcd9d , 0xcd9d , IsDiacritic , "\xcd\x9d" },
  { 0xcd9e , 0xcd9e , 0xcd9e , IsDiacritic , "\xcd\x9e" },
  { 0xcd9f , 0xcd9f , 0xcd9f , IsDiacritic , "\xcd\x9f" },
  { 0xcda0 , 0xcda0 , 0xcda0 , IsDiacritic , "\xcd\xa0" },
  { 0xcda1 , 0xcda1 , 0xcda1 , IsDiacritic , "\xcd\xa1" },
  { 0xcda2 , 0xcda2 , 0xcda2 , IsDiacritic , "\xcd\xa2" },
  { 0xcda3 , 0xcda3 , 0xcda3 , IsDiacritic , "\xcd\xa3" },
  { 0xcda4 , 0xcda4 , 0xcda4 , IsDiacritic , "\xcd\xa4" },
  { 0xcda5 , 0xcda5 , 0xcda5 , IsDiacritic , "\xcd\xa5" },
  { 0xcda6 , 0xcda6 , 0xcda6 , IsDiacritic , "\xcd\xa6" },
  { 0xcda7 , 0xcda7 , 0xcda7 , IsDiacritic , "\xcd\xa7" },
  { 0xcda8 , 0xcda8 , 0xcda8 , IsDiacritic , "\xcd\xa8" },
  { 0xcda9 , 0xcda9 , 0xcda9 , IsDiacritic , "\xcd\xa9" },
  { 0xcdaa , 0xcdaa , 0xcdaa , IsDiacritic , "\xcd\xaa" },
  { 0xcdab , 0xcdab , 0xcdab , IsDiacritic , "\xcd\xab" },
  { 0xcdac , 0xcdac , 0xcdac , IsDiacritic , "\xcd\xac" },
  { 0xcdad , 0xcdad , 0xcdad , IsDiacritic , "\xcd\xad" },
  { 0xcdae , 0xcdae , 0xcdae , IsDiacritic , "\xcd\xae" },
  { 0xcdaf , 0xcdaf , 0xcdaf , IsDiacritic , "\xcd\xaf" },
  { 0xcdb0 , 0xcdb0 , 0xcdb1 , None , "\xcd\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xcdb1 , 0xcdb0 , 0xcdb1 , None , "\xcd\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xcdb2 , 0xcdb2 , 0xcdb3 , None , "\xcd\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xcdb3 , 0xcdb2 , 0xcdb3 , None , "\xcd\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xcdb4 , 0xcdb4 , 0xcdb4 , None , "\xcd\xb4" },
  { 0xcdb5 , 0xcdb5 , 0xcdb5 , None , "\xcd\xb5" },
  { 0xcdb6 , 0xcdb6 , 0xcdb7 , None , "\xcd\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xcdb7 , 0xcdb6 , 0xcdb7 , None , "\xcd\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xcdb8 , 0xcdb8 , 0xcdb8 , None , "\xcd\xb8" },
  { 0xcdb9 , 0xcdb9 , 0xcdb9 , None , "\xcd\xb9" },
  { 0xcdba , 0xcdba , 0xcdba , None , "\xcd\xba" },
  { 0xcdbb , 0xcfbd , 0xcdbb , None , "\xcd\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xcdbc , 0xcfbe , 0xcdbc , None , "\xcd\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xcdbd , 0xcfbf , 0xcdbd , None , "\xcd\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xcdbe , 0xcdbe , 0xcdbe , None , "\xcd\xbe" },
  { 0xcdbf , 0xcdbf , 0xcfb3 , None , "\xcd\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_ce[64] = {
  { 0xce80 , 0xce80 , 0xce80 , None , "\xce\x80" },
  { 0xce81 , 0xce81 , 0xce81 , None , "\xce\x81" },
  { 0xce82 , 0xce82 , 0xce82 , None , "\xce\x82" },
  { 0xce83 , 0xce83 , 0xce83 , None , "\xce\x83" },
  { 0xce84 , 0xce84 , 0xce84 , None , "\xce\x84" },
  { 0xce85 , 0xce85 , 0xce85 , None , "\xce\x85" },
  { 0xce86 , 0xce86 , 0xceac , None , "\xce\x86" }, /* ?? : up=?? : lo=?? */
  { 0xce87 , 0xce87 , 0xce87 , None , "\xce\x87" }, /* ?? : up=?? : lo=?? */
  { 0xce88 , 0xce88 , 0xcead , None , "\xce\x88" }, /* ?? : up=?? : lo=?? */
  { 0xce89 , 0xce89 , 0xceae , None , "\xce\x89" }, /* ?? : up=?? : lo=?? */
  { 0xce8a , 0xce8a , 0xceaf , None , "\xce\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xce8b , 0xce8b , 0xce8b , None , "\xce\x8b" },
  { 0xce8c , 0xce8c , 0xcf8c , None , "\xce\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xce8d , 0xce8d , 0xce8d , None , "\xce\x8d" },
  { 0xce8e , 0xce8e , 0xcf8d , None , "\xce\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xce8f , 0xce8f , 0xcf8e , None , "\xce\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xce90 , 0xce90 , 0xce90 , None , "\xce\x90" }, /* ?? : up=?? : lo=?? */
  { 0xce91 , 0xce91 , 0xceb1 , None , "\xce\x91" }, /* ?? : up=?? : lo=?? */
  { 0xce92 , 0xce92 , 0xceb2 , None , "\xce\x92" }, /* ?? : up=?? : lo=?? */
  { 0xce93 , 0xce93 , 0xceb3 , None , "\xce\x93" }, /* ?? : up=?? : lo=?? */
  { 0xce94 , 0xce94 , 0xceb4 , None , "\xce\x94" }, /* ?? : up=?? : lo=?? */
  { 0xce95 , 0xce95 , 0xceb5 , None , "\xce\x95" }, /* ?? : up=?? : lo=?? */
  { 0xce96 , 0xce96 , 0xceb6 , None , "\xce\x96" }, /* ?? : up=?? : lo=?? */
  { 0xce97 , 0xce97 , 0xceb7 , None , "\xce\x97" }, /* ?? : up=?? : lo=?? */
  { 0xce98 , 0xce98 , 0xceb8 , None , "\xce\x98" }, /* ?? : up=?? : lo=?? */
  { 0xce99 , 0xce99 , 0xceb9 , None , "\xce\x99" }, /* ?? : up=?? : lo=?? */
  { 0xce9a , 0xce9a , 0xceba , None , "\xce\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xce9b , 0xce9b , 0xcebb , None , "\xce\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xce9c , 0xce9c , 0xcebc , None , "\xce\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xce9d , 0xce9d , 0xcebd , None , "\xce\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xce9e , 0xce9e , 0xcebe , None , "\xce\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xce9f , 0xce9f , 0xcebf , None , "\xce\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xcea0 , 0xcea0 , 0xcf80 , None , "\xce\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xcea1 , 0xcea1 , 0xcf81 , None , "\xce\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xcea2 , 0xcea2 , 0xcea2 , None , "\xce\xa2" },
  { 0xcea3 , 0xcea3 , 0xcf83 , None , "\xce\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xcea4 , 0xcea4 , 0xcf84 , None , "\xce\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xcea5 , 0xcea5 , 0xcf85 , None , "\xce\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xcea6 , 0xcea6 , 0xcf86 , None , "\xce\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xcea7 , 0xcea7 , 0xcf87 , None , "\xce\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xcea8 , 0xcea8 , 0xcf88 , None , "\xce\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xcea9 , 0xcea9 , 0xcf89 , None , "\xce\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xceaa , 0xceaa , 0xcf8a , None , "\xce\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xceab , 0xceab , 0xcf8b , None , "\xce\xab" }, /* ?? : up=?? : lo=?? */
  { 0xceac , 0xce86 , 0xceac , None , "\xce\xac" }, /* ?? : up=?? : lo=?? */
  { 0xcead , 0xce88 , 0xcead , None , "\xce\xad" }, /* ?? : up=?? : lo=?? */
  { 0xceae , 0xce89 , 0xceae , None , "\xce\xae" }, /* ?? : up=?? : lo=?? */
  { 0xceaf , 0xce8a , 0xceaf , None , "\xce\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xceb0 , 0xceb0 , 0xceb0 , None , "\xce\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xceb1 , 0xce91 , 0xceb1 , None , "\xce\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xceb2 , 0xce92 , 0xceb2 , None , "\xce\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xceb3 , 0xce93 , 0xceb3 , None , "\xce\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xceb4 , 0xce94 , 0xceb4 , None , "\xce\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xceb5 , 0xce95 , 0xceb5 , None , "\xce\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xceb6 , 0xce96 , 0xceb6 , None , "\xce\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xceb7 , 0xce97 , 0xceb7 , None , "\xce\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xceb8 , 0xce98 , 0xceb8 , None , "\xce\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xceb9 , 0xce99 , 0xceb9 , None , "\xce\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xceba , 0xce9a , 0xceba , None , "\xce\xba" }, /* ?? : up=?? : lo=?? */
  { 0xcebb , 0xce9b , 0xcebb , None , "\xce\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xcebc , 0xce9c , 0xcebc , None , "\xce\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xcebd , 0xce9d , 0xcebd , None , "\xce\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xcebe , 0xce9e , 0xcebe , None , "\xce\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xcebf , 0xce9f , 0xcebf , None , "\xce\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_cf[64] = {
  { 0xcf80 , 0xcea0 , 0xcf80 , None , "\xcf\x80" }, /* ?? : up=?? : lo=?? */
  { 0xcf81 , 0xcea1 , 0xcf81 , None , "\xcf\x81" }, /* ?? : up=?? : lo=?? */
  { 0xcf82 , 0xcea3 , 0xcf82 , None , "\xcf\x82" }, /* ?? : up=?? : lo=?? */
  { 0xcf83 , 0xcea3 , 0xcf83 , None , "\xcf\x83" }, /* ?? : up=?? : lo=?? */
  { 0xcf84 , 0xcea4 , 0xcf84 , None , "\xcf\x84" }, /* ?? : up=?? : lo=?? */
  { 0xcf85 , 0xcea5 , 0xcf85 , None , "\xcf\x85" }, /* ?? : up=?? : lo=?? */
  { 0xcf86 , 0xcea6 , 0xcf86 , None , "\xcf\x86" }, /* ?? : up=?? : lo=?? */
  { 0xcf87 , 0xcea7 , 0xcf87 , None , "\xcf\x87" }, /* ?? : up=?? : lo=?? */
  { 0xcf88 , 0xcea8 , 0xcf88 , None , "\xcf\x88" }, /* ?? : up=?? : lo=?? */
  { 0xcf89 , 0xcea9 , 0xcf89 , None , "\xcf\x89" }, /* ?? : up=?? : lo=?? */
  { 0xcf8a , 0xceaa , 0xcf8a , None , "\xcf\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xcf8b , 0xceab , 0xcf8b , None , "\xcf\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xcf8c , 0xce8c , 0xcf8c , None , "\xcf\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xcf8d , 0xce8e , 0xcf8d , None , "\xcf\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xcf8e , 0xce8f , 0xcf8e , None , "\xcf\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xcf8f , 0xcf8f , 0xcf97 , None , "\xcf\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xcf90 , 0xce92 , 0xcf90 , None , "\xcf\x90" }, /* ?? : up=?? : lo=?? */
  { 0xcf91 , 0xce98 , 0xcf91 , None , "\xcf\x91" }, /* ?? : up=?? : lo=?? */
  { 0xcf92 , 0xcf92 , 0xcf92 , None , "\xcf\x92" }, /* ?? : up=?? : lo=?? */
  { 0xcf93 , 0xcf93 , 0xcf93 , None , "\xcf\x93" }, /* ?? : up=?? : lo=?? */
  { 0xcf94 , 0xcf94 , 0xcf94 , None , "\xcf\x94" }, /* ?? : up=?? : lo=?? */
  { 0xcf95 , 0xcea6 , 0xcf95 , None , "\xcf\x95" }, /* ?? : up=?? : lo=?? */
  { 0xcf96 , 0xcea0 , 0xcf96 , None , "\xcf\x96" }, /* ?? : up=?? : lo=?? */
  { 0xcf97 , 0xcf8f , 0xcf97 , None , "\xcf\x97" }, /* ?? : up=?? : lo=?? */
  { 0xcf98 , 0xcf98 , 0xcf99 , None , "\xcf\x98" }, /* ?? : up=?? : lo=?? */
  { 0xcf99 , 0xcf98 , 0xcf99 , None , "\xcf\x99" }, /* ?? : up=?? : lo=?? */
  { 0xcf9a , 0xcf9a , 0xcf9b , None , "\xcf\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xcf9b , 0xcf9a , 0xcf9b , None , "\xcf\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xcf9c , 0xcf9c , 0xcf9d , None , "\xcf\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xcf9d , 0xcf9c , 0xcf9d , None , "\xcf\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xcf9e , 0xcf9e , 0xcf9f , None , "\xcf\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xcf9f , 0xcf9e , 0xcf9f , None , "\xcf\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xcfa0 , 0xcfa0 , 0xcfa1 , None , "\xcf\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xcfa1 , 0xcfa0 , 0xcfa1 , None , "\xcf\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xcfa2 , 0xcfa2 , 0xcfa3 , None , "\xcf\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xcfa3 , 0xcfa2 , 0xcfa3 , None , "\xcf\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xcfa4 , 0xcfa4 , 0xcfa5 , None , "\xcf\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xcfa5 , 0xcfa4 , 0xcfa5 , None , "\xcf\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xcfa6 , 0xcfa6 , 0xcfa7 , None , "\xcf\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xcfa7 , 0xcfa6 , 0xcfa7 , None , "\xcf\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xcfa8 , 0xcfa8 , 0xcfa9 , None , "\xcf\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xcfa9 , 0xcfa8 , 0xcfa9 , None , "\xcf\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xcfaa , 0xcfaa , 0xcfab , None , "\xcf\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xcfab , 0xcfaa , 0xcfab , None , "\xcf\xab" }, /* ?? : up=?? : lo=?? */
  { 0xcfac , 0xcfac , 0xcfad , None , "\xcf\xac" }, /* ?? : up=?? : lo=?? */
  { 0xcfad , 0xcfac , 0xcfad , None , "\xcf\xad" }, /* ?? : up=?? : lo=?? */
  { 0xcfae , 0xcfae , 0xcfaf , None , "\xcf\xae" }, /* ?? : up=?? : lo=?? */
  { 0xcfaf , 0xcfae , 0xcfaf , None , "\xcf\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xcfb0 , 0xce9a , 0xcfb0 , None , "\xcf\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xcfb1 , 0xcea1 , 0xcfb1 , None , "\xcf\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xcfb2 , 0xcfb9 , 0xcfb2 , None , "\xcf\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xcfb3 , 0xcdbf , 0xcfb3 , None , "\xcf\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xcfb4 , 0xcfb4 , 0xceb8 , None , "\xcf\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xcfb5 , 0xce95 , 0xcfb5 , None , "\xcf\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xcfb6 , 0xcfb6 , 0xcfb6 , None , "\xcf\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xcfb7 , 0xcfb7 , 0xcfb8 , None , "\xcf\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xcfb8 , 0xcfb7 , 0xcfb8 , None , "\xcf\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xcfb9 , 0xcfb9 , 0xcfb2 , None , "\xcf\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xcfba , 0xcfba , 0xcfbb , None , "\xcf\xba" }, /* ?? : up=?? : lo=?? */
  { 0xcfbb , 0xcfba , 0xcfbb , None , "\xcf\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xcfbc , 0xcfbc , 0xcfbc , None , "\xcf\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xcfbd , 0xcfbd , 0xcdbb , None , "\xcf\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xcfbe , 0xcfbe , 0xcdbc , None , "\xcf\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xcfbf , 0xcfbf , 0xcdbd , None , "\xcf\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d0[64] = {
  { 0xd080 , 0xd080 , 0xd190 , None , "\xd0\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd081 , 0xd081 , 0xd191 , None , "\xd0\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd082 , 0xd082 , 0xd192 , None , "\xd0\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd083 , 0xd083 , 0xd193 , None , "\xd0\x83" }, /* ?? : up=?? : lo=?? */
  { 0xd084 , 0xd084 , 0xd194 , None , "\xd0\x84" }, /* ?? : up=?? : lo=?? */
  { 0xd085 , 0xd085 , 0xd195 , None , "\xd0\x85" }, /* ?? : up=?? : lo=?? */
  { 0xd086 , 0xd086 , 0xd196 , None , "\xd0\x86" }, /* ?? : up=?? : lo=?? */
  { 0xd087 , 0xd087 , 0xd197 , None , "\xd0\x87" }, /* ?? : up=?? : lo=?? */
  { 0xd088 , 0xd088 , 0xd198 , None , "\xd0\x88" }, /* ?? : up=?? : lo=?? */
  { 0xd089 , 0xd089 , 0xd199 , None , "\xd0\x89" }, /* ?? : up=?? : lo=?? */
  { 0xd08a , 0xd08a , 0xd19a , None , "\xd0\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xd08b , 0xd08b , 0xd19b , None , "\xd0\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xd08c , 0xd08c , 0xd19c , None , "\xd0\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xd08d , 0xd08d , 0xd19d , None , "\xd0\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xd08e , 0xd08e , 0xd19e , None , "\xd0\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xd08f , 0xd08f , 0xd19f , None , "\xd0\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xd090 , 0xd090 , 0xd0b0 , None , "\xd0\x90" }, /* ?? : up=?? : lo=?? */
  { 0xd091 , 0xd091 , 0xd0b1 , None , "\xd0\x91" }, /* ?? : up=?? : lo=?? */
  { 0xd092 , 0xd092 , 0xd0b2 , None , "\xd0\x92" }, /* ?? : up=?? : lo=?? */
  { 0xd093 , 0xd093 , 0xd0b3 , None , "\xd0\x93" }, /* ?? : up=?? : lo=?? */
  { 0xd094 , 0xd094 , 0xd0b4 , None , "\xd0\x94" }, /* ?? : up=?? : lo=?? */
  { 0xd095 , 0xd095 , 0xd0b5 , None , "\xd0\x95" }, /* ?? : up=?? : lo=?? */
  { 0xd096 , 0xd096 , 0xd0b6 , None , "\xd0\x96" }, /* ?? : up=?? : lo=?? */
  { 0xd097 , 0xd097 , 0xd0b7 , None , "\xd0\x97" }, /* ?? : up=?? : lo=?? */
  { 0xd098 , 0xd098 , 0xd0b8 , None , "\xd0\x98" }, /* ?? : up=?? : lo=?? */
  { 0xd099 , 0xd099 , 0xd0b9 , None , "\xd0\x99" }, /* ?? : up=?? : lo=?? */
  { 0xd09a , 0xd09a , 0xd0ba , None , "\xd0\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xd09b , 0xd09b , 0xd0bb , None , "\xd0\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xd09c , 0xd09c , 0xd0bc , None , "\xd0\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xd09d , 0xd09d , 0xd0bd , None , "\xd0\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xd09e , 0xd09e , 0xd0be , None , "\xd0\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xd09f , 0xd09f , 0xd0bf , None , "\xd0\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xd0a0 , 0xd0a0 , 0xd180 , None , "\xd0\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xd0a1 , 0xd0a1 , 0xd181 , None , "\xd0\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xd0a2 , 0xd0a2 , 0xd182 , None , "\xd0\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xd0a3 , 0xd0a3 , 0xd183 , None , "\xd0\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xd0a4 , 0xd0a4 , 0xd184 , None , "\xd0\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xd0a5 , 0xd0a5 , 0xd185 , None , "\xd0\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xd0a6 , 0xd0a6 , 0xd186 , None , "\xd0\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xd0a7 , 0xd0a7 , 0xd187 , None , "\xd0\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xd0a8 , 0xd0a8 , 0xd188 , None , "\xd0\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xd0a9 , 0xd0a9 , 0xd189 , None , "\xd0\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xd0aa , 0xd0aa , 0xd18a , None , "\xd0\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xd0ab , 0xd0ab , 0xd18b , None , "\xd0\xab" }, /* ?? : up=?? : lo=?? */
  { 0xd0ac , 0xd0ac , 0xd18c , None , "\xd0\xac" }, /* ?? : up=?? : lo=?? */
  { 0xd0ad , 0xd0ad , 0xd18d , None , "\xd0\xad" }, /* ?? : up=?? : lo=?? */
  { 0xd0ae , 0xd0ae , 0xd18e , None , "\xd0\xae" }, /* ?? : up=?? : lo=?? */
  { 0xd0af , 0xd0af , 0xd18f , None , "\xd0\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xd0b0 , 0xd090 , 0xd0b0 , None , "\xd0\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xd0b1 , 0xd091 , 0xd0b1 , None , "\xd0\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xd0b2 , 0xd092 , 0xd0b2 , None , "\xd0\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xd0b3 , 0xd093 , 0xd0b3 , None , "\xd0\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xd0b4 , 0xd094 , 0xd0b4 , None , "\xd0\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xd0b5 , 0xd095 , 0xd0b5 , None , "\xd0\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xd0b6 , 0xd096 , 0xd0b6 , None , "\xd0\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xd0b7 , 0xd097 , 0xd0b7 , None , "\xd0\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xd0b8 , 0xd098 , 0xd0b8 , None , "\xd0\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xd0b9 , 0xd099 , 0xd0b9 , None , "\xd0\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xd0ba , 0xd09a , 0xd0ba , None , "\xd0\xba" }, /* ?? : up=?? : lo=?? */
  { 0xd0bb , 0xd09b , 0xd0bb , None , "\xd0\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xd0bc , 0xd09c , 0xd0bc , None , "\xd0\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xd0bd , 0xd09d , 0xd0bd , None , "\xd0\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xd0be , 0xd09e , 0xd0be , None , "\xd0\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xd0bf , 0xd09f , 0xd0bf , None , "\xd0\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d1[64] = {
  { 0xd180 , 0xd0a0 , 0xd180 , None , "\xd1\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd181 , 0xd0a1 , 0xd181 , None , "\xd1\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd182 , 0xd0a2 , 0xd182 , None , "\xd1\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd183 , 0xd0a3 , 0xd183 , None , "\xd1\x83" }, /* ?? : up=?? : lo=?? */
  { 0xd184 , 0xd0a4 , 0xd184 , None , "\xd1\x84" }, /* ?? : up=?? : lo=?? */
  { 0xd185 , 0xd0a5 , 0xd185 , None , "\xd1\x85" }, /* ?? : up=?? : lo=?? */
  { 0xd186 , 0xd0a6 , 0xd186 , None , "\xd1\x86" }, /* ?? : up=?? : lo=?? */
  { 0xd187 , 0xd0a7 , 0xd187 , None , "\xd1\x87" }, /* ?? : up=?? : lo=?? */
  { 0xd188 , 0xd0a8 , 0xd188 , None , "\xd1\x88" }, /* ?? : up=?? : lo=?? */
  { 0xd189 , 0xd0a9 , 0xd189 , None , "\xd1\x89" }, /* ?? : up=?? : lo=?? */
  { 0xd18a , 0xd0aa , 0xd18a , None , "\xd1\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xd18b , 0xd0ab , 0xd18b , None , "\xd1\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xd18c , 0xd0ac , 0xd18c , None , "\xd1\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xd18d , 0xd0ad , 0xd18d , None , "\xd1\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xd18e , 0xd0ae , 0xd18e , None , "\xd1\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xd18f , 0xd0af , 0xd18f , None , "\xd1\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xd190 , 0xd080 , 0xd190 , None , "\xd1\x90" }, /* ?? : up=?? : lo=?? */
  { 0xd191 , 0xd081 , 0xd191 , None , "\xd1\x91" }, /* ?? : up=?? : lo=?? */
  { 0xd192 , 0xd082 , 0xd192 , None , "\xd1\x92" }, /* ?? : up=?? : lo=?? */
  { 0xd193 , 0xd083 , 0xd193 , None , "\xd1\x93" }, /* ?? : up=?? : lo=?? */
  { 0xd194 , 0xd084 , 0xd194 , None , "\xd1\x94" }, /* ?? : up=?? : lo=?? */
  { 0xd195 , 0xd085 , 0xd195 , None , "\xd1\x95" }, /* ?? : up=?? : lo=?? */
  { 0xd196 , 0xd086 , 0xd196 , None , "\xd1\x96" }, /* ?? : up=?? : lo=?? */
  { 0xd197 , 0xd087 , 0xd197 , None , "\xd1\x97" }, /* ?? : up=?? : lo=?? */
  { 0xd198 , 0xd088 , 0xd198 , None , "\xd1\x98" }, /* ?? : up=?? : lo=?? */
  { 0xd199 , 0xd089 , 0xd199 , None , "\xd1\x99" }, /* ?? : up=?? : lo=?? */
  { 0xd19a , 0xd08a , 0xd19a , None , "\xd1\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xd19b , 0xd08b , 0xd19b , None , "\xd1\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xd19c , 0xd08c , 0xd19c , None , "\xd1\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xd19d , 0xd08d , 0xd19d , None , "\xd1\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xd19e , 0xd08e , 0xd19e , None , "\xd1\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xd19f , 0xd08f , 0xd19f , None , "\xd1\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xd1a0 , 0xd1a0 , 0xd1a1 , None , "\xd1\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xd1a1 , 0xd1a0 , 0xd1a1 , None , "\xd1\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xd1a2 , 0xd1a2 , 0xd1a3 , None , "\xd1\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xd1a3 , 0xd1a2 , 0xd1a3 , None , "\xd1\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xd1a4 , 0xd1a4 , 0xd1a5 , None , "\xd1\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xd1a5 , 0xd1a4 , 0xd1a5 , None , "\xd1\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xd1a6 , 0xd1a6 , 0xd1a7 , None , "\xd1\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xd1a7 , 0xd1a6 , 0xd1a7 , None , "\xd1\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xd1a8 , 0xd1a8 , 0xd1a9 , None , "\xd1\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xd1a9 , 0xd1a8 , 0xd1a9 , None , "\xd1\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xd1aa , 0xd1aa , 0xd1ab , None , "\xd1\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xd1ab , 0xd1aa , 0xd1ab , None , "\xd1\xab" }, /* ?? : up=?? : lo=?? */
  { 0xd1ac , 0xd1ac , 0xd1ad , None , "\xd1\xac" }, /* ?? : up=?? : lo=?? */
  { 0xd1ad , 0xd1ac , 0xd1ad , None , "\xd1\xad" }, /* ?? : up=?? : lo=?? */
  { 0xd1ae , 0xd1ae , 0xd1af , None , "\xd1\xae" }, /* ?? : up=?? : lo=?? */
  { 0xd1af , 0xd1ae , 0xd1af , None , "\xd1\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xd1b0 , 0xd1b0 , 0xd1b1 , None , "\xd1\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xd1b1 , 0xd1b0 , 0xd1b1 , None , "\xd1\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xd1b2 , 0xd1b2 , 0xd1b3 , None , "\xd1\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xd1b3 , 0xd1b2 , 0xd1b3 , None , "\xd1\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xd1b4 , 0xd1b4 , 0xd1b5 , None , "\xd1\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xd1b5 , 0xd1b4 , 0xd1b5 , None , "\xd1\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xd1b6 , 0xd1b6 , 0xd1b7 , None , "\xd1\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xd1b7 , 0xd1b6 , 0xd1b7 , None , "\xd1\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xd1b8 , 0xd1b8 , 0xd1b9 , None , "\xd1\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xd1b9 , 0xd1b8 , 0xd1b9 , None , "\xd1\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xd1ba , 0xd1ba , 0xd1bb , None , "\xd1\xba" }, /* ?? : up=?? : lo=?? */
  { 0xd1bb , 0xd1ba , 0xd1bb , None , "\xd1\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xd1bc , 0xd1bc , 0xd1bd , None , "\xd1\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xd1bd , 0xd1bc , 0xd1bd , None , "\xd1\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xd1be , 0xd1be , 0xd1bf , None , "\xd1\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xd1bf , 0xd1be , 0xd1bf , None , "\xd1\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d2[64] = {
  { 0xd280 , 0xd280 , 0xd281 , None , "\xd2\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd281 , 0xd280 , 0xd281 , None , "\xd2\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd282 , 0xd282 , 0xd282 , None , "\xd2\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd283 , 0xd283 , 0xd283 , IsDiacritic , "\xd2\x83" },
  { 0xd284 , 0xd284 , 0xd284 , IsDiacritic , "\xd2\x84" },
  { 0xd285 , 0xd285 , 0xd285 , IsDiacritic , "\xd2\x85" },
  { 0xd286 , 0xd286 , 0xd286 , IsDiacritic , "\xd2\x86" },
  { 0xd287 , 0xd287 , 0xd287 , IsDiacritic , "\xd2\x87" },
  { 0xd288 , 0xd288 , 0xd288 , IsDiacritic , "\xd2\x88" },
  { 0xd289 , 0xd289 , 0xd289 , IsDiacritic , "\xd2\x89" },
  { 0xd28a , 0xd28a , 0xd28b , None , "\xd2\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xd28b , 0xd28a , 0xd28b , None , "\xd2\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xd28c , 0xd28c , 0xd28d , None , "\xd2\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xd28d , 0xd28c , 0xd28d , None , "\xd2\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xd28e , 0xd28e , 0xd28f , None , "\xd2\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xd28f , 0xd28e , 0xd28f , None , "\xd2\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xd290 , 0xd290 , 0xd291 , None , "\xd2\x90" }, /* ?? : up=?? : lo=?? */
  { 0xd291 , 0xd290 , 0xd291 , None , "\xd2\x91" }, /* ?? : up=?? : lo=?? */
  { 0xd292 , 0xd292 , 0xd293 , None , "\xd2\x92" }, /* ?? : up=?? : lo=?? */
  { 0xd293 , 0xd292 , 0xd293 , None , "\xd2\x93" }, /* ?? : up=?? : lo=?? */
  { 0xd294 , 0xd294 , 0xd295 , None , "\xd2\x94" }, /* ?? : up=?? : lo=?? */
  { 0xd295 , 0xd294 , 0xd295 , None , "\xd2\x95" }, /* ?? : up=?? : lo=?? */
  { 0xd296 , 0xd296 , 0xd297 , None , "\xd2\x96" }, /* ?? : up=?? : lo=?? */
  { 0xd297 , 0xd296 , 0xd297 , None , "\xd2\x97" }, /* ?? : up=?? : lo=?? */
  { 0xd298 , 0xd298 , 0xd299 , None , "\xd2\x98" }, /* ?? : up=?? : lo=?? */
  { 0xd299 , 0xd298 , 0xd299 , None , "\xd2\x99" }, /* ?? : up=?? : lo=?? */
  { 0xd29a , 0xd29a , 0xd29b , None , "\xd2\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xd29b , 0xd29a , 0xd29b , None , "\xd2\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xd29c , 0xd29c , 0xd29d , None , "\xd2\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xd29d , 0xd29c , 0xd29d , None , "\xd2\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xd29e , 0xd29e , 0xd29f , None , "\xd2\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xd29f , 0xd29e , 0xd29f , None , "\xd2\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xd2a0 , 0xd2a0 , 0xd2a1 , None , "\xd2\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xd2a1 , 0xd2a0 , 0xd2a1 , None , "\xd2\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xd2a2 , 0xd2a2 , 0xd2a3 , None , "\xd2\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xd2a3 , 0xd2a2 , 0xd2a3 , None , "\xd2\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xd2a4 , 0xd2a4 , 0xd2a5 , None , "\xd2\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xd2a5 , 0xd2a4 , 0xd2a5 , None , "\xd2\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xd2a6 , 0xd2a6 , 0xd2a7 , None , "\xd2\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xd2a7 , 0xd2a6 , 0xd2a7 , None , "\xd2\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xd2a8 , 0xd2a8 , 0xd2a9 , None , "\xd2\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xd2a9 , 0xd2a8 , 0xd2a9 , None , "\xd2\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xd2aa , 0xd2aa , 0xd2ab , None , "\xd2\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xd2ab , 0xd2aa , 0xd2ab , None , "\xd2\xab" }, /* ?? : up=?? : lo=?? */
  { 0xd2ac , 0xd2ac , 0xd2ad , None , "\xd2\xac" }, /* ?? : up=?? : lo=?? */
  { 0xd2ad , 0xd2ac , 0xd2ad , None , "\xd2\xad" }, /* ?? : up=?? : lo=?? */
  { 0xd2ae , 0xd2ae , 0xd2af , None , "\xd2\xae" }, /* ?? : up=?? : lo=?? */
  { 0xd2af , 0xd2ae , 0xd2af , None , "\xd2\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xd2b0 , 0xd2b0 , 0xd2b1 , None , "\xd2\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xd2b1 , 0xd2b0 , 0xd2b1 , None , "\xd2\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xd2b2 , 0xd2b2 , 0xd2b3 , None , "\xd2\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xd2b3 , 0xd2b2 , 0xd2b3 , None , "\xd2\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xd2b4 , 0xd2b4 , 0xd2b5 , None , "\xd2\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xd2b5 , 0xd2b4 , 0xd2b5 , None , "\xd2\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xd2b6 , 0xd2b6 , 0xd2b7 , None , "\xd2\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xd2b7 , 0xd2b6 , 0xd2b7 , None , "\xd2\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xd2b8 , 0xd2b8 , 0xd2b9 , None , "\xd2\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xd2b9 , 0xd2b8 , 0xd2b9 , None , "\xd2\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xd2ba , 0xd2ba , 0xd2bb , None , "\xd2\xba" }, /* ?? : up=?? : lo=?? */
  { 0xd2bb , 0xd2ba , 0xd2bb , None , "\xd2\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xd2bc , 0xd2bc , 0xd2bd , None , "\xd2\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xd2bd , 0xd2bc , 0xd2bd , None , "\xd2\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xd2be , 0xd2be , 0xd2bf , None , "\xd2\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xd2bf , 0xd2be , 0xd2bf , None , "\xd2\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d3[64] = {
  { 0xd380 , 0xd380 , 0xd38f , None , "\xd3\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd381 , 0xd381 , 0xd382 , None , "\xd3\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd382 , 0xd381 , 0xd382 , None , "\xd3\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd383 , 0xd383 , 0xd384 , None , "\xd3\x83" }, /* ?? : up=?? : lo=?? */
  { 0xd384 , 0xd383 , 0xd384 , None , "\xd3\x84" }, /* ?? : up=?? : lo=?? */
  { 0xd385 , 0xd385 , 0xd386 , None , "\xd3\x85" }, /* ?? : up=?? : lo=?? */
  { 0xd386 , 0xd385 , 0xd386 , None , "\xd3\x86" }, /* ?? : up=?? : lo=?? */
  { 0xd387 , 0xd387 , 0xd388 , None , "\xd3\x87" }, /* ?? : up=?? : lo=?? */
  { 0xd388 , 0xd387 , 0xd388 , None , "\xd3\x88" }, /* ?? : up=?? : lo=?? */
  { 0xd389 , 0xd389 , 0xd38a , None , "\xd3\x89" }, /* ?? : up=?? : lo=?? */
  { 0xd38a , 0xd389 , 0xd38a , None , "\xd3\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xd38b , 0xd38b , 0xd38c , None , "\xd3\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xd38c , 0xd38b , 0xd38c , None , "\xd3\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xd38d , 0xd38d , 0xd38e , None , "\xd3\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xd38e , 0xd38d , 0xd38e , None , "\xd3\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xd38f , 0xd380 , 0xd38f , None , "\xd3\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xd390 , 0xd390 , 0xd391 , None , "\xd3\x90" }, /* ?? : up=?? : lo=?? */
  { 0xd391 , 0xd390 , 0xd391 , None , "\xd3\x91" }, /* ?? : up=?? : lo=?? */
  { 0xd392 , 0xd392 , 0xd393 , None , "\xd3\x92" }, /* ?? : up=?? : lo=?? */
  { 0xd393 , 0xd392 , 0xd393 , None , "\xd3\x93" }, /* ?? : up=?? : lo=?? */
  { 0xd394 , 0xd394 , 0xd395 , None , "\xd3\x94" }, /* ?? : up=?? : lo=?? */
  { 0xd395 , 0xd394 , 0xd395 , None , "\xd3\x95" }, /* ?? : up=?? : lo=?? */
  { 0xd396 , 0xd396 , 0xd397 , None , "\xd3\x96" }, /* ?? : up=?? : lo=?? */
  { 0xd397 , 0xd396 , 0xd397 , None , "\xd3\x97" }, /* ?? : up=?? : lo=?? */
  { 0xd398 , 0xd398 , 0xd399 , None , "\xd3\x98" }, /* ?? : up=?? : lo=?? */
  { 0xd399 , 0xd398 , 0xd399 , None , "\xd3\x99" }, /* ?? : up=?? : lo=?? */
  { 0xd39a , 0xd39a , 0xd39b , None , "\xd3\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xd39b , 0xd39a , 0xd39b , None , "\xd3\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xd39c , 0xd39c , 0xd39d , None , "\xd3\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xd39d , 0xd39c , 0xd39d , None , "\xd3\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xd39e , 0xd39e , 0xd39f , None , "\xd3\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xd39f , 0xd39e , 0xd39f , None , "\xd3\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xd3a0 , 0xd3a0 , 0xd3a1 , None , "\xd3\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xd3a1 , 0xd3a0 , 0xd3a1 , None , "\xd3\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xd3a2 , 0xd3a2 , 0xd3a3 , None , "\xd3\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xd3a3 , 0xd3a2 , 0xd3a3 , None , "\xd3\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xd3a4 , 0xd3a4 , 0xd3a5 , None , "\xd3\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xd3a5 , 0xd3a4 , 0xd3a5 , None , "\xd3\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xd3a6 , 0xd3a6 , 0xd3a7 , None , "\xd3\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xd3a7 , 0xd3a6 , 0xd3a7 , None , "\xd3\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xd3a8 , 0xd3a8 , 0xd3a9 , None , "\xd3\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xd3a9 , 0xd3a8 , 0xd3a9 , None , "\xd3\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xd3aa , 0xd3aa , 0xd3ab , None , "\xd3\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xd3ab , 0xd3aa , 0xd3ab , None , "\xd3\xab" }, /* ?? : up=?? : lo=?? */
  { 0xd3ac , 0xd3ac , 0xd3ad , None , "\xd3\xac" }, /* ?? : up=?? : lo=?? */
  { 0xd3ad , 0xd3ac , 0xd3ad , None , "\xd3\xad" }, /* ?? : up=?? : lo=?? */
  { 0xd3ae , 0xd3ae , 0xd3af , None , "\xd3\xae" }, /* ?? : up=?? : lo=?? */
  { 0xd3af , 0xd3ae , 0xd3af , None , "\xd3\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xd3b0 , 0xd3b0 , 0xd3b1 , None , "\xd3\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xd3b1 , 0xd3b0 , 0xd3b1 , None , "\xd3\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xd3b2 , 0xd3b2 , 0xd3b3 , None , "\xd3\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xd3b3 , 0xd3b2 , 0xd3b3 , None , "\xd3\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xd3b4 , 0xd3b4 , 0xd3b5 , None , "\xd3\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xd3b5 , 0xd3b4 , 0xd3b5 , None , "\xd3\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xd3b6 , 0xd3b6 , 0xd3b7 , None , "\xd3\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xd3b7 , 0xd3b6 , 0xd3b7 , None , "\xd3\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xd3b8 , 0xd3b8 , 0xd3b9 , None , "\xd3\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xd3b9 , 0xd3b8 , 0xd3b9 , None , "\xd3\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xd3ba , 0xd3ba , 0xd3bb , None , "\xd3\xba" }, /* ?? : up=?? : lo=?? */
  { 0xd3bb , 0xd3ba , 0xd3bb , None , "\xd3\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xd3bc , 0xd3bc , 0xd3bd , None , "\xd3\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xd3bd , 0xd3bc , 0xd3bd , None , "\xd3\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xd3be , 0xd3be , 0xd3bf , None , "\xd3\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xd3bf , 0xd3be , 0xd3bf , None , "\xd3\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d4[64] = {
  { 0xd480 , 0xd480 , 0xd481 , None , "\xd4\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd481 , 0xd480 , 0xd481 , None , "\xd4\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd482 , 0xd482 , 0xd483 , None , "\xd4\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd483 , 0xd482 , 0xd483 , None , "\xd4\x83" }, /* ?? : up=?? : lo=?? */
  { 0xd484 , 0xd484 , 0xd485 , None , "\xd4\x84" }, /* ?? : up=?? : lo=?? */
  { 0xd485 , 0xd484 , 0xd485 , None , "\xd4\x85" }, /* ?? : up=?? : lo=?? */
  { 0xd486 , 0xd486 , 0xd487 , None , "\xd4\x86" }, /* ?? : up=?? : lo=?? */
  { 0xd487 , 0xd486 , 0xd487 , None , "\xd4\x87" }, /* ?? : up=?? : lo=?? */
  { 0xd488 , 0xd488 , 0xd489 , None , "\xd4\x88" }, /* ?? : up=?? : lo=?? */
  { 0xd489 , 0xd488 , 0xd489 , None , "\xd4\x89" }, /* ?? : up=?? : lo=?? */
  { 0xd48a , 0xd48a , 0xd48b , None , "\xd4\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xd48b , 0xd48a , 0xd48b , None , "\xd4\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xd48c , 0xd48c , 0xd48d , None , "\xd4\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xd48d , 0xd48c , 0xd48d , None , "\xd4\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xd48e , 0xd48e , 0xd48f , None , "\xd4\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xd48f , 0xd48e , 0xd48f , None , "\xd4\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xd490 , 0xd490 , 0xd491 , None , "\xd4\x90" }, /* ?? : up=?? : lo=?? */
  { 0xd491 , 0xd490 , 0xd491 , None , "\xd4\x91" }, /* ?? : up=?? : lo=?? */
  { 0xd492 , 0xd492 , 0xd493 , None , "\xd4\x92" }, /* ?? : up=?? : lo=?? */
  { 0xd493 , 0xd492 , 0xd493 , None , "\xd4\x93" }, /* ?? : up=?? : lo=?? */
  { 0xd494 , 0xd494 , 0xd495 , None , "\xd4\x94" }, /* ?? : up=?? : lo=?? */
  { 0xd495 , 0xd494 , 0xd495 , None , "\xd4\x95" }, /* ?? : up=?? : lo=?? */
  { 0xd496 , 0xd496 , 0xd497 , None , "\xd4\x96" }, /* ?? : up=?? : lo=?? */
  { 0xd497 , 0xd496 , 0xd497 , None , "\xd4\x97" }, /* ?? : up=?? : lo=?? */
  { 0xd498 , 0xd498 , 0xd499 , None , "\xd4\x98" }, /* ?? : up=?? : lo=?? */
  { 0xd499 , 0xd498 , 0xd499 , None , "\xd4\x99" }, /* ?? : up=?? : lo=?? */
  { 0xd49a , 0xd49a , 0xd49b , None , "\xd4\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xd49b , 0xd49a , 0xd49b , None , "\xd4\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xd49c , 0xd49c , 0xd49d , None , "\xd4\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xd49d , 0xd49c , 0xd49d , None , "\xd4\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xd49e , 0xd49e , 0xd49f , None , "\xd4\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xd49f , 0xd49e , 0xd49f , None , "\xd4\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xd4a0 , 0xd4a0 , 0xd4a1 , None , "\xd4\xa0" }, /* ?? : up=?? : lo=?? */
  { 0xd4a1 , 0xd4a0 , 0xd4a1 , None , "\xd4\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xd4a2 , 0xd4a2 , 0xd4a3 , None , "\xd4\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xd4a3 , 0xd4a2 , 0xd4a3 , None , "\xd4\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xd4a4 , 0xd4a4 , 0xd4a5 , None , "\xd4\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xd4a5 , 0xd4a4 , 0xd4a5 , None , "\xd4\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xd4a6 , 0xd4a6 , 0xd4a7 , None , "\xd4\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xd4a7 , 0xd4a6 , 0xd4a7 , None , "\xd4\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xd4a8 , 0xd4a8 , 0xd4a9 , None , "\xd4\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xd4a9 , 0xd4a8 , 0xd4a9 , None , "\xd4\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xd4aa , 0xd4aa , 0xd4ab , None , "\xd4\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xd4ab , 0xd4aa , 0xd4ab , None , "\xd4\xab" }, /* ?? : up=?? : lo=?? */
  { 0xd4ac , 0xd4ac , 0xd4ad , None , "\xd4\xac" }, /* ?? : up=?? : lo=?? */
  { 0xd4ad , 0xd4ac , 0xd4ad , None , "\xd4\xad" }, /* ?? : up=?? : lo=?? */
  { 0xd4ae , 0xd4ae , 0xd4af , None , "\xd4\xae" }, /* ?? : up=?? : lo=?? */
  { 0xd4af , 0xd4ae , 0xd4af , None , "\xd4\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xd4b0 , 0xd4b0 , 0xd4b0 , None , "\xd4\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xd4b1 , 0xd4b1 , 0xd5a1 , None , "\xd4\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xd4b2 , 0xd4b2 , 0xd5a2 , None , "\xd4\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xd4b3 , 0xd4b3 , 0xd5a3 , None , "\xd4\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xd4b4 , 0xd4b4 , 0xd5a4 , None , "\xd4\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xd4b5 , 0xd4b5 , 0xd5a5 , None , "\xd4\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xd4b6 , 0xd4b6 , 0xd5a6 , None , "\xd4\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xd4b7 , 0xd4b7 , 0xd5a7 , None , "\xd4\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xd4b8 , 0xd4b8 , 0xd5a8 , None , "\xd4\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xd4b9 , 0xd4b9 , 0xd5a9 , None , "\xd4\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xd4ba , 0xd4ba , 0xd5aa , None , "\xd4\xba" }, /* ?? : up=?? : lo=?? */
  { 0xd4bb , 0xd4bb , 0xd5ab , None , "\xd4\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xd4bc , 0xd4bc , 0xd5ac , None , "\xd4\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xd4bd , 0xd4bd , 0xd5ad , None , "\xd4\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xd4be , 0xd4be , 0xd5ae , None , "\xd4\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xd4bf , 0xd4bf , 0xd5af , None , "\xd4\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d5[64] = {
  { 0xd580 , 0xd580 , 0xd5b0 , None , "\xd5\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd581 , 0xd581 , 0xd5b1 , None , "\xd5\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd582 , 0xd582 , 0xd5b2 , None , "\xd5\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd583 , 0xd583 , 0xd5b3 , None , "\xd5\x83" }, /* ?? : up=?? : lo=?? */
  { 0xd584 , 0xd584 , 0xd5b4 , None , "\xd5\x84" }, /* ?? : up=?? : lo=?? */
  { 0xd585 , 0xd585 , 0xd5b5 , None , "\xd5\x85" }, /* ?? : up=?? : lo=?? */
  { 0xd586 , 0xd586 , 0xd5b6 , None , "\xd5\x86" }, /* ?? : up=?? : lo=?? */
  { 0xd587 , 0xd587 , 0xd5b7 , None , "\xd5\x87" }, /* ?? : up=?? : lo=?? */
  { 0xd588 , 0xd588 , 0xd5b8 , None , "\xd5\x88" }, /* ?? : up=?? : lo=?? */
  { 0xd589 , 0xd589 , 0xd5b9 , None , "\xd5\x89" }, /* ?? : up=?? : lo=?? */
  { 0xd58a , 0xd58a , 0xd5ba , None , "\xd5\x8a" }, /* ?? : up=?? : lo=?? */
  { 0xd58b , 0xd58b , 0xd5bb , None , "\xd5\x8b" }, /* ?? : up=?? : lo=?? */
  { 0xd58c , 0xd58c , 0xd5bc , None , "\xd5\x8c" }, /* ?? : up=?? : lo=?? */
  { 0xd58d , 0xd58d , 0xd5bd , None , "\xd5\x8d" }, /* ?? : up=?? : lo=?? */
  { 0xd58e , 0xd58e , 0xd5be , None , "\xd5\x8e" }, /* ?? : up=?? : lo=?? */
  { 0xd58f , 0xd58f , 0xd5bf , None , "\xd5\x8f" }, /* ?? : up=?? : lo=?? */
  { 0xd590 , 0xd590 , 0xd680 , None , "\xd5\x90" }, /* ?? : up=?? : lo=?? */
  { 0xd591 , 0xd591 , 0xd681 , None , "\xd5\x91" }, /* ?? : up=?? : lo=?? */
  { 0xd592 , 0xd592 , 0xd682 , None , "\xd5\x92" }, /* ?? : up=?? : lo=?? */
  { 0xd593 , 0xd593 , 0xd683 , None , "\xd5\x93" }, /* ?? : up=?? : lo=?? */
  { 0xd594 , 0xd594 , 0xd684 , None , "\xd5\x94" }, /* ?? : up=?? : lo=?? */
  { 0xd595 , 0xd595 , 0xd685 , None , "\xd5\x95" }, /* ?? : up=?? : lo=?? */
  { 0xd596 , 0xd596 , 0xd686 , None , "\xd5\x96" }, /* ?? : up=?? : lo=?? */
  { 0xd597 , 0xd597 , 0xd597 , None , "\xd5\x97" },
  { 0xd598 , 0xd598 , 0xd598 , None , "\xd5\x98" },
  { 0xd599 , 0xd599 , 0xd599 , None , "\xd5\x99" }, /* ?? : up=?? : lo=?? */
  { 0xd59a , 0xd59a , 0xd59a , None , "\xd5\x9a" }, /* ?? : up=?? : lo=?? */
  { 0xd59b , 0xd59b , 0xd59b , None , "\xd5\x9b" }, /* ?? : up=?? : lo=?? */
  { 0xd59c , 0xd59c , 0xd59c , None , "\xd5\x9c" }, /* ?? : up=?? : lo=?? */
  { 0xd59d , 0xd59d , 0xd59d , None , "\xd5\x9d" }, /* ?? : up=?? : lo=?? */
  { 0xd59e , 0xd59e , 0xd59e , None , "\xd5\x9e" }, /* ?? : up=?? : lo=?? */
  { 0xd59f , 0xd59f , 0xd59f , None , "\xd5\x9f" }, /* ?? : up=?? : lo=?? */
  { 0xd5a0 , 0xd5a0 , 0xd5a0 , None , "\xd5\xa0" },
  { 0xd5a1 , 0xd4b1 , 0xd5a1 , None , "\xd5\xa1" }, /* ?? : up=?? : lo=?? */
  { 0xd5a2 , 0xd4b2 , 0xd5a2 , None , "\xd5\xa2" }, /* ?? : up=?? : lo=?? */
  { 0xd5a3 , 0xd4b3 , 0xd5a3 , None , "\xd5\xa3" }, /* ?? : up=?? : lo=?? */
  { 0xd5a4 , 0xd4b4 , 0xd5a4 , None , "\xd5\xa4" }, /* ?? : up=?? : lo=?? */
  { 0xd5a5 , 0xd4b5 , 0xd5a5 , None , "\xd5\xa5" }, /* ?? : up=?? : lo=?? */
  { 0xd5a6 , 0xd4b6 , 0xd5a6 , None , "\xd5\xa6" }, /* ?? : up=?? : lo=?? */
  { 0xd5a7 , 0xd4b7 , 0xd5a7 , None , "\xd5\xa7" }, /* ?? : up=?? : lo=?? */
  { 0xd5a8 , 0xd4b8 , 0xd5a8 , None , "\xd5\xa8" }, /* ?? : up=?? : lo=?? */
  { 0xd5a9 , 0xd4b9 , 0xd5a9 , None , "\xd5\xa9" }, /* ?? : up=?? : lo=?? */
  { 0xd5aa , 0xd4ba , 0xd5aa , None , "\xd5\xaa" }, /* ?? : up=?? : lo=?? */
  { 0xd5ab , 0xd4bb , 0xd5ab , None , "\xd5\xab" }, /* ?? : up=?? : lo=?? */
  { 0xd5ac , 0xd4bc , 0xd5ac , None , "\xd5\xac" }, /* ?? : up=?? : lo=?? */
  { 0xd5ad , 0xd4bd , 0xd5ad , None , "\xd5\xad" }, /* ?? : up=?? : lo=?? */
  { 0xd5ae , 0xd4be , 0xd5ae , None , "\xd5\xae" }, /* ?? : up=?? : lo=?? */
  { 0xd5af , 0xd4bf , 0xd5af , None , "\xd5\xaf" }, /* ?? : up=?? : lo=?? */
  { 0xd5b0 , 0xd580 , 0xd5b0 , None , "\xd5\xb0" }, /* ?? : up=?? : lo=?? */
  { 0xd5b1 , 0xd581 , 0xd5b1 , None , "\xd5\xb1" }, /* ?? : up=?? : lo=?? */
  { 0xd5b2 , 0xd582 , 0xd5b2 , None , "\xd5\xb2" }, /* ?? : up=?? : lo=?? */
  { 0xd5b3 , 0xd583 , 0xd5b3 , None , "\xd5\xb3" }, /* ?? : up=?? : lo=?? */
  { 0xd5b4 , 0xd584 , 0xd5b4 , None , "\xd5\xb4" }, /* ?? : up=?? : lo=?? */
  { 0xd5b5 , 0xd585 , 0xd5b5 , None , "\xd5\xb5" }, /* ?? : up=?? : lo=?? */
  { 0xd5b6 , 0xd586 , 0xd5b6 , None , "\xd5\xb6" }, /* ?? : up=?? : lo=?? */
  { 0xd5b7 , 0xd587 , 0xd5b7 , None , "\xd5\xb7" }, /* ?? : up=?? : lo=?? */
  { 0xd5b8 , 0xd588 , 0xd5b8 , None , "\xd5\xb8" }, /* ?? : up=?? : lo=?? */
  { 0xd5b9 , 0xd589 , 0xd5b9 , None , "\xd5\xb9" }, /* ?? : up=?? : lo=?? */
  { 0xd5ba , 0xd58a , 0xd5ba , None , "\xd5\xba" }, /* ?? : up=?? : lo=?? */
  { 0xd5bb , 0xd58b , 0xd5bb , None , "\xd5\xbb" }, /* ?? : up=?? : lo=?? */
  { 0xd5bc , 0xd58c , 0xd5bc , None , "\xd5\xbc" }, /* ?? : up=?? : lo=?? */
  { 0xd5bd , 0xd58d , 0xd5bd , None , "\xd5\xbd" }, /* ?? : up=?? : lo=?? */
  { 0xd5be , 0xd58e , 0xd5be , None , "\xd5\xbe" }, /* ?? : up=?? : lo=?? */
  { 0xd5bf , 0xd58f , 0xd5bf , None , "\xd5\xbf" }, /* ?? : up=?? : lo=?? */
};

const character charmap_d6[64] = {
  { 0xd680 , 0xd590 , 0xd680 , None , "\xd6\x80" }, /* ?? : up=?? : lo=?? */
  { 0xd681 , 0xd591 , 0xd681 , None , "\xd6\x81" }, /* ?? : up=?? : lo=?? */
  { 0xd682 , 0xd592 , 0xd682 , None , "\xd6\x82" }, /* ?? : up=?? : lo=?? */
  { 0xd683 , 0xd593 , 0xd683 , None , "\xd6\x83" }, /* ?? : up=?? : lo=?? */
  { 0xd684 , 0xd594 , 0xd684 , None , "\xd6\x84" }, /* ?? : up=?? : lo=?? */
  { 0xd685 , 0xd595 , 0xd685 , None , "\xd6\x85" }, /* ?? : up=?? : lo=?? */
  { 0xd686 , 0xd596 , 0xd686 , None , "\xd6\x86" }, /* ?? : up=?? : lo=?? */
  { 0xd687 , 0xd687 , 0xd687 , None , "\xd6\x87" }, /* ?? : up=?? : lo=?? */
  { 0xd688 , 0xd688 , 0xd688 , None , "\xd6\x88" },
  { 0xd689 , 0xd689 , 0xd689 , None , "\xd6\x89" },
  { 0xd68a , 0xd68a , 0xd68a , None , "\xd6\x8a" },
  { 0xd68b , 0xd68b , 0xd68b , None , "\xd6\x8b" },
  { 0xd68c , 0xd68c , 0xd68c , None , "\xd6\x8c" },
  { 0xd68d , 0xd68d , 0xd68d , None , "\xd6\x8d" },
  { 0xd68e , 0xd68e , 0xd68e , None , "\xd6\x8e" },
  { 0xd68f , 0xd68f , 0xd68f , None , "\xd6\x8f" },
  { 0xd690 , 0xd690 , 0xd690 , None , "\xd6\x90" },
  { 0xd691 , 0xd691 , 0xd691 , None , "\xd6\x91" },
  { 0xd692 , 0xd692 , 0xd692 , None , "\xd6\x92" },
  { 0xd693 , 0xd693 , 0xd693 , None , "\xd6\x93" },
  { 0xd694 , 0xd694 , 0xd694 , None , "\xd6\x94" },
  { 0xd695 , 0xd695 , 0xd695 , None , "\xd6\x95" },
  { 0xd696 , 0xd696 , 0xd696 , None , "\xd6\x96" },
  { 0xd697 , 0xd697 , 0xd697 , None , "\xd6\x97" },
  { 0xd698 , 0xd698 , 0xd698 , None , "\xd6\x98" },
  { 0xd699 , 0xd699 , 0xd699 , None , "\xd6\x99" },
  { 0xd69a , 0xd69a , 0xd69a , None , "\xd6\x9a" },
  { 0xd69b , 0xd69b , 0xd69b , None , "\xd6\x9b" },
  { 0xd69c , 0xd69c , 0xd69c , None , "\xd6\x9c" },
  { 0xd69d , 0xd69d , 0xd69d , None , "\xd6\x9d" },
  { 0xd69e , 0xd69e , 0xd69e , None , "\xd6\x9e" },
  { 0xd69f , 0xd69f , 0xd69f , None , "\xd6\x9f" },
  { 0xd6a0 , 0xd6a0 , 0xd6a0 , None , "\xd6\xa0" },
  { 0xd6a1 , 0xd6a1 , 0xd6a1 , None , "\xd6\xa1" },
  { 0xd6a2 , 0xd6a2 , 0xd6a2 , None , "\xd6\xa2" },
  { 0xd6a3 , 0xd6a3 , 0xd6a3 , None , "\xd6\xa3" },
  { 0xd6a4 , 0xd6a4 , 0xd6a4 , None , "\xd6\xa4" },
  { 0xd6a5 , 0xd6a5 , 0xd6a5 , None , "\xd6\xa5" },
  { 0xd6a6 , 0xd6a6 , 0xd6a6 , None , "\xd6\xa6" },
  { 0xd6a7 , 0xd6a7 , 0xd6a7 , None , "\xd6\xa7" },
  { 0xd6a8 , 0xd6a8 , 0xd6a8 , None , "\xd6\xa8" },
  { 0xd6a9 , 0xd6a9 , 0xd6a9 , None , "\xd6\xa9" },
  { 0xd6aa , 0xd6aa , 0xd6aa , None , "\xd6\xaa" },
  { 0xd6ab , 0xd6ab , 0xd6ab , None , "\xd6\xab" },
  { 0xd6ac , 0xd6ac , 0xd6ac , None , "\xd6\xac" },
  { 0xd6ad , 0xd6ad , 0xd6ad , None , "\xd6\xad" },
  { 0xd6ae , 0xd6ae , 0xd6ae , None , "\xd6\xae" },
  { 0xd6af , 0xd6af , 0xd6af , None , "\xd6\xaf" },
  { 0xd6b0 , 0xd6b0 , 0xd6b0 , None , "\xd6\xb0" },
  { 0xd6b1 , 0xd6b1 , 0xd6b1 , None , "\xd6\xb1" },
  { 0xd6b2 , 0xd6b2 , 0xd6b2 , None , "\xd6\xb2" },
  { 0xd6b3 , 0xd6b3 , 0xd6b3 , None , "\xd6\xb3" },
  { 0xd6b4 , 0xd6b4 , 0xd6b4 , None , "\xd6\xb4" },
  { 0xd6b5 , 0xd6b5 , 0xd6b5 , None , "\xd6\xb5" },
  { 0xd6b6 , 0xd6b6 , 0xd6b6 , None , "\xd6\xb6" },
  { 0xd6b7 , 0xd6b7 , 0xd6b7 , None , "\xd6\xb7" },
  { 0xd6b8 , 0xd6b8 , 0xd6b8 , None , "\xd6\xb8" },
  { 0xd6b9 , 0xd6b9 , 0xd6b9 , None , "\xd6\xb9" },
  { 0xd6ba , 0xd6ba , 0xd6ba , None , "\xd6\xba" },
  { 0xd6bb , 0xd6bb , 0xd6bb , None , "\xd6\xbb" },
  { 0xd6bc , 0xd6bc , 0xd6bc , None , "\xd6\xbc" },
  { 0xd6bd , 0xd6bd , 0xd6bd , None , "\xd6\xbd" },
  { 0xd6be , 0xd6be , 0xd6be , None , "\xd6\xbe" },
  { 0xd6bf , 0xd6bf , 0xd6bf , None , "\xd6\xbf" },
};

const character* pagemap_24_e1[64] = {
  /* 80 */
  nullptr, nullptr, charmap_e1_82, charmap_e1_83, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  charmap_e1_b8, charmap_e1_b9, charmap_e1_ba, charmap_e1_bb, charmap_e1_bc, charmap_e1_bd, charmap_e1_be, charmap_e1_bf,
};

const character charmap_e1_82[64] = {
  { 0xe18280 , 0xe18280 , 0xe18280 , None , "\xe1\x82\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe18281 , 0xe18281 , 0xe18281 , None , "\xe1\x82\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe18282 , 0xe18282 , 0xe18282 , None , "\xe1\x82\x82" },
  { 0xe18283 , 0xe18283 , 0xe18283 , None , "\xe1\x82\x83" },
  { 0xe18284 , 0xe18284 , 0xe18284 , None , "\xe1\x82\x84" },
  { 0xe18285 , 0xe18285 , 0xe18285 , None , "\xe1\x82\x85" },
  { 0xe18286 , 0xe18286 , 0xe18286 , None , "\xe1\x82\x86" },
  { 0xe18287 , 0xe18287 , 0xe18287 , None , "\xe1\x82\x87" },
  { 0xe18288 , 0xe18288 , 0xe18288 , None , "\xe1\x82\x88" },
  { 0xe18289 , 0xe18289 , 0xe18289 , None , "\xe1\x82\x89" },
  { 0xe1828a , 0xe1828a , 0xe1828a , None , "\xe1\x82\x8a" },
  { 0xe1828b , 0xe1828b , 0xe1828b , None , "\xe1\x82\x8b" },
  { 0xe1828c , 0xe1828c , 0xe1828c , None , "\xe1\x82\x8c" },
  { 0xe1828d , 0xe1828d , 0xe1828d , None , "\xe1\x82\x8d" },
  { 0xe1828e , 0xe1828e , 0xe1828e , None , "\xe1\x82\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1828f , 0xe1828f , 0xe1828f , None , "\xe1\x82\x8f" },
  { 0xe18290 , 0xe18290 , 0xe18290 , None , "\xe1\x82\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe18291 , 0xe18291 , 0xe18291 , None , "\xe1\x82\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe18292 , 0xe18292 , 0xe18292 , None , "\xe1\x82\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe18293 , 0xe18293 , 0xe18293 , None , "\xe1\x82\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe18294 , 0xe18294 , 0xe18294 , None , "\xe1\x82\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe18295 , 0xe18295 , 0xe18295 , None , "\xe1\x82\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe18296 , 0xe18296 , 0xe18296 , None , "\xe1\x82\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe18297 , 0xe18297 , 0xe18297 , None , "\xe1\x82\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe18298 , 0xe18298 , 0xe18298 , None , "\xe1\x82\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe18299 , 0xe18299 , 0xe18299 , None , "\xe1\x82\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1829a , 0xe1829a , 0xe1829a , None , "\xe1\x82\x9a" },
  { 0xe1829b , 0xe1829b , 0xe1829b , None , "\xe1\x82\x9b" },
  { 0xe1829c , 0xe1829c , 0xe1829c , None , "\xe1\x82\x9c" },
  { 0xe1829d , 0xe1829d , 0xe1829d , None , "\xe1\x82\x9d" },
  { 0xe1829e , 0xe1829e , 0xe1829e , None , "\xe1\x82\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1829f , 0xe1829f , 0xe1829f , None , "\xe1\x82\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe182a0 , 0xe182a0 , 0xe2b480 , None , "\xe1\x82\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe182a1 , 0xe182a1 , 0xe2b481 , None , "\xe1\x82\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe182a2 , 0xe182a2 , 0xe2b482 , None , "\xe1\x82\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe182a3 , 0xe182a3 , 0xe2b483 , None , "\xe1\x82\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe182a4 , 0xe182a4 , 0xe2b484 , None , "\xe1\x82\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe182a5 , 0xe182a5 , 0xe2b485 , None , "\xe1\x82\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe182a6 , 0xe182a6 , 0xe2b486 , None , "\xe1\x82\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe182a7 , 0xe182a7 , 0xe2b487 , None , "\xe1\x82\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe182a8 , 0xe182a8 , 0xe2b488 , None , "\xe1\x82\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe182a9 , 0xe182a9 , 0xe2b489 , None , "\xe1\x82\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe182aa , 0xe182aa , 0xe2b48a , None , "\xe1\x82\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe182ab , 0xe182ab , 0xe2b48b , None , "\xe1\x82\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe182ac , 0xe182ac , 0xe2b48c , None , "\xe1\x82\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe182ad , 0xe182ad , 0xe2b48d , None , "\xe1\x82\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe182ae , 0xe182ae , 0xe2b48e , None , "\xe1\x82\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe182af , 0xe182af , 0xe2b48f , None , "\xe1\x82\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe182b0 , 0xe182b0 , 0xe2b490 , None , "\xe1\x82\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe182b1 , 0xe182b1 , 0xe2b491 , None , "\xe1\x82\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe182b2 , 0xe182b2 , 0xe2b492 , None , "\xe1\x82\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe182b3 , 0xe182b3 , 0xe2b493 , None , "\xe1\x82\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe182b4 , 0xe182b4 , 0xe2b494 , None , "\xe1\x82\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe182b5 , 0xe182b5 , 0xe2b495 , None , "\xe1\x82\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe182b6 , 0xe182b6 , 0xe2b496 , None , "\xe1\x82\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe182b7 , 0xe182b7 , 0xe2b497 , None , "\xe1\x82\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe182b8 , 0xe182b8 , 0xe2b498 , None , "\xe1\x82\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe182b9 , 0xe182b9 , 0xe2b499 , None , "\xe1\x82\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe182ba , 0xe182ba , 0xe2b49a , None , "\xe1\x82\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe182bb , 0xe182bb , 0xe2b49b , None , "\xe1\x82\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe182bc , 0xe182bc , 0xe2b49c , None , "\xe1\x82\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe182bd , 0xe182bd , 0xe2b49d , None , "\xe1\x82\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe182be , 0xe182be , 0xe2b49e , None , "\xe1\x82\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe182bf , 0xe182bf , 0xe2b49f , None , "\xe1\x82\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e1_83[64] = {
  { 0xe18380 , 0xe18380 , 0xe2b4a0 , None , "\xe1\x83\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe18381 , 0xe18381 , 0xe2b4a1 , None , "\xe1\x83\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe18382 , 0xe18382 , 0xe2b4a2 , None , "\xe1\x83\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe18383 , 0xe18383 , 0xe2b4a3 , None , "\xe1\x83\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe18384 , 0xe18384 , 0xe2b4a4 , None , "\xe1\x83\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe18385 , 0xe18385 , 0xe2b4a5 , None , "\xe1\x83\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe18386 , 0xe18386 , 0xe18386 , None , "\xe1\x83\x86" },
  { 0xe18387 , 0xe18387 , 0xe2b4a7 , None , "\xe1\x83\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe18388 , 0xe18388 , 0xe18388 , None , "\xe1\x83\x88" },
  { 0xe18389 , 0xe18389 , 0xe18389 , None , "\xe1\x83\x89" },
  { 0xe1838a , 0xe1838a , 0xe1838a , None , "\xe1\x83\x8a" },
  { 0xe1838b , 0xe1838b , 0xe1838b , None , "\xe1\x83\x8b" },
  { 0xe1838c , 0xe1838c , 0xe1838c , None , "\xe1\x83\x8c" },
  { 0xe1838d , 0xe1838d , 0xe2b4ad , None , "\xe1\x83\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1838e , 0xe1838e , 0xe1838e , None , "\xe1\x83\x8e" },
  { 0xe1838f , 0xe1838f , 0xe1838f , None , "\xe1\x83\x8f" },
  { 0xe18390 , 0xe18390 , 0xe18390 , None , "\xe1\x83\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe18391 , 0xe18391 , 0xe18391 , None , "\xe1\x83\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe18392 , 0xe18392 , 0xe18392 , None , "\xe1\x83\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe18393 , 0xe18393 , 0xe18393 , None , "\xe1\x83\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe18394 , 0xe18394 , 0xe18394 , None , "\xe1\x83\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe18395 , 0xe18395 , 0xe18395 , None , "\xe1\x83\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe18396 , 0xe18396 , 0xe18396 , None , "\xe1\x83\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe18397 , 0xe18397 , 0xe18397 , None , "\xe1\x83\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe18398 , 0xe18398 , 0xe18398 , None , "\xe1\x83\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe18399 , 0xe18399 , 0xe18399 , None , "\xe1\x83\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1839a , 0xe1839a , 0xe1839a , None , "\xe1\x83\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1839b , 0xe1839b , 0xe1839b , None , "\xe1\x83\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1839c , 0xe1839c , 0xe1839c , None , "\xe1\x83\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1839d , 0xe1839d , 0xe1839d , None , "\xe1\x83\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1839e , 0xe1839e , 0xe1839e , None , "\xe1\x83\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1839f , 0xe1839f , 0xe1839f , None , "\xe1\x83\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe183a0 , 0xe183a0 , 0xe183a0 , None , "\xe1\x83\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe183a1 , 0xe183a1 , 0xe183a1 , None , "\xe1\x83\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe183a2 , 0xe183a2 , 0xe183a2 , None , "\xe1\x83\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe183a3 , 0xe183a3 , 0xe183a3 , None , "\xe1\x83\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe183a4 , 0xe183a4 , 0xe183a4 , None , "\xe1\x83\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe183a5 , 0xe183a5 , 0xe183a5 , None , "\xe1\x83\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe183a6 , 0xe183a6 , 0xe183a6 , None , "\xe1\x83\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe183a7 , 0xe183a7 , 0xe183a7 , None , "\xe1\x83\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe183a8 , 0xe183a8 , 0xe183a8 , None , "\xe1\x83\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe183a9 , 0xe183a9 , 0xe183a9 , None , "\xe1\x83\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe183aa , 0xe183aa , 0xe183aa , None , "\xe1\x83\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe183ab , 0xe183ab , 0xe183ab , None , "\xe1\x83\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe183ac , 0xe183ac , 0xe183ac , None , "\xe1\x83\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe183ad , 0xe183ad , 0xe183ad , None , "\xe1\x83\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe183ae , 0xe183ae , 0xe183ae , None , "\xe1\x83\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe183af , 0xe183af , 0xe183af , None , "\xe1\x83\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe183b0 , 0xe183b0 , 0xe183b0 , None , "\xe1\x83\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe183b1 , 0xe183b1 , 0xe183b1 , None , "\xe1\x83\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe183b2 , 0xe183b2 , 0xe183b2 , None , "\xe1\x83\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe183b3 , 0xe183b3 , 0xe183b3 , None , "\xe1\x83\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe183b4 , 0xe183b4 , 0xe183b4 , None , "\xe1\x83\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe183b5 , 0xe183b5 , 0xe183b5 , None , "\xe1\x83\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe183b6 , 0xe183b6 , 0xe183b6 , None , "\xe1\x83\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe183b7 , 0xe183b7 , 0xe183b7 , None , "\xe1\x83\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe183b8 , 0xe183b8 , 0xe183b8 , None , "\xe1\x83\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe183b9 , 0xe183b9 , 0xe183b9 , None , "\xe1\x83\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe183ba , 0xe183ba , 0xe183ba , None , "\xe1\x83\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe183bb , 0xe183bb , 0xe183bb , None , "\xe1\x83\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe183bc , 0xe183bc , 0xe183bc , None , "\xe1\x83\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe183bd , 0xe183bd , 0xe183bd , None , "\xe1\x83\xbd" },
  { 0xe183be , 0xe183be , 0xe183be , None , "\xe1\x83\xbe" },
  { 0xe183bf , 0xe183bf , 0xe183bf , None , "\xe1\x83\xbf" },
};

const character charmap_e1_b8[64] = {
  { 0xe1b880 , 0xe1b880 , 0xe1b881 , None , "\xe1\xb8\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1b881 , 0xe1b880 , 0xe1b881 , None , "\xe1\xb8\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1b882 , 0xe1b882 , 0xe1b883 , None , "\xe1\xb8\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1b883 , 0xe1b882 , 0xe1b883 , None , "\xe1\xb8\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1b884 , 0xe1b884 , 0xe1b885 , None , "\xe1\xb8\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1b885 , 0xe1b884 , 0xe1b885 , None , "\xe1\xb8\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1b886 , 0xe1b886 , 0xe1b887 , None , "\xe1\xb8\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1b887 , 0xe1b886 , 0xe1b887 , None , "\xe1\xb8\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1b888 , 0xe1b888 , 0xe1b889 , None , "\xe1\xb8\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1b889 , 0xe1b888 , 0xe1b889 , None , "\xe1\xb8\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1b88a , 0xe1b88a , 0xe1b88b , None , "\xe1\xb8\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1b88b , 0xe1b88a , 0xe1b88b , None , "\xe1\xb8\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1b88c , 0xe1b88c , 0xe1b88d , None , "\xe1\xb8\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1b88d , 0xe1b88c , 0xe1b88d , None , "\xe1\xb8\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1b88e , 0xe1b88e , 0xe1b88f , None , "\xe1\xb8\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1b88f , 0xe1b88e , 0xe1b88f , None , "\xe1\xb8\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe1b890 , 0xe1b890 , 0xe1b891 , None , "\xe1\xb8\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1b891 , 0xe1b890 , 0xe1b891 , None , "\xe1\xb8\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1b892 , 0xe1b892 , 0xe1b893 , None , "\xe1\xb8\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1b893 , 0xe1b892 , 0xe1b893 , None , "\xe1\xb8\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1b894 , 0xe1b894 , 0xe1b895 , None , "\xe1\xb8\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1b895 , 0xe1b894 , 0xe1b895 , None , "\xe1\xb8\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1b896 , 0xe1b896 , 0xe1b897 , None , "\xe1\xb8\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1b897 , 0xe1b896 , 0xe1b897 , None , "\xe1\xb8\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1b898 , 0xe1b898 , 0xe1b899 , None , "\xe1\xb8\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1b899 , 0xe1b898 , 0xe1b899 , None , "\xe1\xb8\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1b89a , 0xe1b89a , 0xe1b89b , None , "\xe1\xb8\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1b89b , 0xe1b89a , 0xe1b89b , None , "\xe1\xb8\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1b89c , 0xe1b89c , 0xe1b89d , None , "\xe1\xb8\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1b89d , 0xe1b89c , 0xe1b89d , None , "\xe1\xb8\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1b89e , 0xe1b89e , 0xe1b89f , None , "\xe1\xb8\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1b89f , 0xe1b89e , 0xe1b89f , None , "\xe1\xb8\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a0 , 0xe1b8a0 , 0xe1b8a1 , None , "\xe1\xb8\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a1 , 0xe1b8a0 , 0xe1b8a1 , None , "\xe1\xb8\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a2 , 0xe1b8a2 , 0xe1b8a3 , None , "\xe1\xb8\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a3 , 0xe1b8a2 , 0xe1b8a3 , None , "\xe1\xb8\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a4 , 0xe1b8a4 , 0xe1b8a5 , None , "\xe1\xb8\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a5 , 0xe1b8a4 , 0xe1b8a5 , None , "\xe1\xb8\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a6 , 0xe1b8a6 , 0xe1b8a7 , None , "\xe1\xb8\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a7 , 0xe1b8a6 , 0xe1b8a7 , None , "\xe1\xb8\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a8 , 0xe1b8a8 , 0xe1b8a9 , None , "\xe1\xb8\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8a9 , 0xe1b8a8 , 0xe1b8a9 , None , "\xe1\xb8\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8aa , 0xe1b8aa , 0xe1b8ab , None , "\xe1\xb8\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8ab , 0xe1b8aa , 0xe1b8ab , None , "\xe1\xb8\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8ac , 0xe1b8ac , 0xe1b8ad , None , "\xe1\xb8\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8ad , 0xe1b8ac , 0xe1b8ad , None , "\xe1\xb8\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8ae , 0xe1b8ae , 0xe1b8af , None , "\xe1\xb8\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8af , 0xe1b8ae , 0xe1b8af , None , "\xe1\xb8\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b0 , 0xe1b8b0 , 0xe1b8b1 , None , "\xe1\xb8\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b1 , 0xe1b8b0 , 0xe1b8b1 , None , "\xe1\xb8\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b2 , 0xe1b8b2 , 0xe1b8b3 , None , "\xe1\xb8\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b3 , 0xe1b8b2 , 0xe1b8b3 , None , "\xe1\xb8\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b4 , 0xe1b8b4 , 0xe1b8b5 , None , "\xe1\xb8\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b5 , 0xe1b8b4 , 0xe1b8b5 , None , "\xe1\xb8\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b6 , 0xe1b8b6 , 0xe1b8b7 , None , "\xe1\xb8\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b7 , 0xe1b8b6 , 0xe1b8b7 , None , "\xe1\xb8\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b8 , 0xe1b8b8 , 0xe1b8b9 , None , "\xe1\xb8\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8b9 , 0xe1b8b8 , 0xe1b8b9 , None , "\xe1\xb8\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8ba , 0xe1b8ba , 0xe1b8bb , None , "\xe1\xb8\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8bb , 0xe1b8ba , 0xe1b8bb , None , "\xe1\xb8\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8bc , 0xe1b8bc , 0xe1b8bd , None , "\xe1\xb8\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8bd , 0xe1b8bc , 0xe1b8bd , None , "\xe1\xb8\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8be , 0xe1b8be , 0xe1b8bf , None , "\xe1\xb8\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe1b8bf , 0xe1b8be , 0xe1b8bf , None , "\xe1\xb8\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e1_b9[64] = {
  { 0xe1b980 , 0xe1b980 , 0xe1b981 , None , "\xe1\xb9\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1b981 , 0xe1b980 , 0xe1b981 , None , "\xe1\xb9\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1b982 , 0xe1b982 , 0xe1b983 , None , "\xe1\xb9\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1b983 , 0xe1b982 , 0xe1b983 , None , "\xe1\xb9\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1b984 , 0xe1b984 , 0xe1b985 , None , "\xe1\xb9\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1b985 , 0xe1b984 , 0xe1b985 , None , "\xe1\xb9\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1b986 , 0xe1b986 , 0xe1b987 , None , "\xe1\xb9\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1b987 , 0xe1b986 , 0xe1b987 , None , "\xe1\xb9\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1b988 , 0xe1b988 , 0xe1b989 , None , "\xe1\xb9\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1b989 , 0xe1b988 , 0xe1b989 , None , "\xe1\xb9\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1b98a , 0xe1b98a , 0xe1b98b , None , "\xe1\xb9\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1b98b , 0xe1b98a , 0xe1b98b , None , "\xe1\xb9\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1b98c , 0xe1b98c , 0xe1b98d , None , "\xe1\xb9\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1b98d , 0xe1b98c , 0xe1b98d , None , "\xe1\xb9\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1b98e , 0xe1b98e , 0xe1b98f , None , "\xe1\xb9\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1b98f , 0xe1b98e , 0xe1b98f , None , "\xe1\xb9\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe1b990 , 0xe1b990 , 0xe1b991 , None , "\xe1\xb9\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1b991 , 0xe1b990 , 0xe1b991 , None , "\xe1\xb9\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1b992 , 0xe1b992 , 0xe1b993 , None , "\xe1\xb9\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1b993 , 0xe1b992 , 0xe1b993 , None , "\xe1\xb9\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1b994 , 0xe1b994 , 0xe1b995 , None , "\xe1\xb9\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1b995 , 0xe1b994 , 0xe1b995 , None , "\xe1\xb9\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1b996 , 0xe1b996 , 0xe1b997 , None , "\xe1\xb9\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1b997 , 0xe1b996 , 0xe1b997 , None , "\xe1\xb9\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1b998 , 0xe1b998 , 0xe1b999 , None , "\xe1\xb9\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1b999 , 0xe1b998 , 0xe1b999 , None , "\xe1\xb9\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1b99a , 0xe1b99a , 0xe1b99b , None , "\xe1\xb9\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1b99b , 0xe1b99a , 0xe1b99b , None , "\xe1\xb9\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1b99c , 0xe1b99c , 0xe1b99d , None , "\xe1\xb9\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1b99d , 0xe1b99c , 0xe1b99d , None , "\xe1\xb9\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1b99e , 0xe1b99e , 0xe1b99f , None , "\xe1\xb9\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1b99f , 0xe1b99e , 0xe1b99f , None , "\xe1\xb9\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a0 , 0xe1b9a0 , 0xe1b9a1 , None , "\xe1\xb9\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a1 , 0xe1b9a0 , 0xe1b9a1 , None , "\xe1\xb9\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a2 , 0xe1b9a2 , 0xe1b9a3 , None , "\xe1\xb9\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a3 , 0xe1b9a2 , 0xe1b9a3 , None , "\xe1\xb9\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a4 , 0xe1b9a4 , 0xe1b9a5 , None , "\xe1\xb9\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a5 , 0xe1b9a4 , 0xe1b9a5 , None , "\xe1\xb9\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a6 , 0xe1b9a6 , 0xe1b9a7 , None , "\xe1\xb9\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a7 , 0xe1b9a6 , 0xe1b9a7 , None , "\xe1\xb9\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a8 , 0xe1b9a8 , 0xe1b9a9 , None , "\xe1\xb9\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9a9 , 0xe1b9a8 , 0xe1b9a9 , None , "\xe1\xb9\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9aa , 0xe1b9aa , 0xe1b9ab , None , "\xe1\xb9\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9ab , 0xe1b9aa , 0xe1b9ab , None , "\xe1\xb9\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9ac , 0xe1b9ac , 0xe1b9ad , None , "\xe1\xb9\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9ad , 0xe1b9ac , 0xe1b9ad , None , "\xe1\xb9\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9ae , 0xe1b9ae , 0xe1b9af , None , "\xe1\xb9\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9af , 0xe1b9ae , 0xe1b9af , None , "\xe1\xb9\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b0 , 0xe1b9b0 , 0xe1b9b1 , None , "\xe1\xb9\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b1 , 0xe1b9b0 , 0xe1b9b1 , None , "\xe1\xb9\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b2 , 0xe1b9b2 , 0xe1b9b3 , None , "\xe1\xb9\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b3 , 0xe1b9b2 , 0xe1b9b3 , None , "\xe1\xb9\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b4 , 0xe1b9b4 , 0xe1b9b5 , None , "\xe1\xb9\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b5 , 0xe1b9b4 , 0xe1b9b5 , None , "\xe1\xb9\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b6 , 0xe1b9b6 , 0xe1b9b7 , None , "\xe1\xb9\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b7 , 0xe1b9b6 , 0xe1b9b7 , None , "\xe1\xb9\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b8 , 0xe1b9b8 , 0xe1b9b9 , None , "\xe1\xb9\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9b9 , 0xe1b9b8 , 0xe1b9b9 , None , "\xe1\xb9\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9ba , 0xe1b9ba , 0xe1b9bb , None , "\xe1\xb9\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9bb , 0xe1b9ba , 0xe1b9bb , None , "\xe1\xb9\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9bc , 0xe1b9bc , 0xe1b9bd , None , "\xe1\xb9\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9bd , 0xe1b9bc , 0xe1b9bd , None , "\xe1\xb9\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9be , 0xe1b9be , 0xe1b9bf , None , "\xe1\xb9\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe1b9bf , 0xe1b9be , 0xe1b9bf , None , "\xe1\xb9\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e1_ba[64] = {
  { 0xe1ba80 , 0xe1ba80 , 0xe1ba81 , None , "\xe1\xba\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba81 , 0xe1ba80 , 0xe1ba81 , None , "\xe1\xba\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba82 , 0xe1ba82 , 0xe1ba83 , None , "\xe1\xba\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba83 , 0xe1ba82 , 0xe1ba83 , None , "\xe1\xba\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba84 , 0xe1ba84 , 0xe1ba85 , None , "\xe1\xba\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba85 , 0xe1ba84 , 0xe1ba85 , None , "\xe1\xba\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba86 , 0xe1ba86 , 0xe1ba87 , None , "\xe1\xba\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba87 , 0xe1ba86 , 0xe1ba87 , None , "\xe1\xba\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba88 , 0xe1ba88 , 0xe1ba89 , None , "\xe1\xba\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba89 , 0xe1ba88 , 0xe1ba89 , None , "\xe1\xba\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba8a , 0xe1ba8a , 0xe1ba8b , None , "\xe1\xba\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba8b , 0xe1ba8a , 0xe1ba8b , None , "\xe1\xba\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba8c , 0xe1ba8c , 0xe1ba8d , None , "\xe1\xba\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba8d , 0xe1ba8c , 0xe1ba8d , None , "\xe1\xba\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba8e , 0xe1ba8e , 0xe1ba8f , None , "\xe1\xba\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba8f , 0xe1ba8e , 0xe1ba8f , None , "\xe1\xba\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba90 , 0xe1ba90 , 0xe1ba91 , None , "\xe1\xba\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba91 , 0xe1ba90 , 0xe1ba91 , None , "\xe1\xba\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba92 , 0xe1ba92 , 0xe1ba93 , None , "\xe1\xba\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba93 , 0xe1ba92 , 0xe1ba93 , None , "\xe1\xba\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba94 , 0xe1ba94 , 0xe1ba95 , None , "\xe1\xba\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba95 , 0xe1ba94 , 0xe1ba95 , None , "\xe1\xba\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba96 , 0xe1ba96 , 0xe1ba96 , None , "\xe1\xba\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba97 , 0xe1ba97 , 0xe1ba97 , None , "\xe1\xba\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba98 , 0xe1ba98 , 0xe1ba98 , None , "\xe1\xba\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba99 , 0xe1ba99 , 0xe1ba99 , None , "\xe1\xba\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba9a , 0xe1ba9a , 0xe1ba9a , None , "\xe1\xba\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba9b , 0xe1b9a0 , 0xe1ba9b , None , "\xe1\xba\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba9c , 0xe1ba9c , 0xe1ba9c , None , "\xe1\xba\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba9d , 0xe1ba9d , 0xe1ba9d , None , "\xe1\xba\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1ba9e , 0xe1ba9e , 0xc39f , None , "\xe1\xba\x9e" }, /* ??? : up=??? : lo=?? */
  { 0xe1ba9f , 0xe1ba9f , 0xe1ba9f , None , "\xe1\xba\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa0 , 0xe1baa0 , 0xe1baa1 , None , "\xe1\xba\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa1 , 0xe1baa0 , 0xe1baa1 , None , "\xe1\xba\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa2 , 0xe1baa2 , 0xe1baa3 , None , "\xe1\xba\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa3 , 0xe1baa2 , 0xe1baa3 , None , "\xe1\xba\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa4 , 0xe1baa4 , 0xe1baa5 , None , "\xe1\xba\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa5 , 0xe1baa4 , 0xe1baa5 , None , "\xe1\xba\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa6 , 0xe1baa6 , 0xe1baa7 , None , "\xe1\xba\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa7 , 0xe1baa6 , 0xe1baa7 , None , "\xe1\xba\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa8 , 0xe1baa8 , 0xe1baa9 , None , "\xe1\xba\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1baa9 , 0xe1baa8 , 0xe1baa9 , None , "\xe1\xba\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1baaa , 0xe1baaa , 0xe1baab , None , "\xe1\xba\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1baab , 0xe1baaa , 0xe1baab , None , "\xe1\xba\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1baac , 0xe1baac , 0xe1baad , None , "\xe1\xba\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1baad , 0xe1baac , 0xe1baad , None , "\xe1\xba\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1baae , 0xe1baae , 0xe1baaf , None , "\xe1\xba\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1baaf , 0xe1baae , 0xe1baaf , None , "\xe1\xba\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab0 , 0xe1bab0 , 0xe1bab1 , None , "\xe1\xba\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab1 , 0xe1bab0 , 0xe1bab1 , None , "\xe1\xba\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab2 , 0xe1bab2 , 0xe1bab3 , None , "\xe1\xba\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab3 , 0xe1bab2 , 0xe1bab3 , None , "\xe1\xba\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab4 , 0xe1bab4 , 0xe1bab5 , None , "\xe1\xba\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab5 , 0xe1bab4 , 0xe1bab5 , None , "\xe1\xba\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab6 , 0xe1bab6 , 0xe1bab7 , None , "\xe1\xba\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab7 , 0xe1bab6 , 0xe1bab7 , None , "\xe1\xba\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab8 , 0xe1bab8 , 0xe1bab9 , None , "\xe1\xba\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bab9 , 0xe1bab8 , 0xe1bab9 , None , "\xe1\xba\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1baba , 0xe1baba , 0xe1babb , None , "\xe1\xba\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1babb , 0xe1baba , 0xe1babb , None , "\xe1\xba\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1babc , 0xe1babc , 0xe1babd , None , "\xe1\xba\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1babd , 0xe1babc , 0xe1babd , None , "\xe1\xba\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1babe , 0xe1babe , 0xe1babf , None , "\xe1\xba\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe1babf , 0xe1babe , 0xe1babf , None , "\xe1\xba\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e1_bb[64] = {
  { 0xe1bb80 , 0xe1bb80 , 0xe1bb81 , None , "\xe1\xbb\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb81 , 0xe1bb80 , 0xe1bb81 , None , "\xe1\xbb\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb82 , 0xe1bb82 , 0xe1bb83 , None , "\xe1\xbb\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb83 , 0xe1bb82 , 0xe1bb83 , None , "\xe1\xbb\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb84 , 0xe1bb84 , 0xe1bb85 , None , "\xe1\xbb\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb85 , 0xe1bb84 , 0xe1bb85 , None , "\xe1\xbb\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb86 , 0xe1bb86 , 0xe1bb87 , None , "\xe1\xbb\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb87 , 0xe1bb86 , 0xe1bb87 , None , "\xe1\xbb\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb88 , 0xe1bb88 , 0xe1bb89 , None , "\xe1\xbb\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb89 , 0xe1bb88 , 0xe1bb89 , None , "\xe1\xbb\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb8a , 0xe1bb8a , 0xe1bb8b , None , "\xe1\xbb\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb8b , 0xe1bb8a , 0xe1bb8b , None , "\xe1\xbb\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb8c , 0xe1bb8c , 0xe1bb8d , None , "\xe1\xbb\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb8d , 0xe1bb8c , 0xe1bb8d , None , "\xe1\xbb\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb8e , 0xe1bb8e , 0xe1bb8f , None , "\xe1\xbb\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb8f , 0xe1bb8e , 0xe1bb8f , None , "\xe1\xbb\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb90 , 0xe1bb90 , 0xe1bb91 , None , "\xe1\xbb\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb91 , 0xe1bb90 , 0xe1bb91 , None , "\xe1\xbb\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb92 , 0xe1bb92 , 0xe1bb93 , None , "\xe1\xbb\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb93 , 0xe1bb92 , 0xe1bb93 , None , "\xe1\xbb\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb94 , 0xe1bb94 , 0xe1bb95 , None , "\xe1\xbb\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb95 , 0xe1bb94 , 0xe1bb95 , None , "\xe1\xbb\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb96 , 0xe1bb96 , 0xe1bb97 , None , "\xe1\xbb\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb97 , 0xe1bb96 , 0xe1bb97 , None , "\xe1\xbb\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb98 , 0xe1bb98 , 0xe1bb99 , None , "\xe1\xbb\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb99 , 0xe1bb98 , 0xe1bb99 , None , "\xe1\xbb\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb9a , 0xe1bb9a , 0xe1bb9b , None , "\xe1\xbb\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb9b , 0xe1bb9a , 0xe1bb9b , None , "\xe1\xbb\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb9c , 0xe1bb9c , 0xe1bb9d , None , "\xe1\xbb\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb9d , 0xe1bb9c , 0xe1bb9d , None , "\xe1\xbb\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb9e , 0xe1bb9e , 0xe1bb9f , None , "\xe1\xbb\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1bb9f , 0xe1bb9e , 0xe1bb9f , None , "\xe1\xbb\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba0 , 0xe1bba0 , 0xe1bba1 , None , "\xe1\xbb\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba1 , 0xe1bba0 , 0xe1bba1 , None , "\xe1\xbb\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba2 , 0xe1bba2 , 0xe1bba3 , None , "\xe1\xbb\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba3 , 0xe1bba2 , 0xe1bba3 , None , "\xe1\xbb\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba4 , 0xe1bba4 , 0xe1bba5 , None , "\xe1\xbb\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba5 , 0xe1bba4 , 0xe1bba5 , None , "\xe1\xbb\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba6 , 0xe1bba6 , 0xe1bba7 , None , "\xe1\xbb\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba7 , 0xe1bba6 , 0xe1bba7 , None , "\xe1\xbb\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba8 , 0xe1bba8 , 0xe1bba9 , None , "\xe1\xbb\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bba9 , 0xe1bba8 , 0xe1bba9 , None , "\xe1\xbb\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbaa , 0xe1bbaa , 0xe1bbab , None , "\xe1\xbb\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbab , 0xe1bbaa , 0xe1bbab , None , "\xe1\xbb\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbac , 0xe1bbac , 0xe1bbad , None , "\xe1\xbb\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbad , 0xe1bbac , 0xe1bbad , None , "\xe1\xbb\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbae , 0xe1bbae , 0xe1bbaf , None , "\xe1\xbb\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbaf , 0xe1bbae , 0xe1bbaf , None , "\xe1\xbb\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb0 , 0xe1bbb0 , 0xe1bbb1 , None , "\xe1\xbb\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb1 , 0xe1bbb0 , 0xe1bbb1 , None , "\xe1\xbb\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb2 , 0xe1bbb2 , 0xe1bbb3 , None , "\xe1\xbb\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb3 , 0xe1bbb2 , 0xe1bbb3 , None , "\xe1\xbb\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb4 , 0xe1bbb4 , 0xe1bbb5 , None , "\xe1\xbb\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb5 , 0xe1bbb4 , 0xe1bbb5 , None , "\xe1\xbb\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb6 , 0xe1bbb6 , 0xe1bbb7 , None , "\xe1\xbb\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb7 , 0xe1bbb6 , 0xe1bbb7 , None , "\xe1\xbb\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb8 , 0xe1bbb8 , 0xe1bbb9 , None , "\xe1\xbb\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbb9 , 0xe1bbb8 , 0xe1bbb9 , None , "\xe1\xbb\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbba , 0xe1bbba , 0xe1bbbb , None , "\xe1\xbb\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbbb , 0xe1bbba , 0xe1bbbb , None , "\xe1\xbb\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbbc , 0xe1bbbc , 0xe1bbbd , None , "\xe1\xbb\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbbd , 0xe1bbbc , 0xe1bbbd , None , "\xe1\xbb\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbbe , 0xe1bbbe , 0xe1bbbf , None , "\xe1\xbb\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe1bbbf , 0xe1bbbe , 0xe1bbbf , None , "\xe1\xbb\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e1_bc[64] = {
  { 0xe1bc80 , 0xe1bc88 , 0xe1bc80 , None , "\xe1\xbc\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc81 , 0xe1bc89 , 0xe1bc81 , None , "\xe1\xbc\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc82 , 0xe1bc8a , 0xe1bc82 , None , "\xe1\xbc\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc83 , 0xe1bc8b , 0xe1bc83 , None , "\xe1\xbc\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc84 , 0xe1bc8c , 0xe1bc84 , None , "\xe1\xbc\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc85 , 0xe1bc8d , 0xe1bc85 , None , "\xe1\xbc\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc86 , 0xe1bc8e , 0xe1bc86 , None , "\xe1\xbc\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc87 , 0xe1bc8f , 0xe1bc87 , None , "\xe1\xbc\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc88 , 0xe1bc88 , 0xe1bc80 , None , "\xe1\xbc\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc89 , 0xe1bc89 , 0xe1bc81 , None , "\xe1\xbc\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc8a , 0xe1bc8a , 0xe1bc82 , None , "\xe1\xbc\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc8b , 0xe1bc8b , 0xe1bc83 , None , "\xe1\xbc\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc8c , 0xe1bc8c , 0xe1bc84 , None , "\xe1\xbc\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc8d , 0xe1bc8d , 0xe1bc85 , None , "\xe1\xbc\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc8e , 0xe1bc8e , 0xe1bc86 , None , "\xe1\xbc\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc8f , 0xe1bc8f , 0xe1bc87 , None , "\xe1\xbc\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc90 , 0xe1bc98 , 0xe1bc90 , None , "\xe1\xbc\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc91 , 0xe1bc99 , 0xe1bc91 , None , "\xe1\xbc\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc92 , 0xe1bc9a , 0xe1bc92 , None , "\xe1\xbc\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc93 , 0xe1bc9b , 0xe1bc93 , None , "\xe1\xbc\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc94 , 0xe1bc9c , 0xe1bc94 , None , "\xe1\xbc\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc95 , 0xe1bc9d , 0xe1bc95 , None , "\xe1\xbc\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc96 , 0xe1bc96 , 0xe1bc96 , None , "\xe1\xbc\x96" },
  { 0xe1bc97 , 0xe1bc97 , 0xe1bc97 , None , "\xe1\xbc\x97" },
  { 0xe1bc98 , 0xe1bc98 , 0xe1bc90 , None , "\xe1\xbc\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc99 , 0xe1bc99 , 0xe1bc91 , None , "\xe1\xbc\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc9a , 0xe1bc9a , 0xe1bc92 , None , "\xe1\xbc\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc9b , 0xe1bc9b , 0xe1bc93 , None , "\xe1\xbc\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc9c , 0xe1bc9c , 0xe1bc94 , None , "\xe1\xbc\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc9d , 0xe1bc9d , 0xe1bc95 , None , "\xe1\xbc\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bc9e , 0xe1bc9e , 0xe1bc9e , None , "\xe1\xbc\x9e" },
  { 0xe1bc9f , 0xe1bc9f , 0xe1bc9f , None , "\xe1\xbc\x9f" },
  { 0xe1bca0 , 0xe1bca8 , 0xe1bca0 , None , "\xe1\xbc\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca1 , 0xe1bca9 , 0xe1bca1 , None , "\xe1\xbc\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca2 , 0xe1bcaa , 0xe1bca2 , None , "\xe1\xbc\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca3 , 0xe1bcab , 0xe1bca3 , None , "\xe1\xbc\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca4 , 0xe1bcac , 0xe1bca4 , None , "\xe1\xbc\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca5 , 0xe1bcad , 0xe1bca5 , None , "\xe1\xbc\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca6 , 0xe1bcae , 0xe1bca6 , None , "\xe1\xbc\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca7 , 0xe1bcaf , 0xe1bca7 , None , "\xe1\xbc\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca8 , 0xe1bca8 , 0xe1bca0 , None , "\xe1\xbc\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bca9 , 0xe1bca9 , 0xe1bca1 , None , "\xe1\xbc\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcaa , 0xe1bcaa , 0xe1bca2 , None , "\xe1\xbc\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcab , 0xe1bcab , 0xe1bca3 , None , "\xe1\xbc\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcac , 0xe1bcac , 0xe1bca4 , None , "\xe1\xbc\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcad , 0xe1bcad , 0xe1bca5 , None , "\xe1\xbc\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcae , 0xe1bcae , 0xe1bca6 , None , "\xe1\xbc\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcaf , 0xe1bcaf , 0xe1bca7 , None , "\xe1\xbc\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb0 , 0xe1bcb8 , 0xe1bcb0 , None , "\xe1\xbc\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb1 , 0xe1bcb9 , 0xe1bcb1 , None , "\xe1\xbc\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb2 , 0xe1bcba , 0xe1bcb2 , None , "\xe1\xbc\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb3 , 0xe1bcbb , 0xe1bcb3 , None , "\xe1\xbc\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb4 , 0xe1bcbc , 0xe1bcb4 , None , "\xe1\xbc\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb5 , 0xe1bcbd , 0xe1bcb5 , None , "\xe1\xbc\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb6 , 0xe1bcbe , 0xe1bcb6 , None , "\xe1\xbc\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb7 , 0xe1bcbf , 0xe1bcb7 , None , "\xe1\xbc\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb8 , 0xe1bcb8 , 0xe1bcb0 , None , "\xe1\xbc\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcb9 , 0xe1bcb9 , 0xe1bcb1 , None , "\xe1\xbc\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcba , 0xe1bcba , 0xe1bcb2 , None , "\xe1\xbc\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcbb , 0xe1bcbb , 0xe1bcb3 , None , "\xe1\xbc\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcbc , 0xe1bcbc , 0xe1bcb4 , None , "\xe1\xbc\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcbd , 0xe1bcbd , 0xe1bcb5 , None , "\xe1\xbc\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcbe , 0xe1bcbe , 0xe1bcb6 , None , "\xe1\xbc\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe1bcbf , 0xe1bcbf , 0xe1bcb7 , None , "\xe1\xbc\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e1_bd[64] = {
  { 0xe1bd80 , 0xe1bd88 , 0xe1bd80 , None , "\xe1\xbd\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd81 , 0xe1bd89 , 0xe1bd81 , None , "\xe1\xbd\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd82 , 0xe1bd8a , 0xe1bd82 , None , "\xe1\xbd\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd83 , 0xe1bd8b , 0xe1bd83 , None , "\xe1\xbd\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd84 , 0xe1bd8c , 0xe1bd84 , None , "\xe1\xbd\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd85 , 0xe1bd8d , 0xe1bd85 , None , "\xe1\xbd\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd86 , 0xe1bd86 , 0xe1bd86 , None , "\xe1\xbd\x86" },
  { 0xe1bd87 , 0xe1bd87 , 0xe1bd87 , None , "\xe1\xbd\x87" },
  { 0xe1bd88 , 0xe1bd88 , 0xe1bd80 , None , "\xe1\xbd\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd89 , 0xe1bd89 , 0xe1bd81 , None , "\xe1\xbd\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd8a , 0xe1bd8a , 0xe1bd82 , None , "\xe1\xbd\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd8b , 0xe1bd8b , 0xe1bd83 , None , "\xe1\xbd\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd8c , 0xe1bd8c , 0xe1bd84 , None , "\xe1\xbd\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd8d , 0xe1bd8d , 0xe1bd85 , None , "\xe1\xbd\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd8e , 0xe1bd8e , 0xe1bd8e , None , "\xe1\xbd\x8e" },
  { 0xe1bd8f , 0xe1bd8f , 0xe1bd8f , None , "\xe1\xbd\x8f" },
  { 0xe1bd90 , 0xe1bd90 , 0xe1bd90 , None , "\xe1\xbd\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd91 , 0xe1bd99 , 0xe1bd91 , None , "\xe1\xbd\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd92 , 0xe1bd92 , 0xe1bd92 , None , "\xe1\xbd\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd93 , 0xe1bd9b , 0xe1bd93 , None , "\xe1\xbd\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd94 , 0xe1bd94 , 0xe1bd94 , None , "\xe1\xbd\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd95 , 0xe1bd9d , 0xe1bd95 , None , "\xe1\xbd\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd96 , 0xe1bd96 , 0xe1bd96 , None , "\xe1\xbd\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd97 , 0xe1bd9f , 0xe1bd97 , None , "\xe1\xbd\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd98 , 0xe1bd98 , 0xe1bd98 , None , "\xe1\xbd\x98" },
  { 0xe1bd99 , 0xe1bd99 , 0xe1bd91 , None , "\xe1\xbd\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd9a , 0xe1bd9a , 0xe1bd9a , None , "\xe1\xbd\x9a" },
  { 0xe1bd9b , 0xe1bd9b , 0xe1bd93 , None , "\xe1\xbd\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd9c , 0xe1bd9c , 0xe1bd9c , None , "\xe1\xbd\x9c" },
  { 0xe1bd9d , 0xe1bd9d , 0xe1bd95 , None , "\xe1\xbd\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bd9e , 0xe1bd9e , 0xe1bd9e , None , "\xe1\xbd\x9e" },
  { 0xe1bd9f , 0xe1bd9f , 0xe1bd97 , None , "\xe1\xbd\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda0 , 0xe1bda8 , 0xe1bda0 , None , "\xe1\xbd\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda1 , 0xe1bda9 , 0xe1bda1 , None , "\xe1\xbd\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda2 , 0xe1bdaa , 0xe1bda2 , None , "\xe1\xbd\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda3 , 0xe1bdab , 0xe1bda3 , None , "\xe1\xbd\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda4 , 0xe1bdac , 0xe1bda4 , None , "\xe1\xbd\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda5 , 0xe1bdad , 0xe1bda5 , None , "\xe1\xbd\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda6 , 0xe1bdae , 0xe1bda6 , None , "\xe1\xbd\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda7 , 0xe1bdaf , 0xe1bda7 , None , "\xe1\xbd\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda8 , 0xe1bda8 , 0xe1bda0 , None , "\xe1\xbd\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bda9 , 0xe1bda9 , 0xe1bda1 , None , "\xe1\xbd\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdaa , 0xe1bdaa , 0xe1bda2 , None , "\xe1\xbd\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdab , 0xe1bdab , 0xe1bda3 , None , "\xe1\xbd\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdac , 0xe1bdac , 0xe1bda4 , None , "\xe1\xbd\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdad , 0xe1bdad , 0xe1bda5 , None , "\xe1\xbd\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdae , 0xe1bdae , 0xe1bda6 , None , "\xe1\xbd\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdaf , 0xe1bdaf , 0xe1bda7 , None , "\xe1\xbd\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb0 , 0xe1beba , 0xe1bdb0 , None , "\xe1\xbd\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb1 , 0xe1bebb , 0xe1bdb1 , None , "\xe1\xbd\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb2 , 0xe1bf88 , 0xe1bdb2 , None , "\xe1\xbd\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb3 , 0xe1bf89 , 0xe1bdb3 , None , "\xe1\xbd\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb4 , 0xe1bf8a , 0xe1bdb4 , None , "\xe1\xbd\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb5 , 0xe1bf8b , 0xe1bdb5 , None , "\xe1\xbd\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb6 , 0xe1bf9a , 0xe1bdb6 , None , "\xe1\xbd\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb7 , 0xe1bf9b , 0xe1bdb7 , None , "\xe1\xbd\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb8 , 0xe1bfb8 , 0xe1bdb8 , None , "\xe1\xbd\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdb9 , 0xe1bfb9 , 0xe1bdb9 , None , "\xe1\xbd\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdba , 0xe1bfaa , 0xe1bdba , None , "\xe1\xbd\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdbb , 0xe1bfab , 0xe1bdbb , None , "\xe1\xbd\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdbc , 0xe1bfba , 0xe1bdbc , None , "\xe1\xbd\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdbd , 0xe1bfbb , 0xe1bdbd , None , "\xe1\xbd\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1bdbe , 0xe1bdbe , 0xe1bdbe , None , "\xe1\xbd\xbe" },
  { 0xe1bdbf , 0xe1bdbf , 0xe1bdbf , None , "\xe1\xbd\xbf" },
};

const character charmap_e1_be[64] = {
  { 0xe1be80 , 0xe1be88 , 0xe1be80 , None , "\xe1\xbe\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe1be81 , 0xe1be89 , 0xe1be81 , None , "\xe1\xbe\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe1be82 , 0xe1be8a , 0xe1be82 , None , "\xe1\xbe\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1be83 , 0xe1be8b , 0xe1be83 , None , "\xe1\xbe\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1be84 , 0xe1be8c , 0xe1be84 , None , "\xe1\xbe\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1be85 , 0xe1be8d , 0xe1be85 , None , "\xe1\xbe\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1be86 , 0xe1be8e , 0xe1be86 , None , "\xe1\xbe\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1be87 , 0xe1be8f , 0xe1be87 , None , "\xe1\xbe\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1be88 , 0xe1be88 , 0xe1be80 , None , "\xe1\xbe\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1be89 , 0xe1be89 , 0xe1be81 , None , "\xe1\xbe\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1be8a , 0xe1be8a , 0xe1be82 , None , "\xe1\xbe\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1be8b , 0xe1be8b , 0xe1be83 , None , "\xe1\xbe\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1be8c , 0xe1be8c , 0xe1be84 , None , "\xe1\xbe\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1be8d , 0xe1be8d , 0xe1be85 , None , "\xe1\xbe\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe1be8e , 0xe1be8e , 0xe1be86 , None , "\xe1\xbe\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe1be8f , 0xe1be8f , 0xe1be87 , None , "\xe1\xbe\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe1be90 , 0xe1be98 , 0xe1be90 , None , "\xe1\xbe\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1be91 , 0xe1be99 , 0xe1be91 , None , "\xe1\xbe\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1be92 , 0xe1be9a , 0xe1be92 , None , "\xe1\xbe\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1be93 , 0xe1be9b , 0xe1be93 , None , "\xe1\xbe\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1be94 , 0xe1be9c , 0xe1be94 , None , "\xe1\xbe\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe1be95 , 0xe1be9d , 0xe1be95 , None , "\xe1\xbe\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe1be96 , 0xe1be9e , 0xe1be96 , None , "\xe1\xbe\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1be97 , 0xe1be9f , 0xe1be97 , None , "\xe1\xbe\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1be98 , 0xe1be98 , 0xe1be90 , None , "\xe1\xbe\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1be99 , 0xe1be99 , 0xe1be91 , None , "\xe1\xbe\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1be9a , 0xe1be9a , 0xe1be92 , None , "\xe1\xbe\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1be9b , 0xe1be9b , 0xe1be93 , None , "\xe1\xbe\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1be9c , 0xe1be9c , 0xe1be94 , None , "\xe1\xbe\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe1be9d , 0xe1be9d , 0xe1be95 , None , "\xe1\xbe\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1be9e , 0xe1be9e , 0xe1be96 , None , "\xe1\xbe\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1be9f , 0xe1be9f , 0xe1be97 , None , "\xe1\xbe\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea0 , 0xe1bea8 , 0xe1bea0 , None , "\xe1\xbe\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea1 , 0xe1bea9 , 0xe1bea1 , None , "\xe1\xbe\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea2 , 0xe1beaa , 0xe1bea2 , None , "\xe1\xbe\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea3 , 0xe1beab , 0xe1bea3 , None , "\xe1\xbe\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea4 , 0xe1beac , 0xe1bea4 , None , "\xe1\xbe\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea5 , 0xe1bead , 0xe1bea5 , None , "\xe1\xbe\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea6 , 0xe1beae , 0xe1bea6 , None , "\xe1\xbe\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea7 , 0xe1beaf , 0xe1bea7 , None , "\xe1\xbe\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea8 , 0xe1bea8 , 0xe1bea0 , None , "\xe1\xbe\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bea9 , 0xe1bea9 , 0xe1bea1 , None , "\xe1\xbe\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1beaa , 0xe1beaa , 0xe1bea2 , None , "\xe1\xbe\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1beab , 0xe1beab , 0xe1bea3 , None , "\xe1\xbe\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1beac , 0xe1beac , 0xe1bea4 , None , "\xe1\xbe\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1bead , 0xe1bead , 0xe1bea5 , None , "\xe1\xbe\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe1beae , 0xe1beae , 0xe1bea6 , None , "\xe1\xbe\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe1beaf , 0xe1beaf , 0xe1bea7 , None , "\xe1\xbe\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb0 , 0xe1beb8 , 0xe1beb0 , None , "\xe1\xbe\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb1 , 0xe1beb9 , 0xe1beb1 , None , "\xe1\xbe\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb2 , 0xe1beb2 , 0xe1beb2 , None , "\xe1\xbe\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb3 , 0xe1bebc , 0xe1beb3 , None , "\xe1\xbe\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb4 , 0xe1beb4 , 0xe1beb4 , None , "\xe1\xbe\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb5 , 0xe1beb5 , 0xe1beb5 , None , "\xe1\xbe\xb5" },
  { 0xe1beb6 , 0xe1beb6 , 0xe1beb6 , None , "\xe1\xbe\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb7 , 0xe1beb7 , 0xe1beb7 , None , "\xe1\xbe\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb8 , 0xe1beb8 , 0xe1beb0 , None , "\xe1\xbe\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1beb9 , 0xe1beb9 , 0xe1beb1 , None , "\xe1\xbe\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1beba , 0xe1beba , 0xe1bdb0 , None , "\xe1\xbe\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1bebb , 0xe1bebb , 0xe1bdb1 , None , "\xe1\xbe\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1bebc , 0xe1bebc , 0xe1beb3 , None , "\xe1\xbe\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1bebd , 0xe1bebd , 0xe1bebd , None , "\xe1\xbe\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe1bebe , 0xce99 , 0xe1bebe , None , "\xe1\xbe\xbe" }, /* ??? : up=?? : lo=??? */
  { 0xe1bebf , 0xe1bebf , 0xe1bebf , None , "\xe1\xbe\xbf" },
};

const character charmap_e1_bf[64] = {
  { 0xe1bf80 , 0xe1bf80 , 0xe1bf80 , None , "\xe1\xbf\x80" },
  { 0xe1bf81 , 0xe1bf81 , 0xe1bf81 , None , "\xe1\xbf\x81" },
  { 0xe1bf82 , 0xe1bf82 , 0xe1bf82 , None , "\xe1\xbf\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf83 , 0xe1bf8c , 0xe1bf83 , None , "\xe1\xbf\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf84 , 0xe1bf84 , 0xe1bf84 , None , "\xe1\xbf\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf85 , 0xe1bf85 , 0xe1bf85 , None , "\xe1\xbf\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf86 , 0xe1bf86 , 0xe1bf86 , None , "\xe1\xbf\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf87 , 0xe1bf87 , 0xe1bf87 , None , "\xe1\xbf\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf88 , 0xe1bf88 , 0xe1bdb2 , None , "\xe1\xbf\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf89 , 0xe1bf89 , 0xe1bdb3 , None , "\xe1\xbf\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf8a , 0xe1bf8a , 0xe1bdb4 , None , "\xe1\xbf\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf8b , 0xe1bf8b , 0xe1bdb5 , None , "\xe1\xbf\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf8c , 0xe1bf8c , 0xe1bf83 , None , "\xe1\xbf\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf8d , 0xe1bf8d , 0xe1bf8d , None , "\xe1\xbf\x8d" },
  { 0xe1bf8e , 0xe1bf8e , 0xe1bf8e , None , "\xe1\xbf\x8e" },
  { 0xe1bf8f , 0xe1bf8f , 0xe1bf8f , None , "\xe1\xbf\x8f" },
  { 0xe1bf90 , 0xe1bf98 , 0xe1bf90 , None , "\xe1\xbf\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf91 , 0xe1bf99 , 0xe1bf91 , None , "\xe1\xbf\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf92 , 0xe1bf92 , 0xe1bf92 , None , "\xe1\xbf\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf93 , 0xe1bf93 , 0xe1bf93 , None , "\xe1\xbf\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf94 , 0xe1bf94 , 0xe1bf94 , None , "\xe1\xbf\x94" },
  { 0xe1bf95 , 0xe1bf95 , 0xe1bf95 , None , "\xe1\xbf\x95" },
  { 0xe1bf96 , 0xe1bf96 , 0xe1bf96 , None , "\xe1\xbf\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf97 , 0xe1bf97 , 0xe1bf97 , None , "\xe1\xbf\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf98 , 0xe1bf98 , 0xe1bf90 , None , "\xe1\xbf\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf99 , 0xe1bf99 , 0xe1bf91 , None , "\xe1\xbf\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf9a , 0xe1bf9a , 0xe1bdb6 , None , "\xe1\xbf\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf9b , 0xe1bf9b , 0xe1bdb7 , None , "\xe1\xbf\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf9c , 0xe1bf9c , 0xe1bf9c , None , "\xe1\xbf\x9c" },
  { 0xe1bf9d , 0xe1bf9d , 0xe1bf9d , None , "\xe1\xbf\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf9e , 0xe1bf9e , 0xe1bf9e , None , "\xe1\xbf\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe1bf9f , 0xe1bf9f , 0xe1bf9f , None , "\xe1\xbf\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa0 , 0xe1bfa8 , 0xe1bfa0 , None , "\xe1\xbf\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa1 , 0xe1bfa9 , 0xe1bfa1 , None , "\xe1\xbf\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa2 , 0xe1bfa2 , 0xe1bfa2 , None , "\xe1\xbf\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa3 , 0xe1bfa3 , 0xe1bfa3 , None , "\xe1\xbf\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa4 , 0xe1bfa4 , 0xe1bfa4 , None , "\xe1\xbf\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa5 , 0xe1bfac , 0xe1bfa5 , None , "\xe1\xbf\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa6 , 0xe1bfa6 , 0xe1bfa6 , None , "\xe1\xbf\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa7 , 0xe1bfa7 , 0xe1bfa7 , None , "\xe1\xbf\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa8 , 0xe1bfa8 , 0xe1bfa0 , None , "\xe1\xbf\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfa9 , 0xe1bfa9 , 0xe1bfa1 , None , "\xe1\xbf\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfaa , 0xe1bfaa , 0xe1bdba , None , "\xe1\xbf\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfab , 0xe1bfab , 0xe1bdbb , None , "\xe1\xbf\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfac , 0xe1bfac , 0xe1bfa5 , None , "\xe1\xbf\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfad , 0xe1bfad , 0xe1bfad , None , "\xe1\xbf\xad" },
  { 0xe1bfae , 0xe1bfae , 0xe1bfae , None , "\xe1\xbf\xae" },
  { 0xe1bfaf , 0xe1bfaf , 0xe1bfaf , None , "\xe1\xbf\xaf" },
  { 0xe1bfb0 , 0xe1bfb0 , 0xe1bfb0 , None , "\xe1\xbf\xb0" },
  { 0xe1bfb1 , 0xe1bfb1 , 0xe1bfb1 , None , "\xe1\xbf\xb1" },
  { 0xe1bfb2 , 0xe1bfb2 , 0xe1bfb2 , None , "\xe1\xbf\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfb3 , 0xe1bfbc , 0xe1bfb3 , None , "\xe1\xbf\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfb4 , 0xe1bfb4 , 0xe1bfb4 , None , "\xe1\xbf\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfb5 , 0xe1bfb5 , 0xe1bfb5 , None , "\xe1\xbf\xb5" },
  { 0xe1bfb6 , 0xe1bfb6 , 0xe1bfb6 , None , "\xe1\xbf\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfb7 , 0xe1bfb7 , 0xe1bfb7 , None , "\xe1\xbf\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfb8 , 0xe1bfb8 , 0xe1bdb8 , None , "\xe1\xbf\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfb9 , 0xe1bfb9 , 0xe1bdb9 , None , "\xe1\xbf\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfba , 0xe1bfba , 0xe1bdbc , None , "\xe1\xbf\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfbb , 0xe1bfbb , 0xe1bdbd , None , "\xe1\xbf\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfbc , 0xe1bfbc , 0xe1bfb3 , None , "\xe1\xbf\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe1bfbd , 0xe1bfbd , 0xe1bfbd , None , "\xe1\xbf\xbd" },
  { 0xe1bfbe , 0xe1bfbe , 0xe1bfbe , None , "\xe1\xbf\xbe" },
  { 0xe1bfbf , 0xe1bfbf , 0xe1bfbf , None , "\xe1\xbf\xbf" },
};

const character* pagemap_24_e2[64] = {
  /* 80 */
  charmap_e2_80, charmap_e2_81, charmap_e2_82, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, charmap_e2_b4, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
};

const character charmap_e2_80[64] = {
  { 0xe28080 , 0xe28080 , 0xe28080 , None , "\xe2\x80\x80" },
  { 0xe28081 , 0xe28081 , 0xe28081 , None , "\xe2\x80\x81" },
  { 0xe28082 , 0xe28082 , 0xe28082 , None , "\xe2\x80\x82" },
  { 0xe28083 , 0xe28083 , 0xe28083 , None , "\xe2\x80\x83" },
  { 0xe28084 , 0xe28084 , 0xe28084 , None , "\xe2\x80\x84" },
  { 0xe28085 , 0xe28085 , 0xe28085 , None , "\xe2\x80\x85" },
  { 0xe28086 , 0xe28086 , 0xe28086 , None , "\xe2\x80\x86" },
  { 0xe28087 , 0xe28087 , 0xe28087 , IsSpace | IsBreaker , "\xe2\x80\x87" }, /* Figure Space */
  { 0xe28088 , 0xe28088 , 0xe28088 , None , "\xe2\x80\x88" },
  { 0xe28089 , 0xe28089 , 0xe28089 , None , "\xe2\x80\x89" },
  { 0xe2808a , 0xe2808a , 0xe2808a , None , "\xe2\x80\x8a" },
  { 0xe2808b , 0xe2808b , 0xe2808b , None , "\xe2\x80\x8b" },
  { 0xe2808c , 0xe2808c , 0xe2808c , None , "\xe2\x80\x8c" },
  { 0xe2808d , 0xe2808d , 0xe2808d , None , "\xe2\x80\x8d" },
  { 0xe2808e , 0xe2808e , 0xe2808e , None , "\xe2\x80\x8e" },
  { 0xe2808f , 0xe2808f , 0xe2808f , None , "\xe2\x80\x8f" },
  { 0xe28090 , 0xe28090 , 0xe28090 , IsPunctuation , "-" }, /* ??? : up=??? : lo=??? */
  { 0xe28091 , 0xe28091 , 0xe28091 , IsPunctuation , "-" }, /* ??? : up=??? : lo=??? */
  { 0xe28092 , 0xe28092 , 0xe28092 , IsPunctuation , "--" }, /* ??? : up=??? : lo=??? */
  { 0xe28093 , 0xe28093 , 0xe28093 , IsPunctuation , "--" }, /* ??? : up=??? : lo=??? */
  { 0xe28094 , 0xe28094 , 0xe28094 , IsPunctuation , "--" }, /* ??? : up=??? : lo=??? */
  { 0xe28095 , 0xe28095 , 0xe28095 , IsPunctuation , "--" }, /* ??? : up=??? : lo=??? */
  { 0xe28096 , 0xe28096 , 0xe28096 , None , "\xe2\x80\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe28097 , 0xe28097 , 0xe28097 , None , "\xe2\x80\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe28098 , 0xe28098 , 0xe28098 , IsPunctuation , "'" }, /* ??? : up=??? : lo=??? */
  { 0xe28099 , 0xe28099 , 0xe28099 , IsPunctuation , "'" }, /* ??? : up=??? : lo=??? */
  { 0xe2809a , 0xe2809a , 0xe2809a , IsPunctuation , "'" }, /* ??? : up=??? : lo=??? */
  { 0xe2809b , 0xe2809b , 0xe2809b , IsPunctuation , "'" }, /* ??? : up=??? : lo=??? */
  { 0xe2809c , 0xe2809c , 0xe2809c , IsPunctuation , "\"" }, /* ??? : up=??? : lo=??? */
  { 0xe2809d , 0xe2809d , 0xe2809d , IsPunctuation , "\"" }, /* ??? : up=??? : lo=??? */
  { 0xe2809e , 0xe2809e , 0xe2809e , IsPunctuation , "\"" }, /* ??? : up=??? : lo=??? */
  { 0xe2809f , 0xe2809f , 0xe2809f , IsPunctuation , "\"" }, /* ??? : up=??? : lo=??? */
  { 0xe280a0 , 0xe280a0 , 0xe280a0 , None , "\xe2\x80\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe280a1 , 0xe280a1 , 0xe280a1 , None , "\xe2\x80\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe280a2 , 0xe280a2 , 0xe280a2 , IsPunctuation , "o" }, /* ??? : up=??? : lo=??? */
  { 0xe280a3 , 0xe280a3 , 0xe280a3 , None , "\xe2\x80\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe280a4 , 0xe280a4 , 0xe280a4 , IsPunctuation , "." }, /* ??? : up=??? : lo=??? */
  { 0xe280a5 , 0xe280a5 , 0xe280a5 , IsPunctuation , ".." }, /* ??? : up=??? : lo=??? */
  { 0xe280a6 , 0xe280a6 , 0xe280a6 , IsPunctuation , "..." }, /* ??? : up=??? : lo=??? */
  { 0xe280a7 , 0xe280a7 , 0xe280a7 , IsPunctuation , "\xe2\x80\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe280a8 , 0xe280a8 , 0xe280a8 , None , "\xe2\x80\xa8" },
  { 0xe280a9 , 0xe280a9 , 0xe280a9 , None , "\xe2\x80\xa9" },
  { 0xe280aa , 0xe280aa , 0xe280aa , None , "\xe2\x80\xaa" },
  { 0xe280ab , 0xe280ab , 0xe280ab , None , "\xe2\x80\xab" },
  { 0xe280ac , 0xe280ac , 0xe280ac , None , "\xe2\x80\xac" },
  { 0xe280ad , 0xe280ad , 0xe280ad , None , "\xe2\x80\xad" },
  { 0xe280ae , 0xe280ae , 0xe280ae , None , "\xe2\x80\xae" },
  { 0xe280af , 0xe280af , 0xe280af , IsSpace , "\xe2\x80\xaf" }, /* Narrow No-Break Space (NNBSP) */
  { 0xe280b0 , 0xe280b0 , 0xe280b0 , None , "\xe2\x80\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe280b1 , 0xe280b1 , 0xe280b1 , None , "\xe2\x80\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe280b2 , 0xe280b2 , 0xe280b2 , None , "\xe2\x80\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe280b3 , 0xe280b3 , 0xe280b3 , None , "\xe2\x80\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe280b4 , 0xe280b4 , 0xe280b4 , None , "\xe2\x80\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe280b5 , 0xe280b5 , 0xe280b5 , None , "\xe2\x80\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe280b6 , 0xe280b6 , 0xe280b6 , None , "\xe2\x80\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe280b7 , 0xe280b7 , 0xe280b7 , None , "\xe2\x80\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe280b8 , 0xe280b8 , 0xe280b8 , None , "\xe2\x80\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe280b9 , 0xe280b9 , 0xe280b9 , None , "\xe2\x80\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe280ba , 0xe280ba , 0xe280ba , None , "\xe2\x80\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe280bb , 0xe280bb , 0xe280bb , None , "\xe2\x80\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe280bc , 0xe280bc , 0xe280bc , None , "\xe2\x80\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe280bd , 0xe280bd , 0xe280bd , None , "\xe2\x80\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe280be , 0xe280be , 0xe280be , None , "\xe2\x80\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe280bf , 0xe280bf , 0xe280bf , None , "\xe2\x80\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e2_81[64] = {
  { 0xe28180 , 0xe28180 , 0xe28180 , None , "\xe2\x81\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe28181 , 0xe28181 , 0xe28181 , None , "\xe2\x81\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe28182 , 0xe28182 , 0xe28182 , None , "\xe2\x81\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe28183 , 0xe28183 , 0xe28183 , None , "\xe2\x81\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe28184 , 0xe28184 , 0xe28184 , None , "/" }, /* ??? : up=??? : lo=??? */
  { 0xe28185 , 0xe28185 , 0xe28185 , None , "\xe2\x81\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe28186 , 0xe28186 , 0xe28186 , None , "\xe2\x81\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe28187 , 0xe28187 , 0xe28187 , None , "??" }, /* ??? : up=??? : lo=??? */
  { 0xe28188 , 0xe28188 , 0xe28188 , None , "?!" }, /* ??? : up=??? : lo=??? */
  { 0xe28189 , 0xe28189 , 0xe28189 , None , "!?" }, /* ??? : up=??? : lo=??? */
  { 0xe2818a , 0xe2818a , 0xe2818a , None , "&" }, /* ??? : up=??? : lo=??? */
  { 0xe2818b , 0xe2818b , 0xe2818b , None , "\xe2\x81\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe2818c , 0xe2818c , 0xe2818c , None , "\xe2\x81\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe2818d , 0xe2818d , 0xe2818d , None , "\xe2\x81\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe2818e , 0xe2818e , 0xe2818e , None , "\xe2\x81\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe2818f , 0xe2818f , 0xe2818f , None , "\xe2\x81\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe28190 , 0xe28190 , 0xe28190 , None , "\xe2\x81\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe28191 , 0xe28191 , 0xe28191 , None , "\xe2\x81\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe28192 , 0xe28192 , 0xe28192 , None , "\xe2\x81\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe28193 , 0xe28193 , 0xe28193 , None , "\xe2\x81\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe28194 , 0xe28194 , 0xe28194 , None , "\xe2\x81\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe28195 , 0xe28195 , 0xe28195 , None , "\xe2\x81\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe28196 , 0xe28196 , 0xe28196 , None , "\xe2\x81\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe28197 , 0xe28197 , 0xe28197 , None , "\xe2\x81\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe28198 , 0xe28198 , 0xe28198 , None , "\xe2\x81\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe28199 , 0xe28199 , 0xe28199 , None , "\xe2\x81\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe2819a , 0xe2819a , 0xe2819a , None , "\xe2\x81\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe2819b , 0xe2819b , 0xe2819b , None , "\xe2\x81\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe2819c , 0xe2819c , 0xe2819c , None , "\xe2\x81\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe2819d , 0xe2819d , 0xe2819d , None , "\xe2\x81\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe2819e , 0xe2819e , 0xe2819e , None , "\xe2\x81\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe2819f , 0xe2819f , 0xe2819f , IsSpace , " " }, /* (ESP MM) */
  { 0xe281a0 , 0xe281a0 , 0xe281a0 , None , "" }, /* hidden */
  { 0xe281a1 , 0xe281a1 , 0xe281a1 , None , "" }, /* hidden */
  { 0xe281a2 , 0xe281a2 , 0xe281a2 , None , "" }, /* hidden */
  { 0xe281a3 , 0xe281a3 , 0xe281a3 , None , "" }, /* hidden */
  { 0xe281a4 , 0xe281a4 , 0xe281a4 , None , "" }, /* hidden */
  { 0xe281a5 , 0xe281a5 , 0xe281a5 , None , "\xe2\x81\xa5" },
  { 0xe281a6 , 0xe281a6 , 0xe281a6 , None , "\xe2\x81\xa6" },
  { 0xe281a7 , 0xe281a7 , 0xe281a7 , None , "\xe2\x81\xa7" },
  { 0xe281a8 , 0xe281a8 , 0xe281a8 , None , "\xe2\x81\xa8" },
  { 0xe281a9 , 0xe281a9 , 0xe281a9 , None , "\xe2\x81\xa9" },
  { 0xe281aa , 0xe281aa , 0xe281aa , None , "\xe2\x81\xaa" },
  { 0xe281ab , 0xe281ab , 0xe281ab , None , "\xe2\x81\xab" },
  { 0xe281ac , 0xe281ac , 0xe281ac , None , "\xe2\x81\xac" },
  { 0xe281ad , 0xe281ad , 0xe281ad , None , "\xe2\x81\xad" },
  { 0xe281ae , 0xe281ae , 0xe281ae , None , "\xe2\x81\xae" },
  { 0xe281af , 0xe281af , 0xe281af , None , "\xe2\x81\xaf" },
  { 0xe281b0 , 0xe281b0 , 0xe281b0 , None , "0" }, /* ??? : up=??? : lo=??? */
  { 0xe281b1 , 0xe281b1 , 0xe281b1 , None , "i" }, /* ??? : up=??? : lo=??? */
  { 0xe281b2 , 0xe281b2 , 0xe281b2 , None , "\xe2\x81\xb2" },
  { 0xe281b3 , 0xe281b3 , 0xe281b3 , None , "\xe2\x81\xb3" },
  { 0xe281b4 , 0xe281b4 , 0xe281b4 , None , "4" }, /* ??? : up=??? : lo=??? */
  { 0xe281b5 , 0xe281b5 , 0xe281b5 , None , "5" }, /* ??? : up=??? : lo=??? */
  { 0xe281b6 , 0xe281b6 , 0xe281b6 , None , "6" }, /* ??? : up=??? : lo=??? */
  { 0xe281b7 , 0xe281b7 , 0xe281b7 , None , "7" }, /* ??? : up=??? : lo=??? */
  { 0xe281b8 , 0xe281b8 , 0xe281b8 , None , "8" }, /* ??? : up=??? : lo=??? */
  { 0xe281b9 , 0xe281b9 , 0xe281b9 , None , "9" }, /* ??? : up=??? : lo=??? */
  { 0xe281ba , 0xe281ba , 0xe281ba , None , "+" }, /* ??? : up=??? : lo=??? */
  { 0xe281bb , 0xe281bb , 0xe281bb , None , "-" }, /* ??? : up=??? : lo=??? */
  { 0xe281bc , 0xe281bc , 0xe281bc , None , "=" }, /* ??? : up=??? : lo=??? */
  { 0xe281bd , 0xe281bd , 0xe281bd , None , "(" }, /* ??? : up=??? : lo=??? */
  { 0xe281be , 0xe281be , 0xe281be , None , ")" }, /* ??? : up=??? : lo=??? */
  { 0xe281bf , 0xe281bf , 0xe281bf , None , "n" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e2_82[64] = {
  { 0xe28280 , 0xe28280 , 0xe28280 , None , "0" }, /* ??? : up=??? : lo=??? */
  { 0xe28281 , 0xe28281 , 0xe28281 , None , "1" }, /* ??? : up=??? : lo=??? */
  { 0xe28282 , 0xe28282 , 0xe28282 , None , "2" }, /* ??? : up=??? : lo=??? */
  { 0xe28283 , 0xe28283 , 0xe28283 , None , "3" }, /* ??? : up=??? : lo=??? */
  { 0xe28284 , 0xe28284 , 0xe28284 , None , "4" }, /* ??? : up=??? : lo=??? */
  { 0xe28285 , 0xe28285 , 0xe28285 , None , "5" }, /* ??? : up=??? : lo=??? */
  { 0xe28286 , 0xe28286 , 0xe28286 , None , "6" }, /* ??? : up=??? : lo=??? */
  { 0xe28287 , 0xe28287 , 0xe28287 , None , "7" }, /* ??? : up=??? : lo=??? */
  { 0xe28288 , 0xe28288 , 0xe28288 , None , "8" }, /* ??? : up=??? : lo=??? */
  { 0xe28289 , 0xe28289 , 0xe28289 , None , "9" }, /* ??? : up=??? : lo=??? */
  { 0xe2828a , 0xe2828a , 0xe2828a , None , "+" }, /* ??? : up=??? : lo=??? */
  { 0xe2828b , 0xe2828b , 0xe2828b , None , "-" }, /* ??? : up=??? : lo=??? */
  { 0xe2828c , 0xe2828c , 0xe2828c , None , "=" }, /* ??? : up=??? : lo=??? */
  { 0xe2828d , 0xe2828d , 0xe2828d , None , "(" }, /* ??? : up=??? : lo=??? */
  { 0xe2828e , 0xe2828e , 0xe2828e , None , ")" }, /* ??? : up=??? : lo=??? */
  { 0xe2828f , 0xe2828f , 0xe2828f , None , "\xe2\x82\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe28290 , 0xe28290 , 0xe28290 , None , "a" }, /* ??? : up=??? : lo=??? */
  { 0xe28291 , 0xe28291 , 0xe28291 , None , "e" }, /* ??? : up=??? : lo=??? */
  { 0xe28292 , 0xe28292 , 0xe28292 , None , "o" }, /* ??? : up=??? : lo=??? */
  { 0xe28293 , 0xe28293 , 0xe28293 , None , "x" }, /* ??? : up=??? : lo=??? */
  { 0xe28294 , 0xe28294 , 0xe28294 , None , "\xe2\x82\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe28295 , 0xe28295 , 0xe28295 , None , "h" }, /* ??? : up=??? : lo=??? */
  { 0xe28296 , 0xe28296 , 0xe28296 , None , "k" }, /* ??? : up=??? : lo=??? */
  { 0xe28297 , 0xe28297 , 0xe28297 , None , "l" }, /* ??? : up=??? : lo=??? */
  { 0xe28298 , 0xe28298 , 0xe28298 , None , "m" }, /* ??? : up=??? : lo=??? */
  { 0xe28299 , 0xe28299 , 0xe28299 , None , "n" }, /* ??? : up=??? : lo=??? */
  { 0xe2829a , 0xe2829a , 0xe2829a , None , "p" }, /* ??? : up=??? : lo=??? */
  { 0xe2829b , 0xe2829b , 0xe2829b , None , "s" }, /* ??? : up=??? : lo=??? */
  { 0xe2829c , 0xe2829c , 0xe2829c , None , "t" }, /* ??? : up=??? : lo=??? */
  { 0xe2829d , 0xe2829d , 0xe2829d , None , "\xe2\x82\x9d" },
  { 0xe2829e , 0xe2829e , 0xe2829e , None , "\xe2\x82\x9e" },
  { 0xe2829f , 0xe2829f , 0xe2829f , None , "\xe2\x82\x9f" },
  { 0xe282a0 , 0xe282a0 , 0xe282a0 , None , "CE" }, /* ??? : up=??? : lo=??? */
  { 0xe282a1 , 0xe282a1 , 0xe282a1 , None , "C=" }, /* ??? : up=??? : lo=??? */
  { 0xe282a2 , 0xe282a2 , 0xe282a2 , None , "Cr" }, /* ??? : up=??? : lo=??? */
  { 0xe282a3 , 0xe282a3 , 0xe282a3 , None , "Fr." }, /* ??? : up=??? : lo=??? */
  { 0xe282a4 , 0xe282a4 , 0xe282a4 , None , "L." }, /* ??? : up=??? : lo=??? */
  { 0xe282a5 , 0xe282a5 , 0xe282a5 , None , "\xe2\x82\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe282a6 , 0xe282a6 , 0xe282a6 , None , "\xe2\x82\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe282a7 , 0xe282a7 , 0xe282a7 , None , "Pts" }, /* ??? : up=??? : lo=??? */
  { 0xe282a8 , 0xe282a8 , 0xe282a8 , None , "Rs" }, /* ??? : up=??? : lo=??? */
  { 0xe282a9 , 0xe282a9 , 0xe282a9 , None , "KRW" }, /* ??? : up=??? : lo=??? */
  { 0xe282aa , 0xe282aa , 0xe282aa , None , "ILS" }, /* ??? : up=??? : lo=??? */
  { 0xe282ab , 0xe282ab , 0xe282ab , None , "Dong" }, /* ??? : up=??? : lo=??? */
  { 0xe282ac , 0xe282ac , 0xe282ac , None , "EUR" }, /* ??? : up=??? : lo=??? */
  { 0xe282ad , 0xe282ad , 0xe282ad , None , "\xe2\x82\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe282ae , 0xe282ae , 0xe282ae , None , "\xe2\x82\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe282af , 0xe282af , 0xe282af , None , "GRD" }, /* ??? : up=??? : lo=??? */
  { 0xe282b0 , 0xe282b0 , 0xe282b0 , None , "\xe2\x82\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe282b1 , 0xe282b1 , 0xe282b1 , None , "PHP" }, /* ??? : up=??? : lo=??? */
  { 0xe282b2 , 0xe282b2 , 0xe282b2 , None , "\xe2\x82\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe282b3 , 0xe282b3 , 0xe282b3 , None , "\xe2\x82\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe282b4 , 0xe282b4 , 0xe282b4 , None , "UAH" }, /* ??? : up=??? : lo=??? */
  { 0xe282b5 , 0xe282b5 , 0xe282b5 , None , "\xe2\x82\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe282b6 , 0xe282b6 , 0xe282b6 , None , "\xe2\x82\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe282b7 , 0xe282b7 , 0xe282b7 , None , "\xe2\x82\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe282b8 , 0xe282b8 , 0xe282b8 , None , "KZT" }, /* ??? : up=??? : lo=??? */
  { 0xe282b9 , 0xe282b9 , 0xe282b9 , None , "INR" }, /* ??? : up=??? : lo=??? */
  { 0xe282ba , 0xe282ba , 0xe282ba , None , "TL" }, /* ??? : up=??? : lo=??? */
  { 0xe282bb , 0xe282bb , 0xe282bb , None , "\xe2\x82\xbb" },
  { 0xe282bc , 0xe282bc , 0xe282bc , None , "\xe2\x82\xbc" },
  { 0xe282bd , 0xe282bd , 0xe282bd , None , "RUB" }, /* ??? : up=??? : lo=??? */
  { 0xe282be , 0xe282be , 0xe282be , None , "GEL" },
  { 0xe282bf , 0xe282bf , 0xe282bf , None , "\xe2\x82\xbf" }, /* ??? : up=??? : lo=??? */
};

const character charmap_e2_b4[64] = {
  { 0xe2b480 , 0xe182a0 , 0xe2b480 , None , "\xe2\xb4\x80" }, /* ??? : up=??? : lo=??? */
  { 0xe2b481 , 0xe182a1 , 0xe2b481 , None , "\xe2\xb4\x81" }, /* ??? : up=??? : lo=??? */
  { 0xe2b482 , 0xe182a2 , 0xe2b482 , None , "\xe2\xb4\x82" }, /* ??? : up=??? : lo=??? */
  { 0xe2b483 , 0xe182a3 , 0xe2b483 , None , "\xe2\xb4\x83" }, /* ??? : up=??? : lo=??? */
  { 0xe2b484 , 0xe182a4 , 0xe2b484 , None , "\xe2\xb4\x84" }, /* ??? : up=??? : lo=??? */
  { 0xe2b485 , 0xe182a5 , 0xe2b485 , None , "\xe2\xb4\x85" }, /* ??? : up=??? : lo=??? */
  { 0xe2b486 , 0xe182a6 , 0xe2b486 , None , "\xe2\xb4\x86" }, /* ??? : up=??? : lo=??? */
  { 0xe2b487 , 0xe182a7 , 0xe2b487 , None , "\xe2\xb4\x87" }, /* ??? : up=??? : lo=??? */
  { 0xe2b488 , 0xe182a8 , 0xe2b488 , None , "\xe2\xb4\x88" }, /* ??? : up=??? : lo=??? */
  { 0xe2b489 , 0xe182a9 , 0xe2b489 , None , "\xe2\xb4\x89" }, /* ??? : up=??? : lo=??? */
  { 0xe2b48a , 0xe182aa , 0xe2b48a , None , "\xe2\xb4\x8a" }, /* ??? : up=??? : lo=??? */
  { 0xe2b48b , 0xe182ab , 0xe2b48b , None , "\xe2\xb4\x8b" }, /* ??? : up=??? : lo=??? */
  { 0xe2b48c , 0xe182ac , 0xe2b48c , None , "\xe2\xb4\x8c" }, /* ??? : up=??? : lo=??? */
  { 0xe2b48d , 0xe182ad , 0xe2b48d , None , "\xe2\xb4\x8d" }, /* ??? : up=??? : lo=??? */
  { 0xe2b48e , 0xe182ae , 0xe2b48e , None , "\xe2\xb4\x8e" }, /* ??? : up=??? : lo=??? */
  { 0xe2b48f , 0xe182af , 0xe2b48f , None , "\xe2\xb4\x8f" }, /* ??? : up=??? : lo=??? */
  { 0xe2b490 , 0xe182b0 , 0xe2b490 , None , "\xe2\xb4\x90" }, /* ??? : up=??? : lo=??? */
  { 0xe2b491 , 0xe182b1 , 0xe2b491 , None , "\xe2\xb4\x91" }, /* ??? : up=??? : lo=??? */
  { 0xe2b492 , 0xe182b2 , 0xe2b492 , None , "\xe2\xb4\x92" }, /* ??? : up=??? : lo=??? */
  { 0xe2b493 , 0xe182b3 , 0xe2b493 , None , "\xe2\xb4\x93" }, /* ??? : up=??? : lo=??? */
  { 0xe2b494 , 0xe182b4 , 0xe2b494 , None , "\xe2\xb4\x94" }, /* ??? : up=??? : lo=??? */
  { 0xe2b495 , 0xe182b5 , 0xe2b495 , None , "\xe2\xb4\x95" }, /* ??? : up=??? : lo=??? */
  { 0xe2b496 , 0xe182b6 , 0xe2b496 , None , "\xe2\xb4\x96" }, /* ??? : up=??? : lo=??? */
  { 0xe2b497 , 0xe182b7 , 0xe2b497 , None , "\xe2\xb4\x97" }, /* ??? : up=??? : lo=??? */
  { 0xe2b498 , 0xe182b8 , 0xe2b498 , None , "\xe2\xb4\x98" }, /* ??? : up=??? : lo=??? */
  { 0xe2b499 , 0xe182b9 , 0xe2b499 , None , "\xe2\xb4\x99" }, /* ??? : up=??? : lo=??? */
  { 0xe2b49a , 0xe182ba , 0xe2b49a , None , "\xe2\xb4\x9a" }, /* ??? : up=??? : lo=??? */
  { 0xe2b49b , 0xe182bb , 0xe2b49b , None , "\xe2\xb4\x9b" }, /* ??? : up=??? : lo=??? */
  { 0xe2b49c , 0xe182bc , 0xe2b49c , None , "\xe2\xb4\x9c" }, /* ??? : up=??? : lo=??? */
  { 0xe2b49d , 0xe182bd , 0xe2b49d , None , "\xe2\xb4\x9d" }, /* ??? : up=??? : lo=??? */
  { 0xe2b49e , 0xe182be , 0xe2b49e , None , "\xe2\xb4\x9e" }, /* ??? : up=??? : lo=??? */
  { 0xe2b49f , 0xe182bf , 0xe2b49f , None , "\xe2\xb4\x9f" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a0 , 0xe18380 , 0xe2b4a0 , None , "\xe2\xb4\xa0" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a1 , 0xe18381 , 0xe2b4a1 , None , "\xe2\xb4\xa1" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a2 , 0xe18382 , 0xe2b4a2 , None , "\xe2\xb4\xa2" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a3 , 0xe18383 , 0xe2b4a3 , None , "\xe2\xb4\xa3" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a4 , 0xe18384 , 0xe2b4a4 , None , "\xe2\xb4\xa4" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a5 , 0xe18385 , 0xe2b4a5 , None , "\xe2\xb4\xa5" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a6 , 0xe2b4a6 , 0xe2b4a6 , None , "\xe2\xb4\xa6" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a7 , 0xe18387 , 0xe2b4a7 , None , "\xe2\xb4\xa7" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a8 , 0xe2b4a8 , 0xe2b4a8 , None , "\xe2\xb4\xa8" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4a9 , 0xe2b4a9 , 0xe2b4a9 , None , "\xe2\xb4\xa9" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4aa , 0xe2b4aa , 0xe2b4aa , None , "\xe2\xb4\xaa" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4ab , 0xe2b4ab , 0xe2b4ab , None , "\xe2\xb4\xab" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4ac , 0xe2b4ac , 0xe2b4ac , None , "\xe2\xb4\xac" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4ad , 0xe1838d , 0xe2b4ad , None , "\xe2\xb4\xad" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4ae , 0xe2b4ae , 0xe2b4ae , None , "\xe2\xb4\xae" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4af , 0xe2b4af , 0xe2b4af , None , "\xe2\xb4\xaf" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b0 , 0xe2b4b0 , 0xe2b4b0 , None , "\xe2\xb4\xb0" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b1 , 0xe2b4b1 , 0xe2b4b1 , None , "\xe2\xb4\xb1" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b2 , 0xe2b4b2 , 0xe2b4b2 , None , "\xe2\xb4\xb2" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b3 , 0xe2b4b3 , 0xe2b4b3 , None , "\xe2\xb4\xb3" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b4 , 0xe2b4b4 , 0xe2b4b4 , None , "\xe2\xb4\xb4" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b5 , 0xe2b4b5 , 0xe2b4b5 , None , "\xe2\xb4\xb5" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b6 , 0xe2b4b6 , 0xe2b4b6 , None , "\xe2\xb4\xb6" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b7 , 0xe2b4b7 , 0xe2b4b7 , None , "\xe2\xb4\xb7" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b8 , 0xe2b4b8 , 0xe2b4b8 , None , "\xe2\xb4\xb8" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4b9 , 0xe2b4b9 , 0xe2b4b9 , None , "\xe2\xb4\xb9" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4ba , 0xe2b4ba , 0xe2b4ba , None , "\xe2\xb4\xba" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4bb , 0xe2b4bb , 0xe2b4bb , None , "\xe2\xb4\xbb" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4bc , 0xe2b4bc , 0xe2b4bc , None , "\xe2\xb4\xbc" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4bd , 0xe2b4bd , 0xe2b4bd , None , "\xe2\xb4\xbd" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4be , 0xe2b4be , 0xe2b4be , None , "\xe2\xb4\xbe" }, /* ??? : up=??? : lo=??? */
  { 0xe2b4bf , 0xe2b4bf , 0xe2b4bf , None , "\xe2\xb4\xbf" }, /* ??? : up=??? : lo=??? */
};

const character* pagemap_32_f0_90[64] = {
  /* 80 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, charmap_f0_90_92, charmap_f0_90_93, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
};

const character charmap_f0_90_92[64] = {
  { 0xf0909280 , 0xf0909280 , 0xf0909280 , None , "\xf0\x90\x92\x80" }, /* ???? : up=???? : lo=???? */
  { 0xf0909281 , 0xf0909281 , 0xf0909281 , None , "\xf0\x90\x92\x81" }, /* ???? : up=???? : lo=???? */
  { 0xf0909282 , 0xf0909282 , 0xf0909282 , None , "\xf0\x90\x92\x82" }, /* ???? : up=???? : lo=???? */
  { 0xf0909283 , 0xf0909283 , 0xf0909283 , None , "\xf0\x90\x92\x83" }, /* ???? : up=???? : lo=???? */
  { 0xf0909284 , 0xf0909284 , 0xf0909284 , None , "\xf0\x90\x92\x84" }, /* ???? : up=???? : lo=???? */
  { 0xf0909285 , 0xf0909285 , 0xf0909285 , None , "\xf0\x90\x92\x85" }, /* ???? : up=???? : lo=???? */
  { 0xf0909286 , 0xf0909286 , 0xf0909286 , None , "\xf0\x90\x92\x86" }, /* ???? : up=???? : lo=???? */
  { 0xf0909287 , 0xf0909287 , 0xf0909287 , None , "\xf0\x90\x92\x87" }, /* ???? : up=???? : lo=???? */
  { 0xf0909288 , 0xf0909288 , 0xf0909288 , None , "\xf0\x90\x92\x88" }, /* ???? : up=???? : lo=???? */
  { 0xf0909289 , 0xf0909289 , 0xf0909289 , None , "\xf0\x90\x92\x89" }, /* ???? : up=???? : lo=???? */
  { 0xf090928a , 0xf090928a , 0xf090928a , None , "\xf0\x90\x92\x8a" }, /* ???? : up=???? : lo=???? */
  { 0xf090928b , 0xf090928b , 0xf090928b , None , "\xf0\x90\x92\x8b" }, /* ???? : up=???? : lo=???? */
  { 0xf090928c , 0xf090928c , 0xf090928c , None , "\xf0\x90\x92\x8c" }, /* ???? : up=???? : lo=???? */
  { 0xf090928d , 0xf090928d , 0xf090928d , None , "\xf0\x90\x92\x8d" }, /* ???? : up=???? : lo=???? */
  { 0xf090928e , 0xf090928e , 0xf090928e , None , "\xf0\x90\x92\x8e" }, /* ???? : up=???? : lo=???? */
  { 0xf090928f , 0xf090928f , 0xf090928f , None , "\xf0\x90\x92\x8f" }, /* ???? : up=???? : lo=???? */
  { 0xf0909290 , 0xf0909290 , 0xf0909290 , None , "\xf0\x90\x92\x90" }, /* ???? : up=???? : lo=???? */
  { 0xf0909291 , 0xf0909291 , 0xf0909291 , None , "\xf0\x90\x92\x91" }, /* ???? : up=???? : lo=???? */
  { 0xf0909292 , 0xf0909292 , 0xf0909292 , None , "\xf0\x90\x92\x92" }, /* ???? : up=???? : lo=???? */
  { 0xf0909293 , 0xf0909293 , 0xf0909293 , None , "\xf0\x90\x92\x93" }, /* ???? : up=???? : lo=???? */
  { 0xf0909294 , 0xf0909294 , 0xf0909294 , None , "\xf0\x90\x92\x94" }, /* ???? : up=???? : lo=???? */
  { 0xf0909295 , 0xf0909295 , 0xf0909295 , None , "\xf0\x90\x92\x95" }, /* ???? : up=???? : lo=???? */
  { 0xf0909296 , 0xf0909296 , 0xf0909296 , None , "\xf0\x90\x92\x96" }, /* ???? : up=???? : lo=???? */
  { 0xf0909297 , 0xf0909297 , 0xf0909297 , None , "\xf0\x90\x92\x97" }, /* ???? : up=???? : lo=???? */
  { 0xf0909298 , 0xf0909298 , 0xf0909298 , None , "\xf0\x90\x92\x98" }, /* ???? : up=???? : lo=???? */
  { 0xf0909299 , 0xf0909299 , 0xf0909299 , None , "\xf0\x90\x92\x99" }, /* ???? : up=???? : lo=???? */
  { 0xf090929a , 0xf090929a , 0xf090929a , None , "\xf0\x90\x92\x9a" }, /* ???? : up=???? : lo=???? */
  { 0xf090929b , 0xf090929b , 0xf090929b , None , "\xf0\x90\x92\x9b" }, /* ???? : up=???? : lo=???? */
  { 0xf090929c , 0xf090929c , 0xf090929c , None , "\xf0\x90\x92\x9c" }, /* ???? : up=???? : lo=???? */
  { 0xf090929d , 0xf090929d , 0xf090929d , None , "\xf0\x90\x92\x9d" }, /* ???? : up=???? : lo=???? */
  { 0xf090929e , 0xf090929e , 0xf090929e , None , "\xf0\x90\x92\x9e" }, /* ???? : up=???? : lo=???? */
  { 0xf090929f , 0xf090929f , 0xf090929f , None , "\xf0\x90\x92\x9f" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a0 , 0xf09092a0 , 0xf09092a0 , None , "\xf0\x90\x92\xa0" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a1 , 0xf09092a1 , 0xf09092a1 , None , "\xf0\x90\x92\xa1" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a2 , 0xf09092a2 , 0xf09092a2 , None , "\xf0\x90\x92\xa2" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a3 , 0xf09092a3 , 0xf09092a3 , None , "\xf0\x90\x92\xa3" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a4 , 0xf09092a4 , 0xf09092a4 , None , "\xf0\x90\x92\xa4" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a5 , 0xf09092a5 , 0xf09092a5 , None , "\xf0\x90\x92\xa5" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a6 , 0xf09092a6 , 0xf09092a6 , None , "\xf0\x90\x92\xa6" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a7 , 0xf09092a7 , 0xf09092a7 , None , "\xf0\x90\x92\xa7" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a8 , 0xf09092a8 , 0xf09092a8 , None , "\xf0\x90\x92\xa8" }, /* ???? : up=???? : lo=???? */
  { 0xf09092a9 , 0xf09092a9 , 0xf09092a9 , None , "\xf0\x90\x92\xa9" }, /* ???? : up=???? : lo=???? */
  { 0xf09092aa , 0xf09092aa , 0xf09092aa , None , "\xf0\x90\x92\xaa" }, /* ???? : up=???? : lo=???? */
  { 0xf09092ab , 0xf09092ab , 0xf09092ab , None , "\xf0\x90\x92\xab" }, /* ???? : up=???? : lo=???? */
  { 0xf09092ac , 0xf09092ac , 0xf09092ac , None , "\xf0\x90\x92\xac" }, /* ???? : up=???? : lo=???? */
  { 0xf09092ad , 0xf09092ad , 0xf09092ad , None , "\xf0\x90\x92\xad" }, /* ???? : up=???? : lo=???? */
  { 0xf09092ae , 0xf09092ae , 0xf09092ae , None , "\xf0\x90\x92\xae" }, /* ???? : up=???? : lo=???? */
  { 0xf09092af , 0xf09092af , 0xf09092af , None , "\xf0\x90\x92\xaf" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b0 , 0xf09092b0 , 0xf0909398 , None , "\xf0\x90\x92\xb0" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b1 , 0xf09092b1 , 0xf0909399 , None , "\xf0\x90\x92\xb1" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b2 , 0xf09092b2 , 0xf090939a , None , "\xf0\x90\x92\xb2" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b3 , 0xf09092b3 , 0xf090939b , None , "\xf0\x90\x92\xb3" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b4 , 0xf09092b4 , 0xf090939c , None , "\xf0\x90\x92\xb4" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b5 , 0xf09092b5 , 0xf090939d , None , "\xf0\x90\x92\xb5" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b6 , 0xf09092b6 , 0xf090939e , None , "\xf0\x90\x92\xb6" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b7 , 0xf09092b7 , 0xf090939f , None , "\xf0\x90\x92\xb7" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b8 , 0xf09092b8 , 0xf09093a0 , None , "\xf0\x90\x92\xb8" }, /* ???? : up=???? : lo=???? */
  { 0xf09092b9 , 0xf09092b9 , 0xf09093a1 , None , "\xf0\x90\x92\xb9" }, /* ???? : up=???? : lo=???? */
  { 0xf09092ba , 0xf09092ba , 0xf09093a2 , None , "\xf0\x90\x92\xba" }, /* ???? : up=???? : lo=???? */
  { 0xf09092bb , 0xf09092bb , 0xf09093a3 , None , "\xf0\x90\x92\xbb" }, /* ???? : up=???? : lo=???? */
  { 0xf09092bc , 0xf09092bc , 0xf09093a4 , None , "\xf0\x90\x92\xbc" }, /* ???? : up=???? : lo=???? */
  { 0xf09092bd , 0xf09092bd , 0xf09093a5 , None , "\xf0\x90\x92\xbd" }, /* ???? : up=???? : lo=???? */
  { 0xf09092be , 0xf09092be , 0xf09093a6 , None , "\xf0\x90\x92\xbe" }, /* ???? : up=???? : lo=???? */
  { 0xf09092bf , 0xf09092bf , 0xf09093a7 , None , "\xf0\x90\x92\xbf" }, /* ???? : up=???? : lo=???? */
};

const character charmap_f0_90_93[64] = {
  { 0xf0909380 , 0xf0909380 , 0xf09093a8 , None , "\xf0\x90\x93\x80" }, /* ???? : up=???? : lo=???? */
  { 0xf0909381 , 0xf0909381 , 0xf09093a9 , None , "\xf0\x90\x93\x81" }, /* ???? : up=???? : lo=???? */
  { 0xf0909382 , 0xf0909382 , 0xf09093aa , None , "\xf0\x90\x93\x82" }, /* ???? : up=???? : lo=???? */
  { 0xf0909383 , 0xf0909383 , 0xf09093ab , None , "\xf0\x90\x93\x83" }, /* ???? : up=???? : lo=???? */
  { 0xf0909384 , 0xf0909384 , 0xf09093ac , None , "\xf0\x90\x93\x84" }, /* ???? : up=???? : lo=???? */
  { 0xf0909385 , 0xf0909385 , 0xf09093ad , None , "\xf0\x90\x93\x85" }, /* ???? : up=???? : lo=???? */
  { 0xf0909386 , 0xf0909386 , 0xf09093ae , None , "\xf0\x90\x93\x86" }, /* ???? : up=???? : lo=???? */
  { 0xf0909387 , 0xf0909387 , 0xf09093af , None , "\xf0\x90\x93\x87" }, /* ???? : up=???? : lo=???? */
  { 0xf0909388 , 0xf0909388 , 0xf09093b0 , None , "\xf0\x90\x93\x88" }, /* ???? : up=???? : lo=???? */
  { 0xf0909389 , 0xf0909389 , 0xf09093b1 , None , "\xf0\x90\x93\x89" }, /* ???? : up=???? : lo=???? */
  { 0xf090938a , 0xf090938a , 0xf09093b2 , None , "\xf0\x90\x93\x8a" }, /* ???? : up=???? : lo=???? */
  { 0xf090938b , 0xf090938b , 0xf09093b3 , None , "\xf0\x90\x93\x8b" }, /* ???? : up=???? : lo=???? */
  { 0xf090938c , 0xf090938c , 0xf09093b4 , None , "\xf0\x90\x93\x8c" }, /* ???? : up=???? : lo=???? */
  { 0xf090938d , 0xf090938d , 0xf09093b5 , None , "\xf0\x90\x93\x8d" }, /* ???? : up=???? : lo=???? */
  { 0xf090938e , 0xf090938e , 0xf09093b6 , None , "\xf0\x90\x93\x8e" }, /* ???? : up=???? : lo=???? */
  { 0xf090938f , 0xf090938f , 0xf09093b7 , None , "\xf0\x90\x93\x8f" }, /* ???? : up=???? : lo=???? */
  { 0xf0909390 , 0xf0909390 , 0xf09093b8 , None , "\xf0\x90\x93\x90" }, /* ???? : up=???? : lo=???? */
  { 0xf0909391 , 0xf0909391 , 0xf09093b9 , None , "\xf0\x90\x93\x91" }, /* ???? : up=???? : lo=???? */
  { 0xf0909392 , 0xf0909392 , 0xf09093ba , None , "\xf0\x90\x93\x92" }, /* ???? : up=???? : lo=???? */
  { 0xf0909393 , 0xf0909393 , 0xf09093bb , None , "\xf0\x90\x93\x93" }, /* ???? : up=???? : lo=???? */
  { 0xf0909394 , 0xf0909394 , 0xf0909394 , None , "\xf0\x90\x93\x94" }, /* ???? : up=???? : lo=???? */
  { 0xf0909395 , 0xf0909395 , 0xf0909395 , None , "\xf0\x90\x93\x95" }, /* ???? : up=???? : lo=???? */
  { 0xf0909396 , 0xf0909396 , 0xf0909396 , None , "\xf0\x90\x93\x96" }, /* ???? : up=???? : lo=???? */
  { 0xf0909397 , 0xf0909397 , 0xf0909397 , None , "\xf0\x90\x93\x97" }, /* ???? : up=???? : lo=???? */
  { 0xf0909398 , 0xf09092b0 , 0xf0909398 , None , "\xf0\x90\x93\x98" }, /* ???? : up=???? : lo=???? */
  { 0xf0909399 , 0xf09092b1 , 0xf0909399 , None , "\xf0\x90\x93\x99" }, /* ???? : up=???? : lo=???? */
  { 0xf090939a , 0xf09092b2 , 0xf090939a , None , "\xf0\x90\x93\x9a" }, /* ???? : up=???? : lo=???? */
  { 0xf090939b , 0xf09092b3 , 0xf090939b , None , "\xf0\x90\x93\x9b" }, /* ???? : up=???? : lo=???? */
  { 0xf090939c , 0xf09092b4 , 0xf090939c , None , "\xf0\x90\x93\x9c" }, /* ???? : up=???? : lo=???? */
  { 0xf090939d , 0xf09092b5 , 0xf090939d , None , "\xf0\x90\x93\x9d" }, /* ???? : up=???? : lo=???? */
  { 0xf090939e , 0xf09092b6 , 0xf090939e , None , "\xf0\x90\x93\x9e" }, /* ???? : up=???? : lo=???? */
  { 0xf090939f , 0xf09092b7 , 0xf090939f , None , "\xf0\x90\x93\x9f" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a0 , 0xf09092b8 , 0xf09093a0 , None , "\xf0\x90\x93\xa0" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a1 , 0xf09092b9 , 0xf09093a1 , None , "\xf0\x90\x93\xa1" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a2 , 0xf09092ba , 0xf09093a2 , None , "\xf0\x90\x93\xa2" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a3 , 0xf09092bb , 0xf09093a3 , None , "\xf0\x90\x93\xa3" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a4 , 0xf09092bc , 0xf09093a4 , None , "\xf0\x90\x93\xa4" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a5 , 0xf09092bd , 0xf09093a5 , None , "\xf0\x90\x93\xa5" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a6 , 0xf09092be , 0xf09093a6 , None , "\xf0\x90\x93\xa6" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a7 , 0xf09092bf , 0xf09093a7 , None , "\xf0\x90\x93\xa7" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a8 , 0xf0909380 , 0xf09093a8 , None , "\xf0\x90\x93\xa8" }, /* ???? : up=???? : lo=???? */
  { 0xf09093a9 , 0xf0909381 , 0xf09093a9 , None , "\xf0\x90\x93\xa9" }, /* ???? : up=???? : lo=???? */
  { 0xf09093aa , 0xf0909382 , 0xf09093aa , None , "\xf0\x90\x93\xaa" }, /* ???? : up=???? : lo=???? */
  { 0xf09093ab , 0xf0909383 , 0xf09093ab , None , "\xf0\x90\x93\xab" }, /* ???? : up=???? : lo=???? */
  { 0xf09093ac , 0xf0909384 , 0xf09093ac , None , "\xf0\x90\x93\xac" }, /* ???? : up=???? : lo=???? */
  { 0xf09093ad , 0xf0909385 , 0xf09093ad , None , "\xf0\x90\x93\xad" }, /* ???? : up=???? : lo=???? */
  { 0xf09093ae , 0xf0909386 , 0xf09093ae , None , "\xf0\x90\x93\xae" }, /* ???? : up=???? : lo=???? */
  { 0xf09093af , 0xf0909387 , 0xf09093af , None , "\xf0\x90\x93\xaf" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b0 , 0xf0909388 , 0xf09093b0 , None , "\xf0\x90\x93\xb0" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b1 , 0xf0909389 , 0xf09093b1 , None , "\xf0\x90\x93\xb1" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b2 , 0xf090938a , 0xf09093b2 , None , "\xf0\x90\x93\xb2" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b3 , 0xf090938b , 0xf09093b3 , None , "\xf0\x90\x93\xb3" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b4 , 0xf090938c , 0xf09093b4 , None , "\xf0\x90\x93\xb4" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b5 , 0xf090938d , 0xf09093b5 , None , "\xf0\x90\x93\xb5" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b6 , 0xf090938e , 0xf09093b6 , None , "\xf0\x90\x93\xb6" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b7 , 0xf090938f , 0xf09093b7 , None , "\xf0\x90\x93\xb7" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b8 , 0xf0909390 , 0xf09093b8 , None , "\xf0\x90\x93\xb8" }, /* ???? : up=???? : lo=???? */
  { 0xf09093b9 , 0xf0909391 , 0xf09093b9 , None , "\xf0\x90\x93\xb9" }, /* ???? : up=???? : lo=???? */
  { 0xf09093ba , 0xf0909392 , 0xf09093ba , None , "\xf0\x90\x93\xba" }, /* ???? : up=???? : lo=???? */
  { 0xf09093bb , 0xf0909393 , 0xf09093bb , None , "\xf0\x90\x93\xbb" }, /* ???? : up=???? : lo=???? */
  { 0xf09093bc , 0xf09093bc , 0xf09093bc , None , "\xf0\x90\x93\xbc" }, /* ???? : up=???? : lo=???? */
  { 0xf09093bd , 0xf09093bd , 0xf09093bd , None , "\xf0\x90\x93\xbd" }, /* ???? : up=???? : lo=???? */
  { 0xf09093be , 0xf09093be , 0xf09093be , None , "\xf0\x90\x93\xbe" }, /* ???? : up=???? : lo=???? */
  { 0xf09093bf , 0xf09093bf , 0xf09093bf , None , "\xf0\x90\x93\xbf" }, /* ???? : up=???? : lo=???? */
};

const character* pagemap_32_f0_9e[64] = {
  /* 80 */
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, charmap_f0_9e_a4, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
};

const character charmap_f0_9e_a4[64] = {
  { 0xf09ea480 , 0xf09ea480 , 0xf09ea4a2 , None , "\xf0\x9e\xa4\x80" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea481 , 0xf09ea481 , 0xf09ea4a3 , None , "\xf0\x9e\xa4\x81" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea482 , 0xf09ea482 , 0xf09ea4a4 , None , "\xf0\x9e\xa4\x82" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea483 , 0xf09ea483 , 0xf09ea4a5 , None , "\xf0\x9e\xa4\x83" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea484 , 0xf09ea484 , 0xf09ea4a6 , None , "\xf0\x9e\xa4\x84" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea485 , 0xf09ea485 , 0xf09ea4a7 , None , "\xf0\x9e\xa4\x85" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea486 , 0xf09ea486 , 0xf09ea4a8 , None , "\xf0\x9e\xa4\x86" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea487 , 0xf09ea487 , 0xf09ea4a9 , None , "\xf0\x9e\xa4\x87" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea488 , 0xf09ea488 , 0xf09ea4aa , None , "\xf0\x9e\xa4\x88" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea489 , 0xf09ea489 , 0xf09ea4ab , None , "\xf0\x9e\xa4\x89" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea48a , 0xf09ea48a , 0xf09ea4ac , None , "\xf0\x9e\xa4\x8a" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea48b , 0xf09ea48b , 0xf09ea4ad , None , "\xf0\x9e\xa4\x8b" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea48c , 0xf09ea48c , 0xf09ea4ae , None , "\xf0\x9e\xa4\x8c" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea48d , 0xf09ea48d , 0xf09ea4af , None , "\xf0\x9e\xa4\x8d" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea48e , 0xf09ea48e , 0xf09ea4b0 , None , "\xf0\x9e\xa4\x8e" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea48f , 0xf09ea48f , 0xf09ea4b1 , None , "\xf0\x9e\xa4\x8f" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea490 , 0xf09ea490 , 0xf09ea4b2 , None , "\xf0\x9e\xa4\x90" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea491 , 0xf09ea491 , 0xf09ea4b3 , None , "\xf0\x9e\xa4\x91" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea492 , 0xf09ea492 , 0xf09ea4b4 , None , "\xf0\x9e\xa4\x92" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea493 , 0xf09ea493 , 0xf09ea4b5 , None , "\xf0\x9e\xa4\x93" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea494 , 0xf09ea494 , 0xf09ea4b6 , None , "\xf0\x9e\xa4\x94" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea495 , 0xf09ea495 , 0xf09ea4b7 , None , "\xf0\x9e\xa4\x95" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea496 , 0xf09ea496 , 0xf09ea4b8 , None , "\xf0\x9e\xa4\x96" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea497 , 0xf09ea497 , 0xf09ea4b9 , None , "\xf0\x9e\xa4\x97" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea498 , 0xf09ea498 , 0xf09ea4ba , None , "\xf0\x9e\xa4\x98" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea499 , 0xf09ea499 , 0xf09ea4bb , None , "\xf0\x9e\xa4\x99" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea49a , 0xf09ea49a , 0xf09ea4bc , None , "\xf0\x9e\xa4\x9a" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea49b , 0xf09ea49b , 0xf09ea4bd , None , "\xf0\x9e\xa4\x9b" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea49c , 0xf09ea49c , 0xf09ea4be , None , "\xf0\x9e\xa4\x9c" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea49d , 0xf09ea49d , 0xf09ea4bf , None , "\xf0\x9e\xa4\x9d" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea49e , 0xf09ea49e , 0xf09ea580 , None , "\xf0\x9e\xa4\x9e" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea49f , 0xf09ea49f , 0xf09ea581 , None , "\xf0\x9e\xa4\x9f" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a0 , 0xf09ea4a0 , 0xf09ea582 , None , "\xf0\x9e\xa4\xa0" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a1 , 0xf09ea4a1 , 0xf09ea583 , None , "\xf0\x9e\xa4\xa1" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a2 , 0xf09ea480 , 0xf09ea4a2 , None , "\xf0\x9e\xa4\xa2" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a3 , 0xf09ea481 , 0xf09ea4a3 , None , "\xf0\x9e\xa4\xa3" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a4 , 0xf09ea482 , 0xf09ea4a4 , None , "\xf0\x9e\xa4\xa4" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a5 , 0xf09ea483 , 0xf09ea4a5 , None , "\xf0\x9e\xa4\xa5" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a6 , 0xf09ea484 , 0xf09ea4a6 , None , "\xf0\x9e\xa4\xa6" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a7 , 0xf09ea485 , 0xf09ea4a7 , None , "\xf0\x9e\xa4\xa7" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a8 , 0xf09ea486 , 0xf09ea4a8 , None , "\xf0\x9e\xa4\xa8" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4a9 , 0xf09ea487 , 0xf09ea4a9 , None , "\xf0\x9e\xa4\xa9" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4aa , 0xf09ea488 , 0xf09ea4aa , None , "\xf0\x9e\xa4\xaa" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4ab , 0xf09ea489 , 0xf09ea4ab , None , "\xf0\x9e\xa4\xab" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4ac , 0xf09ea48a , 0xf09ea4ac , None , "\xf0\x9e\xa4\xac" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4ad , 0xf09ea48b , 0xf09ea4ad , None , "\xf0\x9e\xa4\xad" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4ae , 0xf09ea48c , 0xf09ea4ae , None , "\xf0\x9e\xa4\xae" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4af , 0xf09ea48d , 0xf09ea4af , None , "\xf0\x9e\xa4\xaf" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b0 , 0xf09ea48e , 0xf09ea4b0 , None , "\xf0\x9e\xa4\xb0" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b1 , 0xf09ea48f , 0xf09ea4b1 , None , "\xf0\x9e\xa4\xb1" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b2 , 0xf09ea490 , 0xf09ea4b2 , None , "\xf0\x9e\xa4\xb2" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b3 , 0xf09ea491 , 0xf09ea4b3 , None , "\xf0\x9e\xa4\xb3" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b4 , 0xf09ea492 , 0xf09ea4b4 , None , "\xf0\x9e\xa4\xb4" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b5 , 0xf09ea493 , 0xf09ea4b5 , None , "\xf0\x9e\xa4\xb5" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b6 , 0xf09ea494 , 0xf09ea4b6 , None , "\xf0\x9e\xa4\xb6" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b7 , 0xf09ea495 , 0xf09ea4b7 , None , "\xf0\x9e\xa4\xb7" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b8 , 0xf09ea496 , 0xf09ea4b8 , None , "\xf0\x9e\xa4\xb8" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4b9 , 0xf09ea497 , 0xf09ea4b9 , None , "\xf0\x9e\xa4\xb9" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4ba , 0xf09ea498 , 0xf09ea4ba , None , "\xf0\x9e\xa4\xba" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4bb , 0xf09ea499 , 0xf09ea4bb , None , "\xf0\x9e\xa4\xbb" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4bc , 0xf09ea49a , 0xf09ea4bc , None , "\xf0\x9e\xa4\xbc" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4bd , 0xf09ea49b , 0xf09ea4bd , None , "\xf0\x9e\xa4\xbd" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4be , 0xf09ea49c , 0xf09ea4be , None , "\xf0\x9e\xa4\xbe" }, /* ???? : up=???? : lo=???? */
  { 0xf09ea4bf , 0xf09ea49d , 0xf09ea4bf , None , "\xf0\x9e\xa4\xbf" }, /* ???? : up=???? : lo=???? */
};

}
