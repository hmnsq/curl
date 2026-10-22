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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_IPHLPAPI_H
#ضمن <Iphlpapi.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_UN_H
#ضمن <sys/un.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل HAVE_SOCKET
#غلطي "We cannot compile without socket() support"
#جيڪڏهن_پورو

#إذا defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ضمن <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#جيڪڏهن_پورو
#ضمن <iphlpapi.h>
#جيڪڏهن_پورو

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
#إذا READBUFFER_سائيز < READBUFFER_MIN
# error READBUFFER_سائيز is too small
#جيڪڏهن_پورو

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
  CURL_EASY_STR_CLEAR0(معلومات, STRING_ڪنجي_PASSWD);
  CURL_EASY_STR_CLEAR0(معلومات, STRING_BEARER);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(معلومات, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(معلومات, STRING_ڪنجي_PASSWD_PROXY);
#جيڪڏهن_پورو
  Curl_u8_strset_clear(&معلومات->سيٽ.لڙيون);
  curlx_safefree(معلومات->سيٽ.str_copypostfields);

  لكل(j = (تعداد dupblob)0; j < BLOB_آخري; j++) {
    curlx_safefree(معلومات->سيٽ.blobs[j]);
  }

  Curl_bufref_free(&معلومات->state.referer);
  Curl_bufref_free(&معلومات->state.url);

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(معلومات->سيٽ.mimepostp);
  curlx_safefree(معلومات->سيٽ.mimepostp);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  curl_slist_free_all(معلومات->state.cookielist);
  معلومات->state.cookielist = عدم;
#جيڪڏهن_پورو
}

/* free the URL pieces */
ساكن فراغ مٿي_free(بنية Curl_easy *معلومات)
{
  بنية urlpieces *مٿي = &معلومات->state.مٿي;
  curlx_safefree(مٿي->options);
  curlx_safefree(مٿي->رستو);
  curlx_safefree(مٿي->query);
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

  إذا(!معلومات->state.اندروني && معلومات->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(معلومات->multi, معلومات);
  }
  وإلا {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(معلومات);
    إذا(!معلومات->state.اندروني && معلومات->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(معلومات->multi_easy);
      معلومات->multi_easy = عدم;
    }
  }
  DEBUGASSERT(!معلومات->conn || معلومات->state.اندروني);

  Curl_expire_clear_all(معلومات); /* shut off any timers left */

  إذا(معلومات->state.rangestringalloc)
    curlx_free(معلومات->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(معلومات);

  معلومات->سيٽ.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
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
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  Curl_peer_unlink(&معلومات->state.rtsp_session_origin);
#جيڪڏهن_پورو
  Curl_ssl_free_certinfo(معلومات);

  Curl_bufref_free(&معلومات->state.referer);

  مٿي_free(معلومات);
  curlx_dyn_free(&معلومات->state.headerb);
  Curl_flush_cookies(معلومات, TRUE);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  Curl_altsvc_save(معلومات, معلومات->asi, CURL_EASY_STR(معلومات, STRING_ALTSVC));
  Curl_altsvc_cleanup(&معلومات->asi);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
  Curl_hsts_save(معلومات, معلومات->hsts, CURL_EASY_STR(معلومات, STRING_HSTS));
  إذا(!معلومات->share || !معلومات->share->hsts)
    Curl_hsts_cleanup(&معلومات->hsts);
  curl_slist_free_all(معلومات->state.hstslist); /* clean up list */
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(معلومات);
#جيڪڏهن_پورو
  curlx_safefree(معلومات->state.most_recent_ftp_entrypath);
  curlx_safefree(معلومات->info.contenttype);
  curlx_safefree(معلومات->info.wouldredirect);
  curlx_safefree(معلومات->info.effective_url);

  /* No longer a dirty share, if it exists */
  إذا(Curl_share_easy_unlink(معلومات))
    DEBUGASSERT(0);

  Curl_hash_destroy(&معلومات->meta_hash);
  Curl_creds_unlink(&معلومات->state.creds);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  curlx_safefree(معلومات->state.rangeline);
  curlx_safefree(معلومات->state.http_host);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
  curlx_safefree(معلومات->req.cookiehost);
#جيڪڏهن_پورو

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(معلومات->state.formp);
  curlx_safefree(معلومات->state.formp);
#جيڪڏهن_پورو

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&معلومات->wildcard);
  Curl_freeset(معلومات);
  Curl_سرا_cleanup(معلومات);
  Curl_netrc_cleanup(&معلومات->state.netrc);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DIGEST_AUTH
  curlx_free(معلومات->state.envproxy);
#جيڪڏهن_پورو
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
  بنية UserDefined *سيٽ = &معلومات->سيٽ;

  سيٽ->ٻاهريون = stdout;  /* default output to stdout */
  سيٽ->اندريون_سيٽ = stdin;  /* default input from stdin */
  سيٽ->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&معلومات->سيٽ.لڙيون);

#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wcast-function-type-strict"
#جيڪڏهن_پورو
  /* use fwrite as default function to store output */
  سيٽ->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  سيٽ->fread_func_سيٽ = (curl_read_callback)fread;
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
  سيٽ->is_fread_سيٽ = 0;

  سيٽ->seek_client = ZERO_NULL;

  سيٽ->filesize = -1;        /* we do not know the size */
  سيٽ->postfieldsize = -1;   /* unknown size */
  سيٽ->maxredirs = 30;       /* sensible default */

  سيٽ->طريقو = HTTPREQ_GET; /* Default HTTP request */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  سيٽ->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  سيٽ->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  سيٽ->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  سيٽ->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  سيٽ->ftp_filemethod = FTPFILE_MULTICWD;
  سيٽ->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#جيڪڏهن_پورو
  سيٽ->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  سيٽ->ssl_ca_cache_timeout = 24 * 60 * 60;

  سيٽ->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&معلومات->سيٽ.ssl);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  Curl_ssl_config_init(&معلومات->سيٽ.proxy_ssl);
  سيٽ->proxyport = 0;
  سيٽ->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  سيٽ->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  سيٽ->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
  سيٽ->doh_verifyhost = TRUE;
  سيٽ->doh_verifypeer = TRUE;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
  /* defaults to any auth type */
  سيٽ->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  سيٽ->new_directory_perms = 0755; /* Default permissions */
#جيڪڏهن_پورو

  سيٽ->new_file_perms = 0644;    /* Default permissions */
  سيٽ->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  سيٽ->redir_protocols = CURLPROTO_REDIR;

#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  سيٽ->socks5_gssapi_nec = FALSE;
#جيڪڏهن_پورو

  /* set default minimum TLS version */
