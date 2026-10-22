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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_IPHLPAPI_H
#ضمن <Iphlpapi.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_UN_H
#ضمن <sys/un.h>
#جے_انت

#جے_بنا_تعریف HAVE_SOCKET
#غلطی "We cannot compile without socket() support"
#جے_انت

#إذا defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ضمن <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#جے_انت
#ضمن <iphlpapi.h>
#جے_انت

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
#إذا READBUFFER_ناپ < READBUFFER_MIN
# error READBUFFER_ناپ is too small
#جے_انت

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

فراغ Curl_freeset(بنية Curl_easy *انکڑے)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  تعداد dupblob j;

  CURL_EASY_STR_CLEAR0(انکڑے, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(انکڑے, STRING_کلید_PASSWD);
  CURL_EASY_STR_CLEAR0(انکڑے, STRING_BEARER);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(انکڑے, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(انکڑے, STRING_کلید_PASSWD_PROXY);
#جے_انت
  Curl_u8_strset_clear(&انکڑے->سیٹ.لڑیاں);
  curlx_safefree(انکڑے->سیٹ.str_copypostfields);

  لكل(j = (تعداد dupblob)0; j < BLOB_آخری; j++) {
    curlx_safefree(انکڑے->سیٹ.blobs[j]);
  }

  Curl_bufref_free(&انکڑے->state.referer);
  Curl_bufref_free(&انکڑے->state.url);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(انکڑے->سیٹ.mimepostp);
  curlx_safefree(انکڑے->سیٹ.mimepostp);
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_COOKIES
  curl_slist_free_all(انکڑے->state.cookielist);
  انکڑے->state.cookielist = عدم;
#جے_انت
}

/* free the URL pieces */
ساكن فراغ اتے_free(بنية Curl_easy *انکڑے)
{
  بنية urlpieces *اتے = &انکڑے->state.اتے;
  curlx_safefree(اتے->options);
  curlx_safefree(اتے->رستہ);
  curlx_safefree(اتے->query);
  curl_url_cleanup(انکڑے->state.uh);
  انکڑے->state.uh = عدم;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(بنية Curl_easy **datap)
{
  بنية Curl_easy *انکڑے;

  إذا(!datap || !*datap)
    أرجع CURLE_OK;

  انکڑے = *datap;
  *datap = عدم;

  إذا(!انکڑے->state.اندرونی && انکڑے->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(انکڑے->multi, انکڑے);
  }
  وإلا {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(انکڑے);
    إذا(!انکڑے->state.اندرونی && انکڑے->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(انکڑے->multi_easy);
      انکڑے->multi_easy = عدم;
    }
  }
  DEBUGASSERT(!انکڑے->conn || انکڑے->state.اندرونی);

  Curl_expire_clear_all(انکڑے); /* shut off any timers left */

  إذا(انکڑے->state.rangestringalloc)
    curlx_free(انکڑے->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(انکڑے);

  انکڑے->سیٹ.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  انکڑے->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&انکڑے->req, انکڑے);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(انکڑے);
  Curl_peer_unlink(&انکڑے->state.origin);
  Curl_peer_unlink(&انکڑے->state.initial_origin);
#جے_بنا_تعریف CURL_DISABLE_RTSP
  Curl_peer_unlink(&انکڑے->state.rtsp_session_origin);
#جے_انت
  Curl_ssl_free_certinfo(انکڑے);

  Curl_bufref_free(&انکڑے->state.referer);

  اتے_free(انکڑے);
  curlx_dyn_free(&انکڑے->state.headerb);
  Curl_flush_cookies(انکڑے, TRUE);
#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  Curl_altsvc_save(انکڑے, انکڑے->asi, CURL_EASY_STR(انکڑے, STRING_ALTSVC));
  Curl_altsvc_cleanup(&انکڑے->asi);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HSTS
  Curl_hsts_save(انکڑے, انکڑے->hsts, CURL_EASY_STR(انکڑے, STRING_HSTS));
  إذا(!انکڑے->share || !انکڑے->share->hsts)
    Curl_hsts_cleanup(&انکڑے->hsts);
  curl_slist_free_all(انکڑے->state.hstslist); /* clean up list */
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(انکڑے);
#جے_انت
  curlx_safefree(انکڑے->state.most_recent_ftp_entrypath);
  curlx_safefree(انکڑے->info.contenttype);
  curlx_safefree(انکڑے->info.wouldredirect);
  curlx_safefree(انکڑے->info.effective_url);

  /* No longer a dirty share, if it exists */
  إذا(Curl_share_easy_unlink(انکڑے))
    DEBUGASSERT(0);

  Curl_hash_destroy(&انکڑے->meta_hash);
  Curl_creds_unlink(&انکڑے->state.creds);
#جے_بنا_تعریف CURL_DISABLE_HTTP
  curlx_safefree(انکڑے->state.rangeline);
  curlx_safefree(انکڑے->state.http_host);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_COOKIES
  curlx_safefree(انکڑے->req.cookiehost);
#جے_انت

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(انکڑے->state.formp);
  curlx_safefree(انکڑے->state.formp);
#جے_انت

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&انکڑے->wildcard);
  Curl_freeset(انکڑے);
  Curl_سرخیاں_cleanup(انکڑے);
  Curl_netrc_cleanup(&انکڑے->state.netrc);
#جے_بنا_تعریف CURL_DISABLE_DIGEST_AUTH
  curlx_free(انکڑے->state.envproxy);
#جے_انت
  curlx_memzero(انکڑے, حجم(*انکڑے));
  curlx_free(انکڑے);
  أرجع CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
فراغ Curl_init_userdefined(بنية Curl_easy *انکڑے)
{
  بنية UserDefined *سیٹ = &انکڑے->سیٹ;

  سیٹ->باہرلا = stdout;  /* default output to stdout */
  سیٹ->وچ_سیٹ = stdin;  /* default input from stdin */
  سیٹ->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&انکڑے->سیٹ.لڑیاں);

#إذا defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#جے_انت
  /* use fwrite as default function to store output */
  سیٹ->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  سیٹ->fread_func_سیٹ = (curl_read_callback)fread;
#إذا defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#جے_انت
  سیٹ->is_fread_سیٹ = 0;

  سیٹ->seek_client = ZERO_NULL;

  سیٹ->filesize = -1;        /* we do not know the size */
  سیٹ->postfieldsize = -1;   /* unknown size */
  سیٹ->maxredirs = 30;       /* sensible default */

  سیٹ->طریقہ = HTTPREQ_GET; /* Default HTTP request */
#جے_بنا_تعریف CURL_DISABLE_RTSP
  سیٹ->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  سیٹ->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  سیٹ->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  سیٹ->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  سیٹ->ftp_filemethod = FTPFILE_MULTICWD;
  سیٹ->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#جے_انت
  سیٹ->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  سیٹ->ssl_ca_cache_timeout = 24 * 60 * 60;

  سیٹ->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&انکڑے->سیٹ.ssl);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  Curl_ssl_config_init(&انکڑے->سیٹ.proxy_ssl);
  سیٹ->proxyport = 0;
  سیٹ->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  سیٹ->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  سیٹ->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_DOH
  سیٹ->doh_verifyhost = TRUE;
  سیٹ->doh_verifypeer = TRUE;
#جے_انت
#جے_تعریف_شدہ USE_SSH
  /* defaults to any auth type */
  سیٹ->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  سیٹ->new_directory_perms = 0755; /* Default permissions */
#جے_انت

  سیٹ->new_file_perms = 0644;    /* Default permissions */
  سیٹ->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  سیٹ->redir_protocols = CURLPROTO_REDIR;

#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  سیٹ->socks5_gssapi_nec = FALSE;
#جے_انت

  /* set default minimum TLS version */
#جے_تعریف_شدہ USE_SSL
  Curl_setopt_SSLVERSION(انکڑے, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(انکڑے, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#جے_انت
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  سیٹ->wildcard_enabled = FALSE;
  سیٹ->chunk_bgn = ZERO_NULL;
  سیٹ->chunk_سرا = ZERO_NULL;
  سیٹ->fnmatch = ZERO_NULL;
#جے_انت
  سیٹ->tcp_keepalive = FALSE;
  سیٹ->tcp_keepintvl = 60;
  سیٹ->tcp_keepidle = 60;
  سیٹ->tcp_keepcnt = 9;
  سیٹ->tcp_fastopen = FALSE;
  سیٹ->tcp_nodelay = TRUE;
  سیٹ->ssl_enable_alpn = TRUE;
  سیٹ->expect_100_timeout = 1000L; /* Wait for a second by default. */
  سیٹ->sep_سرخیاں = TRUE; /* separated header lists by default */
  سیٹ->بفر_ناپ = READBUFFER_ناپ;
  سیٹ->upload_بفر_ناپ = UPLOADBUFFER_DEFAULT;
  سیٹ->upload_flags = CURLULFLAG_تکیا;
  سیٹ->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  سیٹ->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  سیٹ->maxconnects = DEFAULT_CONNCACHE_ناپ; /* for easy handles */
  سیٹ->conn_max_idle_ms = 118 * 1000;
  سیٹ->conn_max_age_ms = 24 * 3600 * 1000;
  سیٹ->http09_allowed = FALSE;
  سیٹ->httpwant = CURL_HTTP_نسخہ_NONE;
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  سیٹ->بھار = 0;
#جے_انت
  سیٹ->quick_exit = 0L;
#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
  سیٹ->ws_raw_منوال = FALSE;
  سیٹ->ws_no_auto_pong = FALSE;
#جے_انت
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
  بنية Curl_easy *انکڑے;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  انکڑے = curlx_calloc(1, حجم(بنية Curl_easy));
  إذا(!انکڑے) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  انکڑے->magic = CURLEASY_MAGIC_نمبر;
  /* most recent connection is not yet defined */
  انکڑے->state.آخری_conn_id = -1;
  انکڑے->state.آخری_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  انکڑے->id = -1;
  انکڑے->mid = UINT32_MAX;
  انکڑے->master_mid = UINT32_MAX;
  انکڑے->ترقی.hide = TRUE;

  Curl_hash_init(&انکڑے->meta_hash, 23, CURL_HASH_TYPE_بائٹاں,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_آخری <= UINT8_MAX);
  Curl_u8_strset_init(&انکڑے->سیٹ.لڑیاں);
  curlx_dyn_init(&انکڑے->state.headerb, CURL_MAX_HTTP_سرخی);
  Curl_bufref_init(&انکڑے->state.url);
  Curl_bufref_init(&انکڑے->state.referer);
  Curl_req_init(&انکڑے->req);
  Curl_initinfo(انکڑے);
#جے_بنا_تعریف CURL_DISABLE_HTTP
  Curl_llist_init(&انکڑے->state.httphdrs, عدم);
#جے_انت
  Curl_netrc_init(&انکڑے->state.netrc);
  Curl_init_userdefined(انکڑے);

  *curl = انکڑے;
  أرجع CURLE_OK;
}

فراغ Curl_conn_free(بنية Curl_easy *انکڑے, بنية connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  إذا(conn->scheme && conn->scheme->لگاتار->disconnect &&
     !conn->بٹ.shutdown_handler)
    conn->scheme->لگاتار->disconnect(انکڑے, conn, TRUE);

  لكل(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(انکڑے, conn, i);
  }

#جے_بنا_تعریف CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#جے_انت
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->راہیں_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->راہیں_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
ساكن bool xfer_may_multiplex(ثابت بنية Curl_easy *انکڑے,
                               ثابت بنية connectdata *conn)
{
#جے_بنا_تعریف CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  إذا((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->بٹ.protoconnstart || !conn->بٹ.close)) {

    إذا(Curl_multiplex_wanted(انکڑے->multi) &&
       (انکڑے->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      أرجع TRUE;
  }
#وإلا
  (فراغ)انکڑے;
  (فراغ)conn;
#جے_انت
  أرجع FALSE;
}

#جے_بنا_تعریف CURL_DISABLE_PROXY
ساكن bool proxy_info_matches(ثابت بنية proxy_info *انکڑے,
                               ثابت بنية proxy_info *needle)
{
  إذا((انکڑے->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(انکڑے->peer, needle->peer) &&
     Curl_creds_same(انکڑے->creds, needle->creds)) {
    أرجع TRUE;
  }
  أرجع FALSE;
}
#جے_انت

#جے_تعریف_شدہ USE_SSH
ساكن bool ssh_config_matches(بنية connectdata *one,
                               بنية connectdata *two)
{
  بنية ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  أرجع sshc1 && sshc2 && Curl_safecmp(sshc1->priv_کلید, sshc2->priv_کلید) &&
         Curl_safecmp(sshc1->pub_کلید, sshc2->pub_کلید);
}
#جے_انت

بنية url_conn_میل {
  بنية connectdata *لبھیا;
  بنية Curl_easy *انکڑے;
  بنية connectdata *needle;
  بنية curltime now;
  بنية ssl_filter_config ssl_config;
#جے_بنا_تعریف CURL_DISABLE_PROXY
  بنية ssl_filter_config proxy_ssl_config;
#جے_انت
  ثنائی_ہندسہ(may_multiplex);
  ثنائی_ہندسہ(want_ntlm_http);
  ثنائی_ہندسہ(want_proxy_ntlm_http);
  ثنائی_ہندسہ(want_nego_http);
  ثنائی_ہندسہ(want_proxy_nego_http);
  ثنائی_ہندسہ(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  ثنائی_ہندسہ(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  ثنائی_ہندسہ(wait_pipe);
  ثنائی_ہندسہ(تکیا_pending_conn);
  ثنائی_ہندسہ(تکیا_single_use_conn);
  ثنائی_ہندسہ(تکیا_multiplex_conn);
};

ساكن bool url_میل_connect_config(بنية connectdata *conn,
                                     بنية url_conn_میل *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  إذا(conn->بٹ.connect_only || conn->بٹ.close || conn->بٹ.no_reuse)
    أرجع FALSE;

  /* ip_version must match */
  إذا(m->انکڑے->سیٹ.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->انکڑے->سیٹ.ipver != conn->ip_نسخہ)
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

  إذا(!m->needle->راہیں_peer != !conn->راہیں_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    أرجع FALSE;

  أرجع TRUE;
}

ساكن bool url_میل_fully_connected(بنية connectdata *conn,
                                      بنية url_conn_میل *m)
{
  إذا(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->بٹ.upgrade_وچ_ترقی) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    إذا(m->may_multiplex) {
      m->تکیا_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->انکڑے, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_میل_multi(بنية connectdata *conn,
                            بنية url_conn_میل *m)
{
  إذا(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    إذا(conn->attached_multi != m->انکڑے->multi)
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_میل_multiplex_needs(بنية connectdata *conn,
                                      بنية url_conn_میل *m)
{
  إذا(CONN_INUSE(conn)) {
    إذا(!conn->بٹ.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->تکیا_single_use_conn = TRUE;
      أرجع FALSE;
    }
    m->تکیا_multiplex_conn = TRUE;
    إذا(!m->may_multiplex || !url_میل_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_میل_multiplex_limits(بنية connectdata *conn,
                                       بنية url_conn_میل *m)
{
  إذا(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->بٹ.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    إذا(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->انکڑے->multi)) {
      infof(m->انکڑے, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      أرجع FALSE;
    }
    إذا(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->انکڑے, conn, FIRSTSOCKET)) {
      infof(m->انکڑے, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      أرجع FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->انکڑے, "Multiplexed connection found");
  }
  أرجع TRUE;
}

ساكن bool url_میل_ssl_use(بنية connectdata *conn,
                              بنية url_conn_میل *m)
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
    إذا(!Curl_ssl_conn_config_میل(m->انکڑے, &m->ssl_config, conn, FALSE))
      أرجع FALSE;
  }
  وإلا إذا(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    أرجع FALSE;
  أرجع TRUE;
}

#جے_بنا_تعریف CURL_DISABLE_PROXY
ساكن bool url_میل_proxy_use(بنية connectdata *conn,
                                بنية url_conn_میل *m)
{
  إذا(m->needle->بٹ.origin_is_proxy != conn->بٹ.origin_is_proxy)
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    أرجع FALSE;

  إذا(m->انکڑے->سیٹ.socks5_auth_only && !conn->بٹ.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->انکڑے,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }

  إذا(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    إذا(!Curl_ssl_conn_config_میل(m->انکڑے, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->انکڑے,
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
#تعریف_کرو url_میل_proxy_use(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_HTTP
ساكن bool url_میل_http_multiplex(بنية connectdata *conn,
                                     بنية url_conn_میل *m,
                                     bool *pwait_pipe)
{
  إذا(m->may_multiplex &&
     (m->انکڑے->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_تکیا) {
    إذا(m->انکڑے->سیٹ.pipewait) {
      infof(m->انکڑے, "Server upgrade does not support multiplex yet, wait");
      m->لبھیا = عدم;
      *pwait_pipe = TRUE;
      أرجع TRUE; /* stop searching, we want to wait */
    }
    infof(m->انکڑے, "Server upgrade cannot be used");
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_میل_http_نسخہ(بنية connectdata *conn,
                                   بنية url_conn_میل *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  إذا((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    بدل(Curl_conn_http_نسخہ(m->انکڑے, conn)) {
    حالة 30:
      إذا(!(m->انکڑے->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->انکڑے, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    حالة 20:
      إذا(!(m->انکڑے->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->انکڑے, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    افتراضي:
      إذا(!(m->انکڑے->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->انکڑے, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    }
  }
  أرجع TRUE;
}
#وإلا
#تعریف_کرو url_میل_http_multiplex(c, m, w) ((فراغ)(c), (فراغ)(m), TRUE)
#تعریف_کرو url_میل_http_نسخہ(c, m)      ((فراغ)(c), (فراغ)(m), TRUE)
#جے_انت

ساكن bool url_میل_proto_config(بنية connectdata *conn,
                                   بنية url_conn_میل *m)
{
  إذا(!url_میل_http_نسخہ(conn, m))
    أرجع FALSE;

#جے_تعریف_شدہ USE_SSH
  إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    إذا(!ssh_config_matches(m->needle, conn))
      أرجع FALSE;
  }
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  وإلا إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    إذا(!Curl_ftp_conns_میل(m->needle, conn))
      أرجع FALSE;
  }
#جے_انت
  أرجع TRUE;
}

ساكن bool url_میل_auth(بنية connectdata *conn,
                           بنية url_conn_میل *m)
{
  إذا(!Curl_creds_same(m->needle->creds, conn->creds)) {
    إذا(m->needle->creds)
      أرجع FALSE;
    إذا(!Curl_creds_same(m->انکڑے->state.creds, conn->creds))
      أرجع FALSE;
  }
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  إذا(m->needle->gssapi_delegation != conn->gssapi_delegation)
    أرجع FALSE;
#جے_انت

  أرجع TRUE;
}

ساكن bool url_میل_destination(بنية connectdata *conn,
                                  بنية url_conn_میل *m)
{
  /* Different connect-to peers never match */
  إذا(!Curl_peer_same_destination(m->needle->راہیں_peer, conn->راہیں_peer))
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

ساكن bool url_میل_ssl_config(بنية connectdata *conn,
                                 بنية url_conn_میل *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  إذا(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_میل(m->انکڑے, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->انکڑے, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }
  أرجع TRUE;
}

#جے_تعریف_شدہ USE_NTLM
ساكن bool url_میل_auth_ntlm(بنية connectdata *conn,
                                بنية url_conn_میل *m)
{
  إذا(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    إذا(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->انکڑے->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->انکڑے->state.origin))
      أرجع FALSE;
  }
  وإلا إذا(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    إذا(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->انکڑے->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->انکڑے->state.origin)))
      أرجع FALSE;
  }

#جے_بنا_تعریف CURL_DISABLE_PROXY
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
#جے_انت
  أرجع TRUE;
}
#وإلا
#تعریف_کرو url_میل_auth_ntlm(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#جے_انت

#جے_تعریف_شدہ USE_SPNEGO
ساكن bool url_میل_auth_nego(بنية connectdata *conn,
                                بنية url_conn_میل *m)
{
  إذا(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    إذا(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->انکڑے->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->انکڑے->state.origin))
      أرجع FALSE;
  }
  وإلا إذا(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    إذا(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->انکڑے->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->انکڑے->state.origin)))
      أرجع FALSE;
  }

#جے_بنا_تعریف CURL_DISABLE_PROXY
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
#جے_انت
  أرجع TRUE;
}
#وإلا
#تعریف_کرو url_میل_auth_nego(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#جے_انت

ساكن cpool_میل_نتیجہ url_میل_conn(بنية connectdata *conn,
                                         فراغ *userdata)
{
  بنية url_conn_میل *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  إذا(!url_میل_connect_config(conn, m))
    أرجع CPOOL_میل_CONT;

  /* match for destination and protocol? */
  إذا(!url_میل_destination(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_fully_connected(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_multiplex_needs(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_ssl_use(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_proxy_use(conn, m))
    أرجع CPOOL_میل_CONT;
  إذا(!url_میل_ssl_config(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_http_multiplex(conn, m, &wait_pipe))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_auth(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_proto_config(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_auth_ntlm(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_auth_nego(conn, m))
    أرجع CPOOL_میل_CONT;

  إذا(!url_میل_multiplex_limits(conn, m))
    أرجع CPOOL_میل_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  إذا(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->لبھیا);
    m->wait_pipe = TRUE;
    أرجع CPOOL_میل_لبھیا;
  }

  إذا(m->انکڑے->سیٹ.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->انکڑے, conn, &m->now);
    إذا(age_ms > m->انکڑے->سیٹ.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      أرجع CPOOL_میل_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  إذا(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->انکڑے, &m->now)) {
    infof(m->انکڑے, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    أرجع CPOOL_میل_CLOSE;
  }

  /* conn matches our needs. */
  m->لبھیا = conn;
  أرجع CPOOL_میل_لبھیا;
}

ساكن bool url_میل_نتیجہ(فراغ *userdata)
{
  بنية url_conn_میل *میل = userdata;
  إذا(میل->لبھیا) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(میل->انکڑے, میل->لبھیا, TRUE);
    أرجع TRUE;
  }
  وإلا إذا(میل->تکیا_single_use_conn && !میل->تکیا_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    میل->wait_pipe = FALSE;
  }
  وإلا إذا(میل->تکیا_pending_conn && میل->انکڑے->سیٹ.pipewait) {
    infof(میل->انکڑے,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    میل->wait_pipe = TRUE;
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
ساكن bool url_attach_existing(بنية Curl_easy *انکڑے,
                                بنية connectdata *needle,
                                بنية url_conn_میل *m,
                                ثابت بنية curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  أرجع Curl_cpool_لبھو(انکڑے, needle->destination, TRUE, pnow,
                         url_میل_conn, url_میل_نتیجہ, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
ساكن بنية connectdata *allocate_conn(بنية Curl_easy *انکڑے)
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
  conn->shutdown.شروع_ms[FIRSTSOCKET] =
    conn->shutdown.شروع_ms[SECONDARYSOCKET] = -1;

#جے_بنا_تعریف CURL_DISABLE_FTP
  conn->بٹ.ftp_use_epsv = انکڑے->سیٹ.ftp_use_epsv;
  conn->بٹ.ftp_use_eprt = انکڑے->سیٹ.ftp_use_eprt;
#جے_انت
  conn->ip_نسخہ = انکڑے->سیٹ.ipver;
  conn->بٹ.connect_only = (bool)انکڑے->سیٹ.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  إذا(CURL_EASY_STR(انکڑے, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(انکڑے, STRING_INTERFACE_OPT));
    إذا(!conn->interface_opts)
      اذهب error;
  }
#جے_بنا_تعریف CURL_DISABLE_BINDLOCAL
  conn->localportrange = انکڑے->سیٹ.localportrange;
  conn->localport = انکڑے->سیٹ.localport;
#جے_انت

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = انکڑے->سیٹ.fclosesocket;
  conn->closesocket_client = انکڑے->سیٹ.closesocket_client;
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = انکڑے->سیٹ.gssapi_delegation;
#جے_انت
  أرجع conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  أرجع عدم;
}

ساكن CURLcode url_سیٹ_conn_scheme(بنية Curl_easy *انکڑے,
                                    بنية connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ثابت بنية Curl_scheme *scheme = conn->origin->scheme;
  إذا(scheme->لگاتار &&
     (انکڑے->سیٹ.allowed_protocols & scheme->protocol) &&
     (!انکڑے->state.this_is_a_follow ||
       (انکڑے->سیٹ.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    أرجع CURLE_OK;
  }
  إذا(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(انکڑے, "Protocol \"%s\" is not for transfers", scheme->name);
  وإلا
    failf(انکڑے, "Protocol \"%s\" is disabled%s", scheme->name,
          انکڑے->state.this_is_a_follow ? " (in redirect)" : "");
  أرجع CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_منزل_curlcode(CURLUcode uc)
{
  بدل(uc) {
  افتراضي:
    أرجع CURLE_URL_MALFORMAT;
  حالة CURLUE_UNSUPPORTED_SCHEME:
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  حالة CURLUE_باہرلا_دا_حافظہ:
    أرجع CURLE_باہرلا_دا_حافظہ;
  حالة CURLUE_USER_NOT_ALLOWED:
    أرجع CURLE_LOGIN_DENIED;
  }
}

#جے_بنا_تعریف CURL_DISABLE_HSTS
ساكن CURLcode hsts_upgrade(بنية Curl_easy *انکڑے,
                             CURLU *uh,
                             uint16_t پورٹ_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  إذا(انکڑے->hsts && (انکڑے->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(انکڑے->hsts, انکڑے->state.origin)) {
    حرف *url;
    CURLUcode uc;
    CURLcode نتیجہ;

    uc = curl_url_سیٹ(uh, CURLUPART_SCHEME, "https", 0);
    إذا(uc)
      أرجع Curl_uc_منزل_curlcode(uc);
    Curl_bufref_free(&انکڑے->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    إذا(uc)
      أرجع Curl_uc_منزل_curlcode(uc);
    Curl_bufref_سیٹ(&انکڑے->state.url, url, 0, curl_free);

    نتیجہ = Curl_peer_from_url(uh, انکڑے, پورٹ_override, scope_id,
                                &انکڑے->state.origin);
    إذا(نتیجہ)
      أرجع نتیجہ;
    infof(انکڑے, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  أرجع CURLE_OK;
}
#وإلا
#تعریف_کرو hsts_upgrade(x, y, z, a) CURLE_OK
#جے_انت

ساكن bool str_has_ctrl(ثابت حرف *داخلہ)
{
  إذا(داخلہ) {
    ثابت بنانشانی حرف *str = (ثابت بنانشانی حرف *)داخلہ;
    طالما(*str) {
      إذا(*str < 0x20)
        أرجع TRUE;
      str++;
    }
  }
  أرجع FALSE;
}

#جے_بنا_تعریف CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ساكن CURLcode url_سیٹ_انکڑے_creds_netrc(بنية Curl_easy *انکڑے,
                                         بنية Curl_creds **pcreds)
{
  بنية Curl_creds *ncreds_باہرلا = عدم;
  CURLcode نتیجہ = CURLE_OK;

  إذا(انکڑے->سیٹ.use_netrc) { /* not CURL_NETRC_IGNORED */
    بنية Curl_creds *ncreds_وچ = عدم;
    bool جانچ_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    إذا(*pcreds) {
      بدل((*pcreds)->سروت) {
      حالة CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        جانچ_netrc = FALSE;
        اكسر;
      حالة CREDS_URL: /* only apply when netrc is not required */
        إذا(انکڑے->سیٹ.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_وچ = *pcreds;
        }
        وإلا إذا(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_وچ = *pcreds;
        }
        وإلا
          جانچ_netrc = FALSE;
        اكسر;
      افتراضي: /* ignore credentials from other sources */
        اكسر;
      }
    }

    إذا(!جانچ_netrc)
      اذهب باہرلا;

    ret = Curl_netrc_جانچ(انکڑے, &انکڑے->state.netrc,
                          انکڑے->state.origin->hostname,
                          Curl_creds_user(ncreds_وچ),
                          CURL_EASY_STR(انکڑے, STRING_NETRC_FILE),
                          &ncreds_باہرلا);
    DEBUGASSERT(!ret || !ncreds_باہرلا);
    إذا(ret == NETRC_باہرلا_دا_حافظہ) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
    وإلا إذا(ret && ((ret == NETRC_NO_میل) ||
                    (انکڑے->سیٹ.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(انکڑے, "Could not find host %s in the %s file; using defaults",
            انکڑے->state.origin->hostname,
            (CURL_EASY_STR(انکڑے, STRING_NETRC_FILE) ?
             CURL_EASY_STR(انکڑے, STRING_NETRC_FILE) : ".netrc"));
    }
    وإلا إذا(ret) {
      ثابت حرف *m = Curl_netrc_strerror(ret);
      failf(انکڑے, ".netrc error: %s", m);
      نتیجہ = CURLE_READ_ERROR;
      اذهب باہرلا;
    }
    وإلا إذا(ncreds_باہرلا) {
      إذا(!(انکڑے->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_باہرلا->user) ||
          str_has_ctrl(ncreds_باہرلا->passwd))) {
        failf(انکڑے, "control code detected in .netrc credentials");
        نتیجہ = CURLE_READ_ERROR;
        اذهب باہرلا;
      }
      CURL_TRC_M(انکڑے, "netrc: using credentials for %s as %s",
                 انکڑے->state.origin->hostname, ncreds_باہرلا->user);
      نتیجہ = Curl_creds_ملاؤ(ncreds_باہرلا->user, ncreds_باہرلا->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      إذا(نتیجہ)
        اذهب باہرلا;
      /* for updated strings, we update them in the URL */
      uc = curl_url_سیٹ(انکڑے->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_سیٹ(انکڑے->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      إذا(uc)
        نتیجہ = Curl_uc_منزل_curlcode(uc);
    }
    وإلا
      DEBUGASSERT(0);
  }

#جے_تعریف_شدہ CURLVERBOSE
  Curl_creds_trace(انکڑے, انکڑے->state.creds, "transfer credentials");
#جے_انت

باہرلا:
  Curl_creds_unlink(&ncreds_باہرلا);
  أرجع نتیجہ;
}
#جے_انت /* CURL_DISABLE_NETRC */

ساكن CURLcode url_سیٹ_انکڑے_creds(بنية Curl_easy *انکڑے, CURLU *uh)
{
  بنية Curl_creds *newcreds = عدم;
  CURLcode نتیجہ = CURLE_OK;

  إذا((CURL_EASY_STR(انکڑے, STRING_USERNAME) ||
      CURL_EASY_STR(انکڑے, STRING_PASSWORD) ||
      CURL_EASY_STR(انکڑے, STRING_BEARER) ||
      CURL_EASY_STR(انکڑے, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(انکڑے, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_منزل_origin(انکڑے, انکڑے->state.origin)) {
    نتیجہ = Curl_creds_create(CURL_EASY_STR(انکڑے, STRING_USERNAME),
                               CURL_EASY_STR(انکڑے, STRING_PASSWORD),
                               CURL_EASY_STR(انکڑے, STRING_BEARER),
                               CURL_EASY_STR(انکڑے, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(انکڑے, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    إذا(نتیجہ)
      اذهب باہرلا;
    إذا(newcreds &&
       !(انکڑے->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(انکڑے, "control code detected in credentials");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      اذهب باہرلا;
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
      نتیجہ = Curl_uc_منزل_curlcode(uc);
    إذا(!نتیجہ) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      إذا(uc && (uc != CURLUE_NO_PASSWORD))
        نتیجہ = Curl_uc_منزل_curlcode(uc);
    }
    إذا(!نتیجہ && user) {
      نتیجہ = Curl_urldecode(user, 0, &udecoded, عدم,
                              (انکڑے->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!نتیجہ && passwd) {
      نتیجہ = Curl_urldecode(passwd, 0, &pdecoded, عدم,
                              (انکڑے->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!نتیجہ)
      نتیجہ = Curl_creds_ملاؤ(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    إذا(نتیجہ) {
      failf(انکڑے, "error extracting credentials from URL");
      اذهب باہرلا;
    }
  }

#جے_بنا_تعریف CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  نتیجہ = url_سیٹ_انکڑے_creds_netrc(انکڑے, &newcreds);
#جے_انت /* CURL_DISABLE_NETRC */

باہرلا:
  إذا(!نتیجہ && !Curl_creds_equal(انکڑے->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&انکڑے->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  أرجع نتیجہ;
}

ساكن CURLcode url_سیٹ_conn_origin_etc(بنية Curl_easy *انکڑے,
                                        بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;

  Curl_peer_link(&conn->origin, انکڑے->state.origin);

  /* set the connection scheme */
  نتیجہ = url_سیٹ_conn_scheme(انکڑے, conn);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* set the connection options */
  إذا(CURL_EASY_STR(انکڑے, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(انکڑے, STRING_OPTIONS));
    إذا(!conn->options) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }
  وإلا إذا(انکڑے->state.اتے.options) {
    conn->options = curlx_strdup(انکڑے->state.اتے.options);
    إذا(!conn->options) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

باہرلا:
  أرجع نتیجہ;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ساكن CURLcode تیاری_range(بنية Curl_easy *انکڑے)
{
  بنية UrlState *s = &انکڑے->state;
  s->resume_from = انکڑے->سیٹ.سیٹ_resume_from;
  إذا(s->resume_from || CURL_EASY_STR(انکڑے, STRING_سیٹ_RANGE)) {
    إذا(s->rangestringalloc)
      curlx_free(s->range);

    إذا(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    وإلا
      s->range = curlx_strdup(CURL_EASY_STR(انکڑے, STRING_سیٹ_RANGE));

    إذا(!s->range)
      أرجع CURLE_باہرلا_دا_حافظہ;

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
ساكن CURLcode تیاری_connection_internals(بنية Curl_easy *انکڑے,
                                           بنية connectdata *conn)
{
  بنية Curl_peer *peer = عدم;

  إذا(conn->scheme->لگاتار->تیاری_connection) {
    CURLcode نتیجہ = conn->scheme->لگاتار->تیاری_connection(انکڑے, conn);
    إذا(نتیجہ)
      أرجع نتیجہ;
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
        peer->hostname, peer->scopeid, peer->پورٹ);
    وإلا
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->پورٹ);
  وإلا
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->پورٹ);
  إذا(!conn->destination)
    أرجع CURLE_باہرلا_دا_حافظہ;

  Curl_strntolower(conn->destination, conn->destination,
                   لڑی_لمبائی(conn->destination));

#جے_تعریف_شدہ USE_IPV6
  إذا(انکڑے->سیٹ.scope_id)
    conn->scope_id = انکڑے->سیٹ.scope_id;
  وإلا {
    peer = Curl_conn_get_پہلا_peer(conn, FIRSTSOCKET);
    إذا(!peer)
      أرجع CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#جے_انت

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
  أرجع CURLE_باہرلا_دا_حافظہ;
}

/*
 * Set the login details so they are available in the connection
 */
ساكن CURLcode url_سیٹ_conn_login(بنية Curl_easy *انکڑے,
                                   بنية connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  إذا((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, انکڑے->state.origin);
    إذا(انکڑے->state.creds)
      Curl_creds_link(&conn->creds, انکڑے->state.creds);
    وإلا
      أرجع Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               عدم, عدم, عدم, CREDS_NONE, &conn->creds);
  }
  وإلا إذا(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, انکڑے->state.origin);
    Curl_creds_link(&conn->creds, انکڑے->state.creds);
  }

  أرجع CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
ساكن CURLcode parse_connect_منزل_string(بنية Curl_easy *انکڑے,
                                        ثابت بنية Curl_peer *dest,
                                        ثابت حرف *conn_منزل_سطر,
                                        بنية Curl_peer **pvia_dest)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn_منزل_سطر;
  bool host_میل = FALSE;
  bool پورٹ_میل = FALSE;

  *pvia_dest = عدم;

  إذا(*ptr == ':') {
    /* an empty hostname always matches */
    host_میل = TRUE;
    ptr++;
  }
  وإلا {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = لڑی_لمبائی(dest->hostname);
    host_میل = curl_strnequal(ptr, dest->hostname, hlen);
    إذا(!host_میل && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = لڑی_لمبائی(dest->user_hostname);
      host_میل = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_میل = host_میل && ptr[hlen] == ':';
    إذا(host_میل)
      ptr += hlen + 1;
  }

  إذا(host_میل) {
    إذا(*ptr == ':') {
      /* an empty port always matches */
      پورٹ_میل = TRUE;
      ptr++;
    }
    وإلا {
      /* check whether the URL's port matches */
      ثابت حرف *ptr_اگلا = strchr(ptr, ':');
      إذا(ptr_اگلا) {
        curl_off_t پورٹ_منزل_میل;
        إذا(!curlx_str_نمبر(&ptr, &پورٹ_منزل_میل, 0xffff) &&
           ((uint16_t)پورٹ_منزل_میل == dest->پورٹ)) {
          پورٹ_میل = TRUE;
        }
        ptr = ptr_اگلا + 1;
      }
    }
  }

  إذا(host_میل && پورٹ_میل && ptr && *ptr)
    نتیجہ = Curl_peer_from_connect_منزل(انکڑے, dest, ptr, pvia_dest);

  أرجع نتیجہ;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ساكن CURLcode url_سیٹ_conn_peer(بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية Curl_peer *origin = conn->origin;
  بنية Curl_peer *راہیں_peer = عدم;
  بنية curl_slist *conn_منزل_اندراج = انکڑے->سیٹ.connect_منزل;

  DEBUGASSERT(!conn->راہیں_peer);
  Curl_peer_unlink(&conn->راہیں_peer);

  طالما(conn_منزل_اندراج && !راہیں_peer) {
    نتیجہ = parse_connect_منزل_string(انکڑے, origin, conn_منزل_اندراج->انکڑے,
                                     &راہیں_peer);
    إذا(نتیجہ)
      أرجع نتیجہ;
    conn_منزل_اندراج = conn_منزل_اندراج->اگلا;
  }

#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  إذا(انکڑے->asi && !راہیں_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#جے_تعریف_شدہ DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#وإلا
      0
#جے_انت
       )) {
    /* no connect_to match, try alt-svc! */
    تعداد alpnid srcalpnid = ALPN_none;
    bool کامیابی = FALSE;
    بنية altsvc *as = عدم;
    صحيح allowed_alpns = ALPN_none;
    بنية http_negotiation *neg = &انکڑے->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(انکڑے, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#جے_تعریف_شدہ USE_HTTP3
    إذا(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#جے_انت
#جے_تعریف_شدہ USE_HTTP2
    إذا(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#جے_انت
    إذا(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (صحيح)انکڑے->asi->flags;

    DEBUGF(infof(انکڑے, "check Alt-Svc for host '%s'", origin->hostname));
#جے_تعریف_شدہ USE_HTTP3
    إذا(!کامیابی && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      کامیابی = Curl_altsvc_lookup(انکڑے->asi,
                               origin, ALPN_h3, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }
#جے_انت
#جے_تعریف_شدہ USE_HTTP2
    إذا(!کامیابی && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_پہلوں_knowledge) {
      srcalpnid = ALPN_h2;
      کامیابی = Curl_altsvc_lookup(انکڑے->asi,
                               origin, ALPN_h2, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }
#جے_انت
    إذا(!کامیابی && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      کامیابی = Curl_altsvc_lookup(انکڑے->asi,
                               origin, ALPN_h1, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }

    إذا(کامیابی && same_dest) {
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
    وإلا إذا(کامیابی) {
      نتیجہ = Curl_peer_create(انکڑے, conn->origin->scheme,
                                as->dst.host, as->dst.پورٹ,
                                &راہیں_peer);
      إذا(نتیجہ)
        أرجع نتیجہ;
      infof(انکڑے, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->پورٹ,
            Curl_alpnid2str(as->dst.alpnid),
            راہیں_peer->hostname, راہیں_peer->پورٹ);
      conn->بٹ.altused = TRUE;
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
#جے_انت

  إذا(راہیں_peer)
    conn->راہیں_peer = راہیں_peer;

  أرجع نتیجہ;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
ساكن فراغ url_conn_reuse_adjust(بنية Curl_easy *انکڑے,
                                  بنية connectdata *needle)
{
  بنية connectdata *conn = انکڑے->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  إذا(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#جے_بنا_تعریف CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#جے_انت

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
  Curl_peer_link(&conn->راہیں_peer, needle->راہیں_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->راہیں_peer2, needle->راہیں_peer2);
}

ساكن فراغ conn_meta_freeentry(فراغ *p)
{
  (فراغ)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

ساكن CURLcode url_create_needle(بنية Curl_easy *انکڑے,
                                  بنية connectdata **pneedle)
{
  بنية connectdata *needle = عدم;
  CURLcode نتیجہ = CURLE_OK;
  bool جال_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(انکڑے);
  إذا(!needle) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_بائٹاں,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  نتیجہ = url_سیٹ_conn_origin_etc(انکڑے, needle);
  إذا(نتیجہ)
    اذهب باہرلا;

  DEBUGASSERT(needle->origin);
  جال_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#جے_تعریف_شدہ USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  إذا(جال_scheme && CURL_EASY_STR(انکڑے, STRING_UNIX_SOCKET_رستہ)) {
    نتیجہ = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(انکڑے, STRING_UNIX_SOCKET_رستہ),
      (bool)انکڑے->سیٹ.abstract_unix_socket, &needle->راہیں_peer);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
#جے_انت /* USE_UNIX_SOCKETS */

  إذا(جال_scheme && !needle->راہیں_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    نتیجہ = url_سیٹ_conn_peer(انکڑے, needle);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  إذا(Curl_peer_equal(needle->origin, needle->راہیں_peer)) {
    Curl_peer_unlink(&needle->راہیں_peer);
  }

#جے_بنا_تعریف CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  إذا(جال_scheme &&
     (!needle->راہیں_peer || !needle->راہیں_peer->unix_socket)) {
    نتیجہ = Curl_proxy_init_conn(انکڑے, needle);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
#جے_انت /* CURL_DISABLE_PROXY */

  نتیجہ = url_سیٹ_conn_login(انکڑے, needle); /* default credentials */
  إذا(نتیجہ)
    اذهب باہرلا;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  نتیجہ = تیاری_connection_internals(انکڑے, needle);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    إذا(انکڑے->سیٹ.ssl_enable_alpn)
      needle->بٹ.tls_enable_alpn = TRUE;
  }

  إذا(جال_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->بٹ.tcp_fastopen = انکڑے->سیٹ.tcp_fastopen;
#جے_تعریف_شدہ USE_UNIX_SOCKETS
    إذا(Curl_conn_get_پہلا_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#جے_انت
  }

باہرلا:
  إذا(!نتیجہ) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  وإلا {
    *pneedle = عدم;
    إذا(needle)
      Curl_conn_free(انکڑے, needle);
  }
  أرجع نتیجہ;
}

ساكن CURLcode url_سیٹ_انکڑے_origin_and_creds(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_سیٹ_uh = (انکڑے->سیٹ.uh && !انکڑے->state.this_is_a_follow);
  uint16_t پورٹ_override = انکڑے->state.allow_پورٹ ? انکڑے->سیٹ.use_پورٹ : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  إذا(!Curl_bufref_ptr(&انکڑے->state.url)) {
    نتیجہ = CURLE_URL_MALFORMAT;
    اذهب باہرلا;
  }

  اتے_free(انکڑے); /* cleanup previous leftovers first */

  /* parse the URL */
  إذا(use_سیٹ_uh)
    uh = انکڑے->state.uh = curl_url_dup(انکڑے->سیٹ.uh);
  وإلا
    uh = انکڑے->state.uh = curl_url();
  إذا(!uh) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  إذا(!use_سیٹ_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    إذا(curl_strequal("file", CURL_EASY_STR(انکڑے, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&انکڑے->state.url), عدم, 0, TRUE))
      prepend_sheme = TRUE;
    وإلا {
      uc = curl_url_سیٹ(uh, CURLUPART_URL, Curl_bufref_ptr(&انکڑے->state.url),
                        (بنانشانی صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (انکڑے->سیٹ.disallow_username_وچ_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (انکڑے->سیٹ.رستہ_as_is ? CURLU_رستہ_AS_IS : 0));
      إذا(uc) {
        failf(انکڑے, "URL rejected: %s", curl_url_strerror(uc));
        نتیجہ = Curl_uc_منزل_curlcode(uc);
        اذهب باہرلا;
      }

      إذا(CURL_EASY_STR(انکڑے, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        حرف *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        إذا(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        إذا(uc && uc != CURLUE_NO_SCHEME) {
          نتیجہ = Curl_uc_منزل_curlcode(uc);
          اذهب باہرلا;
        }
      }
    }

    إذا(prepend_sheme) {
      حرف *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(انکڑے, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&انکڑے->state.url));
      إذا(!url) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      uc = curl_url_سیٹ(uh, CURLUPART_URL, url,
                        (بنانشانی صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (انکڑے->سیٹ.disallow_username_وچ_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (انکڑے->سیٹ.رستہ_as_is ? CURLU_رستہ_AS_IS : 0));
      curl_free(url);
      إذا(uc) {
        failf(انکڑے, "URL rejected: %s", curl_url_strerror(uc));
        نتیجہ = Curl_uc_منزل_curlcode(uc);
        اذهب باہرلا;
      }
    }
    {
      حرف *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_خالی_تھاواں);
      إذا(uc) {
        نتیجہ = Curl_uc_منزل_curlcode(uc);
        اذهب باہرلا;
      }
      إذا(prepend_sheme)
        infof(انکڑے, "Applied default protocol '%s'",
              CURL_EASY_STR(انکڑے, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_سیٹ(&انکڑے->state.url, newurl, 0, curl_free);
    }
  }

#جے_تعریف_شدہ USE_IPV6
  scope_id = انکڑے->سیٹ.scope_id;
#جے_انت

  /* `uh` is now as the connection should use it, probably. */
  نتیجہ = Curl_peer_from_url(uh, انکڑے, پورٹ_override, scope_id,
                              &انکڑے->state.origin);
  إذا(نتیجہ)
    اذهب باہرلا;
  /* The origin might get changed when HSTS applies */
  نتیجہ = hsts_upgrade(انکڑے, uh, پورٹ_override, scope_id);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  إذا(!انکڑے->state.initial_origin)
    Curl_peer_link(&انکڑے->state.initial_origin, انکڑے->state.origin);

  uc = curl_url_get(uh, CURLUPART_رستہ, &انکڑے->state.اتے.رستہ, CURLU_URLENCODE);
  إذا(uc) {
    نتیجہ = Curl_uc_منزل_curlcode(uc);
    اذهب باہرلا;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &انکڑے->state.اتے.query,
                    CURLU_GET_خالی_تھاواں);
  إذا(uc && (uc != CURLUE_NO_QUERY)) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &انکڑے->state.اتے.options,
                    CURLU_URLDECODE);
  إذا(uc && (uc != CURLUE_NO_OPTIONS)) {
    نتیجہ = Curl_uc_منزل_curlcode(uc);
    اذهب باہرلا;
  }

  نتیجہ = url_سیٹ_انکڑے_creds(انکڑے, uh);
  إذا(نتیجہ)
    اذهب باہرلا;

باہرلا:
  أرجع نتیجہ;
}

ساكن CURLcode url_میل_init(بنية Curl_easy *انکڑے,
                               بنية connectdata *needle,
                               بنية url_conn_میل *m)
{
  memset(m, 0, حجم(*m));
  m->انکڑے = انکڑے;
  m->needle = needle;
  m->now = *Curl_pgrs_now(انکڑے);
  m->may_multiplex = xfer_may_multiplex(انکڑے, needle);

#جے_تعریف_شدہ USE_NTLM
  m->want_ntlm_http =
    (انکڑے->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_منزل_host(انکڑے);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (انکڑے->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#جے_انت
#جے_انت

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (انکڑے->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_منزل_host(انکڑے);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (انکڑے->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#جے_انت
#جے_انت
  m->require_tls = انکڑے->سیٹ.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = انکڑے->سیٹ.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  أرجع Curl_ssl_filter_config_tmp_init(انکڑے, needle->origin,
                                         &m->ssl_config,
#جے_بنا_تعریف CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#وإلا
                                         عدم);
#جے_انت
}

ساكن فراغ url_میل_destroy(بنية url_conn_میل *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#جے_انت
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ساكن CURLcode url_لبھو_or_create_conn(بنية Curl_easy *انکڑے,
                                        ثابت بنية curltime *pnow)
{
  بنية connectdata *needle = عدم;
  بنية url_conn_میل میل;
  bool میل_initialized = FALSE;
  CURLcode نتیجہ;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  نتیجہ = url_create_needle(انکڑے, &needle);
  إذا(نتیجہ)
    اذهب باہرلا;
  DEBUGASSERT(needle);

  نتیجہ = url_میل_init(انکڑے, needle, &میل);
  إذا(نتیجہ)
    اذهب باہرلا;
  میل_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#جے_بنا_تعریف CURL_DISABLE_FILE
  إذا(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool پورا;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->لگاتار->connect_it);
    انکڑے->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    انکڑے->info.conn_protocol = needle->scheme->protocol & CURLPROTO_نقاب;
    نتیجہ = needle->scheme->لگاتار->connect_it(انکڑے, &پورا);
    إذا(نتیجہ)
      اذهب باہرلا;

    /* Setup a "faked" transfer that will do nothing */
    نتیجہ = Curl_cpool_جوڑو(انکڑے, needle, 0, 0, pnow);
    Curl_attach_connection(انکڑے, needle, TRUE);
    needle = عدم;
    إذا(!نتیجہ) {
      /* Setup whatever necessary for a resumed transfer */
      نتیجہ = تیاری_range(انکڑے);
      إذا(!نتیجہ) {
        Curl_xfer_تیاری_nop(انکڑے);
        نتیجہ = Curl_init_transfer(انکڑے, انکڑے->conn);
      }
    }

    إذا(نتیجہ) {
      DEBUGASSERT(انکڑے->conn->scheme->لگاتار->پورا);
      /* we ignore the return code for the protocol-specific DONE */
      (فراغ)انکڑے->conn->scheme->لگاتار->پورا(انکڑے, نتیجہ, FALSE);
    }
    اذهب باہرلا;
  }
#جے_انت

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  إذا((!انکڑے->سیٹ.reuse_fresh || انکڑے->state.followlocation) &&
     !انکڑے->سیٹ.connect_only) {
    /* Ok, try to find and attach an existing one */
    إذا(url_attach_existing(انکڑے, needle, &میل, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      بنية connectdata *conn = انکڑے->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->بٹ.reuse = TRUE;
      url_conn_reuse_adjust(انکڑے, needle);

#جے_بنا_تعریف CURL_DISABLE_PROXY
      infof(انکڑے, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#وإلا
      infof(انکڑے, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#جے_انت
    }
  }

  إذا(!انکڑے->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    إذا(میل.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(انکڑے, "Waiting on connection to negotiate possible multiplexing.");
      نتیجہ = CURLE_NO_CONNECTION_دستیاب;
      اذهب باہرلا;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    نتیجہ = Curl_cpool_جوڑو(انکڑے, needle,
                            انکڑے->multi->max_کل_connections,
                            انکڑے->multi->max_host_connections,
                            pnow);
    إذا(نتیجہ)
      اذهب باہرلا;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(انکڑے, needle, TRUE);
    needle = عدم;

    /* Clone the SSL filter configs into the new connection. */
    نتیجہ = Curl_ssl_conn_config_clone(&میل.ssl_config,
#جے_بنا_تعریف CURL_DISABLE_PROXY
                                        &میل.proxy_ssl_config,
#وإلا
                                        عدم,
#جے_انت
                                        انکڑے->conn);
    إذا(نتیجہ) {
      DEBUGF(infof(انکڑے, "Error: clone connection SSL config"));
      اذهب باہرلا;
    }

#جے_تعریف_شدہ USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    إذا((انکڑے->state.authhost.picked & CURLAUTH_NTLM) &&
       انکڑے->state.authhost.پورا) {
      infof(انکڑے, "NTLM picked AND auth done set, clear picked");
      انکڑے->state.authhost.picked = CURLAUTH_NONE;
      انکڑے->state.authhost.پورا = FALSE;
    }

    إذا((انکڑے->state.authproxy.picked & CURLAUTH_NTLM) &&
       انکڑے->state.authproxy.پورا) {
      infof(انکڑے, "NTLM-proxy picked AND auth done set, clear picked");
      انکڑے->state.authproxy.picked = CURLAUTH_NONE;
      انکڑے->state.authproxy.پورا = FALSE;
    }
#جے_انت
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  نتیجہ = Curl_init_transfer(انکڑے, انکڑے->conn);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* Setup whatever necessary for a resumed transfer */
  نتیجہ = تیاری_range(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* persist the scheme and handler the transfer is using */
  انکڑے->info.conn_scheme = انکڑے->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  انکڑے->info.conn_protocol = انکڑے->conn->scheme->protocol & CURLPROTO_نقاب;
  انکڑے->info.ورتیا_proxy =
#جے_تعریف_شدہ CURL_DISABLE_PROXY
    0
#وإلا
    (انکڑے->conn->socks_proxy.peer || انکڑے->conn->http_proxy.peer)
#جے_انت
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  نتیجہ = Curl_conn_ev_انکڑے_تیاری(انکڑے);

باہرلا:
  إذا(میل_initialized)
    url_میل_destroy(&میل);
  إذا(needle)
    Curl_conn_free(انکڑے, needle);
  DEBUGASSERT(نتیجہ || انکڑے->conn);
  أرجع نتیجہ;
}

CURLcode Curl_connect(بنية Curl_easy *انکڑے, bool *pconnected)
{
  CURLcode نتیجہ;
  بنية connectdata *conn = عدم;
  ثابت بنية curltime *pnow = عدم;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&انکڑے->req, انکڑے);
  /* Determine the origin of the transfer and what credentials to use */
  نتیجہ = url_سیٹ_انکڑے_origin_and_creds(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(!انکڑے->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(انکڑے);
  Curl_pgrs_ویلا_Was(انکڑے, TIMER_POSTQUEUE, *pnow);
  نتیجہ = url_لبھو_or_create_conn(انکڑے, pnow);
  conn = انکڑے->conn;
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(!انکڑے->conn) { /* just make really sure */
    DEBUGASSERT(0);
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  إذا(conn->بٹ.reuse) {
    إذا(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  وإلا إذا(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_ویلا(انکڑے, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  وإلا {
    نتیجہ = Curl_conn_تیاری(انکڑے, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    إذا(!نتیجہ)
      نتیجہ = Curl_سرخیاں_init(انکڑے);
    CURL_TRC_M(انکڑے, "Curl_conn_setup() -> %d", (صحيح)نتیجہ);
  }

باہرلا:
  إذا(نتیجہ == CURLE_NO_CONNECTION_دستیاب)
    DEBUGASSERT(!conn);

  إذا(نتیجہ && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(انکڑے);
    Curl_conn_close(انکڑے, conn, TRUE);
  }

  أرجع نتیجہ;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(بنية Curl_easy *انکڑے, بنية connectdata *conn)
{
  CURLcode نتیجہ;

  إذا(conn) {
    conn->بٹ.do_ہور = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    إذا(انکڑے->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      انکڑے->state.wildcardmatch = FALSE;
  }

  انکڑے->state.پورا = FALSE; /* *_done() is not called yet */

  انکڑے->req.no_body = انکڑے->سیٹ.opt_no_body;
  إذا(انکڑے->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    انکڑے->state.httpreq = HTTPREQ_سر;

  نتیجہ = Curl_req_شروع(&انکڑے->req, انکڑے);
  إذا(!نتیجہ) {
    Curl_pgrsReset(انکڑے);
  }
  أرجع نتیجہ;
}

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)

فراغ Curl_انکڑے_ترجیح_clear_state(بنية Curl_easy *انکڑے)
{
  انکڑے->state.بھار = 0;
}

#جے_انت /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_سیٹ(بنية connectdata *conn, ثابت حرف *کلید,
                            فراغ *meta_انکڑے, Curl_meta_dtor *meta_dtor)
{
  إذا(!Curl_hash_add2(&conn->meta_hash, کلید, لڑی_لمبائی(کلید) + 1,
                     meta_انکڑے, meta_dtor)) {
    meta_dtor(کلید, لڑی_لمبائی(کلید) + 1, meta_انکڑے);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }
  أرجع CURLE_OK;
}

فراغ Curl_conn_meta_remove(بنية connectdata *conn, ثابت حرف *کلید)
{
  Curl_hash_delete(&conn->meta_hash, کلید, لڑی_لمبائی(کلید) + 1);
}

فراغ *Curl_conn_meta_get(بنية connectdata *conn, ثابت حرف *کلید)
{
  أرجع Curl_hash_pick(&conn->meta_hash, کلید, لڑی_لمبائی(کلید) + 1);
}

بنية Curl_easy *Curl_get_admin(بنية Curl_easy *انکڑے)
{
  بنية Curl_easy *admin;

  إذا(!انکڑے->mid) /* already an admin handle */
    admin = انکڑے;
  وإلا إذا(انکڑے->multi)
    admin = انکڑے->multi->admin;
  وإلا إذا(انکڑے->multi_easy)
    admin = انکڑے->multi_easy->admin;
  وإلا {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = انکڑے;
  }
  إذا(admin != انکڑے) {
    admin->سیٹ.conn_max_idle_ms = انکڑے->سیٹ.conn_max_idle_ms;
    admin->سیٹ.conn_max_age_ms = انکڑے->سیٹ.conn_max_age_ms;
    admin->سیٹ.upkeep_interval_ms = انکڑے->سیٹ.upkeep_interval_ms;
    admin->سیٹ.timeout = انکڑے->سیٹ.timeout;
    admin->سیٹ.server_response_timeout = انکڑے->سیٹ.server_response_timeout;
    admin->سیٹ.no_signal = انکڑے->سیٹ.no_signal;
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
