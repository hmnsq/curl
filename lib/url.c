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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#অন্তর্ভাব <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#অন্তর্ভাব <sys/un.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#যদি defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#অন্তর্ভাব <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#অন্তর্ভাব <iphlpapi.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vssh/vssh.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "cookie.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "http_digest.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "getinfo.h"
#অন্তর্ভাব "pop3.h"
#অন্তর্ভাব "urlapi-int.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "proxy.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "idn.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "conncache.h"
#অন্তর্ভাব "multihandle.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "setopt.h"
#অন্তর্ভাব "altsvc.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "headers.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "peer.h"

/* Now for the protocols */
#অন্তর্ভাব "ftp.h"
#অন্তর্ভাব "dict.h"
#অন্তর্ভাব "telnet.h"
#অন্তর্ভাব "tftp.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "file.h"
#অন্তর্ভাব "curl_ldap.h"
#অন্তর্ভাব "vssh/ssh.h"
#অন্তর্ভাব "imap.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "gopher.h"
#অন্তর্ভাব "mqtt.h"
#অন্তর্ভাব "rtsp.h"
#অন্তর্ভাব "smtp.h"
#অন্তর্ভাব "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#যদি READBUFFER_আকার < READBUFFER_MIN
# error READBUFFER_আকার is too small
#endif

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
স্থির curl_prot_t get_protocol_family(ধ্রুবক কাঠামো Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  ফেরত s->family;
}

শূন্য Curl_freeset(কাঠামো Curl_easy *তথ্য)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  গণনা dupblob j;

  CURL_EASY_STR_CLEAR0(তথ্য, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(তথ্য, STRING_চাবি_PASSWD);
  CURL_EASY_STR_CLEAR0(তথ্য, STRING_BEARER);
#ifndef CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(তথ্য, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(তথ্য, STRING_চাবি_PASSWD_PROXY);
#endif
  Curl_u8_strset_clear(&তথ্য->set.strings);
  curlx_safefree(তথ্য->set.str_copypostfields);

  জন্য(j = (গণনা dupblob)0; j < BLOB_LAST; j++) {
    curlx_safefree(তথ্য->set.blobs[j]);
  }

  Curl_bufref_free(&তথ্য->state.referer);
  Curl_bufref_free(&তথ্য->state.url);

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(তথ্য->set.mimepostp);
  curlx_safefree(তথ্য->set.mimepostp);
#endif

#ifndef CURL_DISABLE_COOKIES
  curl_slist_free_all(তথ্য->state.cookielist);
  তথ্য->state.cookielist = ফাঁকা;
#endif
}

/* free the URL pieces */
স্থির শূন্য up_free(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো urlpieces *up = &তথ্য->state.up;
  curlx_safefree(up->options);
  curlx_safefree(up->path);
  curlx_safefree(up->query);
  curl_url_cleanup(তথ্য->state.uh);
  তথ্য->state.uh = ফাঁকা;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(কাঠামো Curl_easy **datap)
{
  কাঠামো Curl_easy *তথ্য;

  যদি(!datap || !*datap)
    ফেরত CURLE_OK;

  তথ্য = *datap;
  *datap = ফাঁকা;

  যদি(!তথ্য->state.internal && তথ্য->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(তথ্য->multi, তথ্য);
  }
  অন্যথা {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(তথ্য);
    যদি(!তথ্য->state.internal && তথ্য->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(তথ্য->multi_easy);
      তথ্য->multi_easy = ফাঁকা;
    }
  }
  DEBUGASSERT(!তথ্য->conn || তথ্য->state.internal);

  Curl_expire_clear_all(তথ্য); /* shut off any timers left */

  যদি(তথ্য->state.rangestringalloc)
    curlx_free(তথ্য->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(তথ্য);

  তথ্য->set.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  তথ্য->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&তথ্য->req, তথ্য);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(তথ্য);
  Curl_peer_unlink(&তথ্য->state.origin);
  Curl_peer_unlink(&তথ্য->state.initial_origin);
#ifndef CURL_DISABLE_RTSP
  Curl_peer_unlink(&তথ্য->state.rtsp_session_origin);
#endif
  Curl_ssl_free_certinfo(তথ্য);

  Curl_bufref_free(&তথ্য->state.referer);

  up_free(তথ্য);
  curlx_dyn_free(&তথ্য->state.headerb);
  Curl_flush_cookies(তথ্য, TRUE);
#ifndef CURL_DISABLE_ALTSVC
  Curl_altsvc_save(তথ্য, তথ্য->asi, CURL_EASY_STR(তথ্য, STRING_ALTSVC));
  Curl_altsvc_cleanup(&তথ্য->asi);
#endif
#ifndef CURL_DISABLE_HSTS
  Curl_hsts_save(তথ্য, তথ্য->hsts, CURL_EASY_STR(তথ্য, STRING_HSTS));
  যদি(!তথ্য->share || !তথ্য->share->hsts)
    Curl_hsts_cleanup(&তথ্য->hsts);
  curl_slist_free_all(তথ্য->state.hstslist); /* clean up list */
#endif
#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(তথ্য);
#endif
  curlx_safefree(তথ্য->state.most_recent_ftp_entrypath);
  curlx_safefree(তথ্য->info.contenttype);
  curlx_safefree(তথ্য->info.wouldredirect);
  curlx_safefree(তথ্য->info.effective_url);

  /* No longer a dirty share, if it exists */
  যদি(Curl_share_easy_unlink(তথ্য))
    DEBUGASSERT(0);

  Curl_hash_destroy(&তথ্য->meta_hash);
  Curl_creds_unlink(&তথ্য->state.creds);
#ifndef CURL_DISABLE_HTTP
  curlx_safefree(তথ্য->state.rangeline);
  curlx_safefree(তথ্য->state.http_host);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(তথ্য->req.cookiehost);
#endif

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(তথ্য->state.formp);
  curlx_safefree(তথ্য->state.formp);
#endif

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&তথ্য->wildcard);
  Curl_freeset(তথ্য);
  Curl_headers_cleanup(তথ্য);
  Curl_netrc_cleanup(&তথ্য->state.netrc);
#ifndef CURL_DISABLE_DIGEST_AUTH
  curlx_free(তথ্য->state.envproxy);
#endif
  curlx_memzero(তথ্য, আকার(*তথ্য));
  curlx_free(তথ্য);
  ফেরত CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
শূন্য Curl_init_userdefined(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো UserDefined *set = &তথ্য->set;

  set->out = stdout;  /* default output to stdout */
  set->in_set = stdin;  /* default input from stdin */
  set->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&তথ্য->set.strings);

#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  /* use fwrite as default function to store output */
  set->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  set->fread_func_set = (curl_read_callback)fread;
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
  set->is_fread_set = 0;

  set->seek_client = ZERO_NULL;

  set->filesize = -1;        /* we do not know the size */
  set->postfieldsize = -1;   /* unknown size */
  set->maxredirs = 30;       /* sensible default */

  set->method = HTTPREQ_GET; /* Default HTTP request */
#ifndef CURL_DISABLE_RTSP
  set->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#endif
#ifndef CURL_DISABLE_FTP
  set->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  set->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  set->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  set->ftp_filemethod = FTPFILE_MULTICWD;
  set->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#endif
  set->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  set->ssl_ca_cache_timeout = 24 * 60 * 60;

  set->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&তথ্য->set.ssl);