#جيڪڏهن_وصف_ٿيل USE_SSL
  Curl_setopt_SSLVERSION(معلومات, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(معلومات, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#جيڪڏهن_پورو
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  سيٽ->wildcard_enabled = FALSE;
  سيٽ->chunk_bgn = ZERO_NULL;
  سيٽ->chunk_پڇاڙي = ZERO_NULL;
  سيٽ->fnmatch = ZERO_NULL;
#جيڪڏهن_پورو
  سيٽ->tcp_keepalive = FALSE;
  سيٽ->tcp_keepintvl = 60;
  سيٽ->tcp_keepidle = 60;
  سيٽ->tcp_keepcnt = 9;
  سيٽ->tcp_fastopen = FALSE;
  سيٽ->tcp_nodelay = TRUE;
  سيٽ->ssl_enable_alpn = TRUE;
  سيٽ->expect_100_timeout = 1000L; /* Wait for a second by default. */
  سيٽ->sep_سرا = TRUE; /* separated header lists by default */
  سيٽ->بفر_سائيز = READBUFFER_سائيز;
  سيٽ->upload_بفر_سائيز = UPLOADBUFFER_DEFAULT;
  سيٽ->upload_flags = CURLULFLAG_ڏٺل;
  سيٽ->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  سيٽ->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  سيٽ->maxconnects = DEFAULT_CONNCACHE_سائيز; /* for easy handles */
  سيٽ->conn_max_idle_ms = 118 * 1000;
  سيٽ->conn_max_age_ms = 24 * 3600 * 1000;
  سيٽ->http09_allowed = FALSE;
  سيٽ->httpwant = CURL_HTTP_نسخو_NONE;
#إذا defined(USE_HTTP2) || defined(USE_HTTP3)
  سيٽ->وزن = 0;
#جيڪڏهن_پورو
  سيٽ->quick_exit = 0L;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
  سيٽ->ws_raw_گهڻائي = FALSE;
  سيٽ->ws_no_auto_pong = FALSE;
#جيڪڏهن_پورو
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
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  معلومات->magic = CURLEASY_MAGIC_عدد;
  /* most recent connection is not yet defined */
  معلومات->state.آخري_conn_id = -1;
  معلومات->state.آخري_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  معلومات->id = -1;
  معلومات->mid = UINT32_MAX;
  معلومات->master_mid = UINT32_MAX;
  معلومات->ترقي.hide = TRUE;

  Curl_hash_init(&معلومات->meta_hash, 23, CURL_HASH_TYPE_بائيٽون,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_آخري <= UINT8_MAX);
  Curl_u8_strset_init(&معلومات->سيٽ.لڙيون);
  curlx_dyn_init(&معلومات->state.headerb, CURL_MAX_HTTP_سرو);
  Curl_bufref_init(&معلومات->state.url);
  Curl_bufref_init(&معلومات->state.referer);
  Curl_req_init(&معلومات->req);
  Curl_initinfo(معلومات);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  Curl_llist_init(&معلومات->state.httphdrs, عدم);
#جيڪڏهن_پورو
  Curl_netrc_init(&معلومات->state.netrc);
  Curl_init_userdefined(معلومات);

  *curl = معلومات;
  أرجع CURLE_OK;
}

فراغ Curl_conn_free(بنية Curl_easy *معلومات, بنية connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  إذا(conn->scheme && conn->scheme->لاڳيتو->disconnect &&
     !conn->بٽ.shutdown_handler)
    conn->scheme->لاڳيتو->disconnect(معلومات, conn, TRUE);

  لكل(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(معلومات, conn, i);
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#جيڪڏهن_پورو
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->ذريعي_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->ذريعي_peer2);

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
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  إذا((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->بٽ.protoconnstart || !conn->بٽ.close)) {

    إذا(Curl_multiplex_wanted(معلومات->multi) &&
       (معلومات->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      أرجع TRUE;
  }
#وإلا
  (فراغ)معلومات;
  (فراغ)conn;
#جيڪڏهن_پورو
  أرجع FALSE;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SSH
ساكن bool ssh_config_matches(بنية connectdata *one,
                               بنية connectdata *two)
{
  بنية ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  أرجع sshc1 && sshc2 && Curl_safecmp(sshc1->priv_ڪنجي, sshc2->priv_ڪنجي) &&
         Curl_safecmp(sshc1->pub_ڪنجي, sshc2->pub_ڪنجي);
}
#جيڪڏهن_پورو

بنية url_conn_ميلاپ {
  بنية connectdata *مليو;
  بنية Curl_easy *معلومات;
  بنية connectdata *needle;
  بنية curltime now;
  بنية ssl_filter_config ssl_config;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  بنية ssl_filter_config proxy_ssl_config;
#جيڪڏهن_پورو
  ٻٽو_انگ(may_multiplex);
  ٻٽو_انگ(want_ntlm_http);
  ٻٽو_انگ(want_proxy_ntlm_http);
  ٻٽو_انگ(want_nego_http);
  ٻٽو_انگ(want_proxy_nego_http);
  ٻٽو_انگ(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  ٻٽو_انگ(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  ٻٽو_انگ(wait_pipe);
  ٻٽو_انگ(ڏٺل_pending_conn);
  ٻٽو_انگ(ڏٺل_single_use_conn);
  ٻٽو_انگ(ڏٺل_multiplex_conn);
};

ساكن bool url_ميلاپ_connect_config(بنية connectdata *conn,
                                     بنية url_conn_ميلاپ *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  إذا(conn->بٽ.connect_only || conn->بٽ.close || conn->بٽ.no_reuse)
    أرجع FALSE;

  /* ip_version must match */
  إذا(m->معلومات->سيٽ.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->معلومات->سيٽ.ipver != conn->ip_نسخو)
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

  إذا(!m->needle->ذريعي_peer != !conn->ذريعي_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    أرجع FALSE;

  أرجع TRUE;
}

ساكن bool url_ميلاپ_fully_connected(بنية connectdata *conn,
                                      بنية url_conn_ميلاپ *m)
{
  إذا(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->بٽ.upgrade_اندريون_ترقي) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    إذا(m->may_multiplex) {
      m->ڏٺل_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->معلومات, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_ميلاپ_multi(بنية connectdata *conn,
                            بنية url_conn_ميلاپ *m)
{
  إذا(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    إذا(conn->attached_multi != m->معلومات->multi)
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_ميلاپ_multiplex_needs(بنية connectdata *conn,
                                      بنية url_conn_ميلاپ *m)
{
  إذا(CONN_INUSE(conn)) {
    إذا(!conn->بٽ.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->ڏٺل_single_use_conn = TRUE;
      أرجع FALSE;
    }
    m->ڏٺل_multiplex_conn = TRUE;
    إذا(!m->may_multiplex || !url_ميلاپ_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_ميلاپ_multiplex_limits(بنية connectdata *conn,
                                       بنية url_conn_ميلاپ *m)
{
  إذا(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->بٽ.multiplex);
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

ساكن bool url_ميلاپ_ssl_use(بنية connectdata *conn,
                              بنية url_conn_ميلاپ *m)
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
    إذا(!Curl_ssl_conn_config_ميلاپ(m->معلومات, &m->ssl_config, conn, FALSE))
      أرجع FALSE;
  }
  وإلا إذا(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    أرجع FALSE;
  أرجع TRUE;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
ساكن bool url_ميلاپ_proxy_use(بنية connectdata *conn,
                                بنية url_conn_ميلاپ *m)
{
  إذا(m->needle->بٽ.origin_is_proxy != conn->بٽ.origin_is_proxy)
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    أرجع FALSE;

  إذا(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    أرجع FALSE;

  إذا(m->معلومات->سيٽ.socks5_auth_only && !conn->بٽ.socks5_authenticated &&
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
    إذا(!Curl_ssl_conn_config_ميلاپ(m->معلومات, &m->proxy_ssl_config,
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
#وصف_ڪريو url_ميلاپ_proxy_use(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
ساكن bool url_ميلاپ_http_multiplex(بنية connectdata *conn,
                                     بنية url_conn_ميلاپ *m,
                                     bool *pwait_pipe)
{
  إذا(m->may_multiplex &&
     (m->معلومات->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_ڏٺل) {
    إذا(m->معلومات->سيٽ.pipewait) {
      infof(m->معلومات, "Server upgrade does not support multiplex yet, wait");
      m->مليو = عدم;
      *pwait_pipe = TRUE;
      أرجع TRUE; /* stop searching, we want to wait */
    }
    infof(m->معلومات, "Server upgrade cannot be used");
    أرجع FALSE;
  }
  أرجع TRUE;
}

ساكن bool url_ميلاپ_http_نسخو(بنية connectdata *conn,
                                   بنية url_conn_ميلاپ *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  إذا((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    بدل(Curl_conn_http_نسخو(m->معلومات, conn)) {
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
#وصف_ڪريو url_ميلاپ_http_multiplex(c, m, w) ((فراغ)(c), (فراغ)(m), TRUE)
#وصف_ڪريو url_ميلاپ_http_نسخو(c, m)      ((فراغ)(c), (فراغ)(m), TRUE)
#جيڪڏهن_پورو

ساكن bool url_ميلاپ_proto_config(بنية connectdata *conn,
                                   بنية url_conn_ميلاپ *m)
{
  إذا(!url_ميلاپ_http_نسخو(conn, m))
    أرجع FALSE;

#جيڪڏهن_وصف_ٿيل USE_SSH
  إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    إذا(!ssh_config_matches(m->needle, conn))
      أرجع FALSE;
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  وإلا إذا(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    إذا(!Curl_ftp_conns_ميلاپ(m->needle, conn))
      أرجع FALSE;
  }
#جيڪڏهن_پورو
  أرجع TRUE;
}

ساكن bool url_ميلاپ_auth(بنية connectdata *conn,
                           بنية url_conn_ميلاپ *m)
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
#جيڪڏهن_پورو

  أرجع TRUE;
}

ساكن bool url_ميلاپ_destination(بنية connectdata *conn,
                                  بنية url_conn_ميلاپ *m)
{
  /* Different connect-to peers never match */
  إذا(!Curl_peer_same_destination(m->needle->ذريعي_peer, conn->ذريعي_peer))
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

ساكن bool url_ميلاپ_ssl_config(بنية connectdata *conn,
                                 بنية url_conn_ميلاپ *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  إذا(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_ميلاپ(m->معلومات, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->معلومات, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    أرجع FALSE;
  }
  أرجع TRUE;
}

#جيڪڏهن_وصف_ٿيل USE_NTLM
ساكن bool url_ميلاپ_auth_ntlm(بنية connectdata *conn,
                                بنية url_conn_ميلاپ *m)
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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
#جيڪڏهن_پورو
  أرجع TRUE;
}
#وإلا
#وصف_ڪريو url_ميلاپ_auth_ntlm(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SPNEGO
ساكن bool url_ميلاپ_auth_nego(بنية connectdata *conn,
                                بنية url_conn_ميلاپ *m)
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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
#جيڪڏهن_پورو
  أرجع TRUE;
}
#وإلا
#وصف_ڪريو url_ميلاپ_auth_nego(c, m) ((فراغ)(c), (فراغ)(m), TRUE)
#جيڪڏهن_پورو

ساكن cpool_ميلاپ_نتيجو url_ميلاپ_conn(بنية connectdata *conn,
                                         فراغ *userdata)
{
  بنية url_conn_ميلاپ *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  إذا(!url_ميلاپ_connect_config(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  /* match for destination and protocol? */
  إذا(!url_ميلاپ_destination(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_fully_connected(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_multiplex_needs(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_ssl_use(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_proxy_use(conn, m))
    أرجع CPOOL_ميلاپ_CONT;
  إذا(!url_ميلاپ_ssl_config(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_http_multiplex(conn, m, &wait_pipe))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_auth(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_proto_config(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_auth_ntlm(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_auth_nego(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  إذا(!url_ميلاپ_multiplex_limits(conn, m))
    أرجع CPOOL_ميلاپ_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  إذا(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->مليو);
    m->wait_pipe = TRUE;
    أرجع CPOOL_ميلاپ_مليو;
  }

  إذا(m->معلومات->سيٽ.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->معلومات, conn, &m->now);
    إذا(age_ms > m->معلومات->سيٽ.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      أرجع CPOOL_ميلاپ_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  إذا(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->معلومات, &m->now)) {
    infof(m->معلومات, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    أرجع CPOOL_ميلاپ_CLOSE;
  }

  /* conn matches our needs. */
  m->مليو = conn;
  أرجع CPOOL_ميلاپ_مليو;
}

ساكن bool url_ميلاپ_نتيجو(فراغ *userdata)
{
  بنية url_conn_ميلاپ *ميلاپ = userdata;
  إذا(ميلاپ->مليو) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(ميلاپ->معلومات, ميلاپ->مليو, TRUE);
    أرجع TRUE;
  }
  وإلا إذا(ميلاپ->ڏٺل_single_use_conn && !ميلاپ->ڏٺل_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    ميلاپ->wait_pipe = FALSE;
  }
  وإلا إذا(ميلاپ->ڏٺل_pending_conn && ميلاپ->معلومات->سيٽ.pipewait) {
    infof(ميلاپ->معلومات,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    ميلاپ->wait_pipe = TRUE;
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
                                بنية url_conn_ميلاپ *m,
                                ثابت بنية curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  أرجع Curl_cpool_ڳوليو(معلومات, needle->destination, TRUE, pnow,
                         url_ميلاپ_conn, url_ميلاپ_نتيجو, m);
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
  conn->shutdown.شروعات_ms[FIRSTSOCKET] =
    conn->shutdown.شروعات_ms[SECONDARYSOCKET] = -1;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  conn->بٽ.ftp_use_epsv = معلومات->سيٽ.ftp_use_epsv;
  conn->بٽ.ftp_use_eprt = معلومات->سيٽ.ftp_use_eprt;
#جيڪڏهن_پورو
  conn->ip_نسخو = معلومات->سيٽ.ipver;
  conn->بٽ.connect_only = (bool)معلومات->سيٽ.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  إذا(CURL_EASY_STR(معلومات, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(معلومات, STRING_INTERFACE_OPT));
    إذا(!conn->interface_opts)
      اذهب error;
  }
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_BINDLOCAL
  conn->localportrange = معلومات->سيٽ.localportrange;
  conn->localport = معلومات->سيٽ.localport;
#جيڪڏهن_پورو

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = معلومات->سيٽ.fclosesocket;
  conn->closesocket_client = معلومات->سيٽ.closesocket_client;
#إذا defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = معلومات->سيٽ.gssapi_delegation;
#جيڪڏهن_پورو
  أرجع conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  أرجع عدم;
}

ساكن CURLcode url_سيٽ_conn_scheme(بنية Curl_easy *معلومات,
                                    بنية connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ثابت بنية Curl_scheme *scheme = conn->origin->scheme;
  إذا(scheme->لاڳيتو &&
     (معلومات->سيٽ.allowed_protocols & scheme->protocol) &&
     (!معلومات->state.this_is_a_follow ||
       (معلومات->سيٽ.redir_protocols & scheme->protocol))) {
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
  حالة CURLUE_ٻاهريون_جو_يادگيري:
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  حالة CURLUE_USER_NOT_ALLOWED:
    أرجع CURLE_LOGIN_DENIED;
  }
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
ساكن CURLcode hsts_upgrade(بنية Curl_easy *معلومات,
                             CURLU *uh,
                             uint16_t پورٽ_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  إذا(معلومات->hsts && (معلومات->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(معلومات->hsts, معلومات->state.origin)) {
    حرف *url;
    CURLUcode uc;
    CURLcode نتيجو;

    uc = curl_url_سيٽ(uh, CURLUPART_SCHEME, "https", 0);
    إذا(uc)
      أرجع Curl_uc_منزل_curlcode(uc);
    Curl_bufref_free(&معلومات->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    إذا(uc)
      أرجع Curl_uc_منزل_curlcode(uc);
    Curl_bufref_سيٽ(&معلومات->state.url, url, 0, curl_free);

    نتيجو = Curl_peer_from_url(uh, معلومات, پورٽ_override, scope_id,
                                &معلومات->state.origin);
    إذا(نتيجو)
      أرجع نتيجو;
    infof(معلومات, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  أرجع CURLE_OK;
}
#وإلا
#وصف_ڪريو hsts_upgrade(x, y, z, a) CURLE_OK
#جيڪڏهن_پورو

ساكن bool str_has_ctrl(ثابت حرف *داخلا)
{
  إذا(داخلا) {
    ثابت بنانشان حرف *str = (ثابت بنانشان حرف *)داخلا;
    طالما(*str) {
      إذا(*str < 0x20)
        أرجع TRUE;
      str++;
    }
  }
  أرجع FALSE;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ساكن CURLcode url_سيٽ_معلومات_creds_netrc(بنية Curl_easy *معلومات,
                                         بنية Curl_creds **pcreds)
{
  بنية Curl_creds *ncreds_ٻاهريون = عدم;
  CURLcode نتيجو = CURLE_OK;

  إذا(معلومات->سيٽ.use_netrc) { /* not CURL_NETRC_IGNORED */
    بنية Curl_creds *ncreds_اندريون = عدم;
    bool جاچ_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    إذا(*pcreds) {
      بدل((*pcreds)->ذريعو) {
      حالة CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        جاچ_netrc = FALSE;
        اكسر;
      حالة CREDS_URL: /* only apply when netrc is not required */
        إذا(معلومات->سيٽ.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_اندريون = *pcreds;
        }
        وإلا إذا(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_اندريون = *pcreds;
        }
        وإلا
          جاچ_netrc = FALSE;
        اكسر;
      افتراضي: /* ignore credentials from other sources */
        اكسر;
      }
    }

    إذا(!جاچ_netrc)
      اذهب ٻاهريون;

    ret = Curl_netrc_جاچ(معلومات, &معلومات->state.netrc,
                          معلومات->state.origin->hostname,
                          Curl_creds_user(ncreds_اندريون),
                          CURL_EASY_STR(معلومات, STRING_NETRC_FILE),
                          &ncreds_ٻاهريون);
    DEBUGASSERT(!ret || !ncreds_ٻاهريون);
    إذا(ret == NETRC_ٻاهريون_جو_يادگيري) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
    وإلا إذا(ret && ((ret == NETRC_NO_ميلاپ) ||
                    (معلومات->سيٽ.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(معلومات, "Could not find host %s in the %s file; using defaults",
            معلومات->state.origin->hostname,
            (CURL_EASY_STR(معلومات, STRING_NETRC_FILE) ?
             CURL_EASY_STR(معلومات, STRING_NETRC_FILE) : ".netrc"));
    }
    وإلا إذا(ret) {
      ثابت حرف *m = Curl_netrc_strerror(ret);
      failf(معلومات, ".netrc error: %s", m);
      نتيجو = CURLE_READ_ERROR;
      اذهب ٻاهريون;
    }
    وإلا إذا(ncreds_ٻاهريون) {
      إذا(!(معلومات->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_ٻاهريون->user) ||
          str_has_ctrl(ncreds_ٻاهريون->passwd))) {
        failf(معلومات, "control code detected in .netrc credentials");
        نتيجو = CURLE_READ_ERROR;
        اذهب ٻاهريون;
      }
      CURL_TRC_M(معلومات, "netrc: using credentials for %s as %s",
                 معلومات->state.origin->hostname, ncreds_ٻاهريون->user);
      نتيجو = Curl_creds_ملايو(ncreds_ٻاهريون->user, ncreds_ٻاهريون->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      إذا(نتيجو)
        اذهب ٻاهريون;
      /* for updated strings, we update them in the URL */
      uc = curl_url_سيٽ(معلومات->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      إذا(!uc)
        uc = curl_url_سيٽ(معلومات->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      إذا(uc)
        نتيجو = Curl_uc_منزل_curlcode(uc);
    }
    وإلا
      DEBUGASSERT(0);
  }

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  Curl_creds_trace(معلومات, معلومات->state.creds, "transfer credentials");
#جيڪڏهن_پورو

ٻاهريون:
  Curl_creds_unlink(&ncreds_ٻاهريون);
  أرجع نتيجو;
}
#جيڪڏهن_پورو /* CURL_DISABLE_NETRC */

ساكن CURLcode url_سيٽ_معلومات_creds(بنية Curl_easy *معلومات, CURLU *uh)
{
  بنية Curl_creds *newcreds = عدم;
  CURLcode نتيجو = CURLE_OK;

  إذا((CURL_EASY_STR(معلومات, STRING_USERNAME) ||
      CURL_EASY_STR(معلومات, STRING_PASSWORD) ||
      CURL_EASY_STR(معلومات, STRING_BEARER) ||
      CURL_EASY_STR(معلومات, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(معلومات, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_منزل_origin(معلومات, معلومات->state.origin)) {
    نتيجو = Curl_creds_create(CURL_EASY_STR(معلومات, STRING_USERNAME),
                               CURL_EASY_STR(معلومات, STRING_PASSWORD),
                               CURL_EASY_STR(معلومات, STRING_BEARER),
                               CURL_EASY_STR(معلومات, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(معلومات, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    إذا(نتيجو)
      اذهب ٻاهريون;
    إذا(newcreds &&
       !(معلومات->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(معلومات, "control code detected in credentials");
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
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
      نتيجو = Curl_uc_منزل_curlcode(uc);
    إذا(!نتيجو) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      إذا(uc && (uc != CURLUE_NO_PASSWORD))
        نتيجو = Curl_uc_منزل_curlcode(uc);
    }
    إذا(!نتيجو && user) {
      نتيجو = Curl_urldecode(user, 0, &udecoded, عدم,
                              (معلومات->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!نتيجو && passwd) {
      نتيجو = Curl_urldecode(passwd, 0, &pdecoded, عدم,
                              (معلومات->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    إذا(!نتيجو)
      نتيجو = Curl_creds_ملايو(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    إذا(نتيجو) {
      failf(معلومات, "error extracting credentials from URL");
      اذهب ٻاهريون;
    }
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  نتيجو = url_سيٽ_معلومات_creds_netrc(معلومات, &newcreds);
#جيڪڏهن_پورو /* CURL_DISABLE_NETRC */

ٻاهريون:
  إذا(!نتيجو && !Curl_creds_equal(معلومات->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&معلومات->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  أرجع نتيجو;
}

ساكن CURLcode url_سيٽ_conn_origin_etc(بنية Curl_easy *معلومات,
                                        بنية connectdata *conn)
{
  CURLcode نتيجو = CURLE_OK;

  Curl_peer_link(&conn->origin, معلومات->state.origin);

  /* set the connection scheme */
  نتيجو = url_سيٽ_conn_scheme(معلومات, conn);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* set the connection options */
  إذا(CURL_EASY_STR(معلومات, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(معلومات, STRING_OPTIONS));
    إذا(!conn->options) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }
  وإلا إذا(معلومات->state.مٿي.options) {
    conn->options = curlx_strdup(معلومات->state.مٿي.options);
    إذا(!conn->options) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب ٻاهريون;
    }
  }

ٻاهريون:
  أرجع نتيجو;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ساكن CURLcode تياري_range(بنية Curl_easy *معلومات)
{
  بنية UrlState *s = &معلومات->state;
  s->resume_from = معلومات->سيٽ.سيٽ_resume_from;
  إذا(s->resume_from || CURL_EASY_STR(معلومات, STRING_سيٽ_RANGE)) {
    إذا(s->rangestringalloc)
      curlx_free(s->range);

    إذا(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    وإلا
      s->range = curlx_strdup(CURL_EASY_STR(معلومات, STRING_سيٽ_RANGE));

    إذا(!s->range)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

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
ساكن CURLcode تياري_connection_internals(بنية Curl_easy *معلومات,
                                           بنية connectdata *conn)
{
  بنية Curl_peer *peer = عدم;

  إذا(conn->scheme->لاڳيتو->تياري_connection) {
    CURLcode نتيجو = conn->scheme->لاڳيتو->تياري_connection(معلومات, conn);
    إذا(نتيجو)
      أرجع نتيجو;
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
        peer->hostname, peer->scopeid, peer->پورٽ);
    وإلا
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->پورٽ);
  وإلا
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->پورٽ);
  إذا(!conn->destination)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  Curl_strntolower(conn->destination, conn->destination,
                   لڙي_ڊيگهه(conn->destination));

#جيڪڏهن_وصف_ٿيل USE_IPV6
  إذا(معلومات->سيٽ.scope_id)
    conn->scope_id = معلومات->سيٽ.scope_id;
  وإلا {
    peer = Curl_conn_get_پهريون_peer(conn, FIRSTSOCKET);
    إذا(!peer)
      أرجع CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#جيڪڏهن_پورو

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
  أرجع CURLE_ٻاهريون_جو_يادگيري;
}

/*
 * Set the login details so they are available in the connection
 */
ساكن CURLcode url_سيٽ_conn_login(بنية Curl_easy *معلومات,
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
                                        ثابت حرف *conn_منزل_سٽ,
                                        بنية Curl_peer **pvia_dest)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت حرف *ptr = conn_منزل_سٽ;
  bool host_ميلاپ = FALSE;
  bool پورٽ_ميلاپ = FALSE;

  *pvia_dest = عدم;

  إذا(*ptr == ':') {
    /* an empty hostname always matches */
    host_ميلاپ = TRUE;
    ptr++;
  }
  وإلا {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = لڙي_ڊيگهه(dest->hostname);
    host_ميلاپ = curl_strnequal(ptr, dest->hostname, hlen);
    إذا(!host_ميلاپ && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = لڙي_ڊيگهه(dest->user_hostname);
      host_ميلاپ = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_ميلاپ = host_ميلاپ && ptr[hlen] == ':';
    إذا(host_ميلاپ)
      ptr += hlen + 1;
  }

  إذا(host_ميلاپ) {
    إذا(*ptr == ':') {
      /* an empty port always matches */
      پورٽ_ميلاپ = TRUE;
      ptr++;
    }
    وإلا {
      /* check whether the URL's port matches */
      ثابت حرف *ptr_ايندڙ = strchr(ptr, ':');
      إذا(ptr_ايندڙ) {
        curl_off_t پورٽ_منزل_ميلاپ;
        إذا(!curlx_str_عدد(&ptr, &پورٽ_منزل_ميلاپ, 0xffff) &&
           ((uint16_t)پورٽ_منزل_ميلاپ == dest->پورٽ)) {
          پورٽ_ميلاپ = TRUE;
        }
        ptr = ptr_ايندڙ + 1;
      }
    }
  }

  إذا(host_ميلاپ && پورٽ_ميلاپ && ptr && *ptr)
    نتيجو = Curl_peer_from_connect_منزل(معلومات, dest, ptr, pvia_dest);

  أرجع نتيجو;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ساكن CURLcode url_سيٽ_conn_peer(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn)
{
  CURLcode نتيجو = CURLE_OK;
  بنية Curl_peer *origin = conn->origin;
  بنية Curl_peer *ذريعي_peer = عدم;
  بنية curl_slist *conn_منزل_داخلا = معلومات->سيٽ.connect_منزل;

  DEBUGASSERT(!conn->ذريعي_peer);
  Curl_peer_unlink(&conn->ذريعي_peer);

  طالما(conn_منزل_داخلا && !ذريعي_peer) {
    نتيجو = parse_connect_منزل_string(معلومات, origin, conn_منزل_داخلا->معلومات,
                                     &ذريعي_peer);
    إذا(نتيجو)
      أرجع نتيجو;
    conn_منزل_داخلا = conn_منزل_داخلا->ايندڙ;
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  إذا(معلومات->asi && !ذريعي_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#وإلا
      0
#جيڪڏهن_پورو
       )) {
    /* no connect_to match, try alt-svc! */
    تعداد alpnid srcalpnid = ALPN_none;
    bool ڪاميابي = FALSE;
    بنية altsvc *as = عدم;
    صحيح allowed_alpns = ALPN_none;
    بنية http_negotiation *neg = &معلومات->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(معلومات, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#جيڪڏهن_وصف_ٿيل USE_HTTP3
    إذا(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTP2
    إذا(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#جيڪڏهن_پورو
    إذا(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (صحيح)معلومات->asi->flags;

    DEBUGF(infof(معلومات, "check Alt-Svc for host '%s'", origin->hostname));
#جيڪڏهن_وصف_ٿيل USE_HTTP3
    إذا(!ڪاميابي && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      ڪاميابي = Curl_altsvc_lookup(معلومات->asi,
                               origin, ALPN_h3, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTP2
    إذا(!ڪاميابي && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_اڳواٽ_knowledge) {
      srcalpnid = ALPN_h2;
      ڪاميابي = Curl_altsvc_lookup(معلومات->asi,
                               origin, ALPN_h2, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }
#جيڪڏهن_پورو
    إذا(!ڪاميابي && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      ڪاميابي = Curl_altsvc_lookup(معلومات->asi,
                               origin, ALPN_h1, /* from */
                               &as /*منزل*/,
                               allowed_alpns, &same_dest);
    }

    إذا(ڪاميابي && same_dest) {
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
    وإلا إذا(ڪاميابي) {
      نتيجو = Curl_peer_create(معلومات, conn->origin->scheme,
                                as->dst.host, as->dst.پورٽ,
                                &ذريعي_peer);
      إذا(نتيجو)
        أرجع نتيجو;
      infof(معلومات, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->پورٽ,
            Curl_alpnid2str(as->dst.alpnid),
            ذريعي_peer->hostname, ذريعي_peer->پورٽ);
      conn->بٽ.altused = TRUE;
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
#جيڪڏهن_پورو

  إذا(ذريعي_peer)
    conn->ذريعي_peer = ذريعي_peer;

  أرجع نتيجو;
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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#جيڪڏهن_پورو

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
  Curl_peer_link(&conn->ذريعي_peer, needle->ذريعي_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->ذريعي_peer2, needle->ذريعي_peer2);
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
  CURLcode نتيجو = CURLE_OK;
  bool ڄار_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(معلومات);
  إذا(!needle) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_بائيٽون,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  نتيجو = url_سيٽ_conn_origin_etc(معلومات, needle);
  إذا(نتيجو)
    اذهب ٻاهريون;

  DEBUGASSERT(needle->origin);
  ڄار_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  إذا(ڄار_scheme && CURL_EASY_STR(معلومات, STRING_UNIX_SOCKET_رستو)) {
    نتيجو = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(معلومات, STRING_UNIX_SOCKET_رستو),
      (bool)معلومات->سيٽ.abstract_unix_socket, &needle->ذريعي_peer);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
#جيڪڏهن_پورو /* USE_UNIX_SOCKETS */

  إذا(ڄار_scheme && !needle->ذريعي_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    نتيجو = url_سيٽ_conn_peer(معلومات, needle);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  إذا(Curl_peer_equal(needle->origin, needle->ذريعي_peer)) {
    Curl_peer_unlink(&needle->ذريعي_peer);
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  إذا(ڄار_scheme &&
     (!needle->ذريعي_peer || !needle->ذريعي_peer->unix_socket)) {
    نتيجو = Curl_proxy_init_conn(معلومات, needle);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
#جيڪڏهن_پورو /* CURL_DISABLE_PROXY */

  نتيجو = url_سيٽ_conn_login(معلومات, needle); /* default credentials */
  إذا(نتيجو)
    اذهب ٻاهريون;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  نتيجو = تياري_connection_internals(معلومات, needle);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    إذا(معلومات->سيٽ.ssl_enable_alpn)
      needle->بٽ.tls_enable_alpn = TRUE;
  }

  إذا(ڄار_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->بٽ.tcp_fastopen = معلومات->سيٽ.tcp_fastopen;
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
    إذا(Curl_conn_get_پهريون_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#جيڪڏهن_پورو
  }

ٻاهريون:
  إذا(!نتيجو) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  وإلا {
    *pneedle = عدم;
    إذا(needle)
      Curl_conn_free(معلومات, needle);
  }
  أرجع نتيجو;
}

ساكن CURLcode url_سيٽ_معلومات_origin_and_creds(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_سيٽ_uh = (معلومات->سيٽ.uh && !معلومات->state.this_is_a_follow);
  uint16_t پورٽ_override = معلومات->state.allow_پورٽ ? معلومات->سيٽ.use_پورٽ : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  إذا(!Curl_bufref_ptr(&معلومات->state.url)) {
    نتيجو = CURLE_URL_MALFORMAT;
    اذهب ٻاهريون;
  }

  مٿي_free(معلومات); /* cleanup previous leftovers first */

  /* parse the URL */
  إذا(use_سيٽ_uh)
    uh = معلومات->state.uh = curl_url_dup(معلومات->سيٽ.uh);
  وإلا
    uh = معلومات->state.uh = curl_url();
  إذا(!uh) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  إذا(!use_سيٽ_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    إذا(curl_strequal("file", CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&معلومات->state.url), عدم, 0, TRUE))
      prepend_sheme = TRUE;
    وإلا {
      uc = curl_url_سيٽ(uh, CURLUPART_URL, Curl_bufref_ptr(&معلومات->state.url),
                        (بنانشان صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (معلومات->سيٽ.disallow_username_اندريون_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (معلومات->سيٽ.رستو_as_is ? CURLU_رستو_AS_IS : 0));
      إذا(uc) {
        failf(معلومات, "URL rejected: %s", curl_url_strerror(uc));
        نتيجو = Curl_uc_منزل_curlcode(uc);
        اذهب ٻاهريون;
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
          نتيجو = Curl_uc_منزل_curlcode(uc);
          اذهب ٻاهريون;
        }
      }
    }

    إذا(prepend_sheme) {
      حرف *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&معلومات->state.url));
      إذا(!url) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
      uc = curl_url_سيٽ(uh, CURLUPART_URL, url,
                        (بنانشان صحيح)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (معلومات->سيٽ.disallow_username_اندريون_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (معلومات->سيٽ.رستو_as_is ? CURLU_رستو_AS_IS : 0));
      curl_free(url);
      إذا(uc) {
        failf(معلومات, "URL rejected: %s", curl_url_strerror(uc));
        نتيجو = Curl_uc_منزل_curlcode(uc);
        اذهب ٻاهريون;
      }
    }
    {
      حرف *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_خالي_جايون);
      إذا(uc) {
        نتيجو = Curl_uc_منزل_curlcode(uc);
        اذهب ٻاهريون;
      }
      إذا(prepend_sheme)
        infof(معلومات, "Applied default protocol '%s'",
              CURL_EASY_STR(معلومات, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_سيٽ(&معلومات->state.url, newurl, 0, curl_free);
    }
  }

#جيڪڏهن_وصف_ٿيل USE_IPV6
  scope_id = معلومات->سيٽ.scope_id;
#جيڪڏهن_پورو

  /* `uh` is now as the connection should use it, probably. */
  نتيجو = Curl_peer_from_url(uh, معلومات, پورٽ_override, scope_id,
                              &معلومات->state.origin);
  إذا(نتيجو)
    اذهب ٻاهريون;
  /* The origin might get changed when HSTS applies */
  نتيجو = hsts_upgrade(معلومات, uh, پورٽ_override, scope_id);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  إذا(!معلومات->state.initial_origin)
    Curl_peer_link(&معلومات->state.initial_origin, معلومات->state.origin);

  uc = curl_url_get(uh, CURLUPART_رستو, &معلومات->state.مٿي.رستو, CURLU_URLENCODE);
  إذا(uc) {
    نتيجو = Curl_uc_منزل_curlcode(uc);
    اذهب ٻاهريون;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &معلومات->state.مٿي.query,
                    CURLU_GET_خالي_جايون);
  إذا(uc && (uc != CURLUE_NO_QUERY)) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &معلومات->state.مٿي.options,
                    CURLU_URLDECODE);
  إذا(uc && (uc != CURLUE_NO_OPTIONS)) {
    نتيجو = Curl_uc_منزل_curlcode(uc);
    اذهب ٻاهريون;
  }

  نتيجو = url_سيٽ_معلومات_creds(معلومات, uh);
  إذا(نتيجو)
    اذهب ٻاهريون;

ٻاهريون:
  أرجع نتيجو;
}

ساكن CURLcode url_ميلاپ_init(بنية Curl_easy *معلومات,
                               بنية connectdata *needle,
                               بنية url_conn_ميلاپ *m)
{
  memset(m, 0, حجم(*m));
  m->معلومات = معلومات;
  m->needle = needle;
  m->now = *Curl_pgrs_now(معلومات);
  m->may_multiplex = xfer_may_multiplex(معلومات, needle);

#جيڪڏهن_وصف_ٿيل USE_NTLM
  m->want_ntlm_http =
    (معلومات->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_منزل_host(معلومات);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (معلومات->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (معلومات->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_منزل_host(معلومات);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (معلومات->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#جيڪڏهن_پورو
#جيڪڏهن_پورو
  m->require_tls = معلومات->سيٽ.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = معلومات->سيٽ.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  أرجع Curl_ssl_filter_config_tmp_init(معلومات, needle->origin,
                                         &m->ssl_config,
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#وإلا
                                         عدم);
#جيڪڏهن_پورو
}

ساكن فراغ url_ميلاپ_destroy(بنية url_conn_ميلاپ *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#جيڪڏهن_پورو
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ساكن CURLcode url_ڳوليو_or_create_conn(بنية Curl_easy *معلومات,
                                        ثابت بنية curltime *pnow)
{
  بنية connectdata *needle = عدم;
  بنية url_conn_ميلاپ ميلاپ;
  bool ميلاپ_initialized = FALSE;
  CURLcode نتيجو;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  نتيجو = url_create_needle(معلومات, &needle);
  إذا(نتيجو)
    اذهب ٻاهريون;
  DEBUGASSERT(needle);

  نتيجو = url_ميلاپ_init(معلومات, needle, &ميلاپ);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ميلاپ_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FILE
  إذا(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool پورو;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->لاڳيتو->connect_it);
    معلومات->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    معلومات->info.conn_protocol = needle->scheme->protocol & CURLPROTO_نقاب;
    نتيجو = needle->scheme->لاڳيتو->connect_it(معلومات, &پورو);
    إذا(نتيجو)
      اذهب ٻاهريون;

    /* Setup a "faked" transfer that will do nothing */
    نتيجو = Curl_cpool_جوڙيو(معلومات, needle, 0, 0, pnow);
    Curl_attach_connection(معلومات, needle, TRUE);
    needle = عدم;
    إذا(!نتيجو) {
      /* Setup whatever necessary for a resumed transfer */
      نتيجو = تياري_range(معلومات);
      إذا(!نتيجو) {
        Curl_xfer_تياري_nop(معلومات);
        نتيجو = Curl_init_transfer(معلومات, معلومات->conn);
      }
    }

    إذا(نتيجو) {
      DEBUGASSERT(معلومات->conn->scheme->لاڳيتو->پورو);
      /* we ignore the return code for the protocol-specific DONE */
      (فراغ)معلومات->conn->scheme->لاڳيتو->پورو(معلومات, نتيجو, FALSE);
    }
    اذهب ٻاهريون;
  }
#جيڪڏهن_پورو

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  إذا((!معلومات->سيٽ.reuse_fresh || معلومات->state.followlocation) &&
     !معلومات->سيٽ.connect_only) {
    /* Ok, try to find and attach an existing one */
    إذا(url_attach_existing(معلومات, needle, &ميلاپ, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      بنية connectdata *conn = معلومات->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->بٽ.reuse = TRUE;
      url_conn_reuse_adjust(معلومات, needle);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
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
#جيڪڏهن_پورو
    }
  }

  إذا(!معلومات->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    إذا(ميلاپ.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(معلومات, "Waiting on connection to negotiate possible multiplexing.");
      نتيجو = CURLE_NO_CONNECTION_موجود;
      اذهب ٻاهريون;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    نتيجو = Curl_cpool_جوڙيو(معلومات, needle,
                            معلومات->multi->max_ڪل_connections,
                            معلومات->multi->max_host_connections,
                            pnow);
    إذا(نتيجو)
      اذهب ٻاهريون;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(معلومات, needle, TRUE);
    needle = عدم;

    /* Clone the SSL filter configs into the new connection. */
    نتيجو = Curl_ssl_conn_config_clone(&ميلاپ.ssl_config,
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
                                        &ميلاپ.proxy_ssl_config,
#وإلا
                                        عدم,
#جيڪڏهن_پورو
                                        معلومات->conn);
    إذا(نتيجو) {
      DEBUGF(infof(معلومات, "Error: clone connection SSL config"));
      اذهب ٻاهريون;
    }

#جيڪڏهن_وصف_ٿيل USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    إذا((معلومات->state.authhost.picked & CURLAUTH_NTLM) &&
       معلومات->state.authhost.پورو) {
      infof(معلومات, "NTLM picked AND auth done set, clear picked");
      معلومات->state.authhost.picked = CURLAUTH_NONE;
      معلومات->state.authhost.پورو = FALSE;
    }

    إذا((معلومات->state.authproxy.picked & CURLAUTH_NTLM) &&
       معلومات->state.authproxy.پورو) {
      infof(معلومات, "NTLM-proxy picked AND auth done set, clear picked");
      معلومات->state.authproxy.picked = CURLAUTH_NONE;
      معلومات->state.authproxy.پورو = FALSE;
    }
#جيڪڏهن_پورو
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  نتيجو = Curl_init_transfer(معلومات, معلومات->conn);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* Setup whatever necessary for a resumed transfer */
  نتيجو = تياري_range(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* persist the scheme and handler the transfer is using */
  معلومات->info.conn_scheme = معلومات->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  معلومات->info.conn_protocol = معلومات->conn->scheme->protocol & CURLPROTO_نقاب;
  معلومات->info.استعمال_ٿيل_proxy =
#جيڪڏهن_وصف_ٿيل CURL_DISABLE_PROXY
    0
#وإلا
    (معلومات->conn->socks_proxy.peer || معلومات->conn->http_proxy.peer)
#جيڪڏهن_پورو
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  نتيجو = Curl_conn_ev_معلومات_تياري(معلومات);

ٻاهريون:
  إذا(ميلاپ_initialized)
    url_ميلاپ_destroy(&ميلاپ);
  إذا(needle)
    Curl_conn_free(معلومات, needle);
  DEBUGASSERT(نتيجو || معلومات->conn);
  أرجع نتيجو;
}

CURLcode Curl_connect(بنية Curl_easy *معلومات, bool *pconnected)
{
  CURLcode نتيجو;
  بنية connectdata *conn = عدم;
  ثابت بنية curltime *pnow = عدم;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&معلومات->req, معلومات);
  /* Determine the origin of the transfer and what credentials to use */
  نتيجو = url_سيٽ_معلومات_origin_and_creds(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;
  إذا(!معلومات->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    نتيجو = CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(معلومات);
  Curl_pgrs_وقت_Was(معلومات, TIMER_POSTQUEUE, *pnow);
  نتيجو = url_ڳوليو_or_create_conn(معلومات, pnow);
  conn = معلومات->conn;
  إذا(نتيجو)
    اذهب ٻاهريون;
  إذا(!معلومات->conn) { /* just make really sure */
    DEBUGASSERT(0);
    نتيجو = CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }

  إذا(conn->بٽ.reuse) {
    إذا(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  وإلا إذا(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_وقت(معلومات, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  وإلا {
    نتيجو = Curl_conn_تياري(معلومات, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    إذا(!نتيجو)
      نتيجو = Curl_سرا_init(معلومات);
    CURL_TRC_M(معلومات, "Curl_conn_setup() -> %d", (صحيح)نتيجو);
  }

ٻاهريون:
  إذا(نتيجو == CURLE_NO_CONNECTION_موجود)
    DEBUGASSERT(!conn);

  إذا(نتيجو && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(معلومات);
    Curl_conn_close(معلومات, conn, TRUE);
  }

  أرجع نتيجو;
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
  CURLcode نتيجو;

  إذا(conn) {
    conn->بٽ.do_وڌيڪ = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    إذا(معلومات->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      معلومات->state.wildcardmatch = FALSE;
  }

  معلومات->state.پورو = FALSE; /* *_done() is not called yet */

  معلومات->req.no_body = معلومات->سيٽ.opt_no_body;
  إذا(معلومات->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    معلومات->state.httpreq = HTTPREQ_سر;

  نتيجو = Curl_req_شروعات(&معلومات->req, معلومات);
  إذا(!نتيجو) {
    Curl_pgrsReset(معلومات);
  }
  أرجع نتيجو;
}

#إذا defined(USE_HTTP2) || defined(USE_HTTP3)

فراغ Curl_معلومات_ترجيح_clear_state(بنية Curl_easy *معلومات)
{
  معلومات->state.وزن = 0;
}

#جيڪڏهن_پورو /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_سيٽ(بنية connectdata *conn, ثابت حرف *ڪنجي,
                            فراغ *meta_معلومات, Curl_meta_dtor *meta_dtor)
{
  إذا(!Curl_hash_add2(&conn->meta_hash, ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1,
                     meta_معلومات, meta_dtor)) {
    meta_dtor(ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1, meta_معلومات);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  أرجع CURLE_OK;
}

فراغ Curl_conn_meta_remove(بنية connectdata *conn, ثابت حرف *ڪنجي)
{
  Curl_hash_delete(&conn->meta_hash, ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1);
}

فراغ *Curl_conn_meta_get(بنية connectdata *conn, ثابت حرف *ڪنجي)
{
  أرجع Curl_hash_pick(&conn->meta_hash, ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1);
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
    admin->سيٽ.conn_max_idle_ms = معلومات->سيٽ.conn_max_idle_ms;
    admin->سيٽ.conn_max_age_ms = معلومات->سيٽ.conn_max_age_ms;
    admin->سيٽ.upkeep_interval_ms = معلومات->سيٽ.upkeep_interval_ms;
    admin->سيٽ.timeout = معلومات->سيٽ.timeout;
    admin->سيٽ.server_response_timeout = معلومات->سيٽ.server_response_timeout;
    admin->سيٽ.no_signal = معلومات->سيٽ.no_signal;
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
