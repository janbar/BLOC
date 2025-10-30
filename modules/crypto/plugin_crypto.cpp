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

#include "plugin_crypto.h"
#include <blocc/value.h>
#include <blocc/debug.h>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include "aes.h"
#include "md5.h"

/*
 * Create the module CRYPTO
 */
PLUGINCREATOR(CRYPTOPlugin)

namespace bloc
{
namespace plugin
{
namespace crypto
{

/*
 * The module interface
 */

/**********************************************************************/
/*  Constructors                                                      */
/**********************************************************************/
static PLUGIN_TYPE ctor_0_args[]  = {
  { "O", 0 }, // crypto
};

static PLUGIN_TYPE ctor_1_args[]  = {
  { "L", 0 }, // string
};

static PLUGIN_CTOR ctors[] =
{
  { 0,      1,  ctor_0_args,
          "Build a new handle as a copy of the given one." },
  { 1,      1,  ctor_1_args,
          "Build a new handle for the given algorithm.\n"
          "A valid value is among AES-256, AES-192, AES-128." },
};

enum Method
{
  MD5Hash1 = 0, MD5Hash2 = 1,
  SetKey1, SetKey2, NewIV,
  Encrypt1, Encrypt2, Decrypt1,
  CBCStart, CBCEnc1, CBCEnc2, CBCEndEnc1, CBCEndEnc2, CBCDec1, CBCEndDec,
};

/**********************************************************************/
/*  Method arguments                                                  */
/*  mode:         type: decl,ndim                                     */
/**********************************************************************/
static PLUGIN_ARG string_args[]  = {
  { PLUGIN_IN,    { "L", 0 } }, // string
};

static PLUGIN_ARG bytes_args[]  = {
  { PLUGIN_IN,    { "X", 0 } }, // bytes
};

/**********************************************************************/
/*  Methods list                                                      */
/*  id:       name:         ret: decl,ndim  args_count,args:          */
/**********************************************************************/
static PLUGIN_METHOD methods[] =
{
  { MD5Hash1,     "md5",              { "X", 0 },     1, string_args,
          "Generate the MD5 hash of the given string." },
  { MD5Hash2,     "md5",              { "X", 0 },     1, bytes_args,
          "Generate the MD5 hash of the given bytes array." },
  { SetKey1,      "setkey",           { "X", 0 },     1, string_args,
          "Set the cipher key for this session. The key value is generated\n"
          "by the internal hash algorithm." },
  { SetKey2,      "setkey",           { "B", 0 },     1, bytes_args,
          "Set the cipher key for this session with the given raw value." },
  { NewIV,        "newiv",            { "X", 0 },     0, nullptr,
          "Returns a new unpredictable IV, usable by the next encryption\n"
          "session in CBC mode." },
  { Encrypt1,     "encrypt",          { "X", 0 },     1, string_args,
          "One step CBC encryption of the given string.\n"
          "The returned data starts with IV followed by the encrypted data\n"
          "with PKCS7 padding." },
  { Encrypt2,     "encrypt",          { "X", 0 },     1, bytes_args,
          "One step CBC encryption of the given bytes array.\n"
          "The returned data starts with IV followed by the encrypted data\n"
          "with PKCS7 padding." },
  { Decrypt1,     "decrypt",          { "X", 0 },     1, bytes_args,
          "One step CBC decryption of the given cipher data.\n"
          "Cipher data must be prefixed with the IV and padded using PKCS7\n"
          "padding." },
  { CBCStart,    "cbc_start",        { "B", 0 },     1, bytes_args,
          "Initialize CBC encoder with the specified IV." },
  { CBCEnc1,     "cbc_encrypt",       { "X", 0 },     1, string_args,
          "Encrypt the chunk of character data, and return the last\n"
          "completed cipher block." },
  { CBCEnc2,     "cbc_encrypt",       { "X", 0 },     1, bytes_args,
          "Encrypt the chunk of data, and return the last completed\n"
          "cipher block." },
  { CBCEndEnc1,  "cbc_end_encrypt",   { "X", 0 },     1, string_args,
          "Finalize the last cipher block with PKCS7 padding." },
  { CBCEndEnc2,  "cbc_end_encrypt",   { "X", 0 },     1, bytes_args,
          "Finalize the last cipher block with PKCS7 padding." },
  { CBCDec1,     "cbc_decrypt",       { "X", 0 },     1, bytes_args,
          "Decrypt the block of cipher, and return the last completed\n"
          "chunk of data. Note that last block must be processed with\n"
          "the method cbc_end_decrypt(x), which handles PKCS7 padding." },
  { CBCEndDec,   "cbc_end_decrypt",   { "X", 0 },     1, bytes_args,
          "Decrypt the last block of cipher with PKCS7 padding." },
};

/**
 * The state of file handle
 */
struct Handle
{
  AES_ctx _ctx;
  bool _hasKey = false;

