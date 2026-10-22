#यदि_अपरिभाषितम् शीर्षकम्_CURL_सज्जता_VMS_H
#परिभाषय शीर्षकम्_CURL_सज्जता_VMS_H
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

#यदि_परिभाषितम् __DECC
#निर्देशः सन्देशः_2 save
#निर्देशः सन्देशः_2 disable dollarid
#यदि_अन्तः

/* Hide the stuff we are overriding */
#परिभाषय getenv decc_getenv
#यदि_परिभाषितम् __DECC
#  यदि __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#यदि_अन्तः
#अन्तर्भाव <stdlib.h>
वर्ण *decc$getenv(ध्रुव वर्ण *__name);
#अन्तर्भाव <pwd.h>

#अन्तर्भाव <string.h>
#अन्तर्भाव <unixlib.h>

#अपरिभाषय getenv
#अपरिभाषय getpwuid
#परिभाषय getenv   vms_getenv
#परिभाषय getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#परिभाषय sys$नियोजय SYS$नियोजय_2
#परिभाषय sys$dassgn SYS$DASSGN
#परिभाषय sys$qiow   SYS$QIOW

#यदि_परिभाषितम् __DECC
#  यदि __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#यदि_अन्तः

#यदि __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#अन्यथा
#  यदि __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  अन्यथा
#    define decc_getpwuid decc$getpwuid
#  endif
#यदि_अन्तः

संरचना passwd *decc_getpwuid(uid_t uid);

#यदि_परिभाषितम् __DECC
#  यदि __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
स्थिर वर्ण *vms_translate_मार्गः(ध्रुव वर्ण *मार्गः)
{
  वर्ण *unix_मार्गः;
  वर्ण *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(मार्गः, ":[<^");
  यदि(!test_str) {
    प्रत्यागम (वर्ण *)मार्गः;
  }

  unix_मार्गः = decc$translate_vms(मार्गः);

  यदि((पूर्णाङ्क)unix_मार्गः <= 0) {
    /* We can not translate it, so return the original string */
    प्रत्यागम (वर्ण *)मार्गः;
  }
}
#  अन्यथा
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_मार्गः(__path) __path
#  endif
#यदि_अन्तः

#यदि_परिभाषितम् __DECC
#  यदि __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#यदि_अन्तः

स्थिर वर्ण *vms_getenv(ध्रुव वर्ण *envvar)
{
  वर्ण *परिणाम;
  वर्ण *vms_मार्गः;

  /* first use the DECC getenv() function */
  परिणाम = decc$getenv(envvar);
  यदि(!परिणाम) {
    प्रत्यागम परिणाम;
  }

  vms_मार्गः = परिणाम;
  परिणाम = vms_translate_मार्गः(vms_मार्गः);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  प्रत्यागम परिणाम;
}

स्थिर संरचना passwd vms_passwd_cache;

