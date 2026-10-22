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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_IPHLPAPI_H
#अन्तर्भाव <Iphlpapi.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् HAVE_SOCKET
#दोषः "We cannot compile without socket() support"
#यदि_अन्तः

#यदि defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#अन्तर्भाव <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#यदि_अन्तः
#अन्तर्भाव <iphlpapi.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vssh/vssh.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "http_digest.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "getinfo.h"
#अन्तर्भाव "pop3.h"
#अन्तर्भाव "urlapi-int.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "proxy.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "idn.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "conncache.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "setopt.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "headers.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "peer.h"

/* Now for the protocols */
#अन्तर्भाव "ftp.h"
#अन्तर्भाव "dict.h"
#अन्तर्भाव "telnet.h"
#अन्तर्भाव "tftp.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "curl_ldap.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "imap.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "gopher.h"
#अन्तर्भाव "mqtt.h"
#अन्तर्भाव "rtsp.h"
#अन्तर्भाव "smtp.h"
#अन्तर्भाव "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#यदि READBUFFER_आकार < READBUFFER_MIN
# error READBUFFER_आकार is too small
#यदि_अन्तः

/*
 * get_protocol_family()
 *
 * This is used to return the protocol family for a given protocol.
 *
 * Parameters:
 *
 * 's'  [in]  - struct Curl_scheme pointer.
 *
 * Returns the family as a single bit protocol identifier.
 */
स्थिर curl_prot_t get_protocol_family(ध्रुव संरचना Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  प्रत्यागम s->family;
}

