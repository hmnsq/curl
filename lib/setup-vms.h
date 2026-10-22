#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_VMS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਤਿਆਰੀ_VMS_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/

/* JEM, 2012-12-30, VMS now generates config.h, so only define wrappers for */
/*                  getenv(), getpwuid() and provide is_vms_shell()         */
/*                  Also need upper case symbols for system services, and   */
/*                  OpenSSL, and some Kerberos image                        */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#ਹਦਾਇਤ ਸੁਨੇਹਾ_2 save
#ਹਦਾਇਤ ਸੁਨੇਹਾ_2 disable dollarid
#ਜੇ_ਅੰਤ

/* Hide the stuff we are overriding */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getenv decc_getenv
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#  ਜੇ __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
ਅੱਖਰ *decc$getenv(ਅਚਲ ਅੱਖਰ *__name);
#ਅਨ੍ਤਰ੍ਭਾਵ <pwd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unixlib.h>

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ getenv
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ getpwuid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getenv   vms_getenv
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sys$ਸੌਂਪੋ SYS$ਸੌਂਪੋ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sys$dassgn SYS$DASSGN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sys$qiow   SYS$QIOW

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#  ਜੇ __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#ਨਹੀਂਤਾਂ
#  ਜੇ __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  ਨਹੀਂਤਾਂ
#    define decc_getpwuid decc$getpwuid
#  endif
#ਜੇ_ਅੰਤ

