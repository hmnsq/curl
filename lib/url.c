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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_IPHLPAPI_H
#அந்தர்பாவ <Iphlpapi.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#ifdef HAVE_SYS_UN_H
#அந்தர்பாவ <sys/un.h>
#endif

#ifndef HAVE_SOCKET
#error "We cannot compile without socket() support"
#endif

#எனில் defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#எனில் defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#அந்தர்பாவ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#endif
#அந்தர்பாவ <iphlpapi.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vssh/vssh.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "cookie.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "http_digest.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "getinfo.h"
#அந்தர்பாவ "pop3.h"
#அந்தர்பாவ "urlapi-int.h"
#அந்தர்பாவ "hsts.h"
#அந்தர்பாவ "proxy.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "idn.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "conncache.h"
#அந்தர்பாவ "multihandle.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "setopt.h"
#அந்தர்பாவ "altsvc.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "headers.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "peer.h"

/* Now for the protocols */
#அந்தர்பாவ "ftp.h"
#அந்தர்பாவ "dict.h"
#அந்தர்பாவ "telnet.h"
#அந்தர்பாவ "tftp.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "file.h"
#அந்தர்பாவ "curl_ldap.h"
#அந்தர்பாவ "vssh/ssh.h"
#அந்தர்பாவ "imap.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "gopher.h"
#அந்தர்பாவ "mqtt.h"
#அந்தர்பாவ "rtsp.h"
#அந்தர்பாவ "smtp.h"
#அந்தர்பாவ "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#எனில் READBUFFER_SIZE < READBUFFER_MIN
# error READBUFFER_SIZE is too small
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
நிலையான curl_prot_t get_protocol_family(மாறிலி கட்டமைப்பு Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  திரும்பு s->family;
}

வெறுமை Curl_freeset(கட்டமைப்பு Curl_easy *data)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  எண்ணிக்கை dupblob j;

  CURL_EASY_STR_CLEAR0(data, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(data, STRING_சாவி_PASSWD);
  CURL_EASY_STR_CLEAR0(data, STRING_BEARER);
#ifndef CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(data, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(data, STRING_சாவி_PASSWD_PROXY);
#endif
  Curl_u8_strset_clear(&data->set.strings);
  curlx_safefree(data->set.str_copypostfields);

  க்கு(j = (எண்ணிக்கை dupblob)0; j < BLOB_LAST; j++) {
    curlx_safefree(data->set.blobs[j]);
  }

  Curl_bufref_free(&data->state.referer);
  Curl_bufref_free(&data->state.url);

#எனில் !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(data->set.mimepostp);
  curlx_safefree(data->set.mimepostp);
#endif

#ifndef CURL_DISABLE_COOKIES
  curl_slist_free_all(data->state.cookielist);
  data->state.cookielist = வெற்று;
#endif
}