शून्य Curl_freeset(संरचना Curl_easy *दत्त)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  गणना dupblob j;

  CURL_EASY_STR_CLEAR0(दत्त, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(दत्त, STRING_कुञ्चिका_PASSWD);
  CURL_EASY_STR_CLEAR0(दत्त, STRING_BEARER);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(दत्त, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(दत्त, STRING_कुञ्चिका_PASSWD_PROXY);
#यदि_अन्तः
  Curl_u8_strset_clear(&दत्त->समुच्चयः.वाक्यानि);
  curlx_safefree(दत्त->समुच्चयः.str_copypostfields);

  प्रति(j = (गणना dupblob)0; j < BLOB_चरमः; j++) {
    curlx_safefree(दत्त->समुच्चयः.blobs[j]);
  }

  Curl_bufref_free(&दत्त->state.referer);
  Curl_bufref_free(&दत्त->state.url);

#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(दत्त->समुच्चयः.mimepostp);
  curlx_safefree(दत्त->समुच्चयः.mimepostp);
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  curl_slist_free_all(दत्त->state.cookielist);
  दत्त->state.cookielist = रिक्त;
#यदि_अन्तः
}

/* free the URL pieces */
स्थिर शून्य ऊर्ध्वम्_free(संरचना Curl_easy *दत्त)
{
  संरचना urlpieces *ऊर्ध्वम् = &दत्त->state.ऊर्ध्वम्;
  curlx_safefree(ऊर्ध्वम्->options);
  curlx_safefree(ऊर्ध्वम्->मार्गः);
  curlx_safefree(ऊर्ध्वम्->query);
  curl_url_cleanup(दत्त->state.uh);
  दत्त->state.uh = रिक्त;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(संरचना Curl_easy **datap)
{
  संरचना Curl_easy *दत्त;

  यदि(!datap || !*datap)
    प्रत्यागम CURLE_OK;

  दत्त = *datap;
  *datap = रिक्त;

  यदि(!दत्त->state.आन्तरिकम् && दत्त->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(दत्त->multi, दत्त);
  }
  अन्यथा {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(दत्त);
    यदि(!दत्त->state.आन्तरिकम् && दत्त->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(दत्त->multi_easy);
      दत्त->multi_easy = रिक्त;
    }
  }
  DEBUGASSERT(!दत्त->conn || दत्त->state.आन्तरिकम्);

  Curl_expire_clear_all(दत्त); /* shut off any timers left */

  यदि(दत्त->state.rangestringalloc)
    curlx_free(दत्त->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(दत्त);

  दत्त->समुच्चयः.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  दत्त->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&दत्त->req, दत्त);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(दत्त);
  Curl_peer_unlink(&दत्त->state.origin);
  Curl_peer_unlink(&दत्त->state.initial_origin);
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  Curl_peer_unlink(&दत्त->state.rtsp_session_origin);
#यदि_अन्तः
  Curl_ssl_free_certinfo(दत्त);

  Curl_bufref_free(&दत्त->state.referer);

  ऊर्ध्वम्_free(दत्त);
  curlx_dyn_free(&दत्त->state.headerb);
  Curl_flush_cookies(दत्त, TRUE);
#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  Curl_altsvc_save(दत्त, दत्त->asi, CURL_EASY_STR(दत्त, STRING_ALTSVC));
  Curl_altsvc_cleanup(&दत्त->asi);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  Curl_hsts_save(दत्त, दत्त->hsts, CURL_EASY_STR(दत्त, STRING_HSTS));
  यदि(!दत्त->share || !दत्त->share->hsts)
    Curl_hsts_cleanup(&दत्त->hsts);
  curl_slist_free_all(दत्त->state.hstslist); /* clean up list */
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(दत्त);
#यदि_अन्तः
  curlx_safefree(दत्त->state.most_recent_ftp_entrypath);
  curlx_safefree(दत्त->info.contenttype);
  curlx_safefree(दत्त->info.wouldredirect);
  curlx_safefree(दत्त->info.effective_url);

  /* No longer a dirty share, if it exists */
  यदि(Curl_share_easy_unlink(दत्त))
    DEBUGASSERT(0);

  Curl_hash_destroy(&दत्त->meta_hash);
  Curl_creds_unlink(&दत्त->state.creds);
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  curlx_safefree(दत्त->state.rangeline);
  curlx_safefree(दत्त->state.http_host);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
  curlx_safefree(दत्त->req.cookiehost);
#यदि_अन्तः

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(दत्त->state.formp);
  curlx_safefree(दत्त->state.formp);
#यदि_अन्तः

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&दत्त->wildcard);
  Curl_freeset(दत्त);
  Curl_शीर्षकाणि_cleanup(दत्त);
  Curl_netrc_cleanup(&दत्त->state.netrc);
#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
  curlx_free(दत्त->state.envproxy);
#यदि_अन्तः
  curlx_memzero(दत्त, परिमाण(*दत्त));
  curlx_free(दत्त);
  प्रत्यागम CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
शून्य Curl_init_userdefined(संरचना Curl_easy *दत्त)
{
  संरचना UserDefined *समुच्चयः = &दत्त->समुच्चयः;

  समुच्चयः->बहिर्गमनम् = stdout;  /* default output to stdout */
  समुच्चयः->अन्तर्_समुच्चयः = stdin;  /* default input from stdin */
  समुच्चयः->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&दत्त->समुच्चयः.वाक्यानि);

#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
  /* use fwrite as default function to store output */
  समुच्चयः->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  समुच्चयः->fread_func_समुच्चयः = (curl_read_callback)fread;
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
  समुच्चयः->is_fread_समुच्चयः = 0;

  समुच्चयः->seek_client = ZERO_NULL;

  समुच्चयः->filesize = -1;        /* we do not know the size */
  समुच्चयः->postfieldsize = -1;   /* unknown size */
  समुच्चयः->maxredirs = 30;       /* sensible default */

  समुच्चयः->विधानम् = HTTPREQ_GET; /* Default HTTP request */
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  समुच्चयः->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  समुच्चयः->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  समुच्चयः->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  समुच्चयः->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  समुच्चयः->ftp_filemethod = FTPFILE_MULTICWD;
  समुच्चयः->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#यदि_अन्तः
  समुच्चयः->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  समुच्चयः->ssl_ca_cache_timeout = 24 * 60 * 60;

  समुच्चयः->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&दत्त->समुच्चयः.ssl);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  Curl_ssl_config_init(&दत्त->समुच्चयः.proxy_ssl);
  समुच्चयः->proxyport = 0;
  समुच्चयः->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  समुच्चयः->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  समुच्चयः->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  समुच्चयः->doh_verifyhost = TRUE;
  समुच्चयः->doh_verifypeer = TRUE;
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  /* defaults to any auth type */
  समुच्चयः->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  समुच्चयः->new_directory_perms = 0755; /* Default permissions */
#यदि_अन्तः

  समुच्चयः->new_file_perms = 0644;    /* Default permissions */
  समुच्चयः->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  समुच्चयः->redir_protocols = CURLPROTO_REDIR;

#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  समुच्चयः->socks5_gssapi_nec = FALSE;
#यदि_अन्तः

  /* set default minimum TLS version */
#यदि_परिभाषितम् USE_SSL
  Curl_setopt_SSLVERSION(दत्त, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(दत्त, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#यदि_अन्तः
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  समुच्चयः->wildcard_enabled = FALSE;
  समुच्चयः->chunk_bgn = ZERO_NULL;
  समुच्चयः->chunk_अन्तः = ZERO_NULL;
  समुच्चयः->fnmatch = ZERO_NULL;
#यदि_अन्तः
  समुच्चयः->tcp_keepalive = FALSE;
  समुच्चयः->tcp_keepintvl = 60;
  समुच्चयः->tcp_keepidle = 60;
  समुच्चयः->tcp_keepcnt = 9;
  समुच्चयः->tcp_fastopen = FALSE;
  समुच्चयः->tcp_nodelay = TRUE;
  समुच्चयः->ssl_enable_alpn = TRUE;
  समुच्चयः->expect_100_timeout = 1000L; /* Wait for a second by default. */
  समुच्चयः->sep_शीर्षकाणि = TRUE; /* separated header lists by default */
  समुच्चयः->बफर_आकार = READBUFFER_आकार;
  समुच्चयः->upload_बफर_आकार = UPLOADBUFFER_DEFAULT;
  समुच्चयः->upload_flags = CURLULFLAG_आलोकितम्;
  समुच्चयः->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  समुच्चयः->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  समुच्चयः->maxconnects = DEFAULT_CONNCACHE_आकार; /* for easy handles */
  समुच्चयः->conn_max_idle_ms = 118 * 1000;
  समुच्चयः->conn_max_age_ms = 24 * 3600 * 1000;
  समुच्चयः->http09_allowed = FALSE;
  समुच्चयः->httpwant = CURL_HTTP_संस्करणम्_NONE;
#यदि defined(USE_HTTP2) || defined(USE_HTTP3)
  समुच्चयः->weight = 0;
#यदि_अन्तः
  समुच्चयः->quick_exit = 0L;
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  समुच्चयः->ws_raw_बहुलकम् = FALSE;
  समुच्चयः->ws_no_auto_pong = FALSE;
#यदि_अन्तः
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
स्थिर शून्य easy_meta_freeentry(शून्य *p)
{
  (शून्य)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

/**
 * Curl_open()
 *
 * @param curl is a pointer to a Curl_easy pointer that gets set by this
 * function.
 * @return CURLcode
 */
CURLcode Curl_open(संरचना Curl_easy **curl)
{
  संरचना Curl_easy *दत्त;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  दत्त = curlx_calloc(1, परिमाण(संरचना Curl_easy));
  यदि(!दत्त) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  दत्त->magic = CURLEASY_MAGIC_सङ्ख्या;
  /* most recent connection is not yet defined */
  दत्त->state.चरमः_conn_id = -1;
  दत्त->state.चरमः_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  दत्त->id = -1;
  दत्त->mid = UINT32_MAX;
  दत्त->master_mid = UINT32_MAX;
  दत्त->प्रगतिः.hide = TRUE;

  Curl_hash_init(&दत्त->meta_hash, 23, CURL_HASH_TYPE_अष्टकानि,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_चरमः <= UINT8_MAX);
  Curl_u8_strset_init(&दत्त->समुच्चयः.वाक्यानि);
  curlx_dyn_init(&दत्त->state.headerb, CURL_MAX_HTTP_शीर्षकम्);
  Curl_bufref_init(&दत्त->state.url);
  Curl_bufref_init(&दत्त->state.referer);
  Curl_req_init(&दत्त->req);
  Curl_initinfo(दत्त);
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  Curl_llist_init(&दत्त->state.httphdrs, रिक्त);
#यदि_अन्तः
  Curl_netrc_init(&दत्त->state.netrc);
  Curl_init_userdefined(दत्त);

  *curl = दत्त;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_conn_free(संरचना Curl_easy *दत्त, संरचना connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  यदि(conn->scheme && conn->scheme->सततम्->disconnect &&
     !conn->द्वयङ्काः.shutdown_handler)
    conn->scheme->सततम्->disconnect(दत्त, conn, TRUE);

  प्रति(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(दत्त, conn, i);
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#यदि_अन्तः
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->द्वारा_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->द्वारा_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
स्थिर bool xfer_may_multiplex(ध्रुव संरचना Curl_easy *दत्त,
                               ध्रुव संरचना connectdata *conn)
{
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  यदि((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->द्वयङ्काः.protoconnstart || !conn->द्वयङ्काः.close)) {

    यदि(Curl_multiplex_wanted(दत्त->multi) &&
       (दत्त->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      प्रत्यागम TRUE;
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)conn;
#यदि_अन्तः
  प्रत्यागम FALSE;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
स्थिर bool proxy_info_matches(ध्रुव संरचना proxy_info *दत्त,
                               ध्रुव संरचना proxy_info *needle)
{
  यदि((दत्त->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(दत्त->peer, needle->peer) &&
     Curl_creds_same(दत्त->creds, needle->creds)) {
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#यदि_अन्तः

#यदि_परिभाषितम् USE_SSH
स्थिर bool ssh_config_matches(संरचना connectdata *one,
                               संरचना connectdata *two)
{
  संरचना ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  प्रत्यागम sshc1 && sshc2 && Curl_safecmp(sshc1->priv_कुञ्चिका, sshc2->priv_कुञ्चिका) &&
         Curl_safecmp(sshc1->pub_कुञ्चिका, sshc2->pub_कुञ्चिका);
}
#यदि_अन्तः

संरचना url_conn_मेलनम् {
  संरचना connectdata *प्राप्तम्;
  संरचना Curl_easy *दत्त;
  संरचना connectdata *needle;
  संरचना curltime now;
  संरचना ssl_filter_config ssl_config;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  संरचना ssl_filter_config proxy_ssl_config;
#यदि_अन्तः
  BIT(may_multiplex);
  BIT(want_ntlm_http);
  BIT(want_proxy_ntlm_http);
  BIT(want_nego_http);
  BIT(want_proxy_nego_http);
  BIT(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  BIT(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  BIT(wait_pipe);
  BIT(आलोकितम्_pending_conn);
  BIT(आलोकितम्_single_use_conn);
  BIT(आलोकितम्_multiplex_conn);
};

स्थिर bool url_मेलनम्_connect_config(संरचना connectdata *conn,
                                     संरचना url_conn_मेलनम् *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  यदि(conn->द्वयङ्काः.connect_only || conn->द्वयङ्काः.close || conn->द्वयङ्काः.no_reuse)
    प्रत्यागम FALSE;

  /* ip_version must match */
  यदि(m->दत्त->समुच्चयः.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->दत्त->समुच्चयः.ipver != conn->ip_संस्करणम्)
    प्रत्यागम FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  यदि(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    प्रत्यागम FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  यदि((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      वाक्यं_तुलय(conn->interface_opts, m->needle->interface_opts)))
    प्रत्यागम FALSE;

  यदि((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       वाक्यं_तुलय(conn->interface_opts, m->needle->interface_opts)))))
    प्रत्यागम FALSE;

  यदि(!m->needle->द्वारा_peer != !conn->द्वारा_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    प्रत्यागम FALSE;

  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_fully_connected(संरचना connectdata *conn,
                                      संरचना url_conn_मेलनम् *m)
{
  यदि(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->द्वयङ्काः.upgrade_अन्तर्_प्रगतिः) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    यदि(m->may_multiplex) {
      m->आलोकितम्_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->दत्त, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_multi(संरचना connectdata *conn,
                            संरचना url_conn_मेलनम् *m)
{
  यदि(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    यदि(conn->attached_multi != m->दत्त->multi)
      प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_multiplex_needs(संरचना connectdata *conn,
                                      संरचना url_conn_मेलनम् *m)
{
  यदि(CONN_INUSE(conn)) {
    यदि(!conn->द्वयङ्काः.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->आलोकितम्_single_use_conn = TRUE;
      प्रत्यागम FALSE;
    }
    m->आलोकितम्_multiplex_conn = TRUE;
    यदि(!m->may_multiplex || !url_मेलनम्_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_multiplex_limits(संरचना connectdata *conn,
                                       संरचना url_conn_मेलनम् *m)
{
  यदि(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->द्वयङ्काः.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    यदि(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->दत्त->multi)) {
      infof(m->दत्त, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      प्रत्यागम FALSE;
    }
    यदि(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->दत्त, conn, FIRSTSOCKET)) {
      infof(m->दत्त, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      प्रत्यागम FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->दत्त, "Multiplexed connection found");
  }
  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_ssl_use(संरचना connectdata *conn,
                              संरचना url_conn_मेलनम् *m)
{
  यदि(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    यदि(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    यदि(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      प्रत्यागम FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    यदि(!Curl_ssl_conn_config_मेलनम्(m->दत्त, &m->ssl_config, conn, FALSE))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    प्रत्यागम FALSE;
  प्रत्यागम TRUE;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
स्थिर bool url_मेलनम्_proxy_use(संरचना connectdata *conn,
                                संरचना url_conn_मेलनम् *m)
{
  यदि(m->needle->द्वयङ्काः.origin_is_proxy != conn->द्वयङ्काः.origin_is_proxy)
    प्रत्यागम FALSE;

  यदि(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    प्रत्यागम FALSE;

  यदि(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    प्रत्यागम FALSE;

  यदि(m->दत्त->समुच्चयः.socks5_auth_only && !conn->द्वयङ्काः.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->दत्त,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    प्रत्यागम FALSE;
  }

  यदि(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    यदि(!Curl_ssl_conn_config_मेलनम्(m->दत्त, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->दत्त,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      प्रत्यागम FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  प्रत्यागम TRUE;
}
#अन्यथा
#परिभाषय url_मेलनम्_proxy_use(c, m) ((शून्य)(c), (शून्य)(m), TRUE)
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
स्थिर bool url_मेलनम्_http_multiplex(संरचना connectdata *conn,
                                     संरचना url_conn_मेलनम् *m,
                                     bool *pwait_pipe)
{
  यदि(m->may_multiplex &&
     (m->दत्त->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_आलोकितम्) {
    यदि(m->दत्त->समुच्चयः.pipewait) {
      infof(m->दत्त, "Server upgrade does not support multiplex yet, wait");
      m->प्राप्तम् = रिक्त;
      *pwait_pipe = TRUE;
      प्रत्यागम TRUE; /* stop searching, we want to wait */
    }
    infof(m->दत्त, "Server upgrade cannot be used");
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_http_संस्करणम्(संरचना connectdata *conn,
                                   संरचना url_conn_मेलनम् *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  यदि((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    विकल्प(Curl_conn_http_संस्करणम्(m->दत्त, conn)) {
    पक्ष 30:
      यदि(!(m->दत्त->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->दत्त, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        प्रत्यागम FALSE;
      }
      भङ्ग;
    पक्ष 20:
      यदि(!(m->दत्त->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->दत्त, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        प्रत्यागम FALSE;
      }
      भङ्ग;
    स्वभाव:
      यदि(!(m->दत्त->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->दत्त, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        प्रत्यागम FALSE;
      }
      भङ्ग;
    }
  }
  प्रत्यागम TRUE;
}
#अन्यथा
#परिभाषय url_मेलनम्_http_multiplex(c, m, w) ((शून्य)(c), (शून्य)(m), TRUE)
#परिभाषय url_मेलनम्_http_संस्करणम्(c, m)      ((शून्य)(c), (शून्य)(m), TRUE)
#यदि_अन्तः

स्थिर bool url_मेलनम्_proto_config(संरचना connectdata *conn,
                                   संरचना url_conn_मेलनम् *m)
{
  यदि(!url_मेलनम्_http_संस्करणम्(conn, m))
    प्रत्यागम FALSE;

#यदि_परिभाषितम् USE_SSH
  यदि(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    यदि(!ssh_config_matches(m->needle, conn))
      प्रत्यागम FALSE;
  }
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  अन्यथा यदि(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    यदि(!Curl_ftp_conns_मेलनम्(m->needle, conn))
      प्रत्यागम FALSE;
  }
#यदि_अन्तः
  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_auth(संरचना connectdata *conn,
                           संरचना url_conn_मेलनम् *m)
{
  यदि(!Curl_creds_same(m->needle->creds, conn->creds)) {
    यदि(m->needle->creds)
      प्रत्यागम FALSE;
    यदि(!Curl_creds_same(m->दत्त->state.creds, conn->creds))
      प्रत्यागम FALSE;
  }
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  यदि(m->needle->gssapi_delegation != conn->gssapi_delegation)
    प्रत्यागम FALSE;
#यदि_अन्तः

  प्रत्यागम TRUE;
}

स्थिर bool url_मेलनम्_destination(संरचना connectdata *conn,
                                  संरचना url_conn_मेलनम् *m)
{
  /* Different connect-to peers never match */
  यदि(!Curl_peer_same_destination(m->needle->द्वारा_peer, conn->द्वारा_peer))
    प्रत्यागम FALSE;

  यदि(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    प्रत्यागम FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  प्रत्यागम Curl_peer_same_destination(m->needle->origin, conn->origin);
}

स्थिर bool url_मेलनम्_ssl_config(संरचना connectdata *conn,
                                 संरचना url_conn_मेलनम् *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  यदि(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_मेलनम्(m->दत्त, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->दत्त, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

#यदि_परिभाषितम् USE_NTLM
स्थिर bool url_मेलनम्_auth_ntlm(संरचना connectdata *conn,
                                संरचना url_conn_मेलनम् *m)
{
  यदि(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    यदि(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->दत्त->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->दत्त->state.origin))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    यदि(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->दत्त->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->दत्त->state.origin)))
      प्रत्यागम FALSE;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  यदि(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    यदि(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(m->want_proxy_ntlm_http) {
    यदि(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      प्रत्यागम FALSE;
  }
#यदि_अन्तः
  प्रत्यागम TRUE;
}
#अन्यथा
#परिभाषय url_मेलनम्_auth_ntlm(c, m) ((शून्य)(c), (शून्य)(m), TRUE)
#यदि_अन्तः

#यदि_परिभाषितम् USE_SPNEGO
स्थिर bool url_मेलनम्_auth_nego(संरचना connectdata *conn,
                                संरचना url_conn_मेलनम् *m)
{
  यदि(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    यदि(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->दत्त->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->दत्त->state.origin))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    यदि(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->दत्त->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->दत्त->state.origin)))
      प्रत्यागम FALSE;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  यदि(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    यदि(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      प्रत्यागम FALSE;
  }
  अन्यथा यदि(m->want_proxy_nego_http) {
    यदि(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      प्रत्यागम FALSE;
  }
#यदि_अन्तः
  प्रत्यागम TRUE;
}
#अन्यथा
#परिभाषय url_मेलनम्_auth_nego(c, m) ((शून्य)(c), (शून्य)(m), TRUE)
#यदि_अन्तः

स्थिर cpool_मेलनम्_परिणाम url_मेलनम्_conn(संरचना connectdata *conn,
                                         शून्य *userdata)
{
  संरचना url_conn_मेलनम् *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  यदि(!url_मेलनम्_connect_config(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  /* match for destination and protocol? */
  यदि(!url_मेलनम्_destination(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_fully_connected(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_multiplex_needs(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_ssl_use(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_proxy_use(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;
  यदि(!url_मेलनम्_ssl_config(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_http_multiplex(conn, m, &wait_pipe))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_auth(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_proto_config(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_auth_ntlm(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_auth_nego(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  यदि(!url_मेलनम्_multiplex_limits(conn, m))
    प्रत्यागम CPOOL_मेलनम्_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  यदि(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->प्राप्तम्);
    m->wait_pipe = TRUE;
    प्रत्यागम CPOOL_मेलनम्_प्राप्तम्;
  }

  यदि(m->दत्त->समुच्चयः.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->दत्त, conn, &m->now);
    यदि(age_ms > m->दत्त->समुच्चयः.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      प्रत्यागम CPOOL_मेलनम्_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  यदि(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->दत्त, &m->now)) {
    infof(m->दत्त, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    प्रत्यागम CPOOL_मेलनम्_CLOSE;
  }

  /* conn matches our needs. */
  m->प्राप्तम् = conn;
  प्रत्यागम CPOOL_मेलनम्_प्राप्तम्;
}

स्थिर bool url_मेलनम्_परिणाम(शून्य *userdata)
{
  संरचना url_conn_मेलनम् *मेलनम् = userdata;
  यदि(मेलनम्->प्राप्तम्) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(मेलनम्->दत्त, मेलनम्->प्राप्तम्, TRUE);
    प्रत्यागम TRUE;
  }
  अन्यथा यदि(मेलनम्->आलोकितम्_single_use_conn && !मेलनम्->आलोकितम्_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    मेलनम्->wait_pipe = FALSE;
  }
  अन्यथा यदि(मेलनम्->आलोकितम्_pending_conn && मेलनम्->दत्त->समुच्चयः.pipewait) {
    infof(मेलनम्->दत्त,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    मेलनम्->wait_pipe = TRUE;
  }
  प्रत्यागम FALSE;
}

/*
 * Given a transfer and a prototype connection (needle),
 * find and attach an existing connection that matches.
 *
 * Return TRUE if an existing connection was attached.
 * `waitpipe` is TRUE if no existing connection matched, but there
 * might be suitable one in the near future (common cause: multiplexing
 * capability has not been determined yet, e.g. ALPN handshake).
 */
स्थिर bool url_attach_existing(संरचना Curl_easy *दत्त,
                                संरचना connectdata *needle,
                                संरचना url_conn_मेलनम् *m,
                                ध्रुव संरचना curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  प्रत्यागम Curl_cpool_find(दत्त, needle->destination, TRUE, pnow,
                         url_मेलनम्_conn, url_मेलनम्_परिणाम, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
स्थिर संरचना connectdata *allocate_conn(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = curlx_calloc(1, परिमाण(संरचना connectdata));
  यदि(!conn)
    प्रत्यागम रिक्त;

  /* and we setup a few fields in case we end up actually using this struct */
  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->recv_idx = 0; /* default for receiving transfer data */
  conn->send_idx = 0; /* default for sending transfer data */
  conn->connection_id = -1;    /* no ID */
  conn->cpid = UINT32_MAX;  /* not in connection pool yet */
  conn->attached_xfers = 0;
  conn->shutdown.start_ms[FIRSTSOCKET] =
    conn->shutdown.start_ms[SECONDARYSOCKET] = -1;

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  conn->द्वयङ्काः.ftp_use_epsv = दत्त->समुच्चयः.ftp_use_epsv;
  conn->द्वयङ्काः.ftp_use_eprt = दत्त->समुच्चयः.ftp_use_eprt;
#यदि_अन्तः
  conn->ip_संस्करणम् = दत्त->समुच्चयः.ipver;
  conn->द्वयङ्काः.connect_only = (bool)दत्त->समुच्चयः.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  यदि(CURL_EASY_STR(दत्त, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(दत्त, STRING_INTERFACE_OPT));
    यदि(!conn->interface_opts)
      गच्छ error;
  }
#यदि_अपरिभाषितम् CURL_DISABLE_BINDLOCAL
  conn->localportrange = दत्त->समुच्चयः.localportrange;
  conn->localport = दत्त->समुच्चयः.localport;
#यदि_अन्तः

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = दत्त->समुच्चयः.fclosesocket;
  conn->closesocket_client = दत्त->समुच्चयः.closesocket_client;
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = दत्त->समुच्चयः.gssapi_delegation;
#यदि_अन्तः
  प्रत्यागम conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  प्रत्यागम रिक्त;
}

स्थिर CURLcode url_समुच्चयः_conn_scheme(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ध्रुव संरचना Curl_scheme *scheme = conn->origin->scheme;
  यदि(scheme->सततम् &&
     (दत्त->समुच्चयः.allowed_protocols & scheme->protocol) &&
     (!दत्त->state.this_is_a_follow ||
       (दत्त->समुच्चयः.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    प्रत्यागम CURLE_OK;
  }
  यदि(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(दत्त, "Protocol \"%s\" is not for transfers", scheme->name);
  अन्यथा
    failf(दत्त, "Protocol \"%s\" is disabled%s", scheme->name,
          दत्त->state.this_is_a_follow ? " (in redirect)" : "");
  प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_गन्तव्यम्_curlcode(CURLUcode uc)
{
  विकल्प(uc) {
  स्वभाव:
    प्रत्यागम CURLE_URL_MALFORMAT;
  पक्ष CURLUE_UNSUPPORTED_SCHEME:
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  पक्ष CURLUE_बहिर्गमनम्_सम्बन्धि_स्मृतिः:
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  पक्ष CURLUE_USER_NOT_ALLOWED:
    प्रत्यागम CURLE_LOGIN_DENIED;
  }
}

#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
स्थिर CURLcode hsts_upgrade(संरचना Curl_easy *दत्त,
                             CURLU *uh,
                             uint16_t पत्तनम्_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  यदि(दत्त->hsts && (दत्त->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(दत्त->hsts, दत्त->state.origin)) {
    वर्ण *url;
    CURLUcode uc;
    CURLcode परिणाम;

    uc = curl_url_समुच्चयः(uh, CURLUPART_SCHEME, "https", 0);
    यदि(uc)
      प्रत्यागम Curl_uc_गन्तव्यम्_curlcode(uc);
    Curl_bufref_free(&दत्त->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    यदि(uc)
      प्रत्यागम Curl_uc_गन्तव्यम्_curlcode(uc);
    Curl_bufref_समुच्चयः(&दत्त->state.url, url, 0, curl_free);

    परिणाम = Curl_peer_from_url(uh, दत्त, पत्तनम्_override, scope_id,
                                &दत्त->state.origin);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    infof(दत्त, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  प्रत्यागम CURLE_OK;
}
#अन्यथा
#परिभाषय hsts_upgrade(x, y, z, a) CURLE_OK
#यदि_अन्तः

स्थिर bool str_has_ctrl(ध्रुव वर्ण *निवेशः)
{
  यदि(निवेशः) {
    ध्रुव अचिह्नित वर्ण *str = (ध्रुव अचिह्नित वर्ण *)निवेशः;
    यावत्(*str) {
      यदि(*str < 0x20)
        प्रत्यागम TRUE;
      str++;
    }
  }
  प्रत्यागम FALSE;
}

#यदि_अपरिभाषितम् CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
स्थिर CURLcode url_समुच्चयः_दत्त_creds_netrc(संरचना Curl_easy *दत्त,
                                         संरचना Curl_creds **pcreds)
{
  संरचना Curl_creds *ncreds_बहिर्गमनम् = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(दत्त->समुच्चयः.use_netrc) { /* not CURL_NETRC_IGNORED */
    संरचना Curl_creds *ncreds_अन्तर् = रिक्त;
    bool अवलोकनम्_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    यदि(*pcreds) {
      विकल्प((*pcreds)->source) {
      पक्ष CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        अवलोकनम्_netrc = FALSE;
        भङ्ग;
      पक्ष CREDS_URL: /* only apply when netrc is not required */
        यदि(दत्त->समुच्चयः.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_अन्तर् = *pcreds;
        }
        अन्यथा यदि(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_अन्तर् = *pcreds;
        }
        अन्यथा
          अवलोकनम्_netrc = FALSE;
        भङ्ग;
      स्वभाव: /* ignore credentials from other sources */
        भङ्ग;
      }
    }

    यदि(!अवलोकनम्_netrc)
      गच्छ बहिर्गमनम्;

    ret = Curl_netrc_अवलोकनम्(दत्त, &दत्त->state.netrc,
                          दत्त->state.origin->hostname,
                          Curl_creds_user(ncreds_अन्तर्),
                          CURL_EASY_STR(दत्त, STRING_NETRC_FILE),
                          &ncreds_बहिर्गमनम्);
    DEBUGASSERT(!ret || !ncreds_बहिर्गमनम्);
    यदि(ret == NETRC_बहिर्गमनम्_सम्बन्धि_स्मृतिः) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(ret && ((ret == NETRC_NO_मेलनम्) ||
                    (दत्त->समुच्चयः.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(दत्त, "Could not find host %s in the %s file; using defaults",
            दत्त->state.origin->hostname,
            (CURL_EASY_STR(दत्त, STRING_NETRC_FILE) ?
             CURL_EASY_STR(दत्त, STRING_NETRC_FILE) : ".netrc"));
    }
    अन्यथा यदि(ret) {
      ध्रुव वर्ण *m = Curl_netrc_strerror(ret);
      failf(दत्त, ".netrc error: %s", m);
      परिणाम = CURLE_READ_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(ncreds_बहिर्गमनम्) {
      यदि(!(दत्त->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_बहिर्गमनम्->user) ||
          str_has_ctrl(ncreds_बहिर्गमनम्->passwd))) {
        failf(दत्त, "control code detected in .netrc credentials");
        परिणाम = CURLE_READ_ERROR;
        गच्छ बहिर्गमनम्;
      }
      CURL_TRC_M(दत्त, "netrc: using credentials for %s as %s",
                 दत्त->state.origin->hostname, ncreds_बहिर्गमनम्->user);
      परिणाम = Curl_creds_merge(ncreds_बहिर्गमनम्->user, ncreds_बहिर्गमनम्->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      /* for updated strings, we update them in the URL */
      uc = curl_url_समुच्चयः(दत्त->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      यदि(!uc)
        uc = curl_url_समुच्चयः(दत्त->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      यदि(uc)
        परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    }
    अन्यथा
      DEBUGASSERT(0);
  }

#यदि_परिभाषितम् CURLVERBOSE
  Curl_creds_trace(दत्त, दत्त->state.creds, "transfer credentials");
#यदि_अन्तः

बहिर्गमनम्:
  Curl_creds_unlink(&ncreds_बहिर्गमनम्);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* CURL_DISABLE_NETRC */

स्थिर CURLcode url_समुच्चयः_दत्त_creds(संरचना Curl_easy *दत्त, CURLU *uh)
{
  संरचना Curl_creds *newcreds = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि((CURL_EASY_STR(दत्त, STRING_USERNAME) ||
      CURL_EASY_STR(दत्त, STRING_PASSWORD) ||
      CURL_EASY_STR(दत्त, STRING_BEARER) ||
      CURL_EASY_STR(दत्त, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(दत्त, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_गन्तव्यम्_origin(दत्त, दत्त->state.origin)) {
    परिणाम = Curl_creds_create(CURL_EASY_STR(दत्त, STRING_USERNAME),
                               CURL_EASY_STR(दत्त, STRING_PASSWORD),
                               CURL_EASY_STR(दत्त, STRING_BEARER),
                               CURL_EASY_STR(दत्त, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(दत्त, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    यदि(newcreds &&
       !(दत्त->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(दत्त, "control code detected in credentials");
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  यदि(!newcreds || !Curl_creds_has_user(newcreds)) {
    वर्ण *user = रिक्त;
    वर्ण *passwd = रिक्त;
    वर्ण *udecoded = रिक्त;
    वर्ण *pdecoded = रिक्त;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    यदि(uc && (uc != CURLUE_NO_USER))
      परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    यदि(!परिणाम) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      यदि(uc && (uc != CURLUE_NO_PASSWORD))
        परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    }
    यदि(!परिणाम && user) {
      परिणाम = Curl_urldecode(user, 0, &udecoded, रिक्त,
                              (दत्त->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    यदि(!परिणाम && passwd) {
      परिणाम = Curl_urldecode(passwd, 0, &pdecoded, रिक्त,
                              (दत्त->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    यदि(!परिणाम)
      परिणाम = Curl_creds_merge(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    यदि(परिणाम) {
      failf(दत्त, "error extracting credentials from URL");
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_अपरिभाषितम् CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  परिणाम = url_समुच्चयः_दत्त_creds_netrc(दत्त, &newcreds);
#यदि_अन्तः /* CURL_DISABLE_NETRC */

बहिर्गमनम्:
  यदि(!परिणाम && !Curl_creds_equal(दत्त->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&दत्त->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode url_समुच्चयः_conn_origin_etc(संरचना Curl_easy *दत्त,
                                        संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;

  Curl_peer_link(&conn->origin, दत्त->state.origin);

  /* set the connection scheme */
  परिणाम = url_समुच्चयः_conn_scheme(दत्त, conn);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* set the connection options */
  यदि(CURL_EASY_STR(दत्त, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(दत्त, STRING_OPTIONS));
    यदि(!conn->options) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा यदि(दत्त->state.ऊर्ध्वम्.options) {
    conn->options = curlx_strdup(दत्त->state.ऊर्ध्वम्.options);
    यदि(!conn->options) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
स्थिर CURLcode सज्जता_range(संरचना Curl_easy *दत्त)
{
  संरचना UrlState *s = &दत्त->state;
  s->resume_from = दत्त->समुच्चयः.समुच्चयः_resume_from;
  यदि(s->resume_from || CURL_EASY_STR(दत्त, STRING_समुच्चयः_RANGE)) {
    यदि(s->rangestringalloc)
      curlx_free(s->range);

    यदि(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    अन्यथा
      s->range = curlx_strdup(CURL_EASY_STR(दत्त, STRING_समुच्चयः_RANGE));

    यदि(!s->range)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  अन्यथा
    s->use_range = FALSE; /* disable range download */

  प्रत्यागम CURLE_OK;
}

/*
 * setup_connection_internals() -
 *
 * Setup connection internals specific to the requested protocol in the
 * Curl_easy. This is inited and setup before the connection is made but
 * is about the particular protocol that is to be used.
 *
 * This MUST get called after proxy magic has been figured out.
 */
स्थिर CURLcode सज्जता_connection_internals(संरचना Curl_easy *दत्त,
                                           संरचना connectdata *conn)
{
  संरचना Curl_peer *peer = रिक्त;

  यदि(conn->scheme->सततम्->सज्जता_connection) {
    CURLcode परिणाम = conn->scheme->सततम्->सज्जता_connection(दत्त, conn);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  यदि(!peer)
    प्रत्यागम CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  यदि(peer->ipv6)
    यदि(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->पत्तनम्);
    अन्यथा
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->पत्तनम्);
  अन्यथा
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->पत्तनम्);
  यदि(!conn->destination)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  Curl_strntolower(conn->destination, conn->destination,
                   वाक्यदैर्घ्यम्(conn->destination));

#यदि_परिभाषितम् USE_IPV6
  यदि(दत्त->समुच्चयः.scope_id)
    conn->scope_id = दत्त->समुच्चयः.scope_id;
  अन्यथा {
    peer = Curl_conn_get_first_peer(conn, FIRSTSOCKET);
    यदि(!peer)
      प्रत्यागम CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#यदि_अन्तः

  प्रत्यागम CURLE_OK;
}

/*
 * Curl_parse_login_details()
 *
 * This is used to parse a login string for username, password and options in
 * the following formats:
 *
 *   user
 *   user:password
 *   user:password;options
 *   user;options
 *   user;options:password
 *   :password
 *   :password;options
 *   ;options
 *   ;options:password
 *
 * Parameters:
 *
 * login    [in]     - login string.
 * len      [in]     - length of the login string.
 * userp    [in/out] - address where a pointer to newly allocated memory
 *                     holding the user will be stored upon completion.
 * passwdp  [in/out] - address where a pointer to newly allocated memory
 *                     holding the password will be stored upon completion.
 * optionsp [in/out] - OPTIONAL address where a pointer to newly allocated
 *                     memory holding the options will be stored upon
 *                     completion.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_parse_login_details(ध्रुव वर्ण *login, ध्रुव size_t len,
                                  वर्ण **userp, वर्ण **passwdp,
                                  वर्ण **optionsp)
{
  वर्ण *ubuf = रिक्त;
  वर्ण *pbuf = रिक्त;
  ध्रुव वर्ण *psep = रिक्त;
  ध्रुव वर्ण *osep = रिक्त;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  यदि(optionsp)
    osep = memchr(login, ';', len);

  /* Calculate the portion lengths */
  ulen = (psep ?
          (size_t)(osep && psep > osep ? osep - login : psep - login) :
          (osep ? (size_t)(osep - login) : len));
  plen = (psep ?
          (osep && osep > psep ? (size_t)(osep - psep) :
           (size_t)(login + len - psep)) - 1 : 0);
  olen = (osep ?
          (psep && psep > osep ? (size_t)(psep - osep) :
           (size_t)(login + len - osep)) - 1 : 0);

  /* Clone the user portion buffer, which can be zero length */
  ubuf = curlx_memdup0(login, ulen);
  यदि(!ubuf)
    गच्छ error;

  /* Clone the password portion buffer */
  यदि(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    यदि(!pbuf)
      गच्छ error;
  }

  /* Allocate the options portion buffer */
  यदि(optionsp) {
    वर्ण *obuf = रिक्त;
    यदि(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      यदि(!obuf)
        गच्छ error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  प्रत्यागम CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}

/*
 * Set the login details so they are available in the connection
 */
स्थिर CURLcode url_समुच्चयः_conn_login(संरचना Curl_easy *दत्त,
                                   संरचना connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  यदि((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, दत्त->state.origin);
    यदि(दत्त->state.creds)
      Curl_creds_link(&conn->creds, दत्त->state.creds);
    अन्यथा
      प्रत्यागम Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               रिक्त, रिक्त, रिक्त, CREDS_NONE, &conn->creds);
  }
  अन्यथा यदि(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, दत्त->state.origin);
    Curl_creds_link(&conn->creds, दत्त->state.creds);
  }

  प्रत्यागम CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
स्थिर CURLcode parse_connect_गन्तव्यम्_string(संरचना Curl_easy *दत्त,
                                        ध्रुव संरचना Curl_peer *dest,
                                        ध्रुव वर्ण *conn_गन्तव्यम्_पङ्क्तिः,
                                        संरचना Curl_peer **pvia_dest)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ptr = conn_गन्तव्यम्_पङ्क्तिः;
  bool host_मेलनम् = FALSE;
  bool पत्तनम्_मेलनम् = FALSE;

  *pvia_dest = रिक्त;

  यदि(*ptr == ':') {
    /* an empty hostname always matches */
    host_मेलनम् = TRUE;
    ptr++;
  }
  अन्यथा {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = वाक्यदैर्घ्यम्(dest->hostname);
    host_मेलनम् = curl_strnequal(ptr, dest->hostname, hlen);
    यदि(!host_मेलनम् && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = वाक्यदैर्घ्यम्(dest->user_hostname);
      host_मेलनम् = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_मेलनम् = host_मेलनम् && ptr[hlen] == ':';
    यदि(host_मेलनम्)
      ptr += hlen + 1;
  }

  यदि(host_मेलनम्) {
    यदि(*ptr == ':') {
      /* an empty port always matches */
      पत्तनम्_मेलनम् = TRUE;
      ptr++;
    }
    अन्यथा {
      /* check whether the URL's port matches */
      ध्रुव वर्ण *ptr_अग्र = strchr(ptr, ':');
      यदि(ptr_अग्र) {
        curl_off_t पत्तनम्_गन्तव्यम्_मेलनम्;
        यदि(!curlx_str_सङ्ख्या(&ptr, &पत्तनम्_गन्तव्यम्_मेलनम्, 0xffff) &&
           ((uint16_t)पत्तनम्_गन्तव्यम्_मेलनम् == dest->पत्तनम्)) {
          पत्तनम्_मेलनम् = TRUE;
        }
        ptr = ptr_अग्र + 1;
      }
    }
  }

  यदि(host_मेलनम् && पत्तनम्_मेलनम् && ptr && *ptr)
    परिणाम = Curl_peer_from_connect_गन्तव्यम्(दत्त, dest, ptr, pvia_dest);

  प्रत्यागम परिणाम;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
स्थिर CURLcode url_समुच्चयः_conn_peer(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना Curl_peer *origin = conn->origin;
  संरचना Curl_peer *द्वारा_peer = रिक्त;
  संरचना curl_slist *conn_गन्तव्यम्_प्रविष्टिः = दत्त->समुच्चयः.connect_गन्तव्यम्;

  DEBUGASSERT(!conn->द्वारा_peer);
  Curl_peer_unlink(&conn->द्वारा_peer);

  यावत्(conn_गन्तव्यम्_प्रविष्टिः && !द्वारा_peer) {
    परिणाम = parse_connect_गन्तव्यम्_string(दत्त, origin, conn_गन्तव्यम्_प्रविष्टिः->दत्त,
                                     &द्वारा_peer);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    conn_गन्तव्यम्_प्रविष्टिः = conn_गन्तव्यम्_प्रविष्टिः->अग्र;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  यदि(दत्त->asi && !द्वारा_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#यदि_परिभाषितम् DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#अन्यथा
      0
#यदि_अन्तः
       )) {
    /* no connect_to match, try alt-svc! */
    गणना alpnid srcalpnid = ALPN_none;
    bool लब्धम् = FALSE;
    संरचना altsvc *as = रिक्त;
    पूर्णाङ्क allowed_alpns = ALPN_none;
    संरचना http_negotiation *neg = &दत्त->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(दत्त, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#यदि_परिभाषितम् USE_HTTP3
    यदि(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#यदि_अन्तः
#यदि_परिभाषितम् USE_HTTP2
    यदि(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#यदि_अन्तः
    यदि(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (पूर्णाङ्क)दत्त->asi->flags;

    DEBUGF(infof(दत्त, "check Alt-Svc for host '%s'", origin->hostname));
#यदि_परिभाषितम् USE_HTTP3
    यदि(!लब्धम् && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      लब्धम् = Curl_altsvc_lookup(दत्त->asi,
                               origin, ALPN_h3, /* from */
                               &as /*गन्तव्यम्*/,
                               allowed_alpns, &same_dest);
    }
#यदि_अन्तः
#यदि_परिभाषितम् USE_HTTP2
    यदि(!लब्धम् && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_प्राक्_knowledge) {
      srcalpnid = ALPN_h2;
      लब्धम् = Curl_altsvc_lookup(दत्त->asi,
                               origin, ALPN_h2, /* from */
                               &as /*गन्तव्यम्*/,
                               allowed_alpns, &same_dest);
    }
#यदि_अन्तः
    यदि(!लब्धम् && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      लब्धम् = Curl_altsvc_lookup(दत्त->asi,
                               origin, ALPN_h1, /* from */
                               &as /*गन्तव्यम्*/,
                               allowed_alpns, &same_dest);
    }

    यदि(लब्धम् && same_dest) {
      /* same destination, but more HTTPS version options */
      विकल्प(as->dst.alpnid) {
      पक्ष ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        भङ्ग;
      पक्ष ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        भङ्ग;
      पक्ष ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        भङ्ग;
      स्वभाव: /* should not be possible */
        भङ्ग;
      }
    }
    अन्यथा यदि(लब्धम्) {
      परिणाम = Curl_peer_create(दत्त, conn->origin->scheme,
                                as->dst.host, as->dst.पत्तनम्,
                                &द्वारा_peer);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      infof(दत्त, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->पत्तनम्,
            Curl_alpnid2str(as->dst.alpnid),
            द्वारा_peer->hostname, द्वारा_peer->पत्तनम्);
      conn->द्वयङ्काः.altused = TRUE;
      यदि(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        विकल्प(as->dst.alpnid) {
        पक्ष ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          भङ्ग;
        पक्ष ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          भङ्ग;
        पक्ष ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          भङ्ग;
        स्वभाव: /* should not be possible */
          भङ्ग;
        }
      }
    }
  }
#यदि_अन्तः

  यदि(द्वारा_peer)
    conn->द्वारा_peer = द्वारा_peer;

  प्रत्यागम परिणाम;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
स्थिर शून्य url_conn_reuse_adjust(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *needle)
{
  संरचना connectdata *conn = दत्त->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  यदि(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#यदि_अन्तः

  /* Finding a connection for reuse in the cpool matches, among other
   * things on the "remote-relevant" hostname. This is not necessarily
   * the authority of the URL, e.g. conn->origin. For example:
   * - we use a proxy (not tunneling). we want to send all requests
   *   that use the same proxy on this connection.
   * - we have a "connect-to" setting that may redirect the hostname of
   *   a new request to the same remote endpoint of an existing conn.
   *   We want to reuse an existing conn to the remote endpoint.
   * Since connection reuse does not match on conn->origin necessarily, we
   * switch conn to needle's host settings.
   */
  Curl_peer_link(&conn->origin, needle->origin);
  Curl_peer_link(&conn->द्वारा_peer, needle->द्वारा_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->द्वारा_peer2, needle->द्वारा_peer2);
}

स्थिर शून्य conn_meta_freeentry(शून्य *p)
{
  (शून्य)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

स्थिर CURLcode url_create_needle(संरचना Curl_easy *दत्त,
                                  संरचना connectdata **pneedle)
{
  संरचना connectdata *needle = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  bool जालम्_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(दत्त);
  यदि(!needle) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_अष्टकानि,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  परिणाम = url_समुच्चयः_conn_origin_etc(दत्त, needle);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  DEBUGASSERT(needle->origin);
  जालम्_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#यदि_परिभाषितम् USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  यदि(जालम्_scheme && CURL_EASY_STR(दत्त, STRING_UNIX_SOCKET_मार्गः)) {
    परिणाम = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(दत्त, STRING_UNIX_SOCKET_मार्गः),
      (bool)दत्त->समुच्चयः.abstract_unix_socket, &needle->द्वारा_peer);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः /* USE_UNIX_SOCKETS */

  यदि(जालम्_scheme && !needle->द्वारा_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    परिणाम = url_समुच्चयः_conn_peer(दत्त, needle);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  यदि(Curl_peer_equal(needle->origin, needle->द्वारा_peer)) {
    Curl_peer_unlink(&needle->द्वारा_peer);
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  यदि(जालम्_scheme &&
     (!needle->द्वारा_peer || !needle->द्वारा_peer->unix_socket)) {
    परिणाम = Curl_proxy_init_conn(दत्त, needle);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः /* CURL_DISABLE_PROXY */

  परिणाम = url_समुच्चयः_conn_login(दत्त, needle); /* default credentials */
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  परिणाम = सज्जता_connection_internals(दत्त, needle);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    यदि(दत्त->समुच्चयः.ssl_enable_alpn)
      needle->द्वयङ्काः.tls_enable_alpn = TRUE;
  }

  यदि(जालम्_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->द्वयङ्काः.tcp_fastopen = दत्त->समुच्चयः.tcp_fastopen;
#यदि_परिभाषितम् USE_UNIX_SOCKETS
    यदि(Curl_conn_get_first_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#यदि_अन्तः
  }

बहिर्गमनम्:
  यदि(!परिणाम) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  अन्यथा {
    *pneedle = रिक्त;
    यदि(needle)
      Curl_conn_free(दत्त, needle);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode url_समुच्चयः_दत्त_origin_and_creds(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_समुच्चयः_uh = (दत्त->समुच्चयः.uh && !दत्त->state.this_is_a_follow);
  uint16_t पत्तनम्_override = दत्त->state.allow_पत्तनम् ? दत्त->समुच्चयः.use_पत्तनम् : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  यदि(!Curl_bufref_ptr(&दत्त->state.url)) {
    परिणाम = CURLE_URL_MALFORMAT;
    गच्छ बहिर्गमनम्;
  }

  ऊर्ध्वम्_free(दत्त); /* cleanup previous leftovers first */

  /* parse the URL */
  यदि(use_समुच्चयः_uh)
    uh = दत्त->state.uh = curl_url_dup(दत्त->समुच्चयः.uh);
  अन्यथा
    uh = दत्त->state.uh = curl_url();
  यदि(!uh) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  यदि(!use_समुच्चयः_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    यदि(curl_strequal("file", CURL_EASY_STR(दत्त, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&दत्त->state.url), रिक्त, 0, TRUE))
      prepend_sheme = TRUE;
    अन्यथा {
      uc = curl_url_समुच्चयः(uh, CURLUPART_URL, Curl_bufref_ptr(&दत्त->state.url),
                        (अचिह्नित पूर्णाङ्क)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (दत्त->समुच्चयः.disallow_username_अन्तर्_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (दत्त->समुच्चयः.मार्गः_as_is ? CURLU_मार्गः_AS_IS : 0));
      यदि(uc) {
        failf(दत्त, "URL rejected: %s", curl_url_strerror(uc));
        परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
        गच्छ बहिर्गमनम्;
      }

      यदि(CURL_EASY_STR(दत्त, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        वर्ण *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        यदि(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        यदि(uc && uc != CURLUE_NO_SCHEME) {
          परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
          गच्छ बहिर्गमनम्;
        }
      }
    }

    यदि(prepend_sheme) {
      वर्ण *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(दत्त, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&दत्त->state.url));
      यदि(!url) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      uc = curl_url_समुच्चयः(uh, CURLUPART_URL, url,
                        (अचिह्नित पूर्णाङ्क)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (दत्त->समुच्चयः.disallow_username_अन्तर्_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (दत्त->समुच्चयः.मार्गः_as_is ? CURLU_मार्गः_AS_IS : 0));
      curl_free(url);
      यदि(uc) {
        failf(दत्त, "URL rejected: %s", curl_url_strerror(uc));
        परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
        गच्छ बहिर्गमनम्;
      }
    }
    {
      वर्ण *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_रिक्तानि);
      यदि(uc) {
        परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
        गच्छ बहिर्गमनम्;
      }
      यदि(prepend_sheme)
        infof(दत्त, "Applied default protocol '%s'",
              CURL_EASY_STR(दत्त, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_समुच्चयः(&दत्त->state.url, newurl, 0, curl_free);
    }
  }

#यदि_परिभाषितम् USE_IPV6
  scope_id = दत्त->समुच्चयः.scope_id;
#यदि_अन्तः

  /* `uh` is now as the connection should use it, probably. */
  परिणाम = Curl_peer_from_url(uh, दत्त, पत्तनम्_override, scope_id,
                              &दत्त->state.origin);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  /* The origin might get changed when HSTS applies */
  परिणाम = hsts_upgrade(दत्त, uh, पत्तनम्_override, scope_id);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  यदि(!दत्त->state.initial_origin)
    Curl_peer_link(&दत्त->state.initial_origin, दत्त->state.origin);

  uc = curl_url_get(uh, CURLUPART_मार्गः, &दत्त->state.ऊर्ध्वम्.मार्गः, CURLU_URLENCODE);
  यदि(uc) {
    परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    गच्छ बहिर्गमनम्;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &दत्त->state.ऊर्ध्वम्.query,
                    CURLU_GET_रिक्तानि);
  यदि(uc && (uc != CURLUE_NO_QUERY)) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &दत्त->state.ऊर्ध्वम्.options,
                    CURLU_URLDECODE);
  यदि(uc && (uc != CURLUE_NO_OPTIONS)) {
    परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    गच्छ बहिर्गमनम्;
  }

  परिणाम = url_समुच्चयः_दत्त_creds(दत्त, uh);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर CURLcode url_मेलनम्_init(संरचना Curl_easy *दत्त,
                               संरचना connectdata *needle,
                               संरचना url_conn_मेलनम् *m)
{
  स्मृतिं_पूरय(m, 0, परिमाण(*m));
  m->दत्त = दत्त;
  m->needle = needle;
  m->now = *Curl_pgrs_now(दत्त);
  m->may_multiplex = xfer_may_multiplex(दत्त, needle);

#यदि_परिभाषितम् USE_NTLM
  m->want_ntlm_http =
    (दत्त->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_गन्तव्यम्_host(दत्त);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (दत्त->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#यदि_अन्तः
#यदि_अन्तः

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (दत्त->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_गन्तव्यम्_host(दत्त);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (दत्त->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#यदि_अन्तः
#यदि_अन्तः
  m->require_tls = दत्त->समुच्चयः.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = दत्त->समुच्चयः.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  प्रत्यागम Curl_ssl_filter_config_tmp_init(दत्त, needle->origin,
                                         &m->ssl_config,
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#अन्यथा
                                         रिक्त);
#यदि_अन्तः
}

स्थिर शून्य url_मेलनम्_destroy(संरचना url_conn_मेलनम् *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#यदि_अन्तः
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
स्थिर CURLcode url_find_or_create_conn(संरचना Curl_easy *दत्त,
                                        ध्रुव संरचना curltime *pnow)
{
  संरचना connectdata *needle = रिक्त;
  संरचना url_conn_मेलनम् मेलनम्;
  bool मेलनम्_initialized = FALSE;
  CURLcode परिणाम;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  परिणाम = url_create_needle(दत्त, &needle);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  DEBUGASSERT(needle);

  परिणाम = url_मेलनम्_init(दत्त, needle, &मेलनम्);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  मेलनम्_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#यदि_अपरिभाषितम् CURL_DISABLE_FILE
  यदि(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool समाप्तम्;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->सततम्->connect_it);
    दत्त->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    दत्त->info.conn_protocol = needle->scheme->protocol & CURLPROTO_आवरणम्;
    परिणाम = needle->scheme->सततम्->connect_it(दत्त, &समाप्तम्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    /* Setup a "faked" transfer that will do nothing */
    परिणाम = Curl_cpool_सङ्कलय(दत्त, needle, 0, 0, pnow);
    Curl_attach_connection(दत्त, needle, TRUE);
    needle = रिक्त;
    यदि(!परिणाम) {
      /* Setup whatever necessary for a resumed transfer */
      परिणाम = सज्जता_range(दत्त);
      यदि(!परिणाम) {
        Curl_xfer_सज्जता_nop(दत्त);
        परिणाम = Curl_init_transfer(दत्त, दत्त->conn);
      }
    }

    यदि(परिणाम) {
      DEBUGASSERT(दत्त->conn->scheme->सततम्->समाप्तम्);
      /* we ignore the return code for the protocol-specific DONE */
      (शून्य)दत्त->conn->scheme->सततम्->समाप्तम्(दत्त, परिणाम, FALSE);
    }
    गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  यदि((!दत्त->समुच्चयः.reuse_fresh || दत्त->state.followlocation) &&
     !दत्त->समुच्चयः.connect_only) {
    /* Ok, try to find and attach an existing one */
    यदि(url_attach_existing(दत्त, needle, &मेलनम्, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      संरचना connectdata *conn = दत्त->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->द्वयङ्काः.reuse = TRUE;
      url_conn_reuse_adjust(दत्त, needle);

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
      infof(दत्त, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#अन्यथा
      infof(दत्त, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#यदि_अन्तः
    }
  }

  यदि(!दत्त->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    यदि(मेलनम्.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(दत्त, "Waiting on connection to negotiate possible multiplexing.");
      परिणाम = CURLE_NO_CONNECTION_उपलब्धम्;
      गच्छ बहिर्गमनम्;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    परिणाम = Curl_cpool_सङ्कलय(दत्त, needle,
                            दत्त->multi->max_योग_connections,
                            दत्त->multi->max_host_connections,
                            pnow);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(दत्त, needle, TRUE);
    needle = रिक्त;

    /* Clone the SSL filter configs into the new connection. */
    परिणाम = Curl_ssl_conn_config_clone(&मेलनम्.ssl_config,
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
                                        &मेलनम्.proxy_ssl_config,
#अन्यथा
                                        रिक्त,
#यदि_अन्तः
                                        दत्त->conn);
    यदि(परिणाम) {
      DEBUGF(infof(दत्त, "Error: clone connection SSL config"));
      गच्छ बहिर्गमनम्;
    }

#यदि_परिभाषितम् USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    यदि((दत्त->state.authhost.picked & CURLAUTH_NTLM) &&
       दत्त->state.authhost.समाप्तम्) {
      infof(दत्त, "NTLM picked AND auth done set, clear picked");
      दत्त->state.authhost.picked = CURLAUTH_NONE;
      दत्त->state.authhost.समाप्तम् = FALSE;
    }

    यदि((दत्त->state.authproxy.picked & CURLAUTH_NTLM) &&
       दत्त->state.authproxy.समाप्तम्) {
      infof(दत्त, "NTLM-proxy picked AND auth done set, clear picked");
      दत्त->state.authproxy.picked = CURLAUTH_NONE;
      दत्त->state.authproxy.समाप्तम् = FALSE;
    }
#यदि_अन्तः
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  परिणाम = Curl_init_transfer(दत्त, दत्त->conn);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* Setup whatever necessary for a resumed transfer */
  परिणाम = सज्जता_range(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* persist the scheme and handler the transfer is using */
  दत्त->info.conn_scheme = दत्त->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  दत्त->info.conn_protocol = दत्त->conn->scheme->protocol & CURLPROTO_आवरणम्;
  दत्त->info.प्रयुक्तम्_proxy =
#यदि_परिभाषितम् CURL_DISABLE_PROXY
    0
#अन्यथा
    (दत्त->conn->socks_proxy.peer || दत्त->conn->http_proxy.peer)
#यदि_अन्तः
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  परिणाम = Curl_conn_ev_दत्त_सज्जता(दत्त);

बहिर्गमनम्:
  यदि(मेलनम्_initialized)
    url_मेलनम्_destroy(&मेलनम्);
  यदि(needle)
    Curl_conn_free(दत्त, needle);
  DEBUGASSERT(परिणाम || दत्त->conn);
  प्रत्यागम परिणाम;
}

CURLcode Curl_connect(संरचना Curl_easy *दत्त, bool *pconnected)
{
  CURLcode परिणाम;
  संरचना connectdata *conn = रिक्त;
  ध्रुव संरचना curltime *pnow = रिक्त;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&दत्त->req, दत्त);
  /* Determine the origin of the transfer and what credentials to use */
  परिणाम = url_समुच्चयः_दत्त_origin_and_creds(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(!दत्त->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(दत्त);
  Curl_pgrsTimeWas(दत्त, TIMER_POSTQUEUE, *pnow);
  परिणाम = url_find_or_create_conn(दत्त, pnow);
  conn = दत्त->conn;
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(!दत्त->conn) { /* just make really sure */
    DEBUGASSERT(0);
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  यदि(conn->द्वयङ्काः.reuse) {
    यदि(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  अन्यथा यदि(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrsTime(दत्त, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  अन्यथा {
    परिणाम = Curl_conn_सज्जता(दत्त, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    यदि(!परिणाम)
      परिणाम = Curl_शीर्षकाणि_init(दत्त);
    CURL_TRC_M(दत्त, "Curl_conn_setup() -> %d", (पूर्णाङ्क)परिणाम);
  }

बहिर्गमनम्:
  यदि(परिणाम == CURLE_NO_CONNECTION_उपलब्धम्)
    DEBUGASSERT(!conn);

  यदि(परिणाम && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(दत्त);
    Curl_conn_close(दत्त, conn, TRUE);
  }

  प्रत्यागम परिणाम;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(संरचना Curl_easy *दत्त, संरचना connectdata *conn)
{
  CURLcode परिणाम;

  यदि(conn) {
    conn->द्वयङ्काः.do_अधिकम् = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    यदि(दत्त->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      दत्त->state.wildcardmatch = FALSE;
  }

  दत्त->state.समाप्तम् = FALSE; /* *_done() is not called yet */

  दत्त->req.no_body = दत्त->समुच्चयः.opt_no_body;
  यदि(दत्त->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    दत्त->state.httpreq = HTTPREQ_शीर्ष;

  परिणाम = Curl_req_start(&दत्त->req, दत्त);
  यदि(!परिणाम) {
    Curl_pgrsReset(दत्त);
  }
  प्रत्यागम परिणाम;
}

#यदि defined(USE_HTTP2) || defined(USE_HTTP3)

शून्य Curl_दत्त_प्राथमिकता_clear_state(संरचना Curl_easy *दत्त)
{
  दत्त->state.weight = 0;
}

#यदि_अन्तः /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_समुच्चयः(संरचना connectdata *conn, ध्रुव वर्ण *कुञ्चिका,
                            शून्य *meta_दत्त, Curl_meta_dtor *meta_dtor)
{
  यदि(!Curl_hash_add2(&conn->meta_hash, कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1,
                     meta_दत्त, meta_dtor)) {
    meta_dtor(कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1, meta_दत्त);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_conn_meta_remove(संरचना connectdata *conn, ध्रुव वर्ण *कुञ्चिका)
{
  Curl_hash_delete(&conn->meta_hash, कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1);
}

शून्य *Curl_conn_meta_get(संरचना connectdata *conn, ध्रुव वर्ण *कुञ्चिका)
{
  प्रत्यागम Curl_hash_pick(&conn->meta_hash, कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1);
}

संरचना Curl_easy *Curl_get_admin(संरचना Curl_easy *दत्त)
{
  संरचना Curl_easy *admin;

  यदि(!दत्त->mid) /* already an admin handle */
    admin = दत्त;
  अन्यथा यदि(दत्त->multi)
    admin = दत्त->multi->admin;
  अन्यथा यदि(दत्त->multi_easy)
    admin = दत्त->multi_easy->admin;
  अन्यथा {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = दत्त;
  }
  यदि(admin != दत्त) {
    admin->समुच्चयः.conn_max_idle_ms = दत्त->समुच्चयः.conn_max_idle_ms;
    admin->समुच्चयः.conn_max_age_ms = दत्त->समुच्चयः.conn_max_age_ms;
    admin->समुच्चयः.upkeep_interval_ms = दत्त->समुच्चयः.upkeep_interval_ms;
    admin->समुच्चयः.timeout = दत्त->समुच्चयः.timeout;
    admin->समुच्चयः.server_response_timeout = दत्त->समुच्चयः.server_response_timeout;
    admin->समुच्चयः.no_signal = दत्त->समुच्चयः.no_signal;
  }
  प्रत्यागम admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  यदि(r1 && (r1 != CURLE_AGAIN))
    प्रत्यागम r1;
  यदि(r2 && (r2 != CURLE_AGAIN))
    प्रत्यागम r2;
  प्रत्यागम r1;
}
