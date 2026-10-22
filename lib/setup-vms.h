#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_تياري_VMS_H
#وصف_ڪريو سرو_CURL_تياري_VMS_H
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

#جيڪڏهن_وصف_ٿيل __DECC
#هدايت پيغام save
#هدايت پيغام disable dollarid
#جيڪڏهن_پورو

/* Hide the stuff we are overriding */
#وصف_ڪريو getenv decc_getenv
#جيڪڏهن_وصف_ٿيل __DECC
#  إذا __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#جيڪڏهن_پورو
#ضمن <stdlib.h>
حرف *decc$getenv(ثابت حرف *__name);
#ضمن <pwd.h>

#ضمن <string.h>
#ضمن <unixlib.h>

#وصف_رد getenv
#وصف_رد getpwuid
#وصف_ڪريو getenv   vms_getenv
#وصف_ڪريو getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#وصف_ڪريو sys$سونپيو SYS$سونپيو_2
#وصف_ڪريو sys$dassgn SYS$DASSGN
#وصف_ڪريو sys$qiow   SYS$QIOW

#جيڪڏهن_وصف_ٿيل __DECC
#  إذا __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#جيڪڏهن_پورو

#إذا __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#وإلا
#  إذا __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  وإلا
#    define decc_getpwuid decc$getpwuid
#  endif
#جيڪڏهن_پورو

بنية passwd *decc_getpwuid(uid_t uid);

#جيڪڏهن_وصف_ٿيل __DECC
#  إذا __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
ساكن حرف *vms_translate_رستو(ثابت حرف *رستو)
{
  حرف *unix_رستو;
  حرف *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(رستو, ":[<^");
  إذا(!test_str) {
    أرجع (حرف *)رستو;
  }

  unix_رستو = decc$translate_vms(رستو);

  إذا((صحيح)unix_رستو <= 0) {
    /* We can not translate it, so return the original string */
    أرجع (حرف *)رستو;
  }
}
#  وإلا
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_رستو(__path) __path
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __DECC
#  إذا __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#جيڪڏهن_پورو

ساكن حرف *vms_getenv(ثابت حرف *envvar)
{
  حرف *نتيجو;
  حرف *vms_رستو;

  /* first use the DECC getenv() function */
  نتيجو = decc$getenv(envvar);
  إذا(!نتيجو) {
    أرجع نتيجو;
  }

  vms_رستو = نتيجو;
  نتيجو = vms_translate_رستو(vms_رستو);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  أرجع نتيجو;
}

ساكن بنية passwd vms_passwd_cache;

