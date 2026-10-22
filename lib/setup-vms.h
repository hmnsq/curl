#اگر_غیر_تعریف_شدہ سرخی_CURL_تیاری_VMS_H
#تعریف_کرو سرخی_CURL_تیاری_VMS_H
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

#اگر_تعریف_شدہ __DECC
#ہدایت پیغام_2 save
#ہدایت پیغام_2 disable dollarid
#اگر_ختم

/* Hide the stuff we are overriding */
#تعریف_کرو getenv decc_getenv
#اگر_تعریف_شدہ __DECC
#  اگر __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#اگر_ختم
#شامل <stdlib.h>
حرف *decc$getenv(ثابت حرف *__name);
#شامل <pwd.h>

#شامل <string.h>
#شامل <unixlib.h>

#تعریف_منسوخ getenv
#تعریف_منسوخ getpwuid
#تعریف_کرو getenv   vms_getenv
#تعریف_کرو getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#تعریف_کرو sys$تفویض SYS$تفویض_2
#تعریف_کرو sys$dassgn SYS$DASSGN
#تعریف_کرو sys$qiow   SYS$QIOW

#اگر_تعریف_شدہ __DECC
#  اگر __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#اگر_ختم

#اگر __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#ورنہ
#  اگر __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  ورنہ
#    define decc_getpwuid decc$getpwuid
#  endif
#اگر_ختم

ساخت passwd *decc_getpwuid(uid_t uid);

#اگر_تعریف_شدہ __DECC
#  اگر __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
ساکن حرف *vms_translate_راستہ(ثابت حرف *راستہ)
{
  حرف *unix_راستہ;
  حرف *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(راستہ, ":[<^");
  اگر(!test_str) {
    واپس (حرف *)راستہ;
  }

  unix_راستہ = decc$translate_vms(راستہ);

  اگر((صحیح)unix_راستہ <= 0) {
    /* We can not translate it, so return the original string */
    واپس (حرف *)راستہ;
  }
}
#  ورنہ
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_راستہ(__path) __path
#  endif
#اگر_ختم

#اگر_تعریف_شدہ __DECC
#  اگر __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#اگر_ختم

ساکن حرف *vms_getenv(ثابت حرف *envvar)
{
  حرف *نتیجہ;
  حرف *vms_راستہ;

  /* first use the DECC getenv() function */
  نتیجہ = decc$getenv(envvar);
  اگر(!نتیجہ) {
    واپس نتیجہ;
  }

  vms_راستہ = نتیجہ;
  نتیجہ = vms_translate_راستہ(vms_راستہ);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  واپس نتیجہ;
}

ساکن ساخت passwd vms_passwd_cache;

