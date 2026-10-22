#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಸಿದ್ಧತೆ_VMS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಸಿದ್ಧತೆ_VMS_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#ಸೂಚನೆ ಸಂದೇಶ_2 save
#ಸೂಚನೆ ಸಂದೇಶ_2 disable dollarid
#ಆದರೆ_ಅಂತ್ಯ

/* Hide the stuff we are overriding */
#ವ್ಯಾಖ್ಯಾನಿಸು getenv decc_getenv
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#  ಆದರೆ __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <stdlib.h>
ಅಕ್ಷರ *decc$getenv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *__name);
#ಅನ್ತರ್ಭಾವ <pwd.h>

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unixlib.h>

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು getenv
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು getpwuid
#ವ್ಯಾಖ್ಯಾನಿಸು getenv   vms_getenv
#ವ್ಯಾಖ್ಯಾನಿಸು getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#ವ್ಯಾಖ್ಯಾನಿಸು sys$ನಿಯೋಜಿಸು SYS$ನಿಯೋಜಿಸು_2
#ವ್ಯಾಖ್ಯಾನಿಸು sys$dassgn SYS$DASSGN
#ವ್ಯಾಖ್ಯಾನಿಸು sys$qiow   SYS$QIOW

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#  ಆದರೆ __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  ಇಲ್ಲದಿದ್ದರೆ
#    define decc_getpwuid decc$getpwuid
#  endif
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ passwd *decc_getpwuid(uid_t uid);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#  ಆದರೆ __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
ಸ್ಥಿರ ಅಕ್ಷರ *vms_translate_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  ಅಕ್ಷರ *unix_ಮಾರ್ಗ;
  ಅಕ್ಷರ *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(ಮಾರ್ಗ, ":[<^");
  ಆದರೆ(!test_str) {
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)ಮಾರ್ಗ;
  }

  unix_ಮಾರ್ಗ = decc$translate_vms(ಮಾರ್ಗ);

  ಆದರೆ((ಪೂರ್ಣಾಂಕ)unix_ಮಾರ್ಗ <= 0) {
    /* We can not translate it, so return the original string */
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)ಮಾರ್ಗ;
  }
}
#  ಇಲ್ಲದಿದ್ದರೆ
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_ಮಾರ್ಗ(__path) __path
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#  ಆದರೆ __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ *vms_getenv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *envvar)
{
  ಅಕ್ಷರ *ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ *vms_ಮಾರ್ಗ;

  /* first use the DECC getenv() function */
  ಫಲಿತಾಂಶ = decc$getenv(envvar);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  vms_ಮಾರ್ಗ = ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ = vms_translate_ಮಾರ್ಗ(vms_ಮಾರ್ಗ);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ರಚನೆ passwd vms_passwd_cache;

ಸ್ಥಿರ ರಚನೆ passwd *vms_getpwuid(uid_t uid)
{
  ರಚನೆ passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#  ಆದರೆ __INITIAL_POINTER_SIZE
  __char_ptr32 unix_ಮಾರ್ಗ;
#  ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ *unix_ಮಾರ್ಗ;
#  endif
#ಇಲ್ಲದಿದ್ದರೆ
  ಅಕ್ಷರ *unix_ಮಾರ್ಗ;
#ಆದರೆ_ಅಂತ್ಯ

  my_passwd = decc_getpwuid(uid);
  ಆದರೆ(!my_passwd) {
    ಹಿಂತಿರುಗಿ my_passwd;
  }

  unix_ಮಾರ್ಗ = vms_translate_ಮಾರ್ಗ(my_passwd->pw_dir);

  ಆದರೆ((ದೀರ್ಘ)unix_ಮಾರ್ಗ <= 0) {
    /* We can not translate it, so return the original string */
    ಹಿಂತಿರುಗಿ my_passwd;
  }

  /* If no changes needed, return it */
  ಆದರೆ(unix_ಮಾರ್ಗ == my_passwd->pw_dir) {
    ಹಿಂತಿರುಗಿ my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_ಮಾರ್ಗ;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  ಹಿಂತಿರುಗಿ &vms_passwd_cache;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __DECC
#ಸೂಚನೆ ಸಂದೇಶ_2 restore
#ಆದರೆ_ಅಂತ್ಯ

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#ವ್ಯಾಖ್ಯಾನಿಸು ASN1_INTEGER_get           ASN1_INTEGER_GET
#ವ್ಯಾಖ್ಯಾನಿಸು ASN1_STRING_ದತ್ತಾಂಶ           ASN1_STRING_ದತ್ತಾಂಶ_2
#ವ್ಯಾಖ್ಯಾನಿಸು ASN1_STRING_ಉದ್ದಳತೆ         ASN1_STRING_ಉದ್ದಳತೆ_2
#ವ್ಯಾಖ್ಯಾನಿಸು ASN1_STRING_print          ASN1_STRING_PRINT
#ವ್ಯಾಖ್ಯಾನಿಸು ASN1_STRING_ಗಮ್ಯ_UTF8        ASN1_STRING_ಗಮ್ಯ_UTF8_2
#ವ್ಯಾಖ್ಯಾನಿಸು ASN1_STRING_type           ASN1_STRING_TYPE
#ವ್ಯಾಖ್ಯಾನಿಸು BIO_ctrl                   BIO_CTRL
#ವ್ಯಾಖ್ಯಾನಿಸು BIO_free                   BIO_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು BIO_new                    BIO_NEW
#ವ್ಯಾಖ್ಯಾನಿಸು BIO_s_mem                  BIO_S_MEM
#ವ್ಯಾಖ್ಯಾನಿಸು BN_bn2bin                  BN_BN2BIN
#ವ್ಯಾಖ್ಯಾನಿಸು BN_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು                BN_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು_2
#ವ್ಯಾಖ್ಯಾನಿಸು CRYPTO_cleanup_all_ex_ದತ್ತಾಂಶ CRYPTO_CLEANUP_ALL_EX_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು CRYPTO_free                CRYPTO_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು CRYPTO_malloc              CRYPTO_MALLOC
#ವ್ಯಾಖ್ಯಾನಿಸು CONF_modules_ತುಂಬು_file     CONF_MODULES_ತುಂಬು_FILE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_ತುಂಬು_file
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ CONF_modules_ತುಂಬು_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *appname,
                                  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags) {
  ಹಿಂತಿರುಗಿ 1;
}
#  endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು DES_ecb_encrypt             DES_ECB_ENCRYPT
#ವ್ಯಾಖ್ಯಾನಿಸು DES_ಗಣ_ಕೀಲಿ                 DES_ಗಣ_ಕೀಲಿ_2
#ವ್ಯಾಖ್ಯಾನಿಸು DES_ಗಣ_odd_parity          DES_ಗಣ_ODD_PARITY
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_ctrl                 ENGINE_CTRL
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_ಮುಕ್ತಾಯ               ENGINE_ಮುಕ್ತಾಯ_2
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_free                 ENGINE_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_get_ಮೊದಲ            ENGINE_GET_ಮೊದಲ
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_get_id               ENGINE_GET_ID
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_get_ಮುಂದಿನ             ENGINE_GET_ಮುಂದಿನ
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_init                 ENGINE_INIT
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_ತುಂಬು_builtin_engines ENGINE_ತುಂಬು_BUILTIN_ENGINES
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_ತುಂಬು_ಖಾಸಗಿ_ಕೀಲಿ     ENGINE_ತುಂಬು_ಖಾಸಗಿ_ಕೀಲಿ_2
#ವ್ಯಾಖ್ಯಾನಿಸು ENGINE_ಗಣ_default          ENGINE_ಗಣ_DEFAULT
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_clear_error             ERR_CLEAR_ERROR
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_error_string            ERR_ERROR_STRING
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_error_string_n          ERR_ERROR_STRING_N
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_free_ಅಕ್ಷರಸರಣಿಗಳು            ERR_FREE_ಅಕ್ಷರಸರಣಿಗಳು
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_get_error               ERR_GET_ERROR
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_peek_error              ERR_PEEK_ERROR
#ವ್ಯಾಖ್ಯಾನಿಸು ERR_remove_state            ERR_REMOVE_STATE
#ವ್ಯಾಖ್ಯಾನಿಸು EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#ವ್ಯಾಖ್ಯಾನಿಸು EVP_PKEY_free               EVP_PKEY_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು EVP_cleanup                 EVP_CLEANUP
#ವ್ಯಾಖ್ಯಾನಿಸು GENERAL_ಹೆಸರುಗಳು_free          GENERAL_ಹೆಸರುಗಳು_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು i2d_X509_PUBKEY             I2D_X509_PUBKEY
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_Final                   MD4_FINAL
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_Init                    MD4_INIT
#ವ್ಯಾಖ್ಯಾನಿಸು MD4_Update                  MD4_UPDATE
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_Final                   MD5_FINAL
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_Init                    MD5_INIT
#ವ್ಯಾಖ್ಯಾನಿಸು MD5_Update                  MD5_UPDATE
#ವ್ಯಾಖ್ಯಾನಿಸು OPENSSL_ಕೂಡಿಸು_all_algo_noconf OPENSSL_ಕೂಡಿಸು_ALL_ALGO_NOCONF
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __VAX
#ವ್ಯಾಖ್ಯಾನಿಸು OPENSSL_ತುಂಬು_builtin_modules OPENSSL_ತುಂಬು_BUILTIN_MODULES
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PEM_read_X509                   PEM_READ_X509
#ವ್ಯಾಖ್ಯಾನಿಸು PEM_write_bio_X509              PEM_WRITE_BIO_X509
#ವ್ಯಾಖ್ಯಾನಿಸು PKCS12_free                     PKCS12_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು PKCS12_parse                    PKCS12_PARSE
#ವ್ಯಾಖ್ಯಾನಿಸು RAND_ಕೂಡಿಸು                        RAND_ಕೂಡಿಸು_2
#ವ್ಯಾಖ್ಯಾನಿಸು RAND_ಬೈಟುಗಳು                      RAND_ಬೈಟುಗಳು_2
#ವ್ಯಾಖ್ಯಾನಿಸು RAND_file_name                  RAND_FILE_NAME
#ವ್ಯಾಖ್ಯಾನಿಸು RAND_ತುಂಬು_file                  RAND_ತುಂಬು_FILE
#ವ್ಯಾಖ್ಯಾನಿಸು RAND_ಸ್ಥಿತಿ                     RAND_ಸ್ಥಿತಿ_2
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ಕೂಡಿಸು_client_CA           SSL_CTX_ಕೂಡಿಸು_CLIENT_CA
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_check_ಖಾಸಗಿ_ಕೀಲಿ       SSL_CTX_CHECK_ಖಾಸಗಿ_ಕೀಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ctrl                    SSL_CTX_CTRL
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_free                    SSL_CTX_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ತುಂಬು_verify_locations   SSL_CTX_ತುಂಬು_VERIFY_LOCATIONS
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_new                     SSL_CTX_NEW
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ಗಣ_cipher_list         SSL_CTX_ಗಣ_CIPHER_LIST
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ಗಣ_def_passwd_cb_ud    SSL_CTX_ಗಣ_DEF_PASSWD_CB_UD
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ಗಣ_default_passwd_cb   SSL_CTX_ಗಣ_DEFAULT_PASSWD_CB
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ಗಣ_ಸಂದೇಶ_callback        SSL_CTX_ಗಣ_ಸಂದೇಶ_CALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_ಗಣ_verify              SSL_CTX_ಗಣ_VERIFY
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ          SSL_CTX_USE_PRIVATEKEY
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ_file     SSL_CTX_USE_PRIVATEKEY_FILE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_SESSION_free                SSL_SESSION_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_connect                     SSL_CONNECT
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_free                        SSL_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get1_session                SSL_GET1_SESSION
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_certificate             SSL_GET_CERTIFICATE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_error                   SSL_GET_ERROR
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_privatekey              SSL_GET_PRIVATEKEY
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_session                 SSL_GET_SESSION
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_shutdown                SSL_GET_SHUTDOWN
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get_verify_ಫಲಿತಾಂಶ           SSL_GET_VERIFY_ಫಲಿತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_library_init                SSL_LIBRARY_INIT
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_ತುಂಬು_error_ಅಕ್ಷರಸರಣಿಗಳು          SSL_ತುಂಬು_ERROR_ಅಕ್ಷರಸರಣಿಗಳು
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_new                         SSL_NEW
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_peek                        SSL_PEEK
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_pending                     SSL_PENDING
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_read                        SSL_READ
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_ಗಣ_connect_state           SSL_ಗಣ_CONNECT_STATE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_ಗಣ_fd                      SSL_ಗಣ_FD
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_ಗಣ_session                 SSL_ಗಣ_SESSION
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_shutdown                    SSL_SHUTDOWN
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_ಆವೃತ್ತಿ                     SSL_ಆವೃತ್ತಿ_2
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_write                       SSL_WRITE
#ವ್ಯಾಖ್ಯಾನಿಸು SSLeay                          SSLEAY
#ವ್ಯಾಖ್ಯಾನಿಸು SSLv23_client_ವಿಧಾನ            SSLV23_CLIENT_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು SSLv3_client_ವಿಧಾನ             SSLV3_CLIENT_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು TLSv1_client_ವಿಧಾನ             TLSV1_CLIENT_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು UI_create_ವಿಧಾನ                UI_CREATE_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು UI_destroy_ವಿಧಾನ               UI_DESTROY_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು UI_get0_user_ದತ್ತಾಂಶ               UI_GET0_USER_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು UI_get_ಒಳಹರಿವು_flags              UI_GET_ಒಳಹರಿವು_FLAGS
#ವ್ಯಾಖ್ಯಾನಿಸು UI_get_string_type              UI_GET_STRING_TYPE
#ವ್ಯಾಖ್ಯಾನಿಸು UI_create_ವಿಧಾನ                UI_CREATE_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು UI_destroy_ವಿಧಾನ               UI_DESTROY_ವಿಧಾನ
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_get_closer            UI_ವಿಧಾನ_GET_CLOSER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_get_opener            UI_ವಿಧಾನ_GET_OPENER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_get_reader            UI_ವಿಧಾನ_GET_READER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_get_writer            UI_ವಿಧಾನ_GET_WRITER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_ಗಣ_closer            UI_ವಿಧಾನ_ಗಣ_CLOSER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_ಗಣ_opener            UI_ವಿಧಾನ_ಗಣ_OPENER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_ಗಣ_reader            UI_ವಿಧಾನ_ಗಣ_READER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ವಿಧಾನ_ಗಣ_writer            UI_ವಿಧಾನ_ಗಣ_WRITER
#ವ್ಯಾಖ್ಯಾನಿಸು UI_OpenSSL                      UI_OPENSSL
#ವ್ಯಾಖ್ಯಾನಿಸು UI_ಗಣ_ಫಲಿತಾಂಶ                   UI_ಗಣ_ಫಲಿತಾಂಶ_2
#ವ್ಯಾಖ್ಯಾನಿಸು X509V3_EXT_print                X509V3_EXT_PRINT
#ವ್ಯಾಖ್ಯಾನಿಸು X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#ವ್ಯಾಖ್ಯಾನಿಸು X509_EXTENSION_get_ದತ್ತಾಂಶ         X509_EXTENSION_GET_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು X509_LOOKUP_file                X509_LOOKUP_FILE
#ವ್ಯಾಖ್ಯಾನಿಸು X509_NAME_ನಮೂದು_get_ದತ್ತಾಂಶ        X509_NAME_ನಮೂದು_GET_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು X509_NAME_get_ನಮೂದು             X509_NAME_GET_ನಮೂದು
#ವ್ಯಾಖ್ಯಾನಿಸು X509_NAME_get_ಸೂಚ್ಯಂಕ_by_NID      X509_NAME_GET_ಸೂಚ್ಯಂಕ_BY_NID
#ವ್ಯಾಖ್ಯಾನಿಸು X509_NAME_print_ex              X509_NAME_PRINT_EX
#ವ್ಯಾಖ್ಯಾನಿಸು X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#ವ್ಯಾಖ್ಯಾನಿಸು X509_STORE_ಕೂಡಿಸು_lookup           X509_STORE_ಕೂಡಿಸು_LOOKUP
#ವ್ಯಾಖ್ಯಾನಿಸು X509_STORE_ಗಣ_flags            X509_STORE_ಗಣ_FLAGS
#ವ್ಯಾಖ್ಯಾನಿಸು X509_check_issued               X509_CHECK_ISSUED
#ವ್ಯಾಖ್ಯಾನಿಸು X509_free                       X509_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು X509_get_ext_d2i                X509_GET_EXT_D2I
#ವ್ಯಾಖ್ಯಾನಿಸು X509_get_issuer_name            X509_GET_ISSUER_NAME
#ವ್ಯಾಖ್ಯಾನಿಸು X509_get_pubkey                 X509_GET_PUBKEY
#ವ್ಯಾಖ್ಯಾನಿಸು X509_get_serial_ಸಂಖ್ಯೆ           X509_GET_SERIALNUMBER
#ವ್ಯಾಖ್ಯಾನಿಸು X509_get_subject_name           X509_GET_SUBJECT_NAME
#ವ್ಯಾಖ್ಯಾನಿಸು X509_ತುಂಬು_crl_file              X509_ತುಂಬು_CRL_FILE
#ವ್ಯಾಖ್ಯಾನಿಸು X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#ವ್ಯಾಖ್ಯಾನಿಸು d2i_PKCS12_fp                   D2I_PKCS12_FP
#ವ್ಯಾಖ್ಯಾನಿಸು i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#ವ್ಯಾಖ್ಯಾನಿಸು sk_ಅಂಕೆ                          SK_ಅಂಕೆ
#ವ್ಯಾಖ್ಯಾನಿಸು sk_ತೆಗೆ                          SK_ತೆಗೆ
#ವ್ಯಾಖ್ಯಾನಿಸು sk_ತೆಗೆ_free                     SK_ತೆಗೆ_FREE
#ವ್ಯಾಖ್ಯಾನಿಸು sk_ಮೌಲ್ಯ                        SK_ಮೌಲ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು SHA256_Final  SHA256_FINAL
#ವ್ಯಾಖ್ಯಾನಿಸು SHA256_Init   SHA256_INIT
#ವ್ಯಾಖ್ಯಾನಿಸು SHA256_Update SHA256_UPDATE

#ವ್ಯಾಖ್ಯಾನಿಸು USE_UPPERCASE_GSSAPI 1
#ವ್ಯಾಖ್ಯಾನಿಸು gss_seal             GSS_SEAL
#ವ್ಯಾಖ್ಯಾನಿಸು gss_unseal           GSS_UNSEAL

#ವ್ಯಾಖ್ಯಾನಿಸು USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AI_NUMERICHOST
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು USE_IPV6
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/* VAX symbols are always in uppercase */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VAX
#ವ್ಯಾಖ್ಯಾನಿಸು inflate       INFLATE
#ವ್ಯಾಖ್ಯಾನಿಸು inflate_ಕೊನೆ    INFLATEEND
#ವ್ಯಾಖ್ಯಾನಿಸು inflateInit2_ INFLATEINIT2_
#ವ್ಯಾಖ್ಯಾನಿಸು inflateInit_  INFLATEINIT_
#ವ್ಯಾಖ್ಯಾನಿಸು zlib_ಆವೃತ್ತಿ   ZLIBVERSION
#ಆದರೆ_ಅಂತ್ಯ

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_ಕೂಡಿಸು_all_algorithms
#    define Open_SSL_ಕೂಡಿಸು_all_algorithms OPENSSL_ಕೂಡಿಸು_ALL_ALGORITHMS
     ಶೂನ್ಯ OPENSSL_ಕೂಡಿಸು_ALL_ALGORITHMS(ಶೂನ್ಯ);
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SETUP_VMS_H */
