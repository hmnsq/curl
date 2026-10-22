#अगर_अघोषित HEADER_CURL_SETUP_VMS_H
#घोषणा HEADER_CURL_SETUP_VMS_H
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

#अगर_घोषित __DECC
#आशय message save
#आशय message disable dollarid
#पूर्ण_अगर

/* Hide the stuff we are overriding */
#घोषणा दो_पर्या decc_getenv
#अगर_घोषित __DECC
#  अगर __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#पूर्ण_अगर
#समावेश <stdlib.h>
अक्षर *decc$दो_पर्या(स्थिर अक्षर *__name);
#समावेश <pwd.h>

#समावेश <string.h>
#समावेश <unixlib.h>

#अघोषित दो_पर्या
#अघोषित getpwuid
#घोषणा दो_पर्या   vms_getenv
#घोषणा getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#घोषणा sys$assign SYS$ASSIGN
#घोषणा sys$dassgn SYS$DASSGN
#घोषणा sys$qiow   SYS$QIOW

#अगर_घोषित __DECC
#  अगर __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#पूर्ण_अगर

#अगर __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#अन्यथा
#  अगर __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  अन्यथा
#    define decc_getpwuid decc$getpwuid
#  endif
#पूर्ण_अगर

काष्ठा passwd *decc_getpwuid(uid_t uid);

#अगर_घोषित __DECC
#  अगर __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
अटल अक्षर *vms_translate_path(स्थिर अक्षर *path)
{
  अक्षर *unix_path;
  अक्षर *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(path, ":[<^");
  अगर(!test_str) {
    वापस (अक्षर *)path;
  }

  unix_path = decc$translate_vms(path);

  अगर((पूर्णांक)unix_path <= 0) {
    /* We can not translate it, so return the original string */
    वापस (अक्षर *)path;
  }
}
#  अन्यथा
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_path(__path) __path
#  endif
#पूर्ण_अगर

#अगर_घोषित __DECC
#  अगर __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#पूर्ण_अगर

अटल अक्षर *vms_getenv(स्थिर अक्षर *envvar)
{
  अक्षर *परिणाम;
  अक्षर *vms_path;

  /* first use the DECC getenv() function */
  परिणाम = decc$दो_पर्या(envvar);
  अगर(!परिणाम) {
    वापस परिणाम;
  }

  vms_path = परिणाम;
  परिणाम = vms_translate_path(vms_path);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  वापस परिणाम;
}

अटल काष्ठा passwd vms_passwd_cache;