स्थिर संरचना passwd *vms_getpwuid(uid_t uid)
{
  संरचना passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#यदि_परिभाषितम् __DECC
#  यदि __INITIAL_POINTER_SIZE
  __char_ptr32 unix_मार्गः;
#  अन्यथा
  वर्ण *unix_मार्गः;
#  endif
#अन्यथा
  वर्ण *unix_मार्गः;
#यदि_अन्तः

  my_passwd = decc_getpwuid(uid);
  यदि(!my_passwd) {
    प्रत्यागम my_passwd;
  }

  unix_मार्गः = vms_translate_मार्गः(my_passwd->pw_dir);

  यदि((दीर्घ)unix_मार्गः <= 0) {
    /* We can not translate it, so return the original string */
    प्रत्यागम my_passwd;
  }

  /* If no changes needed, return it */
  यदि(unix_मार्गः == my_passwd->pw_dir) {
    प्रत्यागम my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_मार्गः;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  प्रत्यागम &vms_passwd_cache;
}

#यदि_परिभाषितम् __DECC
#निर्देशः सन्देशः_2 restore
#यदि_अन्तः

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#परिभाषय ASN1_INTEGER_get           ASN1_INTEGER_GET
#परिभाषय ASN1_STRING_दत्त           ASN1_STRING_दत्त_2
#परिभाषय ASN1_STRING_दीर्घता         ASN1_STRING_दीर्घता_2
#परिभाषय ASN1_STRING_print          ASN1_STRING_PRINT
#परिभाषय ASN1_STRING_गन्तव्यम्_UTF8        ASN1_STRING_गन्तव्यम्_UTF8_2
#परिभाषय ASN1_STRING_type           ASN1_STRING_TYPE
#परिभाषय BIO_ctrl                   BIO_CTRL
#परिभाषय BIO_free                   BIO_FREE
#परिभाषय BIO_new                    BIO_NEW
#परिभाषय BIO_s_mem                  BIO_S_MEM
#परिभाषय BN_bn2bin                  BN_BN2BIN
#परिभाषय BN_अङ्कः_द्वयङ्काः                BN_अङ्कः_द्वयङ्काः_2
#परिभाषय CRYPTO_cleanup_all_ex_दत्त CRYPTO_CLEANUP_ALL_EX_दत्त
#परिभाषय CRYPTO_free                CRYPTO_FREE
#परिभाषय CRYPTO_malloc              CRYPTO_MALLOC
#परिभाषय CONF_modules_आरोपय_file     CONF_MODULES_आरोपय_FILE
#यदि_परिभाषितम् __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_आरोपय_file
स्थिर पूर्णाङ्क CONF_modules_आरोपय_file(ध्रुव वर्ण *filename,
                                  ध्रुव वर्ण *appname,
                                  अचिह्नित दीर्घ flags) {
  प्रत्यागम 1;
}
#  endif
#यदि_अन्तः
#परिभाषय DES_ecb_encrypt             DES_ECB_ENCRYPT
#परिभाषय DES_समुच्चयः_कुञ्चिका                 DES_समुच्चयः_कुञ्चिका_2
#परिभाषय DES_समुच्चयः_odd_parity          DES_समुच्चयः_ODD_PARITY
#परिभाषय ENGINE_ctrl                 ENGINE_CTRL
#परिभाषय ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#परिभाषय ENGINE_finish               ENGINE_FINISH
#परिभाषय ENGINE_free                 ENGINE_FREE
#परिभाषय ENGINE_get_first            ENGINE_GET_FIRST
#परिभाषय ENGINE_get_id               ENGINE_GET_ID
#परिभाषय ENGINE_get_अग्र             ENGINE_GET_अग्र
#परिभाषय ENGINE_init                 ENGINE_INIT
#परिभाषय ENGINE_आरोपय_builtin_engines ENGINE_आरोपय_BUILTIN_ENGINES
#परिभाषय ENGINE_आरोपय_वैयक्तिकम्_कुञ्चिका     ENGINE_आरोपय_वैयक्तिकम्_कुञ्चिका_2
#परिभाषय ENGINE_समुच्चयः_default          ENGINE_समुच्चयः_DEFAULT
#परिभाषय ERR_clear_error             ERR_CLEAR_ERROR
#परिभाषय ERR_error_string            ERR_ERROR_STRING
#परिभाषय ERR_error_string_n          ERR_ERROR_STRING_N
#परिभाषय ERR_free_वाक्यानि            ERR_FREE_वाक्यानि
#परिभाषय ERR_get_error               ERR_GET_ERROR
#परिभाषय ERR_peek_error              ERR_PEEK_ERROR
#परिभाषय ERR_remove_state            ERR_REMOVE_STATE
#परिभाषय EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#परिभाषय EVP_PKEY_free               EVP_PKEY_FREE
#परिभाषय EVP_cleanup                 EVP_CLEANUP
#परिभाषय GENERAL_नामानि_free          GENERAL_नामानि_FREE
#परिभाषय i2d_X509_PUBKEY             I2D_X509_PUBKEY
#परिभाषय MD4_Final                   MD4_FINAL
#परिभाषय MD4_Init                    MD4_INIT
#परिभाषय MD4_Update                  MD4_UPDATE
#परिभाषय MD5_Final                   MD5_FINAL
#परिभाषय MD5_Init                    MD5_INIT
#परिभाषय MD5_Update                  MD5_UPDATE
#परिभाषय OPENSSL_सङ्कलय_all_algo_noconf OPENSSL_सङ्कलय_ALL_ALGO_NOCONF
#यदि_अपरिभाषितम् __VAX
#परिभाषय OPENSSL_आरोपय_builtin_modules OPENSSL_आरोपय_BUILTIN_MODULES
#यदि_अन्तः
#परिभाषय PEM_read_X509                   PEM_READ_X509
#परिभाषय PEM_write_bio_X509              PEM_WRITE_BIO_X509
#परिभाषय PKCS12_free                     PKCS12_FREE
#परिभाषय PKCS12_parse                    PKCS12_PARSE
#परिभाषय RAND_सङ्कलय                        RAND_सङ्कलय_2
#परिभाषय RAND_अष्टकानि                      RAND_अष्टकानि_2
#परिभाषय RAND_file_name                  RAND_FILE_NAME
#परिभाषय RAND_आरोपय_file                  RAND_आरोपय_FILE
#परिभाषय RAND_स्थितिः                     RAND_स्थितिः_2
#परिभाषय SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#परिभाषय SSL_CTX_सङ्कलय_client_CA           SSL_CTX_सङ्कलय_CLIENT_CA
#परिभाषय SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#परिभाषय SSL_CTX_check_वैयक्तिकम्_कुञ्चिका       SSL_CTX_CHECK_वैयक्तिकम्_कुञ्चिका
#परिभाषय SSL_CTX_ctrl                    SSL_CTX_CTRL
#परिभाषय SSL_CTX_free                    SSL_CTX_FREE
#परिभाषय SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#परिभाषय SSL_CTX_आरोपय_verify_locations   SSL_CTX_आरोपय_VERIFY_LOCATIONS
#परिभाषय SSL_CTX_new                     SSL_CTX_NEW
#परिभाषय SSL_CTX_समुच्चयः_cipher_list         SSL_CTX_समुच्चयः_CIPHER_LIST
#परिभाषय SSL_CTX_समुच्चयः_def_passwd_cb_ud    SSL_CTX_समुच्चयः_DEF_PASSWD_CB_UD
#परिभाषय SSL_CTX_समुच्चयः_default_passwd_cb   SSL_CTX_समुच्चयः_DEFAULT_PASSWD_CB
#परिभाषय SSL_CTX_समुच्चयः_सन्देशः_callback        SSL_CTX_समुच्चयः_सन्देशः_CALLBACK
#परिभाषय SSL_CTX_समुच्चयः_verify              SSL_CTX_समुच्चयः_VERIFY
#परिभाषय SSL_CTX_use_वैयक्तिकम्_कुञ्चिका          SSL_CTX_USE_PRIVATEKEY
#परिभाषय SSL_CTX_use_वैयक्तिकम्_कुञ्चिका_file     SSL_CTX_USE_PRIVATEKEY_FILE
#परिभाषय SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#परिभाषय SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#परिभाषय SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#परिभाषय SSL_SESSION_free                SSL_SESSION_FREE
#परिभाषय SSL_connect                     SSL_CONNECT
#परिभाषय SSL_free                        SSL_FREE
#परिभाषय SSL_get1_session                SSL_GET1_SESSION
#परिभाषय SSL_get_certificate             SSL_GET_CERTIFICATE
#परिभाषय SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#परिभाषय SSL_get_error                   SSL_GET_ERROR
#परिभाषय SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#परिभाषय SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#परिभाषय SSL_get_privatekey              SSL_GET_PRIVATEKEY
#परिभाषय SSL_get_session                 SSL_GET_SESSION
#परिभाषय SSL_get_shutdown                SSL_GET_SHUTDOWN
#परिभाषय SSL_get_verify_परिणाम           SSL_GET_VERIFY_परिणाम
#परिभाषय SSL_library_init                SSL_LIBRARY_INIT
#परिभाषय SSL_आरोपय_error_वाक्यानि          SSL_आरोपय_ERROR_वाक्यानि
#परिभाषय SSL_new                         SSL_NEW
#परिभाषय SSL_peek                        SSL_PEEK
#परिभाषय SSL_pending                     SSL_PENDING
#परिभाषय SSL_read                        SSL_READ
#परिभाषय SSL_समुच्चयः_connect_state           SSL_समुच्चयः_CONNECT_STATE
#परिभाषय SSL_समुच्चयः_fd                      SSL_समुच्चयः_FD
#परिभाषय SSL_समुच्चयः_session                 SSL_समुच्चयः_SESSION
#परिभाषय SSL_shutdown                    SSL_SHUTDOWN
#परिभाषय SSL_संस्करणम्                     SSL_संस्करणम्_2
#परिभाषय SSL_write                       SSL_WRITE
#परिभाषय SSLeay                          SSLEAY
#परिभाषय SSLv23_client_विधानम्            SSLV23_CLIENT_विधानम्
#परिभाषय SSLv3_client_विधानम्             SSLV3_CLIENT_विधानम्
#परिभाषय TLSv1_client_विधानम्             TLSV1_CLIENT_विधानम्
#परिभाषय UI_create_विधानम्                UI_CREATE_विधानम्
#परिभाषय UI_destroy_विधानम्               UI_DESTROY_विधानम्
#परिभाषय UI_get0_user_दत्त               UI_GET0_USER_दत्त
#परिभाषय UI_get_निवेशः_flags              UI_GET_निवेशः_FLAGS
#परिभाषय UI_get_string_type              UI_GET_STRING_TYPE
#परिभाषय UI_create_विधानम्                UI_CREATE_विधानम्
#परिभाषय UI_destroy_विधानम्               UI_DESTROY_विधानम्
#परिभाषय UI_विधानम्_get_closer            UI_विधानम्_GET_CLOSER
#परिभाषय UI_विधानम्_get_opener            UI_विधानम्_GET_OPENER
#परिभाषय UI_विधानम्_get_reader            UI_विधानम्_GET_READER
#परिभाषय UI_विधानम्_get_writer            UI_विधानम्_GET_WRITER
#परिभाषय UI_विधानम्_समुच्चयः_closer            UI_विधानम्_समुच्चयः_CLOSER
#परिभाषय UI_विधानम्_समुच्चयः_opener            UI_विधानम्_समुच्चयः_OPENER
#परिभाषय UI_विधानम्_समुच्चयः_reader            UI_विधानम्_समुच्चयः_READER
#परिभाषय UI_विधानम्_समुच्चयः_writer            UI_विधानम्_समुच्चयः_WRITER
#परिभाषय UI_OpenSSL                      UI_OPENSSL
#परिभाषय UI_समुच्चयः_परिणाम                   UI_समुच्चयः_परिणाम_2
#परिभाषय X509V3_EXT_print                X509V3_EXT_PRINT
#परिभाषय X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#परिभाषय X509_EXTENSION_get_दत्त         X509_EXTENSION_GET_दत्त
#परिभाषय X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#परिभाषय X509_LOOKUP_file                X509_LOOKUP_FILE
#परिभाषय X509_NAME_प्रविष्टिः_get_दत्त        X509_NAME_प्रविष्टिः_GET_दत्त
#परिभाषय X509_NAME_get_प्रविष्टिः             X509_NAME_GET_प्रविष्टिः
#परिभाषय X509_NAME_get_सूचकांक_by_NID      X509_NAME_GET_सूचकांक_BY_NID
#परिभाषय X509_NAME_print_ex              X509_NAME_PRINT_EX
#परिभाषय X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#परिभाषय X509_STORE_सङ्कलय_lookup           X509_STORE_सङ्कलय_LOOKUP
#परिभाषय X509_STORE_समुच्चयः_flags            X509_STORE_समुच्चयः_FLAGS
#परिभाषय X509_check_issued               X509_CHECK_ISSUED
#परिभाषय X509_free                       X509_FREE
#परिभाषय X509_get_ext_d2i                X509_GET_EXT_D2I
#परिभाषय X509_get_issuer_name            X509_GET_ISSUER_NAME
#परिभाषय X509_get_pubkey                 X509_GET_PUBKEY
#परिभाषय X509_get_serial_सङ्ख्या           X509_GET_SERIALNUMBER
#परिभाषय X509_get_subject_name           X509_GET_SUBJECT_NAME
#परिभाषय X509_आरोपय_crl_file              X509_आरोपय_CRL_FILE
#परिभाषय X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#परिभाषय d2i_PKCS12_fp                   D2I_PKCS12_FP
#परिभाषय i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#परिभाषय sk_अङ्कः                          SK_अङ्कः
#परिभाषय sk_निष्कासय                          SK_निष्कासय
#परिभाषय sk_निष्कासय_free                     SK_निष्कासय_FREE
#परिभाषय sk_मानम्                        SK_मानम्
#परिभाषय SHA256_Final  SHA256_FINAL
#परिभाषय SHA256_Init   SHA256_INIT
#परिभाषय SHA256_Update SHA256_UPDATE

#परिभाषय USE_UPPERCASE_GSSAPI 1
#परिभाषय gss_seal             GSS_SEAL
#परिभाषय gss_unseal           GSS_UNSEAL

#परिभाषय USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अपरिभाषितम् AI_NUMERICHOST
#अपरिभाषय USE_IPV6
#यदि_अन्तः
#यदि_अन्तः

/* VAX symbols are always in uppercase */
#यदि_परिभाषितम् __VAX
#परिभाषय inflate       INFLATE
#परिभाषय inflate_अन्तः    INFLATEEND
#परिभाषय inflateInit2_ INFLATEINIT2_
#परिभाषय inflateInit_  INFLATEINIT_
#परिभाषय zlib_संस्करणम्   ZLIBVERSION
#यदि_अन्तः

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#यदि_परिभाषितम् __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_सङ्कलय_all_algorithms
#    define Open_SSL_सङ्कलय_all_algorithms OPENSSL_सङ्कलय_ALL_ALGORITHMS
     शून्य OPENSSL_सङ्कलय_ALL_ALGORITHMS(शून्य);
#  endif
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_SETUP_VMS_H */
