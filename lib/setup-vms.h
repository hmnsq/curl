#که_نه_تعریف_شوی سرلیک_CURL_تیاری_VMS_H
#تعریف_کړه سرلیک_CURL_تیاری_VMS_H
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

#که_تعریف_شوی __DECC
#لارښوونه پیغام_2 save
#لارښوونه پیغام_2 disable dollarid
#د_که_پای

/* Hide the stuff we are overriding */
#تعریف_کړه getenv decc_getenv
#که_تعریف_شوی __DECC
#  إذا __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#د_که_پای
#ضمن <stdlib.h>
حرف *decc$getenv(ثابت حرف *__name);
#ضمن <pwd.h>

#ضمن <string.h>
#ضمن <unixlib.h>

#تعریف_لغوه getenv
#تعریف_لغوه getpwuid
#تعریف_کړه getenv   vms_getenv
#تعریف_کړه getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#تعریف_کړه sys$ټاکل SYS$ټاکل_2
#تعریف_کړه sys$dassgn SYS$DASSGN
#تعریف_کړه sys$qiow   SYS$QIOW

#که_تعریف_شوی __DECC
#  إذا __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#د_که_پای

#إذا __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#وإلا
#  إذا __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  وإلا
#    define decc_getpwuid decc$getpwuid
#  endif
#د_که_پای

بنية passwd *decc_getpwuid(uid_t uid);

#که_تعریف_شوی __DECC
#  إذا __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
ساكن حرف *vms_translate_لار(ثابت حرف *لار)
{
  حرف *unix_لار;
  حرف *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(لار, ":[<^");
  إذا(!test_str) {
    أرجع (حرف *)لار;
  }

  unix_لار = decc$translate_vms(لار);

  إذا((صحيح)unix_لار <= 0) {
    /* We can not translate it, so return the original string */
    أرجع (حرف *)لار;
  }
}
#  وإلا
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_لار(__path) __path
#  endif
#د_که_پای

#که_تعریف_شوی __DECC
#  إذا __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#د_که_پای

ساكن حرف *vms_getenv(ثابت حرف *envvar)
{
  حرف *پایله;
  حرف *vms_لار;

  /* first use the DECC getenv() function */
  پایله = decc$getenv(envvar);
  إذا(!پایله) {
    أرجع پایله;
  }

  vms_لار = پایله;
  پایله = vms_translate_لار(vms_لار);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  أرجع پایله;
}

ساكن بنية passwd vms_passwd_cache;