  ~Handle() { }
  explicit Handle(AES_config config);
  explicit Handle(const Handle& other);

  void MD5_hash(bloc::TabChar& hash, const char * data, unsigned len);
  void generateIV(bloc::TabChar& iv, double random);
  bool keyFromString(bloc::TabChar& buf, const char * data, unsigned len);
  bool setKey(const bloc::TabChar& key);
  bool ECB_encrypt(bloc::TabChar& data);
  bool ECB_decrypt(bloc::TabChar& data);
  bool CBC_encrypt(bloc::TabChar& data);
  bool CBC_decrypt(bloc::TabChar& data);

  bool CBC_start(bloc::TabChar& iv);
  bool CBC_encrypt_chunk(bloc::TabChar& data);
  bool CBC_end_encrypt(bloc::TabChar& data);
  bool CBC_decrypt_chunk(bloc::TabChar& data);
  bool CBC_end_decrypt(bloc::TabChar& data);

  bloc::TabChar _chunk;
};

#define CIPHER_LENGTH(a) (AES_BLOCKLEN * (2 + (a) / AES_BLOCKLEN))

} /* namespace crypto */

void CRYPTOPlugin::declareInterface(PLUGIN_INTERFACE * interface)
{
  interface->name = "crypto";
  interface->method_count = sizeof(crypto::methods) / sizeof(PLUGIN_METHOD);
  interface->methods = crypto::methods;
  interface->ctors_count = sizeof(crypto::ctors) / sizeof(PLUGIN_CTOR);
  interface->ctors = crypto::ctors;
}

void * CRYPTOPlugin::createObject(int ctor_id, bloc::Context& ctx, const std::vector<bloc::Expression*>& args)
{
  switch (ctor_id)
  {
  case 0: /* copy ctor */
  {
    crypto::Handle * h = nullptr;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull() || (h = static_cast<crypto::Handle*>(a0.complex()->instance())) == nullptr)
       throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    crypto::Handle * n = new crypto::Handle(*h);
    return n;
  }

  case 1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    crypto::Handle * h = nullptr;
    Literal * val = a0.literal();
    if (*val == "AES-256")
      h = new crypto::Handle(crypto::AES_config::AES256);
    else if (*val == "AES-192")
      h = new crypto::Handle(crypto::AES_config::AES192);
    else if (*val == "AES-128")
      h = new crypto::Handle(crypto::AES_config::AES128);
    return h;
  }

  default: /* default ctor */
  {
    crypto::Handle * h = new crypto::Handle(crypto::AES_config::AES256);
    return h;
  }
  }

  return nullptr;
}

void CRYPTOPlugin::destroyObject(void * object)
{
  crypto::Handle * h = static_cast<crypto::Handle*>(object);
  delete h;
}