ਬਣਤਰ passwd *decc_getpwuid(uid_t uid);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#  ਜੇ __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
ਸਥਿਰ ਅੱਖਰ *vms_translate_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
  ਅੱਖਰ *unix_ਰਸਤਾ;
  ਅੱਖਰ *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(ਰਸਤਾ, ":[<^");
  ਜੇ(!test_str) {
    ਵਾਪਸ (ਅੱਖਰ *)ਰਸਤਾ;
  }

  unix_ਰਸਤਾ = decc$translate_vms(ਰਸਤਾ);

  ਜੇ((ਪੂਰਨ)unix_ਰਸਤਾ <= 0) {
    /* We can not translate it, so return the original string */
    ਵਾਪਸ (ਅੱਖਰ *)ਰਸਤਾ;
  }
}
#  ਨਹੀਂਤਾਂ
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_ਰਸਤਾ(__path) __path
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#  ਜੇ __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ *vms_getenv(ਅਚਲ ਅੱਖਰ *envvar)
{
  ਅੱਖਰ *ਨਤੀਜਾ;
  ਅੱਖਰ *vms_ਰਸਤਾ;

  /* first use the DECC getenv() function */
  ਨਤੀਜਾ = decc$getenv(envvar);
  ਜੇ(!ਨਤੀਜਾ) {
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  vms_ਰਸਤਾ = ਨਤੀਜਾ;
  ਨਤੀਜਾ = vms_translate_ਰਸਤਾ(vms_ਰਸਤਾ);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਬਣਤਰ passwd vms_passwd_cache;

ਸਥਿਰ ਬਣਤਰ passwd *vms_getpwuid(uid_t uid)
{
  ਬਣਤਰ passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#  ਜੇ __INITIAL_POINTER_SIZE
  __char_ptr32 unix_ਰਸਤਾ;
#  ਨਹੀਂਤਾਂ
  ਅੱਖਰ *unix_ਰਸਤਾ;
#  endif
#ਨਹੀਂਤਾਂ
  ਅੱਖਰ *unix_ਰਸਤਾ;
#ਜੇ_ਅੰਤ

  my_passwd = decc_getpwuid(uid);
  ਜੇ(!my_passwd) {
    ਵਾਪਸ my_passwd;
  }

  unix_ਰਸਤਾ = vms_translate_ਰਸਤਾ(my_passwd->pw_dir);

  ਜੇ((ਲੰਮਾ)unix_ਰਸਤਾ <= 0) {
    /* We can not translate it, so return the original string */
    ਵਾਪਸ my_passwd;
  }

  /* If no changes needed, return it */
  ਜੇ(unix_ਰਸਤਾ == my_passwd->pw_dir) {
    ਵਾਪਸ my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_ਰਸਤਾ;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  ਵਾਪਸ &vms_passwd_cache;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __DECC
#ਹਦਾਇਤ ਸੁਨੇਹਾ_2 restore
#ਜੇ_ਅੰਤ

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASN1_INTEGER_get           ASN1_INTEGER_GET
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASN1_STRING_ਅੰਕੜੇ           ASN1_STRING_ਅੰਕੜੇ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASN1_STRING_ਲੰਮਾਈ         ASN1_STRING_ਲੰਮਾਈ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASN1_STRING_print          ASN1_STRING_PRINT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASN1_STRING_ਮੰਜ਼ਿਲ_UTF8        ASN1_STRING_ਮੰਜ਼ਿਲ_UTF8_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASN1_STRING_type           ASN1_STRING_TYPE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIO_ctrl                   BIO_CTRL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIO_free                   BIO_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIO_new                    BIO_NEW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIO_s_mem                  BIO_S_MEM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BN_bn2bin                  BN_BN2BIN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BN_ਅੰਕ_ਬਿੱਟ                BN_ਅੰਕ_ਬਿੱਟ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRYPTO_cleanup_all_ex_ਅੰਕੜੇ CRYPTO_CLEANUP_ALL_EX_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRYPTO_free                CRYPTO_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRYPTO_malloc              CRYPTO_MALLOC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONF_modules_ਲੱਦੋ_file     CONF_MODULES_ਲੱਦੋ_FILE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_ਲੱਦੋ_file
ਸਥਿਰ ਪੂਰਨ CONF_modules_ਲੱਦੋ_file(ਅਚਲ ਅੱਖਰ *filename,
                                  ਅਚਲ ਅੱਖਰ *appname,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags) {
  ਵਾਪਸ 1;
}
#  endif
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DES_ecb_encrypt             DES_ECB_ENCRYPT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DES_ਸੈੱਟ_ਕੁੰਜੀ                 DES_ਸੈੱਟ_ਕੁੰਜੀ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DES_ਸੈੱਟ_odd_parity          DES_ਸੈੱਟ_ODD_PARITY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_ctrl                 ENGINE_CTRL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_ਅੰਤ               ENGINE_ਅੰਤ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_free                 ENGINE_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_get_ਪਹਿਲਾ            ENGINE_GET_ਪਹਿਲਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_get_id               ENGINE_GET_ID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_get_ਅਗਲਾ             ENGINE_GET_ਅਗਲਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_init                 ENGINE_INIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_ਲੱਦੋ_builtin_engines ENGINE_ਲੱਦੋ_BUILTIN_ENGINES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_ਲੱਦੋ_ਨਿੱਜੀ_ਕੁੰਜੀ     ENGINE_ਲੱਦੋ_ਨਿੱਜੀ_ਕੁੰਜੀ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENGINE_ਸੈੱਟ_default          ENGINE_ਸੈੱਟ_DEFAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_clear_error             ERR_CLEAR_ERROR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_error_string            ERR_ERROR_STRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_error_string_n          ERR_ERROR_STRING_N
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_free_ਲੜੀਆਂ            ERR_FREE_ਲੜੀਆਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_get_error               ERR_GET_ERROR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_peek_error              ERR_PEEK_ERROR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_remove_state            ERR_REMOVE_STATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVP_PKEY_free               EVP_PKEY_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVP_cleanup                 EVP_CLEANUP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GENERAL_ਨਾਂ_free          GENERAL_ਨਾਂ_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ i2d_X509_PUBKEY             I2D_X509_PUBKEY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_Final                   MD4_FINAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_Init                    MD4_INIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD4_Update                  MD4_UPDATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_Final                   MD5_FINAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_Init                    MD5_INIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MD5_Update                  MD5_UPDATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENSSL_ਜੋੜੋ_all_algo_noconf OPENSSL_ਜੋੜੋ_ALL_ALGO_NOCONF
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __VAX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENSSL_ਲੱਦੋ_builtin_modules OPENSSL_ਲੱਦੋ_BUILTIN_MODULES
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PEM_read_X509                   PEM_READ_X509
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PEM_write_bio_X509              PEM_WRITE_BIO_X509
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PKCS12_free                     PKCS12_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PKCS12_parse                    PKCS12_PARSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAND_ਜੋੜੋ                        RAND_ਜੋੜੋ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAND_ਬਾਈਟਾਂ                      RAND_ਬਾਈਟਾਂ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAND_file_name                  RAND_FILE_NAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAND_ਲੱਦੋ_file                  RAND_ਲੱਦੋ_FILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAND_ਸਥਿਤੀ                     RAND_ਸਥਿਤੀ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਜੋੜੋ_client_CA           SSL_CTX_ਜੋੜੋ_CLIENT_CA
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_check_ਨਿੱਜੀ_ਕੁੰਜੀ       SSL_CTX_CHECK_ਨਿੱਜੀ_ਕੁੰਜੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ctrl                    SSL_CTX_CTRL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_free                    SSL_CTX_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਲੱਦੋ_verify_locations   SSL_CTX_ਲੱਦੋ_VERIFY_LOCATIONS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_new                     SSL_CTX_NEW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਸੈੱਟ_cipher_list         SSL_CTX_ਸੈੱਟ_CIPHER_LIST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਸੈੱਟ_def_passwd_cb_ud    SSL_CTX_ਸੈੱਟ_DEF_PASSWD_CB_UD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਸੈੱਟ_default_passwd_cb   SSL_CTX_ਸੈੱਟ_DEFAULT_PASSWD_CB
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਸੈੱਟ_ਸੁਨੇਹਾ_callback        SSL_CTX_ਸੈੱਟ_ਸੁਨੇਹਾ_CALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_ਸੈੱਟ_verify              SSL_CTX_ਸੈੱਟ_VERIFY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ          SSL_CTX_USE_PRIVATEKEY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ_file     SSL_CTX_USE_PRIVATEKEY_FILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_SESSION_free                SSL_SESSION_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_connect                     SSL_CONNECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_free                        SSL_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get1_session                SSL_GET1_SESSION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_certificate             SSL_GET_CERTIFICATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_error                   SSL_GET_ERROR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_privatekey              SSL_GET_PRIVATEKEY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_session                 SSL_GET_SESSION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_shutdown                SSL_GET_SHUTDOWN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get_verify_ਨਤੀਜਾ           SSL_GET_VERIFY_ਨਤੀਜਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_library_init                SSL_LIBRARY_INIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_ਲੱਦੋ_error_ਲੜੀਆਂ          SSL_ਲੱਦੋ_ERROR_ਲੜੀਆਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_new                         SSL_NEW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_peek                        SSL_PEEK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_pending                     SSL_PENDING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_read                        SSL_READ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_ਸੈੱਟ_connect_state           SSL_ਸੈੱਟ_CONNECT_STATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_ਸੈੱਟ_fd                      SSL_ਸੈੱਟ_FD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_ਸੈੱਟ_session                 SSL_ਸੈੱਟ_SESSION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_shutdown                    SSL_SHUTDOWN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_ਸੰਸਕਰਣ                     SSL_ਸੰਸਕਰਣ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_write                       SSL_WRITE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLeay                          SSLEAY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLv23_client_ਵਿਧੀ            SSLV23_CLIENT_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLv3_client_ਵਿਧੀ             SSLV3_CLIENT_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TLSv1_client_ਵਿਧੀ             TLSV1_CLIENT_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_create_ਵਿਧੀ                UI_CREATE_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_destroy_ਵਿਧੀ               UI_DESTROY_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_get0_user_ਅੰਕੜੇ               UI_GET0_USER_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_get_ਨਿਵੇਸ਼_flags              UI_GET_ਨਿਵੇਸ਼_FLAGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_get_string_type              UI_GET_STRING_TYPE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_create_ਵਿਧੀ                UI_CREATE_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_destroy_ਵਿਧੀ               UI_DESTROY_ਵਿਧੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_get_closer            UI_ਵਿਧੀ_GET_CLOSER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_get_opener            UI_ਵਿਧੀ_GET_OPENER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_get_reader            UI_ਵਿਧੀ_GET_READER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_get_writer            UI_ਵਿਧੀ_GET_WRITER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_ਸੈੱਟ_closer            UI_ਵਿਧੀ_ਸੈੱਟ_CLOSER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_ਸੈੱਟ_opener            UI_ਵਿਧੀ_ਸੈੱਟ_OPENER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_ਸੈੱਟ_reader            UI_ਵਿਧੀ_ਸੈੱਟ_READER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਵਿਧੀ_ਸੈੱਟ_writer            UI_ਵਿਧੀ_ਸੈੱਟ_WRITER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_OpenSSL                      UI_OPENSSL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UI_ਸੈੱਟ_ਨਤੀਜਾ                   UI_ਸੈੱਟ_ਨਤੀਜਾ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509V3_EXT_print                X509V3_EXT_PRINT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_EXTENSION_get_ਅੰਕੜੇ         X509_EXTENSION_GET_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_LOOKUP_file                X509_LOOKUP_FILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_NAME_ਇੰਦਰਾਜ_get_ਅੰਕੜੇ        X509_NAME_ਇੰਦਰਾਜ_GET_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_NAME_get_ਇੰਦਰਾਜ             X509_NAME_GET_ਇੰਦਰਾਜ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_NAME_get_ਸੂਚਕਾਂਕ_by_NID      X509_NAME_GET_ਸੂਚਕਾਂਕ_BY_NID
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_NAME_print_ex              X509_NAME_PRINT_EX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_STORE_ਜੋੜੋ_lookup           X509_STORE_ਜੋੜੋ_LOOKUP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_STORE_ਸੈੱਟ_flags            X509_STORE_ਸੈੱਟ_FLAGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_check_issued               X509_CHECK_ISSUED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_free                       X509_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_get_ext_d2i                X509_GET_EXT_D2I
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_get_issuer_name            X509_GET_ISSUER_NAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_get_pubkey                 X509_GET_PUBKEY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_get_serial_ਸੰਖਿਆ           X509_GET_SERIALNUMBER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_get_subject_name           X509_GET_SUBJECT_NAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_ਲੱਦੋ_crl_file              X509_ਲੱਦੋ_CRL_FILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ d2i_PKCS12_fp                   D2I_PKCS12_FP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sk_ਅੰਕ                          SK_ਅੰਕ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sk_ਕੱਢੋ                          SK_ਕੱਢੋ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sk_ਕੱਢੋ_free                     SK_ਕੱਢੋ_FREE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sk_ਮੁੱਲ                        SK_ਮੁੱਲ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHA256_Final  SHA256_FINAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHA256_Init   SHA256_INIT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHA256_Update SHA256_UPDATE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_UPPERCASE_GSSAPI 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gss_seal             GSS_SEAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gss_unseal           GSS_UNSEAL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AI_NUMERICHOST
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ USE_IPV6
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/* VAX symbols are always in uppercase */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VAX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflate       INFLATE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflate_ਸਿਰਾ    INFLATEEND
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflateInit2_ INFLATEINIT2_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ inflateInit_  INFLATEINIT_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ zlib_ਸੰਸਕਰਣ   ZLIBVERSION
#ਜੇ_ਅੰਤ

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_ਜੋੜੋ_all_algorithms
#    define Open_SSL_ਜੋੜੋ_all_algorithms OPENSSL_ਜੋੜੋ_ALL_ALGORITHMS
     ਖਾਲੀ OPENSSL_ਜੋੜੋ_ALL_ALGORITHMS(ਖਾਲੀ);
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_SETUP_VMS_H */
