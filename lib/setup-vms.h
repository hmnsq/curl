#నిర్వచితం_కాకపోతే శీర్షిక_CURL_అమరిక_VMS_H
#నిర్వచించు శీర్షిక_CURL_అమరిక_VMS_H
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

#నిర్వచితమైతే __DECC
#సూచన సందేశం_2 save
#సూచన సందేశం_2 disable dollarid
#అయితే_ముగింపు

/* Hide the stuff we are overriding */
#నిర్వచించు getenv decc_getenv
#నిర్వచితమైతే __DECC
#  ఒకవేళ __INITIAL_POINTER_SIZE != 64
#    define getpwuid decc_getpwuid
#  endif
#అయితే_ముగింపు
#అన్తర్భావ <stdlib.h>
అక్షరం *decc$getenv(స్థిరరాశి అక్షరం *__name);
#అన్తర్భావ <pwd.h>

#అన్తర్భావ <string.h>
#అన్తర్భావ <unixlib.h>

#నిర్వచనం_రద్దు getenv
#నిర్వచనం_రద్దు getpwuid
#నిర్వచించు getenv   vms_getenv
#నిర్వచించు getpwuid vms_getpwuid

/* VAX needs these in upper case when compiling exact case */
#నిర్వచించు sys$కేటాయించు SYS$కేటాయించు_2
#నిర్వచించు sys$dassgn SYS$DASSGN
#నిర్వచించు sys$qiow   SYS$QIOW

#నిర్వచితమైతే __DECC
#  ఒకవేళ __INITIAL_POINTER_SIZE
#    pragma __pointer_size __save
#  endif
#అయితే_ముగింపు

#ఒకవేళ __USE_LONG_GID_T
#  define decc_getpwuid DECC$__LONG_GID_GETPWUID
#లేకపోతే
#  ఒకవేళ __INITIAL_POINTER_SIZE
#    define decc_getpwuid decc$__32_getpwuid
#  లేకపోతే
#    define decc_getpwuid decc$getpwuid
#  endif
#అయితే_ముగింపు

నిర్మాణం passwd *decc_getpwuid(uid_t uid);

#నిర్వచితమైతే __DECC
#  ఒకవేళ __INITIAL_POINTER_SIZE == 32
/* Translate the path, but only if the path is a VMS file specification */
/* The translation is usually only needed for older versions of VMS */
స్థిర అక్షరం *vms_translate_మార్గం(స్థిరరాశి అక్షరం *మార్గం)
{
  అక్షరం *unix_మార్గం;
  అక్షరం *test_str;

  /* See if the result is in VMS format, if not, we are done */
  /* Assume that this is a PATH, not some data */
  test_str = strpbrk(మార్గం, ":[<^");
  ఒకవేళ(!test_str) {
    తిరిగి (అక్షరం *)మార్గం;
  }

  unix_మార్గం = decc$translate_vms(మార్గం);

  ఒకవేళ((పూర్ణాంకం)unix_మార్గం <= 0) {
    /* We can not translate it, so return the original string */
    తిరిగి (అక్షరం *)మార్గం;
  }
}
#  లేకపోతే
   /* VMS translate path is actually not needed on the current 64-bit */
   /* VMS platforms, so instead of figuring out the pointer settings */
   /* Change it to a noop */
#  define vms_translate_మార్గం(__path) __path
#  endif
#అయితే_ముగింపు

#నిర్వచితమైతే __DECC
#  ఒకవేళ __INITIAL_POINTER_SIZE
#    pragma __pointer_size __restore
#  endif
#అయితే_ముగింపు

