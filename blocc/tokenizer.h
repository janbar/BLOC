
#ifndef TOKENIZER_H
#define TOKENIZER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * definitions for token codes
 */
#define TOKEN_SPACE       301
#define TOKEN_INTEGER     302
#define TOKEN_DOUBLE      303
#define TOKEN_FLOAT       304
#define TOKEN_KEYWORD     305
#define TOKEN_LITERALBEG  306
#define TOKEN_LITERALSTR  307
#define TOKEN_LITERALEND  308
#define TOKEN_COMMENTBEG  309
#define TOKEN_COMMENTSTR  310
#define TOKEN_COMMENTEND  311
#define TOKEN_COMMENT     312
#define TOKEN_DIRECTIVE   313
#define TOKEN_HEXANUM     314

#define TOKEN_ISEQUAL     320
#define TOKEN_ISEQMORE    321
#define TOKEN_ISEQLESS    322
#define TOKEN_ISNOTEQ     325

#define TOKEN_ASSIGN      330
#define TOKEN_POPLEFT     331
#define TOKEN_PUSHRIGHT   332
#define TOKEN_INCREMENT   333
#define TOKEN_DECREMENT   334
#define TOKEN_POWER       335
#define TOKEN_AND         336
#define TOKEN_OR          337

/*
 * The handle of token scanner
 */
typedef struct _token_scanner * TOKEN_SCANNER;
typedef int (*TOKEN_CALLBACK)(void * handle, int token, const char * text);
typedef void (*TOKEN_READER)(void * handle, char * buf, int * len, int maxsize);

/**
 * Initialize a new token scanner.
 * @param handle    reference of the caller
 * @param reader    function called to fill the input buffer
 * @return handle
 */
extern TOKEN_SCANNER
tokenizer_init(void * handle, TOKEN_READER reader);

/**
 * Destroy the scanner and free reserved resources
 * @param scanner   handle
 **/
extern void tokenizer_free(TOKEN_SCANNER scanner);

/**
 * Start the scan reading data from reader and pushing discovered tokens to
 * callback.
 * @param scanner
 * @param callback  function called on each discovered token
 *                  to break the scanning it must return nonzero
 */
extern void tokenizer_scan(TOKEN_SCANNER scanner, TOKEN_CALLBACK callback);

/**
 * Process the next token.
 * @param scanner
 * @param token     zero on EOF, less than zero on ERROR, else the token ID
 * @param text      the string of token
 */
extern void tokenizer_lex(TOKEN_SCANNER scanner, int * token, const char ** text);

extern int tokenizer_state(TOKEN_SCANNER scanner);

extern void tokenizer_clear(TOKEN_SCANNER scanner);

extern void tokenizer_enable_space(TOKEN_SCANNER scanner);

extern void tokenizer_disable_space(TOKEN_SCANNER scanner); /* default */

#ifdef __cplusplus
}
#endif

#endif /* TOKENIZER_H */
