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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_IPHLPAPI_H
#समावेश <Iphlpapi.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_UN_H
#समावेश <sys/un.h>
#पूर्ण_अगर

#अगर_अघोषित HAVE_SOCKET
#त्रुटि "We cannot compile without socket() support"
#पूर्ण_अगर

#अगर defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#अगर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#समावेश <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#पूर्ण_अगर
#समावेश <iphlpapi.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "mime.h"
#समावेश "bufref.h"
#समावेश "vtls/vtls.h"
#समावेश "vssh/vssh.h"
#समावेश "transfer.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "cookie.h"
#समावेश "strcase.h"
#समावेश "escape.h"
#समावेश "curl_share.h"
#समावेश "http_digest.h"
#समावेश "multiif.h"
#समावेश "getinfo.h"
#समावेश "pop3.h"
#समावेश "urlapi-int.h"
#समावेश "hsts.h"
#समावेश "proxy.h"
#समावेश "cfilters.h"
#समावेश "idn.h"
#समावेश "http_proxy.h"
#समावेश "conncache.h"
#समावेश "multihandle.h"
#समावेश "curlx/strdup.h"
#समावेश "setopt.h"
#समावेश "altsvc.h"
#समावेश "curlx/dynbuf.h"
#समावेश "headers.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strparse.h"
#समावेश "peer.h"

/* Now for the protocols */
#समावेश "ftp.h"
#समावेश "dict.h"
#समावेश "telnet.h"
#समावेश "tftp.h"
#समावेश "http.h"
#समावेश "vauth/vauth.h"
#समावेश "file.h"
#समावेश "curl_ldap.h"
#समावेश "vssh/ssh.h"
#समावेश "imap.h"
#समावेश "url.h"
#समावेश "connect.h"
#समावेश "gopher.h"
#समावेश "mqtt.h"
#समावेश "rtsp.h"
#समावेश "smtp.h"
#समावेश "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#अगर READBUFFER_आकार < READBUFFER_MIN
# error READBUFFER_आकार is too small
#पूर्ण_अगर

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
अटल curl_prot_t get_protocol_family(स्थिर काष्ठा Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  वापस s->family;
}

व्योम Curl_freeset(काष्ठा Curl_easy *तथ्य)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  क्रमागत dupblob j;

  CURL_EASY_STR_CLEAR0(तथ्य, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(तथ्य, STRING_कुंजी_PASSWD);
  CURL_EASY_STR_CLEAR0(तथ्य, STRING_BEARER);
#अगर_अघोषित CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(तथ्य, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(तथ्य, STRING_कुंजी_PASSWD_PROXY);
#पूर्ण_अगर
  Curl_u8_strset_clear(&तथ्य->set.strings);
  curlx_safefree(तथ्य->set.str_copypostfields);

  क्रम(j = (क्रमागत dupblob)0; j < BLOB_LAST; j++) {
    curlx_safefree(तथ्य->set.blobs[j]);
  }

  Curl_bufref_free(&तथ्य->state.referer);
  Curl_bufref_free(&तथ्य->state.url);

#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(तथ्य->set.mimepostp);
  curlx_safefree(तथ्य->set.mimepostp);
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_COOKIES
  curl_slist_free_all(तथ्य->state.cookielist);
  तथ्य->state.cookielist = शून्य;
#पूर्ण_अगर
}