స్థిర అక్షరం *vms_getenv(స్థిరరాశి అక్షరం *envvar)
{
  అక్షరం *ఫలితం;
  అక్షరం *vms_మార్గం;

  /* first use the DECC getenv() function */
  ఫలితం = decc$getenv(envvar);
  ఒకవేళ(!ఫలితం) {
    తిరిగి ఫలితం;
  }

  vms_మార్గం = ఫలితం;
  ఫలితం = vms_translate_మార్గం(vms_మార్గం);

  /* note that if you backport this to use VAX C RTL, that the VAX C RTL */
  /* may do a malloc(2048) for each call to getenv(), so you will need   */
  /* to add a free(vms_path) */
  /* Do not do a free() for DEC C RTL builds, which should be used for */
  /* VMS 5.5-2 and later, even if using GCC */

  తిరిగి ఫలితం;
}

స్థిర నిర్మాణం passwd vms_passwd_cache;

స్థిర నిర్మాణం passwd *vms_getpwuid(uid_t uid)
{
  నిర్మాణం passwd *my_passwd;

/* Hack needed to support 64-bit builds, decc_getpwnam is 32-bit only */
#నిర్వచితమైతే __DECC
#  ఒకవేళ __INITIAL_POINTER_SIZE
  __char_ptr32 unix_మార్గం;
#  లేకపోతే
  అక్షరం *unix_మార్గం;
#  endif
#లేకపోతే
  అక్షరం *unix_మార్గం;
#అయితే_ముగింపు

  my_passwd = decc_getpwuid(uid);
  ఒకవేళ(!my_passwd) {
    తిరిగి my_passwd;
  }

  unix_మార్గం = vms_translate_మార్గం(my_passwd->pw_dir);

  ఒకవేళ((దీర్ఘ)unix_మార్గం <= 0) {
    /* We can not translate it, so return the original string */
    తిరిగి my_passwd;
  }

  /* If no changes needed, return it */
  ఒకవేళ(unix_మార్గం == my_passwd->pw_dir) {
    తిరిగి my_passwd;
  }

  /* Need to copy the structure returned */
  /* Since curl is only using pw_dir, no need to fix up */
  /* the pw_shell when running under Bash */
  vms_passwd_cache.pw_name = my_passwd->pw_name;
  vms_passwd_cache.pw_uid = my_passwd->pw_uid;
  vms_passwd_cache.pw_gid = my_passwd->pw_uid;
  vms_passwd_cache.pw_dir = unix_మార్గం;
  vms_passwd_cache.pw_shell = my_passwd->pw_shell;

  తిరిగి &vms_passwd_cache;
}

#నిర్వచితమైతే __DECC
#సూచన సందేశం_2 restore
#అయితే_ముగింపు

/* Bug - VMS OpenSSL and Kerberos universal symbols are in uppercase only */
/* VMS libraries should have universal symbols in exact and uppercase */

#నిర్వచించు ASN1_INTEGER_get           ASN1_INTEGER_GET
#నిర్వచించు ASN1_STRING_దత్తాంశం           ASN1_STRING_దత్తాంశం_2
#నిర్వచించు ASN1_STRING_నిడివి         ASN1_STRING_నిడివి_2
#నిర్వచించు ASN1_STRING_print          ASN1_STRING_PRINT
#నిర్వచించు ASN1_STRING_గమ్యం_UTF8        ASN1_STRING_గమ్యం_UTF8_2
#నిర్వచించు ASN1_STRING_type           ASN1_STRING_TYPE
#నిర్వచించు BIO_ctrl                   BIO_CTRL
#నిర్వచించు BIO_free                   BIO_FREE
#నిర్వచించు BIO_new                    BIO_NEW
#నిర్వచించు BIO_s_mem                  BIO_S_MEM
#నిర్వచించు BN_bn2bin                  BN_BN2BIN
#నిర్వచించు BN_అంకె_బిట్లు                BN_అంకె_బిట్లు_2
#నిర్వచించు CRYPTO_cleanup_all_ex_దత్తాంశం CRYPTO_CLEANUP_ALL_EX_దత్తాంశం
#నిర్వచించు CRYPTO_free                CRYPTO_FREE
#నిర్వచించు CRYPTO_malloc              CRYPTO_MALLOC
#నిర్వచించు CONF_modules_ఎక్కించు_file     CONF_MODULES_ఎక్కించు_FILE
#నిర్వచితమైతే __VAX
#  ifdef VMS_OLD_SSL
  /* Ancient OpenSSL on VAX/VMS missing this constant */
