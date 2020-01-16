/* ecrypt-sync-ae.h */

/* 
 * Header file for synchronous stream ciphers with authentication
 * mechanism. 
 * 
 * *** Please only edit parts marked with "[edit]". ***
 */

#ifndef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SYNC_AE
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SYNC_AE

#include "ecrypt-portable.h"

/* ------------------------------------------------------------------------- */

/* Cipher parameters */

/* 
 * The name of your cipher.
 */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_NAME "NLS-v2"    /* [edit] */ 
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_PROFILE "S3___"

/*
 * Specify which key, IV, and MAC sizes are supported by your
 * cipher. A user should be able to enumerate the supported sizes by
 * running the following code:
 *
 * for (i = 0; crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_KEYSIZE(i) <= crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXKEYSIZE; ++i)
 *   {
 *     keysize = crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_KEYSIZE(i);
 *
 *     ...
 *   }
 *
 * All sizes are in bits.
 */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXKEYSIZE 128                 /* [edit] */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_KEYSIZE(i) (80 + (i)*48)       /* [edit] */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXIVSIZE 128                  /* [edit] */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_IVSIZE(i) (32 + (i)*32)        /* [edit] */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXMACSIZE 128                 /* [edit] */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MACSIZE(i) (32 + (i)*32)       /* [edit] */

/* ------------------------------------------------------------------------- */

/* Data structures */

/* 
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx is the structure containing the representation of the
 * internal state of your cipher. 
 *
 * If your cipher also supports unauthenticated encryption, and you
 * want to allow the user to call the unauthenticated functions
 * declared in "ecrypt-sync.h" as well, you might want to set the
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_UAE flag and define the crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx context as an
 * extension of the crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_ctx structure defined in "ecrypt-sync.h".
 */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_UAE                    /* [edit] */
#ifndef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_UAE

typedef struct
{
  /* 
   * [edit]
   *
   * In case your implementation does NOT support unauthenticated
   * encryption, put here all state variable needed during the
   * authenticated encryption process. If it does, use the structure
   * below.
   */
} crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx;

#else

#include "ecrypt-sync.h"

typedef struct
{
  /*
   * [edit]
   *
   * In case your implementation does support unauthenticated
   * encryption, put here all additional state variables (if any)
   * needed to generate the MAC.
   */
  u32 macsize;
} crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_macctx;

typedef struct
{
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_ctx ctx;
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_macctx macctx;
} crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx;

#endif

/* ------------------------------------------------------------------------- */

/* Mandatory functions */

/*
 * Key and message independent initialization. This function will be
 * called once when the program starts (e.g., to build expanded S-box
 * tables).
 */
void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_init(void);

/*
 * Key setup. It is the user's responsibility to select the values of
 * keysize, ivsize, and macsize from the set of supported values
 * specified above.
 */
void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_keysetup(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* key, 
  u32 keysize,                /* Key size in bits. */ 
  u32 ivsize,                 /* IV size in bits. */ 
  u32 macsize);               /* MAC size in bits. */ 

/*
 * IV setup. After having called crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_keysetup(), the user is
 * allowed to call crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ivsetup() different times in order to
 * encrypt/decrypt different messages with the same key but different
 * IV's.
 */
void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ivsetup(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* iv);

/*
 * Authenticated encryption/decryption of arbitrary length messages.
 *
 * For efficiency reasons, the API provides two types of authenticated
 * encrypt/decrypt functions. The crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes() function
 * (declared here) encrypts byte strings of arbitrary length, while
 * the crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks() function (defined later) only
 * accepts lengths which are multiples of crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_BLOCKLENGTH.
 * 
 * The user is allowed to make multiple calls to
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks() to incrementally encrypt a long message,
 * but he is NOT allowed to make additional encryption calls once he
 * has called crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes() (unless he starts a new
 * message of course). For example, this sequence of calls is
 * acceptable:
 *
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_keysetup();
 *
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ivsetup();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_finalize();
 *
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ivsetup();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_finalize();
 *
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ivsetup();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_finalize();
 * 
 * The following sequence is not:
 *
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_keysetup();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ivsetup();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks();
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_finalize();
 */

/*
 * By default crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes() and crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_bytes()
 * are defined as macros which redirect the call to a single function
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_bytes(). If you want to provide separate
 * encryption and decryption functions, please undef
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_BYTE_FUNCTION.
 */
#undef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_BYTE_FUNCTION       /* [edit] */
#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_BYTE_FUNCTION

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes(ctx, plaintext, ciphertext, msglen)   \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_bytes(0, ctx, plaintext, ciphertext, msglen)

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_bytes(ctx, ciphertext, plaintext, msglen)   \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_bytes(1, ctx, ciphertext, plaintext, msglen)

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_bytes(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* input, 
  u8* output, 
  u32 msglen);                /* Message length in bytes. */ 

#else

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* plaintext, 
  u8* ciphertext, 
  u32 msglen);                /* Message length in bytes. */ 

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_bytes(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* ciphertext, 
  u8* plaintext, 
  u32 msglen);                /* Message length in bytes. */ 

#endif

/*
 * MAC output. The crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_finalize() function produces a MAC, and
 * is to be called both after the encryption and the decryption of a
 * message. The user is supposed to check whether both MAC's are
 * identical and to delete all decrypted text if they are not.
 */
void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_finalize(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx,
  u8* mac);

/* ------------------------------------------------------------------------- */

/* Optional features */

