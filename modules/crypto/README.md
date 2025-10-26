# Module CRYPTO for BLOC

The "crypto" module.

It allows data encryption and decryption with the algorithms AES using key size of 128 bits, 192 bits or 256 bits, and CBC mode.
By default the stronger algorithm is selected (AES-256).

The MD5 digest function is also provided to generate IV or Keys as necessary.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "crypto".

```
import crypto;
```

Once loaded, you can print the object interface.
```
help crypto
```

```
CONSTRUCTORS

crypto() is default contructor

crypto(object)
Build a new handle as a copy of the given one.

crypto(string)
Build a new handle for the given algorithm.
A valid value is among AES-256, AES-192, AES-128.


METHODS

md5(string IN) returns bytes
Generate the MD5 hash of the given string.

md5(bytes IN) returns bytes
Generate the MD5 hash of the given bytes array.

setkey(string IN) returns bytes
Set the cipher key for this session. The key value is generated
by the internal hash algorithm.

setkey(bytes IN) returns boolean
Set the cipher key for this session with the given raw value.

newiv() returns bytes
Returns a new unpredictable IV, usable by the next encryption
session in CBC mode.

encrypt(string IN) returns bytes
One step CBC encryption of the given string.
The returned data starts with IV followed by the encrypted data
with PKCS7 padding.

encrypt(bytes IN) returns bytes
One step CBC encryption of the given bytes array.
The returned data starts with IV followed by the encrypted data
with PKCS7 padding.

decrypt(bytes IN) returns bytes
One step CBC decryption of the given cipher data.
Cipher data must be prefixed with the IV and padded using PKCS7
padding.

cbc_start(bytes IN) returns boolean
Initialize CBC encoder with the specified IV.

cbc_encrypt(string IN) returns bytes
Encrypt the chunk of character data, and return the last
completed cipher block.

cbc_encrypt(bytes IN) returns bytes
Encrypt the chunk of data, and return the last completed
cipher block.

cbc_end_encrypt(string IN) returns bytes
Finalize the last cipher block with PKCS7 padding.

cbc_end_encrypt(bytes IN) returns bytes
Finalize the last cipher block with PKCS7 padding.

cbc_decrypt(bytes IN) returns bytes
Decrypt the block of cipher, and return the last completed
chunk of data. Note that last block must be processed with
the method cbc_end_decrypt(x), which handles PKCS7 padding.

cbc_end_decrypt(bytes IN) returns bytes
Decrypt the last block of cipher with PKCS7 padding.
```

Below, I expose some use cases.
```
import crypto;

/**********************************************/
/* One-Step Crypt, then decrypt               */
/**********************************************/

aes = crypto(); /* Using default algorithm AES-256 */

mykey = aes.setkey("Bite the bullet");

cipher = aes.encrypt("Hello World!");

print cipher;
# 00000000:  84 13 68 6b 4b a5 24 9d 38 52 5d f0 32 1e 3c 50  ..hkK.$.8R].2.<P
# 00000010:  d9 d9 82 87 ef 0d ad 55 43 74 1f 26 88 09 0d c8  .......UCt.&....

plain = aes.decrypt(cipher);
# 00000000:  48 65 6c 6c 6f 20 57 6f 72 6c 64 21              Hello.World!

print "Plain text = """ str(plain) """";
# Plain text = "Hello World!"

/**********************************************/
/* Crypt chunk by chunk, then decrypt         */
/**********************************************/

/* Prefix the encrypted data with IV, as is done in One-Step encryption.
 * That allows us to use One-Step decrypt function later...
 */
cipher = aes.newiv(); 

/* Start the CBC session with IV */
aes.cbc_start(cipher);
# TRUE

cipher.concat(aes.cbc_encrypt("She walks in beauty,"));
cipher.concat(aes.cbc_encrypt(" like the night"));
cipher.concat(aes.cbc_encrypt("Of cloudless climes and"));

/* End encrypt for the rest with PKCS7 padding */
cipher.concat(aes.cbc_end_encrypt(" starry skies;"));

print cipher; /* first block is IV */
# 00000000:  16 e3 4e 32 77 43 8f 5d 26 f0 2c 36 66 29 6d 1d  ..N2wC.]&.,6f)m.
# 00000010:  07 e1 92 eb 79 ad a0 2c f6 71 0f c4 30 d5 84 20  ....y..,.q..0...
# 00000020:  80 62 a7 a4 cf a9 a7 eb fc f2 0b 9d 0e 1e 50 01  .b............P.
# 00000030:  02 f5 fd 04 03 17 35 db f1 eb e4 68 02 48 30 7e  ......5....h.H0~
# 00000040:  95 1e e0 82 4f 64 63 5e 19 64 10 2d 3a ad bb e7  ....Odc^.d.-:...
# 00000050:  d4 af 3b d9 b8 13 14 62 8f 8d 17 c2 6a 8c b8 c2  ..;....b....j...

plain = raw();
/* Start the CBC session with stored IV in cipher */
aes.cbc_start(subraw(cipher, 0, 16));

plain.concat(aes.cbc_decrypt(subraw(cipher, 16, 5)));
plain.concat(aes.cbc_decrypt(subraw(cipher, 21, 19)));
plain.concat(aes.cbc_decrypt(subraw(cipher, 40, 10)));

print plain;
# 00000000:  53 68 65 20 77 61 6c 6b 73 20 69 6e 20 62 65 61  She.walks.in.bea
# 00000010:  75 74 79 2c 20 6c 69 6b 65 20 74 68 65 20 6e 69  uty,.like.the.ni

/* End decrypt to handle PKCS7 padding */
plain.concat(aes.cbc_end_decrypt(subraw(cipher, 50, cipher.count()-50)));

print plain;
# 00000000:  53 68 65 20 77 61 6c 6b 73 20 69 6e 20 62 65 61  She.walks.in.bea
# 00000010:  75 74 79 2c 20 6c 69 6b 65 20 74 68 65 20 6e 69  uty,.like.the.ni
# 00000020:  67 68 74 4f 66 20 63 6c 6f 75 64 6c 65 73 73 20  ghtOf.cloudless.
# 00000030:  63 6c 69 6d 65 73 20 61 6e 64 20 73 74 61 72 72  climes.and.starr
# 00000040:  79 20 73 6b 69 65 73 3b                          y.skies;

/* As cipher data is prefixed with IV and padded PKCS7, we can use
 * One-Step decrypt
 */
print str(aes.decrypt(cipher)); 
# She walks in beauty, like the nightOf cloudless climes and starry skies;
```