#    define CONF_MFLAGS_IGNORE_MISSING_FILE 0x10
#    undef CONF_modules_ఎక్కించు_file
స్థిర పూర్ణాంకం CONF_modules_ఎక్కించు_file(స్థిరరాశి అక్షరం *filename,
                                  స్థిరరాశి అక్షరం *appname,
                                  గుర్తులేని దీర్ఘ flags) {
  తిరిగి 1;
}
#  endif
#అయితే_ముగింపు
#నిర్వచించు DES_ecb_encrypt             DES_ECB_ENCRYPT
#నిర్వచించు DES_సమితి_కీలకం                 DES_సమితి_కీలకం_2
#నిర్వచించు DES_సమితి_odd_parity          DES_సమితి_ODD_PARITY
#నిర్వచించు ENGINE_ctrl                 ENGINE_CTRL
#నిర్వచించు ENGINE_ctrl_cmd             ENGINE_CTRL_CMD
#నిర్వచించు ENGINE_ముగింపు               ENGINE_ముగింపు_2
#నిర్వచించు ENGINE_free                 ENGINE_FREE
#నిర్వచించు ENGINE_get_మొదటి            ENGINE_GET_మొదటి
#నిర్వచించు ENGINE_get_id               ENGINE_GET_ID
#నిర్వచించు ENGINE_get_తదుపరి             ENGINE_GET_తదుపరి
#నిర్వచించు ENGINE_init                 ENGINE_INIT
#నిర్వచించు ENGINE_ఎక్కించు_builtin_engines ENGINE_ఎక్కించు_BUILTIN_ENGINES
#నిర్వచించు ENGINE_ఎక్కించు_వ్యక్తిగత_కీలకం     ENGINE_ఎక్కించు_వ్యక్తిగత_కీలకం_2
#నిర్వచించు ENGINE_సమితి_default          ENGINE_సమితి_DEFAULT
#నిర్వచించు ERR_clear_error             ERR_CLEAR_ERROR
#నిర్వచించు ERR_error_string            ERR_ERROR_STRING
#నిర్వచించు ERR_error_string_n          ERR_ERROR_STRING_N
#నిర్వచించు ERR_free_అక్షరశ్రేణులు            ERR_FREE_అక్షరశ్రేణులు
#నిర్వచించు ERR_get_error               ERR_GET_ERROR
#నిర్వచించు ERR_peek_error              ERR_PEEK_ERROR
#నిర్వచించు ERR_remove_state            ERR_REMOVE_STATE
#నిర్వచించు EVP_PKEY_copy_parameters    EVP_PKEY_COPY_PARAMETERS
#నిర్వచించు EVP_PKEY_free               EVP_PKEY_FREE
#నిర్వచించు EVP_cleanup                 EVP_CLEANUP
#నిర్వచించు GENERAL_పేర్లు_free          GENERAL_పేర్లు_FREE
#నిర్వచించు i2d_X509_PUBKEY             I2D_X509_PUBKEY
#నిర్వచించు MD4_Final                   MD4_FINAL
#నిర్వచించు MD4_Init                    MD4_INIT
#నిర్వచించు MD4_Update                  MD4_UPDATE
#నిర్వచించు MD5_Final                   MD5_FINAL
#నిర్వచించు MD5_Init                    MD5_INIT
#నిర్వచించు MD5_Update                  MD5_UPDATE
#నిర్వచించు OPENSSL_కూడు_all_algo_noconf OPENSSL_కూడు_ALL_ALGO_NOCONF
#నిర్వచితం_కాకపోతే __VAX
#నిర్వచించు OPENSSL_ఎక్కించు_builtin_modules OPENSSL_ఎక్కించు_BUILTIN_MODULES
#అయితే_ముగింపు
#నిర్వచించు PEM_read_X509                   PEM_READ_X509
#నిర్వచించు PEM_write_bio_X509              PEM_WRITE_BIO_X509
#నిర్వచించు PKCS12_free                     PKCS12_FREE
#నిర్వచించు PKCS12_parse                    PKCS12_PARSE
#నిర్వచించు RAND_కూడు                        RAND_కూడు_2
#నిర్వచించు RAND_బైట్లు                      RAND_బైట్లు_2
#నిర్వచించు RAND_file_name                  RAND_FILE_NAME
#నిర్వచించు RAND_ఎక్కించు_file                  RAND_ఎక్కించు_FILE
#నిర్వచించు RAND_స్థితి                     RAND_స్థితి_2
#నిర్వచించు SSL_CIPHER_get_name             SSL_CIPHER_GET_NAME
#నిర్వచించు SSL_CTX_కూడు_client_CA           SSL_CTX_కూడు_CLIENT_CA
#నిర్వచించు SSL_CTX_callback_ctrl           SSL_CTX_CALLBACK_CTRL
#నిర్వచించు SSL_CTX_check_వ్యక్తిగత_కీలకం       SSL_CTX_CHECK_వ్యక్తిగత_కీలకం
#నిర్వచించు SSL_CTX_ctrl                    SSL_CTX_CTRL
#నిర్వచించు SSL_CTX_free                    SSL_CTX_FREE
#నిర్వచించు SSL_CTX_get_cert_store          SSL_CTX_GET_CERT_STORE
#నిర్వచించు SSL_CTX_ఎక్కించు_verify_locations   SSL_CTX_ఎక్కించు_VERIFY_LOCATIONS
#నిర్వచించు SSL_CTX_new                     SSL_CTX_NEW
#నిర్వచించు SSL_CTX_సమితి_cipher_list         SSL_CTX_సమితి_CIPHER_LIST
#నిర్వచించు SSL_CTX_సమితి_def_passwd_cb_ud    SSL_CTX_సమితి_DEF_PASSWD_CB_UD
#నిర్వచించు SSL_CTX_సమితి_default_passwd_cb   SSL_CTX_సమితి_DEFAULT_PASSWD_CB
#నిర్వచించు SSL_CTX_సమితి_సందేశం_callback        SSL_CTX_సమితి_సందేశం_CALLBACK
#నిర్వచించు SSL_CTX_సమితి_verify              SSL_CTX_సమితి_VERIFY
#నిర్వచించు SSL_CTX_use_వ్యక్తిగత_కీలకం          SSL_CTX_USE_PRIVATEKEY
#నిర్వచించు SSL_CTX_use_వ్యక్తిగత_కీలకం_file     SSL_CTX_USE_PRIVATEKEY_FILE
#నిర్వచించు SSL_CTX_use_cert_chain_file     SSL_CTX_USE_CERT_CHAIN_FILE
#నిర్వచించు SSL_CTX_use_certificate         SSL_CTX_USE_CERTIFICATE
#నిర్వచించు SSL_CTX_use_certificate_file    SSL_CTX_USE_CERTIFICATE_FILE
#నిర్వచించు SSL_SESSION_free                SSL_SESSION_FREE
#నిర్వచించు SSL_connect                     SSL_CONNECT
#నిర్వచించు SSL_free                        SSL_FREE
#నిర్వచించు SSL_get1_session                SSL_GET1_SESSION
#నిర్వచించు SSL_get_certificate             SSL_GET_CERTIFICATE
#నిర్వచించు SSL_get_current_cipher          SSL_GET_CURRENT_CIPHER
#నిర్వచించు SSL_get_error                   SSL_GET_ERROR
#నిర్వచించు SSL_get_peer_cert_chain         SSL_GET_PEER_CERT_CHAIN
#నిర్వచించు SSL_get_peer_certificate        SSL_GET_PEER_CERTIFICATE
#నిర్వచించు SSL_get_privatekey              SSL_GET_PRIVATEKEY
#నిర్వచించు SSL_get_session                 SSL_GET_SESSION
#నిర్వచించు SSL_get_shutdown                SSL_GET_SHUTDOWN
#నిర్వచించు SSL_get_verify_ఫలితం           SSL_GET_VERIFY_ఫలితం
#నిర్వచించు SSL_library_init                SSL_LIBRARY_INIT
#నిర్వచించు SSL_ఎక్కించు_error_అక్షరశ్రేణులు          SSL_ఎక్కించు_ERROR_అక్షరశ్రేణులు
#నిర్వచించు SSL_new                         SSL_NEW
#నిర్వచించు SSL_peek                        SSL_PEEK
#నిర్వచించు SSL_pending                     SSL_PENDING
#నిర్వచించు SSL_read                        SSL_READ
#నిర్వచించు SSL_సమితి_connect_state           SSL_సమితి_CONNECT_STATE
#నిర్వచించు SSL_సమితి_fd                      SSL_సమితి_FD
#నిర్వచించు SSL_సమితి_session                 SSL_సమితి_SESSION
#నిర్వచించు SSL_shutdown                    SSL_SHUTDOWN
#నిర్వచించు SSL_సంస్కరణ                     SSL_సంస్కరణ_2
#నిర్వచించు SSL_write                       SSL_WRITE
#నిర్వచించు SSLeay                          SSLEAY
#నిర్వచించు SSLv23_client_పద్ధతి            SSLV23_CLIENT_పద్ధతి
#నిర్వచించు SSLv3_client_పద్ధతి             SSLV3_CLIENT_పద్ధతి
#నిర్వచించు TLSv1_client_పద్ధతి             TLSV1_CLIENT_పద్ధతి
#నిర్వచించు UI_create_పద్ధతి                UI_CREATE_పద్ధతి
#నిర్వచించు UI_destroy_పద్ధతి               UI_DESTROY_పద్ధతి
#నిర్వచించు UI_get0_user_దత్తాంశం               UI_GET0_USER_దత్తాంశం
#నిర్వచించు UI_get_ఆదానం_flags              UI_GET_ఆదానం_FLAGS
#నిర్వచించు UI_get_string_type              UI_GET_STRING_TYPE
#నిర్వచించు UI_create_పద్ధతి                UI_CREATE_పద్ధతి
#నిర్వచించు UI_destroy_పద్ధతి               UI_DESTROY_పద్ధతి
#నిర్వచించు UI_పద్ధతి_get_closer            UI_పద్ధతి_GET_CLOSER
#నిర్వచించు UI_పద్ధతి_get_opener            UI_పద్ధతి_GET_OPENER
#నిర్వచించు UI_పద్ధతి_get_reader            UI_పద్ధతి_GET_READER
#నిర్వచించు UI_పద్ధతి_get_writer            UI_పద్ధతి_GET_WRITER
#నిర్వచించు UI_పద్ధతి_సమితి_closer            UI_పద్ధతి_సమితి_CLOSER
#నిర్వచించు UI_పద్ధతి_సమితి_opener            UI_పద్ధతి_సమితి_OPENER
#నిర్వచించు UI_పద్ధతి_సమితి_reader            UI_పద్ధతి_సమితి_READER
#నిర్వచించు UI_పద్ధతి_సమితి_writer            UI_పద్ధతి_సమితి_WRITER
#నిర్వచించు UI_OpenSSL                      UI_OPENSSL
#నిర్వచించు UI_సమితి_ఫలితం                   UI_సమితి_ఫలితం_2
#నిర్వచించు X509V3_EXT_print                X509V3_EXT_PRINT
#నిర్వచించు X509_EXTENSION_get_critical     X509_EXTENSION_GET_CRITICAL
#నిర్వచించు X509_EXTENSION_get_దత్తాంశం         X509_EXTENSION_GET_దత్తాంశం
#నిర్వచించు X509_EXTENSION_get_object       X509_EXTENSION_GET_OBJECT
#నిర్వచించు X509_LOOKUP_file                X509_LOOKUP_FILE
#నిర్వచించు X509_NAME_నమోదు_get_దత్తాంశం        X509_NAME_నమోదు_GET_దత్తాంశం
#నిర్వచించు X509_NAME_get_నమోదు             X509_NAME_GET_నమోదు
#నిర్వచించు X509_NAME_get_సూచీ_by_NID      X509_NAME_GET_సూచీ_BY_NID
#నిర్వచించు X509_NAME_print_ex              X509_NAME_PRINT_EX
#నిర్వచించు X509_STORE_CTX_get_current_cert X509_STORE_CTX_GET_CURRENT_CERT
#నిర్వచించు X509_STORE_కూడు_lookup           X509_STORE_కూడు_LOOKUP
#నిర్వచించు X509_STORE_సమితి_flags            X509_STORE_సమితి_FLAGS
#నిర్వచించు X509_check_issued               X509_CHECK_ISSUED
#నిర్వచించు X509_free                       X509_FREE
#నిర్వచించు X509_get_ext_d2i                X509_GET_EXT_D2I
#నిర్వచించు X509_get_issuer_name            X509_GET_ISSUER_NAME
#నిర్వచించు X509_get_pubkey                 X509_GET_PUBKEY
#నిర్వచించు X509_get_serial_సంఖ్య           X509_GET_SERIALNUMBER
#నిర్వచించు X509_get_subject_name           X509_GET_SUBJECT_NAME
#నిర్వచించు X509_ఎక్కించు_crl_file              X509_ఎక్కించు_CRL_FILE
#నిర్వచించు X509_verify_cert_error_string   X509_VERIFY_CERT_ERROR_STRING
#నిర్వచించు d2i_PKCS12_fp                   D2I_PKCS12_FP
#నిర్వచించు i2t_ASN1_OBJECT                 I2T_ASN1_OBJECT
#నిర్వచించు sk_అంకె                          SK_అంకె
#నిర్వచించు sk_తీయి                          SK_తీయి
#నిర్వచించు sk_తీయి_free                     SK_తీయి_FREE
#నిర్వచించు sk_విలువ                        SK_విలువ
#నిర్వచించు SHA256_Final  SHA256_FINAL
#నిర్వచించు SHA256_Init   SHA256_INIT
#నిర్వచించు SHA256_Update SHA256_UPDATE