bloc::Value * CRYPTOPlugin::executeMethod(
          bloc::Complex& object_this,
          int method_id,
          bloc::Context& ctx,
          const std::vector<bloc::Expression*>& args
          )
{
  crypto::Handle * h = static_cast<crypto::Handle*>(object_this.instance());
  switch (method_id)
  {
  case crypto::MD5Hash1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    bloc::TabChar * buf = new bloc::TabChar();
    h->MD5_hash(*buf, a0.literal()->c_str(), a0.literal()->size());
    return new bloc::Value(buf);
  }

  case crypto::MD5Hash2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    bloc::TabChar * buf = new bloc::TabChar();
    h->MD5_hash(*buf, a0.tabchar()->data(), a0.tabchar()->size());
    return new bloc::Value(buf);
  }

  case crypto::SetKey1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    bloc::TabChar * tmp = new bloc::TabChar();
    if (h->keyFromString(*tmp, a0.literal()->c_str(), a0.literal()->size()) &&
      h->setKey(*tmp))
      return new bloc::Value(tmp);
    delete tmp;
    throw RuntimeError(EXC_RT_OTHER_S, "The given value is not acceptable for"
            " this encryption algorithm.");
  }

  case crypto::SetKey2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (h->setKey(*a0.tabchar()))
      return new bloc::Value(bloc::Bool(true));
    throw RuntimeError(EXC_RT_OTHER_S, "The given value is not acceptable for"
            " this encryption algorithm.");
  }

  case crypto::NewIV:
  {
    // generate new IV
    bloc::TabChar * tmp = new bloc::TabChar();
    h->generateIV(*tmp, ctx.random(1.0));
    return new bloc::Value(tmp);
  }

  default:
    if (!h->_hasKey)
      throw RuntimeError(EXC_RT_OTHER_S, "The encryption key is not set."
              " Set the key first.");
  }

  switch (method_id)
  {
  case crypto::Encrypt1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    // generate new IV
    bloc::TabChar * tmp = new bloc::TabChar();
    h->generateIV(*tmp, ctx.random(1.0));
    // plain data
    bloc::TabChar buf(a0.literal()->begin(), a0.literal()->end());
    if (h->CBC_start(*tmp) && h->CBC_encrypt(buf))
    {
      tmp->insert(tmp->end(), buf.begin(), buf.end());
      return new bloc::Value(tmp);
    }
    delete tmp;
    break;
  }

  case crypto::Encrypt2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    // generate new IV
    bloc::TabChar * tmp = new bloc::TabChar();
    h->generateIV(*tmp, ctx.random(1.0));
    // plain data
    bloc::TabChar buf(*a0.tabchar());
    if (h->CBC_start(*tmp) && h->CBC_encrypt(buf))
    {
      tmp->insert(tmp->end(), buf.begin(), buf.end());
      return new bloc::Value(tmp);
    }
    delete tmp;
    break;
  }

  case crypto::Decrypt1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    if (a0.tabchar()->size() > AES_BLOCKLEN)
    {
      // fetch IV
      bloc::TabChar tmp(a0.tabchar()->begin(), a0.tabchar()->begin() + AES_BLOCKLEN);
      // fetch cipher
      bloc::TabChar * buf = new bloc::TabChar(a0.tabchar()->begin() + AES_BLOCKLEN, a0.tabchar()->end());
      if (h->CBC_start(tmp) && h->CBC_decrypt(*buf))
        return new bloc::Value(buf);
      delete buf;
    }
    break;
  }

  case crypto::CBCStart:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull() || a0.tabchar()->size() != AES_BLOCKLEN)
      throw RuntimeError(EXC_RT_OTHER_S, "Invalid arguments.");
    if (h->CBC_start(*a0.tabchar()))
      return new bloc::Value(bloc::Bool(true));
    break;
  }

  case crypto::CBCEnc1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    bloc::TabChar * buf = new bloc::TabChar(a0.literal()->begin(), a0.literal()->end());
    if (h->CBC_encrypt_chunk(*buf))
      return new bloc::Value(buf);
    delete buf;
    break;
  }

  case crypto::CBCEnc2:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    bloc::TabChar * buf = new bloc::TabChar(*a0.tabchar());
    if (h->CBC_encrypt_chunk(*buf))
      return new bloc::Value(buf);
    delete buf;
    break;
  }

  case crypto::CBCEndEnc1:
  {
    bloc::TabChar * buf;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      buf = new bloc::TabChar();
    else
      buf = new bloc::TabChar(a0.literal()->begin(), a0.literal()->end());
    if (h->CBC_end_encrypt(*buf))
      return new bloc::Value(buf);
    delete buf;
    break;
  }

  case crypto::CBCEndEnc2:
  {
    bloc::TabChar * buf;
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      buf = new bloc::TabChar();
    else
      buf = new bloc::TabChar(*a0.tabchar());
    if (h->CBC_end_encrypt(*buf))
      return new bloc::Value(buf);
    delete buf;
    break;
  }

  case crypto::CBCDec1:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    bloc::TabChar * buf = new bloc::TabChar(*a0.tabchar());
    if (h->CBC_decrypt_chunk(*buf))
      return new bloc::Value(buf);
    delete buf;
    break;
  }

  case crypto::CBCEndDec:
  {
    bloc::Value& a0 = args[0]->value(ctx);
    if (a0.isNull())
      return new bloc::Value(bloc::Value::type_tabchar);
    bloc::TabChar * buf = new bloc::TabChar(*a0.tabchar());
    if (h->CBC_end_decrypt(*buf))
      return new bloc::Value(buf);
    delete buf;
    break;
  }

  default:
    break;
  }
  return nullptr;
}

