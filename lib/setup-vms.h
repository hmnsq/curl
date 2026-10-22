#اگر_تعریف_نشده سرآیند_CURL_راهاندازی_VMS_H
#تعریف_کن سرآیند_CURL_راهاندازی_VMS_H
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

#اگر_تعریف_شده __DECC
#رهنمود پیام_2 save
#رهنمود پیام_2 disable dollarid
#پایان_اگر

/* Hide the stuff we are overriding */
#تعریف_کن getenv decc_getenv
#اگر_تعریف_شده __DECC
#  إذا __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#پایان_اگر
#ضمن <stdlib.h>
حرف *decc$getenv(ثابت حرف *__name);
#ضمن <pwd.h>

#ضمن <string.h>
#ضمن <unixlib.h>

#لغو_تعریف getenv
#لغو_تعریف getpwuid
#تعریف_کن getenv   vms_getenv
#تعریف_کن getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#تعریف_کن sys$تخصیص_بده SYS$تخصیص_بده_2
#تعریف_کن sys$dassgn SYS$DASSGN
#تعریف_کن sys$qiow   SYS$QIOW

#اگر_تعریف_شده __DECC
#  إذا __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#پایان_اگر

#إذا __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#وإلا
#  إذا __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  وإلا
#    define decc_getpwuid decc$getpwuid
#  endif
#پایان_اگر

بنية passwd *decc_getpwuid(uid_t uid);

#اگر_تعریف_شده __DECC
#  إذا __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
ساكن حرف *vms_translate_مسیر(ثابت حرف *مسیر)
{
  حرف *unix_مسیر;
  حرف *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(مسیر, ":[<^");
  إذا(!test_str) {
    أرجع (حرف *)مسیر;
  }

  unix_مسیر = decc$translate_vms(مسیر);

  إذا((صحيح)unix_مسیر <= 0) {
    /* We can not translate it, so return the original string */
    أرجع (حرف *)مسیر;
  }
}
#  وإلا
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_مسیر(__path) __path
#  endif
#پایان_اگر

#اگر_تعریف_شده __DECC
#  إذا __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#پایان_اگر

ساكن حرف *vms_getenv(ثابت حرف *envvar)
{
  حرف *نتیجه;
  حرف *vms_مسیر;

  /* first use the DECC getenv() function */
  نتیجه = decc$getenv(envvar);
  إذا(!نتیجه) {
    أرجع نتیجه;
  }

  vms_مسیر = نتیجه;
  نتیجه = vms_translate_مسیر(vms_مسیر);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  أرجع نتیجه;
}

ساكن بنية passwd vms_passwd_cache;