#నిర్వచించు USE_UPPERCASE_GSSAPI 1
#నిర్వచించు gss_seal             GSS_SEAL
#నిర్వచించు gss_unseal           GSS_UNSEAL

#నిర్వచించు USE_UPPERCASE_KRBAPI 1

/* AI_NUMERICHOST needed for IP V6 support in curl */
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#నిర్వచితం_కాకపోతే AI_NUMERICHOST
#నిర్వచనం_రద్దు USE_IPV6
#అయితే_ముగింపు
#అయితే_ముగింపు

/* VAX symbols are always in uppercase */
#నిర్వచితమైతే __VAX
#నిర్వచించు inflate       INFLATE
#నిర్వచించు inflate_చివర    INFLATEEND
#నిర్వచించు inflateInit2_ INFLATEINIT2_
#నిర్వచించు inflateInit_  INFLATEINIT_
#నిర్వచించు zlib_సంస్కరణ   ZLIBVERSION
#అయితే_ముగింపు

/* Older VAX OpenSSL port defines these as Macros */
/* Need to include the headers first and then redefine */
/* that way a newer port will also work if some one has one */
#నిర్వచితమైతే __VAX

#  include <openssl/evp.h>
#  ifndef Open_SSL_కూడు_all_algorithms
#    define Open_SSL_కూడు_all_algorithms OPENSSL_కూడు_ALL_ALGORITHMS
     శూన్య OPENSSL_కూడు_ALL_ALGORITHMS(శూన్య);
#  endif
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_SETUP_VMS_H */