ساکن ساخت passwd *vms_getpwuid(uid_t uid)
{
  ساخت passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#اگر_تعریف_شدہ __DECC
#  اگر __INITIAL_POINTER_SIZE
  __char_ptr32 unix_راستہ;
#  ورنہ
  حرف *unix_راستہ;
#  endif
#ورنہ
  حرف *unix_راستہ;
#اگر_ختم

  my_passwd = decc_getpwuid(uid);
  اگر(!my_passwd) {
    واپس my_passwd;
  }

  unix_راستہ = vms_translate_راستہ(my_passwd->pw_dir);

  اگر((طویل)unix_راستہ <= 0) {
    /* We can not translate it, so return the original string */
    واپس my_passwd;
  }

  /* If no changes needed, return it */
  اگر(unix_راستہ == my_passwd->pw_dir) {
    واپس my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_راستہ;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  واپس &vms_passwd_cache;
}

#اگر_تعریف_شدہ __DECC
#ہدایت پیغام_2 restore
#اگر_ختم

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#تعریف_کرو ASN1_INTEGER_get           ASN1_INTEGER_GET
#تعریف_کرو ASN1_STRING_ڈیٹا           ASN1_STRING_ڈیٹا_2
#تعریف_کرو ASN1_STRING_لمبائی         ASN1_STRING_لمبائی_2
#تعریف_کرو ASN1_STRING_print          ASN1_STRING_PRINT
#تعریف_کرو ASN1_STRING_مقصد_UTF8        ASN1_STRING_مقصد_UTF8_2
#تعریف_کرو ASN1_STRING_type           ASN1_STRING_TYPE
#تعریف_کرو BIO_ctrl                   BIO_CTRL
#تعریف_کرو BIO_free                   BIO_FREE
#تعریف_کرو BIO_new                    BIO_NEW
#تعریف_کرو BIO_s_mem                  BIO_S_MEM
#تعریف_کرو BN_bn2bin                  BN_BN2BIN
#تعریف_کرو BN_عدد_بٹ                BN_عدد_بٹ_2
#تعریف_کرو CRYPTO_cleanup_all_ex_ڈیٹا CRYPTO_CLEANUP_ALL_EX_ڈیٹا
#تعریف_کرو CRYPTO_free                CRYPTO_FREE
#تعریف_کرو CRYPTO_malloc              CRYPTO_MALLOC
#تعریف_کرو CONF_modules_بھرو_file     CONF_MODULES_بھرو_FILE
#اگر_تعریف_شدہ __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_بھرو_file
ساکن صحیح CONF_modules_بھرو_file(ثابت حرف *filename,
                                  ثابت حرف *appname,
                                  غیرعلامتی طویل flags) {
  واپس 1;
}
#  endif
#اگر_ختم
#تعریف_کرو DES_ecb_encrypt             DES_ECB_ENCRYPT
#تعریف_کرو DES_سیٹ_کلید                 DES_سیٹ_کلید_2
#تعریف_کرو DES_سیٹ_odd_parity          DES_سیٹ_ODD_PARITY
#تعریف_کرو ENGINE_ctrl                 ENGINE_CTRL
#تعریف_کرو ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#تعریف_کرو ENGINE_اختتام               ENGINE_اختتام_2
#تعریف_کرو ENGINE_free                 ENGINE_FREE
#تعریف_کرو ENGINE_get_پہلا            ENGINE_GET_پہلا
#تعریف_کرو ENGINE_get_id               ENGINE_GET_ID
#تعریف_کرو ENGINE_get_اگلا             ENGINE_GET_اگلا
#تعریف_کرو ENGINE_init                 ENGINE_INIT
#تعریف_کرو ENGINE_بھرو_builtin_engines ENGINE_بھرو_BUILTIN_ENGINES
#تعریف_کرو ENGINE_بھرو_نجی_کلید     ENGINE_بھرو_نجی_کلید_2
#تعریف_کرو ENGINE_سیٹ_default          ENGINE_سیٹ_DEFAULT
#تعریف_کرو ERR_clear_error             ERR_CLEAR_ERROR
#تعریف_کرو ERR_error_string            ERR_ERROR_STRING
#تعریف_کرو ERR_error_string_n          ERR_ERROR_STRING_N
#تعریف_کرو ERR_free_لڑیاں            ERR_FREE_لڑیاں
#تعریف_کرو ERR_get_error               ERR_GET_ERROR
#تعریف_کرو ERR_peek_error              ERR_PEEK_ERROR
#تعریف_کرو ERR_remove_state            ERR_REMOVE_STATE
#تعریف_کرو EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#تعریف_کرو EVP_PKEY_free               EVP_PKEY_FREE
#تعریف_کرو EVP_cleanup                 EVP_CLEANUP
#تعریف_کرو GENERAL_نام_free          GENERAL_نام_FREE
#تعریف_کرو i2d_X509_PUBKEY             I2D_X509_PUBKEY
#تعریف_کرو MD4_Final                   MD4_FINAL
#تعریف_کرو MD4_Init                    MD4_INIT
#تعریف_کرو MD4_Update                  MD4_UPDATE
#تعریف_کرو MD5_Final                   MD5_FINAL
#تعریف_کرو MD5_Init                    MD5_INIT
#تعریف_کرو MD5_Update                  MD5_UPDATE
#تعریف_کرو OPENSSL_جوڑو_all_algo_noconf OPENSSL_جوڑو_ALL_ALGO_NOCONF
#اگر_غیر_تعریف_شدہ __VAX
#تعریف_کرو OPENSSL_بھرو_builtin_modules OPENSSL_بھرو_BUILTIN_MODULES
#اگر_ختم
#تعریف_کرو PEM_read_X509                   PEM_READ_X509
#تعریف_کرو PEM_write_bio_X509              PEM_WRITE_BIO_X509
#تعریف_کرو PKCS12_free                     PKCS12_FREE
#تعریف_کرو PKCS12_parse                    PKCS12_PARSE
#تعریف_کرو RAND_جوڑو                        RAND_جوڑو_2
#تعریف_کرو RAND_بائٹیں                      RAND_بائٹیں_2
#تعریف_کرو RAND_file_name                  RAND_FILE_NAME
#تعریف_کرو RAND_بھرو_file                  RAND_بھرو_FILE
#تعریف_کرو RAND_حالت                     RAND_حالت_2
#تعریف_کرو SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#تعریف_کرو SSL_CTX_جوڑو_client_CA           SSL_CTX_جوڑو_CLIENT_CA
#تعریف_کرو SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#تعریف_کرو SSL_CTX_check_نجی_کلید       SSL_CTX_CHECK_نجی_کلید
#تعریف_کرو SSL_CTX_ctrl                    SSL_CTX_CTRL
#تعریف_کرو SSL_CTX_free                    SSL_CTX_FREE
#تعریف_کرو SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#تعریف_کرو SSL_CTX_بھرو_verify_locations   SSL_CTX_بھرو_VERIFY_LOCATIONS
#تعریف_کرو SSL_CTX_new                     SSL_CTX_NEW
#تعریف_کرو SSL_CTX_سیٹ_cipher_list         SSL_CTX_سیٹ_CIPHER_LIST
#تعریف_کرو SSL_CTX_سیٹ_def_passwd_cb_ud    SSL_CTX_سیٹ_DEF_PASSWD_CB_UD
#تعریف_کرو SSL_CTX_سیٹ_default_passwd_cb   SSL_CTX_سیٹ_DEFAULT_PASSWD_CB
#تعریف_کرو SSL_CTX_سیٹ_پیغام_callback        SSL_CTX_سیٹ_پیغام_CALLBACK
#تعریف_کرو SSL_CTX_سیٹ_verify              SSL_CTX_سیٹ_VERIFY
#تعریف_کرو SSL_CTX_use_نجی_کلید          SSL_CTX_USE_PRIVATEKEY
#تعریف_کرو SSL_CTX_use_نجی_کلید_file     SSL_CTX_USE_PRIVATEKEY_FILE
#تعریف_کرو SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#تعریف_کرو SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#تعریف_کرو SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#تعریف_کرو SSL_SESSION_free                SSL_SESSION_FREE
#تعریف_کرو SSL_connect                     SSL_CONNECT
#تعریف_کرو SSL_free                        SSL_FREE
#تعریف_کرو SSL_get1_session                SSL_GET1_SESSION
#تعریف_کرو SSL_get_certificate             SSL_GET_CERTIFICATE
#تعریف_کرو SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#تعریف_کرو SSL_get_error                   SSL_GET_ERROR
#تعریف_کرو SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#تعریف_کرو SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#تعریف_کرو SSL_get_privatekey              SSL_GET_PRIVATEKEY
#تعریف_کرو SSL_get_session                 SSL_GET_SESSION
#تعریف_کرو SSL_get_shutdown                SSL_GET_SHUTDOWN
#تعریف_کرو SSL_get_verify_نتیجہ           SSL_GET_VERIFY_نتیجہ
#تعریف_کرو SSL_library_init                SSL_LIBRARY_INIT
#تعریف_کرو SSL_بھرو_error_لڑیاں          SSL_بھرو_ERROR_لڑیاں
#تعریف_کرو SSL_new                         SSL_NEW
#تعریف_کرو SSL_peek                        SSL_PEEK
#تعریف_کرو SSL_pending                     SSL_PENDING
#تعریف_کرو SSL_read                        SSL_READ
#تعریف_کرو SSL_سیٹ_connect_state           SSL_سیٹ_CONNECT_STATE
#تعریف_کرو SSL_سیٹ_fd                      SSL_سیٹ_FD
#تعریف_کرو SSL_سیٹ_session                 SSL_سیٹ_SESSION
#تعریف_کرو SSL_shutdown                    SSL_SHUTDOWN
#تعریف_کرو SSL_نسخہ                     SSL_نسخہ_2
#تعریف_کرو SSL_write                       SSL_WRITE
#تعریف_کرو SSLeay                          SSLEAY
#تعریف_کرو SSLv23_client_طریقہ            SSLV23_CLIENT_طریقہ
#تعریف_کرو SSLv3_client_طریقہ             SSLV3_CLIENT_طریقہ
#تعریف_کرو TLSv1_client_طریقہ             TLSV1_CLIENT_طریقہ
#تعریف_کرو UI_create_طریقہ                UI_CREATE_طریقہ
#تعریف_کرو UI_destroy_طریقہ               UI_DESTROY_طریقہ
#تعریف_کرو UI_get0_user_ڈیٹا               UI_GET0_USER_ڈیٹا
#تعریف_کرو UI_get_داخلہ_flags              UI_GET_داخلہ_FLAGS
#تعریف_کرو UI_get_string_type              UI_GET_STRING_TYPE
#تعریف_کرو UI_create_طریقہ                UI_CREATE_طریقہ
#تعریف_کرو UI_destroy_طریقہ               UI_DESTROY_طریقہ
#تعریف_کرو UI_طریقہ_get_closer            UI_طریقہ_GET_CLOSER
#تعریف_کرو UI_طریقہ_get_opener            UI_طریقہ_GET_OPENER
#تعریف_کرو UI_طریقہ_get_reader            UI_طریقہ_GET_READER
#تعریف_کرو UI_طریقہ_get_writer            UI_طریقہ_GET_WRITER
#تعریف_کرو UI_طریقہ_سیٹ_closer            UI_طریقہ_سیٹ_CLOSER
#تعریف_کرو UI_طریقہ_سیٹ_opener            UI_طریقہ_سیٹ_OPENER
#تعریف_کرو UI_طریقہ_سیٹ_reader            UI_طریقہ_سیٹ_READER
#تعریف_کرو UI_طریقہ_سیٹ_writer            UI_طریقہ_سیٹ_WRITER
#تعریف_کرو UI_OpenSSL                      UI_OPENSSL
#تعریف_کرو UI_سیٹ_نتیجہ                   UI_سیٹ_نتیجہ_2
#تعریف_کرو X509V3_EXT_print                X509V3_EXT_PRINT
#تعریف_کرو X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#تعریف_کرو X509_EXTENSION_get_ڈیٹا         X509_EXTENSION_GET_ڈیٹا
#تعریف_کرو X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#تعریف_کرو X509_LOOKUP_file                X509_LOOKUP_FILE
#تعریف_کرو X509_NAME_اندراج_get_ڈیٹا        X509_NAME_اندراج_GET_ڈیٹا
#تعریف_کرو X509_NAME_get_اندراج             X509_NAME_GET_اندراج
#تعریف_کرو X509_NAME_get_اشاریہ_by_NID      X509_NAME_GET_اشاریہ_BY_NID
#تعریف_کرو X509_NAME_print_ex              X509_NAME_PRINT_EX
#تعریف_کرو X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#تعریف_کرو X509_STORE_جوڑو_lookup           X509_STORE_جوڑو_LOOKUP
#تعریف_کرو X509_STORE_سیٹ_flags            X509_STORE_سیٹ_FLAGS
#تعریف_کرو X509_check_issued               X509_CHECK_ISSUED
#تعریف_کرو X509_free                       X509_FREE
#تعریف_کرو X509_get_ext_d2i                X509_GET_EXT_D2I
#تعریف_کرو X509_get_issuer_name            X509_GET_ISSUER_NAME
#تعریف_کرو X509_get_pubkey                 X509_GET_PUBKEY
#تعریف_کرو X509_get_serial_نمبر           X509_GET_SERIALNUMBER
#تعریف_کرو X509_get_subject_name           X509_GET_SUBJECT_NAME
#تعریف_کرو X509_بھرو_crl_file              X509_بھرو_CRL_FILE
#تعریف_کرو X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#تعریف_کرو d2i_PKCS12_fp                   D2I_PKCS12_FP
#تعریف_کرو i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#تعریف_کرو sk_عدد                          SK_عدد
#تعریف_کرو sk_نکالو                          SK_نکالو
#تعریف_کرو sk_نکالو_free                     SK_نکالو_FREE
#تعریف_کرو sk_قدر                        SK_قدر
#تعریف_کرو SHA256_Final  SHA256_FINAL
#تعریف_کرو SHA256_Init   SHA256_INIT
#تعریف_کرو SHA256_Update SHA256_UPDATE

#تعریف_کرو USE_UPPERCASE_GSSAPI 1
#تعریف_کرو gss_seal             GSS_SEAL
#تعریف_کرو gss_unseal           GSS_UNSEAL

#تعریف_کرو USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_غیر_تعریف_شدہ AI_NUMERICHOST
#تعریف_منسوخ USE_IPV6
#اگر_ختم
#اگر_ختم

/* VAX symbols are always in uppercase */
#اگر_تعریف_شدہ __VAX
#تعریف_کرو inflate       INFLATE
#تعریف_کرو inflate_انتہا    INFLATEEND
#تعریف_کرو inflateInit2_ INFLATEINIT2_
#تعریف_کرو inflateInit_  INFLATEINIT_
#تعریف_کرو zlib_نسخہ   ZLIBVERSION
#اگر_ختم

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#اگر_تعریف_شدہ __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_جوڑو_all_algorithms
#    define Open_SSL_جوڑو_all_algorithms OPENSSL_جوڑو_ALL_ALGORITHMS
     خالی OPENSSL_جوڑو_ALL_ALGORITHMS(خالی);
#  endif
#اگر_ختم

#اگر_ختم /* HEADER_CURL_SETUP_VMS_H */
