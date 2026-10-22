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
#ضمن "curl_setup.h"

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NET_IF_H
#ضمن <net/if.h>
#د_که_پای
#که_تعریف_شوی HAVE_IPHLPAPI_H
#ضمن <Iphlpapi.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#د_که_پای

#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_UN_H
#ضمن <sys/un.h>
#د_که_پای

#که_نه_تعریف_شوی HAVE_SOCKET
#تېروتنه "We cannot compile without socket() support"
#د_که_پای

#إذا defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ضمن <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#د_که_پای
#ضمن <iphlpapi.h>
#د_که_پای

#ضمن "urldata.h"
#ضمن "mime.h"
#ضمن "bufref.h"
#ضمن "vtls/vtls.h"
#ضمن "vssh/vssh.h"
#ضمن "transfer.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "cookie.h"
#ضمن "strcase.h"
#ضمن "escape.h"
#ضمن "curl_share.h"
#ضمن "http_digest.h"
#ضمن "multiif.h"
#ضمن "getinfo.h"
#ضمن "pop3.h"
#ضمن "urlapi-int.h"
#ضمن "hsts.h"
#ضمن "proxy.h"
#ضمن "cfilters.h"
#ضمن "idn.h"
#ضمن "http_proxy.h"
#ضمن "conncache.h"
#ضمن "multihandle.h"
#ضمن "curlx/strdup.h"
#ضمن "setopt.h"
#ضمن "altsvc.h"
#ضمن "curlx/dynbuf.h"
#ضمن "headers.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strparse.h"
#ضمن "peer.h"

/* Now for the protocols */
#ضمن "ftp.h"
#ضمن "dict.h"
#ضمن "telnet.h"
#ضمن "tftp.h"
#ضمن "http.h"
#ضمن "vauth/vauth.h"
#ضمن "file.h"
#ضمن "curl_ldap.h"
#ضمن "vssh/ssh.h"
#ضمن "imap.h"
#ضمن "url.h"
#ضمن "connect.h"
#ضمن "gopher.h"
#ضمن "mqtt.h"
#ضمن "rtsp.h"
#ضمن "smtp.h"
#ضمن "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#إذا READBUFFER_اندازه < READBUFFER_MIN
# error READBUFFER_اندازه is too small
#د_که_پای

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
ساكن curl_prot_t get_protocol_family(ثابت بنية Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  أرجع s->family;
}

