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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_IPHLPAPI_H
#شامل <Iphlpapi.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_UN_H
#شامل <sys/un.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_SOCKET
#غلطی "We cannot compile without socket() support"
#اگر_ختم

#اگر defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#شامل <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#اگر_ختم
#شامل <iphlpapi.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "mime.h"
#شامل "bufref.h"
#شامل "vtls/vtls.h"
#شامل "vssh/vssh.h"
#شامل "transfer.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "cookie.h"
#شامل "strcase.h"
#شامل "escape.h"
#شامل "curl_share.h"
#شامل "http_digest.h"
#شامل "multiif.h"
#شامل "getinfo.h"
#شامل "pop3.h"
#شامل "urlapi-int.h"
#شامل "hsts.h"
#شامل "proxy.h"
#شامل "cfilters.h"
#شامل "idn.h"
#شامل "http_proxy.h"
#شامل "conncache.h"
#شامل "multihandle.h"
#شامل "curlx/strdup.h"
#شامل "setopt.h"
#شامل "altsvc.h"
#شامل "curlx/dynbuf.h"
#شامل "headers.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strparse.h"
#شامل "peer.h"

/* Now for the protocols */
#شامل "ftp.h"
#شامل "dict.h"
#شامل "telnet.h"
#شامل "tftp.h"
#شامل "http.h"
#شامل "vauth/vauth.h"
#شامل "file.h"
#شامل "curl_ldap.h"
#شامل "vssh/ssh.h"
#شامل "imap.h"
#شامل "url.h"
#شامل "connect.h"
#شامل "gopher.h"
#شامل "mqtt.h"
#شامل "rtsp.h"
#شامل "smtp.h"
#شامل "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#اگر READBUFFER_سائز < READBUFFER_MIN
# error READBUFFER_سائز is too small
#اگر_ختم

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
ساکن curl_prot_t get_protocol_family(ثابت ساخت Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  واپس s->family;
}