अटल काष्ठा passwd *vms_getpwuid(uid_t uid)
{
  काष्ठा passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#अगर_घोषित __DECC
#  अगर __INITIAL_POINTER_SIZE
  __char_ptr32 unix_path;
#  अन्यथा
  अक्षर *unix_path;
#  endif
#अन्यथा
  अक्षर *unix_path;
#पूर्ण_अगर

  my_passwd = decc_getpwuid(uid);
  अगर(!my_passwd) {
    वापस my_passwd;
  }

  unix_path = vms_translate_path(my_passwd->pw_dir);

  अगर((दीर्घ)unix_path <= 0) {
    /* We can not translate it, so return the original string */
    वापस my_passwd;
  }

  /* If no changes needed, return it */
  अगर(unix_path == my_passwd->pw_dir) {
    वापस my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_path;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  वापस &vms_passwd_cache;
}

#अगर_घोषित __DECC
#आशय message restore
#पूर्ण_अगर

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#घोषणा ASN1_INTEGER_get           ASN1_INTEGER_GET
#घोषणा ASN1_STRING_तथ्य           ASN1_STRING_तथ्य_2
#घोषणा ASN1_STRING_लंबाई         ASN1_STRING_लंबाई_2
#घोषणा ASN1_STRING_print          ASN1_STRING_PRINT
#घोषणा ASN1_STRING_to_UTF8        ASN1_STRING_TO_UTF8
#घोषणा ASN1_STRING_type           ASN1_STRING_TYPE
#घोषणा BIO_ctrl                   BIO_CTRL
#घोषणा BIO_free                   BIO_FREE
#घोषणा BIO_new                    BIO_NEW
#घोषणा BIO_s_mem                  BIO_S_MEM
#घोषणा BN_bn2bin                  BN_BN2BIN
#घोषणा BN_अंक_bits                BN_अंक_BITS
#घोषणा CRYPTO_cleanup_all_ex_तथ्य CRYPTO_CLEANUP_ALL_EX_तथ्य
#घोषणा CRYPTO_free                CRYPTO_FREE
#घोषणा CRYPTO_malloc              CRYPTO_MALLOC
#घोषणा CONF_modules_load_file     CONF_MODULES_LOAD_FILE
#अगर_घोषित __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_load_file
अटल पूर्णांक CONF_modules_load_file(स्थिर अक्षर *filename,
                                  स्थिर अक्षर *appname,
                                  अचिन्हित दीर्घ flags) {
  वापस 1;
}
#  endif
#पूर्ण_अगर
#घोषणा DES_ecb_encrypt             DES_ECB_ENCRYPT
#घोषणा DES_set_कुंजी                 DES_SET_कुंजी
#घोषणा DES_set_odd_parity          DES_SET_ODD_PARITY
#घोषणा ENGINE_ctrl                 ENGINE_CTRL
#घोषणा ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#घोषणा ENGINE_finish               ENGINE_FINISH
#घोषणा ENGINE_free                 ENGINE_FREE
#घोषणा ENGINE_get_first            ENGINE_GET_FIRST
#घोषणा ENGINE_get_id               ENGINE_GET_ID
#घोषणा ENGINE_get_अग्र             ENGINE_GET_अग्र
#घोषणा ENGINE_init                 ENGINE_INIT
#घोषणा ENGINE_load_builtin_engines ENGINE_LOAD_BUILTIN_ENGINES
#घोषणा ENGINE_load_private_कुंजी     ENGINE_LOAD_PRIVATE_कुंजी
#घोषणा ENGINE_set_default          ENGINE_SET_DEFAULT
#घोषणा ERR_clear_error             ERR_CLEAR_ERROR
#घोषणा ERR_error_string            ERR_ERROR_STRING
#घोषणा ERR_error_string_n          ERR_ERROR_STRING_N
#घोषणा ERR_free_strings            ERR_FREE_STRINGS
#घोषणा ERR_get_error               ERR_GET_ERROR
#घोषणा ERR_peek_error              ERR_PEEK_ERROR
#घोषणा ERR_remove_state            ERR_REMOVE_STATE
#घोषणा EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#घोषणा EVP_PKEY_free               EVP_PKEY_FREE
#घोषणा EVP_cleanup                 EVP_CLEANUP
#घोषणा GENERAL_NAMES_free          GENERAL_NAMES_FREE
#घोषणा i2d_X509_PUBKEY             I2D_X509_PUBKEY
#घोषणा MD4_Final                   MD4_FINAL
#घोषणा MD4_Init                    MD4_INIT
#घोषणा MD4_Update                  MD4_UPDATE
#घोषणा MD5_Final                   MD5_FINAL
#घोषणा MD5_Init                    MD5_INIT
#घोषणा MD5_Update                  MD5_UPDATE
#घोषणा OPENSSL_add_all_algo_noconf OPENSSL_ADD_ALL_ALGO_NOCONF
#अगर_अघोषित __VAX
#घोषणा OPENSSL_load_builtin_modules OPENSSL_LOAD_BUILTIN_MODULES
#पूर्ण_अगर
#घोषणा PEM_read_X509                   PEM_READ_X509
#घोषणा PEM_write_bio_X509              PEM_WRITE_BIO_X509
#घोषणा PKCS12_free                     PKCS12_FREE
#घोषणा PKCS12_parse                    PKCS12_PARSE
#घोषणा RAND_add                        RAND_ADD
#घोषणा RAND_bytes                      RAND_BYTES
#घोषणा RAND_file_name                  RAND_FILE_NAME
#घोषणा RAND_load_file                  RAND_LOAD_FILE
#घोषणा RAND_status                     RAND_STATUS
#घोषणा SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#घोषणा SSL_CTX_add_client_CA           SSL_CTX_ADD_CLIENT_CA
#घोषणा SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#घोषणा SSL_CTX_check_private_कुंजी       SSL_CTX_CHECK_PRIVATE_कुंजी
#घोषणा SSL_CTX_ctrl                    SSL_CTX_CTRL
#घोषणा SSL_CTX_free                    SSL_CTX_FREE
#घोषणा SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#घोषणा SSL_CTX_load_verify_locations   SSL_CTX_LOAD_VERIFY_LOCATIONS
#घोषणा SSL_CTX_new                     SSL_CTX_NEW
#घोषणा SSL_CTX_set_cipher_list         SSL_CTX_SET_CIPHER_LIST
#घोषणा SSL_CTX_set_def_passwd_cb_ud    SSL_CTX_SET_DEF_PASSWD_CB_UD
#घोषणा SSL_CTX_set_default_passwd_cb   SSL_CTX_SET_DEFAULT_PASSWD_CB
#घोषणा SSL_CTX_set_संदेश_callback        SSL_CTX_SET_संदेश_CALLBACK
#घोषणा SSL_CTX_set_verify              SSL_CTX_SET_VERIFY
#घोषणा SSL_CTX_use_Private_कुंजी          SSL_CTX_USE_PRIVATEKEY
#घोषणा SSL_CTX_use_Private_कुंजी_file     SSL_CTX_USE_PRIVATEKEY_FILE
#घोषणा SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#घोषणा SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#घोषणा SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#घोषणा SSL_SESSION_free                SSL_SESSION_FREE
#घोषणा SSL_connect                     SSL_CONNECT
#घोषणा SSL_free                        SSL_FREE
#घोषणा SSL_get1_session                SSL_GET1_SESSION
#घोषणा SSL_get_certificate             SSL_GET_CERTIFICATE
#घोषणा SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#घोषणा SSL_get_error                   SSL_GET_ERROR
#घोषणा SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#घोषणा SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#घोषणा SSL_get_privatekey              SSL_GET_PRIVATEKEY
#घोषणा SSL_get_session                 SSL_GET_SESSION
#घोषणा SSL_get_shutdown                SSL_GET_SHUTDOWN
#घोषणा SSL_get_verify_परिणाम           SSL_GET_VERIFY_परिणाम
#घोषणा SSL_library_init                SSL_LIBRARY_INIT
#घोषणा SSL_load_error_strings          SSL_LOAD_ERROR_STRINGS
#घोषणा SSL_new                         SSL_NEW
#घोषणा SSL_peek                        SSL_PEEK
#घोषणा SSL_pending                     SSL_PENDING
#घोषणा SSL_read                        SSL_READ
#घोषणा SSL_set_connect_state           SSL_SET_CONNECT_STATE
#घोषणा SSL_set_fd                      SSL_SET_FD
#घोषणा SSL_set_session                 SSL_SET_SESSION
#घोषणा SSL_shutdown                    SSL_SHUTDOWN
#घोषणा SSL_version                     SSL_VERSION
#घोषणा SSL_write                       SSL_WRITE
#घोषणा SSLeay                          SSLEAY
#घोषणा SSLv23_client_method            SSLV23_CLIENT_METHOD
#घोषणा SSLv3_client_method             SSLV3_CLIENT_METHOD
#घोषणा TLSv1_client_method             TLSV1_CLIENT_METHOD
#घोषणा UI_create_method                UI_CREATE_METHOD
#घोषणा UI_destroy_method               UI_DESTROY_METHOD
#घोषणा UI_get0_user_तथ्य               UI_GET0_USER_तथ्य
#घोषणा UI_get_निवेश_flags              UI_GET_निवेश_FLAGS
#घोषणा UI_get_string_type              UI_GET_STRING_TYPE
#घोषणा UI_create_method                UI_CREATE_METHOD
#घोषणा UI_destroy_method               UI_DESTROY_METHOD
#घोषणा UI_method_get_closer            UI_METHOD_GET_CLOSER
#घोषणा UI_method_get_opener            UI_METHOD_GET_OPENER
#घोषणा UI_method_get_reader            UI_METHOD_GET_READER
#घोषणा UI_method_get_writer            UI_METHOD_GET_WRITER
#घोषणा UI_method_set_closer            UI_METHOD_SET_CLOSER
#घोषणा UI_method_set_opener            UI_METHOD_SET_OPENER
#घोषणा UI_method_set_reader            UI_METHOD_SET_READER
#घोषणा UI_method_set_writer            UI_METHOD_SET_WRITER
#घोषणा UI_OpenSSL                      UI_OPENSSL
#घोषणा UI_set_परिणाम                   UI_SET_परिणाम
#घोषणा X509V3_EXT_print                X509V3_EXT_PRINT
#घोषणा X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#घोषणा X509_EXTENSION_get_तथ्य         X509_EXTENSION_GET_तथ्य
#घोषणा X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#घोषणा X509_LOOKUP_file                X509_LOOKUP_FILE
#घोषणा X509_NAME_ENTRY_get_तथ्य        X509_NAME_ENTRY_GET_तथ्य
#घोषणा X509_NAME_get_entry             X509_NAME_GET_ENTRY
#घोषणा X509_NAME_get_सूचकांक_by_NID      X509_NAME_GET_सूचकांक_BY_NID
#घोषणा X509_NAME_print_ex              X509_NAME_PRINT_EX
#घोषणा X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#घोषणा X509_STORE_add_lookup           X509_STORE_ADD_LOOKUP
#घोषणा X509_STORE_set_flags            X509_STORE_SET_FLAGS
#घोषणा X509_check_issued               X509_CHECK_ISSUED
#घोषणा X509_free                       X509_FREE
#घोषणा X509_get_ext_d2i                X509_GET_EXT_D2I
#घोषणा X509_get_issuer_name            X509_GET_ISSUER_NAME
#घोषणा X509_get_pubkey                 X509_GET_PUBKEY
#घोषणा X509_get_serial_संख्या           X509_GET_SERIALNUMBER
#घोषणा X509_get_subject_name           X509_GET_SUBJECT_NAME
#घोषणा X509_load_crl_file              X509_LOAD_CRL_FILE
#घोषणा X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#घोषणा d2i_PKCS12_fp                   D2I_PKCS12_FP
#घोषणा i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#घोषणा sk_अंक                          SK_अंक
#घोषणा sk_निकालो                          SK_निकालो
#घोषणा sk_निकालो_free                     SK_निकालो_FREE
#घोषणा sk_मूल्य                        SK_मूल्य
#घोषणा SHA256_Final  SHA256_FINAL
#घोषणा SHA256_Init   SHA256_INIT
#घोषणा SHA256_Update SHA256_UPDATE

#घोषणा USE_UPPERCASE_GSSAPI 1
#घोषणा gss_seal             GSS_SEAL
#घोषणा gss_unseal           GSS_UNSEAL

#घोषणा USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#अगर_अघोषित AI_NUMERICHOST
#अघोषित USE_IPV6
#पूर्ण_अगर
#पूर्ण_अगर

/* VAX symbols are always in uppercase */
#अगर_घोषित __VAX
#घोषणा inflate       INFLATE
#घोषणा inflateEnd    INFLATEEND
#घोषणा inflateInit2_ INFLATEINIT2_
#घोषणा inflateInit_  INFLATEINIT_
#घोषणा zlibVersion   ZLIBVERSION
#पूर्ण_अगर

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#अगर_घोषित __VAX

#  include <openssl/evp.h>
#  ifndef OpenSSL_add_all_algorithms
#    define OpenSSL_add_all_algorithms OPENSSL_ADD_ALL_ALGORITHMS
     व्योम OPENSSL_ADD_ALL_ALGORITHMS(व्योम);
#  endif
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_SETUP_VMS_H */