/* free the URL pieces */
अटल व्योम up_free(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा urlpieces *up = &तथ्य->state.up;
  curlx_safefree(up->options);
  curlx_safefree(up->path);
  curlx_safefree(up->query);
  curl_url_cleanup(तथ्य->state.uh);
  तथ्य->state.uh = शून्य;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(काष्ठा Curl_easy **datap)
{
  काष्ठा Curl_easy *तथ्य;

  अगर(!datap || !*datap)
    वापस CURLE_OK;

  तथ्य = *datap;
  *datap = शून्य;

  अगर(!तथ्य->state.internal && तथ्य->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(तथ्य->multi, तथ्य);
  }
  अन्यथा {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(तथ्य);
    अगर(!तथ्य->state.internal && तथ्य->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(तथ्य->multi_easy);
      तथ्य->multi_easy = शून्य;
    }
  }
  DEBUGASSERT(!तथ्य->conn || तथ्य->state.internal);

  Curl_expire_clear_all(तथ्य); /* shut off any timers left */

  अगर(तथ्य->state.rangestringalloc)
    curlx_free(तथ्य->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(तथ्य);

  तथ्य->set.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  तथ्य->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&तथ्य->req, तथ्य);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(तथ्य);
  Curl_peer_unlink(&तथ्य->state.origin);
  Curl_peer_unlink(&तथ्य->state.initial_origin);
#अगर_अघोषित CURL_DISABLE_RTSP
  Curl_peer_unlink(&तथ्य->state.rtsp_session_origin);
#पूर्ण_अगर
  Curl_ssl_free_certinfo(तथ्य);

  Curl_bufref_free(&तथ्य->state.referer);

  up_free(तथ्य);
  curlx_dyn_free(&तथ्य->state.headerb);
  Curl_flush_cookies(तथ्य, TRUE);
#अगर_अघोषित CURL_DISABLE_ALTSVC
  Curl_altsvc_save(तथ्य, तथ्य->asi, CURL_EASY_STR(तथ्य, STRING_ALTSVC));
  Curl_altsvc_cleanup(&तथ्य->asi);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  Curl_hsts_save(तथ्य, तथ्य->hsts, CURL_EASY_STR(तथ्य, STRING_HSTS));
  अगर(!तथ्य->share || !तथ्य->share->hsts)
    Curl_hsts_cleanup(&तथ्य->hsts);
  curl_slist_free_all(तथ्य->state.hstslist); /* clean up list */
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(तथ्य);
#पूर्ण_अगर
  curlx_safefree(तथ्य->state.most_recent_ftp_entrypath);
  curlx_safefree(तथ्य->info.contenttype);
  curlx_safefree(तथ्य->info.wouldredirect);
  curlx_safefree(तथ्य->info.effective_url);

  /* No longer a dirty share, if it exists */
  अगर(Curl_share_easy_unlink(तथ्य))
    DEBUGASSERT(0);

  Curl_hash_destroy(&तथ्य->meta_hash);
  Curl_creds_unlink(&तथ्य->state.creds);
#अगर_अघोषित CURL_DISABLE_HTTP
  curlx_safefree(तथ्य->state.rangeline);
  curlx_safefree(तथ्य->state.http_host);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_COOKIES
  curlx_safefree(तथ्य->req.cookiehost);
#पूर्ण_अगर

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(तथ्य->state.formp);
  curlx_safefree(तथ्य->state.formp);
#पूर्ण_अगर

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&तथ्य->wildcard);
  Curl_freeset(तथ्य);
  Curl_headers_cleanup(तथ्य);
  Curl_netrc_cleanup(&तथ्य->state.netrc);
#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
  curlx_free(तथ्य->state.envproxy);
#पूर्ण_अगर
  curlx_memzero(तथ्य, माप(*तथ्य));
  curlx_free(तथ्य);
  वापस CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
व्योम Curl_init_userdefined(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा UserDefined *set = &तथ्य->set;

  set->out = मानक_निकास;  /* default output to stdout */
  set->in_set = मानक_निवेश;  /* default input from stdin */
  set->err = मानक_त्रुटि;  /* default stderr to stderr */

  Curl_u8_strset_init(&तथ्य->set.strings);

#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
  /* use fwrite as default function to store output */
  set->fwrite_func = (curl_write_callback)ख_डालो;

  /* use fread as default function to read input */
  set->fread_func_set = (curl_read_callback)ख_पढ़ो;
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
  set->is_fread_set = 0;

  set->seek_client = ZERO_NULL;

  set->filesize = -1;        /* we do not know the size */
  set->postfieldsize = -1;   /* unknown size */
  set->maxredirs = 30;       /* sensible default */

  set->method = HTTPREQ_GET; /* Default HTTP request */
#अगर_अघोषित CURL_DISABLE_RTSP
  set->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  set->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  set->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  set->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  set->ftp_filemethod = FTPFILE_MULTICWD;
  set->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#पूर्ण_अगर
  set->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  set->ssl_ca_cache_timeout = 24 * 60 * 60;

  set->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&तथ्य->set.ssl);
#अगर_अघोषित CURL_DISABLE_PROXY
  Curl_ssl_config_init(&तथ्य->set.proxy_ssl);
  set->proxyport = 0;
  set->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  set->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  set->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_DOH
  set->doh_verifyhost = TRUE;
  set->doh_verifypeer = TRUE;
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  /* defaults to any auth type */
  set->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  set->new_directory_perms = 0755; /* Default permissions */
#पूर्ण_अगर

  set->new_file_perms = 0644;    /* Default permissions */
  set->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  set->redir_protocols = CURLPROTO_REDIR;

#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  set->socks5_gssapi_nec = FALSE;
#पूर्ण_अगर

  /* set default minimum TLS version */
#अगर_घोषित USE_SSL
  Curl_setopt_SSLVERSION(तथ्य, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#अगर_अघोषित CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(तथ्य, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  set->wildcard_enabled = FALSE;
  set->chunk_bgn = ZERO_NULL;
  set->chunk_end = ZERO_NULL;
  set->fnmatch = ZERO_NULL;
#पूर्ण_अगर
  set->tcp_keepalive = FALSE;
  set->tcp_keepintvl = 60;
  set->tcp_keepidle = 60;
  set->tcp_keepcnt = 9;
  set->tcp_fastopen = FALSE;
  set->tcp_nodelay = TRUE;
  set->ssl_enable_alpn = TRUE;
  set->expect_100_timeout = 1000L; /* Wait for a second by default. */
  set->sep_headers = TRUE; /* separated header lists by default */
  set->बफर_आकार = READBUFFER_आकार;
  set->upload_बफर_आकार = UPLOADBUFFER_DEFAULT;
  set->upload_flags = CURLULFLAG_SEEN;
  set->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  set->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  set->maxconnects = DEFAULT_CONNCACHE_आकार; /* for easy handles */
  set->conn_max_idle_ms = 118 * 1000;
  set->conn_max_age_ms = 24 * 3600 * 1000;
  set->http09_allowed = FALSE;
  set->httpwant = CURL_HTTP_VERSION_NONE;
#अगर defined(USE_HTTP2) || defined(USE_HTTP3)
  set->weight = 0;
#पूर्ण_अगर
  set->quick_exit = 0L;
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  set->ws_raw_mode = FALSE;
  set->ws_no_auto_pong = FALSE;
#पूर्ण_अगर
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
अटल व्योम easy_meta_freeentry(व्योम *p)
{
  (व्योम)p;
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
CURLcode Curl_open(काष्ठा Curl_easy **curl)
{
  काष्ठा Curl_easy *तथ्य;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  तथ्य = curlx_calloc(1, माप(काष्ठा Curl_easy));
  अगर(!तथ्य) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: calloc of Curl_easy failed\n"));
    वापस CURLE_OUT_OF_MEMORY;
  }

  तथ्य->magic = CURLEASY_MAGIC_संख्या;
  /* most recent connection is not yet defined */
  तथ्य->state.last_conn_id = -1;
  तथ्य->state.last_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  तथ्य->id = -1;
  तथ्य->mid = UINT32_MAX;
  तथ्य->master_mid = UINT32_MAX;
  तथ्य->progress.hide = TRUE;

  Curl_hash_init(&तथ्य->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_LAST <= UINT8_MAX);
  Curl_u8_strset_init(&तथ्य->set.strings);
  curlx_dyn_init(&तथ्य->state.headerb, CURL_MAX_HTTP_HEADER);
  Curl_bufref_init(&तथ्य->state.url);
  Curl_bufref_init(&तथ्य->state.referer);
  Curl_req_init(&तथ्य->req);
  Curl_initinfo(तथ्य);
#अगर_अघोषित CURL_DISABLE_HTTP
  Curl_llist_init(&तथ्य->state.httphdrs, शून्य);
#पूर्ण_अगर
  Curl_netrc_init(&तथ्य->state.netrc);
  Curl_init_userdefined(तथ्य);

  *curl = तथ्य;
  वापस CURLE_OK;
}

व्योम Curl_conn_free(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  अगर(conn->scheme && conn->scheme->run->disconnect &&
     !conn->bits.shutdown_handler)
    conn->scheme->run->disconnect(तथ्य, conn, TRUE);

  क्रम(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(तथ्य, conn, i);
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#पूर्ण_अगर
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->via_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->via_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
अटल bool xfer_may_multiplex(स्थिर काष्ठा Curl_easy *तथ्य,
                               स्थिर काष्ठा connectdata *conn)
{
#अगर_अघोषित CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  अगर((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->bits.protoconnstart || !conn->bits.बंद)) {

    अगर(Curl_multiplex_wanted(तथ्य->multi) &&
       (तथ्य->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      वापस TRUE;
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)conn;
#पूर्ण_अगर
  वापस FALSE;
}

#अगर_अघोषित CURL_DISABLE_PROXY
अटल bool proxy_info_matches(स्थिर काष्ठा proxy_info *तथ्य,
                               स्थिर काष्ठा proxy_info *needle)
{
  अगर((तथ्य->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(तथ्य->peer, needle->peer) &&
     Curl_creds_same(तथ्य->creds, needle->creds)) {
    वापस TRUE;
  }
  वापस FALSE;
}
#पूर्ण_अगर

#अगर_घोषित USE_SSH
अटल bool ssh_config_matches(काष्ठा connectdata *one,
                               काष्ठा connectdata *two)
{
  काष्ठा ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  वापस sshc1 && sshc2 && Curl_safecmp(sshc1->priv_कुंजी, sshc2->priv_कुंजी) &&
         Curl_safecmp(sshc1->pub_कुंजी, sshc2->pub_कुंजी);
}
#पूर्ण_अगर

काष्ठा url_conn_match {
  काष्ठा connectdata *found;
  काष्ठा Curl_easy *तथ्य;
  काष्ठा connectdata *needle;
  काष्ठा curltime now;
  काष्ठा ssl_filter_config ssl_config;
#अगर_अघोषित CURL_DISABLE_PROXY
  काष्ठा ssl_filter_config proxy_ssl_config;
#पूर्ण_अगर
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
  BIT(seen_pending_conn);
  BIT(seen_single_use_conn);
  BIT(seen_multiplex_conn);
};

अटल bool url_match_connect_config(काष्ठा connectdata *conn,
                                     काष्ठा url_conn_match *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  अगर(conn->bits.connect_only || conn->bits.बंद || conn->bits.no_reuse)
    वापस FALSE;

  /* ip_version must match */
  अगर(m->तथ्य->set.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->तथ्य->set.ipver != conn->ip_version)
    वापस FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  अगर(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    वापस FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  अगर((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      म_भेद(conn->interface_opts, m->needle->interface_opts)))
    वापस FALSE;

  अगर((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       म_भेद(conn->interface_opts, m->needle->interface_opts)))))
    वापस FALSE;

  अगर(!m->needle->via_peer != !conn->via_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    वापस FALSE;

  वापस TRUE;
}

अटल bool url_match_fully_connected(काष्ठा connectdata *conn,
                                      काष्ठा url_conn_match *m)
{
  अगर(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->bits.upgrade_in_progress) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    अगर(m->may_multiplex) {
      m->seen_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->तथ्य, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    वापस FALSE;
  }
  वापस TRUE;
}

अटल bool url_match_multi(काष्ठा connectdata *conn,
                            काष्ठा url_conn_match *m)
{
  अगर(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    अगर(conn->attached_multi != m->तथ्य->multi)
      वापस FALSE;
  }
  वापस TRUE;
}

अटल bool url_match_multiplex_needs(काष्ठा connectdata *conn,
                                      काष्ठा url_conn_match *m)
{
  अगर(CONN_INUSE(conn)) {
    अगर(!conn->bits.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->seen_single_use_conn = TRUE;
      वापस FALSE;
    }
    m->seen_multiplex_conn = TRUE;
    अगर(!m->may_multiplex || !url_match_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      वापस FALSE;
  }
  वापस TRUE;
}

अटल bool url_match_multiplex_limits(काष्ठा connectdata *conn,
                                       काष्ठा url_conn_match *m)
{
  अगर(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->bits.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    अगर(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->तथ्य->multi)) {
      infof(m->तथ्य, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      वापस FALSE;
    }
    अगर(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->तथ्य, conn, FIRSTSOCKET)) {
      infof(m->तथ्य, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      वापस FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->तथ्य, "Multiplexed connection found");
  }
  वापस TRUE;
}

अटल bool url_match_ssl_use(काष्ठा connectdata *conn,
                              काष्ठा url_conn_match *m)
{
  अगर(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    अगर(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      वापस FALSE;
  }
  अन्यथा अगर(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    अगर(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      वापस FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    अगर(!Curl_ssl_conn_config_match(m->तथ्य, &m->ssl_config, conn, FALSE))
      वापस FALSE;
  }
  अन्यथा अगर(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    वापस FALSE;
  वापस TRUE;
}

#अगर_अघोषित CURL_DISABLE_PROXY
अटल bool url_match_proxy_use(काष्ठा connectdata *conn,
                                काष्ठा url_conn_match *m)
{
  अगर(m->needle->bits.origin_is_proxy != conn->bits.origin_is_proxy)
    वापस FALSE;

  अगर(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    वापस FALSE;

  अगर(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    वापस FALSE;

  अगर(m->तथ्य->set.socks5_auth_only && !conn->bits.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->तथ्य,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    वापस FALSE;
  }

  अगर(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    अगर(!Curl_ssl_conn_config_match(m->तथ्य, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->तथ्य,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      वापस FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  वापस TRUE;
}
#अन्यथा
#घोषणा url_match_proxy_use(c, m) ((व्योम)(c), (व्योम)(m), TRUE)
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_HTTP
अटल bool url_match_http_multiplex(काष्ठा connectdata *conn,
                                     काष्ठा url_conn_match *m,
                                     bool *pwait_pipe)
{
  अगर(m->may_multiplex &&
     (m->तथ्य->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_seen) {
    अगर(m->तथ्य->set.pipewait) {
      infof(m->तथ्य, "Server upgrade does not support multiplex yet, wait");
      m->found = शून्य;
      *pwait_pipe = TRUE;
      वापस TRUE; /* stop searching, we want to wait */
    }
    infof(m->तथ्य, "Server upgrade cannot be used");
    वापस FALSE;
  }
  वापस TRUE;
}

अटल bool url_match_http_version(काष्ठा connectdata *conn,
                                   काष्ठा url_conn_match *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  अगर((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    चयन(Curl_conn_http_version(m->तथ्य, conn)) {
    हाल 30:
      अगर(!(m->तथ्य->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->तथ्य, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        वापस FALSE;
      }
      अवरोध;
    हाल 20:
      अगर(!(m->तथ्य->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->तथ्य, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        वापस FALSE;
      }
      अवरोध;
    शेष:
      अगर(!(m->तथ्य->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->तथ्य, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        वापस FALSE;
      }
      अवरोध;
    }
  }
  वापस TRUE;
}
#अन्यथा
#घोषणा url_match_http_multiplex(c, m, w) ((व्योम)(c), (व्योम)(m), TRUE)
#घोषणा url_match_http_version(c, m)      ((व्योम)(c), (व्योम)(m), TRUE)
#पूर्ण_अगर

अटल bool url_match_proto_config(काष्ठा connectdata *conn,
                                   काष्ठा url_conn_match *m)
{
  अगर(!url_match_http_version(conn, m))
    वापस FALSE;

#अगर_घोषित USE_SSH
  अगर(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    अगर(!ssh_config_matches(m->needle, conn))
      वापस FALSE;
  }
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  अन्यथा अगर(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    अगर(!Curl_ftp_conns_match(m->needle, conn))
      वापस FALSE;
  }
#पूर्ण_अगर
  वापस TRUE;
}

अटल bool url_match_auth(काष्ठा connectdata *conn,
                           काष्ठा url_conn_match *m)
{
  अगर(!Curl_creds_same(m->needle->creds, conn->creds)) {
    अगर(m->needle->creds)
      वापस FALSE;
    अगर(!Curl_creds_same(m->तथ्य->state.creds, conn->creds))
      वापस FALSE;
  }
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  अगर(m->needle->gssapi_delegation != conn->gssapi_delegation)
    वापस FALSE;
#पूर्ण_अगर

  वापस TRUE;
}

अटल bool url_match_destination(काष्ठा connectdata *conn,
                                  काष्ठा url_conn_match *m)
{
  /* Different connect-to peers never match */
  अगर(!Curl_peer_same_destination(m->needle->via_peer, conn->via_peer))
    वापस FALSE;

  अगर(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    वापस FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  वापस Curl_peer_same_destination(m->needle->origin, conn->origin);
}

अटल bool url_match_ssl_config(काष्ठा connectdata *conn,
                                 काष्ठा url_conn_match *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  अगर(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_match(m->तथ्य, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->तथ्य, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    वापस FALSE;
  }
  वापस TRUE;
}

#अगर_घोषित USE_NTLM
अटल bool url_match_auth_ntlm(काष्ठा connectdata *conn,
                                काष्ठा url_conn_match *m)
{
  अगर(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    अगर(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->तथ्य->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->तथ्य->state.origin))
      वापस FALSE;
  }
  अन्यथा अगर(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    अगर(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->तथ्य->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->तथ्य->state.origin)))
      वापस FALSE;
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  अगर(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    अगर(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      वापस FALSE;
  }
  अन्यथा अगर(m->want_proxy_ntlm_http) {
    अगर(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      वापस FALSE;
  }
#पूर्ण_अगर
  वापस TRUE;
}
#अन्यथा
#घोषणा url_match_auth_ntlm(c, m) ((व्योम)(c), (व्योम)(m), TRUE)
#पूर्ण_अगर

#अगर_घोषित USE_SPNEGO
अटल bool url_match_auth_nego(काष्ठा connectdata *conn,
                                काष्ठा url_conn_match *m)
{
  अगर(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    अगर(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->तथ्य->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->तथ्य->state.origin))
      वापस FALSE;
  }
  अन्यथा अगर(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    अगर(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->तथ्य->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->तथ्य->state.origin)))
      वापस FALSE;
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  अगर(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    अगर(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      वापस FALSE;
  }
  अन्यथा अगर(m->want_proxy_nego_http) {
    अगर(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      वापस FALSE;
  }
#पूर्ण_अगर
  वापस TRUE;
}
#अन्यथा
#घोषणा url_match_auth_nego(c, m) ((व्योम)(c), (व्योम)(m), TRUE)
#पूर्ण_अगर

अटल cpool_match_परिणाम url_match_conn(काष्ठा connectdata *conn,
                                         व्योम *userdata)
{
  काष्ठा url_conn_match *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  अगर(!url_match_connect_config(conn, m))
    वापस CPOOL_MATCH_CONT;

  /* match for destination and protocol? */
  अगर(!url_match_destination(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_fully_connected(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_multiplex_needs(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_ssl_use(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_proxy_use(conn, m))
    वापस CPOOL_MATCH_CONT;
  अगर(!url_match_ssl_config(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_http_multiplex(conn, m, &wait_pipe))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_auth(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_proto_config(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_auth_ntlm(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_auth_nego(conn, m))
    वापस CPOOL_MATCH_CONT;

  अगर(!url_match_multiplex_limits(conn, m))
    वापस CPOOL_MATCH_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  अगर(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->found);
    m->wait_pipe = TRUE;
    वापस CPOOL_MATCH_FOUND;
  }

  अगर(m->तथ्य->set.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->तथ्य, conn, &m->now);
    अगर(age_ms > m->तथ्य->set.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      वापस CPOOL_MATCH_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  अगर(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->तथ्य, &m->now)) {
    infof(m->तथ्य, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    वापस CPOOL_MATCH_CLOSE;
  }

  /* conn matches our needs. */
  m->found = conn;
  वापस CPOOL_MATCH_FOUND;
}

अटल bool url_match_परिणाम(व्योम *userdata)
{
  काष्ठा url_conn_match *match = userdata;
  अगर(match->found) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(match->तथ्य, match->found, TRUE);
    वापस TRUE;
  }
  अन्यथा अगर(match->seen_single_use_conn && !match->seen_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    match->wait_pipe = FALSE;
  }
  अन्यथा अगर(match->seen_pending_conn && match->तथ्य->set.pipewait) {
    infof(match->तथ्य,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    match->wait_pipe = TRUE;
  }
  वापस FALSE;
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
अटल bool url_attach_existing(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *needle,
                                काष्ठा url_conn_match *m,
                                स्थिर काष्ठा curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  वापस Curl_cpool_find(तथ्य, needle->destination, TRUE, pnow,
                         url_match_conn, url_match_परिणाम, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
अटल काष्ठा connectdata *allocate_conn(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = curlx_calloc(1, माप(काष्ठा connectdata));
  अगर(!conn)
    वापस शून्य;

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

#अगर_अघोषित CURL_DISABLE_FTP
  conn->bits.ftp_use_epsv = तथ्य->set.ftp_use_epsv;
  conn->bits.ftp_use_eprt = तथ्य->set.ftp_use_eprt;
#पूर्ण_अगर
  conn->ip_version = तथ्य->set.ipver;
  conn->bits.connect_only = (bool)तथ्य->set.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  अगर(CURL_EASY_STR(तथ्य, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(तथ्य, STRING_INTERFACE_OPT));
    अगर(!conn->interface_opts)
      जाओ error;
  }
#अगर_अघोषित CURL_DISABLE_BINDLOCAL
  conn->localportrange = तथ्य->set.localportrange;
  conn->localport = तथ्य->set.localport;
#पूर्ण_अगर

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = तथ्य->set.fclosesocket;
  conn->closesocket_client = तथ्य->set.closesocket_client;
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = तथ्य->set.gssapi_delegation;
#पूर्ण_अगर
  वापस conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  वापस शून्य;
}

अटल CURLcode url_set_conn_scheme(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  स्थिर काष्ठा Curl_scheme *scheme = conn->origin->scheme;
  अगर(scheme->run &&
     (तथ्य->set.allowed_protocols & scheme->protocol) &&
     (!तथ्य->state.this_is_a_follow ||
       (तथ्य->set.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    वापस CURLE_OK;
  }
  अगर(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(तथ्य, "Protocol \"%s\" is not for transfers", scheme->name);
  अन्यथा
    failf(तथ्य, "Protocol \"%s\" is disabled%s", scheme->name,
          तथ्य->state.this_is_a_follow ? " (in redirect)" : "");
  वापस CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_to_curlcode(CURLUcode uc)
{
  चयन(uc) {
  शेष:
    वापस CURLE_URL_MALFORMAT;
  हाल CURLUE_UNSUPPORTED_SCHEME:
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  हाल CURLUE_OUT_OF_MEMORY:
    वापस CURLE_OUT_OF_MEMORY;
  हाल CURLUE_USER_NOT_ALLOWED:
    वापस CURLE_LOGIN_DENIED;
  }
}

#अगर_अघोषित CURL_DISABLE_HSTS
अटल CURLcode hsts_upgrade(काष्ठा Curl_easy *तथ्य,
                             CURLU *uh,
                             uint16_t port_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  अगर(तथ्य->hsts && (तथ्य->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(तथ्य->hsts, तथ्य->state.origin)) {
    अक्षर *url;
    CURLUcode uc;
    CURLcode परिणाम;

    uc = curl_url_set(uh, CURLUPART_SCHEME, "https", 0);
    अगर(uc)
      वापस Curl_uc_to_curlcode(uc);
    Curl_bufref_free(&तथ्य->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    अगर(uc)
      वापस Curl_uc_to_curlcode(uc);
    Curl_bufref_set(&तथ्य->state.url, url, 0, curl_free);

    परिणाम = Curl_peer_from_url(uh, तथ्य, port_override, scope_id,
                                &तथ्य->state.origin);
    अगर(परिणाम)
      वापस परिणाम;
    infof(तथ्य, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  वापस CURLE_OK;
}
#अन्यथा
#घोषणा hsts_upgrade(x, y, z, a) CURLE_OK
#पूर्ण_अगर

अटल bool str_has_ctrl(स्थिर अक्षर *निवेश)
{
  अगर(निवेश) {
    स्थिर अचिन्हित अक्षर *str = (स्थिर अचिन्हित अक्षर *)निवेश;
    जबतक(*str) {
      अगर(*str < 0x20)
        वापस TRUE;
      str++;
    }
  }
  वापस FALSE;
}

#अगर_अघोषित CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
अटल CURLcode url_set_तथ्य_creds_netrc(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा Curl_creds **pcreds)
{
  काष्ठा Curl_creds *ncreds_out = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(तथ्य->set.use_netrc) { /* not CURL_NETRC_IGNORED */
    काष्ठा Curl_creds *ncreds_in = शून्य;
    bool scan_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    अगर(*pcreds) {
      चयन((*pcreds)->source) {
      हाल CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        scan_netrc = FALSE;
        अवरोध;
      हाल CREDS_URL: /* only apply when netrc is not required */
        अगर(तथ्य->set.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_in = *pcreds;
        }
        अन्यथा अगर(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_in = *pcreds;
        }
        अन्यथा
          scan_netrc = FALSE;
        अवरोध;
      शेष: /* ignore credentials from other sources */
        अवरोध;
      }
    }

    अगर(!scan_netrc)
      जाओ out;

    ret = Curl_netrc_scan(तथ्य, &तथ्य->state.netrc,
                          तथ्य->state.origin->hostname,
                          Curl_creds_user(ncreds_in),
                          CURL_EASY_STR(तथ्य, STRING_NETRC_FILE),
                          &ncreds_out);
    DEBUGASSERT(!ret || !ncreds_out);
    अगर(ret == NETRC_OUT_OF_MEMORY) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
    अन्यथा अगर(ret && ((ret == NETRC_NO_MATCH) ||
                    (तथ्य->set.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(तथ्य, "Could not find host %s in the %s file; using defaults",
            तथ्य->state.origin->hostname,
            (CURL_EASY_STR(तथ्य, STRING_NETRC_FILE) ?
             CURL_EASY_STR(तथ्य, STRING_NETRC_FILE) : ".netrc"));
    }
    अन्यथा अगर(ret) {
      स्थिर अक्षर *m = Curl_netrc_strerror(ret);
      failf(तथ्य, ".netrc error: %s", m);
      परिणाम = CURLE_READ_ERROR;
      जाओ out;
    }
    अन्यथा अगर(ncreds_out) {
      अगर(!(तथ्य->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_out->user) ||
          str_has_ctrl(ncreds_out->passwd))) {
        failf(तथ्य, "control code detected in .netrc credentials");
        परिणाम = CURLE_READ_ERROR;
        जाओ out;
      }
      CURL_TRC_M(तथ्य, "netrc: using credentials for %s as %s",
                 तथ्य->state.origin->hostname, ncreds_out->user);
      परिणाम = Curl_creds_merge(ncreds_out->user, ncreds_out->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      अगर(परिणाम)
        जाओ out;
      /* for updated strings, we update them in the URL */
      uc = curl_url_set(तथ्य->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      अगर(!uc)
        uc = curl_url_set(तथ्य->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      अगर(uc)
        परिणाम = Curl_uc_to_curlcode(uc);
    }
    अन्यथा
      DEBUGASSERT(0);
  }

#अगर_घोषित CURLVERBOSE
  Curl_creds_trace(तथ्य, तथ्य->state.creds, "transfer credentials");
#पूर्ण_अगर

out:
  Curl_creds_unlink(&ncreds_out);
  वापस परिणाम;
}
#पूर्ण_अगर /* CURL_DISABLE_NETRC */

अटल CURLcode url_set_तथ्य_creds(काष्ठा Curl_easy *तथ्य, CURLU *uh)
{
  काष्ठा Curl_creds *newcreds = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर((CURL_EASY_STR(तथ्य, STRING_USERNAME) ||
      CURL_EASY_STR(तथ्य, STRING_PASSWORD) ||
      CURL_EASY_STR(तथ्य, STRING_BEARER) ||
      CURL_EASY_STR(तथ्य, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(तथ्य, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_to_origin(तथ्य, तथ्य->state.origin)) {
    परिणाम = Curl_creds_create(CURL_EASY_STR(तथ्य, STRING_USERNAME),
                               CURL_EASY_STR(तथ्य, STRING_PASSWORD),
                               CURL_EASY_STR(तथ्य, STRING_BEARER),
                               CURL_EASY_STR(तथ्य, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(तथ्य, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    अगर(परिणाम)
      जाओ out;
    अगर(newcreds &&
       !(तथ्य->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(तथ्य, "control code detected in credentials");
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  अगर(!newcreds || !Curl_creds_has_user(newcreds)) {
    अक्षर *user = शून्य;
    अक्षर *passwd = शून्य;
    अक्षर *udecoded = शून्य;
    अक्षर *pdecoded = शून्य;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    अगर(uc && (uc != CURLUE_NO_USER))
      परिणाम = Curl_uc_to_curlcode(uc);
    अगर(!परिणाम) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      अगर(uc && (uc != CURLUE_NO_PASSWORD))
        परिणाम = Curl_uc_to_curlcode(uc);
    }
    अगर(!परिणाम && user) {
      परिणाम = Curl_urldecode(user, 0, &udecoded, शून्य,
                              (तथ्य->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    अगर(!परिणाम && passwd) {
      परिणाम = Curl_urldecode(passwd, 0, &pdecoded, शून्य,
                              (तथ्य->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    अगर(!परिणाम)
      परिणाम = Curl_creds_merge(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    अगर(परिणाम) {
      failf(तथ्य, "error extracting credentials from URL");
      जाओ out;
    }
  }

#अगर_अघोषित CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  परिणाम = url_set_तथ्य_creds_netrc(तथ्य, &newcreds);
#पूर्ण_अगर /* CURL_DISABLE_NETRC */

out:
  अगर(!परिणाम && !Curl_creds_equal(तथ्य->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&तथ्य->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  वापस परिणाम;
}

अटल CURLcode url_set_conn_origin_etc(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;

  Curl_peer_link(&conn->origin, तथ्य->state.origin);

  /* set the connection scheme */
  परिणाम = url_set_conn_scheme(तथ्य, conn);
  अगर(परिणाम)
    जाओ out;

  /* set the connection options */
  अगर(CURL_EASY_STR(तथ्य, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(तथ्य, STRING_OPTIONS));
    अगर(!conn->options) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }
  अन्यथा अगर(तथ्य->state.up.options) {
    conn->options = curlx_strdup(तथ्य->state.up.options);
    अगर(!conn->options) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

out:
  वापस परिणाम;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
अटल CURLcode setup_range(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा UrlState *s = &तथ्य->state;
  s->resume_from = तथ्य->set.set_resume_from;
  अगर(s->resume_from || CURL_EASY_STR(तथ्य, STRING_SET_RANGE)) {
    अगर(s->rangestringalloc)
      curlx_free(s->range);

    अगर(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    अन्यथा
      s->range = curlx_strdup(CURL_EASY_STR(तथ्य, STRING_SET_RANGE));

    अगर(!s->range)
      वापस CURLE_OUT_OF_MEMORY;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  अन्यथा
    s->use_range = FALSE; /* disable range download */

  वापस CURLE_OK;
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
अटल CURLcode setup_connection_internals(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा connectdata *conn)
{
  काष्ठा Curl_peer *peer = शून्य;

  अगर(conn->scheme->run->setup_connection) {
    CURLcode परिणाम = conn->scheme->run->setup_connection(तथ्य, conn);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  अगर(!peer)
    वापस CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  अगर(peer->ipv6)
    अगर(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->port);
    अन्यथा
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->port);
  अन्यथा
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->port);
  अगर(!conn->destination)
    वापस CURLE_OUT_OF_MEMORY;

  Curl_strntolower(conn->destination, conn->destination,
                   म_माप(conn->destination));

#अगर_घोषित USE_IPV6
  अगर(तथ्य->set.scope_id)
    conn->scope_id = तथ्य->set.scope_id;
  अन्यथा {
    peer = Curl_conn_get_first_peer(conn, FIRSTSOCKET);
    अगर(!peer)
      वापस CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#पूर्ण_अगर

  वापस CURLE_OK;
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
CURLcode Curl_parse_login_details(स्थिर अक्षर *login, स्थिर माप_प्रकार len,
                                  अक्षर **userp, अक्षर **passwdp,
                                  अक्षर **optionsp)
{
  अक्षर *ubuf = शून्य;
  अक्षर *pbuf = शून्य;
  स्थिर अक्षर *psep = शून्य;
  स्थिर अक्षर *osep = शून्य;
  माप_प्रकार ulen;
  माप_प्रकार plen;
  माप_प्रकार olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = स_प्रथम(login, ':', len);

  /* Attempt to find the options separator */
  अगर(optionsp)
    osep = स_प्रथम(login, ';', len);

  /* Calculate the portion lengths */
  ulen = (psep ?
          (माप_प्रकार)(osep && psep > osep ? osep - login : psep - login) :
          (osep ? (माप_प्रकार)(osep - login) : len));
  plen = (psep ?
          (osep && osep > psep ? (माप_प्रकार)(osep - psep) :
           (माप_प्रकार)(login + len - psep)) - 1 : 0);
  olen = (osep ?
          (psep && psep > osep ? (माप_प्रकार)(psep - osep) :
           (माप_प्रकार)(login + len - osep)) - 1 : 0);

  /* Clone the user portion buffer, which can be zero length */
  ubuf = curlx_memdup0(login, ulen);
  अगर(!ubuf)
    जाओ error;

  /* Clone the password portion buffer */
  अगर(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    अगर(!pbuf)
      जाओ error;
  }

  /* Allocate the options portion buffer */
  अगर(optionsp) {
    अक्षर *obuf = शून्य;
    अगर(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      अगर(!obuf)
        जाओ error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  वापस CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  वापस CURLE_OUT_OF_MEMORY;
}

/*
 * Set the login details so they are available in the connection
 */
अटल CURLcode url_set_conn_login(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  अगर((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, तथ्य->state.origin);
    अगर(तथ्य->state.creds)
      Curl_creds_link(&conn->creds, तथ्य->state.creds);
    अन्यथा
      वापस Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               शून्य, शून्य, शून्य, CREDS_NONE, &conn->creds);
  }
  अन्यथा अगर(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, तथ्य->state.origin);
    Curl_creds_link(&conn->creds, तथ्य->state.creds);
  }

  वापस CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
अटल CURLcode parse_connect_to_string(काष्ठा Curl_easy *तथ्य,
                                        स्थिर काष्ठा Curl_peer *dest,
                                        स्थिर अक्षर *conn_to_पंक्ति,
                                        काष्ठा Curl_peer **pvia_dest)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ptr = conn_to_पंक्ति;
  bool host_match = FALSE;
  bool port_match = FALSE;

  *pvia_dest = शून्य;

  अगर(*ptr == ':') {
    /* an empty hostname always matches */
    host_match = TRUE;
    ptr++;
  }
  अन्यथा {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    माप_प्रकार hlen = म_माप(dest->hostname);
    host_match = curl_strnequal(ptr, dest->hostname, hlen);
    अगर(!host_match && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = म_माप(dest->user_hostname);
      host_match = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_match = host_match && ptr[hlen] == ':';
    अगर(host_match)
      ptr += hlen + 1;
  }

  अगर(host_match) {
    अगर(*ptr == ':') {
      /* an empty port always matches */
      port_match = TRUE;
      ptr++;
    }
    अन्यथा {
      /* check whether the URL's port matches */
      स्थिर अक्षर *ptr_अग्र = म_अक्षर(ptr, ':');
      अगर(ptr_अग्र) {
        curl_off_t port_to_match;
        अगर(!curlx_str_संख्या(&ptr, &port_to_match, 0xffff) &&
           ((uint16_t)port_to_match == dest->port)) {
          port_match = TRUE;
        }
        ptr = ptr_अग्र + 1;
      }
    }
  }

  अगर(host_match && port_match && ptr && *ptr)
    परिणाम = Curl_peer_from_connect_to(तथ्य, dest, ptr, pvia_dest);

  वापस परिणाम;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
अटल CURLcode url_set_conn_peer(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा Curl_peer *origin = conn->origin;
  काष्ठा Curl_peer *via_peer = शून्य;
  काष्ठा curl_slist *conn_to_entry = तथ्य->set.connect_to;

  DEBUGASSERT(!conn->via_peer);
  Curl_peer_unlink(&conn->via_peer);

  जबतक(conn_to_entry && !via_peer) {
    परिणाम = parse_connect_to_string(तथ्य, origin, conn_to_entry->तथ्य,
                                     &via_peer);
    अगर(परिणाम)
      वापस परिणाम;
    conn_to_entry = conn_to_entry->अग्र;
  }

#अगर_अघोषित CURL_DISABLE_ALTSVC
  अगर(तथ्य->asi && !via_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#अगर_घोषित DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      दो_पर्या("CURL_ALTSVC_HTTP")
#अन्यथा
      0
#पूर्ण_अगर
       )) {
    /* no connect_to match, try alt-svc! */
    क्रमागत alpnid srcalpnid = ALPN_none;
    bool hit = FALSE;
    काष्ठा altsvc *as = शून्य;
    पूर्णांक allowed_alpns = ALPN_none;
    काष्ठा http_negotiation *neg = &तथ्य->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(तथ्य, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#अगर_घोषित USE_HTTP3
    अगर(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#पूर्ण_अगर
#अगर_घोषित USE_HTTP2
    अगर(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#पूर्ण_अगर
    अगर(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (पूर्णांक)तथ्य->asi->flags;

    DEBUGF(infof(तथ्य, "check Alt-Svc for host '%s'", origin->hostname));
#अगर_घोषित USE_HTTP3
    अगर(!hit && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      hit = Curl_altsvc_lookup(तथ्य->asi,
                               origin, ALPN_h3, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }
#पूर्ण_अगर
#अगर_घोषित USE_HTTP2
    अगर(!hit && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_prior_knowledge) {
      srcalpnid = ALPN_h2;
      hit = Curl_altsvc_lookup(तथ्य->asi,
                               origin, ALPN_h2, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }
#पूर्ण_अगर
    अगर(!hit && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      hit = Curl_altsvc_lookup(तथ्य->asi,
                               origin, ALPN_h1, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }

    अगर(hit && same_dest) {
      /* same destination, but more HTTPS version options */
      चयन(as->dst.alpnid) {
      हाल ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        अवरोध;
      हाल ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        अवरोध;
      हाल ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        अवरोध;
      शेष: /* should not be possible */
        अवरोध;
      }
    }
    अन्यथा अगर(hit) {
      परिणाम = Curl_peer_create(तथ्य, conn->origin->scheme,
                                as->dst.host, as->dst.port,
                                &via_peer);
      अगर(परिणाम)
        वापस परिणाम;
      infof(तथ्य, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->port,
            Curl_alpnid2str(as->dst.alpnid),
            via_peer->hostname, via_peer->port);
      conn->bits.altused = TRUE;
      अगर(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        चयन(as->dst.alpnid) {
        हाल ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          अवरोध;
        हाल ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          अवरोध;
        हाल ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          अवरोध;
        शेष: /* should not be possible */
          अवरोध;
        }
      }
    }
  }
#पूर्ण_अगर

  अगर(via_peer)
    conn->via_peer = via_peer;

  वापस परिणाम;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
अटल व्योम url_conn_reuse_adjust(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *needle)
{
  काष्ठा connectdata *conn = तथ्य->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  अगर(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#पूर्ण_अगर

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
  Curl_peer_link(&conn->via_peer, needle->via_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->via_peer2, needle->via_peer2);
}

अटल व्योम conn_meta_freeentry(व्योम *p)
{
  (व्योम)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

अटल CURLcode url_create_needle(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata **pneedle)
{
  काष्ठा connectdata *needle = शून्य;
  CURLcode परिणाम = CURLE_OK;
  bool network_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(तथ्य);
  अगर(!needle) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  परिणाम = url_set_conn_origin_etc(तथ्य, needle);
  अगर(परिणाम)
    जाओ out;

  DEBUGASSERT(needle->origin);
  network_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#अगर_घोषित USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  अगर(network_scheme && CURL_EASY_STR(तथ्य, STRING_UNIX_SOCKET_PATH)) {
    परिणाम = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(तथ्य, STRING_UNIX_SOCKET_PATH),
      (bool)तथ्य->set.abstract_unix_socket, &needle->via_peer);
    अगर(परिणाम)
      जाओ out;
  }
#पूर्ण_अगर /* USE_UNIX_SOCKETS */

  अगर(network_scheme && !needle->via_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    परिणाम = url_set_conn_peer(तथ्य, needle);
    अगर(परिणाम)
      जाओ out;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  अगर(Curl_peer_equal(needle->origin, needle->via_peer)) {
    Curl_peer_unlink(&needle->via_peer);
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  अगर(network_scheme &&
     (!needle->via_peer || !needle->via_peer->unix_socket)) {
    परिणाम = Curl_proxy_init_conn(तथ्य, needle);
    अगर(परिणाम)
      जाओ out;
  }
#पूर्ण_अगर /* CURL_DISABLE_PROXY */

  परिणाम = url_set_conn_login(तथ्य, needle); /* default credentials */
  अगर(परिणाम)
    जाओ out;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  परिणाम = setup_connection_internals(तथ्य, needle);
  अगर(परिणाम)
    जाओ out;

  अगर(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    अगर(तथ्य->set.ssl_enable_alpn)
      needle->bits.tls_enable_alpn = TRUE;
  }

  अगर(network_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->bits.tcp_fastopen = तथ्य->set.tcp_fastopen;
#अगर_घोषित USE_UNIX_SOCKETS
    अगर(Curl_conn_get_first_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#पूर्ण_अगर
  }

out:
  अगर(!परिणाम) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  अन्यथा {
    *pneedle = शून्य;
    अगर(needle)
      Curl_conn_free(तथ्य, needle);
  }
  वापस परिणाम;
}

अटल CURLcode url_set_तथ्य_origin_and_creds(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_set_uh = (तथ्य->set.uh && !तथ्य->state.this_is_a_follow);
  uint16_t port_override = तथ्य->state.allow_port ? तथ्य->set.use_port : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  अगर(!Curl_bufref_ptr(&तथ्य->state.url)) {
    परिणाम = CURLE_URL_MALFORMAT;
    जाओ out;
  }

  up_free(तथ्य); /* cleanup previous leftovers first */

  /* parse the URL */
  अगर(use_set_uh)
    uh = तथ्य->state.uh = curl_url_dup(तथ्य->set.uh);
  अन्यथा
    uh = तथ्य->state.uh = curl_url();
  अगर(!uh) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  अगर(!use_set_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    अगर(curl_strequal("file", CURL_EASY_STR(तथ्य, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&तथ्य->state.url), शून्य, 0, TRUE))
      prepend_sheme = TRUE;
    अन्यथा {
      uc = curl_url_set(uh, CURLUPART_URL, Curl_bufref_ptr(&तथ्य->state.url),
                        (अचिन्हित पूर्णांक)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (तथ्य->set.disallow_username_in_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (तथ्य->set.path_as_is ? CURLU_PATH_AS_IS : 0));
      अगर(uc) {
        failf(तथ्य, "URL rejected: %s", curl_url_strerror(uc));
        परिणाम = Curl_uc_to_curlcode(uc);
        जाओ out;
      }

      अगर(CURL_EASY_STR(तथ्य, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        अक्षर *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        अगर(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        अगर(uc && uc != CURLUE_NO_SCHEME) {
          परिणाम = Curl_uc_to_curlcode(uc);
          जाओ out;
        }
      }
    }

    अगर(prepend_sheme) {
      अक्षर *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(तथ्य, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&तथ्य->state.url));
      अगर(!url) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      uc = curl_url_set(uh, CURLUPART_URL, url,
                        (अचिन्हित पूर्णांक)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (तथ्य->set.disallow_username_in_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (तथ्य->set.path_as_is ? CURLU_PATH_AS_IS : 0));
      curl_free(url);
      अगर(uc) {
        failf(तथ्य, "URL rejected: %s", curl_url_strerror(uc));
        परिणाम = Curl_uc_to_curlcode(uc);
        जाओ out;
      }
    }
    {
      अक्षर *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_EMPTY);
      अगर(uc) {
        परिणाम = Curl_uc_to_curlcode(uc);
        जाओ out;
      }
      अगर(prepend_sheme)
        infof(तथ्य, "Applied default protocol '%s'",
              CURL_EASY_STR(तथ्य, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_set(&तथ्य->state.url, newurl, 0, curl_free);
    }
  }

#अगर_घोषित USE_IPV6
  scope_id = तथ्य->set.scope_id;
#पूर्ण_अगर

  /* `uh` is now as the connection should use it, probably. */
  परिणाम = Curl_peer_from_url(uh, तथ्य, port_override, scope_id,
                              &तथ्य->state.origin);
  अगर(परिणाम)
    जाओ out;
  /* The origin might get changed when HSTS applies */
  परिणाम = hsts_upgrade(तथ्य, uh, port_override, scope_id);
  अगर(परिणाम)
    जाओ out;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  अगर(!तथ्य->state.initial_origin)
    Curl_peer_link(&तथ्य->state.initial_origin, तथ्य->state.origin);

  uc = curl_url_get(uh, CURLUPART_PATH, &तथ्य->state.up.path, CURLU_URLENCODE);
  अगर(uc) {
    परिणाम = Curl_uc_to_curlcode(uc);
    जाओ out;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &तथ्य->state.up.query,
                    CURLU_GET_EMPTY);
  अगर(uc && (uc != CURLUE_NO_QUERY)) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &तथ्य->state.up.options,
                    CURLU_URLDECODE);
  अगर(uc && (uc != CURLUE_NO_OPTIONS)) {
    परिणाम = Curl_uc_to_curlcode(uc);
    जाओ out;
  }

  परिणाम = url_set_तथ्य_creds(तथ्य, uh);
  अगर(परिणाम)
    जाओ out;

out:
  वापस परिणाम;
}

अटल CURLcode url_match_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *needle,
                               काष्ठा url_conn_match *m)
{
  स_रखो(m, 0, माप(*m));
  m->तथ्य = तथ्य;
  m->needle = needle;
  m->now = *Curl_pgrs_now(तथ्य);
  m->may_multiplex = xfer_may_multiplex(तथ्य, needle);

#अगर_घोषित USE_NTLM
  m->want_ntlm_http =
    (तथ्य->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_to_host(तथ्य);
#अगर_अघोषित CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (तथ्य->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#पूर्ण_अगर
#पूर्ण_अगर

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (तथ्य->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_to_host(तथ्य);
#अगर_अघोषित CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (तथ्य->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#पूर्ण_अगर
#पूर्ण_अगर
  m->require_tls = तथ्य->set.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = तथ्य->set.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  वापस Curl_ssl_filter_config_tmp_init(तथ्य, needle->origin,
                                         &m->ssl_config,
#अगर_अघोषित CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#अन्यथा
                                         शून्य);
#पूर्ण_अगर
}

अटल व्योम url_match_destroy(काष्ठा url_conn_match *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#अगर_अघोषित CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#पूर्ण_अगर
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
अटल CURLcode url_find_or_create_conn(काष्ठा Curl_easy *तथ्य,
                                        स्थिर काष्ठा curltime *pnow)
{
  काष्ठा connectdata *needle = शून्य;
  काष्ठा url_conn_match match;
  bool match_initialized = FALSE;
  CURLcode परिणाम;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  परिणाम = url_create_needle(तथ्य, &needle);
  अगर(परिणाम)
    जाओ out;
  DEBUGASSERT(needle);

  परिणाम = url_match_init(तथ्य, needle, &match);
  अगर(परिणाम)
    जाओ out;
  match_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#अगर_अघोषित CURL_DISABLE_FILE
  अगर(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool done;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->run->connect_it);
    तथ्य->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    तथ्य->info.conn_protocol = needle->scheme->protocol & CURLPROTO_MASK;
    परिणाम = needle->scheme->run->connect_it(तथ्य, &done);
    अगर(परिणाम)
      जाओ out;

    /* Setup a "faked" transfer that will do nothing */
    परिणाम = Curl_cpool_add(तथ्य, needle, 0, 0, pnow);
    Curl_attach_connection(तथ्य, needle, TRUE);
    needle = शून्य;
    अगर(!परिणाम) {
      /* Setup whatever necessary for a resumed transfer */
      परिणाम = setup_range(तथ्य);
      अगर(!परिणाम) {
        Curl_xfer_setup_nop(तथ्य);
        परिणाम = Curl_init_transfer(तथ्य, तथ्य->conn);
      }
    }

    अगर(परिणाम) {
      DEBUGASSERT(तथ्य->conn->scheme->run->done);
      /* we ignore the return code for the protocol-specific DONE */
      (व्योम)तथ्य->conn->scheme->run->done(तथ्य, परिणाम, FALSE);
    }
    जाओ out;
  }
#पूर्ण_अगर

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  अगर((!तथ्य->set.reuse_fresh || तथ्य->state.followlocation) &&
     !तथ्य->set.connect_only) {
    /* Ok, try to find and attach an existing one */
    अगर(url_attach_existing(तथ्य, needle, &match, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      काष्ठा connectdata *conn = तथ्य->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->bits.reuse = TRUE;
      url_conn_reuse_adjust(तथ्य, needle);

#अगर_अघोषित CURL_DISABLE_PROXY
      infof(तथ्य, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#अन्यथा
      infof(तथ्य, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#पूर्ण_अगर
    }
  }

  अगर(!तथ्य->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    अगर(match.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(तथ्य, "Waiting on connection to negotiate possible multiplexing.");
      परिणाम = CURLE_NO_CONNECTION_AVAILABLE;
      जाओ out;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    परिणाम = Curl_cpool_add(तथ्य, needle,
                            तथ्य->multi->max_योग_connections,
                            तथ्य->multi->max_host_connections,
                            pnow);
    अगर(परिणाम)
      जाओ out;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(तथ्य, needle, TRUE);
    needle = शून्य;

    /* Clone the SSL filter configs into the new connection. */
    परिणाम = Curl_ssl_conn_config_clone(&match.ssl_config,
#अगर_अघोषित CURL_DISABLE_PROXY
                                        &match.proxy_ssl_config,
#अन्यथा
                                        शून्य,
#पूर्ण_अगर
                                        तथ्य->conn);
    अगर(परिणाम) {
      DEBUGF(infof(तथ्य, "Error: clone connection SSL config"));
      जाओ out;
    }

#अगर_घोषित USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    अगर((तथ्य->state.authhost.picked & CURLAUTH_NTLM) &&
       तथ्य->state.authhost.done) {
      infof(तथ्य, "NTLM picked AND auth done set, clear picked");
      तथ्य->state.authhost.picked = CURLAUTH_NONE;
      तथ्य->state.authhost.done = FALSE;
    }

    अगर((तथ्य->state.authproxy.picked & CURLAUTH_NTLM) &&
       तथ्य->state.authproxy.done) {
      infof(तथ्य, "NTLM-proxy picked AND auth done set, clear picked");
      तथ्य->state.authproxy.picked = CURLAUTH_NONE;
      तथ्य->state.authproxy.done = FALSE;
    }
#पूर्ण_अगर
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  परिणाम = Curl_init_transfer(तथ्य, तथ्य->conn);
  अगर(परिणाम)
    जाओ out;

  /* Setup whatever necessary for a resumed transfer */
  परिणाम = setup_range(तथ्य);
  अगर(परिणाम)
    जाओ out;

  /* persist the scheme and handler the transfer is using */
  तथ्य->info.conn_scheme = तथ्य->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  तथ्य->info.conn_protocol = तथ्य->conn->scheme->protocol & CURLPROTO_MASK;
  तथ्य->info.used_proxy =
#अगर_घोषित CURL_DISABLE_PROXY
    0
#अन्यथा
    (तथ्य->conn->socks_proxy.peer || तथ्य->conn->http_proxy.peer)
#पूर्ण_अगर
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  परिणाम = Curl_conn_ev_तथ्य_setup(तथ्य);

out:
  अगर(match_initialized)
    url_match_destroy(&match);
  अगर(needle)
    Curl_conn_free(तथ्य, needle);
  DEBUGASSERT(परिणाम || तथ्य->conn);
  वापस परिणाम;
}

CURLcode Curl_connect(काष्ठा Curl_easy *तथ्य, bool *pconnected)
{
  CURLcode परिणाम;
  काष्ठा connectdata *conn = शून्य;
  स्थिर काष्ठा curltime *pnow = शून्य;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&तथ्य->req, तथ्य);
  /* Determine the origin of the transfer and what credentials to use */
  परिणाम = url_set_तथ्य_origin_and_creds(तथ्य);
  अगर(परिणाम)
    जाओ out;
  अगर(!तथ्य->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(तथ्य);
  Curl_pgrsTimeWas(तथ्य, TIMER_POSTQUEUE, *pnow);
  परिणाम = url_find_or_create_conn(तथ्य, pnow);
  conn = तथ्य->conn;
  अगर(परिणाम)
    जाओ out;
  अगर(!तथ्य->conn) { /* just make really sure */
    DEBUGASSERT(0);
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  अगर(conn->bits.reuse) {
    अगर(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  अन्यथा अगर(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrsTime(तथ्य, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  अन्यथा {
    परिणाम = Curl_conn_setup(तथ्य, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    अगर(!परिणाम)
      परिणाम = Curl_headers_init(तथ्य);
    CURL_TRC_M(तथ्य, "Curl_conn_setup() -> %d", (पूर्णांक)परिणाम);
  }

out:
  अगर(परिणाम == CURLE_NO_CONNECTION_AVAILABLE)
    DEBUGASSERT(!conn);

  अगर(परिणाम && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(तथ्य);
    Curl_conn_close(तथ्य, conn, TRUE);
  }

  वापस परिणाम;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn)
{
  CURLcode परिणाम;

  अगर(conn) {
    conn->bits.do_more = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    अगर(तथ्य->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      तथ्य->state.wildcardmatch = FALSE;
  }

  तथ्य->state.done = FALSE; /* *_done() is not called yet */

  तथ्य->req.no_body = तथ्य->set.opt_no_body;
  अगर(तथ्य->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    तथ्य->state.httpreq = HTTPREQ_शीर्ष;

  परिणाम = Curl_req_start(&तथ्य->req, तथ्य);
  अगर(!परिणाम) {
    Curl_pgrsReset(तथ्य);
  }
  वापस परिणाम;
}

#अगर defined(USE_HTTP2) || defined(USE_HTTP3)

व्योम Curl_तथ्य_priority_clear_state(काष्ठा Curl_easy *तथ्य)
{
  तथ्य->state.weight = 0;
}

#पूर्ण_अगर /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_set(काष्ठा connectdata *conn, स्थिर अक्षर *कुंजी,
                            व्योम *meta_तथ्य, Curl_meta_dtor *meta_dtor)
{
  अगर(!Curl_hash_add2(&conn->meta_hash, कुंजी, म_माप(कुंजी) + 1,
                     meta_तथ्य, meta_dtor)) {
    meta_dtor(कुंजी, म_माप(कुंजी) + 1, meta_तथ्य);
    वापस CURLE_OUT_OF_MEMORY;
  }
  वापस CURLE_OK;
}

व्योम Curl_conn_meta_remove(काष्ठा connectdata *conn, स्थिर अक्षर *कुंजी)
{
  Curl_hash_delete(&conn->meta_hash, कुंजी, म_माप(कुंजी) + 1);
}

व्योम *Curl_conn_meta_get(काष्ठा connectdata *conn, स्थिर अक्षर *कुंजी)
{
  वापस Curl_hash_pick(&conn->meta_hash, कुंजी, म_माप(कुंजी) + 1);
}

काष्ठा Curl_easy *Curl_get_admin(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_easy *admin;

  अगर(!तथ्य->mid) /* already an admin handle */
    admin = तथ्य;
  अन्यथा अगर(तथ्य->multi)
    admin = तथ्य->multi->admin;
  अन्यथा अगर(तथ्य->multi_easy)
    admin = तथ्य->multi_easy->admin;
  अन्यथा {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = तथ्य;
  }
  अगर(admin != तथ्य) {
    admin->set.conn_max_idle_ms = तथ्य->set.conn_max_idle_ms;
    admin->set.conn_max_age_ms = तथ्य->set.conn_max_age_ms;
    admin->set.upkeep_interval_ms = तथ्य->set.upkeep_interval_ms;
    admin->set.timeout = तथ्य->set.timeout;
    admin->set.server_response_timeout = तथ्य->set.server_response_timeout;
    admin->set.no_signal = तथ्य->set.no_signal;
  }
  वापस admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  अगर(r1 && (r1 != CURLE_AGAIN))
    वापस r1;
  अगर(r2 && (r2 != CURLE_AGAIN))
    वापस r2;
  वापस r1;
}