ساكن بنية passwd *vms_getpwuid(uid_t uid)
{
  بنية passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#اگر_تعریف_شده __DECC
#  إذا __INITIAL_POINTER_SIZE
  __char_ptr32 unix_مسیر;
#  وإلا
  حرف *unix_مسیر;
#  endif
#وإلا
  حرف *unix_مسیر;
#پایان_اگر

  my_passwd = decc_getpwuid(uid);
  إذا(!my_passwd) {
    أرجع my_passwd;
  }

  unix_مسیر = vms_translate_مسیر(my_passwd->pw_dir);

  إذا((دراز)unix_مسیر <= 0) {
    /* We can not translate it, so return the original string */
    أرجع my_passwd;
  }

  /* If no changes needed, return it */
  إذا(unix_مسیر == my_passwd->pw_dir) {
    أرجع my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_مسیر;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  أرجع &vms_passwd_cache;
}

#اگر_تعریف_شده __DECC
#رهنمود پیام_2 restore
#پایان_اگر

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#تعریف_کن ASN1_INTEGER_get           ASN1_INTEGER_GET
#تعریف_کن ASN1_STRING_داده           ASN1_STRING_داده_2
#تعریف_کن ASN1_STRING_درازی         ASN1_STRING_درازی_2
#تعریف_کن ASN1_STRING_print          ASN1_STRING_PRINT
#تعریف_کن ASN1_STRING_مقصد_UTF8        ASN1_STRING_مقصد_UTF8_2
#تعریف_کن ASN1_STRING_type           ASN1_STRING_TYPE
#تعریف_کن BIO_ctrl                   BIO_CTRL
#تعریف_کن BIO_free                   BIO_FREE
#تعریف_کن BIO_new                    BIO_NEW
#تعریف_کن BIO_s_mem                  BIO_S_MEM
#تعریف_کن BN_bn2bin                  BN_BN2BIN
#تعریف_کن BN_رقم_بیتها                BN_رقم_بیتها_2
#تعریف_کن CRYPTO_cleanup_all_ex_داده CRYPTO_CLEANUP_ALL_EX_داده
#تعریف_کن CRYPTO_free                CRYPTO_FREE
#تعریف_کن CRYPTO_malloc              CRYPTO_MALLOC
#تعریف_کن CONF_modules_بارگیری_file     CONF_MODULES_بارگیری_FILE
#اگر_تعریف_شده __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_بارگیری_file
ساكن صحيح CONF_modules_بارگیری_file(ثابت حرف *filename,
                                  ثابت حرف *appname,
                                  بیعلامت دراز flags) {
  أرجع 1;
}
#  endif
#پایان_اگر
#تعریف_کن DES_ecb_encrypt             DES_ECB_ENCRYPT
#تعریف_کن DES_مجموعه_کلید                 DES_مجموعه_کلید_2
#تعریف_کن DES_مجموعه_odd_parity          DES_مجموعه_ODD_PARITY
#تعریف_کن ENGINE_ctrl                 ENGINE_CTRL
#تعریف_کن ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#تعریف_کن ENGINE_پایان               ENGINE_پایان_2
#تعریف_کن ENGINE_free                 ENGINE_FREE
#تعریف_کن ENGINE_get_اولی            ENGINE_GET_اولی
#تعریف_کن ENGINE_get_id               ENGINE_GET_ID
#تعریف_کن ENGINE_get_بعدی             ENGINE_GET_بعدی
#تعریف_کن ENGINE_init                 ENGINE_INIT
#تعریف_کن ENGINE_بارگیری_builtin_engines ENGINE_بارگیری_BUILTIN_ENGINES
#تعریف_کن ENGINE_بارگیری_خصوصی_کلید     ENGINE_بارگیری_خصوصی_کلید_2
#تعریف_کن ENGINE_مجموعه_default          ENGINE_مجموعه_DEFAULT
#تعریف_کن ERR_clear_error             ERR_CLEAR_ERROR
#تعریف_کن ERR_error_string            ERR_ERROR_STRING
#تعریف_کن ERR_error_string_n          ERR_ERROR_STRING_N
#تعریف_کن ERR_free_رشتهها            ERR_FREE_رشتهها
#تعریف_کن ERR_get_error               ERR_GET_ERROR
#تعریف_کن ERR_peek_error              ERR_PEEK_ERROR
#تعریف_کن ERR_remove_state            ERR_REMOVE_STATE
#تعریف_کن EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#تعریف_کن EVP_PKEY_free               EVP_PKEY_FREE
#تعریف_کن EVP_cleanup                 EVP_CLEANUP
#تعریف_کن GENERAL_نامها_free          GENERAL_نامها_FREE
#تعریف_کن i2d_X509_PUBKEY             I2D_X509_PUBKEY
#تعریف_کن MD4_Final                   MD4_FINAL
#تعریف_کن MD4_Init                    MD4_INIT
#تعریف_کن MD4_Update                  MD4_UPDATE
#تعریف_کن MD5_Final                   MD5_FINAL
#تعریف_کن MD5_Init                    MD5_INIT
#تعریف_کن MD5_Update                  MD5_UPDATE
#تعریف_کن OPENSSL_جمع_کن_all_algo_noconf OPENSSL_جمع_کن_ALL_ALGO_NOCONF
#اگر_تعریف_نشده __VAX
#تعریف_کن OPENSSL_بارگیری_builtin_modules OPENSSL_بارگیری_BUILTIN_MODULES
#پایان_اگر
#تعریف_کن PEM_read_X509                   PEM_READ_X509
#تعریف_کن PEM_write_bio_X509              PEM_WRITE_BIO_X509
#تعریف_کن PKCS12_free                     PKCS12_FREE
#تعریف_کن PKCS12_parse                    PKCS12_PARSE
#تعریف_کن RAND_جمع_کن                        RAND_جمع_کن_2
#تعریف_کن RAND_بایتها                      RAND_بایتها_2
#تعریف_کن RAND_file_name                  RAND_FILE_NAME
#تعریف_کن RAND_بارگیری_file                  RAND_بارگیری_FILE
#تعریف_کن RAND_وضعیت                     RAND_وضعیت_2
#تعریف_کن SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#تعریف_کن SSL_CTX_جمع_کن_client_CA           SSL_CTX_جمع_کن_CLIENT_CA
#تعریف_کن SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#تعریف_کن SSL_CTX_check_خصوصی_کلید       SSL_CTX_CHECK_خصوصی_کلید
#تعریف_کن SSL_CTX_ctrl                    SSL_CTX_CTRL
#تعریف_کن SSL_CTX_free                    SSL_CTX_FREE
#تعریف_کن SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#تعریف_کن SSL_CTX_بارگیری_verify_locations   SSL_CTX_بارگیری_VERIFY_LOCATIONS
#تعریف_کن SSL_CTX_new                     SSL_CTX_NEW
#تعریف_کن SSL_CTX_مجموعه_cipher_list         SSL_CTX_مجموعه_CIPHER_LIST
#تعریف_کن SSL_CTX_مجموعه_def_passwd_cb_ud    SSL_CTX_مجموعه_DEF_PASSWD_CB_UD
#تعریف_کن SSL_CTX_مجموعه_default_passwd_cb   SSL_CTX_مجموعه_DEFAULT_PASSWD_CB
#تعریف_کن SSL_CTX_مجموعه_پیام_callback        SSL_CTX_مجموعه_پیام_CALLBACK
#تعریف_کن SSL_CTX_مجموعه_verify              SSL_CTX_مجموعه_VERIFY
#تعریف_کن SSL_CTX_use_خصوصی_کلید          SSL_CTX_USE_PRIVATEKEY
#تعریف_کن SSL_CTX_use_خصوصی_کلید_file     SSL_CTX_USE_PRIVATEKEY_FILE
#تعریف_کن SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#تعریف_کن SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#تعریف_کن SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#تعریف_کن SSL_SESSION_free                SSL_SESSION_FREE
#تعریف_کن SSL_connect                     SSL_CONNECT
#تعریف_کن SSL_free                        SSL_FREE
#تعریف_کن SSL_get1_session                SSL_GET1_SESSION
#تعریف_کن SSL_get_certificate             SSL_GET_CERTIFICATE
#تعریف_کن SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#تعریف_کن SSL_get_error                   SSL_GET_ERROR
#تعریف_کن SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#تعریف_کن SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#تعریف_کن SSL_get_privatekey              SSL_GET_PRIVATEKEY
#تعریف_کن SSL_get_session                 SSL_GET_SESSION
#تعریف_کن SSL_get_shutdown                SSL_GET_SHUTDOWN
#تعریف_کن SSL_get_verify_نتیجه           SSL_GET_VERIFY_نتیجه
#تعریف_کن SSL_library_init                SSL_LIBRARY_INIT
#تعریف_کن SSL_بارگیری_error_رشتهها          SSL_بارگیری_ERROR_رشتهها
#تعریف_کن SSL_new                         SSL_NEW
#تعریف_کن SSL_peek                        SSL_PEEK
#تعریف_کن SSL_pending                     SSL_PENDING
#تعریف_کن SSL_read                        SSL_READ
#تعریف_کن SSL_مجموعه_connect_state           SSL_مجموعه_CONNECT_STATE
#تعریف_کن SSL_مجموعه_fd                      SSL_مجموعه_FD
#تعریف_کن SSL_مجموعه_session                 SSL_مجموعه_SESSION
#تعریف_کن SSL_shutdown                    SSL_SHUTDOWN
#تعریف_کن SSL_نسخه                     SSL_نسخه_2
#تعریف_کن SSL_write                       SSL_WRITE
#تعریف_کن SSLeay                          SSLEAY
#تعریف_کن SSLv23_client_روش            SSLV23_CLIENT_روش
#تعریف_کن SSLv3_client_روش             SSLV3_CLIENT_روش
#تعریف_کن TLSv1_client_روش             TLSV1_CLIENT_روش
#تعریف_کن UI_create_روش                UI_CREATE_روش
#تعریف_کن UI_destroy_روش               UI_DESTROY_روش
#تعریف_کن UI_get0_user_داده               UI_GET0_USER_داده
#تعریف_کن UI_get_درون_داد_flags              UI_GET_درون_داد_FLAGS
#تعریف_کن UI_get_string_type              UI_GET_STRING_TYPE
#تعریف_کن UI_create_روش                UI_CREATE_روش
#تعریف_کن UI_destroy_روش               UI_DESTROY_روش
#تعریف_کن UI_روش_get_closer            UI_روش_GET_CLOSER
#تعریف_کن UI_روش_get_opener            UI_روش_GET_OPENER
#تعریف_کن UI_روش_get_reader            UI_روش_GET_READER
#تعریف_کن UI_روش_get_writer            UI_روش_GET_WRITER
#تعریف_کن UI_روش_مجموعه_closer            UI_روش_مجموعه_CLOSER
#تعریف_کن UI_روش_مجموعه_opener            UI_روش_مجموعه_OPENER
#تعریف_کن UI_روش_مجموعه_reader            UI_روش_مجموعه_READER
#تعریف_کن UI_روش_مجموعه_writer            UI_روش_مجموعه_WRITER
#تعریف_کن UI_OpenSSL                      UI_OPENSSL
#تعریف_کن UI_مجموعه_نتیجه                   UI_مجموعه_نتیجه_2
#تعریف_کن X509V3_EXT_print                X509V3_EXT_PRINT
#تعریف_کن X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#تعریف_کن X509_EXTENSION_get_داده         X509_EXTENSION_GET_داده
#تعریف_کن X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#تعریف_کن X509_LOOKUP_file                X509_LOOKUP_FILE
#تعریف_کن X509_NAME_مدخل_get_داده        X509_NAME_مدخل_GET_داده
#تعریف_کن X509_NAME_get_مدخل             X509_NAME_GET_مدخل
#تعریف_کن X509_NAME_get_شاخص_by_NID      X509_NAME_GET_شاخص_BY_NID
#تعریف_کن X509_NAME_print_ex              X509_NAME_PRINT_EX
#تعریف_کن X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#تعریف_کن X509_STORE_جمع_کن_lookup           X509_STORE_جمع_کن_LOOKUP
#تعریف_کن X509_STORE_مجموعه_flags            X509_STORE_مجموعه_FLAGS
#تعریف_کن X509_check_issued               X509_CHECK_ISSUED
#تعریف_کن X509_free                       X509_FREE
#تعریف_کن X509_get_ext_d2i                X509_GET_EXT_D2I
#تعریف_کن X509_get_issuer_name            X509_GET_ISSUER_NAME
#تعریف_کن X509_get_pubkey                 X509_GET_PUBKEY
#تعریف_کن X509_get_serial_عدد           X509_GET_SERIALNUMBER
#تعریف_کن X509_get_subject_name           X509_GET_SUBJECT_NAME
#تعریف_کن X509_بارگیری_crl_file              X509_بارگیری_CRL_FILE
#تعریف_کن X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#تعریف_کن d2i_PKCS12_fp                   D2I_PKCS12_FP
#تعریف_کن i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#تعریف_کن sk_رقم                          SK_رقم
#تعریف_کن sk_برداشت                          SK_برداشت
#تعریف_کن sk_برداشت_free                     SK_برداشت_FREE
#تعریف_کن sk_مقدار                        SK_مقدار
#تعریف_کن SHA256_Final  SHA256_FINAL
#تعریف_کن SHA256_Init   SHA256_INIT
#تعریف_کن SHA256_Update SHA256_UPDATE

#تعریف_کن USE_UPPERCASE_GSSAPI 1
#تعریف_کن gss_seal             GSS_SEAL
#تعریف_کن gss_unseal           GSS_UNSEAL

#تعریف_کن USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#اگر_تعریف_نشده AI_NUMERICHOST
#لغو_تعریف USE_IPV6
#پایان_اگر
#پایان_اگر

/* VAX symbols are always in uppercase */
#اگر_تعریف_شده __VAX
#تعریف_کن inflate       INFLATE
#تعریف_کن inflate_انتها    INFLATEEND
#تعریف_کن inflateInit2_ INFLATEINIT2_
#تعریف_کن inflateInit_  INFLATEINIT_
#تعریف_کن zlib_نسخه   ZLIBVERSION
#پایان_اگر

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#اگر_تعریف_شده __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_جمع_کن_all_algorithms
#    define Open_SSL_جمع_کن_all_algorithms OPENSSL_جمع_کن_ALL_ALGORITHMS
     فراغ OPENSSL_جمع_کن_ALL_ALGORITHMS(فراغ);
#  endif
#پایان_اگر

#پایان_اگر /* HEADER_CURL_SETUP_VMS_H */
