#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <stddef.h>

namespace bloc
{
namespace plugin
{
namespace crypto
{

#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128 bits block only
#define AES_KEYEXPSZ 240 // Key expension size for max key len (256 bits)

typedef enum {
  AES128 = 0,
  AES192 = 1,
  AES256 = 2,
} AES_config;

typedef struct {
  uint8_t key_size;
  uint8_t nb;
  uint8_t nk;
  uint8_t nr;
} AES_params;

typedef struct
{
  const AES_params * params;
  uint8_t round_key[AES_KEYEXPSZ]; // size of max key expansion
  uint8_t iv[AES_BLOCKLEN];
} AES_ctx;

void AES_init_ctx(AES_ctx* ctx, AES_config conf);
void AES_ctx_set_key(AES_ctx* ctx, const uint8_t* key);
void AES_ctx_set_iv(AES_ctx* ctx, const uint8_t* iv);

void AES_ECB_encrypt(const AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(const AES_ctx* ctx, uint8_t* buf);

void AES_CBC_encrypt_buffer(AES_ctx* ctx, uint8_t* buf, size_t length);
void AES_CBC_decrypt_buffer(AES_ctx* ctx, uint8_t* buf, size_t length);

} // namespace crypto
} // namespace plugin
} // namespace bloc

#endif /* AES_H */