ساكن بنية passwd *vms_getpwuid(uid_t uid)
{
  بنية passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#جيڪڏهن_وصف_ٿيل __DECC
#  إذا __INITIAL_POINTER_SIZE
  __char_ptr32 unix_رستو;
#  وإلا
  حرف *unix_رستو;
#  endif
#وإلا
  حرف *unix_رستو;
#جيڪڏهن_پورو

  my_passwd = decc_getpwuid(uid);
  إذا(!my_passwd) {
    أرجع my_passwd;
  }

  unix_رستو = vms_translate_رستو(my_passwd->pw_dir);

  إذا((ڊگهو)unix_رستو <= 0) {
    /* We can not translate it, so return the original string */
    أرجع my_passwd;
  }

  /* If no changes needed, return it */
  إذا(unix_رستو == my_passwd->pw_dir) {
    أرجع my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_رستو;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  أرجع &vms_passwd_cache;
}

#جيڪڏهن_وصف_ٿيل __DECC
#هدايت پيغام restore
#جيڪڏهن_پورو

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#وصف_ڪريو ASN1_INTEGER_get           ASN1_INTEGER_GET
#وصف_ڪريو ASN1_STRING_معلومات           ASN1_STRING_معلومات_2
#وصف_ڪريو ASN1_STRING_ڊيگهائي         ASN1_STRING_ڊيگهائي_2
#وصف_ڪريو ASN1_STRING_print          ASN1_STRING_PRINT
#وصف_ڪريو ASN1_STRING_منزل_UTF8        ASN1_STRING_منزل_UTF8_2
#وصف_ڪريو ASN1_STRING_type           ASN1_STRING_TYPE
#وصف_ڪريو BIO_ctrl                   BIO_CTRL
#وصف_ڪريو BIO_free                   BIO_FREE
#وصف_ڪريو BIO_new                    BIO_NEW
#وصف_ڪريو BIO_s_mem                  BIO_S_MEM
#وصف_ڪريو BN_bn2bin                  BN_BN2BIN
#وصف_ڪريو BN_انگ_بٽ                BN_انگ_بٽ_2
#وصف_ڪريو CRYPTO_cleanup_all_ex_معلومات CRYPTO_CLEANUP_ALL_EX_معلومات
#وصف_ڪريو CRYPTO_free                CRYPTO_FREE
#وصف_ڪريو CRYPTO_malloc              CRYPTO_MALLOC
#وصف_ڪريو CONF_modules_ڀريو_file     CONF_MODULES_ڀريو_FILE
#جيڪڏهن_وصف_ٿيل __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_ڀريو_file
ساكن صحيح CONF_modules_ڀريو_file(ثابت حرف *filename,
                                  ثابت حرف *appname,
                                  بنانشان ڊگهو flags) {
  أرجع 1;
}
#  endif
#جيڪڏهن_پورو
#وصف_ڪريو DES_ecb_encrypt             DES_ECB_ENCRYPT
#وصف_ڪريو DES_سيٽ_ڪنجي                 DES_سيٽ_ڪنجي_2
#وصف_ڪريو DES_سيٽ_odd_parity          DES_سيٽ_ODD_PARITY
#وصف_ڪريو ENGINE_ctrl                 ENGINE_CTRL
#وصف_ڪريو ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#وصف_ڪريو ENGINE_پڄاڻي               ENGINE_پڄاڻي_2
#وصف_ڪريو ENGINE_free                 ENGINE_FREE
#وصف_ڪريو ENGINE_get_پهريون            ENGINE_GET_پهريون
#وصف_ڪريو ENGINE_get_id               ENGINE_GET_ID
#وصف_ڪريو ENGINE_get_ايندڙ             ENGINE_GET_ايندڙ
#وصف_ڪريو ENGINE_init                 ENGINE_INIT
#وصف_ڪريو ENGINE_ڀريو_builtin_engines ENGINE_ڀريو_BUILTIN_ENGINES
#وصف_ڪريو ENGINE_ڀريو_ذاتي_ڪنجي     ENGINE_ڀريو_ذاتي_ڪنجي_2
#وصف_ڪريو ENGINE_سيٽ_default          ENGINE_سيٽ_DEFAULT
#وصف_ڪريو ERR_clear_error             ERR_CLEAR_ERROR
#وصف_ڪريو ERR_error_string            ERR_ERROR_STRING
#وصف_ڪريو ERR_error_string_n          ERR_ERROR_STRING_N
#وصف_ڪريو ERR_free_لڙيون            ERR_FREE_لڙيون
#وصف_ڪريو ERR_get_error               ERR_GET_ERROR
#وصف_ڪريو ERR_peek_error              ERR_PEEK_ERROR
#وصف_ڪريو ERR_remove_state            ERR_REMOVE_STATE
#وصف_ڪريو EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#وصف_ڪريو EVP_PKEY_free               EVP_PKEY_FREE
#وصف_ڪريو EVP_cleanup                 EVP_CLEANUP
#وصف_ڪريو GENERAL_نالا_free          GENERAL_نالا_FREE
#وصف_ڪريو i2d_X509_PUBKEY             I2D_X509_PUBKEY
#وصف_ڪريو MD4_Final                   MD4_FINAL
#وصف_ڪريو MD4_Init                    MD4_INIT
#وصف_ڪريو MD4_Update                  MD4_UPDATE
#وصف_ڪريو MD5_Final                   MD5_FINAL
#وصف_ڪريو MD5_Init                    MD5_INIT
#وصف_ڪريو MD5_Update                  MD5_UPDATE
#وصف_ڪريو OPENSSL_جوڙيو_all_algo_noconf OPENSSL_جوڙيو_ALL_ALGO_NOCONF
#جيڪڏهن_وصف_نه_ٿيل __VAX
#وصف_ڪريو OPENSSL_ڀريو_builtin_modules OPENSSL_ڀريو_BUILTIN_MODULES
#جيڪڏهن_پورو
#وصف_ڪريو PEM_read_X509                   PEM_READ_X509
#وصف_ڪريو PEM_write_bio_X509              PEM_WRITE_BIO_X509
#وصف_ڪريو PKCS12_free                     PKCS12_FREE
#وصف_ڪريو PKCS12_parse                    PKCS12_PARSE
#وصف_ڪريو RAND_جوڙيو                        RAND_جوڙيو_2
#وصف_ڪريو RAND_بائيٽون                      RAND_بائيٽون_2
#وصف_ڪريو RAND_file_name                  RAND_FILE_NAME
#وصف_ڪريو RAND_ڀريو_file                  RAND_ڀريو_FILE
#وصف_ڪريو RAND_حالت                     RAND_حالت_2
#وصف_ڪريو SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#وصف_ڪريو SSL_CTX_جوڙيو_client_CA           SSL_CTX_جوڙيو_CLIENT_CA
#وصف_ڪريو SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#وصف_ڪريو SSL_CTX_check_ذاتي_ڪنجي       SSL_CTX_CHECK_ذاتي_ڪنجي
#وصف_ڪريو SSL_CTX_ctrl                    SSL_CTX_CTRL
#وصف_ڪريو SSL_CTX_free                    SSL_CTX_FREE
#وصف_ڪريو SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#وصف_ڪريو SSL_CTX_ڀريو_verify_locations   SSL_CTX_ڀريو_VERIFY_LOCATIONS
#وصف_ڪريو SSL_CTX_new                     SSL_CTX_NEW
#وصف_ڪريو SSL_CTX_سيٽ_cipher_list         SSL_CTX_سيٽ_CIPHER_LIST
#وصف_ڪريو SSL_CTX_سيٽ_def_passwd_cb_ud    SSL_CTX_سيٽ_DEF_PASSWD_CB_UD
#وصف_ڪريو SSL_CTX_سيٽ_default_passwd_cb   SSL_CTX_سيٽ_DEFAULT_PASSWD_CB
#وصف_ڪريو SSL_CTX_سيٽ_نياپو_callback        SSL_CTX_سيٽ_نياپو_CALLBACK
#وصف_ڪريو SSL_CTX_سيٽ_verify              SSL_CTX_سيٽ_VERIFY
#وصف_ڪريو SSL_CTX_use_ذاتي_ڪنجي          SSL_CTX_USE_PRIVATEKEY
#وصف_ڪريو SSL_CTX_use_ذاتي_ڪنجي_file     SSL_CTX_USE_PRIVATEKEY_FILE
#وصف_ڪريو SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#وصف_ڪريو SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#وصف_ڪريو SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#وصف_ڪريو SSL_SESSION_free                SSL_SESSION_FREE
#وصف_ڪريو SSL_connect                     SSL_CONNECT
#وصف_ڪريو SSL_free                        SSL_FREE
#وصف_ڪريو SSL_get1_session                SSL_GET1_SESSION
#وصف_ڪريو SSL_get_certificate             SSL_GET_CERTIFICATE
#وصف_ڪريو SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#وصف_ڪريو SSL_get_error                   SSL_GET_ERROR
#وصف_ڪريو SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#وصف_ڪريو SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#وصف_ڪريو SSL_get_privatekey              SSL_GET_PRIVATEKEY
#وصف_ڪريو SSL_get_session                 SSL_GET_SESSION
#وصف_ڪريو SSL_get_shutdown                SSL_GET_SHUTDOWN
#وصف_ڪريو SSL_get_verify_نتيجو           SSL_GET_VERIFY_نتيجو
#وصف_ڪريو SSL_library_init                SSL_LIBRARY_INIT
#وصف_ڪريو SSL_ڀريو_error_لڙيون          SSL_ڀريو_ERROR_لڙيون
#وصف_ڪريو SSL_new                         SSL_NEW
#وصف_ڪريو SSL_peek                        SSL_PEEK
#وصف_ڪريو SSL_pending                     SSL_PENDING
#وصف_ڪريو SSL_read                        SSL_READ
#وصف_ڪريو SSL_سيٽ_connect_state           SSL_سيٽ_CONNECT_STATE
#وصف_ڪريو SSL_سيٽ_fd                      SSL_سيٽ_FD
#وصف_ڪريو SSL_سيٽ_session                 SSL_سيٽ_SESSION
#وصف_ڪريو SSL_shutdown                    SSL_SHUTDOWN
#وصف_ڪريو SSL_نسخو                     SSL_نسخو_2
#وصف_ڪريو SSL_write                       SSL_WRITE
#وصف_ڪريو SSLeay                          SSLEAY
#وصف_ڪريو SSLv23_client_طريقو            SSLV23_CLIENT_طريقو
#وصف_ڪريو SSLv3_client_طريقو             SSLV3_CLIENT_طريقو
#وصف_ڪريو TLSv1_client_طريقو             TLSV1_CLIENT_طريقو
#وصف_ڪريو UI_create_طريقو                UI_CREATE_طريقو
#وصف_ڪريو UI_destroy_طريقو               UI_DESTROY_طريقو
#وصف_ڪريو UI_get0_user_معلومات               UI_GET0_USER_معلومات
#وصف_ڪريو UI_get_داخلا_flags              UI_GET_داخلا_FLAGS
#وصف_ڪريو UI_get_string_type              UI_GET_STRING_TYPE
#وصف_ڪريو UI_create_طريقو                UI_CREATE_طريقو
#وصف_ڪريو UI_destroy_طريقو               UI_DESTROY_طريقو
#وصف_ڪريو UI_طريقو_get_closer            UI_طريقو_GET_CLOSER
#وصف_ڪريو UI_طريقو_get_opener            UI_طريقو_GET_OPENER
#وصف_ڪريو UI_طريقو_get_reader            UI_طريقو_GET_READER
#وصف_ڪريو UI_طريقو_get_writer            UI_طريقو_GET_WRITER
#وصف_ڪريو UI_طريقو_سيٽ_closer            UI_طريقو_سيٽ_CLOSER
#وصف_ڪريو UI_طريقو_سيٽ_opener            UI_طريقو_سيٽ_OPENER
#وصف_ڪريو UI_طريقو_سيٽ_reader            UI_طريقو_سيٽ_READER
#وصف_ڪريو UI_طريقو_سيٽ_writer            UI_طريقو_سيٽ_WRITER
#وصف_ڪريو UI_OpenSSL                      UI_OPENSSL
#وصف_ڪريو UI_سيٽ_نتيجو                   UI_سيٽ_نتيجو_2
#وصف_ڪريو X509V3_EXT_print                X509V3_EXT_PRINT
#وصف_ڪريو X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#وصف_ڪريو X509_EXTENSION_get_معلومات         X509_EXTENSION_GET_معلومات
#وصف_ڪريو X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#وصف_ڪريو X509_LOOKUP_file                X509_LOOKUP_FILE
#وصف_ڪريو X509_NAME_داخلا_get_معلومات        X509_NAME_داخلا_GET_معلومات
#وصف_ڪريو X509_NAME_get_داخلا             X509_NAME_GET_داخلا
#وصف_ڪريو X509_NAME_get_اشاريو_by_NID      X509_NAME_GET_اشاريو_BY_NID
#وصف_ڪريو X509_NAME_print_ex              X509_NAME_PRINT_EX
#وصف_ڪريو X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#وصف_ڪريو X509_STORE_جوڙيو_lookup           X509_STORE_جوڙيو_LOOKUP
#وصف_ڪريو X509_STORE_سيٽ_flags            X509_STORE_سيٽ_FLAGS
#وصف_ڪريو X509_check_issued               X509_CHECK_ISSUED
#وصف_ڪريو X509_free                       X509_FREE
#وصف_ڪريو X509_get_ext_d2i                X509_GET_EXT_D2I
#وصف_ڪريو X509_get_issuer_name            X509_GET_ISSUER_NAME
#وصف_ڪريو X509_get_pubkey                 X509_GET_PUBKEY
#وصف_ڪريو X509_get_serial_عدد           X509_GET_SERIALNUMBER
#وصف_ڪريو X509_get_subject_name           X509_GET_SUBJECT_NAME
#وصف_ڪريو X509_ڀريو_crl_file              X509_ڀريو_CRL_FILE
#وصف_ڪريو X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#وصف_ڪريو d2i_PKCS12_fp                   D2I_PKCS12_FP
#وصف_ڪريو i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#وصف_ڪريو sk_انگ                          SK_انگ
#وصف_ڪريو sk_ڪڍو                          SK_ڪڍو
#وصف_ڪريو sk_ڪڍو_free                     SK_ڪڍو_FREE
#وصف_ڪريو sk_قدر                        SK_قدر
#وصف_ڪريو SHA256_Final  SHA256_FINAL
#وصف_ڪريو SHA256_Init   SHA256_INIT
#وصف_ڪريو SHA256_Update SHA256_UPDATE

#وصف_ڪريو USE_UPPERCASE_GSSAPI 1
#وصف_ڪريو gss_seal             GSS_SEAL
#وصف_ڪريو gss_unseal           GSS_UNSEAL

#وصف_ڪريو USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_وصف_نه_ٿيل AI_NUMERICHOST
#وصف_رد USE_IPV6
#جيڪڏهن_پورو
#جيڪڏهن_پورو

/* VAX symbols are always in uppercase */
#جيڪڏهن_وصف_ٿيل __VAX
#وصف_ڪريو inflate       INFLATE
#وصف_ڪريو inflate_پڇاڙي    INFLATEEND
#وصف_ڪريو inflateInit2_ INFLATEINIT2_
#وصف_ڪريو inflateInit_  INFLATEINIT_
#وصف_ڪريو zlib_نسخو   ZLIBVERSION
#جيڪڏهن_پورو

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#جيڪڏهن_وصف_ٿيل __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_جوڙيو_all_algorithms
#    define Open_SSL_جوڙيو_all_algorithms OPENSSL_جوڙيو_ALL_ALGORITHMS
     فراغ OPENSSL_جوڙيو_ALL_ALGORITHMS(فراغ);
#  endif
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_SETUP_VMS_H */