/* free the URL pieces */
நிலையான வெறுமை up_free(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு urlpieces *up = &data->state.up;
  curlx_safefree(up->options);
  curlx_safefree(up->path);
  curlx_safefree(up->query);
  curl_url_cleanup(data->state.uh);
  data->state.uh = வெற்று;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(கட்டமைப்பு Curl_easy **datap)
{
  கட்டமைப்பு Curl_easy *data;

  எனில்(!datap || !*datap)
    திரும்பு CURLE_OK;

  data = *datap;
  *datap = வெற்று;

  எனில்(!data->state.internal && data->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(data->multi, data);
  }
  இல்லையெனில் {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(data);
    எனில்(!data->state.internal && data->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(data->multi_easy);
      data->multi_easy = வெற்று;
    }
  }
  DEBUGASSERT(!data->conn || data->state.internal);

  Curl_expire_clear_all(data); /* shut off any timers left */

  எனில்(data->state.rangestringalloc)
    curlx_free(data->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(data);

  data->set.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  data->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&data->req, data);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(data);
  Curl_peer_unlink(&data->state.origin);
  Curl_peer_unlink(&data->state.initial_origin);
#ifndef CURL_DISABLE_RTSP
  Curl_peer_unlink(&data->state.rtsp_session_origin);
#endif
  Curl_ssl_free_certinfo(data);

  Curl_bufref_free(&data->state.referer);

  up_free(data);
  curlx_dyn_free(&data->state.headerb);
  Curl_flush_cookies(data, TRUE);
#ifndef CURL_DISABLE_ALTSVC
  Curl_altsvc_save(data, data->asi, CURL_EASY_STR(data, STRING_ALTSVC));
  Curl_altsvc_cleanup(&data->asi);
#endif
#ifndef CURL_DISABLE_HSTS
  Curl_hsts_save(data, data->hsts, CURL_EASY_STR(data, STRING_HSTS));
  எனில்(!data->share || !data->share->hsts)
    Curl_hsts_cleanup(&data->hsts);
  curl_slist_free_all(data->state.hstslist); /* clean up list */
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(data);
#endif
  curlx_safefree(data->state.most_recent_ftp_entrypath);
  curlx_safefree(data->info.contenttype);
  curlx_safefree(data->info.wouldredirect);
  curlx_safefree(data->info.effective_url);

  /* No longer a dirty share, if it exists */
  எனில்(Curl_share_easy_unlink(data))
    DEBUGASSERT(0);

  Curl_hash_destroy(&data->meta_hash);
  Curl_creds_unlink(&data->state.creds);
#ifndef CURL_DISABLE_HTTP
  curlx_safefree(data->state.rangeline);
  curlx_safefree(data->state.http_host);
#endif
#ifndef CURL_DISABLE_COOKIES
  curlx_safefree(data->req.cookiehost);
#endif

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(data->state.formp);
  curlx_safefree(data->state.formp);
#endif

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&data->wildcard);
  Curl_freeset(data);
  Curl_headers_cleanup(data);
  Curl_netrc_cleanup(&data->state.netrc);
#ifndef CURL_DISABLE_DIGEST_AUTH
  curlx_free(data->state.envproxy);
#endif
  curlx_memzero(data, அளவு(*data));
  curlx_free(data);
  திரும்பு CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
வெறுமை Curl_init_userdefined(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு UserDefined *set = &data->set;

  set->out = stdout;  /* default output to stdout */
  set->in_set = stdin;  /* default input from stdin */
  set->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&data->set.strings);

#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  /* use fwrite as default function to store output */
  set->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  set->fread_func_set = (curl_read_callback)fread;
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
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

  Curl_ssl_config_init(&data->set.ssl);
#ifndef CURL_DISABLE_PROXY
  Curl_ssl_config_init(&data->set.proxy_ssl);
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

#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  set->socks5_gssapi_nec = FALSE;
#endif

  /* set default minimum TLS version */
#ifdef USE_SSL
  Curl_setopt_SSLVERSION(data, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#ifndef CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(data, CURLOPT_PROXY_SSLVERSION,
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
  set->buffer_size = READBUFFER_SIZE;
  set->upload_buffer_size = UPLOADBUFFER_DEFAULT;
  set->upload_flags = CURLULFLAG_SEEN;
  set->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  set->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  set->maxconnects = DEFAULT_CONNCACHE_SIZE; /* for easy handles */
  set->conn_max_idle_ms = 118 * 1000;
  set->conn_max_age_ms = 24 * 3600 * 1000;
  set->http09_allowed = FALSE;
  set->httpwant = CURL_HTTP_VERSION_NONE;
#எனில் defined(USE_HTTP2) || defined(USE_HTTP3)
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
நிலையான வெறுமை easy_meta_freeentry(வெறுமை *p)
{
  (வெறுமை)p;
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
CURLcode Curl_open(கட்டமைப்பு Curl_easy **curl)
{
  கட்டமைப்பு Curl_easy *data;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  data = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_easy));
  எனில்(!data) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  data->magic = CURLEASY_MAGIC_எண்;
  /* most recent connection is not yet defined */
  data->state.last_conn_id = -1;
  data->state.last_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  data->id = -1;
  data->mid = UINT32_MAX;
  data->master_mid = UINT32_MAX;
  data->progress.hide = TRUE;

  Curl_hash_init(&data->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_LAST <= UINT8_MAX);
  Curl_u8_strset_init(&data->set.strings);
  curlx_dyn_init(&data->state.headerb, CURL_MAX_HTTP_HEADER);
  Curl_bufref_init(&data->state.url);
  Curl_bufref_init(&data->state.referer);
  Curl_req_init(&data->req);
  Curl_initinfo(data);
#ifndef CURL_DISABLE_HTTP
  Curl_llist_init(&data->state.httphdrs, வெற்று);
#endif
  Curl_netrc_init(&data->state.netrc);
  Curl_init_userdefined(data);

  *curl = data;
  திரும்பு CURLE_OK;
}

வெறுமை Curl_conn_free(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  எனில்(conn->scheme && conn->scheme->run->disconnect &&
     !conn->bits.shutdown_handler)
    conn->scheme->run->disconnect(data, conn, TRUE);

  க்கு(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(data, conn, i);
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
நிலையான bool xfer_may_multiplex(மாறிலி கட்டமைப்பு Curl_easy *data,
                               மாறிலி கட்டமைப்பு connectdata *conn)
{
#ifndef CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  எனில்((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->bits.protoconnstart || !conn->bits.close)) {

    எனில்(Curl_multiplex_wanted(data->multi) &&
       (data->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      திரும்பு TRUE;
  }
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)conn;
#endif
  திரும்பு FALSE;
}

#ifndef CURL_DISABLE_PROXY
நிலையான bool proxy_info_matches(மாறிலி கட்டமைப்பு proxy_info *data,
                               மாறிலி கட்டமைப்பு proxy_info *needle)
{
  எனில்((data->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(data->peer, needle->peer) &&
     Curl_creds_same(data->creds, needle->creds)) {
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
#endif

#ifdef USE_SSH
நிலையான bool ssh_config_matches(கட்டமைப்பு connectdata *one,
                               கட்டமைப்பு connectdata *two)
{
  கட்டமைப்பு ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  திரும்பு sshc1 && sshc2 && Curl_safecmp(sshc1->priv_சாவி, sshc2->priv_சாவி) &&
         Curl_safecmp(sshc1->pub_சாவி, sshc2->pub_சாவி);
}
#endif

கட்டமைப்பு url_conn_match {
  கட்டமைப்பு connectdata *found;
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு connectdata *needle;
  கட்டமைப்பு curltime now;
  கட்டமைப்பு ssl_filter_config ssl_config;
#ifndef CURL_DISABLE_PROXY
  கட்டமைப்பு ssl_filter_config proxy_ssl_config;
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

நிலையான bool url_match_connect_config(கட்டமைப்பு connectdata *conn,
                                     கட்டமைப்பு url_conn_match *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  எனில்(conn->bits.connect_only || conn->bits.close || conn->bits.no_reuse)
    திரும்பு FALSE;

  /* ip_version must match */
  எனில்(m->data->set.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->data->set.ipver != conn->ip_version)
    திரும்பு FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  எனில்(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    திரும்பு FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  எனில்((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    திரும்பு FALSE;

  எனில்((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    திரும்பு FALSE;

  எனில்(!m->needle->via_peer != !conn->via_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    திரும்பு FALSE;

  திரும்பு TRUE;
}

நிலையான bool url_match_fully_connected(கட்டமைப்பு connectdata *conn,
                                      கட்டமைப்பு url_conn_match *m)
{
  எனில்(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->bits.upgrade_in_progress) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    எனில்(m->may_multiplex) {
      m->seen_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->data, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான bool url_match_multi(கட்டமைப்பு connectdata *conn,
                            கட்டமைப்பு url_conn_match *m)
{
  எனில்(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    எனில்(conn->attached_multi != m->data->multi)
      திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான bool url_match_multiplex_needs(கட்டமைப்பு connectdata *conn,
                                      கட்டமைப்பு url_conn_match *m)
{
  எனில்(CONN_INUSE(conn)) {
    எனில்(!conn->bits.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->seen_single_use_conn = TRUE;
      திரும்பு FALSE;
    }
    m->seen_multiplex_conn = TRUE;
    எனில்(!m->may_multiplex || !url_match_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான bool url_match_multiplex_limits(கட்டமைப்பு connectdata *conn,
                                       கட்டமைப்பு url_conn_match *m)
{
  எனில்(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->bits.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    எனில்(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->data->multi)) {
      infof(m->data, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      திரும்பு FALSE;
    }
    எனில்(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->data, conn, FIRSTSOCKET)) {
      infof(m->data, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      திரும்பு FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->data, "Multiplexed connection found");
  }
  திரும்பு TRUE;
}

நிலையான bool url_match_ssl_use(கட்டமைப்பு connectdata *conn,
                              கட்டமைப்பு url_conn_match *m)
{
  எனில்(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    எனில்(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    எனில்(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      திரும்பு FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    எனில்(!Curl_ssl_conn_config_match(m->data, &m->ssl_config, conn, FALSE))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    திரும்பு FALSE;
  திரும்பு TRUE;
}

#ifndef CURL_DISABLE_PROXY
நிலையான bool url_match_proxy_use(கட்டமைப்பு connectdata *conn,
                                கட்டமைப்பு url_conn_match *m)
{
  எனில்(m->needle->bits.origin_is_proxy != conn->bits.origin_is_proxy)
    திரும்பு FALSE;

  எனில்(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    திரும்பு FALSE;

  எனில்(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    திரும்பு FALSE;

  எனில்(m->data->set.socks5_auth_only && !conn->bits.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->data,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    திரும்பு FALSE;
  }

  எனில்(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    எனில்(!Curl_ssl_conn_config_match(m->data, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->data,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      திரும்பு FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  திரும்பு TRUE;
}
#இல்லையெனில்
#define url_match_proxy_use(c, m) ((வெறுமை)(c), (வெறுமை)(m), TRUE)
#endif

#ifndef CURL_DISABLE_HTTP
நிலையான bool url_match_http_multiplex(கட்டமைப்பு connectdata *conn,
                                     கட்டமைப்பு url_conn_match *m,
                                     bool *pwait_pipe)
{
  எனில்(m->may_multiplex &&
     (m->data->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_seen) {
    எனில்(m->data->set.pipewait) {
      infof(m->data, "Server upgrade does not support multiplex yet, wait");
      m->found = வெற்று;
      *pwait_pipe = TRUE;
      திரும்பு TRUE; /* stop searching, we want to wait */
    }
    infof(m->data, "Server upgrade cannot be used");
    திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

நிலையான bool url_match_http_version(கட்டமைப்பு connectdata *conn,
                                   கட்டமைப்பு url_conn_match *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  எனில்((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    தேர்வு(Curl_conn_http_version(m->data, conn)) {
    நிலை 30:
      எனில்(!(m->data->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->data, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        திரும்பு FALSE;
      }
      நிறுத்து;
    நிலை 20:
      எனில்(!(m->data->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->data, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        திரும்பு FALSE;
      }
      நிறுத்து;
    இயல்பு:
      எனில்(!(m->data->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->data, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        திரும்பு FALSE;
      }
      நிறுத்து;
    }
  }
  திரும்பு TRUE;
}
#இல்லையெனில்
#define url_match_http_multiplex(c, m, w) ((வெறுமை)(c), (வெறுமை)(m), TRUE)
#define url_match_http_version(c, m)      ((வெறுமை)(c), (வெறுமை)(m), TRUE)
#endif

நிலையான bool url_match_proto_config(கட்டமைப்பு connectdata *conn,
                                   கட்டமைப்பு url_conn_match *m)
{
  எனில்(!url_match_http_version(conn, m))
    திரும்பு FALSE;

#ifdef USE_SSH
  எனில்(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    எனில்(!ssh_config_matches(m->needle, conn))
      திரும்பு FALSE;
  }
#endif
#ifndef CURL_DISABLE_FTP
  இல்லையெனில் எனில்(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    எனில்(!Curl_ftp_conns_match(m->needle, conn))
      திரும்பு FALSE;
  }
#endif
  திரும்பு TRUE;
}

நிலையான bool url_match_auth(கட்டமைப்பு connectdata *conn,
                           கட்டமைப்பு url_conn_match *m)
{
  எனில்(!Curl_creds_same(m->needle->creds, conn->creds)) {
    எனில்(m->needle->creds)
      திரும்பு FALSE;
    எனில்(!Curl_creds_same(m->data->state.creds, conn->creds))
      திரும்பு FALSE;
  }
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  எனில்(m->needle->gssapi_delegation != conn->gssapi_delegation)
    திரும்பு FALSE;
#endif

  திரும்பு TRUE;
}

நிலையான bool url_match_destination(கட்டமைப்பு connectdata *conn,
                                  கட்டமைப்பு url_conn_match *m)
{
  /* Different connect-to peers never match */
  எனில்(!Curl_peer_same_destination(m->needle->via_peer, conn->via_peer))
    திரும்பு FALSE;

  எனில்(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    திரும்பு FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  திரும்பு Curl_peer_same_destination(m->needle->origin, conn->origin);
}

நிலையான bool url_match_ssl_config(கட்டமைப்பு connectdata *conn,
                                 கட்டமைப்பு url_conn_match *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  எனில்(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_match(m->data, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->data, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

#ifdef USE_NTLM
நிலையான bool url_match_auth_ntlm(கட்டமைப்பு connectdata *conn,
                                கட்டமைப்பு url_conn_match *m)
{
  எனில்(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    எனில்(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->data->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->data->state.origin))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    எனில்(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->data->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->data->state.origin)))
      திரும்பு FALSE;
  }

#ifndef CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  எனில்(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    எனில்(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(m->want_proxy_ntlm_http) {
    எனில்(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      திரும்பு FALSE;
  }
#endif
  திரும்பு TRUE;
}
#இல்லையெனில்
#define url_match_auth_ntlm(c, m) ((வெறுமை)(c), (வெறுமை)(m), TRUE)
#endif

#ifdef USE_SPNEGO
நிலையான bool url_match_auth_nego(கட்டமைப்பு connectdata *conn,
                                கட்டமைப்பு url_conn_match *m)
{
  எனில்(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    எனில்(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->data->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->data->state.origin))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    எனில்(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->data->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->data->state.origin)))
      திரும்பு FALSE;
  }

#ifndef CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  எனில்(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    எனில்(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(m->want_proxy_nego_http) {
    எனில்(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      திரும்பு FALSE;
  }
#endif
  திரும்பு TRUE;
}
#இல்லையெனில்
#define url_match_auth_nego(c, m) ((வெறுமை)(c), (வெறுமை)(m), TRUE)
#endif

நிலையான cpool_match_result url_match_conn(கட்டமைப்பு connectdata *conn,
                                         வெறுமை *userdata)
{
  கட்டமைப்பு url_conn_match *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  எனில்(!url_match_connect_config(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  /* match for destination and protocol? */
  எனில்(!url_match_destination(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_fully_connected(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_multiplex_needs(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_ssl_use(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_proxy_use(conn, m))
    திரும்பு CPOOL_MATCH_CONT;
  எனில்(!url_match_ssl_config(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_http_multiplex(conn, m, &wait_pipe))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_auth(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_proto_config(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_auth_ntlm(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_auth_nego(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  எனில்(!url_match_multiplex_limits(conn, m))
    திரும்பு CPOOL_MATCH_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  எனில்(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->found);
    m->wait_pipe = TRUE;
    திரும்பு CPOOL_MATCH_FOUND;
  }

  எனில்(m->data->set.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->data, conn, &m->now);
    எனில்(age_ms > m->data->set.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      திரும்பு CPOOL_MATCH_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  எனில்(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->data, &m->now)) {
    infof(m->data, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    திரும்பு CPOOL_MATCH_CLOSE;
  }

  /* conn matches our needs. */
  m->found = conn;
  திரும்பு CPOOL_MATCH_FOUND;
}

நிலையான bool url_match_result(வெறுமை *userdata)
{
  கட்டமைப்பு url_conn_match *match = userdata;
  எனில்(match->found) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(match->data, match->found, TRUE);
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(match->seen_single_use_conn && !match->seen_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    match->wait_pipe = FALSE;
  }
  இல்லையெனில் எனில்(match->seen_pending_conn && match->data->set.pipewait) {
    infof(match->data,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    match->wait_pipe = TRUE;
  }
  திரும்பு FALSE;
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
நிலையான bool url_attach_existing(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *needle,
                                கட்டமைப்பு url_conn_match *m,
                                மாறிலி கட்டமைப்பு curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  திரும்பு Curl_cpool_find(data, needle->destination, TRUE, pnow,
                         url_match_conn, url_match_result, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
நிலையான கட்டமைப்பு connectdata *allocate_conn(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு connectdata *conn = curlx_calloc(1, அளவு(கட்டமைப்பு connectdata));
  எனில்(!conn)
    திரும்பு வெற்று;

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
  conn->bits.ftp_use_epsv = data->set.ftp_use_epsv;
  conn->bits.ftp_use_eprt = data->set.ftp_use_eprt;
#endif
  conn->ip_version = data->set.ipver;
  conn->bits.connect_only = (bool)data->set.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  எனில்(CURL_EASY_STR(data, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(data, STRING_INTERFACE_OPT));
    எனில்(!conn->interface_opts)
      செல் error;
  }
#ifndef CURL_DISABLE_BINDLOCAL
  conn->localportrange = data->set.localportrange;
  conn->localport = data->set.localport;
#endif

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = data->set.fclosesocket;
  conn->closesocket_client = data->set.closesocket_client;
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = data->set.gssapi_delegation;
#endif
  திரும்பு conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  திரும்பு வெற்று;
}

நிலையான CURLcode url_set_conn_scheme(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  மாறிலி கட்டமைப்பு Curl_scheme *scheme = conn->origin->scheme;
  எனில்(scheme->run &&
     (data->set.allowed_protocols & scheme->protocol) &&
     (!data->state.this_is_a_follow ||
       (data->set.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    திரும்பு CURLE_OK;
  }
  எனில்(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(data, "Protocol \"%s\" is not for transfers", scheme->name);
  இல்லையெனில்
    failf(data, "Protocol \"%s\" is disabled%s", scheme->name,
          data->state.this_is_a_follow ? " (in redirect)" : "");
  திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_to_curlcode(CURLUcode uc)
{
  தேர்வு(uc) {
  இயல்பு:
    திரும்பு CURLE_URL_MALFORMAT;
  நிலை CURLUE_UNSUPPORTED_SCHEME:
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  நிலை CURLUE_OUT_OF_MEMORY:
    திரும்பு CURLE_OUT_OF_MEMORY;
  நிலை CURLUE_USER_NOT_ALLOWED:
    திரும்பு CURLE_LOGIN_DENIED;
  }
}

#ifndef CURL_DISABLE_HSTS
நிலையான CURLcode hsts_upgrade(கட்டமைப்பு Curl_easy *data,
                             CURLU *uh,
                             uint16_t port_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  எனில்(data->hsts && (data->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(data->hsts, data->state.origin)) {
    எழுத்து *url;
    CURLUcode uc;
    CURLcode result;

    uc = curl_url_set(uh, CURLUPART_SCHEME, "https", 0);
    எனில்(uc)
      திரும்பு Curl_uc_to_curlcode(uc);
    Curl_bufref_free(&data->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    எனில்(uc)
      திரும்பு Curl_uc_to_curlcode(uc);
    Curl_bufref_set(&data->state.url, url, 0, curl_free);

    result = Curl_peer_from_url(uh, data, port_override, scope_id,
                                &data->state.origin);
    எனில்(result)
      திரும்பு result;
    infof(data, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  திரும்பு CURLE_OK;
}
#இல்லையெனில்
#define hsts_upgrade(x, y, z, a) CURLE_OK
#endif

நிலையான bool str_has_ctrl(மாறிலி எழுத்து *உள்ளீடு)
{
  எனில்(உள்ளீடு) {
    மாறிலி unsigned எழுத்து *str = (மாறிலி unsigned எழுத்து *)உள்ளீடு;
    போது(*str) {
      எனில்(*str < 0x20)
        திரும்பு TRUE;
      str++;
    }
  }
  திரும்பு FALSE;
}

#ifndef CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
நிலையான CURLcode url_set_data_creds_netrc(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு Curl_creds **pcreds)
{
  கட்டமைப்பு Curl_creds *ncreds_out = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்(data->set.use_netrc) { /* not CURL_NETRC_IGNORED */
    கட்டமைப்பு Curl_creds *ncreds_in = வெற்று;
    bool scan_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    எனில்(*pcreds) {
      தேர்வு((*pcreds)->source) {
      நிலை CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        scan_netrc = FALSE;
        நிறுத்து;
      நிலை CREDS_URL: /* only apply when netrc is not required */
        எனில்(data->set.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_in = *pcreds;
        }
        இல்லையெனில் எனில்(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_in = *pcreds;
        }
        இல்லையெனில்
          scan_netrc = FALSE;
        நிறுத்து;
      இயல்பு: /* ignore credentials from other sources */
        நிறுத்து;
      }
    }

    எனில்(!scan_netrc)
      செல் out;

    ret = Curl_netrc_scan(data, &data->state.netrc,
                          data->state.origin->hostname,
                          Curl_creds_user(ncreds_in),
                          CURL_EASY_STR(data, STRING_NETRC_FILE),
                          &ncreds_out);
    DEBUGASSERT(!ret || !ncreds_out);
    எனில்(ret == NETRC_OUT_OF_MEMORY) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
    இல்லையெனில் எனில்(ret && ((ret == NETRC_NO_MATCH) ||
                    (data->set.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(data, "Could not find host %s in the %s file; using defaults",
            data->state.origin->hostname,
            (CURL_EASY_STR(data, STRING_NETRC_FILE) ?
             CURL_EASY_STR(data, STRING_NETRC_FILE) : ".netrc"));
    }
    இல்லையெனில் எனில்(ret) {
      மாறிலி எழுத்து *m = Curl_netrc_strerror(ret);
      failf(data, ".netrc error: %s", m);
      result = CURLE_READ_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(ncreds_out) {
      எனில்(!(data->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_out->user) ||
          str_has_ctrl(ncreds_out->passwd))) {
        failf(data, "control code detected in .netrc credentials");
        result = CURLE_READ_ERROR;
        செல் out;
      }
      CURL_TRC_M(data, "netrc: using credentials for %s as %s",
                 data->state.origin->hostname, ncreds_out->user);
      result = Curl_creds_merge(ncreds_out->user, ncreds_out->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      எனில்(result)
        செல் out;
      /* for updated strings, we update them in the URL */
      uc = curl_url_set(data->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      எனில்(!uc)
        uc = curl_url_set(data->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      எனில்(uc)
        result = Curl_uc_to_curlcode(uc);
    }
    இல்லையெனில்
      DEBUGASSERT(0);
  }

#ifdef CURLVERBOSE
  Curl_creds_trace(data, data->state.creds, "transfer credentials");
#endif

out:
  Curl_creds_unlink(&ncreds_out);
  திரும்பு result;
}
#endif /* CURL_DISABLE_NETRC */

நிலையான CURLcode url_set_data_creds(கட்டமைப்பு Curl_easy *data, CURLU *uh)
{
  கட்டமைப்பு Curl_creds *newcreds = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்((CURL_EASY_STR(data, STRING_USERNAME) ||
      CURL_EASY_STR(data, STRING_PASSWORD) ||
      CURL_EASY_STR(data, STRING_BEARER) ||
      CURL_EASY_STR(data, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(data, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_to_origin(data, data->state.origin)) {
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_USERNAME),
                               CURL_EASY_STR(data, STRING_PASSWORD),
                               CURL_EASY_STR(data, STRING_BEARER),
                               CURL_EASY_STR(data, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(data, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    எனில்(result)
      செல் out;
    எனில்(newcreds &&
       !(data->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(data, "control code detected in credentials");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  எனில்(!newcreds || !Curl_creds_has_user(newcreds)) {
    எழுத்து *user = வெற்று;
    எழுத்து *passwd = வெற்று;
    எழுத்து *udecoded = வெற்று;
    எழுத்து *pdecoded = வெற்று;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    எனில்(uc && (uc != CURLUE_NO_USER))
      result = Curl_uc_to_curlcode(uc);
    எனில்(!result) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      எனில்(uc && (uc != CURLUE_NO_PASSWORD))
        result = Curl_uc_to_curlcode(uc);
    }
    எனில்(!result && user) {
      result = Curl_urldecode(user, 0, &udecoded, வெற்று,
                              (data->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    எனில்(!result && passwd) {
      result = Curl_urldecode(passwd, 0, &pdecoded, வெற்று,
                              (data->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    எனில்(!result)
      result = Curl_creds_merge(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    எனில்(result) {
      failf(data, "error extracting credentials from URL");
      செல் out;
    }
  }

#ifndef CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  result = url_set_data_creds_netrc(data, &newcreds);
#endif /* CURL_DISABLE_NETRC */

out:
  எனில்(!result && !Curl_creds_equal(data->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&data->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  திரும்பு result;
}

நிலையான CURLcode url_set_conn_origin_etc(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;

  Curl_peer_link(&conn->origin, data->state.origin);

  /* set the connection scheme */
  result = url_set_conn_scheme(data, conn);
  எனில்(result)
    செல் out;

  /* set the connection options */
  எனில்(CURL_EASY_STR(data, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(data, STRING_OPTIONS));
    எனில்(!conn->options) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }
  இல்லையெனில் எனில்(data->state.up.options) {
    conn->options = curlx_strdup(data->state.up.options);
    எனில்(!conn->options) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }

out:
  திரும்பு result;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
நிலையான CURLcode setup_range(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு UrlState *s = &data->state;
  s->resume_from = data->set.set_resume_from;
  எனில்(s->resume_from || CURL_EASY_STR(data, STRING_SET_RANGE)) {
    எனில்(s->rangestringalloc)
      curlx_free(s->range);

    எனில்(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    இல்லையெனில்
      s->range = curlx_strdup(CURL_EASY_STR(data, STRING_SET_RANGE));

    எனில்(!s->range)
      திரும்பு CURLE_OUT_OF_MEMORY;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  இல்லையெனில்
    s->use_range = FALSE; /* disable range download */

  திரும்பு CURLE_OK;
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
நிலையான CURLcode setup_connection_internals(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_peer *peer = வெற்று;

  எனில்(conn->scheme->run->setup_connection) {
    CURLcode result = conn->scheme->run->setup_connection(data, conn);
    எனில்(result)
      திரும்பு result;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  எனில்(!peer)
    திரும்பு CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  எனில்(peer->ipv6)
    எனில்(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->port);
    இல்லையெனில்
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->port);
  இல்லையெனில்
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->port);
  எனில்(!conn->destination)
    திரும்பு CURLE_OUT_OF_MEMORY;

  Curl_strntolower(conn->destination, conn->destination,
                   strlen(conn->destination));

#ifdef USE_IPV6
  எனில்(data->set.scope_id)
    conn->scope_id = data->set.scope_id;
  இல்லையெனில் {
    peer = Curl_conn_get_first_peer(conn, FIRSTSOCKET);
    எனில்(!peer)
      திரும்பு CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#endif

  திரும்பு CURLE_OK;
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
CURLcode Curl_parse_login_details(மாறிலி எழுத்து *login, மாறிலி size_t len,
                                  எழுத்து **userp, எழுத்து **passwdp,
                                  எழுத்து **optionsp)
{
  எழுத்து *ubuf = வெற்று;
  எழுத்து *pbuf = வெற்று;
  மாறிலி எழுத்து *psep = வெற்று;
  மாறிலி எழுத்து *osep = வெற்று;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  எனில்(optionsp)
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
  எனில்(!ubuf)
    செல் error;

  /* Clone the password portion buffer */
  எனில்(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    எனில்(!pbuf)
      செல் error;
  }

  /* Allocate the options portion buffer */
  எனில்(optionsp) {
    எழுத்து *obuf = வெற்று;
    எனில்(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      எனில்(!obuf)
        செல் error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  திரும்பு CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  திரும்பு CURLE_OUT_OF_MEMORY;
}

/*
 * Set the login details so they are available in the connection
 */
நிலையான CURLcode url_set_conn_login(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  எனில்((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, data->state.origin);
    எனில்(data->state.creds)
      Curl_creds_link(&conn->creds, data->state.creds);
    இல்லையெனில்
      திரும்பு Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               வெற்று, வெற்று, வெற்று, CREDS_NONE, &conn->creds);
  }
  இல்லையெனில் எனில்(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, data->state.origin);
    Curl_creds_link(&conn->creds, data->state.creds);
  }

  திரும்பு CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
நிலையான CURLcode parse_connect_to_string(கட்டமைப்பு Curl_easy *data,
                                        மாறிலி கட்டமைப்பு Curl_peer *dest,
                                        மாறிலி எழுத்து *conn_to_வரி,
                                        கட்டமைப்பு Curl_peer **pvia_dest)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ptr = conn_to_வரி;
  bool host_match = FALSE;
  bool port_match = FALSE;

  *pvia_dest = வெற்று;

  எனில்(*ptr == ':') {
    /* an empty hostname always matches */
    host_match = TRUE;
    ptr++;
  }
  இல்லையெனில் {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = strlen(dest->hostname);
    host_match = curl_strnequal(ptr, dest->hostname, hlen);
    எனில்(!host_match && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = strlen(dest->user_hostname);
      host_match = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_match = host_match && ptr[hlen] == ':';
    எனில்(host_match)
      ptr += hlen + 1;
  }

  எனில்(host_match) {
    எனில்(*ptr == ':') {
      /* an empty port always matches */
      port_match = TRUE;
      ptr++;
    }
    இல்லையெனில் {
      /* check whether the URL's port matches */
      மாறிலி எழுத்து *ptr_next = strchr(ptr, ':');
      எனில்(ptr_next) {
        curl_off_t port_to_match;
        எனில்(!curlx_str_எண்(&ptr, &port_to_match, 0xffff) &&
           ((uint16_t)port_to_match == dest->port)) {
          port_match = TRUE;
        }
        ptr = ptr_next + 1;
      }
    }
  }

  எனில்(host_match && port_match && ptr && *ptr)
    result = Curl_peer_from_connect_to(data, dest, ptr, pvia_dest);

  திரும்பு result;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
நிலையான CURLcode url_set_conn_peer(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு Curl_peer *origin = conn->origin;
  கட்டமைப்பு Curl_peer *via_peer = வெற்று;
  கட்டமைப்பு curl_slist *conn_to_entry = data->set.connect_to;

  DEBUGASSERT(!conn->via_peer);
  Curl_peer_unlink(&conn->via_peer);

  போது(conn_to_entry && !via_peer) {
    result = parse_connect_to_string(data, origin, conn_to_entry->data,
                                     &via_peer);
    எனில்(result)
      திரும்பு result;
    conn_to_entry = conn_to_entry->next;
  }

#ifndef CURL_DISABLE_ALTSVC
  எனில்(data->asi && !via_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#ifdef DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#இல்லையெனில்
      0
#endif
       )) {
    /* no connect_to match, try alt-svc! */
    எண்ணிக்கை alpnid srcalpnid = ALPN_none;
    bool hit = FALSE;
    கட்டமைப்பு altsvc *as = வெற்று;
    முழு allowed_alpns = ALPN_none;
    கட்டமைப்பு http_negotiation *neg = &data->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(data, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#ifdef USE_HTTP3
    எனில்(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#endif
#ifdef USE_HTTP2
    எனில்(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#endif
    எனில்(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (முழு)data->asi->flags;

    DEBUGF(infof(data, "check Alt-Svc for host '%s'", origin->hostname));
#ifdef USE_HTTP3
    எனில்(!hit && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      hit = Curl_altsvc_lookup(data->asi,
                               origin, ALPN_h3, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }
#endif
#ifdef USE_HTTP2
    எனில்(!hit && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_prior_knowledge) {
      srcalpnid = ALPN_h2;
      hit = Curl_altsvc_lookup(data->asi,
                               origin, ALPN_h2, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }
#endif
    எனில்(!hit && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      hit = Curl_altsvc_lookup(data->asi,
                               origin, ALPN_h1, /* from */
                               &as /* to */,
                               allowed_alpns, &same_dest);
    }

    எனில்(hit && same_dest) {
      /* same destination, but more HTTPS version options */
      தேர்வு(as->dst.alpnid) {
      நிலை ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        நிறுத்து;
      நிலை ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        நிறுத்து;
      நிலை ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        நிறுத்து;
      இயல்பு: /* should not be possible */
        நிறுத்து;
      }
    }
    இல்லையெனில் எனில்(hit) {
      result = Curl_peer_create(data, conn->origin->scheme,
                                as->dst.host, as->dst.port,
                                &via_peer);
      எனில்(result)
        திரும்பு result;
      infof(data, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->port,
            Curl_alpnid2str(as->dst.alpnid),
            via_peer->hostname, via_peer->port);
      conn->bits.altused = TRUE;
      எனில்(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        தேர்வு(as->dst.alpnid) {
        நிலை ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          நிறுத்து;
        நிலை ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          நிறுத்து;
        நிலை ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          நிறுத்து;
        இயல்பு: /* should not be possible */
          நிறுத்து;
        }
      }
    }
  }
#endif

  எனில்(via_peer)
    conn->via_peer = via_peer;

  திரும்பு result;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
நிலையான வெறுமை url_conn_reuse_adjust(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *needle)
{
  கட்டமைப்பு connectdata *conn = data->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  எனில்(needle->creds) {
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

நிலையான வெறுமை conn_meta_freeentry(வெறுமை *p)
{
  (வெறுமை)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

நிலையான CURLcode url_create_needle(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata **pneedle)
{
  கட்டமைப்பு connectdata *needle = வெற்று;
  CURLcode result = CURLE_OK;
  bool network_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(data);
  எனில்(!needle) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  result = url_set_conn_origin_etc(data, needle);
  எனில்(result)
    செல் out;

  DEBUGASSERT(needle->origin);
  network_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#ifdef USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  எனில்(network_scheme && CURL_EASY_STR(data, STRING_UNIX_SOCKET_PATH)) {
    result = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(data, STRING_UNIX_SOCKET_PATH),
      (bool)data->set.abstract_unix_socket, &needle->via_peer);
    எனில்(result)
      செல் out;
  }
#endif /* USE_UNIX_SOCKETS */

  எனில்(network_scheme && !needle->via_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    result = url_set_conn_peer(data, needle);
    எனில்(result)
      செல் out;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  எனில்(Curl_peer_equal(needle->origin, needle->via_peer)) {
    Curl_peer_unlink(&needle->via_peer);
  }

#ifndef CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  எனில்(network_scheme &&
     (!needle->via_peer || !needle->via_peer->unix_socket)) {
    result = Curl_proxy_init_conn(data, needle);
    எனில்(result)
      செல் out;
  }
#endif /* CURL_DISABLE_PROXY */

  result = url_set_conn_login(data, needle); /* default credentials */
  எனில்(result)
    செல் out;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  result = setup_connection_internals(data, needle);
  எனில்(result)
    செல் out;

  எனில்(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    எனில்(data->set.ssl_enable_alpn)
      needle->bits.tls_enable_alpn = TRUE;
  }

  எனில்(network_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->bits.tcp_fastopen = data->set.tcp_fastopen;
#ifdef USE_UNIX_SOCKETS
    எனில்(Curl_conn_get_first_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#endif
  }

out:
  எனில்(!result) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  இல்லையெனில் {
    *pneedle = வெற்று;
    எனில்(needle)
      Curl_conn_free(data, needle);
  }
  திரும்பு result;
}

நிலையான CURLcode url_set_data_origin_and_creds(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_set_uh = (data->set.uh && !data->state.this_is_a_follow);
  uint16_t port_override = data->state.allow_port ? data->set.use_port : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  எனில்(!Curl_bufref_ptr(&data->state.url)) {
    result = CURLE_URL_MALFORMAT;
    செல் out;
  }

  up_free(data); /* cleanup previous leftovers first */

  /* parse the URL */
  எனில்(use_set_uh)
    uh = data->state.uh = curl_url_dup(data->set.uh);
  இல்லையெனில்
    uh = data->state.uh = curl_url();
  எனில்(!uh) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  எனில்(!use_set_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    எனில்(curl_strequal("file", CURL_EASY_STR(data, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&data->state.url), வெற்று, 0, TRUE))
      prepend_sheme = TRUE;
    இல்லையெனில் {
      uc = curl_url_set(uh, CURLUPART_URL, Curl_bufref_ptr(&data->state.url),
                        (unsigned முழு)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (data->set.disallow_username_in_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
      எனில்(uc) {
        failf(data, "URL rejected: %s", curl_url_strerror(uc));
        result = Curl_uc_to_curlcode(uc);
        செல் out;
      }

      எனில்(CURL_EASY_STR(data, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        எழுத்து *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        எனில்(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        எனில்(uc && uc != CURLUE_NO_SCHEME) {
          result = Curl_uc_to_curlcode(uc);
          செல் out;
        }
      }
    }

    எனில்(prepend_sheme) {
      எழுத்து *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(data, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&data->state.url));
      எனில்(!url) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      uc = curl_url_set(uh, CURLUPART_URL, url,
                        (unsigned முழு)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (data->set.disallow_username_in_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
      curl_free(url);
      எனில்(uc) {
        failf(data, "URL rejected: %s", curl_url_strerror(uc));
        result = Curl_uc_to_curlcode(uc);
        செல் out;
      }
    }
    {
      எழுத்து *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_EMPTY);
      எனில்(uc) {
        result = Curl_uc_to_curlcode(uc);
        செல் out;
      }
      எனில்(prepend_sheme)
        infof(data, "Applied default protocol '%s'",
              CURL_EASY_STR(data, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_set(&data->state.url, newurl, 0, curl_free);
    }
  }

#ifdef USE_IPV6
  scope_id = data->set.scope_id;
#endif

  /* `uh` is now as the connection should use it, probably. */
  result = Curl_peer_from_url(uh, data, port_override, scope_id,
                              &data->state.origin);
  எனில்(result)
    செல் out;
  /* The origin might get changed when HSTS applies */
  result = hsts_upgrade(data, uh, port_override, scope_id);
  எனில்(result)
    செல் out;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  எனில்(!data->state.initial_origin)
    Curl_peer_link(&data->state.initial_origin, data->state.origin);

  uc = curl_url_get(uh, CURLUPART_PATH, &data->state.up.path, CURLU_URLENCODE);
  எனில்(uc) {
    result = Curl_uc_to_curlcode(uc);
    செல் out;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &data->state.up.query,
                    CURLU_GET_EMPTY);
  எனில்(uc && (uc != CURLUE_NO_QUERY)) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &data->state.up.options,
                    CURLU_URLDECODE);
  எனில்(uc && (uc != CURLUE_NO_OPTIONS)) {
    result = Curl_uc_to_curlcode(uc);
    செல் out;
  }

  result = url_set_data_creds(data, uh);
  எனில்(result)
    செல் out;

out:
  திரும்பு result;
}

நிலையான CURLcode url_match_init(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *needle,
                               கட்டமைப்பு url_conn_match *m)
{
  memset(m, 0, அளவு(*m));
  m->data = data;
  m->needle = needle;
  m->now = *Curl_pgrs_now(data);
  m->may_multiplex = xfer_may_multiplex(data, needle);

#ifdef USE_NTLM
  m->want_ntlm_http =
    (data->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_to_host(data);
#ifndef CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (data->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#endif
#endif

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (data->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_to_host(data);
#ifndef CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (data->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#endif
#endif
  m->require_tls = data->set.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = data->set.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  திரும்பு Curl_ssl_filter_config_tmp_init(data, needle->origin,
                                         &m->ssl_config,
#ifndef CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#இல்லையெனில்
                                         வெற்று);
#endif
}

நிலையான வெறுமை url_match_destroy(கட்டமைப்பு url_conn_match *m)
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
நிலையான CURLcode url_find_or_create_conn(கட்டமைப்பு Curl_easy *data,
                                        மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு connectdata *needle = வெற்று;
  கட்டமைப்பு url_conn_match match;
  bool match_initialized = FALSE;
  CURLcode result;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  result = url_create_needle(data, &needle);
  எனில்(result)
    செல் out;
  DEBUGASSERT(needle);

  result = url_match_init(data, needle, &match);
  எனில்(result)
    செல் out;
  match_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#ifndef CURL_DISABLE_FILE
  எனில்(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool done;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->run->connect_it);
    data->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    data->info.conn_protocol = needle->scheme->protocol & CURLPROTO_MASK;
    result = needle->scheme->run->connect_it(data, &done);
    எனில்(result)
      செல் out;

    /* Setup a "faked" transfer that will do nothing */
    result = Curl_cpool_add(data, needle, 0, 0, pnow);
    Curl_attach_connection(data, needle, TRUE);
    needle = வெற்று;
    எனில்(!result) {
      /* Setup whatever necessary for a resumed transfer */
      result = setup_range(data);
      எனில்(!result) {
        Curl_xfer_setup_nop(data);
        result = Curl_init_transfer(data, data->conn);
      }
    }

    எனில்(result) {
      DEBUGASSERT(data->conn->scheme->run->done);
      /* we ignore the return code for the protocol-specific DONE */
      (வெறுமை)data->conn->scheme->run->done(data, result, FALSE);
    }
    செல் out;
  }
#endif

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  எனில்((!data->set.reuse_fresh || data->state.followlocation) &&
     !data->set.connect_only) {
    /* Ok, try to find and attach an existing one */
    எனில்(url_attach_existing(data, needle, &match, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      கட்டமைப்பு connectdata *conn = data->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->bits.reuse = TRUE;
      url_conn_reuse_adjust(data, needle);

#ifndef CURL_DISABLE_PROXY
      infof(data, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#இல்லையெனில்
      infof(data, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#endif
    }
  }

  எனில்(!data->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    எனில்(match.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(data, "Waiting on connection to negotiate possible multiplexing.");
      result = CURLE_NO_CONNECTION_AVAILABLE;
      செல் out;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    result = Curl_cpool_add(data, needle,
                            data->multi->max_total_connections,
                            data->multi->max_host_connections,
                            pnow);
    எனில்(result)
      செல் out;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(data, needle, TRUE);
    needle = வெற்று;

    /* Clone the SSL filter configs into the new connection. */
    result = Curl_ssl_conn_config_clone(&match.ssl_config,
#ifndef CURL_DISABLE_PROXY
                                        &match.proxy_ssl_config,
#இல்லையெனில்
                                        வெற்று,
#endif
                                        data->conn);
    எனில்(result) {
      DEBUGF(infof(data, "Error: clone connection SSL config"));
      செல் out;
    }

#ifdef USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    எனில்((data->state.authhost.picked & CURLAUTH_NTLM) &&
       data->state.authhost.done) {
      infof(data, "NTLM picked AND auth done set, clear picked");
      data->state.authhost.picked = CURLAUTH_NONE;
      data->state.authhost.done = FALSE;
    }

    எனில்((data->state.authproxy.picked & CURLAUTH_NTLM) &&
       data->state.authproxy.done) {
      infof(data, "NTLM-proxy picked AND auth done set, clear picked");
      data->state.authproxy.picked = CURLAUTH_NONE;
      data->state.authproxy.done = FALSE;
    }
#endif
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  result = Curl_init_transfer(data, data->conn);
  எனில்(result)
    செல் out;

  /* Setup whatever necessary for a resumed transfer */
  result = setup_range(data);
  எனில்(result)
    செல் out;

  /* persist the scheme and handler the transfer is using */
  data->info.conn_scheme = data->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  data->info.conn_protocol = data->conn->scheme->protocol & CURLPROTO_MASK;
  data->info.used_proxy =
#ifdef CURL_DISABLE_PROXY
    0
#இல்லையெனில்
    (data->conn->socks_proxy.peer || data->conn->http_proxy.peer)
#endif
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  result = Curl_conn_ev_data_setup(data);

out:
  எனில்(match_initialized)
    url_match_destroy(&match);
  எனில்(needle)
    Curl_conn_free(data, needle);
  DEBUGASSERT(result || data->conn);
  திரும்பு result;
}

CURLcode Curl_connect(கட்டமைப்பு Curl_easy *data, bool *pconnected)
{
  CURLcode result;
  கட்டமைப்பு connectdata *conn = வெற்று;
  மாறிலி கட்டமைப்பு curltime *pnow = வெற்று;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&data->req, data);
  /* Determine the origin of the transfer and what credentials to use */
  result = url_set_data_origin_and_creds(data);
  எனில்(result)
    செல் out;
  எனில்(!data->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(data);
  Curl_pgrsTimeWas(data, TIMER_POSTQUEUE, *pnow);
  result = url_find_or_create_conn(data, pnow);
  conn = data->conn;
  எனில்(result)
    செல் out;
  எனில்(!data->conn) { /* just make really sure */
    DEBUGASSERT(0);
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  எனில்(conn->bits.reuse) {
    எனில்(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  இல்லையெனில் எனில்(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrsTime(data, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  இல்லையெனில் {
    result = Curl_conn_setup(data, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    எனில்(!result)
      result = Curl_headers_init(data);
    CURL_TRC_M(data, "Curl_conn_setup() -> %d", (முழு)result);
  }

out:
  எனில்(result == CURLE_NO_CONNECTION_AVAILABLE)
    DEBUGASSERT(!conn);

  எனில்(result && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
  }

  திரும்பு result;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn)
{
  CURLcode result;

  எனில்(conn) {
    conn->bits.do_more = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    எனில்(data->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      data->state.wildcardmatch = FALSE;
  }

  data->state.done = FALSE; /* *_done() is not called yet */

  data->req.no_body = data->set.opt_no_body;
  எனில்(data->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    data->state.httpreq = HTTPREQ_HEAD;

  result = Curl_req_start(&data->req, data);
  எனில்(!result) {
    Curl_pgrsReset(data);
  }
  திரும்பு result;
}

#எனில் defined(USE_HTTP2) || defined(USE_HTTP3)

வெறுமை Curl_data_priority_clear_state(கட்டமைப்பு Curl_easy *data)
{
  data->state.weight = 0;
}

#endif /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_set(கட்டமைப்பு connectdata *conn, மாறிலி எழுத்து *சாவி,
                            வெறுமை *meta_data, Curl_meta_dtor *meta_dtor)
{
  எனில்(!Curl_hash_add2(&conn->meta_hash, சாவி, strlen(சாவி) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(சாவி, strlen(சாவி) + 1, meta_data);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  திரும்பு CURLE_OK;
}

வெறுமை Curl_conn_meta_remove(கட்டமைப்பு connectdata *conn, மாறிலி எழுத்து *சாவி)
{
  Curl_hash_delete(&conn->meta_hash, சாவி, strlen(சாவி) + 1);
}

வெறுமை *Curl_conn_meta_get(கட்டமைப்பு connectdata *conn, மாறிலி எழுத்து *சாவி)
{
  திரும்பு Curl_hash_pick(&conn->meta_hash, சாவி, strlen(சாவி) + 1);
}

கட்டமைப்பு Curl_easy *Curl_get_admin(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_easy *admin;

  எனில்(!data->mid) /* already an admin handle */
    admin = data;
  இல்லையெனில் எனில்(data->multi)
    admin = data->multi->admin;
  இல்லையெனில் எனில்(data->multi_easy)
    admin = data->multi_easy->admin;
  இல்லையெனில் {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = data;
  }
  எனில்(admin != data) {
    admin->set.conn_max_idle_ms = data->set.conn_max_idle_ms;
    admin->set.conn_max_age_ms = data->set.conn_max_age_ms;
    admin->set.upkeep_interval_ms = data->set.upkeep_interval_ms;
    admin->set.timeout = data->set.timeout;
    admin->set.server_response_timeout = data->set.server_response_timeout;
    admin->set.no_signal = data->set.no_signal;
  }
  திரும்பு admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  எனில்(r1 && (r1 != CURLE_AGAIN))
    திரும்பு r1;
  எனில்(r2 && (r2 != CURLE_AGAIN))
    திரும்பு r2;
  திரும்பு r1;
}