فراغ Curl_freeset(بنية Curl_easy *معلومات)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  تعداد dupblob j;

  CURL_EASY_STR_CLEAR0(معلومات, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(معلومات, STRING_کلی_PASSWD);
  CURL_EASY_STR_CLEAR0(معلومات, STRING_BEARER);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(معلومات, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(معلومات, STRING_کلی_PASSWD_PROXY);
#د_که_پای
  Curl_u8_strset_clear(&معلومات->سېټ.مزي_ګانې);
  curlx_safefree(معلومات->سېټ.str_copypostfields);

  لكل(j = (تعداد dupblob)0; j < BLOB_وروستی; j++) {
    curlx_safefree(معلومات->سېټ.blobs[j]);
  }

  Curl_bufref_free(&معلومات->state.referer);
  Curl_bufref_free(&معلومات->state.url);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(معلومات->سېټ.mimepostp);
  curlx_safefree(معلومات->سېټ.mimepostp);
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  curl_slist_free_all(معلومات->state.cookielist);
  معلومات->state.cookielist = عدم;
#د_که_پای
}

/* free the URL pieces */
ساكن فراغ پورته_free(بنية Curl_easy *معلومات)
{
  بنية urlpieces *پورته = &معلومات->state.پورته;
  curlx_safefree(پورته->options);
  curlx_safefree(پورته->لار);
  curlx_safefree(پورته->query);
  curl_url_cleanup(معلومات->state.uh);
  معلومات->state.uh = عدم;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(بنية Curl_easy **datap)
{
  بنية Curl_easy *معلومات;

  إذا(!datap || !*datap)
    أرجع CURLE_OK;

  معلومات = *datap;
  *datap = عدم;

  إذا(!معلومات->state.دننی && معلومات->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(معلومات->multi, معلومات);
  }
  وإلا {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(معلومات);
    إذا(!معلومات->state.دننی && معلومات->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(معلومات->multi_easy);
      معلومات->multi_easy = عدم;
    }
  }
  DEBUGASSERT(!معلومات->conn || معلومات->state.دننی);

  Curl_expire_clear_all(معلومات); /* shut off any timers left */

  إذا(معلومات->state.rangestringalloc)
    curlx_free(معلومات->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(معلومات);

  معلومات->سېټ.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  معلومات->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&معلومات->req, معلومات);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(معلومات);
  Curl_peer_unlink(&معلومات->state.origin);
  Curl_peer_unlink(&معلومات->state.initial_origin);
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  Curl_peer_unlink(&معلومات->state.rtsp_session_origin);
#د_که_پای
  Curl_ssl_free_certinfo(معلومات);

  Curl_bufref_free(&معلومات->state.referer);

  پورته_free(معلومات);
  curlx_dyn_free(&معلومات->state.headerb);
  Curl_flush_cookies(معلومات, TRUE);
#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  Curl_altsvc_save(معلومات, معلومات->asi, CURL_EASY_STR(معلومات, STRING_ALTSVC));
  Curl_altsvc_cleanup(&معلومات->asi);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  Curl_hsts_save(معلومات, معلومات->hsts, CURL_EASY_STR(معلومات, STRING_HSTS));
  إذا(!معلومات->share || !معلومات->share->hsts)
    Curl_hsts_cleanup(&معلومات->hsts);
  curl_slist_free_all(معلومات->state.hstslist); /* clean up list */
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(معلومات);
#د_که_پای
  curlx_safefree(معلومات->state.most_recent_ftp_entrypath);
  curlx_safefree(معلومات->info.contenttype);
  curlx_safefree(معلومات->info.wouldredirect);
  curlx_safefree(معلومات->info.effective_url);

  /* No longer a dirty share, if it exists */
  إذا(Curl_share_easy_unlink(معلومات))
    DEBUGASSERT(0);

  Curl_hash_destroy(&معلومات->meta_hash);
  Curl_creds_unlink(&معلومات->state.creds);
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  curlx_safefree(معلومات->state.rangeline);
  curlx_safefree(معلومات->state.http_host);
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_COOKIES
  curlx_safefree(معلومات->req.cookiehost);
#د_که_پای

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(معلومات->state.formp);
  curlx_safefree(معلومات->state.formp);
#د_که_پای

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&معلومات->wildcard);
  Curl_freeset(معلومات);
  Curl_سرلیکونه_cleanup(معلومات);
  Curl_netrc_cleanup(&معلومات->state.netrc);
#که_نه_تعریف_شوی CURL_DISABLE_DIGEST_AUTH
  curlx_free(معلومات->state.envproxy);
#د_که_پای
  curlx_memzero(معلومات, حجم(*معلومات));
  curlx_free(معلومات);
  أرجع CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
فراغ Curl_init_userdefined(بنية Curl_easy *معلومات)
{
  بنية UserDefined *سېټ = &معلومات->سېټ;

  سېټ->بهر = stdout;  /* default output to stdout */
  سېټ->دننه_سېټ = stdin;  /* default input from stdin */
  سېټ->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&معلومات->سېټ.مزي_ګانې);

#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wcast-function-type-strict"
#د_که_پای
  /* use fwrite as default function to store output */
  سېټ->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  سېټ->fread_func_سېټ = (curl_read_callback)fread;
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic راوباسه
#د_که_پای
  سېټ->is_fread_سېټ = 0;

  سېټ->seek_client = ZERO_NULL;

  سېټ->filesize = -1;        /* we do not know the size */
  سېټ->postfieldsize = -1;   /* unknown size */
  سېټ->maxredirs = 30;       /* sensible default */

  سېټ->لاره = HTTPREQ_GET; /* Default HTTP request */
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  سېټ->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  سېټ->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  سېټ->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  سېټ->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  سېټ->ftp_filemethod = FTPFILE_MULTICWD;
  سېټ->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#د_که_پای
  سېټ->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  سېټ->ssl_ca_cache_timeout = 24 * 60 * 60;

  سېټ->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&معلومات->سېټ.ssl);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  Curl_ssl_config_init(&معلومات->سېټ.proxy_ssl);
  سېټ->proxyport = 0;
  سېټ->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  سېټ->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  سېټ->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_DOH
  سېټ->doh_verifyhost = TRUE;
  سېټ->doh_verifypeer = TRUE;
#د_که_پای
#که_تعریف_شوی USE_SSH
  /* defaults to any auth type */
  سېټ->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  سېټ->new_directory_perms = 0755; /* Default permissions */
#د_که_پای

  سېټ->new_file_perms = 0644;    /* Default permissions */
  سېټ->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  سېټ->redir_protocols = CURLPROTO_REDIR;

#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  سېټ->socks5_gssapi_nec = FALSE;
#د_که_پای

  /* set default minimum TLS version */
#که_تعریف_شوی USE_SSL
  Curl_setopt_SSLVERSION(معلومات, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(معلومات, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#د_که_پای
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  سېټ->wildcard_enabled = FALSE;
  سېټ->chunk_bgn = ZERO_NULL;
  سېټ->chunk_پای_ټکی = ZERO_NULL;
  سېټ->fnmatch = ZERO_NULL;
#د_که_پای
  سېټ->tcp_keepalive = FALSE;
  سېټ->tcp_keepintvl = 60;
  سېټ->tcp_keepidle = 60;
  سېټ->tcp_keepcnt = 9;
  سېټ->tcp_fastopen = FALSE;
  سېټ->tcp_nodelay = TRUE;
  سېټ->ssl_enable_alpn = TRUE;
  سېټ->expect_100_timeout = 1000L; /* Wait for a second by default. */
  سېټ->sep_سرلیکونه = TRUE; /* separated header lists by default */
  سېټ->بفر_اندازه = READBUFFER_اندازه;
  سېټ->upload_بفر_اندازه = UPLOADBUFFER_DEFAULT;
  سېټ->upload_flags = CURLULFLAG_کتل_شوی;
  سېټ->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  سېټ->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  سېټ->maxconnects = DEFAULT_CONNCACHE_اندازه; /* for easy handles */
  سېټ->conn_max_idle_ms = 118 * 1000;
  سېټ->conn_max_age_ms = 24 * 3600 * 1000;
  سېټ->http09_allowed = FALSE;
  سېټ->httpwant = CURL_HTTP_نسخه_NONE;
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  سېټ->وزن = 0;
#د_که_پای
  سېټ->quick_exit = 0L;
#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS
  سېټ->ws_raw_ډېرپېښ = FALSE;
  سېټ->ws_no_auto_pong = FALSE;
#د_که_پای
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ساكن فراغ easy_meta_freeentry(فراغ *p)
{
  (فراغ)p;
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
CURLcode Curl_open(بنية Curl_easy **curl)
{
  بنية Curl_easy *معلومات;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  معلومات = curlx_calloc(1, حجم(بنية Curl_easy));
  إذا(!معلومات) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    أرجع CURLE_بهر_د_حافظه;
  }

  معلومات->magic = CURLEASY_MAGIC_شمېره;
  /* most recent connection is not yet defined */
  معلومات->state.وروستی_conn_id = -1;
  معلومات->state.وروستی_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  معلومات->id = -1;
  معلومات->mid = UINT32_MAX;
  معلومات->master_mid = UINT32_MAX;
  معلومات->پرمختګ.hide = TRUE;

  Curl_hash_init(&معلومات->meta_hash, 23, CURL_HASH_TYPE_بایټونه,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_وروستی <= UINT8_MAX);
  Curl_u8_strset_init(&معلومات->سېټ.مزي_ګانې);
  curlx_dyn_init(&معلومات->state.headerb, CURL_MAX_HTTP_سرلیک);
  Curl_bufref_init(&معلومات->state.url);
  Curl_bufref_init(&معلومات->state.referer);
  Curl_req_init(&معلومات->req);
  Curl_initinfo(معلومات);
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  Curl_llist_init(&معلومات->state.httphdrs, عدم);
#د_که_پای
  Curl_netrc_init(&معلومات->state.netrc);
  Curl_init_userdefined(معلومات);

  *curl = معلومات;
  أرجع CURLE_OK;
}

فراغ Curl_conn_free(بنية Curl_easy *معلومات, بنية connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  إذا(conn->scheme && conn->scheme->پرلپسې->disconnect &&
     !conn->بټونه.shutdown_handler)
    conn->scheme->پرلپسې->disconnect(معلومات, conn, TRUE);

  لكل(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(معلومات, conn, i);
  }

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#د_که_پای
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->له_لارې_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->له_لارې_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
ساكن bool xfer_may_multiplex(ثابت بنية Curl_easy *معلومات,
                               ثابت بنية connectdata *conn)
{
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  إذا((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->بټونه.protoconnstart || !conn->بټونه.close)) {

    إذا(Curl_multiplex_wanted(معلومات->multi) &&
       (معلومات->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      أرجع TRUE;
  }
#وإلا
  (فراغ)معلومات;
  (فراغ)conn;
#د_که_پای
  أرجع FALSE;
}

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
ساكن bool proxy_info_matches(ثابت بنية proxy_info *معلومات,
                               ثابت بنية proxy_info *needle)
{
  إذا((معلومات->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(معلومات->peer, needle->peer) &&
     Curl_creds_same(معلومات->creds, needle->creds)) {
    أرجع TRUE;
  }
  أرجع FALSE;
}
#د_که_پای

#که_تعریف_شوی USE_SSH
ساكن bool ssh_config_matches(بنية connectdata *one,
                               بنية connectdata *two)
{
  بنية ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  أرجع sshc1 && sshc2 && Curl_safecmp(sshc1->priv_کلی, sshc2->priv_کلی) &&
         Curl_safecmp(sshc1->pub_کلی, sshc2->pub_کلی);
}
#د_که_پای

بنية url_conn_سمون {
  بنية connectdata *وموندل;
  بنية Curl_easy *معلومات;
  بنية connectdata *needle;
  بنية curltime now;
  بنية ssl_filter_config ssl_config;
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  بنية ssl_filter_config proxy_ssl_config;
#د_که_پای
  بټ(may_multiplex);
  بټ(want_ntlm_http);
  بټ(want_proxy_ntlm_http);
  بټ(want_nego_http);
  بټ(want_proxy_nego_http);
  بټ(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  بټ(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  بټ(wait_pipe);
  بټ(کتل_شوی_pending_conn);
  بټ(کتل_شوی_single_use_conn);
  بټ(کتل_شوی_multiplex_conn);
};

ساكن bool url_سمون_connect_config(بنية connectdata *conn,
                                     بنية url_conn_سمون *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  إذا(conn->بټونه.connect_only || conn->بټونه.close || conn->بټونه.no_reuse)
    أرجع FALSE;

  /* ip_version must match */
  إذا(m->معلومات->سېټ.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->معلومات->سېټ.ipver != conn->ip_نسخه)
    أرجع FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  إذا(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    أرجع FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  إذا((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    أرجع FALSE;

  إذا((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    أرجع FALSE;

  إذا(!m->needle->له_لارې_peer != !conn->له_لارې_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    أرجع FALSE;

  أرجع TRUE;
}

ساكن bool url_سمون_fully_connected(بنية connectdata *conn,
                                      بنية url_conn_سمون *m)
{
  إذا(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->بټونه.upgrade_دننه_پرمختګ) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    إذا(m->may_multiplex) {
      m->کتل_شوی_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->معلومات, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_سمون_multi(بنية connectdata *conn,
                            بنية url_conn_سمون *m)
{
  إذا(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    إذا(conn->attached_multi != m->معلومات->multi)
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_سمون_multiplex_needs(بنية connectdata *conn,
                                      بنية url_conn_سمون *m)
{
  إذا(CONN_INUSE(conn)) {
    إذا(!conn->بټونه.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->کتل_شوی_single_use_conn = TRUE;
      أرجع FALSE;
    }
    m->کتل_شوی_multiplex_conn = TRUE;
    إذا(!m->may_multiplex || !url_سمون_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_سمون_multiplex_limits(بنية connectdata *conn,
                                       بنية url_conn_سمون *m)
{
  إذا(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->بټونه.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    إذا(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->معلومات->multi)) {
      infof(m->معلومات, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      أرجع FALSE;
    }
    إذا(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->معلومات, conn, FIRSTSOCKET)) {
      infof(m->معلومات, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      أرجع FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->معلومات, "Multiplexed connection found");
  }
  أرجع TRUE;
}

ساكن bool url_سمون_ssl_use(بنية connectdata *conn,
                              بنية url_conn_سمون *m)
{
  إذا(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    إذا(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      أرجع FALSE;
  }
  وإلا إذا(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    إذا(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      أرجع FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    إذا(!Curl_ssl_conn_config_سمون(m->معلومات, &m->ssl_config, conn, FALSE))
      أرجع FALSE;
  }
  وإلا إذا(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    أرجع FALSE;
  أرجع TRUE;
}

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
ساكن bool url_سمون_proxy_use(بنية connectdata *conn,
                                بنية url_conn_سمون *m)
{
  إذا(m->needle->بټونه.origin_is_proxy != conn->بټونه.origin_is_proxy)
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    أرجع FALSE;

  إذا(m->معلومات->سېټ.socks5_auth_only && !conn->بټونه.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->معلومات,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }

  إذا(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    إذا(!Curl_ssl_conn_config_سمون(m->معلومات, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->معلومات,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      أرجع FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  أرجع TRUE;
}
#وإلا
#تعریف_کړه url_سمون_proxy_use(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_HTTP
ساكن bool url_سمون_http_multiplex(بنية connectdata *conn,
                                     بنية url_conn_سمون *m,
                                     bool *pwait_pipe)
{
  إذا(m->may_multiplex &&
     (m->معلومات->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_کتل_شوی) {
    إذا(m->معلومات->سېټ.pipewait) {
      infof(m->معلومات, "Server upgrade does not support multiplex yet, wait");
      m->وموندل = عدم;
      *pwait_pipe = TRUE;
      أرجع TRUE; /* stop searching, we want to wait */
    }
    infof(m->معلومات, "Server upgrade cannot be used");
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_سمون_http_نسخه(بنية connectdata *conn,
                                   بنية url_conn_سمون *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  إذا((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    بدل(Curl_conn_http_نسخه(m->معلومات, conn)) {
    حالة 30:
      إذا(!(m->معلومات->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->معلومات, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    حالة 20:
      إذا(!(m->معلومات->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->معلومات, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    افتراضي:
      إذا(!(m->معلومات->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->معلومات, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    }
  }
  أرجع TRUE;
}
#وإلا
#تعریف_کړه url_سمون_http_multiplex(c, m, w) ((فراغ)(c), (فراغ)(m), TRUE)
#تعریف_کړه url_سمون_http_نسخه(c, m)      ((فراغ)(c), (فراغ)(m), TRUE)
#د_که_پای

ساكن bool url_سمون_proto_config(بنية connectdata *conn,
                                   بنية url_conn_سمون *m)
{
  إذا(!url_سمون_http_نسخه(conn, m))
    أرجع FALSE;

#که_تعریف_شوی USE_SSH
  إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    إذا(!ssh_config_matches(m->needle, conn))
      أرجع FALSE;
  }
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  وإلا إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    إذا(!Curl_ftp_conns_سمون(m->needle, conn))
      أرجع FALSE;
  }
#د_که_پای
  أرجع TRUE;
}

ساكن bool url_سمون_auth(بنية connectdata *conn,
                           بنية url_conn_سمون *m)
{
  إذا(!Curl_creds_same(m->needle->creds, conn->creds)) {
    إذا(m->needle->creds)
      أرجع FALSE;
    إذا(!Curl_creds_same(m->معلومات->state.creds, conn->creds))
      أرجع FALSE;
  }
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  إذا(m->needle->gssapi_delegation != conn->gssapi_delegation)
    أرجع FALSE;
#د_که_پای

  أرجع TRUE;
}

ساكن bool url_سمون_destination(بنية connectdata *conn,
                                  بنية url_conn_سمون *m)
{
  /* Different connect-to peers never match */
  إذا(!Curl_peer_same_destination(m->needle->له_لارې_peer, conn->له_لارې_peer))
    أرجع FALSE;

  إذا(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    أرجع FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  أرجع Curl_peer_same_destination(m->needle->origin, conn->origin);
}

ساكن bool url_سمون_ssl_config(بنية connectdata *conn,
                                 بنية url_conn_سمون *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  إذا(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_سمون(m->معلومات, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->معلومات, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }
  أرجع TRUE;
}

#که_تعریف_شوی USE_NTLM
ساكن bool url_سمون_auth_ntlm(بنية connectdata *conn,
                                بنية url_conn_سمون *m)
{
  إذا(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    إذا(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->معلومات->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->معلومات->state.origin))
      أرجع FALSE;
  }
  وإلا إذا(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    إذا(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->معلومات->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->معلومات->state.origin)))
      أرجع FALSE;
  }

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  إذا(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    إذا(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      أرجع FALSE;
  }
  وإلا إذا(m->want_proxy_ntlm_http) {
    إذا(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      أرجع FALSE;
  }
#د_که_پای
  أرجع TRUE;
}
#وإلا
#تعریف_کړه url_سمون_auth_ntlm(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#د_که_پای

#که_تعریف_شوی USE_SPNEGO
ساكن bool url_سمون_auth_nego(بنية connectdata *conn,
                                بنية url_conn_سمون *m)
{
  إذا(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    إذا(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->معلومات->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->معلومات->state.origin))
      أرجع FALSE;
  }
  وإلا إذا(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    إذا(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->معلومات->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->معلومات->state.origin)))
      أرجع FALSE;
  }

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  إذا(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    إذا(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      أرجع FALSE;
  }
  وإلا إذا(m->want_proxy_nego_http) {
    إذا(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      أرجع FALSE;
  }
#د_که_پای
  أرجع TRUE;
}
#وإلا
#تعریف_کړه url_سمون_auth_nego(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#د_که_پای

ساكن cpool_سمون_پایله url_سمون_conn(بنية connectdata *conn,
                                         فراغ *userdata)
{
  بنية url_conn_سمون *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  إذا(!url_سمون_connect_config(conn, m))
    أرجع CPOOL_سمون_CONT;

  /* match for destination and protocol? */
  إذا(!url_سمون_destination(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_fully_connected(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_multiplex_needs(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_ssl_use(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_proxy_use(conn, m))
    أرجع CPOOL_سمون_CONT;
  إذا(!url_سمون_ssl_config(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_http_multiplex(conn, m, &wait_pipe))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_auth(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_proto_config(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_auth_ntlm(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_auth_nego(conn, m))
    أرجع CPOOL_سمون_CONT;

  إذا(!url_سمون_multiplex_limits(conn, m))
    أرجع CPOOL_سمون_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  إذا(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->وموندل);
    m->wait_pipe = TRUE;
    أرجع CPOOL_سمون_وموندل;
  }

  إذا(m->معلومات->سېټ.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->معلومات, conn, &m->now);
    إذا(age_ms > m->معلومات->سېټ.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      أرجع CPOOL_سمون_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  إذا(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->معلومات, &m->now)) {
    infof(m->معلومات, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    أرجع CPOOL_سمون_CLOSE;
  }

  /* conn matches our needs. */
  m->وموندل = conn;
  أرجع CPOOL_سمون_وموندل;
}

ساكن bool url_سمون_پایله(فراغ *userdata)
{
  بنية url_conn_سمون *سمون = userdata;
  إذا(سمون->وموندل) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(سمون->معلومات, سمون->وموندل, TRUE);
    أرجع TRUE;
  }
  وإلا إذا(سمون->کتل_شوی_single_use_conn && !سمون->کتل_شوی_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    سمون->wait_pipe = FALSE;
  }
  وإلا إذا(سمون->کتل_شوی_pending_conn && سمون->معلومات->سېټ.pipewait) {
    infof(سمون->معلومات,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    سمون->wait_pipe = TRUE;
  }
  أرجع FALSE;
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
ساكن bool url_attach_existing(بنية Curl_easy *معلومات,
                                بنية connectdata *needle,
                                بنية url_conn_سمون *m,
                                ثابت بنية curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  أرجع Curl_cpool_پیدا_کړه(معلومات, needle->destination, TRUE, pnow,
                         url_سمون_conn, url_سمون_پایله, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
ساكن بنية connectdata *allocate_conn(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = curlx_calloc(1, حجم(بنية connectdata));
  إذا(!conn)
    أرجع عدم;

  /* and we setup a few fields in case we end up actually using this struct */
  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->recv_idx = 0; /* default for receiving transfer data */
  conn->send_idx = 0; /* default for sending transfer data */
  conn->connection_id = -1;    /* no ID */
  conn->cpid = UINT32_MAX;  /* not in connection pool yet */
  conn->attached_xfers = 0;
  conn->shutdown.پیل_ms[FIRSTSOCKET] =
    conn->shutdown.پیل_ms[SECONDARYSOCKET] = -1;

#که_نه_تعریف_شوی CURL_DISABLE_FTP
  conn->بټونه.ftp_use_epsv = معلومات->سېټ.ftp_use_epsv;
  conn->بټونه.ftp_use_eprt = معلومات->سېټ.ftp_use_eprt;
#د_که_پای
  conn->ip_نسخه = معلومات->سېټ.ipver;
  conn->بټونه.connect_only = (bool)معلومات->سېټ.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  إذا(CURL_EASY_STR(معلومات, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(معلومات, STRING_INTERFACE_OPT));
    إذا(!conn->interface_opts)
      اذهب error;
  }
#که_نه_تعریف_شوی CURL_DISABLE_BINDLOCAL
  conn->localportrange = معلومات->سېټ.localportrange;
  conn->localport = معلومات->سېټ.localport;
#د_که_پای

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = معلومات->سېټ.fclosesocket;
  conn->closesocket_client = معلومات->سېټ.closesocket_client;
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = معلومات->سېټ.gssapi_delegation;
#د_که_پای
  أرجع conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  أرجع عدم;
}

ساكن CURLcode url_سېټ_conn_scheme(بنية Curl_easy *معلومات,
                                    بنية connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ثابت بنية Curl_scheme *scheme = conn->origin->scheme;
  إذا(scheme->پرلپسې &&
     (معلومات->سېټ.allowed_protocols & scheme->protocol) &&
     (!معلومات->state.this_is_a_follow ||
       (معلومات->سېټ.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    أرجع CURLE_OK;
  }
  إذا(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(معلومات, "Protocol \"%s\" is not for transfers", scheme->name);
  وإلا
    failf(معلومات, "Protocol \"%s\" is disabled%s", scheme->name,
          معلومات->state.this_is_a_follow ? " (in redirect)" : "");
  أرجع CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_منزل_curlcode(CURLUcode uc)
{
  بدل(uc) {
  افتراضي:
    أرجع CURLE_URL_MALFORMAT;
  حالة CURLUE_UNSUPPORTED_SCHEME:
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  حالة CURLUE_بهر_د_حافظه:
    أرجع CURLE_بهر_د_حافظه;
  حالة CURLUE_USER_NOT_ALLOWED:
    أرجع CURLE_LOGIN_DENIED;
  }
}

#که_نه_تعریف_شوی CURL_DISABLE_HSTS
ساكن CURLcode hsts_upgrade(بنية Curl_easy *معلومات,
                             CURLU *uh,
                             uint16_t پورټ_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  إذا(معلومات->hsts && (معلومات->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(معلومات->hsts, معلومات->state.origin)) {
    حرف *url;
    CURLUcode uc;
    CURLcode پایله;

    uc = curl_url_سېټ(uh, CURLUPART_SCHEME, "https", 0);
    إذا(uc)
      أرجع Curl_uc_منزل_curlcode(uc);
    Curl_bufref_free(&معلومات->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    إذا(uc)
      أرجع Curl_uc_منزل_curlcode(uc);
    Curl_bufref_سېټ(&معلومات->state.url, url, 0, curl_free);

    پایله = Curl_peer_from_url(uh, معلومات, پورټ_override, scope_id,
                                &معلومات->state.origin);
    إذا(پایله)
      أرجع پایله;
    infof(معلومات, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  أرجع CURLE_OK;
}
#وإلا
#تعریف_کړه hsts_upgrade(x, y, z, a) CURLE_OK
#د_که_پای

ساكن bool str_has_ctrl(ثابت حرف *ننوت)
{
  إذا(ننوت) {
    ثابت بینښه حرف *str = (ثابت بینښه حرف *)ننوت;
    طالما(*str) {
      إذا(*str < 0x20)
        أرجع TRUE;
      str++;
    }
  }
  أرجع FALSE;
}

#که_نه_تعریف_شوی CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ساكن CURLcode url_سېټ_معلومات_creds_netrc(بنية Curl_easy *معلومات,
                                         بنية Curl_creds **pcreds)
{
  بنية Curl_creds *ncreds_بهر = عدم;
  CURLcode پایله = CURLE_OK;

  إذا(معلومات->سېټ.use_netrc) { /* not CURL_NETRC_IGNORED */
    بنية Curl_creds *ncreds_دننه = عدم;
    bool لټون_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    إذا(*pcreds) {
      بدل((*pcreds)->سرچینه) {
      حالة CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        لټون_netrc = FALSE;
        اكسر;
      حالة CREDS_URL: /* only apply when netrc is not required */
        إذا(معلومات->سېټ.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_دننه = *pcreds;
        }
        وإلا إذا(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_دننه = *pcreds;
        }
        وإلا
          لټون_netrc = FALSE;
        اكسر;
      افتراضي: /* ignore credentials from other sources */
        اكسر;
      }
    }

    إذا(!لټون_netrc)
      اذهب بهر;

    ret = Curl_netrc_لټون(معلومات, &معلومات->state.netrc,
                          معلومات->state.origin->hostname,
                          Curl_creds_user(ncreds_دننه),
                          CURL_EASY_STR(معلومات, STRING_NETRC_FILE),
                          &ncreds_بهر);
    DEBUGASSERT(!ret || !ncreds_بهر);
    إذا(ret == NETRC_بهر_د_حافظه) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
    وإلا إذا(ret && ((ret == NETRC_NO_سمون) ||
                    (معلومات->سېټ.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(معلومات, "Could not find host %s in the %s file; using defaults",
            معلومات->state.origin->hostname,
            (CURL_EASY_STR(معلومات, STRING_NETRC_FILE) ?
             CURL_EASY_STR(معلومات, STRING_NETRC_FILE) : ".netrc"));
    }
    وإلا إذا(ret) {
      ثابت حرف *m = Curl_netrc_strerror(ret);
      failf(معلومات, ".netrc error: %s", m);
      پایله = CURLE_READ_ERROR;
      اذهب بهر;
    }
    وإلا إذا(ncreds_بهر) {
      إذا(!(معلومات->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_بهر->user) ||
          str_has_ctrl(ncreds_بهر->passwd))) {
        failf(معلومات, "control code detected in .netrc credentials");
        پایله = CURLE_READ_ERROR;
        اذهب بهر;
      }
      CURL_TRC_M(معلومات, "netrc: using credentials for %s as %s",
                 معلومات->state.origin->hostname, ncreds_بهر->user);
      پایله = Curl_creds_یوځای(ncreds_بهر->user, ncreds_بهر->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      إذا(پایله)
        اذهب بهر;
      /* for updated strings, we update them in the URL */
      uc = curl_url_سېټ(معلومات->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_سېټ(معلومات->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      إذا(uc)
        پایله = Curl_uc_منزل_curlcode(uc);
    }
    وإلا
      DEBUGASSERT(0);
  }

#که_تعریف_شوی CURLVERBOSE
  Curl_creds_trace(معلومات, معلومات->state.creds, "transfer credentials");
#د_که_پای

بهر:
  Curl_creds_unlink(&ncreds_بهر);
  أرجع پایله;
}
#د_که_پای /* CURL_DISABLE_NETRC */

ساكن CURLcode url_سېټ_معلومات_creds(بنية Curl_easy *معلومات, CURLU *uh)
{
  بنية Curl_creds *newcreds = عدم;
  CURLcode پایله = CURLE_OK;

  إذا((CURL_EASY_STR(معلومات, STRING_USERNAME) ||
      CURL_EASY_STR(معلومات, STRING_PASSWORD) ||
      CURL_EASY_STR(معلومات, STRING_BEARER) ||
      CURL_EASY_STR(معلومات, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(معلومات, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_منزل_origin(معلومات, معلومات->state.origin)) {
    پایله = Curl_creds_create(CURL_EASY_STR(معلومات, STRING_USERNAME),
                               CURL_EASY_STR(معلومات, STRING_PASSWORD),
                               CURL_EASY_STR(معلومات, STRING_BEARER),
                               CURL_EASY_STR(معلومات, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(معلومات, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    إذا(پایله)
      اذهب بهر;
    إذا(newcreds &&
       !(معلومات->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(معلومات, "control code detected in credentials");
      پایله = CURLE_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  إذا(!newcreds || !Curl_creds_has_user(newcreds)) {
    حرف *user = عدم;
    حرف *passwd = عدم;
    حرف *udecoded = عدم;
    حرف *pdecoded = عدم;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    إذا(uc && (uc != CURLUE_NO_USER))
      پایله = Curl_uc_منزل_curlcode(uc);
    إذا(!پایله) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      إذا(uc && (uc != CURLUE_NO_PASSWORD))
        پایله = Curl_uc_منزل_curlcode(uc);
    }
    إذا(!پایله && user) {
      پایله = Curl_urldecode(user, 0, &udecoded, عدم,
                              (معلومات->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!پایله && passwd) {
      پایله = Curl_urldecode(passwd, 0, &pdecoded, عدم,
                              (معلومات->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!پایله)
      پایله = Curl_creds_یوځای(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    إذا(پایله) {
      failf(معلومات, "error extracting credentials from URL");
      اذهب بهر;
    }
  }

#که_نه_تعریف_شوی CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  پایله = url_سېټ_معلومات_creds_netrc(معلومات, &newcreds);
#د_که_پای /* CURL_DISABLE_NETRC */

بهر:
  إذا(!پایله && !Curl_creds_equal(معلومات->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&معلومات->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  أرجع پایله;
}

ساكن CURLcode url_سېټ_conn_origin_etc(بنية Curl_easy *معلومات,
                                        بنية connectdata *conn)
{
  CURLcode پایله = CURLE_OK;

  Curl_peer_link(&conn->origin, معلومات->state.origin);

  /* set the connection scheme */
  پایله = url_سېټ_conn_scheme(معلومات, conn);
  إذا(پایله)
    اذهب بهر;

  /* set the connection options */
  إذا(CURL_EASY_STR(معلومات, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(معلومات, STRING_OPTIONS));
    إذا(!conn->options) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }
  وإلا إذا(معلومات->state.پورته.options) {
    conn->options = curlx_strdup(معلومات->state.پورته.options);
    إذا(!conn->options) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب بهر;
    }
  }

بهر:
  أرجع پایله;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ساكن CURLcode تیاری_range(بنية Curl_easy *معلومات)
{
  بنية UrlState *s = &معلومات->state;
  s->resume_from = معلومات->سېټ.سېټ_resume_from;
  إذا(s->resume_from || CURL_EASY_STR(معلومات, STRING_سېټ_RANGE)) {
    إذا(s->rangestringalloc)
      curlx_free(s->range);

    إذا(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    وإلا
      s->range = curlx_strdup(CURL_EASY_STR(معلومات, STRING_سېټ_RANGE));

    إذا(!s->range)
      أرجع CURLE_بهر_د_حافظه;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  وإلا
    s->use_range = FALSE; /* disable range download */

  أرجع CURLE_OK;
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
ساكن CURLcode تیاری_connection_internals(بنية Curl_easy *معلومات,
                                           بنية connectdata *conn)
{
  بنية Curl_peer *peer = عدم;

  إذا(conn->scheme->پرلپسې->تیاری_connection) {
    CURLcode پایله = conn->scheme->پرلپسې->تیاری_connection(معلومات, conn);
    إذا(پایله)
      أرجع پایله;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  إذا(!peer)
    أرجع CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  إذا(peer->ipv6)
    إذا(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->پورټ);
    وإلا
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->پورټ);
  وإلا
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->پورټ);
  إذا(!conn->destination)
    أرجع CURLE_بهر_د_حافظه;

  Curl_strntolower(conn->destination, conn->destination,
                   مزي_اوږدوالی(conn->destination));

#که_تعریف_شوی USE_IPV6
  إذا(معلومات->سېټ.scope_id)
    conn->scope_id = معلومات->سېټ.scope_id;
  وإلا {
    peer = Curl_conn_get_لومړی_peer(conn, FIRSTSOCKET);
    إذا(!peer)
      أرجع CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#د_که_پای

  أرجع CURLE_OK;
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
CURLcode Curl_parse_login_details(ثابت حرف *login, ثابت size_t len,
                                  حرف **userp, حرف **passwdp,
                                  حرف **optionsp)
{
  حرف *ubuf = عدم;
  حرف *pbuf = عدم;
  ثابت حرف *psep = عدم;
  ثابت حرف *osep = عدم;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  إذا(optionsp)
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
  إذا(!ubuf)
    اذهب error;

  /* Clone the password portion buffer */
  إذا(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    إذا(!pbuf)
      اذهب error;
  }

  /* Allocate the options portion buffer */
  إذا(optionsp) {
    حرف *obuf = عدم;
    إذا(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      إذا(!obuf)
        اذهب error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  أرجع CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  أرجع CURLE_بهر_د_حافظه;
}

/*
 * Set the login details so they are available in the connection
 */
ساكن CURLcode url_سېټ_conn_login(بنية Curl_easy *معلومات,
                                   بنية connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  إذا((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, معلومات->state.origin);
    إذا(معلومات->state.creds)
      Curl_creds_link(&conn->creds, معلومات->state.creds);
    وإلا
      أرجع Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               عدم, عدم, عدم, CREDS_NONE, &conn->creds);
  }
  وإلا إذا(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, معلومات->state.origin);
    Curl_creds_link(&conn->creds, معلومات->state.creds);
  }

  أرجع CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
ساكن CURLcode parse_connect_منزل_string(بنية Curl_easy *معلومات,
                                        ثابت بنية Curl_peer *dest,
                                        ثابت حرف *conn_منزل_کرښه,
                                        بنية Curl_peer **pvia_dest)
{
  CURLcode پایله = CURLE_OK;
  ثابت حرف *ptr = conn_منزل_کرښه;
  bool host_سمون = FALSE;
  bool پورټ_سمون = FALSE;

  *pvia_dest = عدم;

  إذا(*ptr == ':') {
    /* an empty hostname always matches */
    host_سمون = TRUE;
    ptr++;
  }
  وإلا {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = مزي_اوږدوالی(dest->hostname);
    host_سمون = curl_strnequal(ptr, dest->hostname, hlen);
    إذا(!host_سمون && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = مزي_اوږدوالی(dest->user_hostname);
      host_سمون = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_سمون = host_سمون && ptr[hlen] == ':';
    إذا(host_سمون)
      ptr += hlen + 1;
  }

  إذا(host_سمون) {
    إذا(*ptr == ':') {
      /* an empty port always matches */
      پورټ_سمون = TRUE;
      ptr++;
    }
    وإلا {
      /* check whether the URL's port matches */
      ثابت حرف *ptr_بل = strchr(ptr, ':');
      إذا(ptr_بل) {
        curl_off_t پورټ_منزل_سمون;
        إذا(!curlx_str_شمېره(&ptr, &پورټ_منزل_سمون, 0xffff) &&
           ((uint16_t)پورټ_منزل_سمون == dest->پورټ)) {
          پورټ_سمون = TRUE;
        }
        ptr = ptr_بل + 1;
      }
    }
  }

  إذا(host_سمون && پورټ_سمون && ptr && *ptr)
    پایله = Curl_peer_from_connect_منزل(معلومات, dest, ptr, pvia_dest);

  أرجع پایله;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ساكن CURLcode url_سېټ_conn_peer(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  CURLcode پایله = CURLE_OK;
  بنية Curl_peer *origin = conn->origin;
  بنية Curl_peer *له_لارې_peer = عدم;
  بنية curl_slist *conn_منزل_ثبت = معلومات->سېټ.connect_منزل;

  DEBUGASSERT(!conn->له_لارې_peer);
  Curl_peer_unlink(&conn->له_لارې_peer);

  طالما(conn_منزل_ثبت && !له_لارې_peer) {
    پایله = parse_connect_منزل_string(معلومات, origin, conn_منزل_ثبت->معلومات,
                                     &له_لارې_peer);
    إذا(پایله)
      أرجع پایله;
    conn_منزل_ثبت = conn_منزل_ثبت->بل;
  }

#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  إذا(معلومات->asi && !له_لارې_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#که_تعریف_شوی DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#وإلا
      0
#د_که_پای
       )) {
    /* no connect_to match, try alt-svc! */
    تعداد alpnid srcalpnid = ALPN_none;
    bool موندنه = FALSE;
    بنية altsvc *as = عدم;
    صحيح allowed_alpns = ALPN_none;
    بنية http_negotiation *neg = &معلومات->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(معلومات, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#که_تعریف_شوی USE_HTTP3
    إذا(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#د_که_پای
#که_تعریف_شوی USE_HTTP2
    إذا(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#د_که_پای
    إذا(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (صحيح)معلومات->asi->flags;

    DEBUGF(infof(معلومات, "check Alt-Svc for host '%s'", origin->hostname));
#که_تعریف_شوی USE_HTTP3
    إذا(!موندنه && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      موندنه = Curl_altsvc_lookup(معلومات->asi,
                               origin, ALPN_h3, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }
#د_که_پای
#که_تعریف_شوی USE_HTTP2
    إذا(!موندنه && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_مخکینی_knowledge) {
      srcalpnid = ALPN_h2;
      موندنه = Curl_altsvc_lookup(معلومات->asi,
                               origin, ALPN_h2, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }
#د_که_پای
    إذا(!موندنه && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      موندنه = Curl_altsvc_lookup(معلومات->asi,
                               origin, ALPN_h1, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }

    إذا(موندنه && same_dest) {
      /* same destination, but more HTTPS version options */
      بدل(as->dst.alpnid) {
      حالة ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        اكسر;
      حالة ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        اكسر;
      حالة ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        اكسر;
      افتراضي: /* should not be possible */
        اكسر;
      }
    }
    وإلا إذا(موندنه) {
      پایله = Curl_peer_create(معلومات, conn->origin->scheme,
                                as->dst.host, as->dst.پورټ,
                                &له_لارې_peer);
      إذا(پایله)
        أرجع پایله;
      infof(معلومات, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->پورټ,
            Curl_alpnid2str(as->dst.alpnid),
            له_لارې_peer->hostname, له_لارې_peer->پورټ);
      conn->بټونه.altused = TRUE;
      إذا(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        بدل(as->dst.alpnid) {
        حالة ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          اكسر;
        حالة ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          اكسر;
        حالة ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          اكسر;
        افتراضي: /* should not be possible */
          اكسر;
        }
      }
    }
  }
#د_که_پای

  إذا(له_لارې_peer)
    conn->له_لارې_peer = له_لارې_peer;

  أرجع پایله;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
ساكن فراغ url_conn_reuse_adjust(بنية Curl_easy *معلومات,
                                  بنية connectdata *needle)
{
  بنية connectdata *conn = معلومات->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  إذا(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#د_که_پای

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
  Curl_peer_link(&conn->له_لارې_peer, needle->له_لارې_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->له_لارې_peer2, needle->له_لارې_peer2);
}

ساكن فراغ conn_meta_freeentry(فراغ *p)
{
  (فراغ)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

ساكن CURLcode url_create_needle(بنية Curl_easy *معلومات,
                                  بنية connectdata **pneedle)
{
  بنية connectdata *needle = عدم;
  CURLcode پایله = CURLE_OK;
  bool شبکه_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(معلومات);
  إذا(!needle) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_بایټونه,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  پایله = url_سېټ_conn_origin_etc(معلومات, needle);
  إذا(پایله)
    اذهب بهر;

  DEBUGASSERT(needle->origin);
  شبکه_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#که_تعریف_شوی USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  إذا(شبکه_scheme && CURL_EASY_STR(معلومات, STRING_UNIX_SOCKET_لار)) {
    پایله = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(معلومات, STRING_UNIX_SOCKET_لار),
      (bool)معلومات->سېټ.abstract_unix_socket, &needle->له_لارې_peer);
    إذا(پایله)
      اذهب بهر;
  }
#د_که_پای /* USE_UNIX_SOCKETS */

  إذا(شبکه_scheme && !needle->له_لارې_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    پایله = url_سېټ_conn_peer(معلومات, needle);
    إذا(پایله)
      اذهب بهر;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  إذا(Curl_peer_equal(needle->origin, needle->له_لارې_peer)) {
    Curl_peer_unlink(&needle->له_لارې_peer);
  }

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  إذا(شبکه_scheme &&
     (!needle->له_لارې_peer || !needle->له_لارې_peer->unix_socket)) {
    پایله = Curl_proxy_init_conn(معلومات, needle);
    إذا(پایله)
      اذهب بهر;
  }
#د_که_پای /* CURL_DISABLE_PROXY */

  پایله = url_سېټ_conn_login(معلومات, needle); /* default credentials */
  إذا(پایله)
    اذهب بهر;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  پایله = تیاری_connection_internals(معلومات, needle);
  إذا(پایله)
    اذهب بهر;

  إذا(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    إذا(معلومات->سېټ.ssl_enable_alpn)
      needle->بټونه.tls_enable_alpn = TRUE;
  }

  إذا(شبکه_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->بټونه.tcp_fastopen = معلومات->سېټ.tcp_fastopen;
#که_تعریف_شوی USE_UNIX_SOCKETS
    إذا(Curl_conn_get_لومړی_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#د_که_پای
  }

بهر:
  إذا(!پایله) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  وإلا {
    *pneedle = عدم;
    إذا(needle)
      Curl_conn_free(معلومات, needle);
  }
  أرجع پایله;
}

ساكن CURLcode url_سېټ_معلومات_origin_and_creds(بنية Curl_easy *معلومات)
{
  CURLcode پایله = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_سېټ_uh = (معلومات->سېټ.uh && !معلومات->state.this_is_a_follow);
  uint16_t پورټ_override = معلومات->state.allow_پورټ ? معلومات->سېټ.use_پورټ : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  إذا(!Curl_bufref_ptr(&معلومات->state.url)) {
    پایله = CURLE_URL_MALFORMAT;
    اذهب بهر;
  }

  پورته_free(معلومات); /* cleanup previous leftovers first */

  /* parse the URL */
  إذا(use_سېټ_uh)
    uh = معلومات->state.uh = curl_url_dup(معلومات->سېټ.uh);
  وإلا
    uh = معلومات->state.uh = curl_url();
  إذا(!uh) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  إذا(!use_سېټ_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    إذا(curl_strequal("file", CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&معلومات->state.url), عدم, 0, TRUE))
      prepend_sheme = TRUE;
    وإلا {
      uc = curl_url_سېټ(uh, CURLUPART_URL, Curl_bufref_ptr(&معلومات->state.url),
                        (بینښه صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (معلومات->سېټ.disallow_username_دننه_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (معلومات->سېټ.لار_as_is ? CURLU_لار_AS_IS : 0));
      إذا(uc) {
        failf(معلومات, "URL rejected: %s", curl_url_strerror(uc));
        پایله = Curl_uc_منزل_curlcode(uc);
        اذهب بهر;
      }

      إذا(CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        حرف *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        إذا(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        إذا(uc && uc != CURLUE_NO_SCHEME) {
          پایله = Curl_uc_منزل_curlcode(uc);
          اذهب بهر;
        }
      }
    }

    إذا(prepend_sheme) {
      حرف *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&معلومات->state.url));
      إذا(!url) {
        پایله = CURLE_بهر_د_حافظه;
        اذهب بهر;
      }
      uc = curl_url_سېټ(uh, CURLUPART_URL, url,
                        (بینښه صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (معلومات->سېټ.disallow_username_دننه_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (معلومات->سېټ.لار_as_is ? CURLU_لار_AS_IS : 0));
      curl_free(url);
      إذا(uc) {
        failf(معلومات, "URL rejected: %s", curl_url_strerror(uc));
        پایله = Curl_uc_منزل_curlcode(uc);
        اذهب بهر;
      }
    }
    {
      حرف *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_تشه);
      إذا(uc) {
        پایله = Curl_uc_منزل_curlcode(uc);
        اذهب بهر;
      }
      إذا(prepend_sheme)
        infof(معلومات, "Applied default protocol '%s'",
              CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_سېټ(&معلومات->state.url, newurl, 0, curl_free);
    }
  }

#که_تعریف_شوی USE_IPV6
  scope_id = معلومات->سېټ.scope_id;
#د_که_پای

  /* `uh` is now as the connection should use it, probably. */
  پایله = Curl_peer_from_url(uh, معلومات, پورټ_override, scope_id,
                              &معلومات->state.origin);
  إذا(پایله)
    اذهب بهر;
  /* The origin might get changed when HSTS applies */
  پایله = hsts_upgrade(معلومات, uh, پورټ_override, scope_id);
  إذا(پایله)
    اذهب بهر;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  إذا(!معلومات->state.initial_origin)
    Curl_peer_link(&معلومات->state.initial_origin, معلومات->state.origin);

  uc = curl_url_get(uh, CURLUPART_لار, &معلومات->state.پورته.لار, CURLU_URLENCODE);
  إذا(uc) {
    پایله = Curl_uc_منزل_curlcode(uc);
    اذهب بهر;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &معلومات->state.پورته.query,
                    CURLU_GET_تشه);
  إذا(uc && (uc != CURLUE_NO_QUERY)) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &معلومات->state.پورته.options,
                    CURLU_URLDECODE);
  إذا(uc && (uc != CURLUE_NO_OPTIONS)) {
    پایله = Curl_uc_منزل_curlcode(uc);
    اذهب بهر;
  }

  پایله = url_سېټ_معلومات_creds(معلومات, uh);
  إذا(پایله)
    اذهب بهر;

بهر:
  أرجع پایله;
}

ساكن CURLcode url_سمون_init(بنية Curl_easy *معلومات,
                               بنية connectdata *needle,
                               بنية url_conn_سمون *m)
{
  memset(m, 0, حجم(*m));
  m->معلومات = معلومات;
  m->needle = needle;
  m->now = *Curl_pgrs_now(معلومات);
  m->may_multiplex = xfer_may_multiplex(معلومات, needle);

#که_تعریف_شوی USE_NTLM
  m->want_ntlm_http =
    (معلومات->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_منزل_host(معلومات);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (معلومات->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#د_که_پای
#د_که_پای

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (معلومات->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_منزل_host(معلومات);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (معلومات->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#د_که_پای
#د_که_پای
  m->require_tls = معلومات->سېټ.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = معلومات->سېټ.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  أرجع Curl_ssl_filter_config_tmp_init(معلومات, needle->origin,
                                         &m->ssl_config,
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#وإلا
                                         عدم);
#د_که_پای
}

ساكن فراغ url_سمون_destroy(بنية url_conn_سمون *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#د_که_پای
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ساكن CURLcode url_پیدا_کړه_or_create_conn(بنية Curl_easy *معلومات,
                                        ثابت بنية curltime *pnow)
{
  بنية connectdata *needle = عدم;
  بنية url_conn_سمون سمون;
  bool سمون_initialized = FALSE;
  CURLcode پایله;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  پایله = url_create_needle(معلومات, &needle);
  إذا(پایله)
    اذهب بهر;
  DEBUGASSERT(needle);

  پایله = url_سمون_init(معلومات, needle, &سمون);
  إذا(پایله)
    اذهب بهر;
  سمون_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#که_نه_تعریف_شوی CURL_DISABLE_FILE
  إذا(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool بشپړ;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->پرلپسې->connect_it);
    معلومات->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    معلومات->info.conn_protocol = needle->scheme->protocol & CURLPROTO_ماسک;
    پایله = needle->scheme->پرلپسې->connect_it(معلومات, &بشپړ);
    إذا(پایله)
      اذهب بهر;

    /* Setup a "faked" transfer that will do nothing */
    پایله = Curl_cpool_جمع_کړه(معلومات, needle, 0, 0, pnow);
    Curl_attach_connection(معلومات, needle, TRUE);
    needle = عدم;
    إذا(!پایله) {
      /* Setup whatever necessary for a resumed transfer */
      پایله = تیاری_range(معلومات);
      إذا(!پایله) {
        Curl_xfer_تیاری_nop(معلومات);
        پایله = Curl_init_transfer(معلومات, معلومات->conn);
      }
    }

    إذا(پایله) {
      DEBUGASSERT(معلومات->conn->scheme->پرلپسې->بشپړ);
      /* we ignore the return code for the protocol-specific DONE */
      (فراغ)معلومات->conn->scheme->پرلپسې->بشپړ(معلومات, پایله, FALSE);
    }
    اذهب بهر;
  }
#د_که_پای

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  إذا((!معلومات->سېټ.reuse_fresh || معلومات->state.followlocation) &&
     !معلومات->سېټ.connect_only) {
    /* Ok, try to find and attach an existing one */
    إذا(url_attach_existing(معلومات, needle, &سمون, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      بنية connectdata *conn = معلومات->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->بټونه.reuse = TRUE;
      url_conn_reuse_adjust(معلومات, needle);

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
      infof(معلومات, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#وإلا
      infof(معلومات, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#د_که_پای
    }
  }

  إذا(!معلومات->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    إذا(سمون.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(معلومات, "Waiting on connection to negotiate possible multiplexing.");
      پایله = CURLE_NO_CONNECTION_شته;
      اذهب بهر;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    پایله = Curl_cpool_جمع_کړه(معلومات, needle,
                            معلومات->multi->max_ټول_connections,
                            معلومات->multi->max_host_connections,
                            pnow);
    إذا(پایله)
      اذهب بهر;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(معلومات, needle, TRUE);
    needle = عدم;

    /* Clone the SSL filter configs into the new connection. */
    پایله = Curl_ssl_conn_config_clone(&سمون.ssl_config,
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
                                        &سمون.proxy_ssl_config,
#وإلا
                                        عدم,
#د_که_پای
                                        معلومات->conn);
    إذا(پایله) {
      DEBUGF(infof(معلومات, "Error: clone connection SSL config"));
      اذهب بهر;
    }

#که_تعریف_شوی USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    إذا((معلومات->state.authhost.picked & CURLAUTH_NTLM) &&
       معلومات->state.authhost.بشپړ) {
      infof(معلومات, "NTLM picked AND auth done set, clear picked");
      معلومات->state.authhost.picked = CURLAUTH_NONE;
      معلومات->state.authhost.بشپړ = FALSE;
    }

    إذا((معلومات->state.authproxy.picked & CURLAUTH_NTLM) &&
       معلومات->state.authproxy.بشپړ) {
      infof(معلومات, "NTLM-proxy picked AND auth done set, clear picked");
      معلومات->state.authproxy.picked = CURLAUTH_NONE;
      معلومات->state.authproxy.بشپړ = FALSE;
    }
#د_که_پای
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  پایله = Curl_init_transfer(معلومات, معلومات->conn);
  إذا(پایله)
    اذهب بهر;

  /* Setup whatever necessary for a resumed transfer */
  پایله = تیاری_range(معلومات);
  إذا(پایله)
    اذهب بهر;

  /* persist the scheme and handler the transfer is using */
  معلومات->info.conn_scheme = معلومات->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  معلومات->info.conn_protocol = معلومات->conn->scheme->protocol & CURLPROTO_ماسک;
  معلومات->info.کارول_شوی_proxy =
#که_تعریف_شوی CURL_DISABLE_PROXY
    0
#وإلا
    (معلومات->conn->socks_proxy.peer || معلومات->conn->http_proxy.peer)
#د_که_پای
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  پایله = Curl_conn_ev_معلومات_تیاری(معلومات);

بهر:
  إذا(سمون_initialized)
    url_سمون_destroy(&سمون);
  إذا(needle)
    Curl_conn_free(معلومات, needle);
  DEBUGASSERT(پایله || معلومات->conn);
  أرجع پایله;
}

CURLcode Curl_connect(بنية Curl_easy *معلومات, bool *pconnected)
{
  CURLcode پایله;
  بنية connectdata *conn = عدم;
  ثابت بنية curltime *pnow = عدم;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&معلومات->req, معلومات);
  /* Determine the origin of the transfer and what credentials to use */
  پایله = url_سېټ_معلومات_origin_and_creds(معلومات);
  إذا(پایله)
    اذهب بهر;
  إذا(!معلومات->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(معلومات);
  Curl_pgrs_وخت_Was(معلومات, TIMER_POSTQUEUE, *pnow);
  پایله = url_پیدا_کړه_or_create_conn(معلومات, pnow);
  conn = معلومات->conn;
  إذا(پایله)
    اذهب بهر;
  إذا(!معلومات->conn) { /* just make really sure */
    DEBUGASSERT(0);
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  إذا(conn->بټونه.reuse) {
    إذا(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  وإلا إذا(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_وخت(معلومات, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  وإلا {
    پایله = Curl_conn_تیاری(معلومات, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    إذا(!پایله)
      پایله = Curl_سرلیکونه_init(معلومات);
    CURL_TRC_M(معلومات, "Curl_conn_setup() -> %d", (صحيح)پایله);
  }

بهر:
  إذا(پایله == CURLE_NO_CONNECTION_شته)
    DEBUGASSERT(!conn);

  إذا(پایله && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(معلومات);
    Curl_conn_close(معلومات, conn, TRUE);
  }

  أرجع پایله;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(بنية Curl_easy *معلومات, بنية connectdata *conn)
{
  CURLcode پایله;

  إذا(conn) {
    conn->بټونه.do_نور = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    إذا(معلومات->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      معلومات->state.wildcardmatch = FALSE;
  }

  معلومات->state.بشپړ = FALSE; /* *_done() is not called yet */

  معلومات->req.no_body = معلومات->سېټ.opt_no_body;
  إذا(معلومات->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    معلومات->state.httpreq = HTTPREQ_سر;

  پایله = Curl_req_پیل(&معلومات->req, معلومات);
  إذا(!پایله) {
    Curl_pgrsReset(معلومات);
  }
  أرجع پایله;
}

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)

فراغ Curl_معلومات_لومړیتوب_clear_state(بنية Curl_easy *معلومات)
{
  معلومات->state.وزن = 0;
}

#د_که_پای /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_سېټ(بنية connectdata *conn, ثابت حرف *کلی,
                            فراغ *meta_معلومات, Curl_meta_dtor *meta_dtor)
{
  إذا(!Curl_hash_add2(&conn->meta_hash, کلی, مزي_اوږدوالی(کلی) + 1,
                     meta_معلومات, meta_dtor)) {
    meta_dtor(کلی, مزي_اوږدوالی(کلی) + 1, meta_معلومات);
    أرجع CURLE_بهر_د_حافظه;
  }
  أرجع CURLE_OK;
}

فراغ Curl_conn_meta_remove(بنية connectdata *conn, ثابت حرف *کلی)
{
  Curl_hash_delete(&conn->meta_hash, کلی, مزي_اوږدوالی(کلی) + 1);
}

فراغ *Curl_conn_meta_get(بنية connectdata *conn, ثابت حرف *کلی)
{
  أرجع Curl_hash_pick(&conn->meta_hash, کلی, مزي_اوږدوالی(کلی) + 1);
}

بنية Curl_easy *Curl_get_admin(بنية Curl_easy *معلومات)
{
  بنية Curl_easy *admin;

  إذا(!معلومات->mid) /* already an admin handle */
    admin = معلومات;
  وإلا إذا(معلومات->multi)
    admin = معلومات->multi->admin;
  وإلا إذا(معلومات->multi_easy)
    admin = معلومات->multi_easy->admin;
  وإلا {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = معلومات;
  }
  إذا(admin != معلومات) {
    admin->سېټ.conn_max_idle_ms = معلومات->سېټ.conn_max_idle_ms;
    admin->سېټ.conn_max_age_ms = معلومات->سېټ.conn_max_age_ms;
    admin->سېټ.upkeep_interval_ms = معلومات->سېټ.upkeep_interval_ms;
    admin->سېټ.timeout = معلومات->سېټ.timeout;
    admin->سېټ.server_response_timeout = معلومات->سېټ.server_response_timeout;
    admin->سېټ.no_signal = معلومات->سېټ.no_signal;
  }
  أرجع admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  إذا(r1 && (r1 != CURLE_AGAIN))
    أرجع r1;
  إذا(r2 && (r2 != CURLE_AGAIN))
    أرجع r2;
  أرجع r1;
}