/* 
 * Please set the crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_AAD flag if your cipher can
 * authenticate associated data (without encrypting it). The function
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_authenticate_x() will be called (possibly multiple times)
 * before any call to crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_x(). The function has both a
 * byte and a block version (defined later), which are intended to be
 * used in the same way as the encryption function above.
 */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_AAD                    /* [edit] */
#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_AAD

/*
 * If crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_INCREMENTAL_AAD flag is NOT set, the user is
 * only allowed to call crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_authenticate_x() once. This
 * restriction is important for schemes which need to know the length
 * of the complete AAD in advance.
 */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_INCREMENTAL_AAD       /* [edit] */

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_authenticate_bytes(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx,
  const u8* aad,
  u32 aadlen);                /* Length of associated data in bytes. */

#endif

/* ------------------------------------------------------------------------- */

/* Optional optimizations */

/* 
 * By default, the functions in this section are implemented using
 * calls to functions declared above. However, you might want to
 * implement them differently for performance reasons.
 */

/*
 * All-in-one authenticated encryption/decryption of (short) packets.
 *
 * The default definitions of these functions can be found in
 * "ecrypt-sync-ae.c". If you want to implement them differently,
 * please undef the crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_USES_DEFAULT_ALL_IN_ONE flag.
 */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_USES_DEFAULT_ALL_IN_ONE        /* [edit] */

/*
 * Undef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_PACKET_FUNCTION if you want to provide
 * separate packet encryption and decryption functions.
 */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_PACKET_FUNCTION     /* [edit] */
#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_PACKET_FUNCTION

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_packet(                                     \
    ctx, iv, aad, aadlen, plaintext, ciphertext, mglen, mac)          \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_packet(0,                                         \
    ctx, iv, aad, aadlen, plaintext, ciphertext, mglen, mac)

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_packet(                                     \
    ctx, iv, aad, aadlen, ciphertext, plaintext, mglen, mac)          \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_packet(1,                                         \
    ctx, iv, aad, aadlen, ciphertext, plaintext, mglen, mac)

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_packet(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* iv,
  const u8* aad,
  u32 aadlen,
  const u8* input, 
  u8* output, 
  u32 msglen,
  u8* mac);

#else

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_packet(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* iv,
  const u8* aad,
  u32 aadlen,
  const u8* plaintext, 
  u8* ciphertext, 
  u32 msglen,
  u8* mac);

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_packet(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* iv,
  const u8* aad,
  u32 aadlen,
  const u8* ciphertext, 
  u8* plaintext, 
  u32 msglen,
  u8* mac);

#endif

/*
 * Authenticated encryption/decryption of blocks.
 * 
 * By default, these functions are defined as macros. If you want to
 * provide a different implementation, please undef the
 * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_USES_DEFAULT_BLOCK_MACROS flag and implement the functions
 * declared below.
 */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_BLOCKLENGTH 4                  /* [edit] */

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_USES_DEFAULT_BLOCK_MACROS

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks(ctx, plaintext, ciphertext, blocks)  \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_bytes(ctx, plaintext, ciphertext,                 \
    (blocks) * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_BLOCKLENGTH)

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_blocks(ctx, ciphertext, plaintext, blocks)  \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_bytes(ctx, ciphertext, plaintext,                 \
    (blocks) * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_BLOCKLENGTH)

#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_AAD

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_authenticate_blocks(ctx, aad, blocks)               \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_authenticate_bytes(ctx, aad,                              \
    (blocks) * crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_BLOCKLENGTH)

#endif

#else

/*
 * Undef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_BLOCK_FUNCTION if you want to provide
 * separate block encryption and decryption functions.
 */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_BLOCK_FUNCTION      /* [edit] */
#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_HAS_SINGLE_BLOCK_FUNCTION

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks(ctx, plaintext, ciphertext, blocks)  \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_blocks(0, ctx, plaintext, ciphertext, blocks)

#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_blocks(ctx, ciphertext, plaintext, blocks)  \
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_blocks(1, ctx, ciphertext, plaintext, blocks)

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_process_blocks(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* input, 
  u8* output, 
  u32 blocks);                /* Message length in blocks. */

#else

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_encrypt_blocks(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* plaintext, 
  u8* ciphertext, 
  u32 blocks);                /* Message length in blocks. */ 

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_decrypt_blocks(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx, 
  const u8* ciphertext, 
  u8* plaintext, 
  u32 blocks);                /* Message length in blocks. */ 

#endif

#ifdef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_SUPPORTS_AAD

void crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_authenticate_blocks(
  crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_AE_ctx* ctx,
  const u8* aad,
  u32 blocks);                /* Message length in blocks. */ 

#endif

#endif

/*
 * If your cipher can be implemented in different ways, you can use
 * the crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_VARIANT parameter to allow the user to choose between
 * them at compile time (e.g., gcc -Dcrypto_stream_nlsv2_e_v2_sync_2_ECRYPT_VARIANT=3 ...). Please
 * only use this possibility if you really think it could make a
 * significant difference and keep the number of variants
 * (crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXVARIANT) as small as possible (definitely not more than
 * 10). Note also that all variants should have exactly the same
 * external interface (i.e., the same crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_BLOCKLENGTH, etc.).
 */
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXVARIANT 2                   /* [edit] */

#ifndef crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_VARIANT
#define crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_VARIANT 2
#endif

#if (crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_VARIANT > crypto_stream_nlsv2_e_v2_sync_2_ECRYPT_MAXVARIANT)
#error this variant does not exist
#endif

/* ------------------------------------------------------------------------- */

#endif
