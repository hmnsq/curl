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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_IPHLPAPI_H
#ضمن <Iphlpapi.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_UN_H
#ضمن <sys/un.h>
#پایان_اگر

#اگر_تعریف_نشده HAVE_SOCKET
#خطا "We cannot compile without socket() support"
#پایان_اگر

#إذا defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ضمن <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#پایان_اگر
#ضمن <iphlpapi.h>
#پایان_اگر

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
#پایان_اگر

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

فراغ Curl_freeset(بنية Curl_easy *داده)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  تعداد dupblob j;

  CURL_EASY_STR_CLEAR0(داده, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(داده, STRING_کلید_PASSWD);
  CURL_EASY_STR_CLEAR0(داده, STRING_BEARER);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(داده, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(داده, STRING_کلید_PASSWD_PROXY);
#پایان_اگر
  Curl_u8_strset_clear(&داده->مجموعه.رشتهها);
  curlx_safefree(داده->مجموعه.str_copypostfields);

  لكل(j = (تعداد dupblob)0; j < BLOB_آخرین; j++) {
    curlx_safefree(داده->مجموعه.blobs[j]);
  }

  Curl_bufref_free(&داده->state.referer);
  Curl_bufref_free(&داده->state.url);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(داده->مجموعه.mimepostp);
  curlx_safefree(داده->مجموعه.mimepostp);
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  curl_slist_free_all(داده->state.cookielist);
  داده->state.cookielist = عدم;
#پایان_اگر
}

/* free the URL pieces */
ساكن فراغ به_بالا_free(بنية Curl_easy *داده)
{
  بنية urlpieces *به_بالا = &داده->state.به_بالا;
  curlx_safefree(به_بالا->options);
  curlx_safefree(به_بالا->مسیر);
  curlx_safefree(به_بالا->query);
  curl_url_cleanup(داده->state.uh);
  داده->state.uh = عدم;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(بنية Curl_easy **datap)
{
  بنية Curl_easy *داده;

  إذا(!datap || !*datap)
    أرجع CURLE_OK;

  داده = *datap;
  *datap = عدم;

  إذا(!داده->state.داخلی && داده->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(داده->multi, داده);
  }
  وإلا {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(داده);
    إذا(!داده->state.داخلی && داده->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(داده->multi_easy);
      داده->multi_easy = عدم;
    }
  }
  DEBUGASSERT(!داده->conn || داده->state.داخلی);

  Curl_expire_clear_all(داده); /* shut off any timers left */

  إذا(داده->state.rangestringalloc)
    curlx_free(داده->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(داده);

  داده->مجموعه.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  داده->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&داده->req, داده);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(داده);
  Curl_peer_unlink(&داده->state.origin);
  Curl_peer_unlink(&داده->state.initial_origin);
#اگر_تعریف_نشده CURL_DISABLE_RTSP
  Curl_peer_unlink(&داده->state.rtsp_session_origin);
#پایان_اگر
  Curl_ssl_free_certinfo(داده);

  Curl_bufref_free(&داده->state.referer);

  به_بالا_free(داده);
  curlx_dyn_free(&داده->state.headerb);
  Curl_flush_cookies(داده, TRUE);
#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
  Curl_altsvc_save(داده, داده->asi, CURL_EASY_STR(داده, STRING_ALTSVC));
  Curl_altsvc_cleanup(&داده->asi);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HSTS
  Curl_hsts_save(داده, داده->hsts, CURL_EASY_STR(داده, STRING_HSTS));
  إذا(!داده->share || !داده->share->hsts)
    Curl_hsts_cleanup(&داده->hsts);
  curl_slist_free_all(داده->state.hstslist); /* clean up list */
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(داده);
#پایان_اگر
  curlx_safefree(داده->state.most_recent_ftp_entrypath);
  curlx_safefree(داده->info.contenttype);
  curlx_safefree(داده->info.wouldredirect);
  curlx_safefree(داده->info.effective_url);

  /* No longer a dirty share, if it exists */
  إذا(Curl_share_easy_unlink(داده))
    DEBUGASSERT(0);

  Curl_hash_destroy(&داده->meta_hash);
  Curl_creds_unlink(&داده->state.creds);
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  curlx_safefree(داده->state.rangeline);
  curlx_safefree(داده->state.http_host);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
  curlx_safefree(داده->req.cookiehost);
#پایان_اگر

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(داده->state.formp);
  curlx_safefree(داده->state.formp);
#پایان_اگر

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&داده->wildcard);
  Curl_freeset(داده);
  Curl_سرآیندها_cleanup(داده);
  Curl_netrc_cleanup(&داده->state.netrc);
#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
  curlx_free(داده->state.envproxy);
#پایان_اگر
  curlx_memzero(داده, حجم(*داده));
  curlx_free(داده);
  أرجع CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
فراغ Curl_init_userdefined(بنية Curl_easy *داده)
{
  بنية UserDefined *مجموعه = &داده->مجموعه;

  مجموعه->بیرون = stdout;  /* default output to stdout */
  مجموعه->درون_مجموعه = stdin;  /* default input from stdin */
  مجموعه->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&داده->مجموعه.رشتهها);

#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wcast-function-type-strict"
#پایان_اگر
  /* use fwrite as default function to store output */
  مجموعه->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  مجموعه->fread_func_مجموعه = (curl_read_callback)fread;
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic برداشت
#پایان_اگر
  مجموعه->is_fread_مجموعه = 0;

  مجموعه->seek_client = ZERO_NULL;

  مجموعه->filesize = -1;        /* we do not know the size */
  مجموعه->postfieldsize = -1;   /* unknown size */
  مجموعه->maxredirs = 30;       /* sensible default */

  مجموعه->روش = HTTPREQ_GET; /* Default HTTP request */
#اگر_تعریف_نشده CURL_DISABLE_RTSP
  مجموعه->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_FTP
  مجموعه->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  مجموعه->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  مجموعه->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  مجموعه->ftp_filemethod = FTPFILE_MULTICWD;
  مجموعه->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#پایان_اگر
  مجموعه->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  مجموعه->ssl_ca_cache_timeout = 24 * 60 * 60;

  مجموعه->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&داده->مجموعه.ssl);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  Curl_ssl_config_init(&داده->مجموعه.proxy_ssl);
  مجموعه->proxyport = 0;
  مجموعه->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  مجموعه->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  مجموعه->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_DOH
  مجموعه->doh_verifyhost = TRUE;
  مجموعه->doh_verifypeer = TRUE;
#پایان_اگر
#اگر_تعریف_شده USE_SSH
  /* defaults to any auth type */
  مجموعه->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  مجموعه->new_directory_perms = 0755; /* Default permissions */
#پایان_اگر

  مجموعه->new_file_perms = 0644;    /* Default permissions */
  مجموعه->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  مجموعه->redir_protocols = CURLPROTO_REDIR;

#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  مجموعه->socks5_gssapi_nec = FALSE;
#پایان_اگر

  /* set default minimum TLS version */
#اگر_تعریف_شده USE_SSL
  Curl_setopt_SSLVERSION(داده, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(داده, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#پایان_اگر
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_FTP
  مجموعه->wildcard_enabled = FALSE;
  مجموعه->chunk_bgn = ZERO_NULL;
  مجموعه->chunk_انتها = ZERO_NULL;
  مجموعه->fnmatch = ZERO_NULL;
#پایان_اگر
  مجموعه->tcp_keepalive = FALSE;
  مجموعه->tcp_keepintvl = 60;
  مجموعه->tcp_keepidle = 60;
  مجموعه->tcp_keepcnt = 9;
  مجموعه->tcp_fastopen = FALSE;
  مجموعه->tcp_nodelay = TRUE;
  مجموعه->ssl_enable_alpn = TRUE;
  مجموعه->expect_100_timeout = 1000L; /* Wait for a second by default. */
  مجموعه->sep_سرآیندها = TRUE; /* separated header lists by default */
  مجموعه->میانگیر_اندازه = READBUFFER_اندازه;
  مجموعه->upload_میانگیر_اندازه = UPLOADBUFFER_DEFAULT;
  مجموعه->upload_flags = CURLULFLAG_دیده;
  مجموعه->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  مجموعه->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  مجموعه->maxconnects = DEFAULT_CONNCACHE_اندازه; /* for easy handles */
  مجموعه->conn_max_idle_ms = 118 * 1000;
  مجموعه->conn_max_age_ms = 24 * 3600 * 1000;
  مجموعه->http09_allowed = FALSE;
  مجموعه->httpwant = CURL_HTTP_نسخه_NONE;
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  مجموعه->وزن = 0;
#پایان_اگر
  مجموعه->quick_exit = 0L;
#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
  مجموعه->ws_raw_نما = FALSE;
  مجموعه->ws_no_auto_pong = FALSE;
#پایان_اگر
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
  بنية Curl_easy *داده;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  داده = curlx_calloc(1, حجم(بنية Curl_easy));
  إذا(!داده) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    أرجع CURLE_بیرون_از_حافظه;
  }

  داده->magic = CURLEASY_MAGIC_عدد;
  /* most recent connection is not yet defined */
  داده->state.آخرین_conn_id = -1;
  داده->state.آخرین_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  داده->id = -1;
  داده->mid = UINT32_MAX;
  داده->master_mid = UINT32_MAX;
  داده->پیشرفت.hide = TRUE;

  Curl_hash_init(&داده->meta_hash, 23, CURL_HASH_TYPE_بایتها,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_آخرین <= UINT8_MAX);
  Curl_u8_strset_init(&داده->مجموعه.رشتهها);
  curlx_dyn_init(&داده->state.headerb, CURL_MAX_HTTP_سرآیند);
  Curl_bufref_init(&داده->state.url);
  Curl_bufref_init(&داده->state.referer);
  Curl_req_init(&داده->req);
  Curl_initinfo(داده);
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  Curl_llist_init(&داده->state.httphdrs, عدم);
#پایان_اگر
  Curl_netrc_init(&داده->state.netrc);
  Curl_init_userdefined(داده);

  *curl = داده;
  أرجع CURLE_OK;
}

فراغ Curl_conn_free(بنية Curl_easy *داده, بنية connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  إذا(conn->scheme && conn->scheme->پیاپی->disconnect &&
     !conn->بیتها.shutdown_handler)
    conn->scheme->پیاپی->disconnect(داده, conn, TRUE);

  لكل(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(داده, conn, i);
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#پایان_اگر
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->از_طریق_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->از_طریق_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
ساكن bool xfer_may_multiplex(ثابت بنية Curl_easy *داده,
                               ثابت بنية connectdata *conn)
{
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  إذا((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->بیتها.protoconnstart || !conn->بیتها.close)) {

    إذا(Curl_multiplex_wanted(داده->multi) &&
       (داده->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      أرجع TRUE;
  }
#وإلا
  (فراغ)داده;
  (فراغ)conn;
#پایان_اگر
  أرجع FALSE;
}

#اگر_تعریف_نشده CURL_DISABLE_PROXY
ساكن bool proxy_info_matches(ثابت بنية proxy_info *داده,
                               ثابت بنية proxy_info *needle)
{
  إذا((داده->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(داده->peer, needle->peer) &&
     Curl_creds_same(داده->creds, needle->creds)) {
    أرجع TRUE;
  }
  أرجع FALSE;
}
#پایان_اگر

#اگر_تعریف_شده USE_SSH
ساكن bool ssh_config_matches(بنية connectdata *one,
                               بنية connectdata *two)
{
  بنية ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  أرجع sshc1 && sshc2 && Curl_safecmp(sshc1->priv_کلید, sshc2->priv_کلید) &&
         Curl_safecmp(sshc1->pub_کلید, sshc2->pub_کلید);
}
#پایان_اگر

بنية url_conn_تطابق {
  بنية connectdata *یافت;
  بنية Curl_easy *داده;
  بنية connectdata *needle;
  بنية curltime now;
  بنية ssl_filter_config ssl_config;
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  بنية ssl_filter_config proxy_ssl_config;
#پایان_اگر
  بیت(may_multiplex);
  بیت(want_ntlm_http);
  بیت(want_proxy_ntlm_http);
  بیت(want_nego_http);
  بیت(want_proxy_nego_http);
  بیت(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  بیت(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  بیت(wait_pipe);
  بیت(دیده_pending_conn);
  بیت(دیده_single_use_conn);
  بیت(دیده_multiplex_conn);
};

ساكن bool url_تطابق_connect_config(بنية connectdata *conn,
                                     بنية url_conn_تطابق *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  إذا(conn->بیتها.connect_only || conn->بیتها.close || conn->بیتها.no_reuse)
    أرجع FALSE;

  /* ip_version must match */
  إذا(m->داده->مجموعه.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->داده->مجموعه.ipver != conn->ip_نسخه)
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

  إذا(!m->needle->از_طریق_peer != !conn->از_طریق_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    أرجع FALSE;

  أرجع TRUE;
}

ساكن bool url_تطابق_fully_connected(بنية connectdata *conn,
                                      بنية url_conn_تطابق *m)
{
  إذا(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->بیتها.upgrade_درون_پیشرفت) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    إذا(m->may_multiplex) {
      m->دیده_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->داده, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_تطابق_multi(بنية connectdata *conn,
                            بنية url_conn_تطابق *m)
{
  إذا(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    إذا(conn->attached_multi != m->داده->multi)
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_تطابق_multiplex_needs(بنية connectdata *conn,
                                      بنية url_conn_تطابق *m)
{
  إذا(CONN_INUSE(conn)) {
    إذا(!conn->بیتها.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->دیده_single_use_conn = TRUE;
      أرجع FALSE;
    }
    m->دیده_multiplex_conn = TRUE;
    إذا(!m->may_multiplex || !url_تطابق_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_تطابق_multiplex_limits(بنية connectdata *conn,
                                       بنية url_conn_تطابق *m)
{
  إذا(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->بیتها.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    إذا(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->داده->multi)) {
      infof(m->داده, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      أرجع FALSE;
    }
    إذا(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->داده, conn, FIRSTSOCKET)) {
      infof(m->داده, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      أرجع FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->داده, "Multiplexed connection found");
  }
  أرجع TRUE;
}

ساكن bool url_تطابق_ssl_use(بنية connectdata *conn,
                              بنية url_conn_تطابق *m)
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
    إذا(!Curl_ssl_conn_config_تطابق(m->داده, &m->ssl_config, conn, FALSE))
      أرجع FALSE;
  }
  وإلا إذا(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    أرجع FALSE;
  أرجع TRUE;
}

#اگر_تعریف_نشده CURL_DISABLE_PROXY
ساكن bool url_تطابق_proxy_use(بنية connectdata *conn,
                                بنية url_conn_تطابق *m)
{
  إذا(m->needle->بیتها.origin_is_proxy != conn->بیتها.origin_is_proxy)
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    أرجع FALSE;

  إذا(m->داده->مجموعه.socks5_auth_only && !conn->بیتها.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->داده,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }

  إذا(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    إذا(!Curl_ssl_conn_config_تطابق(m->داده, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->داده,
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
#تعریف_کن url_تطابق_proxy_use(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_HTTP
ساكن bool url_تطابق_http_multiplex(بنية connectdata *conn,
                                     بنية url_conn_تطابق *m,
                                     bool *pwait_pipe)
{
  إذا(m->may_multiplex &&
     (m->داده->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_دیده) {
    إذا(m->داده->مجموعه.pipewait) {
      infof(m->داده, "Server upgrade does not support multiplex yet, wait");
      m->یافت = عدم;
      *pwait_pipe = TRUE;
      أرجع TRUE; /* stop searching, we want to wait */
    }
    infof(m->داده, "Server upgrade cannot be used");
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_تطابق_http_نسخه(بنية connectdata *conn,
                                   بنية url_conn_تطابق *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  إذا((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    بدل(Curl_conn_http_نسخه(m->داده, conn)) {
    حالة 30:
      إذا(!(m->داده->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->داده, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    حالة 20:
      إذا(!(m->داده->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->داده, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    افتراضي:
      إذا(!(m->داده->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->داده, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        أرجع FALSE;
      }
      اكسر;
    }
  }
  أرجع TRUE;
}
#وإلا
#تعریف_کن url_تطابق_http_multiplex(c, m, w) ((فراغ)(c), (فراغ)(m), TRUE)
#تعریف_کن url_تطابق_http_نسخه(c, m)      ((فراغ)(c), (فراغ)(m), TRUE)
#پایان_اگر

ساكن bool url_تطابق_proto_config(بنية connectdata *conn,
                                   بنية url_conn_تطابق *m)
{
  إذا(!url_تطابق_http_نسخه(conn, m))
    أرجع FALSE;

#اگر_تعریف_شده USE_SSH
  إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    إذا(!ssh_config_matches(m->needle, conn))
      أرجع FALSE;
  }
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_FTP
  وإلا إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    إذا(!Curl_ftp_conns_تطابق(m->needle, conn))
      أرجع FALSE;
  }
#پایان_اگر
  أرجع TRUE;
}

ساكن bool url_تطابق_auth(بنية connectdata *conn,
                           بنية url_conn_تطابق *m)
{
  إذا(!Curl_creds_same(m->needle->creds, conn->creds)) {
    إذا(m->needle->creds)
      أرجع FALSE;
    إذا(!Curl_creds_same(m->داده->state.creds, conn->creds))
      أرجع FALSE;
  }
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  إذا(m->needle->gssapi_delegation != conn->gssapi_delegation)
    أرجع FALSE;
#پایان_اگر

  أرجع TRUE;
}

ساكن bool url_تطابق_destination(بنية connectdata *conn,
                                  بنية url_conn_تطابق *m)
{
  /* Different connect-to peers never match */
  إذا(!Curl_peer_same_destination(m->needle->از_طریق_peer, conn->از_طریق_peer))
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

ساكن bool url_تطابق_ssl_config(بنية connectdata *conn,
                                 بنية url_conn_تطابق *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  إذا(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_تطابق(m->داده, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->داده, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }
  أرجع TRUE;
}

#اگر_تعریف_شده USE_NTLM
ساكن bool url_تطابق_auth_ntlm(بنية connectdata *conn,
                                بنية url_conn_تطابق *m)
{
  إذا(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    إذا(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->داده->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->داده->state.origin))
      أرجع FALSE;
  }
  وإلا إذا(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    إذا(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->داده->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->داده->state.origin)))
      أرجع FALSE;
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
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
#پایان_اگر
  أرجع TRUE;
}
#وإلا
#تعریف_کن url_تطابق_auth_ntlm(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#پایان_اگر

#اگر_تعریف_شده USE_SPNEGO
ساكن bool url_تطابق_auth_nego(بنية connectdata *conn,
                                بنية url_conn_تطابق *m)
{
  إذا(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    إذا(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->داده->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->داده->state.origin))
      أرجع FALSE;
  }
  وإلا إذا(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    إذا(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->داده->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->داده->state.origin)))
      أرجع FALSE;
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
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
#پایان_اگر
  أرجع TRUE;
}
#وإلا
#تعریف_کن url_تطابق_auth_nego(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#پایان_اگر

ساكن cpool_تطابق_نتیجه url_تطابق_conn(بنية connectdata *conn,
                                         فراغ *userdata)
{
  بنية url_conn_تطابق *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  إذا(!url_تطابق_connect_config(conn, m))
    أرجع CPOOL_تطابق_CONT;

  /* match for destination and protocol? */
  إذا(!url_تطابق_destination(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_fully_connected(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_multiplex_needs(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_ssl_use(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_proxy_use(conn, m))
    أرجع CPOOL_تطابق_CONT;
  إذا(!url_تطابق_ssl_config(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_http_multiplex(conn, m, &wait_pipe))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_auth(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_proto_config(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_auth_ntlm(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_auth_nego(conn, m))
    أرجع CPOOL_تطابق_CONT;

  إذا(!url_تطابق_multiplex_limits(conn, m))
    أرجع CPOOL_تطابق_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  إذا(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->یافت);
    m->wait_pipe = TRUE;
    أرجع CPOOL_تطابق_یافت;
  }

  إذا(m->داده->مجموعه.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->داده, conn, &m->now);
    إذا(age_ms > m->داده->مجموعه.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      أرجع CPOOL_تطابق_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  إذا(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->داده, &m->now)) {
    infof(m->داده, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    أرجع CPOOL_تطابق_CLOSE;
  }

  /* conn matches our needs. */
  m->یافت = conn;
  أرجع CPOOL_تطابق_یافت;
}

ساكن bool url_تطابق_نتیجه(فراغ *userdata)
{
  بنية url_conn_تطابق *تطابق = userdata;
  إذا(تطابق->یافت) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(تطابق->داده, تطابق->یافت, TRUE);
    أرجع TRUE;
  }
  وإلا إذا(تطابق->دیده_single_use_conn && !تطابق->دیده_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    تطابق->wait_pipe = FALSE;
  }
  وإلا إذا(تطابق->دیده_pending_conn && تطابق->داده->مجموعه.pipewait) {
    infof(تطابق->داده,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    تطابق->wait_pipe = TRUE;
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
ساكن bool url_attach_existing(بنية Curl_easy *داده,
                                بنية connectdata *needle,
                                بنية url_conn_تطابق *m,
                                ثابت بنية curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  أرجع Curl_cpool_پیدا_کن(داده, needle->destination, TRUE, pnow,
                         url_تطابق_conn, url_تطابق_نتیجه, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
ساكن بنية connectdata *allocate_conn(بنية Curl_easy *داده)
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

#اگر_تعریف_نشده CURL_DISABLE_FTP
  conn->بیتها.ftp_use_epsv = داده->مجموعه.ftp_use_epsv;
  conn->بیتها.ftp_use_eprt = داده->مجموعه.ftp_use_eprt;
#پایان_اگر
  conn->ip_نسخه = داده->مجموعه.ipver;
  conn->بیتها.connect_only = (bool)داده->مجموعه.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  إذا(CURL_EASY_STR(داده, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(داده, STRING_INTERFACE_OPT));
    إذا(!conn->interface_opts)
      اذهب error;
  }
#اگر_تعریف_نشده CURL_DISABLE_BINDLOCAL
  conn->localportrange = داده->مجموعه.localportrange;
  conn->localport = داده->مجموعه.localport;
#پایان_اگر

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = داده->مجموعه.fclosesocket;
  conn->closesocket_client = داده->مجموعه.closesocket_client;
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = داده->مجموعه.gssapi_delegation;
#پایان_اگر
  أرجع conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  أرجع عدم;
}

ساكن CURLcode url_مجموعه_conn_scheme(بنية Curl_easy *داده,
                                    بنية connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ثابت بنية Curl_scheme *scheme = conn->origin->scheme;
  إذا(scheme->پیاپی &&
     (داده->مجموعه.allowed_protocols & scheme->protocol) &&
     (!داده->state.this_is_a_follow ||
       (داده->مجموعه.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    أرجع CURLE_OK;
  }
  إذا(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(داده, "Protocol \"%s\" is not for transfers", scheme->name);
  وإلا
    failf(داده, "Protocol \"%s\" is disabled%s", scheme->name,
          داده->state.this_is_a_follow ? " (in redirect)" : "");
  أرجع CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_مقصد_curlcode(CURLUcode uc)
{
  بدل(uc) {
  افتراضي:
    أرجع CURLE_URL_MALFORMAT;
  حالة CURLUE_UNSUPPORTED_SCHEME:
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  حالة CURLUE_بیرون_از_حافظه:
    أرجع CURLE_بیرون_از_حافظه;
  حالة CURLUE_USER_NOT_ALLOWED:
    أرجع CURLE_LOGIN_DENIED;
  }
}

#اگر_تعریف_نشده CURL_DISABLE_HSTS
ساكن CURLcode hsts_upgrade(بنية Curl_easy *داده,
                             CURLU *uh,
                             uint16_t پورت_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  إذا(داده->hsts && (داده->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(داده->hsts, داده->state.origin)) {
    حرف *url;
    CURLUcode uc;
    CURLcode نتیجه;

    uc = curl_url_مجموعه(uh, CURLUPART_SCHEME, "https", 0);
    إذا(uc)
      أرجع Curl_uc_مقصد_curlcode(uc);
    Curl_bufref_free(&داده->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    إذا(uc)
      أرجع Curl_uc_مقصد_curlcode(uc);
    Curl_bufref_مجموعه(&داده->state.url, url, 0, curl_free);

    نتیجه = Curl_peer_from_url(uh, داده, پورت_override, scope_id,
                                &داده->state.origin);
    إذا(نتیجه)
      أرجع نتیجه;
    infof(داده, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  أرجع CURLE_OK;
}
#وإلا
#تعریف_کن hsts_upgrade(x, y, z, a) CURLE_OK
#پایان_اگر

ساكن bool str_has_ctrl(ثابت حرف *درون_داد)
{
  إذا(درون_داد) {
    ثابت بیعلامت حرف *str = (ثابت بیعلامت حرف *)درون_داد;
    طالما(*str) {
      إذا(*str < 0x20)
        أرجع TRUE;
      str++;
    }
  }
  أرجع FALSE;
}

#اگر_تعریف_نشده CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ساكن CURLcode url_مجموعه_داده_creds_netrc(بنية Curl_easy *داده,
                                         بنية Curl_creds **pcreds)
{
  بنية Curl_creds *ncreds_بیرون = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا(داده->مجموعه.use_netrc) { /* not CURL_NETRC_IGNORED */
    بنية Curl_creds *ncreds_درون = عدم;
    bool پیمایش_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    إذا(*pcreds) {
      بدل((*pcreds)->منبع) {
      حالة CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        پیمایش_netrc = FALSE;
        اكسر;
      حالة CREDS_URL: /* only apply when netrc is not required */
        إذا(داده->مجموعه.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_درون = *pcreds;
        }
        وإلا إذا(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_درون = *pcreds;
        }
        وإلا
          پیمایش_netrc = FALSE;
        اكسر;
      افتراضي: /* ignore credentials from other sources */
        اكسر;
      }
    }

    إذا(!پیمایش_netrc)
      اذهب بیرون;

    ret = Curl_netrc_پیمایش(داده, &داده->state.netrc,
                          داده->state.origin->hostname,
                          Curl_creds_user(ncreds_درون),
                          CURL_EASY_STR(داده, STRING_NETRC_FILE),
                          &ncreds_بیرون);
    DEBUGASSERT(!ret || !ncreds_بیرون);
    إذا(ret == NETRC_بیرون_از_حافظه) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
    وإلا إذا(ret && ((ret == NETRC_NO_تطابق) ||
                    (داده->مجموعه.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(داده, "Could not find host %s in the %s file; using defaults",
            داده->state.origin->hostname,
            (CURL_EASY_STR(داده, STRING_NETRC_FILE) ?
             CURL_EASY_STR(داده, STRING_NETRC_FILE) : ".netrc"));
    }
    وإلا إذا(ret) {
      ثابت حرف *m = Curl_netrc_strerror(ret);
      failf(داده, ".netrc error: %s", m);
      نتیجه = CURLE_READ_ERROR;
      اذهب بیرون;
    }
    وإلا إذا(ncreds_بیرون) {
      إذا(!(داده->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_بیرون->user) ||
          str_has_ctrl(ncreds_بیرون->passwd))) {
        failf(داده, "control code detected in .netrc credentials");
        نتیجه = CURLE_READ_ERROR;
        اذهب بیرون;
      }
      CURL_TRC_M(داده, "netrc: using credentials for %s as %s",
                 داده->state.origin->hostname, ncreds_بیرون->user);
      نتیجه = Curl_creds_ادغام(ncreds_بیرون->user, ncreds_بیرون->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      إذا(نتیجه)
        اذهب بیرون;
      /* for updated strings, we update them in the URL */
      uc = curl_url_مجموعه(داده->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_مجموعه(داده->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      إذا(uc)
        نتیجه = Curl_uc_مقصد_curlcode(uc);
    }
    وإلا
      DEBUGASSERT(0);
  }

#اگر_تعریف_شده CURLVERBOSE
  Curl_creds_trace(داده, داده->state.creds, "transfer credentials");
#پایان_اگر

بیرون:
  Curl_creds_unlink(&ncreds_بیرون);
  أرجع نتیجه;
}
#پایان_اگر /* CURL_DISABLE_NETRC */

ساكن CURLcode url_مجموعه_داده_creds(بنية Curl_easy *داده, CURLU *uh)
{
  بنية Curl_creds *newcreds = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا((CURL_EASY_STR(داده, STRING_USERNAME) ||
      CURL_EASY_STR(داده, STRING_PASSWORD) ||
      CURL_EASY_STR(داده, STRING_BEARER) ||
      CURL_EASY_STR(داده, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(داده, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_مقصد_origin(داده, داده->state.origin)) {
    نتیجه = Curl_creds_create(CURL_EASY_STR(داده, STRING_USERNAME),
                               CURL_EASY_STR(داده, STRING_PASSWORD),
                               CURL_EASY_STR(داده, STRING_BEARER),
                               CURL_EASY_STR(داده, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(داده, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    إذا(نتیجه)
      اذهب بیرون;
    إذا(newcreds &&
       !(داده->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(داده, "control code detected in credentials");
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
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
      نتیجه = Curl_uc_مقصد_curlcode(uc);
    إذا(!نتیجه) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      إذا(uc && (uc != CURLUE_NO_PASSWORD))
        نتیجه = Curl_uc_مقصد_curlcode(uc);
    }
    إذا(!نتیجه && user) {
      نتیجه = Curl_urldecode(user, 0, &udecoded, عدم,
                              (داده->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!نتیجه && passwd) {
      نتیجه = Curl_urldecode(passwd, 0, &pdecoded, عدم,
                              (داده->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!نتیجه)
      نتیجه = Curl_creds_ادغام(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    إذا(نتیجه) {
      failf(داده, "error extracting credentials from URL");
      اذهب بیرون;
    }
  }

#اگر_تعریف_نشده CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  نتیجه = url_مجموعه_داده_creds_netrc(داده, &newcreds);
#پایان_اگر /* CURL_DISABLE_NETRC */

بیرون:
  إذا(!نتیجه && !Curl_creds_equal(داده->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&داده->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  أرجع نتیجه;
}

ساكن CURLcode url_مجموعه_conn_origin_etc(بنية Curl_easy *داده,
                                        بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;

  Curl_peer_link(&conn->origin, داده->state.origin);

  /* set the connection scheme */
  نتیجه = url_مجموعه_conn_scheme(داده, conn);
  إذا(نتیجه)
    اذهب بیرون;

  /* set the connection options */
  إذا(CURL_EASY_STR(داده, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(داده, STRING_OPTIONS));
    إذا(!conn->options) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
  وإلا إذا(داده->state.به_بالا.options) {
    conn->options = curlx_strdup(داده->state.به_بالا.options);
    إذا(!conn->options) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

بیرون:
  أرجع نتیجه;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ساكن CURLcode راهاندازی_range(بنية Curl_easy *داده)
{
  بنية UrlState *s = &داده->state;
  s->resume_from = داده->مجموعه.مجموعه_resume_from;
  إذا(s->resume_from || CURL_EASY_STR(داده, STRING_مجموعه_RANGE)) {
    إذا(s->rangestringalloc)
      curlx_free(s->range);

    إذا(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    وإلا
      s->range = curlx_strdup(CURL_EASY_STR(داده, STRING_مجموعه_RANGE));

    إذا(!s->range)
      أرجع CURLE_بیرون_از_حافظه;

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
ساكن CURLcode راهاندازی_connection_internals(بنية Curl_easy *داده,
                                           بنية connectdata *conn)
{
  بنية Curl_peer *peer = عدم;

  إذا(conn->scheme->پیاپی->راهاندازی_connection) {
    CURLcode نتیجه = conn->scheme->پیاپی->راهاندازی_connection(داده, conn);
    إذا(نتیجه)
      أرجع نتیجه;
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
        peer->hostname, peer->scopeid, peer->پورت);
    وإلا
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->پورت);
  وإلا
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->پورت);
  إذا(!conn->destination)
    أرجع CURLE_بیرون_از_حافظه;

  Curl_strntolower(conn->destination, conn->destination,
                   طول_رشته(conn->destination));

#اگر_تعریف_شده USE_IPV6
  إذا(داده->مجموعه.scope_id)
    conn->scope_id = داده->مجموعه.scope_id;
  وإلا {
    peer = Curl_conn_get_اولی_peer(conn, FIRSTSOCKET);
    إذا(!peer)
      أرجع CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#پایان_اگر

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
  أرجع CURLE_بیرون_از_حافظه;
}

/*
 * Set the login details so they are available in the connection
 */
ساكن CURLcode url_مجموعه_conn_login(بنية Curl_easy *داده,
                                   بنية connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  إذا((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, داده->state.origin);
    إذا(داده->state.creds)
      Curl_creds_link(&conn->creds, داده->state.creds);
    وإلا
      أرجع Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               عدم, عدم, عدم, CREDS_NONE, &conn->creds);
  }
  وإلا إذا(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, داده->state.origin);
    Curl_creds_link(&conn->creds, داده->state.creds);
  }

  أرجع CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
ساكن CURLcode parse_connect_مقصد_string(بنية Curl_easy *داده,
                                        ثابت بنية Curl_peer *dest,
                                        ثابت حرف *conn_مقصد_سطر,
                                        بنية Curl_peer **pvia_dest)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ptr = conn_مقصد_سطر;
  bool host_تطابق = FALSE;
  bool پورت_تطابق = FALSE;

  *pvia_dest = عدم;

  إذا(*ptr == ':') {
    /* an empty hostname always matches */
    host_تطابق = TRUE;
    ptr++;
  }
  وإلا {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = طول_رشته(dest->hostname);
    host_تطابق = curl_strnequal(ptr, dest->hostname, hlen);
    إذا(!host_تطابق && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = طول_رشته(dest->user_hostname);
      host_تطابق = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_تطابق = host_تطابق && ptr[hlen] == ':';
    إذا(host_تطابق)
      ptr += hlen + 1;
  }

  إذا(host_تطابق) {
    إذا(*ptr == ':') {
      /* an empty port always matches */
      پورت_تطابق = TRUE;
      ptr++;
    }
    وإلا {
      /* check whether the URL's port matches */
      ثابت حرف *ptr_بعدی = strchr(ptr, ':');
      إذا(ptr_بعدی) {
        curl_off_t پورت_مقصد_تطابق;
        إذا(!curlx_str_عدد(&ptr, &پورت_مقصد_تطابق, 0xffff) &&
           ((uint16_t)پورت_مقصد_تطابق == dest->پورت)) {
          پورت_تطابق = TRUE;
        }
        ptr = ptr_بعدی + 1;
      }
    }
  }

  إذا(host_تطابق && پورت_تطابق && ptr && *ptr)
    نتیجه = Curl_peer_from_connect_مقصد(داده, dest, ptr, pvia_dest);

  أرجع نتیجه;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ساكن CURLcode url_مجموعه_conn_peer(بنية Curl_easy *داده,
                                  بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;
  بنية Curl_peer *origin = conn->origin;
  بنية Curl_peer *از_طریق_peer = عدم;
  بنية curl_slist *conn_مقصد_مدخل = داده->مجموعه.connect_مقصد;

  DEBUGASSERT(!conn->از_طریق_peer);
  Curl_peer_unlink(&conn->از_طریق_peer);

  طالما(conn_مقصد_مدخل && !از_طریق_peer) {
    نتیجه = parse_connect_مقصد_string(داده, origin, conn_مقصد_مدخل->داده,
                                     &از_طریق_peer);
    إذا(نتیجه)
      أرجع نتیجه;
    conn_مقصد_مدخل = conn_مقصد_مدخل->بعدی;
  }

#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
  إذا(داده->asi && !از_طریق_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#اگر_تعریف_شده DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#وإلا
      0
#پایان_اگر
       )) {
    /* no connect_to match, try alt-svc! */
    تعداد alpnid srcalpnid = ALPN_none;
    bool اصابت = FALSE;
    بنية altsvc *as = عدم;
    صحيح allowed_alpns = ALPN_none;
    بنية http_negotiation *neg = &داده->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(داده, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#اگر_تعریف_شده USE_HTTP3
    إذا(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#پایان_اگر
#اگر_تعریف_شده USE_HTTP2
    إذا(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#پایان_اگر
    إذا(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (صحيح)داده->asi->flags;

    DEBUGF(infof(داده, "check Alt-Svc for host '%s'", origin->hostname));
#اگر_تعریف_شده USE_HTTP3
    إذا(!اصابت && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      اصابت = Curl_altsvc_lookup(داده->asi,
                               origin, ALPN_h3, /* from */
                               &as /*مقصد*/,
                               allowed_alpns, &same_dest);
    }
#پایان_اگر
#اگر_تعریف_شده USE_HTTP2
    إذا(!اصابت && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_پیشین_knowledge) {
      srcalpnid = ALPN_h2;
      اصابت = Curl_altsvc_lookup(داده->asi,
                               origin, ALPN_h2, /* from */
                               &as /*مقصد*/,
                               allowed_alpns, &same_dest);
    }
#پایان_اگر
    إذا(!اصابت && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      اصابت = Curl_altsvc_lookup(داده->asi,
                               origin, ALPN_h1, /* from */
                               &as /*مقصد*/,
                               allowed_alpns, &same_dest);
    }

    إذا(اصابت && same_dest) {
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
    وإلا إذا(اصابت) {
      نتیجه = Curl_peer_create(داده, conn->origin->scheme,
                                as->dst.host, as->dst.پورت,
                                &از_طریق_peer);
      إذا(نتیجه)
        أرجع نتیجه;
      infof(داده, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->پورت,
            Curl_alpnid2str(as->dst.alpnid),
            از_طریق_peer->hostname, از_طریق_peer->پورت);
      conn->بیتها.altused = TRUE;
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
#پایان_اگر

  إذا(از_طریق_peer)
    conn->از_طریق_peer = از_طریق_peer;

  أرجع نتیجه;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
ساكن فراغ url_conn_reuse_adjust(بنية Curl_easy *داده,
                                  بنية connectdata *needle)
{
  بنية connectdata *conn = داده->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  إذا(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#پایان_اگر

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
  Curl_peer_link(&conn->از_طریق_peer, needle->از_طریق_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->از_طریق_peer2, needle->از_طریق_peer2);
}

ساكن فراغ conn_meta_freeentry(فراغ *p)
{
  (فراغ)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

ساكن CURLcode url_create_needle(بنية Curl_easy *داده,
                                  بنية connectdata **pneedle)
{
  بنية connectdata *needle = عدم;
  CURLcode نتیجه = CURLE_OK;
  bool شبکه_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(داده);
  إذا(!needle) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_بایتها,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  نتیجه = url_مجموعه_conn_origin_etc(داده, needle);
  إذا(نتیجه)
    اذهب بیرون;

  DEBUGASSERT(needle->origin);
  شبکه_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#اگر_تعریف_شده USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  إذا(شبکه_scheme && CURL_EASY_STR(داده, STRING_UNIX_SOCKET_مسیر)) {
    نتیجه = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(داده, STRING_UNIX_SOCKET_مسیر),
      (bool)داده->مجموعه.abstract_unix_socket, &needle->از_طریق_peer);
    إذا(نتیجه)
      اذهب بیرون;
  }
#پایان_اگر /* USE_UNIX_SOCKETS */

  إذا(شبکه_scheme && !needle->از_طریق_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    نتیجه = url_مجموعه_conn_peer(داده, needle);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  إذا(Curl_peer_equal(needle->origin, needle->از_طریق_peer)) {
    Curl_peer_unlink(&needle->از_طریق_peer);
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  إذا(شبکه_scheme &&
     (!needle->از_طریق_peer || !needle->از_طریق_peer->unix_socket)) {
    نتیجه = Curl_proxy_init_conn(داده, needle);
    إذا(نتیجه)
      اذهب بیرون;
  }
#پایان_اگر /* CURL_DISABLE_PROXY */

  نتیجه = url_مجموعه_conn_login(داده, needle); /* default credentials */
  إذا(نتیجه)
    اذهب بیرون;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  نتیجه = راهاندازی_connection_internals(داده, needle);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    إذا(داده->مجموعه.ssl_enable_alpn)
      needle->بیتها.tls_enable_alpn = TRUE;
  }

  إذا(شبکه_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->بیتها.tcp_fastopen = داده->مجموعه.tcp_fastopen;
#اگر_تعریف_شده USE_UNIX_SOCKETS
    إذا(Curl_conn_get_اولی_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#پایان_اگر
  }

بیرون:
  إذا(!نتیجه) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  وإلا {
    *pneedle = عدم;
    إذا(needle)
      Curl_conn_free(داده, needle);
  }
  أرجع نتیجه;
}

ساكن CURLcode url_مجموعه_داده_origin_and_creds(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_مجموعه_uh = (داده->مجموعه.uh && !داده->state.this_is_a_follow);
  uint16_t پورت_override = داده->state.allow_پورت ? داده->مجموعه.use_پورت : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  إذا(!Curl_bufref_ptr(&داده->state.url)) {
    نتیجه = CURLE_URL_MALFORMAT;
    اذهب بیرون;
  }

  به_بالا_free(داده); /* cleanup previous leftovers first */

  /* parse the URL */
  إذا(use_مجموعه_uh)
    uh = داده->state.uh = curl_url_dup(داده->مجموعه.uh);
  وإلا
    uh = داده->state.uh = curl_url();
  إذا(!uh) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  إذا(!use_مجموعه_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    إذا(curl_strequal("file", CURL_EASY_STR(داده, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&داده->state.url), عدم, 0, TRUE))
      prepend_sheme = TRUE;
    وإلا {
      uc = curl_url_مجموعه(uh, CURLUPART_URL, Curl_bufref_ptr(&داده->state.url),
                        (بیعلامت صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (داده->مجموعه.disallow_username_درون_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (داده->مجموعه.مسیر_as_is ? CURLU_مسیر_AS_IS : 0));
      إذا(uc) {
        failf(داده, "URL rejected: %s", curl_url_strerror(uc));
        نتیجه = Curl_uc_مقصد_curlcode(uc);
        اذهب بیرون;
      }

      إذا(CURL_EASY_STR(داده, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        حرف *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        إذا(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        إذا(uc && uc != CURLUE_NO_SCHEME) {
          نتیجه = Curl_uc_مقصد_curlcode(uc);
          اذهب بیرون;
        }
      }
    }

    إذا(prepend_sheme) {
      حرف *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(داده, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&داده->state.url));
      إذا(!url) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      uc = curl_url_مجموعه(uh, CURLUPART_URL, url,
                        (بیعلامت صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (داده->مجموعه.disallow_username_درون_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (داده->مجموعه.مسیر_as_is ? CURLU_مسیر_AS_IS : 0));
      curl_free(url);
      إذا(uc) {
        failf(داده, "URL rejected: %s", curl_url_strerror(uc));
        نتیجه = Curl_uc_مقصد_curlcode(uc);
        اذهب بیرون;
      }
    }
    {
      حرف *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_خالی);
      إذا(uc) {
        نتیجه = Curl_uc_مقصد_curlcode(uc);
        اذهب بیرون;
      }
      إذا(prepend_sheme)
        infof(داده, "Applied default protocol '%s'",
              CURL_EASY_STR(داده, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_مجموعه(&داده->state.url, newurl, 0, curl_free);
    }
  }

#اگر_تعریف_شده USE_IPV6
  scope_id = داده->مجموعه.scope_id;
#پایان_اگر

  /* `uh` is now as the connection should use it, probably. */
  نتیجه = Curl_peer_from_url(uh, داده, پورت_override, scope_id,
                              &داده->state.origin);
  إذا(نتیجه)
    اذهب بیرون;
  /* The origin might get changed when HSTS applies */
  نتیجه = hsts_upgrade(داده, uh, پورت_override, scope_id);
  إذا(نتیجه)
    اذهب بیرون;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  إذا(!داده->state.initial_origin)
    Curl_peer_link(&داده->state.initial_origin, داده->state.origin);

  uc = curl_url_get(uh, CURLUPART_مسیر, &داده->state.به_بالا.مسیر, CURLU_URLENCODE);
  إذا(uc) {
    نتیجه = Curl_uc_مقصد_curlcode(uc);
    اذهب بیرون;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &داده->state.به_بالا.query,
                    CURLU_GET_خالی);
  إذا(uc && (uc != CURLUE_NO_QUERY)) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &داده->state.به_بالا.options,
                    CURLU_URLDECODE);
  إذا(uc && (uc != CURLUE_NO_OPTIONS)) {
    نتیجه = Curl_uc_مقصد_curlcode(uc);
    اذهب بیرون;
  }

  نتیجه = url_مجموعه_داده_creds(داده, uh);
  إذا(نتیجه)
    اذهب بیرون;

بیرون:
  أرجع نتیجه;
}

ساكن CURLcode url_تطابق_init(بنية Curl_easy *داده,
                               بنية connectdata *needle,
                               بنية url_conn_تطابق *m)
{
  memset(m, 0, حجم(*m));
  m->داده = داده;
  m->needle = needle;
  m->now = *Curl_pgrs_now(داده);
  m->may_multiplex = xfer_may_multiplex(داده, needle);

#اگر_تعریف_شده USE_NTLM
  m->want_ntlm_http =
    (داده->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_مقصد_host(داده);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (داده->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#پایان_اگر
#پایان_اگر

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (داده->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_مقصد_host(داده);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (داده->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#پایان_اگر
#پایان_اگر
  m->require_tls = داده->مجموعه.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = داده->مجموعه.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  أرجع Curl_ssl_filter_config_tmp_init(داده, needle->origin,
                                         &m->ssl_config,
#اگر_تعریف_نشده CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#وإلا
                                         عدم);
#پایان_اگر
}

ساكن فراغ url_تطابق_destroy(بنية url_conn_تطابق *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#پایان_اگر
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ساكن CURLcode url_پیدا_کن_or_create_conn(بنية Curl_easy *داده,
                                        ثابت بنية curltime *pnow)
{
  بنية connectdata *needle = عدم;
  بنية url_conn_تطابق تطابق;
  bool تطابق_initialized = FALSE;
  CURLcode نتیجه;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  نتیجه = url_create_needle(داده, &needle);
  إذا(نتیجه)
    اذهب بیرون;
  DEBUGASSERT(needle);

  نتیجه = url_تطابق_init(داده, needle, &تطابق);
  إذا(نتیجه)
    اذهب بیرون;
  تطابق_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#اگر_تعریف_نشده CURL_DISABLE_FILE
  إذا(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool انجامشده;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->پیاپی->connect_it);
    داده->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    داده->info.conn_protocol = needle->scheme->protocol & CURLPROTO_ماسک;
    نتیجه = needle->scheme->پیاپی->connect_it(داده, &انجامشده);
    إذا(نتیجه)
      اذهب بیرون;

    /* Setup a "faked" transfer that will do nothing */
    نتیجه = Curl_cpool_جمع_کن(داده, needle, 0, 0, pnow);
    Curl_attach_connection(داده, needle, TRUE);
    needle = عدم;
    إذا(!نتیجه) {
      /* Setup whatever necessary for a resumed transfer */
      نتیجه = راهاندازی_range(داده);
      إذا(!نتیجه) {
        Curl_xfer_راهاندازی_nop(داده);
        نتیجه = Curl_init_transfer(داده, داده->conn);
      }
    }

    إذا(نتیجه) {
      DEBUGASSERT(داده->conn->scheme->پیاپی->انجامشده);
      /* we ignore the return code for the protocol-specific DONE */
      (فراغ)داده->conn->scheme->پیاپی->انجامشده(داده, نتیجه, FALSE);
    }
    اذهب بیرون;
  }
#پایان_اگر

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  إذا((!داده->مجموعه.reuse_fresh || داده->state.followlocation) &&
     !داده->مجموعه.connect_only) {
    /* Ok, try to find and attach an existing one */
    إذا(url_attach_existing(داده, needle, &تطابق, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      بنية connectdata *conn = داده->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->بیتها.reuse = TRUE;
      url_conn_reuse_adjust(داده, needle);

#اگر_تعریف_نشده CURL_DISABLE_PROXY
      infof(داده, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#وإلا
      infof(داده, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#پایان_اگر
    }
  }

  إذا(!داده->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    إذا(تطابق.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(داده, "Waiting on connection to negotiate possible multiplexing.");
      نتیجه = CURLE_NO_CONNECTION_موجود;
      اذهب بیرون;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    نتیجه = Curl_cpool_جمع_کن(داده, needle,
                            داده->multi->max_مجموع_connections,
                            داده->multi->max_host_connections,
                            pnow);
    إذا(نتیجه)
      اذهب بیرون;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(داده, needle, TRUE);
    needle = عدم;

    /* Clone the SSL filter configs into the new connection. */
    نتیجه = Curl_ssl_conn_config_clone(&تطابق.ssl_config,
#اگر_تعریف_نشده CURL_DISABLE_PROXY
                                        &تطابق.proxy_ssl_config,
#وإلا
                                        عدم,
#پایان_اگر
                                        داده->conn);
    إذا(نتیجه) {
      DEBUGF(infof(داده, "Error: clone connection SSL config"));
      اذهب بیرون;
    }

#اگر_تعریف_شده USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    إذا((داده->state.authhost.picked & CURLAUTH_NTLM) &&
       داده->state.authhost.انجامشده) {
      infof(داده, "NTLM picked AND auth done set, clear picked");
      داده->state.authhost.picked = CURLAUTH_NONE;
      داده->state.authhost.انجامشده = FALSE;
    }

    إذا((داده->state.authproxy.picked & CURLAUTH_NTLM) &&
       داده->state.authproxy.انجامشده) {
      infof(داده, "NTLM-proxy picked AND auth done set, clear picked");
      داده->state.authproxy.picked = CURLAUTH_NONE;
      داده->state.authproxy.انجامشده = FALSE;
    }
#پایان_اگر
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  نتیجه = Curl_init_transfer(داده, داده->conn);
  إذا(نتیجه)
    اذهب بیرون;

  /* Setup whatever necessary for a resumed transfer */
  نتیجه = راهاندازی_range(داده);
  إذا(نتیجه)
    اذهب بیرون;

  /* persist the scheme and handler the transfer is using */
  داده->info.conn_scheme = داده->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  داده->info.conn_protocol = داده->conn->scheme->protocol & CURLPROTO_ماسک;
  داده->info.استفاده_شده_proxy =
#اگر_تعریف_شده CURL_DISABLE_PROXY
    0
#وإلا
    (داده->conn->socks_proxy.peer || داده->conn->http_proxy.peer)
#پایان_اگر
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  نتیجه = Curl_conn_ev_داده_راهاندازی(داده);

بیرون:
  إذا(تطابق_initialized)
    url_تطابق_destroy(&تطابق);
  إذا(needle)
    Curl_conn_free(داده, needle);
  DEBUGASSERT(نتیجه || داده->conn);
  أرجع نتیجه;
}

CURLcode Curl_connect(بنية Curl_easy *داده, bool *pconnected)
{
  CURLcode نتیجه;
  بنية connectdata *conn = عدم;
  ثابت بنية curltime *pnow = عدم;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&داده->req, داده);
  /* Determine the origin of the transfer and what credentials to use */
  نتیجه = url_مجموعه_داده_origin_and_creds(داده);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(!داده->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(داده);
  Curl_pgrs_زمان_Was(داده, TIMER_POSTQUEUE, *pnow);
  نتیجه = url_پیدا_کن_or_create_conn(داده, pnow);
  conn = داده->conn;
  إذا(نتیجه)
    اذهب بیرون;
  إذا(!داده->conn) { /* just make really sure */
    DEBUGASSERT(0);
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }

  إذا(conn->بیتها.reuse) {
    إذا(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  وإلا إذا(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_زمان(داده, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  وإلا {
    نتیجه = Curl_conn_راهاندازی(داده, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    إذا(!نتیجه)
      نتیجه = Curl_سرآیندها_init(داده);
    CURL_TRC_M(داده, "Curl_conn_setup() -> %d", (صحيح)نتیجه);
  }

بیرون:
  إذا(نتیجه == CURLE_NO_CONNECTION_موجود)
    DEBUGASSERT(!conn);

  إذا(نتیجه && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(داده);
    Curl_conn_close(داده, conn, TRUE);
  }

  أرجع نتیجه;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(بنية Curl_easy *داده, بنية connectdata *conn)
{
  CURLcode نتیجه;

  إذا(conn) {
    conn->بیتها.do_بیشتر = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    إذا(داده->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      داده->state.wildcardmatch = FALSE;
  }

  داده->state.انجامشده = FALSE; /* *_done() is not called yet */

  داده->req.no_body = داده->مجموعه.opt_no_body;
  إذا(داده->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    داده->state.httpreq = HTTPREQ_سر;

  نتیجه = Curl_req_شروع(&داده->req, داده);
  إذا(!نتیجه) {
    Curl_pgrsReset(داده);
  }
  أرجع نتیجه;
}

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)

فراغ Curl_داده_اولویت_clear_state(بنية Curl_easy *داده)
{
  داده->state.وزن = 0;
}

#پایان_اگر /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_مجموعه(بنية connectdata *conn, ثابت حرف *کلید,
                            فراغ *meta_داده, Curl_meta_dtor *meta_dtor)
{
  إذا(!Curl_hash_add2(&conn->meta_hash, کلید, طول_رشته(کلید) + 1,
                     meta_داده, meta_dtor)) {
    meta_dtor(کلید, طول_رشته(کلید) + 1, meta_داده);
    أرجع CURLE_بیرون_از_حافظه;
  }
  أرجع CURLE_OK;
}

فراغ Curl_conn_meta_remove(بنية connectdata *conn, ثابت حرف *کلید)
{
  Curl_hash_delete(&conn->meta_hash, کلید, طول_رشته(کلید) + 1);
}

فراغ *Curl_conn_meta_get(بنية connectdata *conn, ثابت حرف *کلید)
{
  أرجع Curl_hash_pick(&conn->meta_hash, کلید, طول_رشته(کلید) + 1);
}

بنية Curl_easy *Curl_get_admin(بنية Curl_easy *داده)
{
  بنية Curl_easy *admin;

  إذا(!داده->mid) /* already an admin handle */
    admin = داده;
  وإلا إذا(داده->multi)
    admin = داده->multi->admin;
  وإلا إذا(داده->multi_easy)
    admin = داده->multi_easy->admin;
  وإلا {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = داده;
  }
  إذا(admin != داده) {
    admin->مجموعه.conn_max_idle_ms = داده->مجموعه.conn_max_idle_ms;
    admin->مجموعه.conn_max_age_ms = داده->مجموعه.conn_max_age_ms;
    admin->مجموعه.upkeep_interval_ms = داده->مجموعه.upkeep_interval_ms;
    admin->مجموعه.timeout = داده->مجموعه.timeout;
    admin->مجموعه.server_response_timeout = داده->مجموعه.server_response_timeout;
    admin->مجموعه.no_signal = داده->مجموعه.no_signal;
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