crypto::Handle::Handle(AES_config config)
{
  AES_init_ctx(&_ctx, config);
}

crypto::Handle::Handle(const Handle& other)
{
  memcpy(&_ctx, &other._ctx, sizeof(AES_ctx));
  _hasKey = other._hasKey;
}

void crypto::Handle::MD5_hash(bloc::TabChar& hash, const char * data, unsigned len)
{
  MD5_CTX ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, reinterpret_cast<const uint8_t*>(data), len);
  MD5Final(&ctx);
  hash.assign(ctx.digest, ctx.digest + 16); // digest has 16 bytes size
}

void crypto::Handle::generateIV(bloc::TabChar& iv, double random)
{
  double seed = random * reinterpret_cast<uintptr_t>(&iv);
  MD5_hash(iv, reinterpret_cast<char*>(&seed), sizeof(double));
}

bool crypto::Handle::keyFromString(bloc::TabChar& buf, const char * data, unsigned len)
{
  unsigned key_size = _ctx.params->key_size;
  bloc::TabChar k1;
  MD5_hash(k1, data, len);
  if (key_size == 16)
  {
    buf.assign(k1.begin(), k1.end());
  }
  else
  {
    bloc::TabChar k2 = k1;
    for (bloc::TabChar::iterator it = k2.begin(); it != k2.end(); ++it)
      *it ^= 0x55;
    MD5_hash(k1, k1.data(), k1.size());
    MD5_hash(k2, k2.data(), k2.size());
    if (key_size == 24)
    {
      buf.assign(k1.begin() + 4, k1.end());
      buf.insert(buf.end(), k2.begin(), k2.end() - 4);
    }
    else
    {
      buf.assign(k1.begin(), k1.end());
      buf.insert(buf.end(), k2.begin(), k2.end());
    }
  }
  return true;
}

bool crypto::Handle::setKey(const bloc::TabChar& key)
{
  if (key.size() != _ctx.params->key_size)
    return false;
  const uint8_t * _key = reinterpret_cast<const uint8_t*>(key.data());
  AES_ctx_set_key(&_ctx, _key);
  _hasKey = true;
  return true;
}

bool crypto::Handle::ECB_encrypt(bloc::TabChar& data)
{
  size_t len = data.size();
  size_t padding = AES_BLOCKLEN - (len % AES_BLOCKLEN);
  len += padding;
  data.resize(len, static_cast<char>(padding & 0xff));
  size_t p = 0;
  uint8_t * buf = reinterpret_cast<uint8_t*>(data.data());
  while (p < len)
  {
    AES_ECB_encrypt(&_ctx, buf + p);
    p += AES_BLOCKLEN;
  }
  return true;
}

bool crypto::Handle::ECB_decrypt(bloc::TabChar& data)
{
  size_t len = data.size();
  size_t padding = AES_BLOCKLEN - (len % AES_BLOCKLEN);
  if (len == 0 || padding < AES_BLOCKLEN)
    return false;
  size_t p = 0;
  uint8_t * buf = reinterpret_cast<uint8_t*>(data.data());
  while (p < len)
  {
    AES_ECB_decrypt(&_ctx, buf + p);
    p += AES_BLOCKLEN;
  }
  padding = data[len - 1];
  if (padding > AES_BLOCKLEN)
    return false;
  data.resize(len - padding);
  return true;
}