ساكن بنية passwd *vms_getpwuid(uid_t uid)
{
  بنية passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#که_تعریف_شوی __DECC
#  إذا __INITIAL_POINTER_SIZE
  __char_ptr32 unix_لار;
#  وإلا
  حرف *unix_لار;
#  endif
#وإلا
  حرف *unix_لار;
#د_که_پای

  my_passwd = decc_getpwuid(uid);
  إذا(!my_passwd) {
    أرجع my_passwd;
  }

  unix_لار = vms_translate_لار(my_passwd->pw_dir);

  إذا((اوږد)unix_لار <= 0) {
    /* We can not translate it, so return the original string */
    أرجع my_passwd;
  }

  /* If no changes needed, return it */
  إذا(unix_لار == my_passwd->pw_dir) {
    أرجع my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_لار;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  أرجع &vms_passwd_cache;
}

#که_تعریف_شوی __DECC
#لارښوونه پیغام_2 restore
#د_که_پای

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#تعریف_کړه ASN1_INTEGER_get           ASN1_INTEGER_GET
#تعریف_کړه ASN1_STRING_معلومات           ASN1_STRING_معلومات_2
#تعریف_کړه ASN1_STRING_اوږدتیا         ASN1_STRING_اوږدتیا_2
#تعریف_کړه ASN1_STRING_print          ASN1_STRING_PRINT
#تعریف_کړه ASN1_STRING_منزل_UTF8        ASN1_STRING_منزل_UTF8_2
#تعریف_کړه ASN1_STRING_type           ASN1_STRING_TYPE
#تعریف_کړه BIO_ctrl                   BIO_CTRL
#تعریف_کړه BIO_free                   BIO_FREE
#تعریف_کړه BIO_new                    BIO_NEW
#تعریف_کړه BIO_s_mem                  BIO_S_MEM
#تعریف_کړه BN_bn2bin                  BN_BN2BIN
#تعریف_کړه BN_عدد_بټونه                BN_عدد_بټونه_2
#تعریف_کړه CRYPTO_cleanup_all_ex_معلومات CRYPTO_CLEANUP_ALL_EX_معلومات
#تعریف_کړه CRYPTO_free                CRYPTO_FREE
#تعریف_کړه CRYPTO_malloc              CRYPTO_MALLOC
#تعریف_کړه CONF_modules_بارول_file     CONF_MODULES_بارول_FILE
#که_تعریف_شوی __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_بارول_file
ساكن صحيح CONF_modules_بارول_file(ثابت حرف *filename,
                                  ثابت حرف *appname,
                                  بینښه اوږد flags) {
  أرجع 1;
}
#  endif
#د_که_پای
#تعریف_کړه DES_ecb_encrypt             DES_ECB_ENCRYPT
#تعریف_کړه DES_سېټ_کلی                 DES_سېټ_کلی_2
#تعریف_کړه DES_سېټ_odd_parity          DES_سېټ_ODD_PARITY
#تعریف_کړه ENGINE_ctrl                 ENGINE_CTRL
#تعریف_کړه ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#تعریف_کړه ENGINE_پای               ENGINE_پای_2
#تعریف_کړه ENGINE_free                 ENGINE_FREE
#تعریف_کړه ENGINE_get_لومړی            ENGINE_GET_لومړی
#تعریف_کړه ENGINE_get_id               ENGINE_GET_ID
#تعریف_کړه ENGINE_get_بل             ENGINE_GET_بل
#تعریف_کړه ENGINE_init                 ENGINE_INIT
#تعریف_کړه ENGINE_بارول_builtin_engines ENGINE_بارول_BUILTIN_ENGINES
#تعریف_کړه ENGINE_بارول_شخصي_کلی     ENGINE_بارول_شخصي_کلی_2
#تعریف_کړه ENGINE_سېټ_default          ENGINE_سېټ_DEFAULT
#تعریف_کړه ERR_clear_error             ERR_CLEAR_ERROR
#تعریف_کړه ERR_error_string            ERR_ERROR_STRING
#تعریف_کړه ERR_error_string_n          ERR_ERROR_STRING_N
#تعریف_کړه ERR_free_مزي_ګانې            ERR_FREE_مزي_ګانې
#تعریف_کړه ERR_get_error               ERR_GET_ERROR
#تعریف_کړه ERR_peek_error              ERR_PEEK_ERROR
#تعریف_کړه ERR_remove_state            ERR_REMOVE_STATE
#تعریف_کړه EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#تعریف_کړه EVP_PKEY_free               EVP_PKEY_FREE
#تعریف_کړه EVP_cleanup                 EVP_CLEANUP
#تعریف_کړه GENERAL_نومونه_free          GENERAL_نومونه_FREE
#تعریف_کړه i2d_X509_PUBKEY             I2D_X509_PUBKEY
#تعریف_کړه MD4_Final                   MD4_FINAL
#تعریف_کړه MD4_Init                    MD4_INIT
#تعریف_کړه MD4_Update                  MD4_UPDATE
#تعریف_کړه MD5_Final                   MD5_FINAL
#تعریف_کړه MD5_Init                    MD5_INIT
#تعریف_کړه MD5_Update                  MD5_UPDATE
#تعریف_کړه OPENSSL_جمع_کړه_all_algo_noconf OPENSSL_جمع_کړه_ALL_ALGO_NOCONF
#که_نه_تعریف_شوی __VAX
#تعریف_کړه OPENSSL_بارول_builtin_modules OPENSSL_بارول_BUILTIN_MODULES
#د_که_پای
#تعریف_کړه PEM_read_X509                   PEM_READ_X509
#تعریف_کړه PEM_write_bio_X509              PEM_WRITE_BIO_X509
#تعریف_کړه PKCS12_free                     PKCS12_FREE
#تعریف_کړه PKCS12_parse                    PKCS12_PARSE
#تعریف_کړه RAND_جمع_کړه                        RAND_جمع_کړه_2
#تعریف_کړه RAND_بایټونه                      RAND_بایټونه_2
#تعریف_کړه RAND_file_name                  RAND_FILE_NAME
#تعریف_کړه RAND_بارول_file                  RAND_بارول_FILE
#تعریف_کړه RAND_حالت                     RAND_حالت_2
#تعریف_کړه SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#تعریف_کړه SSL_CTX_جمع_کړه_client_CA           SSL_CTX_جمع_کړه_CLIENT_CA
#تعریف_کړه SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#تعریف_کړه SSL_CTX_check_شخصي_کلی       SSL_CTX_CHECK_شخصي_کلی
#تعریف_کړه SSL_CTX_ctrl                    SSL_CTX_CTRL
#تعریف_کړه SSL_CTX_free                    SSL_CTX_FREE
#تعریف_کړه SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#تعریف_کړه SSL_CTX_بارول_verify_locations   SSL_CTX_بارول_VERIFY_LOCATIONS
#تعریف_کړه SSL_CTX_new                     SSL_CTX_NEW
#تعریف_کړه SSL_CTX_سېټ_cipher_list         SSL_CTX_سېټ_CIPHER_LIST
#تعریف_کړه SSL_CTX_سېټ_def_passwd_cb_ud    SSL_CTX_سېټ_DEF_PASSWD_CB_UD
#تعریف_کړه SSL_CTX_سېټ_default_passwd_cb   SSL_CTX_سېټ_DEFAULT_PASSWD_CB
#تعریف_کړه SSL_CTX_سېټ_پیغام_callback        SSL_CTX_سېټ_پیغام_CALLBACK
#تعریف_کړه SSL_CTX_سېټ_verify              SSL_CTX_سېټ_VERIFY
#تعریف_کړه SSL_CTX_use_شخصي_کلی          SSL_CTX_USE_PRIVATEKEY
#تعریف_کړه SSL_CTX_use_شخصي_کلی_file     SSL_CTX_USE_PRIVATEKEY_FILE
#تعریف_کړه SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#تعریف_کړه SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#تعریف_کړه SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#تعریف_کړه SSL_SESSION_free                SSL_SESSION_FREE
#تعریف_کړه SSL_connect                     SSL_CONNECT
#تعریف_کړه SSL_free                        SSL_FREE
#تعریف_کړه SSL_get1_session                SSL_GET1_SESSION
#تعریف_کړه SSL_get_certificate             SSL_GET_CERTIFICATE
#تعریف_کړه SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#تعریف_کړه SSL_get_error                   SSL_GET_ERROR
#تعریف_کړه SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#تعریف_کړه SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#تعریف_کړه SSL_get_privatekey              SSL_GET_PRIVATEKEY
#تعریف_کړه SSL_get_session                 SSL_GET_SESSION
#تعریف_کړه SSL_get_shutdown                SSL_GET_SHUTDOWN
#تعریف_کړه SSL_get_verify_پایله           SSL_GET_VERIFY_پایله
#تعریف_کړه SSL_library_init                SSL_LIBRARY_INIT
#تعریف_کړه SSL_بارول_error_مزي_ګانې          SSL_بارول_ERROR_مزي_ګانې
#تعریف_کړه SSL_new                         SSL_NEW
#تعریف_کړه SSL_peek                        SSL_PEEK
#تعریف_کړه SSL_pending                     SSL_PENDING
#تعریف_کړه SSL_read                        SSL_READ
#تعریف_کړه SSL_سېټ_connect_state           SSL_سېټ_CONNECT_STATE
#تعریف_کړه SSL_سېټ_fd                      SSL_سېټ_FD
#تعریف_کړه SSL_سېټ_session                 SSL_سېټ_SESSION
#تعریف_کړه SSL_shutdown                    SSL_SHUTDOWN
#تعریف_کړه SSL_نسخه                     SSL_نسخه_2
#تعریف_کړه SSL_write                       SSL_WRITE
#تعریف_کړه SSLeay                          SSLEAY
#تعریف_کړه SSLv23_client_لاره            SSLV23_CLIENT_لاره
#تعریف_کړه SSLv3_client_لاره             SSLV3_CLIENT_لاره
#تعریف_کړه TLSv1_client_لاره             TLSV1_CLIENT_لاره
#تعریف_کړه UI_create_لاره                UI_CREATE_لاره
#تعریف_کړه UI_destroy_لاره               UI_DESTROY_لاره
#تعریف_کړه UI_get0_user_معلومات               UI_GET0_USER_معلومات
#تعریف_کړه UI_get_ننوت_flags              UI_GET_ننوت_FLAGS
#تعریف_کړه UI_get_string_type              UI_GET_STRING_TYPE
#تعریف_کړه UI_create_لاره                UI_CREATE_لاره
#تعریف_کړه UI_destroy_لاره               UI_DESTROY_لاره
#تعریف_کړه UI_لاره_get_closer            UI_لاره_GET_CLOSER
#تعریف_کړه UI_لاره_get_opener            UI_لاره_GET_OPENER
#تعریف_کړه UI_لاره_get_reader            UI_لاره_GET_READER
#تعریف_کړه UI_لاره_get_writer            UI_لاره_GET_WRITER
#تعریف_کړه UI_لاره_سېټ_closer            UI_لاره_سېټ_CLOSER
#تعریف_کړه UI_لاره_سېټ_opener            UI_لاره_سېټ_OPENER
#تعریف_کړه UI_لاره_سېټ_reader            UI_لاره_سېټ_READER
#تعریف_کړه UI_لاره_سېټ_writer            UI_لاره_سېټ_WRITER
#تعریف_کړه UI_OpenSSL                      UI_OPENSSL
#تعریف_کړه UI_سېټ_پایله                   UI_سېټ_پایله_2
#تعریف_کړه X509V3_EXT_print                X509V3_EXT_PRINT
#تعریف_کړه X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#تعریف_کړه X509_EXTENSION_get_معلومات         X509_EXTENSION_GET_معلومات
#تعریف_کړه X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#تعریف_کړه X509_LOOKUP_file                X509_LOOKUP_FILE
#تعریف_کړه X509_NAME_ثبت_get_معلومات        X509_NAME_ثبت_GET_معلومات
#تعریف_کړه X509_NAME_get_ثبت             X509_NAME_GET_ثبت
#تعریف_کړه X509_NAME_get_شاخص_by_NID      X509_NAME_GET_شاخص_BY_NID
#تعریف_کړه X509_NAME_print_ex              X509_NAME_PRINT_EX
#تعریف_کړه X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#تعریف_کړه X509_STORE_جمع_کړه_lookup           X509_STORE_جمع_کړه_LOOKUP
#تعریف_کړه X509_STORE_سېټ_flags            X509_STORE_سېټ_FLAGS
#تعریف_کړه X509_check_issued               X509_CHECK_ISSUED
#تعریف_کړه X509_free                       X509_FREE
#تعریف_کړه X509_get_ext_d2i                X509_GET_EXT_D2I
#تعریف_کړه X509_get_issuer_name            X509_GET_ISSUER_NAME
#تعریف_کړه X509_get_pubkey                 X509_GET_PUBKEY
#تعریف_کړه X509_get_serial_شمېره           X509_GET_SERIALNUMBER
#تعریف_کړه X509_get_subject_name           X509_GET_SUBJECT_NAME
#تعریف_کړه X509_بارول_crl_file              X509_بارول_CRL_FILE
#تعریف_کړه X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#تعریف_کړه d2i_PKCS12_fp                   D2I_PKCS12_FP
#تعریف_کړه i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#تعریف_کړه sk_عدد                          SK_عدد
#تعریف_کړه sk_راوباسه                          SK_راوباسه
#تعریف_کړه sk_راوباسه_free                     SK_راوباسه_FREE
#تعریف_کړه sk_ارزښت                        SK_ارزښت
#تعریف_کړه SHA256_Final  SHA256_FINAL
#تعریف_کړه SHA256_Init   SHA256_INIT
#تعریف_کړه SHA256_Update SHA256_UPDATE

#تعریف_کړه USE_UPPERCASE_GSSAPI 1
#تعریف_کړه gss_seal             GSS_SEAL
#تعریف_کړه gss_unseal           GSS_UNSEAL

#تعریف_کړه USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#که_نه_تعریف_شوی AI_NUMERICHOST
#تعریف_لغوه USE_IPV6
#د_که_پای
#د_که_پای

/* VAX symbols are always in uppercase */
#که_تعریف_شوی __VAX
#تعریف_کړه inflate       INFLATE
#تعریف_کړه inflate_پای_ټکی    INFLATEEND
#تعریف_کړه inflateInit2_ INFLATEINIT2_
#تعریف_کړه inflateInit_  INFLATEINIT_
#تعریف_کړه zlib_نسخه   ZLIBVERSION
#د_که_پای

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#که_تعریف_شوی __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_جمع_کړه_all_algorithms
#    define Open_SSL_جمع_کړه_all_algorithms OPENSSL_جمع_کړه_ALL_ALGORITHMS
     فراغ OPENSSL_جمع_کړه_ALL_ALGORITHMS(فراغ);
#  endif
#د_که_پای

#د_که_پای /* HEADER_CURL_SETUP_VMS_H */