#ifndef CURL_DISABLE_PROXY
  Curl_ssl_config_init(&তথ্য->set.proxy_ssl);
  set->proxyport = 0;
  set->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  set->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  set->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#endif

#ifndef CURL_DISABLE_DOH
  set->doh_verifyhost = TRUE;
  set->doh_verifypeer = TRUE;
#endif
#ifdef USE_SSH
  /* defaults to any auth type */
  set->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  set->new_directory_perms = 0755; /* Default permissions */
#endif

  set->new_file_perms = 0644;    /* Default permissions */
  set->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  set->redir_protocols = CURLPROTO_REDIR;

#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  set->socks5_gssapi_nec = FALSE;
#endif

  /* set default minimum TLS version */
#ifdef USE_SSL
  Curl_setopt_SSLVERSION(তথ্য, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#ifndef CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(তথ্য, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#endif
#endif
#ifndef CURL_DISABLE_FTP
  set->wildcard_enabled = FALSE;
  set->chunk_bgn = ZERO_NULL;
  set->chunk_end = ZERO_NULL;
  set->fnmatch = ZERO_NULL;
#endif
  set->tcp_keepalive = FALSE;
  set->tcp_keepintvl = 60;
  set->tcp_keepidle = 60;
  set->tcp_keepcnt = 9;
  set->tcp_fastopen = FALSE;
  set->tcp_nodelay = TRUE;
  set->ssl_enable_alpn = TRUE;
  set->expect_100_timeout = 1000L; /* Wait for a second by default. */
  set->sep_headers = TRUE; /* separated header lists by default */
  set->বাফার_আকার = READBUFFER_আকার;
  set->upload_বাফার_আকার = UPLOADBUFFER_DEFAULT;
  set->upload_flags = CURLULFLAG_SEEN;
  set->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  set->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  set->maxconnects = DEFAULT_CONNCACHE_আকার; /* for easy handles */
  set->conn_max_idle_ms = 118 * 1000;
  set->conn_max_age_ms = 24 * 3600 * 1000;
  set->http09_allowed = FALSE;
  set->httpwant = CURL_HTTP_VERSION_NONE;
#যদি defined(USE_HTTP2) || defined(USE_HTTP3)
  set->weight = 0;
#endif
  set->quick_exit = 0L;
#ifndef CURL_DISABLE_WEBSOCKETS
  set->ws_raw_mode = FALSE;
  set->ws_no_auto_pong = FALSE;
#endif
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
স্থির শূন্য easy_meta_freeentry(শূন্য *p)
{
  (শূন্য)p;
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
CURLcode Curl_open(কাঠামো Curl_easy **curl)
{
  কাঠামো Curl_easy *তথ্য;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  তথ্য = curlx_calloc(1, আকার(কাঠামো Curl_easy));
  যদি(!তথ্য) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  তথ্য->magic = CURLEASY_MAGIC_সংখ্যা;
  /* most recent connection is not yet defined */
  তথ্য->state.last_conn_id = -1;
  তথ্য->state.last_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  তথ্য->id = -1;
  তথ্য->mid = UINT32_MAX;
  তথ্য->master_mid = UINT32_MAX;
  তথ্য->progress.hide = TRUE;

  Curl_hash_init(&তথ্য->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_LAST <= UINT8_MAX);
  Curl_u8_strset_init(&তথ্য->set.strings);
  curlx_dyn_init(&তথ্য->state.headerb, CURL_MAX_HTTP_HEADER);
  Curl_bufref_init(&তথ্য->state.url);
  Curl_bufref_init(&তথ্য->state.referer);
  Curl_req_init(&তথ্য->req);
  Curl_initinfo(তথ্য);
#ifndef CURL_DISABLE_HTTP
  Curl_llist_init(&তথ্য->state.httphdrs, ফাঁকা);
#endif
  Curl_netrc_init(&তথ্য->state.netrc);
  Curl_init_userdefined(তথ্য);

  *curl = তথ্য;
  ফেরত CURLE_OK;
}

শূন্য Curl_conn_free(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  যদি(conn->scheme && conn->scheme->run->disconnect &&
     !conn->bits.shutdown_handler)
    conn->scheme->run->disconnect(তথ্য, conn, TRUE);

  জন্য(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(তথ্য, conn, i);
  }

#ifndef CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#endif
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
স্থির bool xfer_may_multiplex(ধ্রুবক কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক কাঠামো connectdata *conn)
{
#ifndef CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  যদি((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->bits.protoconnstart || !conn->bits.close)) {

    যদি(Curl_multiplex_wanted(তথ্য->multi) &&
       (তথ্য->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      ফেরত TRUE;
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)conn;
#endif
  ফেরত FALSE;
}

#ifndef CURL_DISABLE_PROXY
স্থির bool proxy_info_matches(ধ্রুবক কাঠামো proxy_info *তথ্য,
                               ধ্রুবক কাঠামো proxy_info *needle)
{
  যদি((তথ্য->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(তথ্য->peer, needle->peer) &&
     Curl_creds_same(তথ্য->creds, needle->creds)) {
    ফেরত TRUE;
  }
  ফেরত FALSE;
}
#endif

#ifdef USE_SSH
স্থির bool ssh_config_matches(কাঠামো connectdata *one,
                               কাঠামো connectdata *two)
{
  কাঠামো ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  ফেরত sshc1 && sshc2 && Curl_safecmp(sshc1->priv_চাবি, sshc2->priv_চাবি) &&
         Curl_safecmp(sshc1->pub_চাবি, sshc2->pub_চাবি);
}
#endif

কাঠামো url_conn_match {
  কাঠামো connectdata *found;
  কাঠামো Curl_easy *তথ্য;
  কাঠামো connectdata *needle;
  কাঠামো curltime now;
  কাঠামো ssl_filter_config ssl_config;
#ifndef CURL_DISABLE_PROXY
  কাঠামো ssl_filter_config proxy_ssl_config;
#endif
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

স্থির bool url_match_connect_config(কাঠামো connectdata *conn,
                                     কাঠামো url_conn_match *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  যদি(conn->bits.connect_only || conn->bits.close || conn->bits.no_reuse)
    ফেরত FALSE;

  /* ip_version must match */
  যদি(m->তথ্য->set.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->তথ্য->set.ipver != conn->ip_version)
    ফেরত FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  যদি(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    ফেরত FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  যদি((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    ফেরত FALSE;

  যদি((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    ফেরত FALSE;

  যদি(!m->needle->via_peer != !conn->via_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    ফেরত FALSE;

  ফেরত TRUE;
}

স্থির bool url_match_fully_connected(কাঠামো connectdata *conn,
                                      কাঠামো url_conn_match *m)
{
  যদি(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->bits.upgrade_in_progress) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    যদি(m->may_multiplex) {
      m->seen_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->তথ্য, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির bool url_match_multi(কাঠামো connectdata *conn,
                            কাঠামো url_conn_match *m)
{
  যদি(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    যদি(conn->attached_multi != m->তথ্য->multi)
      ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির bool url_match_multiplex_needs(কাঠামো connectdata *conn,
                                      কাঠামো url_conn_match *m)
{
  যদি(CONN_INUSE(conn)) {
    যদি(!conn->bits.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->seen_single_use_conn = TRUE;
      ফেরত FALSE;
    }
    m->seen_multiplex_conn = TRUE;
    যদি(!m->may_multiplex || !url_match_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির bool url_match_multiplex_limits(কাঠামো connectdata *conn,
                                       কাঠামো url_conn_match *m)
{
  যদি(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->bits.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    যদি(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->তথ্য->multi)) {
      infof(m->তথ্য, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      ফেরত FALSE;
    }
    যদি(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->তথ্য, conn, FIRSTSOCKET)) {
      infof(m->তথ্য, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      ফেরত FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->তথ্য, "Multiplexed connection found");
  }
  ফেরত TRUE;
}

স্থির bool url_match_ssl_use(কাঠামো connectdata *conn,
                              কাঠামো url_conn_match *m)
{
  যদি(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ফেরত FALSE;
  }
  অন্যথা যদি(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    যদি(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      ফেরত FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    যদি(!Curl_ssl_conn_config_match(m->তথ্য, &m->ssl_config, conn, FALSE))
      ফেরত FALSE;
  }
  অন্যথা যদি(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    ফেরত FALSE;
  ফেরত TRUE;
}

#ifndef CURL_DISABLE_PROXY
স্থির bool url_match_proxy_use(কাঠামো connectdata *conn,
                                কাঠামো url_conn_match *m)
{
  যদি(m->needle->bits.origin_is_proxy != conn->bits.origin_is_proxy)
    ফেরত FALSE;

  যদি(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    ফেরত FALSE;

  যদি(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    ফেরত FALSE;

  যদি(m->তথ্য->set.socks5_auth_only && !conn->bits.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->তথ্য,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    ফেরত FALSE;
  }

  যদি(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    যদি(!Curl_ssl_conn_config_match(m->তথ্য, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->তথ্য,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      ফেরত FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  ফেরত TRUE;
}
#অন্যথা
#define url_match_proxy_use(c, m) ((শূন্য)(c), (শূন্য)(m), TRUE)
#endif

#ifndef CURL_DISABLE_HTTP
স্থির bool url_match_http_multiplex(কাঠামো connectdata *conn,
                                     কাঠামো url_conn_match *m,
                                     bool *pwait_pipe)
{
  যদি(m->may_multiplex &&
     (m->তথ্য->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_seen) {
    যদি(m->তথ্য->set.pipewait) {
      infof(m->তথ্য, "Server upgrade does not support multiplex yet, wait");
      m->found = ফাঁকা;
      *pwait_pipe = TRUE;
      ফেরত TRUE; /* stop searching, we want to wait */
    }
    infof(m->তথ্য, "Server upgrade cannot be used");
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

স্থির bool url_match_http_version(কাঠামো connectdata *conn,
                                   কাঠামো url_conn_match *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  যদি((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    নির্বাচন(Curl_conn_http_version(m->তথ্য, conn)) {
    ক্ষেত্র 30:
      যদি(!(m->তথ্য->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->তথ্য, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        ফেরত FALSE;
      }
      বিরতি;
    ক্ষেত্র 20:
      যদি(!(m->তথ্য->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->তথ্য, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        ফেরত FALSE;
      }
      বিরতি;
    পূর্বনির্ধারিত:
      যদি(!(m->তথ্য->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->তথ্য, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        ফেরত FALSE;
      }
      বিরতি;
    }
  }
  ফেরত TRUE;
}
#অন্যথা
#define url_match_http_multiplex(c, m, w) ((শূন্য)(c), (শূন্য)(m), TRUE)
#define url_match_http_version(c, m)      ((শূন্য)(c), (শূন্য)(m), TRUE)
#endif

স্থির bool url_match_proto_config(কাঠামো connectdata *conn,
                                   কাঠামো url_conn_match *m)
{
  যদি(!url_match_http_version(conn, m))
    ফেরত FALSE;

#ifdef USE_SSH
  যদি(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    যদি(!ssh_config_matches(m->needle, conn))
      ফেরত FALSE;
  }
#endif
#ifndef CURL_DISABLE_FTP
  অন্যথা যদি(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    যদি(!Curl_ftp_conns_match(m->needle, conn))
      ফেরত FALSE;
  }
#endif
  ফেরত TRUE;
}

স্থির bool url_match_auth(কাঠামো connectdata *conn,
                           কাঠামো url_conn_match *m)
{
  যদি(!Curl_creds_same(m->needle->creds, conn->creds)) {
    যদি(m->needle->creds)
      ফেরত FALSE;
    যদি(!Curl_creds_same(m->তথ্য->state.creds, conn->creds))
      ফেরত FALSE;
  }
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  যদি(m->needle->gssapi_delegation != conn->gssapi_delegation)
    ফেরত FALSE;
#endif

  ফেরত TRUE;
}

স্থির bool url_match_destination(কাঠামো connectdata *conn,
                                  কাঠামো url_conn_match *m)
{
  /* Different connect-to peers never match */
  যদি(!Curl_peer_same_destination(m->needle->via_peer, conn->via_peer))
    ফেরত FALSE;

  যদি(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    ফেরত FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  ফেরত Curl_peer_same_destination(m->needle->origin, conn->origin);
}

স্থির bool url_match_ssl_config(কাঠামো connectdata *conn,
                                 কাঠামো url_conn_match *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  যদি(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_match(m->তথ্য, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->তথ্য, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

#ifdef USE_NTLM
স্থির bool url_match_auth_ntlm(কাঠামো connectdata *conn,
                                কাঠামো url_conn_match *m)
{
  যদি(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    যদি(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->তথ্য->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->তথ্য->state.origin))
      ফেরত FALSE;
  }
  অন্যথা যদি(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    যদি(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->তথ্য->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->তথ্য->state.origin)))
      ফেরত FALSE;
  }

#ifndef CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  যদি(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    যদি(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ফেরত FALSE;
  }
  অন্যথা যদি(m->want_proxy_ntlm_http) {
    যদি(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ফেরত FALSE;
  }
#endif
  ফেরত TRUE;
}
#অন্যথা
#define url_match_auth_ntlm(c, m) ((শূন্য)(c), (শূন্য)(m), TRUE)
#endif

#ifdef USE_SPNEGO
স্থির bool url_match_auth_nego(কাঠামো connectdata *conn,
                                কাঠামো url_conn_match *m)
{
  যদি(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    যদি(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->তথ্য->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->তথ্য->state.origin))
      ফেরত FALSE;
  }
  অন্যথা যদি(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    যদি(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->তথ্য->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->তথ্য->state.origin)))
      ফেরত FALSE;
  }

#ifndef CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  যদি(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    যদি(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ফেরত FALSE;
  }
  অন্যথা যদি(m->want_proxy_nego_http) {
    যদি(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ফেরত FALSE;
  }
#endif
  ফেরত TRUE;
}
#অন্যথা
#define url_match_auth_nego(c, m) ((শূন্য)(c), (শূন্য)(m), TRUE)
#endif

স্থির cpool_match_ফলাফল url_match_conn(কাঠামো connectdata *conn,
                                         শূন্য *userdata)
{
  কাঠামো url_conn_match *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  যদি(!url_match_connect_config(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  /* match for destination and protocol? */
  যদি(!url_match_destination(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_fully_connected(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_multiplex_needs(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_ssl_use(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_proxy_use(conn, m))
    ফেরত CPOOL_MATCH_CONT;
  যদি(!url_match_ssl_config(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_http_multiplex(conn, m, &wait_pipe))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_auth(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_proto_config(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_auth_ntlm(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_auth_nego(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  যদি(!url_match_multiplex_limits(conn, m))
    ফেরত CPOOL_MATCH_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  যদি(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->found);
    m->wait_pipe = TRUE;
    ফেরত CPOOL_MATCH_FOUND;
  }

  যদি(m->তথ্য->set.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->তথ্য, conn, &m->now);
    যদি(age_ms > m->তথ্য->set.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      ফেরত CPOOL_MATCH_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  যদি(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->তথ্য, &m->now)) {
    infof(m->তথ্য, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    ফেরত CPOOL_MATCH_CLOSE;
  }

  /* conn matches our needs. */
  m->found = conn;
  ফেরত CPOOL_MATCH_FOUND;
}

স্থির bool url_match_ফলাফল(শূন্য *userdata)
{
  কাঠামো url_conn_match *match = userdata;
  যদি(match->found) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(match->তথ্য, match->found, TRUE);
    ফেরত TRUE;
  }
  অন্যথা যদি(match->seen_single_use_conn && !match->seen_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    match->wait_pipe = FALSE;
  }
  অন্যথা যদি(match->seen_pending_conn && match->তথ্য->set.pipewait) {
    infof(match->তথ্য,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    match->wait_pipe = TRUE;
  }
  ফেরত FALSE;
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
স্থির bool url_attach_existing(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *needle,
                                কাঠামো url_conn_match *m,
                                ধ্রুবক কাঠামো curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  ফেরত Curl_cpool_find(তথ্য, needle->destination, TRUE, pnow,
                         url_match_conn, url_match_ফলাফল, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
স্থির কাঠামো connectdata *allocate_conn(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = curlx_calloc(1, আকার(কাঠামো connectdata));
  যদি(!conn)
    ফেরত ফাঁকা;

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

#ifndef CURL_DISABLE_FTP
  conn->bits.ftp_use_epsv = তথ্য->set.ftp_use_epsv;
  conn->bits.ftp_use_eprt = তথ্য->set.ftp_use_eprt;
#endif
  conn->ip_version = তথ্য->set.ipver;
  conn->bits.connect_only = (bool)তথ্য->set.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  যদি(CURL_EASY_STR(তথ্য, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(তথ্য, STRING_INTERFACE_OPT));
    যদি(!conn->interface_opts)
      যাও error;
  }
#ifndef CURL_DISABLE_BINDLOCAL
  conn->localportrange = তথ্য->set.localportrange;
  conn->localport = তথ্য->set.localport;
#endif

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = তথ্য->set.fclosesocket;
  conn->closesocket_client = তথ্য->set.closesocket_client;
#যদি defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = তথ্য->set.gssapi_delegation;
#endif
  ফেরত conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  ফেরত ফাঁকা;
}

স্থির CURLcode url_set_conn_scheme(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ধ্রুবক কাঠামো Curl_scheme *scheme = conn->origin->scheme;
  যদি(scheme->run &&
     (তথ্য->set.allowed_protocols & scheme->protocol) &&
     (!তথ্য->state.this_is_a_follow ||
       (তথ্য->set.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    ফেরত CURLE_OK;
  }
  যদি(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(তথ্য, "Protocol \"%s\" is not for transfers", scheme->name);
  অন্যথা
    failf(তথ্য, "Protocol \"%s\" is disabled%s", scheme->name,
          তথ্য->state.this_is_a_follow ? " (in redirect)" : "");
  ফেরত CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_to_curlcode(CURLUcode uc)
{
  নির্বাচন(uc) {
  পূর্বনির্ধারিত:
    ফেরত CURLE_URL_MALFORMAT;
  ক্ষেত্র CURLUE_UNSUPPORTED_SCHEME:
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  ক্ষেত্র CURLUE_OUT_OF_MEMORY:
    ফেরত CURLE_OUT_OF_MEMORY;
  ক্ষেত্র CURLUE_USER_NOT_ALLOWED:
    ফেরত CURLE_LOGIN_DENIED;
  }
}

#ifndef CURL_DISABLE_HSTS
স্থির CURLcode hsts_upgrade(কাঠামো Curl_easy *তথ্য,
                             CURLU *uh,
                             uint16_t port_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  যদি(তথ্য->hsts && (তথ্য->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(তথ্য->hsts, তথ্য->state.origin)) {
    অক্ষর *url;
    CURLUcode uc;
    CURLcode ফলাফল;

    uc = curl_url_set(uh, CURLUPART_SCHEME, "https", 0);
    যদি(uc)
      ফেরত Curl_uc_to_curlcode(uc);
    Curl_bufref_free(&তথ্য->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    যদি(uc)
      ফেরত Curl_uc_to_curlcode(uc);
    Curl_bufref_set(&তথ্য->state.url, url, 0, curl_free);

    ফলাফল = Curl_peer_from_url(uh, তথ্য, port_override, scope_id,
                                &তথ্য->state.origin);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    infof(তথ্য, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  ফেরত CURLE_OK;
}
#অন্যথা
#define hsts_upgrade(x, y, z, a) CURLE_OK
#endif

স্থির bool str_has_ctrl(ধ্রুবক অক্ষর *প্রবেশ)
{
  যদি(প্রবেশ) {
    ধ্রুবক unsigned অক্ষর *str = (ধ্রুবক unsigned অক্ষর *)প্রবেশ;
    যখন(*str) {
      যদি(*str < 0x20)
        ফেরত TRUE;
      str++;
    }
  }
  ফেরত FALSE;
}

#ifndef CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
স্থির CURLcode url_set_তথ্য_creds_netrc(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো Curl_creds **pcreds)
{
  কাঠামো Curl_creds *ncreds_out = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(তথ্য->set.use_netrc) { /* not CURL_NETRC_IGNORED */
    কাঠামো Curl_creds *ncreds_in = ফাঁকা;
    bool scan_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    যদি(*pcreds) {
      নির্বাচন((*pcreds)->source) {
      ক্ষেত্র CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        scan_netrc = FALSE;
        বিরতি;
      ক্ষেত্র CREDS_URL: /* only apply when netrc is not required */
        যদি(তথ্য->set.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_in = *pcreds;
        }
        অন্যথা যদি(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_in = *pcreds;
        }
        অন্যথা
          scan_netrc = FALSE;
        বিরতি;
      পূর্বনির্ধারিত: /* ignore credentials from other sources */
        বিরতি;
      }
    }

    যদি(!scan_netrc)
      যাও out;

    ret = Curl_netrc_scan(তথ্য, &তথ্য->state.netrc,
                          তথ্য->state.origin->hostname,
                          Curl_creds_user(ncreds_in),
                          CURL_EASY_STR(তথ্য, STRING_NETRC_FILE),
                          &ncreds_out);
    DEBUGASSERT(!ret || !ncreds_out);
    যদি(ret == NETRC_OUT_OF_MEMORY) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
    অন্যথা যদি(ret && ((ret == NETRC_NO_MATCH) ||
                    (তথ্য->set.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(তথ্য, "Could not find host %s in the %s file; using defaults",
            তথ্য->state.origin->hostname,
            (CURL_EASY_STR(তথ্য, STRING_NETRC_FILE) ?
             CURL_EASY_STR(তথ্য, STRING_NETRC_FILE) : ".netrc"));
    }
    অন্যথা যদি(ret) {
      ধ্রুবক অক্ষর *m = Curl_netrc_strerror(ret);
      failf(তথ্য, ".netrc error: %s", m);
      ফলাফল = CURLE_READ_ERROR;
      যাও out;
    }
    অন্যথা যদি(ncreds_out) {
      যদি(!(তথ্য->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_out->user) ||
          str_has_ctrl(ncreds_out->passwd))) {
        failf(তথ্য, "control code detected in .netrc credentials");
        ফলাফল = CURLE_READ_ERROR;
        যাও out;
      }
      CURL_TRC_M(তথ্য, "netrc: using credentials for %s as %s",
                 তথ্য->state.origin->hostname, ncreds_out->user);
      ফলাফল = Curl_creds_merge(ncreds_out->user, ncreds_out->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      যদি(ফলাফল)
        যাও out;
      /* for updated strings, we update them in the URL */
      uc = curl_url_set(তথ্য->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      যদি(!uc)
        uc = curl_url_set(তথ্য->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      যদি(uc)
        ফলাফল = Curl_uc_to_curlcode(uc);
    }
    অন্যথা
      DEBUGASSERT(0);
  }

#ifdef CURLVERBOSE
  Curl_creds_trace(তথ্য, তথ্য->state.creds, "transfer credentials");
#endif

out:
  Curl_creds_unlink(&ncreds_out);
  ফেরত ফলাফল;
}
#endif /* CURL_DISABLE_NETRC */

স্থির CURLcode url_set_তথ্য_creds(কাঠামো Curl_easy *তথ্য, CURLU *uh)
{
  কাঠামো Curl_creds *newcreds = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি((CURL_EASY_STR(তথ্য, STRING_USERNAME) ||
      CURL_EASY_STR(তথ্য, STRING_PASSWORD) ||
      CURL_EASY_STR(তথ্য, STRING_BEARER) ||
      CURL_EASY_STR(তথ্য, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(তথ্য, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_to_origin(তথ্য, তথ্য->state.origin)) {
    ফলাফল = Curl_creds_create(CURL_EASY_STR(তথ্য, STRING_USERNAME),
                               CURL_EASY_STR(তথ্য, STRING_PASSWORD),
                               CURL_EASY_STR(তথ্য, STRING_BEARER),
                               CURL_EASY_STR(তথ্য, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(তথ্য, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    যদি(ফলাফল)
      যাও out;
    যদি(newcreds &&
       !(তথ্য->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(তথ্য, "control code detected in credentials");
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  যদি(!newcreds || !Curl_creds_has_user(newcreds)) {
    অক্ষর *user = ফাঁকা;
    অক্ষর *passwd = ফাঁকা;
    অক্ষর *udecoded = ফাঁকা;
    অক্ষর *pdecoded = ফাঁকা;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    যদি(uc && (uc != CURLUE_NO_USER))
      ফলাফল = Curl_uc_to_curlcode(uc);
    যদি(!ফলাফল) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      যদি(uc && (uc != CURLUE_NO_PASSWORD))
        ফলাফল = Curl_uc_to_curlcode(uc);
    }
    যদি(!ফলাফল && user) {
      ফলাফল = Curl_urldecode(user, 0, &udecoded, ফাঁকা,
                              (তথ্য->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    যদি(!ফলাফল && passwd) {
      ফলাফল = Curl_urldecode(passwd, 0, &pdecoded, ফাঁকা,
                              (তথ্য->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    যদি(!ফলাফল)
      ফলাফল = Curl_creds_merge(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    যদি(ফলাফল) {
      failf(তথ্য, "error extracting credentials from URL");
      যাও out;
    }
  }

#ifndef CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  ফলাফল = url_set_তথ্য_creds_netrc(তথ্য, &newcreds);
#endif /* CURL_DISABLE_NETRC */

out:
  যদি(!ফলাফল && !Curl_creds_equal(তথ্য->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&তথ্য->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  ফেরত ফলাফল;
}

স্থির CURLcode url_set_conn_origin_etc(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;

  Curl_peer_link(&conn->origin, তথ্য->state.origin);

  /* set the connection scheme */
  ফলাফল = url_set_conn_scheme(তথ্য, conn);
  যদি(ফলাফল)
    যাও out;

  /* set the connection options */
  যদি(CURL_EASY_STR(তথ্য, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(তথ্য, STRING_OPTIONS));
    যদি(!conn->options) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }
  অন্যথা যদি(তথ্য->state.up.options) {
    conn->options = curlx_strdup(তথ্য->state.up.options);
    যদি(!conn->options) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

out:
  ফেরত ফলাফল;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
স্থির CURLcode setup_range(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো UrlState *s = &তথ্য->state;
  s->resume_from = তথ্য->set.set_resume_from;
  যদি(s->resume_from || CURL_EASY_STR(তথ্য, STRING_SET_RANGE)) {
    যদি(s->rangestringalloc)
      curlx_free(s->range);

    যদি(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    অন্যথা
      s->range = curlx_strdup(CURL_EASY_STR(তথ্য, STRING_SET_RANGE));

    যদি(!s->range)
      ফেরত CURLE_OUT_OF_MEMORY;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  অন্যথা
    s->use_range = FALSE; /* disable range download */

  ফেরত CURLE_OK;
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
স্থির CURLcode setup_connection_internals(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো connectdata *conn)
{
  কাঠামো Curl_peer *peer = ফাঁকা;

  যদি(conn->scheme->run->setup_connection) {
    CURLcode ফলাফল = conn->scheme->run->setup_connection(তথ্য, conn);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  যদি(!peer)
    ফেরত CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  যদি(peer->ipv6)
    যদি(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->port);
    অন্যথা
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->port);
  অন্যথা
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->port);
  যদি(!conn->destination)
    ফেরত CURLE_OUT_OF_MEMORY;

  Curl_strntolower(conn->destination, conn->destination,
                   strlen(conn->destination));

#ifdef USE_IPV6
  যদি(তথ্য->set.scope_id)
    conn->scope_id = তথ্য->set.scope_id;
  অন্যথা {
    peer = Curl_conn_get_first_peer(conn, FIRSTSOCKET);
    যদি(!peer)
      ফেরত CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#endif

  ফেরত CURLE_OK;
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
CURLcode Curl_parse_login_details(ধ্রুবক অক্ষর *login, ধ্রুবক size_t len,
                                  অক্ষর **userp, অক্ষর **passwdp,
                                  অক্ষর **optionsp)
{
  অক্ষর *ubuf = ফাঁকা;
  অক্ষর *pbuf = ফাঁকা;
  ধ্রুবক অক্ষর *psep = ফাঁকা;
  ধ্রুবক অক্ষর *osep = ফাঁকা;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  যদি(optionsp)
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
  যদি(!ubuf)
    যাও error;

  /* Clone the password portion buffer */
  যদি(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    যদি(!pbuf)
      যাও error;
  }

  /* Allocate the options portion buffer */
  যদি(optionsp) {
    অক্ষর *obuf = ফাঁকা;
    যদি(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      যদি(!obuf)
        যাও error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  ফেরত CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  ফেরত CURLE_OUT_OF_MEMORY;
}

/*
 * Set the login details so they are available in the connection
 */
স্থির CURLcode url_set_conn_login(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  যদি((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, তথ্য->state.origin);
    যদি(তথ্য->state.creds)
      Curl_creds_link(&conn->creds, তথ্য->state.creds);
    অন্যথা
      ফেরত Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               ফাঁকা, ফাঁকা, ফাঁকা, CREDS_NONE, &conn->creds);
  }
  অন্যথা যদি(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, তথ্য->state.origin);
    Curl_creds_link(&conn->creds, তথ্য->state.creds);
  }

  ফেরত CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
স্থির CURLcode parse_connect_to_string(কাঠামো Curl_easy *তথ্য,
                                        ধ্রুবক কাঠামো Curl_peer *dest,
                                        ধ্রুবক অক্ষর *conn_to_সারি,
                                        কাঠামো Curl_peer **pvia_dest)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ptr = conn_to_সারি;
  bool host_match = FALSE;
  bool port_match = FALSE;

  *pvia_dest = ফাঁকা;

  যদি(*ptr == ':') {
    /* an empty hostname always matches */
    host_match = TRUE;
    ptr++;
  }
  অন্যথা {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = strlen(dest->hostname);
    host_match = curl_strnequal(ptr, dest->hostname, hlen);
    যদি(!host_match && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = strlen(dest->user_hostname);
      host_match = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_match = host_match && ptr[hlen] == ':';
    যদি(host_match)
      ptr += hlen + 1;
  }

  যদি(host_match) {
    যদি(*ptr == ':') {
      /* an empty port always matches */
      port_match = TRUE;
      ptr++;
    }
    অন্যথা {
      /* check whether the URL's port matches */
      ধ্রুবক অক্ষর *ptr_পরবর্তী = strchr(ptr, ':');
      যদি(ptr_পরবর্তী) {
        curl_off_t port_to_match;
        যদি(!curlx_str_সংখ্যা(&ptr, &port_to_match, 0xffff) &&
           ((uint16_t)port_to_match == dest->port)) {
          port_match = TRUE;
        }
        ptr = ptr_পরবর্তী + 1;
      }
    }
  }

  যদি(host_match && port_match && ptr && *ptr)
    ফলাফল = Curl_peer_from_connect_to(তথ্য, dest, ptr, pvia_dest);

  ফেরত ফলাফল;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
স্থির CURLcode url_set_conn_peer(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো Curl_peer *origin = conn->origin;
  কাঠামো Curl_peer *via_peer = ফাঁকা;
  কাঠামো curl_slist *conn_to_entry = তথ্য->set.connect_to;

  DEBUGASSERT(!conn->via_peer);
  Curl_peer_unlink(&conn->via_peer);

  যখন(conn_to_entry && !via_peer) {
    ফলাফল = parse_connect_to_string(তথ্য, origin, conn_to_entry->তথ্য,
                                     &via_peer);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    conn_to_entry = conn_to_entry->পরবর্তী;
  }

#ifndef CURL_DISABLE_ALTSVC
  যদি(তথ্য->asi && !via_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#ifdef DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#অন্যথা
      0
#endif
       )) {
    /* no connect_to match, try alt-svc! */
    গণনা alpnid srcalpnid = ALPN_none;
    bool hit = FALSE;
    কাঠামো altsvc *as = ফাঁকা;
    পূর্ণসংখ্যা allowed_alpns = ALPN_none;
    কাঠামো http_negotiation *neg = &তথ্য->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(তথ্য, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#ifdef USE_HTTP3
    যদি(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#endif
#ifdef USE_HTTP2
    যদি(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#endif
    যদি(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (পূর্ণসংখ্যা)তথ্য->asi->flags;

    DEBUGF(infof(তথ্য, "check Alt-Svc for host '%s'", origin->hostname));
#ifdef USE_HTTP3
    যদি(!hit && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      hit = Curl_altsvc_lookup(তথ্য->asi,
                               origin, ALPN_h3, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }
#endif
#ifdef USE_HTTP2
    যদি(!hit && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_prior_knowledge) {
      srcalpnid = ALPN_h2;
      hit = Curl_altsvc_lookup(তথ্য->asi,
                               origin, ALPN_h2, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }
#endif
    যদি(!hit && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      hit = Curl_altsvc_lookup(তথ্য->asi,
                               origin, ALPN_h1, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }

    যদি(hit && same_dest) {
      /* same destination, but more HTTPS version options */
      নির্বাচন(as->dst.alpnid) {
      ক্ষেত্র ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        বিরতি;
      ক্ষেত্র ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        বিরতি;
      ক্ষেত্র ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        বিরতি;
      পূর্বনির্ধারিত: /* should not be possible */
        বিরতি;
      }
    }
    অন্যথা যদি(hit) {
      ফলাফল = Curl_peer_create(তথ্য, conn->origin->scheme,
                                as->dst.host, as->dst.port,
                                &via_peer);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      infof(তথ্য, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->port,
            Curl_alpnid2str(as->dst.alpnid),
            via_peer->hostname, via_peer->port);
      conn->bits.altused = TRUE;
      যদি(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        নির্বাচন(as->dst.alpnid) {
        ক্ষেত্র ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          বিরতি;
        ক্ষেত্র ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          বিরতি;
        ক্ষেত্র ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          বিরতি;
        পূর্বনির্ধারিত: /* should not be possible */
          বিরতি;
        }
      }
    }
  }
#endif

  যদি(via_peer)
    conn->via_peer = via_peer;

  ফেরত ফলাফল;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
স্থির শূন্য url_conn_reuse_adjust(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *needle)
{
  কাঠামো connectdata *conn = তথ্য->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  যদি(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#ifndef CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#endif

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

স্থির শূন্য conn_meta_freeentry(শূন্য *p)
{
  (শূন্য)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

স্থির CURLcode url_create_needle(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata **pneedle)
{
  কাঠামো connectdata *needle = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  bool network_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(তথ্য);
  যদি(!needle) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  ফলাফল = url_set_conn_origin_etc(তথ্য, needle);
  যদি(ফলাফল)
    যাও out;

  DEBUGASSERT(needle->origin);
  network_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#ifdef USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  যদি(network_scheme && CURL_EASY_STR(তথ্য, STRING_UNIX_SOCKET_PATH)) {
    ফলাফল = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(তথ্য, STRING_UNIX_SOCKET_PATH),
      (bool)তথ্য->set.abstract_unix_socket, &needle->via_peer);
    যদি(ফলাফল)
      যাও out;
  }
#endif /* USE_UNIX_SOCKETS */

  যদি(network_scheme && !needle->via_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    ফলাফল = url_set_conn_peer(তথ্য, needle);
    যদি(ফলাফল)
      যাও out;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  যদি(Curl_peer_equal(needle->origin, needle->via_peer)) {
    Curl_peer_unlink(&needle->via_peer);
  }

#ifndef CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  যদি(network_scheme &&
     (!needle->via_peer || !needle->via_peer->unix_socket)) {
    ফলাফল = Curl_proxy_init_conn(তথ্য, needle);
    যদি(ফলাফল)
      যাও out;
  }
#endif /* CURL_DISABLE_PROXY */

  ফলাফল = url_set_conn_login(তথ্য, needle); /* default credentials */
  যদি(ফলাফল)
    যাও out;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  ফলাফল = setup_connection_internals(তথ্য, needle);
  যদি(ফলাফল)
    যাও out;

  যদি(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    যদি(তথ্য->set.ssl_enable_alpn)
      needle->bits.tls_enable_alpn = TRUE;
  }

  যদি(network_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->bits.tcp_fastopen = তথ্য->set.tcp_fastopen;
#ifdef USE_UNIX_SOCKETS
    যদি(Curl_conn_get_first_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#endif
  }

out:
  যদি(!ফলাফল) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  অন্যথা {
    *pneedle = ফাঁকা;
    যদি(needle)
      Curl_conn_free(তথ্য, needle);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode url_set_তথ্য_origin_and_creds(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_set_uh = (তথ্য->set.uh && !তথ্য->state.this_is_a_follow);
  uint16_t port_override = তথ্য->state.allow_port ? তথ্য->set.use_port : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  যদি(!Curl_bufref_ptr(&তথ্য->state.url)) {
    ফলাফল = CURLE_URL_MALFORMAT;
    যাও out;
  }

  up_free(তথ্য); /* cleanup previous leftovers first */

  /* parse the URL */
  যদি(use_set_uh)
    uh = তথ্য->state.uh = curl_url_dup(তথ্য->set.uh);
  অন্যথা
    uh = তথ্য->state.uh = curl_url();
  যদি(!uh) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  যদি(!use_set_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    যদি(curl_strequal("file", CURL_EASY_STR(তথ্য, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&তথ্য->state.url), ফাঁকা, 0, TRUE))
      prepend_sheme = TRUE;
    অন্যথা {
      uc = curl_url_set(uh, CURLUPART_URL, Curl_bufref_ptr(&তথ্য->state.url),
                        (unsigned পূর্ণসংখ্যা)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (তথ্য->set.disallow_username_in_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (তথ্য->set.path_as_is ? CURLU_PATH_AS_IS : 0));
      যদি(uc) {
        failf(তথ্য, "URL rejected: %s", curl_url_strerror(uc));
        ফলাফল = Curl_uc_to_curlcode(uc);
        যাও out;
      }

      যদি(CURL_EASY_STR(তথ্য, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        অক্ষর *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        যদি(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        যদি(uc && uc != CURLUE_NO_SCHEME) {
          ফলাফল = Curl_uc_to_curlcode(uc);
          যাও out;
        }
      }
    }

    যদি(prepend_sheme) {
      অক্ষর *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(তথ্য, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&তথ্য->state.url));
      যদি(!url) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      uc = curl_url_set(uh, CURLUPART_URL, url,
                        (unsigned পূর্ণসংখ্যা)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (তথ্য->set.disallow_username_in_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (তথ্য->set.path_as_is ? CURLU_PATH_AS_IS : 0));
      curl_free(url);
      যদি(uc) {
        failf(তথ্য, "URL rejected: %s", curl_url_strerror(uc));
        ফলাফল = Curl_uc_to_curlcode(uc);
        যাও out;
      }
    }
    {
      অক্ষর *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_EMPTY);
      যদি(uc) {
        ফলাফল = Curl_uc_to_curlcode(uc);
        যাও out;
      }
      যদি(prepend_sheme)
        infof(তথ্য, "Applied default protocol '%s'",
              CURL_EASY_STR(তথ্য, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_set(&তথ্য->state.url, newurl, 0, curl_free);
    }
  }

#ifdef USE_IPV6
  scope_id = তথ্য->set.scope_id;
#endif

  /* `uh` is now as the connection should use it, probably. */
  ফলাফল = Curl_peer_from_url(uh, তথ্য, port_override, scope_id,
                              &তথ্য->state.origin);
  যদি(ফলাফল)
    যাও out;
  /* The origin might get changed when HSTS applies */
  ফলাফল = hsts_upgrade(তথ্য, uh, port_override, scope_id);
  যদি(ফলাফল)
    যাও out;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  যদি(!তথ্য->state.initial_origin)
    Curl_peer_link(&তথ্য->state.initial_origin, তথ্য->state.origin);

  uc = curl_url_get(uh, CURLUPART_PATH, &তথ্য->state.up.path, CURLU_URLENCODE);
  যদি(uc) {
    ফলাফল = Curl_uc_to_curlcode(uc);
    যাও out;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &তথ্য->state.up.query,
                    CURLU_GET_EMPTY);
  যদি(uc && (uc != CURLUE_NO_QUERY)) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &তথ্য->state.up.options,
                    CURLU_URLDECODE);
  যদি(uc && (uc != CURLUE_NO_OPTIONS)) {
    ফলাফল = Curl_uc_to_curlcode(uc);
    যাও out;
  }

  ফলাফল = url_set_তথ্য_creds(তথ্য, uh);
  যদি(ফলাফল)
    যাও out;

out:
  ফেরত ফলাফল;
}

স্থির CURLcode url_match_init(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *needle,
                               কাঠামো url_conn_match *m)
{
  memset(m, 0, আকার(*m));
  m->তথ্য = তথ্য;
  m->needle = needle;
  m->now = *Curl_pgrs_now(তথ্য);
  m->may_multiplex = xfer_may_multiplex(তথ্য, needle);

#ifdef USE_NTLM
  m->want_ntlm_http =
    (তথ্য->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_to_host(তথ্য);
#ifndef CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (তথ্য->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#endif
#endif

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (তথ্য->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_to_host(তথ্য);
#ifndef CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (তথ্য->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#endif
#endif
  m->require_tls = তথ্য->set.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = তথ্য->set.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  ফেরত Curl_ssl_filter_config_tmp_init(তথ্য, needle->origin,
                                         &m->ssl_config,
#ifndef CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#অন্যথা
                                         ফাঁকা);
#endif
}

স্থির শূন্য url_match_destroy(কাঠামো url_conn_match *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#ifndef CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#endif
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
স্থির CURLcode url_find_or_create_conn(কাঠামো Curl_easy *তথ্য,
                                        ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো connectdata *needle = ফাঁকা;
  কাঠামো url_conn_match match;
  bool match_initialized = FALSE;
  CURLcode ফলাফল;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  ফলাফল = url_create_needle(তথ্য, &needle);
  যদি(ফলাফল)
    যাও out;
  DEBUGASSERT(needle);

  ফলাফল = url_match_init(তথ্য, needle, &match);
  যদি(ফলাফল)
    যাও out;
  match_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#ifndef CURL_DISABLE_FILE
  যদি(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool done;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->run->connect_it);
    তথ্য->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    তথ্য->info.conn_protocol = needle->scheme->protocol & CURLPROTO_MASK;
    ফলাফল = needle->scheme->run->connect_it(তথ্য, &done);
    যদি(ফলাফল)
      যাও out;

    /* Setup a "faked" transfer that will do nothing */
    ফলাফল = Curl_cpool_add(তথ্য, needle, 0, 0, pnow);
    Curl_attach_connection(তথ্য, needle, TRUE);
    needle = ফাঁকা;
    যদি(!ফলাফল) {
      /* Setup whatever necessary for a resumed transfer */
      ফলাফল = setup_range(তথ্য);
      যদি(!ফলাফল) {
        Curl_xfer_setup_nop(তথ্য);
        ফলাফল = Curl_init_transfer(তথ্য, তথ্য->conn);
      }
    }

    যদি(ফলাফল) {
      DEBUGASSERT(তথ্য->conn->scheme->run->done);
      /* we ignore the return code for the protocol-specific DONE */
      (শূন্য)তথ্য->conn->scheme->run->done(তথ্য, ফলাফল, FALSE);
    }
    যাও out;
  }
#endif

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  যদি((!তথ্য->set.reuse_fresh || তথ্য->state.followlocation) &&
     !তথ্য->set.connect_only) {
    /* Ok, try to find and attach an existing one */
    যদি(url_attach_existing(তথ্য, needle, &match, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      কাঠামো connectdata *conn = তথ্য->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->bits.reuse = TRUE;
      url_conn_reuse_adjust(তথ্য, needle);

#ifndef CURL_DISABLE_PROXY
      infof(তথ্য, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#অন্যথা
      infof(তথ্য, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#endif
    }
  }

  যদি(!তথ্য->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    যদি(match.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(তথ্য, "Waiting on connection to negotiate possible multiplexing.");
      ফলাফল = CURLE_NO_CONNECTION_AVAILABLE;
      যাও out;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    ফলাফল = Curl_cpool_add(তথ্য, needle,
                            তথ্য->multi->max_যোগ_connections,
                            তথ্য->multi->max_host_connections,
                            pnow);
    যদি(ফলাফল)
      যাও out;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(তথ্য, needle, TRUE);
    needle = ফাঁকা;

    /* Clone the SSL filter configs into the new connection. */
    ফলাফল = Curl_ssl_conn_config_clone(&match.ssl_config,
#ifndef CURL_DISABLE_PROXY
                                        &match.proxy_ssl_config,
#অন্যথা
                                        ফাঁকা,
#endif
                                        তথ্য->conn);
    যদি(ফলাফল) {
      DEBUGF(infof(তথ্য, "Error: clone connection SSL config"));
      যাও out;
    }

#ifdef USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    যদি((তথ্য->state.authhost.picked & CURLAUTH_NTLM) &&
       তথ্য->state.authhost.done) {
      infof(তথ্য, "NTLM picked AND auth done set, clear picked");
      তথ্য->state.authhost.picked = CURLAUTH_NONE;
      তথ্য->state.authhost.done = FALSE;
    }

    যদি((তথ্য->state.authproxy.picked & CURLAUTH_NTLM) &&
       তথ্য->state.authproxy.done) {
      infof(তথ্য, "NTLM-proxy picked AND auth done set, clear picked");
      তথ্য->state.authproxy.picked = CURLAUTH_NONE;
      তথ্য->state.authproxy.done = FALSE;
    }
#endif
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  ফলাফল = Curl_init_transfer(তথ্য, তথ্য->conn);
  যদি(ফলাফল)
    যাও out;

  /* Setup whatever necessary for a resumed transfer */
  ফলাফল = setup_range(তথ্য);
  যদি(ফলাফল)
    যাও out;

  /* persist the scheme and handler the transfer is using */
  তথ্য->info.conn_scheme = তথ্য->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  তথ্য->info.conn_protocol = তথ্য->conn->scheme->protocol & CURLPROTO_MASK;
  তথ্য->info.used_proxy =
#ifdef CURL_DISABLE_PROXY
    0
#অন্যথা
    (তথ্য->conn->socks_proxy.peer || তথ্য->conn->http_proxy.peer)
#endif
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  ফলাফল = Curl_conn_ev_তথ্য_setup(তথ্য);

out:
  যদি(match_initialized)
    url_match_destroy(&match);
  যদি(needle)
    Curl_conn_free(তথ্য, needle);
  DEBUGASSERT(ফলাফল || তথ্য->conn);
  ফেরত ফলাফল;
}

CURLcode Curl_connect(কাঠামো Curl_easy *তথ্য, bool *pconnected)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = ফাঁকা;
  ধ্রুবক কাঠামো curltime *pnow = ফাঁকা;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&তথ্য->req, তথ্য);
  /* Determine the origin of the transfer and what credentials to use */
  ফলাফল = url_set_তথ্য_origin_and_creds(তথ্য);
  যদি(ফলাফল)
    যাও out;
  যদি(!তথ্য->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(তথ্য);
  Curl_pgrsTimeWas(তথ্য, TIMER_POSTQUEUE, *pnow);
  ফলাফল = url_find_or_create_conn(তথ্য, pnow);
  conn = তথ্য->conn;
  যদি(ফলাফল)
    যাও out;
  যদি(!তথ্য->conn) { /* just make really sure */
    DEBUGASSERT(0);
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }

  যদি(conn->bits.reuse) {
    যদি(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  অন্যথা যদি(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrsTime(তথ্য, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  অন্যথা {
    ফলাফল = Curl_conn_setup(তথ্য, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    যদি(!ফলাফল)
      ফলাফল = Curl_headers_init(তথ্য);
    CURL_TRC_M(তথ্য, "Curl_conn_setup() -> %d", (পূর্ণসংখ্যা)ফলাফল);
  }

out:
  যদি(ফলাফল == CURLE_NO_CONNECTION_AVAILABLE)
    DEBUGASSERT(!conn);

  যদি(ফলাফল && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(তথ্য);
    Curl_conn_close(তথ্য, conn, TRUE);
  }

  ফেরত ফলাফল;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn)
{
  CURLcode ফলাফল;

  যদি(conn) {
    conn->bits.do_more = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    যদি(তথ্য->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      তথ্য->state.wildcardmatch = FALSE;
  }

  তথ্য->state.done = FALSE; /* *_done() is not called yet */

  তথ্য->req.no_body = তথ্য->set.opt_no_body;
  যদি(তথ্য->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    তথ্য->state.httpreq = HTTPREQ_শীর্ষ;

  ফলাফল = Curl_req_start(&তথ্য->req, তথ্য);
  যদি(!ফলাফল) {
    Curl_pgrsReset(তথ্য);
  }
  ফেরত ফলাফল;
}

#যদি defined(USE_HTTP2) || defined(USE_HTTP3)

শূন্য Curl_তথ্য_priority_clear_state(কাঠামো Curl_easy *তথ্য)
{
  তথ্য->state.weight = 0;
}

#endif /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_set(কাঠামো connectdata *conn, ধ্রুবক অক্ষর *চাবি,
                            শূন্য *meta_তথ্য, Curl_meta_dtor *meta_dtor)
{
  যদি(!Curl_hash_add2(&conn->meta_hash, চাবি, strlen(চাবি) + 1,
                     meta_তথ্য, meta_dtor)) {
    meta_dtor(চাবি, strlen(চাবি) + 1, meta_তথ্য);
    ফেরত CURLE_OUT_OF_MEMORY;
  }
  ফেরত CURLE_OK;
}

শূন্য Curl_conn_meta_remove(কাঠামো connectdata *conn, ধ্রুবক অক্ষর *চাবি)
{
  Curl_hash_delete(&conn->meta_hash, চাবি, strlen(চাবি) + 1);
}

শূন্য *Curl_conn_meta_get(কাঠামো connectdata *conn, ধ্রুবক অক্ষর *চাবি)
{
  ফেরত Curl_hash_pick(&conn->meta_hash, চাবি, strlen(চাবি) + 1);
}

কাঠামো Curl_easy *Curl_get_admin(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_easy *admin;

  যদি(!তথ্য->mid) /* already an admin handle */
    admin = তথ্য;
  অন্যথা যদি(তথ্য->multi)
    admin = তথ্য->multi->admin;
  অন্যথা যদি(তথ্য->multi_easy)
    admin = তথ্য->multi_easy->admin;
  অন্যথা {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = তথ্য;
  }
  যদি(admin != তথ্য) {
    admin->set.conn_max_idle_ms = তথ্য->set.conn_max_idle_ms;
    admin->set.conn_max_age_ms = তথ্য->set.conn_max_age_ms;
    admin->set.upkeep_interval_ms = তথ্য->set.upkeep_interval_ms;
    admin->set.timeout = তথ্য->set.timeout;
    admin->set.server_response_timeout = তথ্য->set.server_response_timeout;
    admin->set.no_signal = তথ্য->set.no_signal;
  }
  ফেরত admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  যদি(r1 && (r1 != CURLE_AGAIN))
    ফেরত r1;
  যদি(r2 && (r2 != CURLE_AGAIN))
    ফেরত r2;
  ফেরত r1;
}