bool crypto::Handle::CBC_encrypt(bloc::TabChar& data)
{
  size_t len = data.size();
  size_t padding = AES_BLOCKLEN - (len % AES_BLOCKLEN);
  len += padding;
  data.resize(len, static_cast<char>(padding & 0xff));
  uint8_t * buf = reinterpret_cast<uint8_t*>(data.data());
  AES_CBC_encrypt_buffer(&_ctx, buf, len);
  return true;
}

bool crypto::Handle::CBC_decrypt(bloc::TabChar& data)
{
  size_t len = data.size();
  size_t padding = AES_BLOCKLEN - (len % AES_BLOCKLEN);
  if (len == 0 || padding < AES_BLOCKLEN)
    return false;
  uint8_t * buf = reinterpret_cast<uint8_t*>(data.data());
  AES_CBC_decrypt_buffer(&_ctx, buf, len);
  padding = data[len - 1];
  if (padding > AES_BLOCKLEN)
    return false;
  data.resize(len - padding);
  return true;
}

bool crypto::Handle::CBC_start(bloc::TabChar& iv)
{
  if (iv.size() != AES_BLOCKLEN)
    return false;
  AES_ctx_set_iv(&_ctx, reinterpret_cast<const uint8_t*>(iv.data()));
  _chunk.clear();
  return true;
}

bool crypto::Handle::CBC_encrypt_chunk(bloc::TabChar& data)
{
  _chunk.insert(_chunk.end(), data.begin(), data.end());
  size_t len = (_chunk.size() / AES_BLOCKLEN) * AES_BLOCKLEN;
  if (len > 0)
  {
    uint8_t * buf = reinterpret_cast<uint8_t*>(_chunk.data());
    AES_CBC_encrypt_buffer(&_ctx, buf, len);
    data.swap(_chunk);
    _chunk.assign(data.begin() + len, data.end());
    data.resize(len);
  }
  else
  {
    data.clear();
  }
  return true;
}

bool crypto::Handle::CBC_end_encrypt(bloc::TabChar& data)
{
  _chunk.insert(_chunk.end(), data.begin(), data.end());
  size_t len = _chunk.size();
  size_t padding = AES_BLOCKLEN - (len % AES_BLOCKLEN);
  len += padding;
  _chunk.resize(len, static_cast<char>(padding & 0xff));
  uint8_t * buf = reinterpret_cast<uint8_t*>(_chunk.data());
  AES_CBC_encrypt_buffer(&_ctx, buf, len);
  data.swap(_chunk);
  _chunk.clear();
  return true;
}

bool crypto::Handle::CBC_decrypt_chunk(bloc::TabChar& data)
{
  _chunk.insert(_chunk.end(), data.begin(), data.end());
  size_t len = (_chunk.size() / AES_BLOCKLEN) * AES_BLOCKLEN;
  if (len > 0)
  {
    uint8_t * buf = reinterpret_cast<uint8_t*>(_chunk.data());
    AES_CBC_decrypt_buffer(&_ctx, buf, len);
    data.swap(_chunk);
    _chunk.assign(data.begin() + len, data.end());
    data.resize(len);
  }
  else
  {
    data.clear();
  }
  return true;
}

bool crypto::Handle::CBC_end_decrypt(bloc::TabChar& data)
{
  _chunk.insert(_chunk.end(), data.begin(), data.end());
  size_t len = _chunk.size();
  if (len == 0)
    return false;
  uint8_t * buf = reinterpret_cast<uint8_t*>(_chunk.data());
  AES_CBC_decrypt_buffer(&_ctx, buf, len);
  size_t padding = _chunk[len - 1];
  if (padding > len || padding > AES_BLOCKLEN)
    return false;
  data.swap(_chunk);
  data.resize(len - padding);
  _chunk.clear();
  return true;
}

} /* namespace plugin */
} /* namespace bloc */