خالی Curl_freeset(ساخت Curl_easy *ڈیٹا)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  شمار dupblob j;

  CURL_EASY_STR_CLEAR0(ڈیٹا, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(ڈیٹا, STRING_کلید_PASSWD);
  CURL_EASY_STR_CLEAR0(ڈیٹا, STRING_BEARER);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(ڈیٹا, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(ڈیٹا, STRING_کلید_PASSWD_PROXY);
#اگر_ختم
  Curl_u8_strset_clear(&ڈیٹا->سیٹ.لڑیاں);
  curlx_safefree(ڈیٹا->سیٹ.str_copypostfields);

  برائے(j = (شمار dupblob)0; j < BLOB_آخری; j++) {
    curlx_safefree(ڈیٹا->سیٹ.blobs[j]);
  }

  Curl_bufref_free(&ڈیٹا->state.referer);
  Curl_bufref_free(&ڈیٹا->state.url);

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(ڈیٹا->سیٹ.mimepostp);
  curlx_safefree(ڈیٹا->سیٹ.mimepostp);
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  curl_slist_free_all(ڈیٹا->state.cookielist);
  ڈیٹا->state.cookielist = تہی;
#اگر_ختم
}

/* free the URL pieces */
ساکن خالی اوپر_free(ساخت Curl_easy *ڈیٹا)
{
  ساخت urlpieces *اوپر = &ڈیٹا->state.اوپر;
  curlx_safefree(اوپر->options);
  curlx_safefree(اوپر->راستہ);
  curlx_safefree(اوپر->query);
  curl_url_cleanup(ڈیٹا->state.uh);
  ڈیٹا->state.uh = تہی;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(ساخت Curl_easy **datap)
{
  ساخت Curl_easy *ڈیٹا;

  اگر(!datap || !*datap)
    واپس CURLE_OK;

  ڈیٹا = *datap;
  *datap = تہی;

  اگر(!ڈیٹا->state.اندرونی_2 && ڈیٹا->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(ڈیٹا->multi, ڈیٹا);
  }
  ورنہ {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(ڈیٹا);
    اگر(!ڈیٹا->state.اندرونی_2 && ڈیٹا->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(ڈیٹا->multi_easy);
      ڈیٹا->multi_easy = تہی;
    }
  }
  DEBUGASSERT(!ڈیٹا->conn || ڈیٹا->state.اندرونی_2);

  Curl_expire_clear_all(ڈیٹا); /* shut off any timers left */

  اگر(ڈیٹا->state.rangestringalloc)
    curlx_free(ڈیٹا->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(ڈیٹا);

  ڈیٹا->سیٹ.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  ڈیٹا->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&ڈیٹا->req, ڈیٹا);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(ڈیٹا);
  Curl_peer_unlink(&ڈیٹا->state.origin);
  Curl_peer_unlink(&ڈیٹا->state.initial_origin);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  Curl_peer_unlink(&ڈیٹا->state.rtsp_session_origin);
#اگر_ختم
  Curl_ssl_free_certinfo(ڈیٹا);

  Curl_bufref_free(&ڈیٹا->state.referer);

  اوپر_free(ڈیٹا);
  curlx_dyn_free(&ڈیٹا->state.headerb);
  Curl_flush_cookies(ڈیٹا, TRUE);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  Curl_altsvc_save(ڈیٹا, ڈیٹا->asi, CURL_EASY_STR(ڈیٹا, STRING_ALTSVC));
  Curl_altsvc_cleanup(&ڈیٹا->asi);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  Curl_hsts_save(ڈیٹا, ڈیٹا->hsts, CURL_EASY_STR(ڈیٹا, STRING_HSTS));
  اگر(!ڈیٹا->share || !ڈیٹا->share->hsts)
    Curl_hsts_cleanup(&ڈیٹا->hsts);
  curl_slist_free_all(ڈیٹا->state.hstslist); /* clean up list */
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(ڈیٹا);
#اگر_ختم
  curlx_safefree(ڈیٹا->state.most_recent_ftp_entrypath);
  curlx_safefree(ڈیٹا->info.contenttype);
  curlx_safefree(ڈیٹا->info.wouldredirect);
  curlx_safefree(ڈیٹا->info.effective_url);

  /* No longer a dirty share, if it exists */
  اگر(Curl_share_easy_unlink(ڈیٹا))
    DEBUGASSERT(0);

  Curl_hash_destroy(&ڈیٹا->meta_hash);
  Curl_creds_unlink(&ڈیٹا->state.creds);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  curlx_safefree(ڈیٹا->state.rangeline);
  curlx_safefree(ڈیٹا->state.http_host);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
  curlx_safefree(ڈیٹا->req.cookiehost);
#اگر_ختم

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(ڈیٹا->state.formp);
  curlx_safefree(ڈیٹا->state.formp);
#اگر_ختم

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&ڈیٹا->wildcard);
  Curl_freeset(ڈیٹا);
  Curl_سرخیاں_cleanup(ڈیٹا);
  Curl_netrc_cleanup(&ڈیٹا->state.netrc);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
  curlx_free(ڈیٹا->state.envproxy);
#اگر_ختم
  curlx_memzero(ڈیٹا, حجم(*ڈیٹا));
  curlx_free(ڈیٹا);
  واپس CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
خالی Curl_init_userdefined(ساخت Curl_easy *ڈیٹا)
{
  ساخت UserDefined *سیٹ = &ڈیٹا->سیٹ;

  سیٹ->برآمد = stdout;  /* default output to stdout */
  سیٹ->اندر_سیٹ = stdin;  /* default input from stdin */
  سیٹ->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&ڈیٹا->سیٹ.لڑیاں);

#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
  /* use fwrite as default function to store output */
  سیٹ->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  سیٹ->fread_func_سیٹ = (curl_read_callback)fread;
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
  سیٹ->is_fread_سیٹ = 0;

  سیٹ->seek_client = ZERO_NULL;

  سیٹ->filesize = -1;        /* we do not know the size */
  سیٹ->postfieldsize = -1;   /* unknown size */
  سیٹ->maxredirs = 30;       /* sensible default */

  سیٹ->طریقہ = HTTPREQ_GET; /* Default HTTP request */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  سیٹ->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  سیٹ->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  سیٹ->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  سیٹ->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  سیٹ->ftp_filemethod = FTPFILE_MULTICWD;
  سیٹ->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#اگر_ختم
  سیٹ->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  سیٹ->ssl_ca_cache_timeout = 24 * 60 * 60;

  سیٹ->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&ڈیٹا->سیٹ.ssl);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  Curl_ssl_config_init(&ڈیٹا->سیٹ.proxy_ssl);
  سیٹ->proxyport = 0;
  سیٹ->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  سیٹ->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  سیٹ->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  سیٹ->doh_verifyhost = TRUE;
  سیٹ->doh_verifypeer = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
  /* defaults to any auth type */
  سیٹ->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  سیٹ->new_directory_perms = 0755; /* Default permissions */
#اگر_ختم

  سیٹ->new_file_perms = 0644;    /* Default permissions */
  سیٹ->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  سیٹ->redir_protocols = CURLPROTO_REDIR;

#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  سیٹ->socks5_gssapi_nec = FALSE;
#اگر_ختم

  /* set default minimum TLS version */
#اگر_تعریف_شدہ USE_SSL
  Curl_setopt_SSLVERSION(ڈیٹا, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(ڈیٹا, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#اگر_ختم
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  سیٹ->wildcard_enabled = FALSE;
  سیٹ->chunk_bgn = ZERO_NULL;
  سیٹ->chunk_انتہا = ZERO_NULL;
  سیٹ->fnmatch = ZERO_NULL;
#اگر_ختم
  سیٹ->tcp_keepalive = FALSE;
  سیٹ->tcp_keepintvl = 60;
  سیٹ->tcp_keepidle = 60;
  سیٹ->tcp_keepcnt = 9;
  سیٹ->tcp_fastopen = FALSE;
  سیٹ->tcp_nodelay = TRUE;
  سیٹ->ssl_enable_alpn = TRUE;
  سیٹ->expect_100_timeout = 1000L; /* Wait for a second by default. */
  سیٹ->sep_سرخیاں = TRUE; /* separated header lists by default */
  سیٹ->بفر_سائز = READBUFFER_سائز;
  سیٹ->upload_بفر_سائز = UPLOADBUFFER_DEFAULT;
  سیٹ->upload_flags = CURLULFLAG_نظر_آیا;
  سیٹ->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  سیٹ->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  سیٹ->maxconnects = DEFAULT_CONNCACHE_سائز; /* for easy handles */
  سیٹ->conn_max_idle_ms = 118 * 1000;
  سیٹ->conn_max_age_ms = 24 * 3600 * 1000;
  سیٹ->http09_allowed = FALSE;
  سیٹ->httpwant = CURL_HTTP_نسخہ_NONE;
#اگر defined(USE_HTTP2) || defined(USE_HTTP3)
  سیٹ->وزن = 0;
#اگر_ختم
  سیٹ->quick_exit = 0L;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  سیٹ->ws_raw_منوال = FALSE;
  سیٹ->ws_no_auto_pong = FALSE;
#اگر_ختم
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ساکن خالی easy_meta_freeentry(خالی *p)
{
  (خالی)p;
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
CURLcode Curl_open(ساخت Curl_easy **curl)
{
  ساخت Curl_easy *ڈیٹا;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  ڈیٹا = curlx_calloc(1, حجم(ساخت Curl_easy));
  اگر(!ڈیٹا) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    واپس CURLE_برآمد_کا_حافظہ;
  }

  ڈیٹا->magic = CURLEASY_MAGIC_نمبر;
  /* most recent connection is not yet defined */
  ڈیٹا->state.آخری_conn_id = -1;
  ڈیٹا->state.آخری_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  ڈیٹا->id = -1;
  ڈیٹا->mid = UINT32_MAX;
  ڈیٹا->master_mid = UINT32_MAX;
  ڈیٹا->پیش_رفت.hide = TRUE;

  Curl_hash_init(&ڈیٹا->meta_hash, 23, CURL_HASH_TYPE_بائٹیں,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_آخری <= UINT8_MAX);
  Curl_u8_strset_init(&ڈیٹا->سیٹ.لڑیاں);
  curlx_dyn_init(&ڈیٹا->state.headerb, CURL_MAX_HTTP_سرخی);
  Curl_bufref_init(&ڈیٹا->state.url);
  Curl_bufref_init(&ڈیٹا->state.referer);
  Curl_req_init(&ڈیٹا->req);
  Curl_initinfo(ڈیٹا);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  Curl_llist_init(&ڈیٹا->state.httphdrs, تہی);
#اگر_ختم
  Curl_netrc_init(&ڈیٹا->state.netrc);
  Curl_init_userdefined(ڈیٹا);

  *curl = ڈیٹا;
  واپس CURLE_OK;
}

خالی Curl_conn_free(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  اگر(conn->scheme && conn->scheme->تسلسل->disconnect &&
     !conn->بٹ.shutdown_handler)
    conn->scheme->تسلسل->disconnect(ڈیٹا, conn, TRUE);

  برائے(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(ڈیٹا, conn, i);
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#اگر_ختم
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->بذریعہ_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->بذریعہ_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
ساکن bool xfer_may_multiplex(ثابت ساخت Curl_easy *ڈیٹا,
                               ثابت ساخت connectdata *conn)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  اگر((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->بٹ.protoconnstart || !conn->بٹ.close)) {

    اگر(Curl_multiplex_wanted(ڈیٹا->multi) &&
       (ڈیٹا->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      واپس TRUE;
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)conn;
#اگر_ختم
  واپس FALSE;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
ساکن bool proxy_info_matches(ثابت ساخت proxy_info *ڈیٹا,
                               ثابت ساخت proxy_info *needle)
{
  اگر((ڈیٹا->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(ڈیٹا->peer, needle->peer) &&
     Curl_creds_same(ڈیٹا->creds, needle->creds)) {
    واپس TRUE;
  }
  واپس FALSE;
}
#اگر_ختم

#اگر_تعریف_شدہ USE_SSH
ساکن bool ssh_config_matches(ساخت connectdata *one,
                               ساخت connectdata *two)
{
  ساخت ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  واپس sshc1 && sshc2 && Curl_safecmp(sshc1->priv_کلید, sshc2->priv_کلید) &&
         Curl_safecmp(sshc1->pub_کلید, sshc2->pub_کلید);
}
#اگر_ختم

ساخت url_conn_مطابقت {
  ساخت connectdata *ملا;
  ساخت Curl_easy *ڈیٹا;
  ساخت connectdata *needle;
  ساخت curltime now;
  ساخت ssl_filter_config ssl_config;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  ساخت ssl_filter_config proxy_ssl_config;
#اگر_ختم
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
  ثنائی_ہندسہ(نظر_آیا_pending_conn);
  ثنائی_ہندسہ(نظر_آیا_single_use_conn);
  ثنائی_ہندسہ(نظر_آیا_multiplex_conn);
};

ساکن bool url_مطابقت_connect_config(ساخت connectdata *conn,
                                     ساخت url_conn_مطابقت *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  اگر(conn->بٹ.connect_only || conn->بٹ.close || conn->بٹ.no_reuse)
    واپس FALSE;

  /* ip_version must match */
  اگر(m->ڈیٹا->سیٹ.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->ڈیٹا->سیٹ.ipver != conn->ip_نسخہ)
    واپس FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  اگر(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    واپس FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  اگر((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    واپس FALSE;

  اگر((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    واپس FALSE;

  اگر(!m->needle->بذریعہ_peer != !conn->بذریعہ_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    واپس FALSE;

  واپس TRUE;
}

ساکن bool url_مطابقت_fully_connected(ساخت connectdata *conn,
                                      ساخت url_conn_مطابقت *m)
{
  اگر(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->بٹ.upgrade_اندر_پیش_رفت) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    اگر(m->may_multiplex) {
      m->نظر_آیا_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->ڈیٹا, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    واپس FALSE;
  }
  واپس TRUE;
}

ساکن bool url_مطابقت_multi(ساخت connectdata *conn,
                            ساخت url_conn_مطابقت *m)
{
  اگر(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    اگر(conn->attached_multi != m->ڈیٹا->multi)
      واپس FALSE;
  }
  واپس TRUE;
}

ساکن bool url_مطابقت_multiplex_needs(ساخت connectdata *conn,
                                      ساخت url_conn_مطابقت *m)
{
  اگر(CONN_INUSE(conn)) {
    اگر(!conn->بٹ.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->نظر_آیا_single_use_conn = TRUE;
      واپس FALSE;
    }
    m->نظر_آیا_multiplex_conn = TRUE;
    اگر(!m->may_multiplex || !url_مطابقت_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      واپس FALSE;
  }
  واپس TRUE;
}

ساکن bool url_مطابقت_multiplex_limits(ساخت connectdata *conn,
                                       ساخت url_conn_مطابقت *m)
{
  اگر(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->بٹ.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    اگر(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->ڈیٹا->multi)) {
      infof(m->ڈیٹا, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      واپس FALSE;
    }
    اگر(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->ڈیٹا, conn, FIRSTSOCKET)) {
      infof(m->ڈیٹا, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      واپس FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->ڈیٹا, "Multiplexed connection found");
  }
  واپس TRUE;
}

ساکن bool url_مطابقت_ssl_use(ساخت connectdata *conn,
                              ساخت url_conn_مطابقت *m)
{
  اگر(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      واپس FALSE;
  }
  ورنہ اگر(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    اگر(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      واپس FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    اگر(!Curl_ssl_conn_config_مطابقت(m->ڈیٹا, &m->ssl_config, conn, FALSE))
      واپس FALSE;
  }
  ورنہ اگر(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    واپس FALSE;
  واپس TRUE;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
ساکن bool url_مطابقت_proxy_use(ساخت connectdata *conn,
                                ساخت url_conn_مطابقت *m)
{
  اگر(m->needle->بٹ.origin_is_proxy != conn->بٹ.origin_is_proxy)
    واپس FALSE;

  اگر(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    واپس FALSE;

  اگر(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    واپس FALSE;

  اگر(m->ڈیٹا->سیٹ.socks5_auth_only && !conn->بٹ.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->ڈیٹا,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    واپس FALSE;
  }

  اگر(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    اگر(!Curl_ssl_conn_config_مطابقت(m->ڈیٹا, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->ڈیٹا,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      واپس FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  واپس TRUE;
}
#ورنہ
#تعریف_کرو url_مطابقت_proxy_use(c, m) ((خالی)(c), (خالی)(m), TRUE)
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
ساکن bool url_مطابقت_http_multiplex(ساخت connectdata *conn,
                                     ساخت url_conn_مطابقت *m,
                                     bool *pwait_pipe)
{
  اگر(m->may_multiplex &&
     (m->ڈیٹا->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_نظر_آیا) {
    اگر(m->ڈیٹا->سیٹ.pipewait) {
      infof(m->ڈیٹا, "Server upgrade does not support multiplex yet, wait");
      m->ملا = تہی;
      *pwait_pipe = TRUE;
      واپس TRUE; /* stop searching, we want to wait */
    }
    infof(m->ڈیٹا, "Server upgrade cannot be used");
    واپس FALSE;
  }
  واپس TRUE;
}

ساکن bool url_مطابقت_http_نسخہ(ساخت connectdata *conn,
                                   ساخت url_conn_مطابقت *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  اگر((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    انتخاب(Curl_conn_http_نسخہ(m->ڈیٹا, conn)) {
    حالت 30:
      اگر(!(m->ڈیٹا->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->ڈیٹا, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        واپس FALSE;
      }
      توڑو;
    حالت 20:
      اگر(!(m->ڈیٹا->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->ڈیٹا, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        واپس FALSE;
      }
      توڑو;
    طےشدہ:
      اگر(!(m->ڈیٹا->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->ڈیٹا, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        واپس FALSE;
      }
      توڑو;
    }
  }
  واپس TRUE;
}
#ورنہ
#تعریف_کرو url_مطابقت_http_multiplex(c, m, w) ((خالی)(c), (خالی)(m), TRUE)
#تعریف_کرو url_مطابقت_http_نسخہ(c, m)      ((خالی)(c), (خالی)(m), TRUE)
#اگر_ختم

ساکن bool url_مطابقت_proto_config(ساخت connectdata *conn,
                                   ساخت url_conn_مطابقت *m)
{
  اگر(!url_مطابقت_http_نسخہ(conn, m))
    واپس FALSE;

#اگر_تعریف_شدہ USE_SSH
  اگر(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    اگر(!ssh_config_matches(m->needle, conn))
      واپس FALSE;
  }
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  ورنہ اگر(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    اگر(!Curl_ftp_conns_مطابقت(m->needle, conn))
      واپس FALSE;
  }
#اگر_ختم
  واپس TRUE;
}

ساکن bool url_مطابقت_auth(ساخت connectdata *conn,
                           ساخت url_conn_مطابقت *m)
{
  اگر(!Curl_creds_same(m->needle->creds, conn->creds)) {
    اگر(m->needle->creds)
      واپس FALSE;
    اگر(!Curl_creds_same(m->ڈیٹا->state.creds, conn->creds))
      واپس FALSE;
  }
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  اگر(m->needle->gssapi_delegation != conn->gssapi_delegation)
    واپس FALSE;
#اگر_ختم

  واپس TRUE;
}

ساکن bool url_مطابقت_destination(ساخت connectdata *conn,
                                  ساخت url_conn_مطابقت *m)
{
  /* Different connect-to peers never match */
  اگر(!Curl_peer_same_destination(m->needle->بذریعہ_peer, conn->بذریعہ_peer))
    واپس FALSE;

  اگر(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    واپس FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  واپس Curl_peer_same_destination(m->needle->origin, conn->origin);
}

ساکن bool url_مطابقت_ssl_config(ساخت connectdata *conn,
                                 ساخت url_conn_مطابقت *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  اگر(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_مطابقت(m->ڈیٹا, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->ڈیٹا, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    واپس FALSE;
  }
  واپس TRUE;
}

#اگر_تعریف_شدہ USE_NTLM
ساکن bool url_مطابقت_auth_ntlm(ساخت connectdata *conn,
                                ساخت url_conn_مطابقت *m)
{
  اگر(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    اگر(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->ڈیٹا->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->ڈیٹا->state.origin))
      واپس FALSE;
  }
  ورنہ اگر(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    اگر(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->ڈیٹا->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->ڈیٹا->state.origin)))
      واپس FALSE;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  اگر(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    اگر(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      واپس FALSE;
  }
  ورنہ اگر(m->want_proxy_ntlm_http) {
    اگر(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      واپس FALSE;
  }
#اگر_ختم
  واپس TRUE;
}
#ورنہ
#تعریف_کرو url_مطابقت_auth_ntlm(c, m) ((خالی)(c), (خالی)(m), TRUE)
#اگر_ختم

#اگر_تعریف_شدہ USE_SPNEGO
ساکن bool url_مطابقت_auth_nego(ساخت connectdata *conn,
                                ساخت url_conn_مطابقت *m)
{
  اگر(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    اگر(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->ڈیٹا->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->ڈیٹا->state.origin))
      واپس FALSE;
  }
  ورنہ اگر(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    اگر(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->ڈیٹا->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->ڈیٹا->state.origin)))
      واپس FALSE;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  اگر(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    اگر(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      واپس FALSE;
  }
  ورنہ اگر(m->want_proxy_nego_http) {
    اگر(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      واپس FALSE;
  }
#اگر_ختم
  واپس TRUE;
}
#ورنہ
#تعریف_کرو url_مطابقت_auth_nego(c, m) ((خالی)(c), (خالی)(m), TRUE)
#اگر_ختم

ساکن cpool_مطابقت_نتیجہ url_مطابقت_conn(ساخت connectdata *conn,
                                         خالی *userdata)
{
  ساخت url_conn_مطابقت *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  اگر(!url_مطابقت_connect_config(conn, m))
    واپس CPOOL_مطابقت_CONT;

  /* match for destination and protocol? */
  اگر(!url_مطابقت_destination(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_fully_connected(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_multiplex_needs(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_ssl_use(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_proxy_use(conn, m))
    واپس CPOOL_مطابقت_CONT;
  اگر(!url_مطابقت_ssl_config(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_http_multiplex(conn, m, &wait_pipe))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_auth(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_proto_config(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_auth_ntlm(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_auth_nego(conn, m))
    واپس CPOOL_مطابقت_CONT;

  اگر(!url_مطابقت_multiplex_limits(conn, m))
    واپس CPOOL_مطابقت_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  اگر(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->ملا);
    m->wait_pipe = TRUE;
    واپس CPOOL_مطابقت_ملا;
  }

  اگر(m->ڈیٹا->سیٹ.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->ڈیٹا, conn, &m->now);
    اگر(age_ms > m->ڈیٹا->سیٹ.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      واپس CPOOL_مطابقت_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  اگر(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->ڈیٹا, &m->now)) {
    infof(m->ڈیٹا, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    واپس CPOOL_مطابقت_CLOSE;
  }

  /* conn matches our needs. */
  m->ملا = conn;
  واپس CPOOL_مطابقت_ملا;
}

ساکن bool url_مطابقت_نتیجہ(خالی *userdata)
{
  ساخت url_conn_مطابقت *مطابقت = userdata;
  اگر(مطابقت->ملا) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(مطابقت->ڈیٹا, مطابقت->ملا, TRUE);
    واپس TRUE;
  }
  ورنہ اگر(مطابقت->نظر_آیا_single_use_conn && !مطابقت->نظر_آیا_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    مطابقت->wait_pipe = FALSE;
  }
  ورنہ اگر(مطابقت->نظر_آیا_pending_conn && مطابقت->ڈیٹا->سیٹ.pipewait) {
    infof(مطابقت->ڈیٹا,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    مطابقت->wait_pipe = TRUE;
  }
  واپس FALSE;
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
ساکن bool url_attach_existing(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *needle,
                                ساخت url_conn_مطابقت *m,
                                ثابت ساخت curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  واپس Curl_cpool_ڈھونڈو(ڈیٹا, needle->destination, TRUE, pnow,
                         url_مطابقت_conn, url_مطابقت_نتیجہ, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
ساکن ساخت connectdata *allocate_conn(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = curlx_calloc(1, حجم(ساخت connectdata));
  اگر(!conn)
    واپس تہی;

  /* and we setup a few fields in case we end up actually using this struct */
  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->recv_idx = 0; /* default for receiving transfer data */
  conn->send_idx = 0; /* default for sending transfer data */
  conn->connection_id = -1;    /* no ID */
  conn->cpid = UINT32_MAX;  /* not in connection pool yet */
  conn->attached_xfers = 0;
  conn->shutdown.آغاز_ms[FIRSTSOCKET] =
    conn->shutdown.آغاز_ms[SECONDARYSOCKET] = -1;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  conn->بٹ.ftp_use_epsv = ڈیٹا->سیٹ.ftp_use_epsv;
  conn->بٹ.ftp_use_eprt = ڈیٹا->سیٹ.ftp_use_eprt;
#اگر_ختم
  conn->ip_نسخہ = ڈیٹا->سیٹ.ipver;
  conn->بٹ.connect_only = (bool)ڈیٹا->سیٹ.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  اگر(CURL_EASY_STR(ڈیٹا, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(ڈیٹا, STRING_INTERFACE_OPT));
    اگر(!conn->interface_opts)
      جاؤ error;
  }
#اگر_غیر_تعریف_شدہ CURL_DISABLE_BINDLOCAL
  conn->localportrange = ڈیٹا->سیٹ.localportrange;
  conn->localport = ڈیٹا->سیٹ.localport;
#اگر_ختم

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = ڈیٹا->سیٹ.fclosesocket;
  conn->closesocket_client = ڈیٹا->سیٹ.closesocket_client;
#اگر defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = ڈیٹا->سیٹ.gssapi_delegation;
#اگر_ختم
  واپس conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  واپس تہی;
}

ساکن CURLcode url_سیٹ_conn_scheme(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ثابت ساخت Curl_scheme *scheme = conn->origin->scheme;
  اگر(scheme->تسلسل &&
     (ڈیٹا->سیٹ.allowed_protocols & scheme->protocol) &&
     (!ڈیٹا->state.this_is_a_follow ||
       (ڈیٹا->سیٹ.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    واپس CURLE_OK;
  }
  اگر(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(ڈیٹا, "Protocol \"%s\" is not for transfers", scheme->name);
  ورنہ
    failf(ڈیٹا, "Protocol \"%s\" is disabled%s", scheme->name,
          ڈیٹا->state.this_is_a_follow ? " (in redirect)" : "");
  واپس CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_مقصد_curlcode(CURLUcode uc)
{
  انتخاب(uc) {
  طےشدہ:
    واپس CURLE_URL_MALFORMAT;
  حالت CURLUE_UNSUPPORTED_SCHEME:
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  حالت CURLUE_برآمد_کا_حافظہ:
    واپس CURLE_برآمد_کا_حافظہ;
  حالت CURLUE_USER_NOT_ALLOWED:
    واپس CURLE_LOGIN_DENIED;
  }
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
ساکن CURLcode hsts_upgrade(ساخت Curl_easy *ڈیٹا,
                             CURLU *uh,
                             uint16_t پورٹ_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  اگر(ڈیٹا->hsts && (ڈیٹا->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(ڈیٹا->hsts, ڈیٹا->state.origin)) {
    حرف *url;
    CURLUcode uc;
    CURLcode نتیجہ;

    uc = curl_url_سیٹ(uh, CURLUPART_SCHEME, "https", 0);
    اگر(uc)
      واپس Curl_uc_مقصد_curlcode(uc);
    Curl_bufref_free(&ڈیٹا->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    اگر(uc)
      واپس Curl_uc_مقصد_curlcode(uc);
    Curl_bufref_سیٹ(&ڈیٹا->state.url, url, 0, curl_free);

    نتیجہ = Curl_peer_from_url(uh, ڈیٹا, پورٹ_override, scope_id,
                                &ڈیٹا->state.origin);
    اگر(نتیجہ)
      واپس نتیجہ;
    infof(ڈیٹا, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  واپس CURLE_OK;
}
#ورنہ
#تعریف_کرو hsts_upgrade(x, y, z, a) CURLE_OK
#اگر_ختم

ساکن bool str_has_ctrl(ثابت حرف *داخلہ)
{
  اگر(داخلہ) {
    ثابت غیرعلامتی حرف *str = (ثابت غیرعلامتی حرف *)داخلہ;
    جب(*str) {
      اگر(*str < 0x20)
        واپس TRUE;
      str++;
    }
  }
  واپس FALSE;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ساکن CURLcode url_سیٹ_ڈیٹا_creds_netrc(ساخت Curl_easy *ڈیٹا,
                                         ساخت Curl_creds **pcreds)
{
  ساخت Curl_creds *ncreds_برآمد = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(ڈیٹا->سیٹ.use_netrc) { /* not CURL_NETRC_IGNORED */
    ساخت Curl_creds *ncreds_اندر = تہی;
    bool جائزہ_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    اگر(*pcreds) {
      انتخاب((*pcreds)->ماخذ) {
      حالت CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        جائزہ_netrc = FALSE;
        توڑو;
      حالت CREDS_URL: /* only apply when netrc is not required */
        اگر(ڈیٹا->سیٹ.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_اندر = *pcreds;
        }
        ورنہ اگر(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_اندر = *pcreds;
        }
        ورنہ
          جائزہ_netrc = FALSE;
        توڑو;
      طےشدہ: /* ignore credentials from other sources */
        توڑو;
      }
    }

    اگر(!جائزہ_netrc)
      جاؤ برآمد;

    ret = Curl_netrc_جائزہ(ڈیٹا, &ڈیٹا->state.netrc,
                          ڈیٹا->state.origin->hostname,
                          Curl_creds_user(ncreds_اندر),
                          CURL_EASY_STR(ڈیٹا, STRING_NETRC_FILE),
                          &ncreds_برآمد);
    DEBUGASSERT(!ret || !ncreds_برآمد);
    اگر(ret == NETRC_برآمد_کا_حافظہ) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
    ورنہ اگر(ret && ((ret == NETRC_NO_مطابقت) ||
                    (ڈیٹا->سیٹ.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(ڈیٹا, "Could not find host %s in the %s file; using defaults",
            ڈیٹا->state.origin->hostname,
            (CURL_EASY_STR(ڈیٹا, STRING_NETRC_FILE) ?
             CURL_EASY_STR(ڈیٹا, STRING_NETRC_FILE) : ".netrc"));
    }
    ورنہ اگر(ret) {
      ثابت حرف *m = Curl_netrc_strerror(ret);
      failf(ڈیٹا, ".netrc error: %s", m);
      نتیجہ = CURLE_READ_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(ncreds_برآمد) {
      اگر(!(ڈیٹا->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_برآمد->user) ||
          str_has_ctrl(ncreds_برآمد->passwd))) {
        failf(ڈیٹا, "control code detected in .netrc credentials");
        نتیجہ = CURLE_READ_ERROR;
        جاؤ برآمد;
      }
      CURL_TRC_M(ڈیٹا, "netrc: using credentials for %s as %s",
                 ڈیٹا->state.origin->hostname, ncreds_برآمد->user);
      نتیجہ = Curl_creds_ملاؤ(ncreds_برآمد->user, ncreds_برآمد->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      اگر(نتیجہ)
        جاؤ برآمد;
      /* for updated strings, we update them in the URL */
      uc = curl_url_سیٹ(ڈیٹا->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      اگر(!uc)
        uc = curl_url_سیٹ(ڈیٹا->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      اگر(uc)
        نتیجہ = Curl_uc_مقصد_curlcode(uc);
    }
    ورنہ
      DEBUGASSERT(0);
  }

#اگر_تعریف_شدہ CURLVERBOSE
  Curl_creds_trace(ڈیٹا, ڈیٹا->state.creds, "transfer credentials");
#اگر_ختم

برآمد:
  Curl_creds_unlink(&ncreds_برآمد);
  واپس نتیجہ;
}
#اگر_ختم /* CURL_DISABLE_NETRC */

ساکن CURLcode url_سیٹ_ڈیٹا_creds(ساخت Curl_easy *ڈیٹا, CURLU *uh)
{
  ساخت Curl_creds *newcreds = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر((CURL_EASY_STR(ڈیٹا, STRING_USERNAME) ||
      CURL_EASY_STR(ڈیٹا, STRING_PASSWORD) ||
      CURL_EASY_STR(ڈیٹا, STRING_BEARER) ||
      CURL_EASY_STR(ڈیٹا, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(ڈیٹا, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_مقصد_origin(ڈیٹا, ڈیٹا->state.origin)) {
    نتیجہ = Curl_creds_create(CURL_EASY_STR(ڈیٹا, STRING_USERNAME),
                               CURL_EASY_STR(ڈیٹا, STRING_PASSWORD),
                               CURL_EASY_STR(ڈیٹا, STRING_BEARER),
                               CURL_EASY_STR(ڈیٹا, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(ڈیٹا, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    اگر(نتیجہ)
      جاؤ برآمد;
    اگر(newcreds &&
       !(ڈیٹا->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(ڈیٹا, "control code detected in credentials");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  اگر(!newcreds || !Curl_creds_has_user(newcreds)) {
    حرف *user = تہی;
    حرف *passwd = تہی;
    حرف *udecoded = تہی;
    حرف *pdecoded = تہی;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    اگر(uc && (uc != CURLUE_NO_USER))
      نتیجہ = Curl_uc_مقصد_curlcode(uc);
    اگر(!نتیجہ) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      اگر(uc && (uc != CURLUE_NO_PASSWORD))
        نتیجہ = Curl_uc_مقصد_curlcode(uc);
    }
    اگر(!نتیجہ && user) {
      نتیجہ = Curl_urldecode(user, 0, &udecoded, تہی,
                              (ڈیٹا->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    اگر(!نتیجہ && passwd) {
      نتیجہ = Curl_urldecode(passwd, 0, &pdecoded, تہی,
                              (ڈیٹا->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    اگر(!نتیجہ)
      نتیجہ = Curl_creds_ملاؤ(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    اگر(نتیجہ) {
      failf(ڈیٹا, "error extracting credentials from URL");
      جاؤ برآمد;
    }
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  نتیجہ = url_سیٹ_ڈیٹا_creds_netrc(ڈیٹا, &newcreds);
#اگر_ختم /* CURL_DISABLE_NETRC */

برآمد:
  اگر(!نتیجہ && !Curl_creds_equal(ڈیٹا->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&ڈیٹا->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  واپس نتیجہ;
}

ساکن CURLcode url_سیٹ_conn_origin_etc(ساخت Curl_easy *ڈیٹا,
                                        ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;

  Curl_peer_link(&conn->origin, ڈیٹا->state.origin);

  /* set the connection scheme */
  نتیجہ = url_سیٹ_conn_scheme(ڈیٹا, conn);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* set the connection options */
  اگر(CURL_EASY_STR(ڈیٹا, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(ڈیٹا, STRING_OPTIONS));
    اگر(!conn->options) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
  ورنہ اگر(ڈیٹا->state.اوپر.options) {
    conn->options = curlx_strdup(ڈیٹا->state.اوپر.options);
    اگر(!conn->options) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

برآمد:
  واپس نتیجہ;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ساکن CURLcode تیاری_range(ساخت Curl_easy *ڈیٹا)
{
  ساخت UrlState *s = &ڈیٹا->state;
  s->resume_from = ڈیٹا->سیٹ.سیٹ_resume_from;
  اگر(s->resume_from || CURL_EASY_STR(ڈیٹا, STRING_سیٹ_RANGE)) {
    اگر(s->rangestringalloc)
      curlx_free(s->range);

    اگر(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    ورنہ
      s->range = curlx_strdup(CURL_EASY_STR(ڈیٹا, STRING_سیٹ_RANGE));

    اگر(!s->range)
      واپس CURLE_برآمد_کا_حافظہ;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  ورنہ
    s->use_range = FALSE; /* disable range download */

  واپس CURLE_OK;
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
ساکن CURLcode تیاری_connection_internals(ساخت Curl_easy *ڈیٹا,
                                           ساخت connectdata *conn)
{
  ساخت Curl_peer *peer = تہی;

  اگر(conn->scheme->تسلسل->تیاری_connection) {
    CURLcode نتیجہ = conn->scheme->تسلسل->تیاری_connection(ڈیٹا, conn);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  اگر(!peer)
    واپس CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  اگر(peer->ipv6)
    اگر(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->پورٹ);
    ورنہ
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->پورٹ);
  ورنہ
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->پورٹ);
  اگر(!conn->destination)
    واپس CURLE_برآمد_کا_حافظہ;

  Curl_strntolower(conn->destination, conn->destination,
                   لڑی_لمبائی(conn->destination));

#اگر_تعریف_شدہ USE_IPV6
  اگر(ڈیٹا->سیٹ.scope_id)
    conn->scope_id = ڈیٹا->سیٹ.scope_id;
  ورنہ {
    peer = Curl_conn_get_پہلا_peer(conn, FIRSTSOCKET);
    اگر(!peer)
      واپس CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#اگر_ختم

  واپس CURLE_OK;
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
  حرف *ubuf = تہی;
  حرف *pbuf = تہی;
  ثابت حرف *psep = تہی;
  ثابت حرف *osep = تہی;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  اگر(optionsp)
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
  اگر(!ubuf)
    جاؤ error;

  /* Clone the password portion buffer */
  اگر(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    اگر(!pbuf)
      جاؤ error;
  }

  /* Allocate the options portion buffer */
  اگر(optionsp) {
    حرف *obuf = تہی;
    اگر(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      اگر(!obuf)
        جاؤ error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  واپس CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  واپس CURLE_برآمد_کا_حافظہ;
}

/*
 * Set the login details so they are available in the connection
 */
ساکن CURLcode url_سیٹ_conn_login(ساخت Curl_easy *ڈیٹا,
                                   ساخت connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  اگر((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, ڈیٹا->state.origin);
    اگر(ڈیٹا->state.creds)
      Curl_creds_link(&conn->creds, ڈیٹا->state.creds);
    ورنہ
      واپس Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               تہی, تہی, تہی, CREDS_NONE, &conn->creds);
  }
  ورنہ اگر(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, ڈیٹا->state.origin);
    Curl_creds_link(&conn->creds, ڈیٹا->state.creds);
  }

  واپس CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
ساکن CURLcode parse_connect_مقصد_string(ساخت Curl_easy *ڈیٹا,
                                        ثابت ساخت Curl_peer *dest,
                                        ثابت حرف *conn_مقصد_سطر,
                                        ساخت Curl_peer **pvia_dest)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn_مقصد_سطر;
  bool host_مطابقت = FALSE;
  bool پورٹ_مطابقت = FALSE;

  *pvia_dest = تہی;

  اگر(*ptr == ':') {
    /* an empty hostname always matches */
    host_مطابقت = TRUE;
    ptr++;
  }
  ورنہ {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = لڑی_لمبائی(dest->hostname);
    host_مطابقت = curl_strnequal(ptr, dest->hostname, hlen);
    اگر(!host_مطابقت && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = لڑی_لمبائی(dest->user_hostname);
      host_مطابقت = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_مطابقت = host_مطابقت && ptr[hlen] == ':';
    اگر(host_مطابقت)
      ptr += hlen + 1;
  }

  اگر(host_مطابقت) {
    اگر(*ptr == ':') {
      /* an empty port always matches */
      پورٹ_مطابقت = TRUE;
      ptr++;
    }
    ورنہ {
      /* check whether the URL's port matches */
      ثابت حرف *ptr_اگلا = strchr(ptr, ':');
      اگر(ptr_اگلا) {
        curl_off_t پورٹ_مقصد_مطابقت;
        اگر(!curlx_str_نمبر(&ptr, &پورٹ_مقصد_مطابقت, 0xffff) &&
           ((uint16_t)پورٹ_مقصد_مطابقت == dest->پورٹ)) {
          پورٹ_مطابقت = TRUE;
        }
        ptr = ptr_اگلا + 1;
      }
    }
  }

  اگر(host_مطابقت && پورٹ_مطابقت && ptr && *ptr)
    نتیجہ = Curl_peer_from_connect_مقصد(ڈیٹا, dest, ptr, pvia_dest);

  واپس نتیجہ;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ساکن CURLcode url_سیٹ_conn_peer(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت Curl_peer *origin = conn->origin;
  ساخت Curl_peer *بذریعہ_peer = تہی;
  ساخت curl_slist *conn_مقصد_اندراج = ڈیٹا->سیٹ.connect_مقصد;

  DEBUGASSERT(!conn->بذریعہ_peer);
  Curl_peer_unlink(&conn->بذریعہ_peer);

  جب(conn_مقصد_اندراج && !بذریعہ_peer) {
    نتیجہ = parse_connect_مقصد_string(ڈیٹا, origin, conn_مقصد_اندراج->ڈیٹا,
                                     &بذریعہ_peer);
    اگر(نتیجہ)
      واپس نتیجہ;
    conn_مقصد_اندراج = conn_مقصد_اندراج->اگلا;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  اگر(ڈیٹا->asi && !بذریعہ_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#اگر_تعریف_شدہ DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#ورنہ
      0
#اگر_ختم
       )) {
    /* no connect_to match, try alt-svc! */
    شمار alpnid srcalpnid = ALPN_none;
    bool کامیابی = FALSE;
    ساخت altsvc *as = تہی;
    صحیح allowed_alpns = ALPN_none;
    ساخت http_negotiation *neg = &ڈیٹا->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(ڈیٹا, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#اگر_تعریف_شدہ USE_HTTP3
    اگر(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTP2
    اگر(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#اگر_ختم
    اگر(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (صحیح)ڈیٹا->asi->flags;

    DEBUGF(infof(ڈیٹا, "check Alt-Svc for host '%s'", origin->hostname));
#اگر_تعریف_شدہ USE_HTTP3
    اگر(!کامیابی && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      کامیابی = Curl_altsvc_lookup(ڈیٹا->asi,
                               origin, ALPN_h3, /* from */
                               &as /*مقصد*/,
                               allowed_alpns, &same_dest);
    }
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTP2
    اگر(!کامیابی && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_پیشگی_knowledge) {
      srcalpnid = ALPN_h2;
      کامیابی = Curl_altsvc_lookup(ڈیٹا->asi,
                               origin, ALPN_h2, /* from */
                               &as /*مقصد*/,
                               allowed_alpns, &same_dest);
    }
#اگر_ختم
    اگر(!کامیابی && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      کامیابی = Curl_altsvc_lookup(ڈیٹا->asi,
                               origin, ALPN_h1, /* from */
                               &as /*مقصد*/,
                               allowed_alpns, &same_dest);
    }

    اگر(کامیابی && same_dest) {
      /* same destination, but more HTTPS version options */
      انتخاب(as->dst.alpnid) {
      حالت ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        توڑو;
      حالت ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        توڑو;
      حالت ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        توڑو;
      طےشدہ: /* should not be possible */
        توڑو;
      }
    }
    ورنہ اگر(کامیابی) {
      نتیجہ = Curl_peer_create(ڈیٹا, conn->origin->scheme,
                                as->dst.host, as->dst.پورٹ,
                                &بذریعہ_peer);
      اگر(نتیجہ)
        واپس نتیجہ;
      infof(ڈیٹا, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->پورٹ,
            Curl_alpnid2str(as->dst.alpnid),
            بذریعہ_peer->hostname, بذریعہ_peer->پورٹ);
      conn->بٹ.altused = TRUE;
      اگر(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        انتخاب(as->dst.alpnid) {
        حالت ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          توڑو;
        حالت ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          توڑو;
        حالت ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          توڑو;
        طےشدہ: /* should not be possible */
          توڑو;
        }
      }
    }
  }
#اگر_ختم

  اگر(بذریعہ_peer)
    conn->بذریعہ_peer = بذریعہ_peer;

  واپس نتیجہ;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
ساکن خالی url_conn_reuse_adjust(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *needle)
{
  ساخت connectdata *conn = ڈیٹا->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  اگر(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#اگر_ختم

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
  Curl_peer_link(&conn->بذریعہ_peer, needle->بذریعہ_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->بذریعہ_peer2, needle->بذریعہ_peer2);
}

ساکن خالی conn_meta_freeentry(خالی *p)
{
  (خالی)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

ساکن CURLcode url_create_needle(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata **pneedle)
{
  ساخت connectdata *needle = تہی;
  CURLcode نتیجہ = CURLE_OK;
  bool جال_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(ڈیٹا);
  اگر(!needle) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_بائٹیں,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  نتیجہ = url_سیٹ_conn_origin_etc(ڈیٹا, needle);
  اگر(نتیجہ)
    جاؤ برآمد;

  DEBUGASSERT(needle->origin);
  جال_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  اگر(جال_scheme && CURL_EASY_STR(ڈیٹا, STRING_UNIX_SOCKET_راستہ)) {
    نتیجہ = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(ڈیٹا, STRING_UNIX_SOCKET_راستہ),
      (bool)ڈیٹا->سیٹ.abstract_unix_socket, &needle->بذریعہ_peer);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
#اگر_ختم /* USE_UNIX_SOCKETS */

  اگر(جال_scheme && !needle->بذریعہ_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    نتیجہ = url_سیٹ_conn_peer(ڈیٹا, needle);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  اگر(Curl_peer_equal(needle->origin, needle->بذریعہ_peer)) {
    Curl_peer_unlink(&needle->بذریعہ_peer);
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  اگر(جال_scheme &&
     (!needle->بذریعہ_peer || !needle->بذریعہ_peer->unix_socket)) {
    نتیجہ = Curl_proxy_init_conn(ڈیٹا, needle);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
#اگر_ختم /* CURL_DISABLE_PROXY */

  نتیجہ = url_سیٹ_conn_login(ڈیٹا, needle); /* default credentials */
  اگر(نتیجہ)
    جاؤ برآمد;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  نتیجہ = تیاری_connection_internals(ڈیٹا, needle);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    اگر(ڈیٹا->سیٹ.ssl_enable_alpn)
      needle->بٹ.tls_enable_alpn = TRUE;
  }

  اگر(جال_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->بٹ.tcp_fastopen = ڈیٹا->سیٹ.tcp_fastopen;
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
    اگر(Curl_conn_get_پہلا_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#اگر_ختم
  }

برآمد:
  اگر(!نتیجہ) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  ورنہ {
    *pneedle = تہی;
    اگر(needle)
      Curl_conn_free(ڈیٹا, needle);
  }
  واپس نتیجہ;
}

ساکن CURLcode url_سیٹ_ڈیٹا_origin_and_creds(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_سیٹ_uh = (ڈیٹا->سیٹ.uh && !ڈیٹا->state.this_is_a_follow);
  uint16_t پورٹ_override = ڈیٹا->state.allow_پورٹ ? ڈیٹا->سیٹ.use_پورٹ : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  اگر(!Curl_bufref_ptr(&ڈیٹا->state.url)) {
    نتیجہ = CURLE_URL_MALFORMAT;
    جاؤ برآمد;
  }

  اوپر_free(ڈیٹا); /* cleanup previous leftovers first */

  /* parse the URL */
  اگر(use_سیٹ_uh)
    uh = ڈیٹا->state.uh = curl_url_dup(ڈیٹا->سیٹ.uh);
  ورنہ
    uh = ڈیٹا->state.uh = curl_url();
  اگر(!uh) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  اگر(!use_سیٹ_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    اگر(curl_strequal("file", CURL_EASY_STR(ڈیٹا, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&ڈیٹا->state.url), تہی, 0, TRUE))
      prepend_sheme = TRUE;
    ورنہ {
      uc = curl_url_سیٹ(uh, CURLUPART_URL, Curl_bufref_ptr(&ڈیٹا->state.url),
                        (غیرعلامتی صحیح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (ڈیٹا->سیٹ.disallow_username_اندر_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (ڈیٹا->سیٹ.راستہ_as_is ? CURLU_راستہ_AS_IS : 0));
      اگر(uc) {
        failf(ڈیٹا, "URL rejected: %s", curl_url_strerror(uc));
        نتیجہ = Curl_uc_مقصد_curlcode(uc);
        جاؤ برآمد;
      }

      اگر(CURL_EASY_STR(ڈیٹا, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        حرف *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        اگر(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        اگر(uc && uc != CURLUE_NO_SCHEME) {
          نتیجہ = Curl_uc_مقصد_curlcode(uc);
          جاؤ برآمد;
        }
      }
    }

    اگر(prepend_sheme) {
      حرف *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(ڈیٹا, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&ڈیٹا->state.url));
      اگر(!url) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      uc = curl_url_سیٹ(uh, CURLUPART_URL, url,
                        (غیرعلامتی صحیح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (ڈیٹا->سیٹ.disallow_username_اندر_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (ڈیٹا->سیٹ.راستہ_as_is ? CURLU_راستہ_AS_IS : 0));
      curl_free(url);
      اگر(uc) {
        failf(ڈیٹا, "URL rejected: %s", curl_url_strerror(uc));
        نتیجہ = Curl_uc_مقصد_curlcode(uc);
        جاؤ برآمد;
      }
    }
    {
      حرف *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_خالی_خانے);
      اگر(uc) {
        نتیجہ = Curl_uc_مقصد_curlcode(uc);
        جاؤ برآمد;
      }
      اگر(prepend_sheme)
        infof(ڈیٹا, "Applied default protocol '%s'",
              CURL_EASY_STR(ڈیٹا, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_سیٹ(&ڈیٹا->state.url, newurl, 0, curl_free);
    }
  }

#اگر_تعریف_شدہ USE_IPV6
  scope_id = ڈیٹا->سیٹ.scope_id;
#اگر_ختم

  /* `uh` is now as the connection should use it, probably. */
  نتیجہ = Curl_peer_from_url(uh, ڈیٹا, پورٹ_override, scope_id,
                              &ڈیٹا->state.origin);
  اگر(نتیجہ)
    جاؤ برآمد;
  /* The origin might get changed when HSTS applies */
  نتیجہ = hsts_upgrade(ڈیٹا, uh, پورٹ_override, scope_id);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  اگر(!ڈیٹا->state.initial_origin)
    Curl_peer_link(&ڈیٹا->state.initial_origin, ڈیٹا->state.origin);

  uc = curl_url_get(uh, CURLUPART_راستہ, &ڈیٹا->state.اوپر.راستہ, CURLU_URLENCODE);
  اگر(uc) {
    نتیجہ = Curl_uc_مقصد_curlcode(uc);
    جاؤ برآمد;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &ڈیٹا->state.اوپر.query,
                    CURLU_GET_خالی_خانے);
  اگر(uc && (uc != CURLUE_NO_QUERY)) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &ڈیٹا->state.اوپر.options,
                    CURLU_URLDECODE);
  اگر(uc && (uc != CURLUE_NO_OPTIONS)) {
    نتیجہ = Curl_uc_مقصد_curlcode(uc);
    جاؤ برآمد;
  }

  نتیجہ = url_سیٹ_ڈیٹا_creds(ڈیٹا, uh);
  اگر(نتیجہ)
    جاؤ برآمد;

برآمد:
  واپس نتیجہ;
}

ساکن CURLcode url_مطابقت_init(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *needle,
                               ساخت url_conn_مطابقت *m)
{
  memset(m, 0, حجم(*m));
  m->ڈیٹا = ڈیٹا;
  m->needle = needle;
  m->now = *Curl_pgrs_now(ڈیٹا);
  m->may_multiplex = xfer_may_multiplex(ڈیٹا, needle);

#اگر_تعریف_شدہ USE_NTLM
  m->want_ntlm_http =
    (ڈیٹا->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_مقصد_host(ڈیٹا);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (ڈیٹا->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#اگر_ختم
#اگر_ختم

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (ڈیٹا->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_مقصد_host(ڈیٹا);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (ڈیٹا->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#اگر_ختم
#اگر_ختم
  m->require_tls = ڈیٹا->سیٹ.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = ڈیٹا->سیٹ.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  واپس Curl_ssl_filter_config_tmp_init(ڈیٹا, needle->origin,
                                         &m->ssl_config,
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#ورنہ
                                         تہی);
#اگر_ختم
}

ساکن خالی url_مطابقت_destroy(ساخت url_conn_مطابقت *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#اگر_ختم
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ساکن CURLcode url_ڈھونڈو_or_create_conn(ساخت Curl_easy *ڈیٹا,
                                        ثابت ساخت curltime *pnow)
{
  ساخت connectdata *needle = تہی;
  ساخت url_conn_مطابقت مطابقت;
  bool مطابقت_initialized = FALSE;
  CURLcode نتیجہ;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  نتیجہ = url_create_needle(ڈیٹا, &needle);
  اگر(نتیجہ)
    جاؤ برآمد;
  DEBUGASSERT(needle);

  نتیجہ = url_مطابقت_init(ڈیٹا, needle, &مطابقت);
  اگر(نتیجہ)
    جاؤ برآمد;
  مطابقت_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FILE
  اگر(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool مکمل;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->تسلسل->connect_it);
    ڈیٹا->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    ڈیٹا->info.conn_protocol = needle->scheme->protocol & CURLPROTO_نقاب;
    نتیجہ = needle->scheme->تسلسل->connect_it(ڈیٹا, &مکمل);
    اگر(نتیجہ)
      جاؤ برآمد;

    /* Setup a "faked" transfer that will do nothing */
    نتیجہ = Curl_cpool_جوڑو(ڈیٹا, needle, 0, 0, pnow);
    Curl_attach_connection(ڈیٹا, needle, TRUE);
    needle = تہی;
    اگر(!نتیجہ) {
      /* Setup whatever necessary for a resumed transfer */
      نتیجہ = تیاری_range(ڈیٹا);
      اگر(!نتیجہ) {
        Curl_xfer_تیاری_nop(ڈیٹا);
        نتیجہ = Curl_init_transfer(ڈیٹا, ڈیٹا->conn);
      }
    }

    اگر(نتیجہ) {
      DEBUGASSERT(ڈیٹا->conn->scheme->تسلسل->مکمل);
      /* we ignore the return code for the protocol-specific DONE */
      (خالی)ڈیٹا->conn->scheme->تسلسل->مکمل(ڈیٹا, نتیجہ, FALSE);
    }
    جاؤ برآمد;
  }
#اگر_ختم

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  اگر((!ڈیٹا->سیٹ.reuse_fresh || ڈیٹا->state.followlocation) &&
     !ڈیٹا->سیٹ.connect_only) {
    /* Ok, try to find and attach an existing one */
    اگر(url_attach_existing(ڈیٹا, needle, &مطابقت, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      ساخت connectdata *conn = ڈیٹا->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->بٹ.reuse = TRUE;
      url_conn_reuse_adjust(ڈیٹا, needle);

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
      infof(ڈیٹا, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#ورنہ
      infof(ڈیٹا, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#اگر_ختم
    }
  }

  اگر(!ڈیٹا->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    اگر(مطابقت.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(ڈیٹا, "Waiting on connection to negotiate possible multiplexing.");
      نتیجہ = CURLE_NO_CONNECTION_دستیاب;
      جاؤ برآمد;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    نتیجہ = Curl_cpool_جوڑو(ڈیٹا, needle,
                            ڈیٹا->multi->max_کل_connections,
                            ڈیٹا->multi->max_host_connections,
                            pnow);
    اگر(نتیجہ)
      جاؤ برآمد;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(ڈیٹا, needle, TRUE);
    needle = تہی;

    /* Clone the SSL filter configs into the new connection. */
    نتیجہ = Curl_ssl_conn_config_clone(&مطابقت.ssl_config,
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
                                        &مطابقت.proxy_ssl_config,
#ورنہ
                                        تہی,
#اگر_ختم
                                        ڈیٹا->conn);
    اگر(نتیجہ) {
      DEBUGF(infof(ڈیٹا, "Error: clone connection SSL config"));
      جاؤ برآمد;
    }

#اگر_تعریف_شدہ USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    اگر((ڈیٹا->state.authhost.picked & CURLAUTH_NTLM) &&
       ڈیٹا->state.authhost.مکمل) {
      infof(ڈیٹا, "NTLM picked AND auth done set, clear picked");
      ڈیٹا->state.authhost.picked = CURLAUTH_NONE;
      ڈیٹا->state.authhost.مکمل = FALSE;
    }

    اگر((ڈیٹا->state.authproxy.picked & CURLAUTH_NTLM) &&
       ڈیٹا->state.authproxy.مکمل) {
      infof(ڈیٹا, "NTLM-proxy picked AND auth done set, clear picked");
      ڈیٹا->state.authproxy.picked = CURLAUTH_NONE;
      ڈیٹا->state.authproxy.مکمل = FALSE;
    }
#اگر_ختم
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  نتیجہ = Curl_init_transfer(ڈیٹا, ڈیٹا->conn);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* Setup whatever necessary for a resumed transfer */
  نتیجہ = تیاری_range(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* persist the scheme and handler the transfer is using */
  ڈیٹا->info.conn_scheme = ڈیٹا->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  ڈیٹا->info.conn_protocol = ڈیٹا->conn->scheme->protocol & CURLPROTO_نقاب;
  ڈیٹا->info.مستعمل_proxy =
#اگر_تعریف_شدہ CURL_DISABLE_PROXY
    0
#ورنہ
    (ڈیٹا->conn->socks_proxy.peer || ڈیٹا->conn->http_proxy.peer)
#اگر_ختم
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  نتیجہ = Curl_conn_ev_ڈیٹا_تیاری(ڈیٹا);

برآمد:
  اگر(مطابقت_initialized)
    url_مطابقت_destroy(&مطابقت);
  اگر(needle)
    Curl_conn_free(ڈیٹا, needle);
  DEBUGASSERT(نتیجہ || ڈیٹا->conn);
  واپس نتیجہ;
}

CURLcode Curl_connect(ساخت Curl_easy *ڈیٹا, bool *pconnected)
{
  CURLcode نتیجہ;
  ساخت connectdata *conn = تہی;
  ثابت ساخت curltime *pnow = تہی;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&ڈیٹا->req, ڈیٹا);
  /* Determine the origin of the transfer and what credentials to use */
  نتیجہ = url_سیٹ_ڈیٹا_origin_and_creds(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(!ڈیٹا->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(ڈیٹا);
  Curl_pgrs_وقت_Was(ڈیٹا, TIMER_POSTQUEUE, *pnow);
  نتیجہ = url_ڈھونڈو_or_create_conn(ڈیٹا, pnow);
  conn = ڈیٹا->conn;
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(!ڈیٹا->conn) { /* just make really sure */
    DEBUGASSERT(0);
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  اگر(conn->بٹ.reuse) {
    اگر(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  ورنہ اگر(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_وقت(ڈیٹا, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  ورنہ {
    نتیجہ = Curl_conn_تیاری(ڈیٹا, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    اگر(!نتیجہ)
      نتیجہ = Curl_سرخیاں_init(ڈیٹا);
    CURL_TRC_M(ڈیٹا, "Curl_conn_setup() -> %d", (صحیح)نتیجہ);
  }

برآمد:
  اگر(نتیجہ == CURLE_NO_CONNECTION_دستیاب)
    DEBUGASSERT(!conn);

  اگر(نتیجہ && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(ڈیٹا);
    Curl_conn_close(ڈیٹا, conn, TRUE);
  }

  واپس نتیجہ;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn)
{
  CURLcode نتیجہ;

  اگر(conn) {
    conn->بٹ.do_مزید = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    اگر(ڈیٹا->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      ڈیٹا->state.wildcardmatch = FALSE;
  }

  ڈیٹا->state.مکمل = FALSE; /* *_done() is not called yet */

  ڈیٹا->req.no_body = ڈیٹا->سیٹ.opt_no_body;
  اگر(ڈیٹا->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    ڈیٹا->state.httpreq = HTTPREQ_سر;

  نتیجہ = Curl_req_آغاز(&ڈیٹا->req, ڈیٹا);
  اگر(!نتیجہ) {
    Curl_pgrsReset(ڈیٹا);
  }
  واپس نتیجہ;
}

#اگر defined(USE_HTTP2) || defined(USE_HTTP3)

خالی Curl_ڈیٹا_ترجیح_clear_state(ساخت Curl_easy *ڈیٹا)
{
  ڈیٹا->state.وزن = 0;
}

#اگر_ختم /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_سیٹ(ساخت connectdata *conn, ثابت حرف *کلید,
                            خالی *meta_ڈیٹا, Curl_meta_dtor *meta_dtor)
{
  اگر(!Curl_hash_add2(&conn->meta_hash, کلید, لڑی_لمبائی(کلید) + 1,
                     meta_ڈیٹا, meta_dtor)) {
    meta_dtor(کلید, لڑی_لمبائی(کلید) + 1, meta_ڈیٹا);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  واپس CURLE_OK;
}

خالی Curl_conn_meta_remove(ساخت connectdata *conn, ثابت حرف *کلید)
{
  Curl_hash_delete(&conn->meta_hash, کلید, لڑی_لمبائی(کلید) + 1);
}

خالی *Curl_conn_meta_get(ساخت connectdata *conn, ثابت حرف *کلید)
{
  واپس Curl_hash_pick(&conn->meta_hash, کلید, لڑی_لمبائی(کلید) + 1);
}

ساخت Curl_easy *Curl_get_admin(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_easy *admin;

  اگر(!ڈیٹا->mid) /* already an admin handle */
    admin = ڈیٹا;
  ورنہ اگر(ڈیٹا->multi)
    admin = ڈیٹا->multi->admin;
  ورنہ اگر(ڈیٹا->multi_easy)
    admin = ڈیٹا->multi_easy->admin;
  ورنہ {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = ڈیٹا;
  }
  اگر(admin != ڈیٹا) {
    admin->سیٹ.conn_max_idle_ms = ڈیٹا->سیٹ.conn_max_idle_ms;
    admin->سیٹ.conn_max_age_ms = ڈیٹا->سیٹ.conn_max_age_ms;
    admin->سیٹ.upkeep_interval_ms = ڈیٹا->سیٹ.upkeep_interval_ms;
    admin->سیٹ.timeout = ڈیٹا->سیٹ.timeout;
    admin->سیٹ.server_response_timeout = ڈیٹا->سیٹ.server_response_timeout;
    admin->سیٹ.no_signal = ڈیٹا->سیٹ.no_signal;
  }
  واپس admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  اگر(r1 && (r1 != CURLE_AGAIN))
    واپس r1;
  اگر(r2 && (r2 != CURLE_AGAIN))
    واپس r2;
  واپس r1;
}
