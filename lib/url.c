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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_IPHLPAPI_H
#అన్తర్భావ <Iphlpapi.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_UN_H
#అన్తర్భావ <sys/un.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_SOCKET
#దోషం "We cannot compile without socket() support"
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ఒకవేళ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#అన్తర్భావ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#అయితే_ముగింపు
#అన్తర్భావ <iphlpapi.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "mime.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vssh/vssh.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "cookie.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "http_digest.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "getinfo.h"
#అన్తర్భావ "pop3.h"
#అన్తర్భావ "urlapi-int.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "proxy.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "idn.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "conncache.h"
#అన్తర్భావ "multihandle.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "setopt.h"
#అన్తర్భావ "altsvc.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "headers.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "peer.h"

/* Now for the protocols */
#అన్తర్భావ "ftp.h"
#అన్తర్భావ "dict.h"
#అన్తర్భావ "telnet.h"
#అన్తర్భావ "tftp.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "file.h"
#అన్తర్భావ "curl_ldap.h"
#అన్తర్భావ "vssh/ssh.h"
#అన్తర్భావ "imap.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "gopher.h"
#అన్తర్భావ "mqtt.h"
#అన్తర్భావ "rtsp.h"
#అన్తర్భావ "smtp.h"
#అన్తర్భావ "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#ఒకవేళ READBUFFER_కొలత < READBUFFER_MIN
# error READBUFFER_కొలత is too small
#అయితే_ముగింపు

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
స్థిర curl_prot_t get_protocol_family(స్థిరరాశి నిర్మాణం Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  తిరిగి s->family;
}

శూన్య Curl_freeset(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  గణన dupblob j;

  CURL_EASY_STR_CLEAR0(దత్తాంశం, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(దత్తాంశం, STRING_కీలకం_PASSWD);
  CURL_EASY_STR_CLEAR0(దత్తాంశం, STRING_BEARER);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(దత్తాంశం, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(దత్తాంశం, STRING_కీలకం_PASSWD_PROXY);
#అయితే_ముగింపు
  Curl_u8_strset_clear(&దత్తాంశం->సమితి.అక్షరశ్రేణులు);
  curlx_safefree(దత్తాంశం->సమితి.str_copypostfields);

  కొరకు(j = (గణన dupblob)0; j < BLOB_చివరి; j++) {
    curlx_safefree(దత్తాంశం->సమితి.blobs[j]);
  }

  Curl_bufref_free(&దత్తాంశం->state.referer);
  Curl_bufref_free(&దత్తాంశం->state.url);

#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(దత్తాంశం->సమితి.mimepostp);
  curlx_safefree(దత్తాంశం->సమితి.mimepostp);
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  curl_slist_free_all(దత్తాంశం->state.cookielist);
  దత్తాంశం->state.cookielist = ఖాళీ;
#అయితే_ముగింపు
}

/* free the URL pieces */
స్థిర శూన్య పైకి_free(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం urlpieces *పైకి = &దత్తాంశం->state.పైకి;
  curlx_safefree(పైకి->options);
  curlx_safefree(పైకి->మార్గం);
  curlx_safefree(పైకి->query);
  curl_url_cleanup(దత్తాంశం->state.uh);
  దత్తాంశం->state.uh = ఖాళీ;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(నిర్మాణం Curl_easy **datap)
{
  నిర్మాణం Curl_easy *దత్తాంశం;

  ఒకవేళ(!datap || !*datap)
    తిరిగి CURLE_OK;

  దత్తాంశం = *datap;
  *datap = ఖాళీ;

  ఒకవేళ(!దత్తాంశం->state.అంతర్గత && దత్తాంశం->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(దత్తాంశం->multi, దత్తాంశం);
  }
  లేకపోతే {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(దత్తాంశం);
    ఒకవేళ(!దత్తాంశం->state.అంతర్గత && దత్తాంశం->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(దత్తాంశం->multi_easy);
      దత్తాంశం->multi_easy = ఖాళీ;
    }
  }
  DEBUGASSERT(!దత్తాంశం->conn || దత్తాంశం->state.అంతర్గత);

  Curl_expire_clear_all(దత్తాంశం); /* shut off any timers left */

  ఒకవేళ(దత్తాంశం->state.rangestringalloc)
    curlx_free(దత్తాంశం->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(దత్తాంశం);

  దత్తాంశం->సమితి.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  దత్తాంశం->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&దత్తాంశం->req, దత్తాంశం);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(దత్తాంశం);
  Curl_peer_unlink(&దత్తాంశం->state.origin);
  Curl_peer_unlink(&దత్తాంశం->state.initial_origin);
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  Curl_peer_unlink(&దత్తాంశం->state.rtsp_session_origin);
#అయితే_ముగింపు
  Curl_ssl_free_certinfo(దత్తాంశం);

  Curl_bufref_free(&దత్తాంశం->state.referer);

  పైకి_free(దత్తాంశం);
  curlx_dyn_free(&దత్తాంశం->state.headerb);
  Curl_flush_cookies(దత్తాంశం, TRUE);
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  Curl_altsvc_save(దత్తాంశం, దత్తాంశం->asi, CURL_EASY_STR(దత్తాంశం, STRING_ALTSVC));
  Curl_altsvc_cleanup(&దత్తాంశం->asi);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  Curl_hsts_save(దత్తాంశం, దత్తాంశం->hsts, CURL_EASY_STR(దత్తాంశం, STRING_HSTS));
  ఒకవేళ(!దత్తాంశం->share || !దత్తాంశం->share->hsts)
    Curl_hsts_cleanup(&దత్తాంశం->hsts);
  curl_slist_free_all(దత్తాంశం->state.hstslist); /* clean up list */
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(దత్తాంశం);
#అయితే_ముగింపు
  curlx_safefree(దత్తాంశం->state.most_recent_ftp_entrypath);
  curlx_safefree(దత్తాంశం->info.contenttype);
  curlx_safefree(దత్తాంశం->info.wouldredirect);
  curlx_safefree(దత్తాంశం->info.effective_url);

  /* No longer a dirty share, if it exists */
  ఒకవేళ(Curl_share_easy_unlink(దత్తాంశం))
    DEBUGASSERT(0);

  Curl_hash_destroy(&దత్తాంశం->meta_hash);
  Curl_creds_unlink(&దత్తాంశం->state.creds);
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  curlx_safefree(దత్తాంశం->state.rangeline);
  curlx_safefree(దత్తాంశం->state.http_host);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
  curlx_safefree(దత్తాంశం->req.cookiehost);
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(దత్తాంశం->state.formp);
  curlx_safefree(దత్తాంశం->state.formp);
#అయితే_ముగింపు

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&దత్తాంశం->wildcard);
  Curl_freeset(దత్తాంశం);
  Curl_శీర్షికలు_cleanup(దత్తాంశం);
  Curl_netrc_cleanup(&దత్తాంశం->state.netrc);
#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
  curlx_free(దత్తాంశం->state.envproxy);
#అయితే_ముగింపు
  curlx_memzero(దత్తాంశం, పరిమాణం(*దత్తాంశం));
  curlx_free(దత్తాంశం);
  తిరిగి CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
శూన్య Curl_init_userdefined(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం UserDefined *సమితి = &దత్తాంశం->సమితి;

  సమితి->బహిర్గతం = stdout;  /* default output to stdout */
  సమితి->లోపల_సమితి = stdin;  /* default input from stdin */
  సమితి->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&దత్తాంశం->సమితి.అక్షరశ్రేణులు);

#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
  /* use fwrite as default function to store output */
  సమితి->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  సమితి->fread_func_సమితి = (curl_read_callback)fread;
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
  సమితి->is_fread_సమితి = 0;

  సమితి->seek_client = ZERO_NULL;

  సమితి->filesize = -1;        /* we do not know the size */
  సమితి->postfieldsize = -1;   /* unknown size */
  సమితి->maxredirs = 30;       /* sensible default */

  సమితి->పద్ధతి = HTTPREQ_GET; /* Default HTTP request */
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  సమితి->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  సమితి->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  సమితి->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  సమితి->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  సమితి->ftp_filemethod = FTPFILE_MULTICWD;
  సమితి->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#అయితే_ముగింపు
  సమితి->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  సమితి->ssl_ca_cache_timeout = 24 * 60 * 60;

  సమితి->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&దత్తాంశం->సమితి.ssl);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  Curl_ssl_config_init(&దత్తాంశం->సమితి.proxy_ssl);
  సమితి->proxyport = 0;
  సమితి->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  సమితి->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  సమితి->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  సమితి->doh_verifyhost = TRUE;
  సమితి->doh_verifypeer = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  /* defaults to any auth type */
  సమితి->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  సమితి->new_directory_perms = 0755; /* Default permissions */
#అయితే_ముగింపు

  సమితి->new_file_perms = 0644;    /* Default permissions */
  సమితి->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  సమితి->redir_protocols = CURLPROTO_REDIR;

#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  సమితి->socks5_gssapi_nec = FALSE;
#అయితే_ముగింపు

  /* set default minimum TLS version */
#నిర్వచితమైతే USE_SSL
  Curl_setopt_SSLVERSION(దత్తాంశం, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(దత్తాంశం, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#అయితే_ముగింపు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  సమితి->wildcard_enabled = FALSE;
  సమితి->chunk_bgn = ZERO_NULL;
  సమితి->chunk_చివర = ZERO_NULL;
  సమితి->fnmatch = ZERO_NULL;
#అయితే_ముగింపు
  సమితి->tcp_keepalive = FALSE;
  సమితి->tcp_keepintvl = 60;
  సమితి->tcp_keepidle = 60;
  సమితి->tcp_keepcnt = 9;
  సమితి->tcp_fastopen = FALSE;
  సమితి->tcp_nodelay = TRUE;
  సమితి->ssl_enable_alpn = TRUE;
  సమితి->expect_100_timeout = 1000L; /* Wait for a second by default. */
  సమితి->sep_శీర్షికలు = TRUE; /* separated header lists by default */
  సమితి->బఫరు_కొలత = READBUFFER_కొలత;
  సమితి->upload_బఫరు_కొలత = UPLOADBUFFER_DEFAULT;
  సమితి->upload_flags = CURLULFLAG_చూసిన;
  సమితి->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  సమితి->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  సమితి->maxconnects = DEFAULT_CONNCACHE_కొలత; /* for easy handles */
  సమితి->conn_max_idle_ms = 118 * 1000;
  సమితి->conn_max_age_ms = 24 * 3600 * 1000;
  సమితి->http09_allowed = FALSE;
  సమితి->httpwant = CURL_HTTP_సంస్కరణ_NONE;
#ఒకవేళ defined(USE_HTTP2) || defined(USE_HTTP3)
  సమితి->బరువు = 0;
#అయితే_ముగింపు
  సమితి->quick_exit = 0L;
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  సమితి->ws_raw_బహుళకం = FALSE;
  సమితి->ws_no_auto_pong = FALSE;
#అయితే_ముగింపు
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
స్థిర శూన్య easy_meta_freeentry(శూన్య *p)
{
  (శూన్య)p;
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
CURLcode Curl_open(నిర్మాణం Curl_easy **curl)
{
  నిర్మాణం Curl_easy *దత్తాంశం;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  దత్తాంశం = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_easy));
  ఒకవేళ(!దత్తాంశం) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  దత్తాంశం->magic = CURLEASY_MAGIC_సంఖ్య;
  /* most recent connection is not yet defined */
  దత్తాంశం->state.చివరి_conn_id = -1;
  దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  దత్తాంశం->id = -1;
  దత్తాంశం->mid = UINT32_MAX;
  దత్తాంశం->master_mid = UINT32_MAX;
  దత్తాంశం->పురోగతి.hide = TRUE;

  Curl_hash_init(&దత్తాంశం->meta_hash, 23, CURL_HASH_TYPE_బైట్లు,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_చివరి <= UINT8_MAX);
  Curl_u8_strset_init(&దత్తాంశం->సమితి.అక్షరశ్రేణులు);
  curlx_dyn_init(&దత్తాంశం->state.headerb, CURL_MAX_HTTP_శీర్షిక);
  Curl_bufref_init(&దత్తాంశం->state.url);
  Curl_bufref_init(&దత్తాంశం->state.referer);
  Curl_req_init(&దత్తాంశం->req);
  Curl_initinfo(దత్తాంశం);
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  Curl_llist_init(&దత్తాంశం->state.httphdrs, ఖాళీ);
#అయితే_ముగింపు
  Curl_netrc_init(&దత్తాంశం->state.netrc);
  Curl_init_userdefined(దత్తాంశం);

  *curl = దత్తాంశం;
  తిరిగి CURLE_OK;
}

శూన్య Curl_conn_free(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  ఒకవేళ(conn->scheme && conn->scheme->నిరంతరం->disconnect &&
     !conn->బిట్లు.shutdown_handler)
    conn->scheme->నిరంతరం->disconnect(దత్తాంశం, conn, TRUE);

  కొరకు(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(దత్తాంశం, conn, i);
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#అయితే_ముగింపు
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->ద్వారా_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->ద్వారా_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
స్థిర bool xfer_may_multiplex(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి నిర్మాణం connectdata *conn)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  ఒకవేళ((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->బిట్లు.protoconnstart || !conn->బిట్లు.close)) {

    ఒకవేళ(Curl_multiplex_wanted(దత్తాంశం->multi) &&
       (దత్తాంశం->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      తిరిగి TRUE;
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
#అయితే_ముగింపు
  తిరిగి FALSE;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
స్థిర bool proxy_info_matches(స్థిరరాశి నిర్మాణం proxy_info *దత్తాంశం,
                               స్థిరరాశి నిర్మాణం proxy_info *needle)
{
  ఒకవేళ((దత్తాంశం->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(దత్తాంశం->peer, needle->peer) &&
     Curl_creds_same(దత్తాంశం->creds, needle->creds)) {
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SSH
స్థిర bool ssh_config_matches(నిర్మాణం connectdata *one,
                               నిర్మాణం connectdata *two)
{
  నిర్మాణం ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  తిరిగి sshc1 && sshc2 && Curl_safecmp(sshc1->priv_కీలకం, sshc2->priv_కీలకం) &&
         Curl_safecmp(sshc1->pub_కీలకం, sshc2->pub_కీలకం);
}
#అయితే_ముగింపు

నిర్మాణం url_conn_సరిపోలిక {
  నిర్మాణం connectdata *దొరికింది;
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం connectdata *needle;
  నిర్మాణం curltime now;
  నిర్మాణం ssl_filter_config ssl_config;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  నిర్మాణం ssl_filter_config proxy_ssl_config;
#అయితే_ముగింపు
  బిట్టు(may_multiplex);
  బిట్టు(want_ntlm_http);
  బిట్టు(want_proxy_ntlm_http);
  బిట్టు(want_nego_http);
  బిట్టు(want_proxy_nego_http);
  బిట్టు(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  బిట్టు(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  బిట్టు(wait_pipe);
  బిట్టు(చూసిన_pending_conn);
  బిట్టు(చూసిన_single_use_conn);
  బిట్టు(చూసిన_multiplex_conn);
};

స్థిర bool url_సరిపోలిక_connect_config(నిర్మాణం connectdata *conn,
                                     నిర్మాణం url_conn_సరిపోలిక *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  ఒకవేళ(conn->బిట్లు.connect_only || conn->బిట్లు.close || conn->బిట్లు.no_reuse)
    తిరిగి FALSE;

  /* ip_version must match */
  ఒకవేళ(m->దత్తాంశం->సమితి.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->దత్తాంశం->సమితి.ipver != conn->ip_సంస్కరణ)
    తిరిగి FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  ఒకవేళ(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    తిరిగి FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  ఒకవేళ((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    తిరిగి FALSE;

  ఒకవేళ((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    తిరిగి FALSE;

  ఒకవేళ(!m->needle->ద్వారా_peer != !conn->ద్వారా_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    తిరిగి FALSE;

  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_fully_connected(నిర్మాణం connectdata *conn,
                                      నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->బిట్లు.upgrade_లోపల_పురోగతి) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    ఒకవేళ(m->may_multiplex) {
      m->చూసిన_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->దత్తాంశం, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_multi(నిర్మాణం connectdata *conn,
                            నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    ఒకవేళ(conn->attached_multi != m->దత్తాంశం->multi)
      తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_multiplex_needs(నిర్మాణం connectdata *conn,
                                      నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(CONN_INUSE(conn)) {
    ఒకవేళ(!conn->బిట్లు.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->చూసిన_single_use_conn = TRUE;
      తిరిగి FALSE;
    }
    m->చూసిన_multiplex_conn = TRUE;
    ఒకవేళ(!m->may_multiplex || !url_సరిపోలిక_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_multiplex_limits(నిర్మాణం connectdata *conn,
                                       నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->బిట్లు.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    ఒకవేళ(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->దత్తాంశం->multi)) {
      infof(m->దత్తాంశం, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      తిరిగి FALSE;
    }
    ఒకవేళ(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->దత్తాంశం, conn, FIRSTSOCKET)) {
      infof(m->దత్తాంశం, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      తిరిగి FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->దత్తాంశం, "Multiplexed connection found");
  }
  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_ssl_use(నిర్మాణం connectdata *conn,
                              నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    ఒకవేళ(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    ఒకవేళ(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      తిరిగి FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    ఒకవేళ(!Curl_ssl_conn_config_సరిపోలిక(m->దత్తాంశం, &m->ssl_config, conn, FALSE))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    తిరిగి FALSE;
  తిరిగి TRUE;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
స్థిర bool url_సరిపోలిక_proxy_use(నిర్మాణం connectdata *conn,
                                నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(m->needle->బిట్లు.origin_is_proxy != conn->బిట్లు.origin_is_proxy)
    తిరిగి FALSE;

  ఒకవేళ(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    తిరిగి FALSE;

  ఒకవేళ(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    తిరిగి FALSE;

  ఒకవేళ(m->దత్తాంశం->సమితి.socks5_auth_only && !conn->బిట్లు.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->దత్తాంశం,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    తిరిగి FALSE;
  }

  ఒకవేళ(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    ఒకవేళ(!Curl_ssl_conn_config_సరిపోలిక(m->దత్తాంశం, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->దత్తాంశం,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      తిరిగి FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  తిరిగి TRUE;
}
#లేకపోతే
#నిర్వచించు url_సరిపోలిక_proxy_use(c, m) ((శూన్య)(c), (శూన్య)(m), TRUE)
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
స్థిర bool url_సరిపోలిక_http_multiplex(నిర్మాణం connectdata *conn,
                                     నిర్మాణం url_conn_సరిపోలిక *m,
                                     bool *pwait_pipe)
{
  ఒకవేళ(m->may_multiplex &&
     (m->దత్తాంశం->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_చూసిన) {
    ఒకవేళ(m->దత్తాంశం->సమితి.pipewait) {
      infof(m->దత్తాంశం, "Server upgrade does not support multiplex yet, wait");
      m->దొరికింది = ఖాళీ;
      *pwait_pipe = TRUE;
      తిరిగి TRUE; /* stop searching, we want to wait */
    }
    infof(m->దత్తాంశం, "Server upgrade cannot be used");
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_http_సంస్కరణ(నిర్మాణం connectdata *conn,
                                   నిర్మాణం url_conn_సరిపోలిక *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  ఒకవేళ((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    ఎంపిక(Curl_conn_http_సంస్కరణ(m->దత్తాంశం, conn)) {
    స్థితి 30:
      ఒకవేళ(!(m->దత్తాంశం->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->దత్తాంశం, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        తిరిగి FALSE;
      }
      ఆపు;
    స్థితి 20:
      ఒకవేళ(!(m->దత్తాంశం->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->దత్తాంశం, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        తిరిగి FALSE;
      }
      ఆపు;
    అప్రమేయ:
      ఒకవేళ(!(m->దత్తాంశం->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->దత్తాంశం, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        తిరిగి FALSE;
      }
      ఆపు;
    }
  }
  తిరిగి TRUE;
}
#లేకపోతే
#నిర్వచించు url_సరిపోలిక_http_multiplex(c, m, w) ((శూన్య)(c), (శూన్య)(m), TRUE)
#నిర్వచించు url_సరిపోలిక_http_సంస్కరణ(c, m)      ((శూన్య)(c), (శూన్య)(m), TRUE)
#అయితే_ముగింపు

స్థిర bool url_సరిపోలిక_proto_config(నిర్మాణం connectdata *conn,
                                   నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(!url_సరిపోలిక_http_సంస్కరణ(conn, m))
    తిరిగి FALSE;

#నిర్వచితమైతే USE_SSH
  ఒకవేళ(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    ఒకవేళ(!ssh_config_matches(m->needle, conn))
      తిరిగి FALSE;
  }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  లేకపోతే ఒకవేళ(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    ఒకవేళ(!Curl_ftp_conns_సరిపోలిక(m->needle, conn))
      తిరిగి FALSE;
  }
#అయితే_ముగింపు
  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_auth(నిర్మాణం connectdata *conn,
                           నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(!Curl_creds_same(m->needle->creds, conn->creds)) {
    ఒకవేళ(m->needle->creds)
      తిరిగి FALSE;
    ఒకవేళ(!Curl_creds_same(m->దత్తాంశం->state.creds, conn->creds))
      తిరిగి FALSE;
  }
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  ఒకవేళ(m->needle->gssapi_delegation != conn->gssapi_delegation)
    తిరిగి FALSE;
#అయితే_ముగింపు

  తిరిగి TRUE;
}

స్థిర bool url_సరిపోలిక_destination(నిర్మాణం connectdata *conn,
                                  నిర్మాణం url_conn_సరిపోలిక *m)
{
  /* Different connect-to peers never match */
  ఒకవేళ(!Curl_peer_same_destination(m->needle->ద్వారా_peer, conn->ద్వారా_peer))
    తిరిగి FALSE;

  ఒకవేళ(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    తిరిగి FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  తిరిగి Curl_peer_same_destination(m->needle->origin, conn->origin);
}

స్థిర bool url_సరిపోలిక_ssl_config(నిర్మాణం connectdata *conn,
                                 నిర్మాణం url_conn_సరిపోలిక *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  ఒకవేళ(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_సరిపోలిక(m->దత్తాంశం, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->దత్తాంశం, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

#నిర్వచితమైతే USE_NTLM
స్థిర bool url_సరిపోలిక_auth_ntlm(నిర్మాణం connectdata *conn,
                                నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    ఒకవేళ(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->దత్తాంశం->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->దత్తాంశం->state.origin))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    ఒకవేళ(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->దత్తాంశం->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->దత్తాంశం->state.origin)))
      తిరిగి FALSE;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  ఒకవేళ(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    ఒకవేళ(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(m->want_proxy_ntlm_http) {
    ఒకవేళ(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      తిరిగి FALSE;
  }
#అయితే_ముగింపు
  తిరిగి TRUE;
}
#లేకపోతే
#నిర్వచించు url_సరిపోలిక_auth_ntlm(c, m) ((శూన్య)(c), (శూన్య)(m), TRUE)
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SPNEGO
స్థిర bool url_సరిపోలిక_auth_nego(నిర్మాణం connectdata *conn,
                                నిర్మాణం url_conn_సరిపోలిక *m)
{
  ఒకవేళ(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    ఒకవేళ(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->దత్తాంశం->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->దత్తాంశం->state.origin))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    ఒకవేళ(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->దత్తాంశం->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->దత్తాంశం->state.origin)))
      తిరిగి FALSE;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  ఒకవేళ(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    ఒకవేళ(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      తిరిగి FALSE;
  }
  లేకపోతే ఒకవేళ(m->want_proxy_nego_http) {
    ఒకవేళ(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      తిరిగి FALSE;
  }
#అయితే_ముగింపు
  తిరిగి TRUE;
}
#లేకపోతే
#నిర్వచించు url_సరిపోలిక_auth_nego(c, m) ((శూన్య)(c), (శూన్య)(m), TRUE)
#అయితే_ముగింపు

స్థిర cpool_సరిపోలిక_ఫలితం url_సరిపోలిక_conn(నిర్మాణం connectdata *conn,
                                         శూన్య *userdata)
{
  నిర్మాణం url_conn_సరిపోలిక *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  ఒకవేళ(!url_సరిపోలిక_connect_config(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  /* match for destination and protocol? */
  ఒకవేళ(!url_సరిపోలిక_destination(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_fully_connected(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_multiplex_needs(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_ssl_use(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_proxy_use(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;
  ఒకవేళ(!url_సరిపోలిక_ssl_config(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_http_multiplex(conn, m, &wait_pipe))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_auth(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_proto_config(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_auth_ntlm(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_auth_nego(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  ఒకవేళ(!url_సరిపోలిక_multiplex_limits(conn, m))
    తిరిగి CPOOL_సరిపోలిక_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  ఒకవేళ(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->దొరికింది);
    m->wait_pipe = TRUE;
    తిరిగి CPOOL_సరిపోలిక_దొరికింది;
  }

  ఒకవేళ(m->దత్తాంశం->సమితి.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->దత్తాంశం, conn, &m->now);
    ఒకవేళ(age_ms > m->దత్తాంశం->సమితి.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      తిరిగి CPOOL_సరిపోలిక_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  ఒకవేళ(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->దత్తాంశం, &m->now)) {
    infof(m->దత్తాంశం, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    తిరిగి CPOOL_సరిపోలిక_CLOSE;
  }

  /* conn matches our needs. */
  m->దొరికింది = conn;
  తిరిగి CPOOL_సరిపోలిక_దొరికింది;
}

స్థిర bool url_సరిపోలిక_ఫలితం(శూన్య *userdata)
{
  నిర్మాణం url_conn_సరిపోలిక *సరిపోలిక = userdata;
  ఒకవేళ(సరిపోలిక->దొరికింది) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(సరిపోలిక->దత్తాంశం, సరిపోలిక->దొరికింది, TRUE);
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(సరిపోలిక->చూసిన_single_use_conn && !సరిపోలిక->చూసిన_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    సరిపోలిక->wait_pipe = FALSE;
  }
  లేకపోతే ఒకవేళ(సరిపోలిక->చూసిన_pending_conn && సరిపోలిక->దత్తాంశం->సమితి.pipewait) {
    infof(సరిపోలిక->దత్తాంశం,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    సరిపోలిక->wait_pipe = TRUE;
  }
  తిరిగి FALSE;
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
స్థిర bool url_attach_existing(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *needle,
                                నిర్మాణం url_conn_సరిపోలిక *m,
                                స్థిరరాశి నిర్మాణం curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  తిరిగి Curl_cpool_కనుగొను(దత్తాంశం, needle->destination, TRUE, pnow,
                         url_సరిపోలిక_conn, url_సరిపోలిక_ఫలితం, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
స్థిర నిర్మాణం connectdata *allocate_conn(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = curlx_calloc(1, పరిమాణం(నిర్మాణం connectdata));
  ఒకవేళ(!conn)
    తిరిగి ఖాళీ;

  /* and we setup a few fields in case we end up actually using this struct */
  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->recv_idx = 0; /* default for receiving transfer data */
  conn->send_idx = 0; /* default for sending transfer data */
  conn->connection_id = -1;    /* no ID */
  conn->cpid = UINT32_MAX;  /* not in connection pool yet */
  conn->attached_xfers = 0;
  conn->shutdown.ప్రారంభం_ms[FIRSTSOCKET] =
    conn->shutdown.ప్రారంభం_ms[SECONDARYSOCKET] = -1;

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  conn->బిట్లు.ftp_use_epsv = దత్తాంశం->సమితి.ftp_use_epsv;
  conn->బిట్లు.ftp_use_eprt = దత్తాంశం->సమితి.ftp_use_eprt;
#అయితే_ముగింపు
  conn->ip_సంస్కరణ = దత్తాంశం->సమితి.ipver;
  conn->బిట్లు.connect_only = (bool)దత్తాంశం->సమితి.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(దత్తాంశం, STRING_INTERFACE_OPT));
    ఒకవేళ(!conn->interface_opts)
      వెళ్ళు error;
  }
#నిర్వచితం_కాకపోతే CURL_DISABLE_BINDLOCAL
  conn->localportrange = దత్తాంశం->సమితి.localportrange;
  conn->localport = దత్తాంశం->సమితి.localport;
#అయితే_ముగింపు

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = దత్తాంశం->సమితి.fclosesocket;
  conn->closesocket_client = దత్తాంశం->సమితి.closesocket_client;
#ఒకవేళ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = దత్తాంశం->సమితి.gssapi_delegation;
#అయితే_ముగింపు
  తిరిగి conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  తిరిగి ఖాళీ;
}

స్థిర CURLcode url_సమితి_conn_scheme(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  స్థిరరాశి నిర్మాణం Curl_scheme *scheme = conn->origin->scheme;
  ఒకవేళ(scheme->నిరంతరం &&
     (దత్తాంశం->సమితి.allowed_protocols & scheme->protocol) &&
     (!దత్తాంశం->state.this_is_a_follow ||
       (దత్తాంశం->సమితి.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    తిరిగి CURLE_OK;
  }
  ఒకవేళ(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(దత్తాంశం, "Protocol \"%s\" is not for transfers", scheme->name);
  లేకపోతే
    failf(దత్తాంశం, "Protocol \"%s\" is disabled%s", scheme->name,
          దత్తాంశం->state.this_is_a_follow ? " (in redirect)" : "");
  తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_గమ్యం_curlcode(CURLUcode uc)
{
  ఎంపిక(uc) {
  అప్రమేయ:
    తిరిగి CURLE_URL_MALFORMAT;
  స్థితి CURLUE_UNSUPPORTED_SCHEME:
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  స్థితి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  స్థితి CURLUE_USER_NOT_ALLOWED:
    తిరిగి CURLE_LOGIN_DENIED;
  }
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
స్థిర CURLcode hsts_upgrade(నిర్మాణం Curl_easy *దత్తాంశం,
                             CURLU *uh,
                             uint16_t పోర్టు_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  ఒకవేళ(దత్తాంశం->hsts && (దత్తాంశం->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(దత్తాంశం->hsts, దత్తాంశం->state.origin)) {
    అక్షరం *url;
    CURLUcode uc;
    CURLcode ఫలితం;

    uc = curl_url_సమితి(uh, CURLUPART_SCHEME, "https", 0);
    ఒకవేళ(uc)
      తిరిగి Curl_uc_గమ్యం_curlcode(uc);
    Curl_bufref_free(&దత్తాంశం->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    ఒకవేళ(uc)
      తిరిగి Curl_uc_గమ్యం_curlcode(uc);
    Curl_bufref_సమితి(&దత్తాంశం->state.url, url, 0, curl_free);

    ఫలితం = Curl_peer_from_url(uh, దత్తాంశం, పోర్టు_override, scope_id,
                                &దత్తాంశం->state.origin);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    infof(దత్తాంశం, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  తిరిగి CURLE_OK;
}
#లేకపోతే
#నిర్వచించు hsts_upgrade(x, y, z, a) CURLE_OK
#అయితే_ముగింపు

స్థిర bool str_has_ctrl(స్థిరరాశి అక్షరం *ఆదానం)
{
  ఒకవేళ(ఆదానం) {
    స్థిరరాశి గుర్తులేని అక్షరం *str = (స్థిరరాశి గుర్తులేని అక్షరం *)ఆదానం;
    అయితే(*str) {
      ఒకవేళ(*str < 0x20)
        తిరిగి TRUE;
      str++;
    }
  }
  తిరిగి FALSE;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
స్థిర CURLcode url_సమితి_దత్తాంశం_creds_netrc(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం Curl_creds **pcreds)
{
  నిర్మాణం Curl_creds *ncreds_బహిర్గతం = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(దత్తాంశం->సమితి.use_netrc) { /* not CURL_NETRC_IGNORED */
    నిర్మాణం Curl_creds *ncreds_లోపల = ఖాళీ;
    bool పరిశీలన_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    ఒకవేళ(*pcreds) {
      ఎంపిక((*pcreds)->ఆధారం) {
      స్థితి CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        పరిశీలన_netrc = FALSE;
        ఆపు;
      స్థితి CREDS_URL: /* only apply when netrc is not required */
        ఒకవేళ(దత్తాంశం->సమితి.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_లోపల = *pcreds;
        }
        లేకపోతే ఒకవేళ(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_లోపల = *pcreds;
        }
        లేకపోతే
          పరిశీలన_netrc = FALSE;
        ఆపు;
      అప్రమేయ: /* ignore credentials from other sources */
        ఆపు;
      }
    }

    ఒకవేళ(!పరిశీలన_netrc)
      వెళ్ళు బహిర్గతం;

    ret = Curl_netrc_పరిశీలన(దత్తాంశం, &దత్తాంశం->state.netrc,
                          దత్తాంశం->state.origin->hostname,
                          Curl_creds_user(ncreds_లోపల),
                          CURL_EASY_STR(దత్తాంశం, STRING_NETRC_FILE),
                          &ncreds_బహిర్గతం);
    DEBUGASSERT(!ret || !ncreds_బహిర్గతం);
    ఒకవేళ(ret == NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ret && ((ret == NETRC_NO_సరిపోలిక) ||
                    (దత్తాంశం->సమితి.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(దత్తాంశం, "Could not find host %s in the %s file; using defaults",
            దత్తాంశం->state.origin->hostname,
            (CURL_EASY_STR(దత్తాంశం, STRING_NETRC_FILE) ?
             CURL_EASY_STR(దత్తాంశం, STRING_NETRC_FILE) : ".netrc"));
    }
    లేకపోతే ఒకవేళ(ret) {
      స్థిరరాశి అక్షరం *m = Curl_netrc_strerror(ret);
      failf(దత్తాంశం, ".netrc error: %s", m);
      ఫలితం = CURLE_READ_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ncreds_బహిర్గతం) {
      ఒకవేళ(!(దత్తాంశం->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_బహిర్గతం->user) ||
          str_has_ctrl(ncreds_బహిర్గతం->passwd))) {
        failf(దత్తాంశం, "control code detected in .netrc credentials");
        ఫలితం = CURLE_READ_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      CURL_TRC_M(దత్తాంశం, "netrc: using credentials for %s as %s",
                 దత్తాంశం->state.origin->hostname, ncreds_బహిర్గతం->user);
      ఫలితం = Curl_creds_విలీనం(ncreds_బహిర్గతం->user, ncreds_బహిర్గతం->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      /* for updated strings, we update them in the URL */
      uc = curl_url_సమితి(దత్తాంశం->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      ఒకవేళ(!uc)
        uc = curl_url_సమితి(దత్తాంశం->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      ఒకవేళ(uc)
        ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    }
    లేకపోతే
      DEBUGASSERT(0);
  }

#నిర్వచితమైతే CURLVERBOSE
  Curl_creds_trace(దత్తాంశం, దత్తాంశం->state.creds, "transfer credentials");
#అయితే_ముగింపు

బహిర్గతం:
  Curl_creds_unlink(&ncreds_బహిర్గతం);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* CURL_DISABLE_NETRC */

స్థిర CURLcode url_సమితి_దత్తాంశం_creds(నిర్మాణం Curl_easy *దత్తాంశం, CURLU *uh)
{
  నిర్మాణం Curl_creds *newcreds = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ((CURL_EASY_STR(దత్తాంశం, STRING_USERNAME) ||
      CURL_EASY_STR(దత్తాంశం, STRING_PASSWORD) ||
      CURL_EASY_STR(దత్తాంశం, STRING_BEARER) ||
      CURL_EASY_STR(దత్తాంశం, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(దత్తాంశం, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_గమ్యం_origin(దత్తాంశం, దత్తాంశం->state.origin)) {
    ఫలితం = Curl_creds_create(CURL_EASY_STR(దత్తాంశం, STRING_USERNAME),
                               CURL_EASY_STR(దత్తాంశం, STRING_PASSWORD),
                               CURL_EASY_STR(దత్తాంశం, STRING_BEARER),
                               CURL_EASY_STR(దత్తాంశం, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(దత్తాంశం, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(newcreds &&
       !(దత్తాంశం->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(దత్తాంశం, "control code detected in credentials");
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  ఒకవేళ(!newcreds || !Curl_creds_has_user(newcreds)) {
    అక్షరం *user = ఖాళీ;
    అక్షరం *passwd = ఖాళీ;
    అక్షరం *udecoded = ఖాళీ;
    అక్షరం *pdecoded = ఖాళీ;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    ఒకవేళ(uc && (uc != CURLUE_NO_USER))
      ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    ఒకవేళ(!ఫలితం) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      ఒకవేళ(uc && (uc != CURLUE_NO_PASSWORD))
        ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    }
    ఒకవేళ(!ఫలితం && user) {
      ఫలితం = Curl_urldecode(user, 0, &udecoded, ఖాళీ,
                              (దత్తాంశం->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    ఒకవేళ(!ఫలితం && passwd) {
      ఫలితం = Curl_urldecode(passwd, 0, &pdecoded, ఖాళీ,
                              (దత్తాంశం->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_creds_విలీనం(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "error extracting credentials from URL");
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  ఫలితం = url_సమితి_దత్తాంశం_creds_netrc(దత్తాంశం, &newcreds);
#అయితే_ముగింపు /* CURL_DISABLE_NETRC */

బహిర్గతం:
  ఒకవేళ(!ఫలితం && !Curl_creds_equal(దత్తాంశం->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&దత్తాంశం->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  తిరిగి ఫలితం;
}

స్థిర CURLcode url_సమితి_conn_origin_etc(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;

  Curl_peer_link(&conn->origin, దత్తాంశం->state.origin);

  /* set the connection scheme */
  ఫలితం = url_సమితి_conn_scheme(దత్తాంశం, conn);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* set the connection options */
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(దత్తాంశం, STRING_OPTIONS));
    ఒకవేళ(!conn->options) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->state.పైకి.options) {
    conn->options = curlx_strdup(దత్తాంశం->state.పైకి.options);
    ఒకవేళ(!conn->options) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  తిరిగి ఫలితం;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
స్థిర CURLcode అమరిక_range(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం UrlState *s = &దత్తాంశం->state;
  s->resume_from = దత్తాంశం->సమితి.సమితి_resume_from;
  ఒకవేళ(s->resume_from || CURL_EASY_STR(దత్తాంశం, STRING_సమితి_RANGE)) {
    ఒకవేళ(s->rangestringalloc)
      curlx_free(s->range);

    ఒకవేళ(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    లేకపోతే
      s->range = curlx_strdup(CURL_EASY_STR(దత్తాంశం, STRING_సమితి_RANGE));

    ఒకవేళ(!s->range)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  లేకపోతే
    s->use_range = FALSE; /* disable range download */

  తిరిగి CURLE_OK;
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
స్థిర CURLcode అమరిక_connection_internals(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_peer *peer = ఖాళీ;

  ఒకవేళ(conn->scheme->నిరంతరం->అమరిక_connection) {
    CURLcode ఫలితం = conn->scheme->నిరంతరం->అమరిక_connection(దత్తాంశం, conn);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  ఒకవేళ(!peer)
    తిరిగి CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  ఒకవేళ(peer->ipv6)
    ఒకవేళ(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->పోర్టు);
    లేకపోతే
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->పోర్టు);
  లేకపోతే
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->పోర్టు);
  ఒకవేళ(!conn->destination)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  Curl_strntolower(conn->destination, conn->destination,
                   అక్షరశ్రేణి_పొడవు(conn->destination));

#నిర్వచితమైతే USE_IPV6
  ఒకవేళ(దత్తాంశం->సమితి.scope_id)
    conn->scope_id = దత్తాంశం->సమితి.scope_id;
  లేకపోతే {
    peer = Curl_conn_get_మొదటి_peer(conn, FIRSTSOCKET);
    ఒకవేళ(!peer)
      తిరిగి CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
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
CURLcode Curl_parse_login_details(స్థిరరాశి అక్షరం *login, స్థిరరాశి size_t len,
                                  అక్షరం **userp, అక్షరం **passwdp,
                                  అక్షరం **optionsp)
{
  అక్షరం *ubuf = ఖాళీ;
  అక్షరం *pbuf = ఖాళీ;
  స్థిరరాశి అక్షరం *psep = ఖాళీ;
  స్థిరరాశి అక్షరం *osep = ఖాళీ;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  ఒకవేళ(optionsp)
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
  ఒకవేళ(!ubuf)
    వెళ్ళు error;

  /* Clone the password portion buffer */
  ఒకవేళ(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    ఒకవేళ(!pbuf)
      వెళ్ళు error;
  }

  /* Allocate the options portion buffer */
  ఒకవేళ(optionsp) {
    అక్షరం *obuf = ఖాళీ;
    ఒకవేళ(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      ఒకవేళ(!obuf)
        వెళ్ళు error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  తిరిగి CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

/*
 * Set the login details so they are available in the connection
 */
స్థిర CURLcode url_సమితి_conn_login(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  ఒకవేళ((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, దత్తాంశం->state.origin);
    ఒకవేళ(దత్తాంశం->state.creds)
      Curl_creds_link(&conn->creds, దత్తాంశం->state.creds);
    లేకపోతే
      తిరిగి Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               ఖాళీ, ఖాళీ, ఖాళీ, CREDS_NONE, &conn->creds);
  }
  లేకపోతే ఒకవేళ(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, దత్తాంశం->state.origin);
    Curl_creds_link(&conn->creds, దత్తాంశం->state.creds);
  }

  తిరిగి CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
స్థిర CURLcode parse_connect_గమ్యం_string(నిర్మాణం Curl_easy *దత్తాంశం,
                                        స్థిరరాశి నిర్మాణం Curl_peer *dest,
                                        స్థిరరాశి అక్షరం *conn_గమ్యం_పంక్తి,
                                        నిర్మాణం Curl_peer **pvia_dest)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ptr = conn_గమ్యం_పంక్తి;
  bool host_సరిపోలిక = FALSE;
  bool పోర్టు_సరిపోలిక = FALSE;

  *pvia_dest = ఖాళీ;

  ఒకవేళ(*ptr == ':') {
    /* an empty hostname always matches */
    host_సరిపోలిక = TRUE;
    ptr++;
  }
  లేకపోతే {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = అక్షరశ్రేణి_పొడవు(dest->hostname);
    host_సరిపోలిక = curl_strnequal(ptr, dest->hostname, hlen);
    ఒకవేళ(!host_సరిపోలిక && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = అక్షరశ్రేణి_పొడవు(dest->user_hostname);
      host_సరిపోలిక = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_సరిపోలిక = host_సరిపోలిక && ptr[hlen] == ':';
    ఒకవేళ(host_సరిపోలిక)
      ptr += hlen + 1;
  }

  ఒకవేళ(host_సరిపోలిక) {
    ఒకవేళ(*ptr == ':') {
      /* an empty port always matches */
      పోర్టు_సరిపోలిక = TRUE;
      ptr++;
    }
    లేకపోతే {
      /* check whether the URL's port matches */
      స్థిరరాశి అక్షరం *ptr_తదుపరి = strchr(ptr, ':');
      ఒకవేళ(ptr_తదుపరి) {
        curl_off_t పోర్టు_గమ్యం_సరిపోలిక;
        ఒకవేళ(!curlx_str_సంఖ్య(&ptr, &పోర్టు_గమ్యం_సరిపోలిక, 0xffff) &&
           ((uint16_t)పోర్టు_గమ్యం_సరిపోలిక == dest->పోర్టు)) {
          పోర్టు_సరిపోలిక = TRUE;
        }
        ptr = ptr_తదుపరి + 1;
      }
    }
  }

  ఒకవేళ(host_సరిపోలిక && పోర్టు_సరిపోలిక && ptr && *ptr)
    ఫలితం = Curl_peer_from_connect_గమ్యం(దత్తాంశం, dest, ptr, pvia_dest);

  తిరిగి ఫలితం;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
స్థిర CURLcode url_సమితి_conn_peer(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం Curl_peer *origin = conn->origin;
  నిర్మాణం Curl_peer *ద్వారా_peer = ఖాళీ;
  నిర్మాణం curl_slist *conn_గమ్యం_నమోదు = దత్తాంశం->సమితి.connect_గమ్యం;

  DEBUGASSERT(!conn->ద్వారా_peer);
  Curl_peer_unlink(&conn->ద్వారా_peer);

  అయితే(conn_గమ్యం_నమోదు && !ద్వారా_peer) {
    ఫలితం = parse_connect_గమ్యం_string(దత్తాంశం, origin, conn_గమ్యం_నమోదు->దత్తాంశం,
                                     &ద్వారా_peer);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    conn_గమ్యం_నమోదు = conn_గమ్యం_నమోదు->తదుపరి;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  ఒకవేళ(దత్తాంశం->asi && !ద్వారా_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#నిర్వచితమైతే DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#లేకపోతే
      0
#అయితే_ముగింపు
       )) {
    /* no connect_to match, try alt-svc! */
    గణన alpnid srcalpnid = ALPN_none;
    bool తగిలింది = FALSE;
    నిర్మాణం altsvc *as = ఖాళీ;
    పూర్ణాంకం allowed_alpns = ALPN_none;
    నిర్మాణం http_negotiation *neg = &దత్తాంశం->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(దత్తాంశం, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#నిర్వచితమైతే USE_HTTP3
    ఒకవేళ(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTP2
    ఒకవేళ(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#అయితే_ముగింపు
    ఒకవేళ(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (పూర్ణాంకం)దత్తాంశం->asi->flags;

    DEBUGF(infof(దత్తాంశం, "check Alt-Svc for host '%s'", origin->hostname));
#నిర్వచితమైతే USE_HTTP3
    ఒకవేళ(!తగిలింది && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      తగిలింది = Curl_altsvc_lookup(దత్తాంశం->asi,
                               origin, ALPN_h3, /* from */
                               &as /*గమ్యం*/,
                               allowed_alpns, &same_dest);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTP2
    ఒకవేళ(!తగిలింది && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_పూర్వ_knowledge) {
      srcalpnid = ALPN_h2;
      తగిలింది = Curl_altsvc_lookup(దత్తాంశం->asi,
                               origin, ALPN_h2, /* from */
                               &as /*గమ్యం*/,
                               allowed_alpns, &same_dest);
    }
#అయితే_ముగింపు
    ఒకవేళ(!తగిలింది && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      తగిలింది = Curl_altsvc_lookup(దత్తాంశం->asi,
                               origin, ALPN_h1, /* from */
                               &as /*గమ్యం*/,
                               allowed_alpns, &same_dest);
    }

    ఒకవేళ(తగిలింది && same_dest) {
      /* same destination, but more HTTPS version options */
      ఎంపిక(as->dst.alpnid) {
      స్థితి ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        ఆపు;
      స్థితి ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        ఆపు;
      స్థితి ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        ఆపు;
      అప్రమేయ: /* should not be possible */
        ఆపు;
      }
    }
    లేకపోతే ఒకవేళ(తగిలింది) {
      ఫలితం = Curl_peer_create(దత్తాంశం, conn->origin->scheme,
                                as->dst.host, as->dst.పోర్టు,
                                &ద్వారా_peer);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      infof(దత్తాంశం, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->పోర్టు,
            Curl_alpnid2str(as->dst.alpnid),
            ద్వారా_peer->hostname, ద్వారా_peer->పోర్టు);
      conn->బిట్లు.altused = TRUE;
      ఒకవేళ(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        ఎంపిక(as->dst.alpnid) {
        స్థితి ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          ఆపు;
        స్థితి ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          ఆపు;
        స్థితి ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          ఆపు;
        అప్రమేయ: /* should not be possible */
          ఆపు;
        }
      }
    }
  }
#అయితే_ముగింపు

  ఒకవేళ(ద్వారా_peer)
    conn->ద్వారా_peer = ద్వారా_peer;

  తిరిగి ఫలితం;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
స్థిర శూన్య url_conn_reuse_adjust(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *needle)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  ఒకవేళ(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#అయితే_ముగింపు

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
  Curl_peer_link(&conn->ద్వారా_peer, needle->ద్వారా_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->ద్వారా_peer2, needle->ద్వారా_peer2);
}

స్థిర శూన్య conn_meta_freeentry(శూన్య *p)
{
  (శూన్య)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

స్థిర CURLcode url_create_needle(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata **pneedle)
{
  నిర్మాణం connectdata *needle = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  bool జాలం_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(దత్తాంశం);
  ఒకవేళ(!needle) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_బైట్లు,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  ఫలితం = url_సమితి_conn_origin_etc(దత్తాంశం, needle);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  DEBUGASSERT(needle->origin);
  జాలం_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#నిర్వచితమైతే USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  ఒకవేళ(జాలం_scheme && CURL_EASY_STR(దత్తాంశం, STRING_UNIX_SOCKET_మార్గం)) {
    ఫలితం = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(దత్తాంశం, STRING_UNIX_SOCKET_మార్గం),
      (bool)దత్తాంశం->సమితి.abstract_unix_socket, &needle->ద్వారా_peer);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు /* USE_UNIX_SOCKETS */

  ఒకవేళ(జాలం_scheme && !needle->ద్వారా_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    ఫలితం = url_సమితి_conn_peer(దత్తాంశం, needle);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  ఒకవేళ(Curl_peer_equal(needle->origin, needle->ద్వారా_peer)) {
    Curl_peer_unlink(&needle->ద్వారా_peer);
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  ఒకవేళ(జాలం_scheme &&
     (!needle->ద్వారా_peer || !needle->ద్వారా_peer->unix_socket)) {
    ఫలితం = Curl_proxy_init_conn(దత్తాంశం, needle);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు /* CURL_DISABLE_PROXY */

  ఫలితం = url_సమితి_conn_login(దత్తాంశం, needle); /* default credentials */
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  ఫలితం = అమరిక_connection_internals(దత్తాంశం, needle);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    ఒకవేళ(దత్తాంశం->సమితి.ssl_enable_alpn)
      needle->బిట్లు.tls_enable_alpn = TRUE;
  }

  ఒకవేళ(జాలం_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->బిట్లు.tcp_fastopen = దత్తాంశం->సమితి.tcp_fastopen;
#నిర్వచితమైతే USE_UNIX_SOCKETS
    ఒకవేళ(Curl_conn_get_మొదటి_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#అయితే_ముగింపు
  }

బహిర్గతం:
  ఒకవేళ(!ఫలితం) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  లేకపోతే {
    *pneedle = ఖాళీ;
    ఒకవేళ(needle)
      Curl_conn_free(దత్తాంశం, needle);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode url_సమితి_దత్తాంశం_origin_and_creds(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_సమితి_uh = (దత్తాంశం->సమితి.uh && !దత్తాంశం->state.this_is_a_follow);
  uint16_t పోర్టు_override = దత్తాంశం->state.allow_పోర్టు ? దత్తాంశం->సమితి.use_పోర్టు : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  ఒకవేళ(!Curl_bufref_ptr(&దత్తాంశం->state.url)) {
    ఫలితం = CURLE_URL_MALFORMAT;
    వెళ్ళు బహిర్గతం;
  }

  పైకి_free(దత్తాంశం); /* cleanup previous leftovers first */

  /* parse the URL */
  ఒకవేళ(use_సమితి_uh)
    uh = దత్తాంశం->state.uh = curl_url_dup(దత్తాంశం->సమితి.uh);
  లేకపోతే
    uh = దత్తాంశం->state.uh = curl_url();
  ఒకవేళ(!uh) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(!use_సమితి_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    ఒకవేళ(curl_strequal("file", CURL_EASY_STR(దత్తాంశం, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&దత్తాంశం->state.url), ఖాళీ, 0, TRUE))
      prepend_sheme = TRUE;
    లేకపోతే {
      uc = curl_url_సమితి(uh, CURLUPART_URL, Curl_bufref_ptr(&దత్తాంశం->state.url),
                        (గుర్తులేని పూర్ణాంకం)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (దత్తాంశం->సమితి.disallow_username_లోపల_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (దత్తాంశం->సమితి.మార్గం_as_is ? CURLU_మార్గం_AS_IS : 0));
      ఒకవేళ(uc) {
        failf(దత్తాంశం, "URL rejected: %s", curl_url_strerror(uc));
        ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
        వెళ్ళు బహిర్గతం;
      }

      ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        అక్షరం *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        ఒకవేళ(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        ఒకవేళ(uc && uc != CURLUE_NO_SCHEME) {
          ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
          వెళ్ళు బహిర్గతం;
        }
      }
    }

    ఒకవేళ(prepend_sheme) {
      అక్షరం *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(దత్తాంశం, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&దత్తాంశం->state.url));
      ఒకవేళ(!url) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      uc = curl_url_సమితి(uh, CURLUPART_URL, url,
                        (గుర్తులేని పూర్ణాంకం)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (దత్తాంశం->సమితి.disallow_username_లోపల_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (దత్తాంశం->సమితి.మార్గం_as_is ? CURLU_మార్గం_AS_IS : 0));
      curl_free(url);
      ఒకవేళ(uc) {
        failf(దత్తాంశం, "URL rejected: %s", curl_url_strerror(uc));
        ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
        వెళ్ళు బహిర్గతం;
      }
    }
    {
      అక్షరం *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_ఖాళీలు);
      ఒకవేళ(uc) {
        ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(prepend_sheme)
        infof(దత్తాంశం, "Applied default protocol '%s'",
              CURL_EASY_STR(దత్తాంశం, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_సమితి(&దత్తాంశం->state.url, newurl, 0, curl_free);
    }
  }

#నిర్వచితమైతే USE_IPV6
  scope_id = దత్తాంశం->సమితి.scope_id;
#అయితే_ముగింపు

  /* `uh` is now as the connection should use it, probably. */
  ఫలితం = Curl_peer_from_url(uh, దత్తాంశం, పోర్టు_override, scope_id,
                              &దత్తాంశం->state.origin);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  /* The origin might get changed when HSTS applies */
  ఫలితం = hsts_upgrade(దత్తాంశం, uh, పోర్టు_override, scope_id);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  ఒకవేళ(!దత్తాంశం->state.initial_origin)
    Curl_peer_link(&దత్తాంశం->state.initial_origin, దత్తాంశం->state.origin);

  uc = curl_url_get(uh, CURLUPART_మార్గం, &దత్తాంశం->state.పైకి.మార్గం, CURLU_URLENCODE);
  ఒకవేళ(uc) {
    ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    వెళ్ళు బహిర్గతం;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &దత్తాంశం->state.పైకి.query,
                    CURLU_GET_ఖాళీలు);
  ఒకవేళ(uc && (uc != CURLUE_NO_QUERY)) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &దత్తాంశం->state.పైకి.options,
                    CURLU_URLDECODE);
  ఒకవేళ(uc && (uc != CURLUE_NO_OPTIONS)) {
    ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = url_సమితి_దత్తాంశం_creds(దత్తాంశం, uh);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర CURLcode url_సరిపోలిక_init(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *needle,
                               నిర్మాణం url_conn_సరిపోలిక *m)
{
  memset(m, 0, పరిమాణం(*m));
  m->దత్తాంశం = దత్తాంశం;
  m->needle = needle;
  m->now = *Curl_pgrs_now(దత్తాంశం);
  m->may_multiplex = xfer_may_multiplex(దత్తాంశం, needle);

#నిర్వచితమైతే USE_NTLM
  m->want_ntlm_http =
    (దత్తాంశం->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_గమ్యం_host(దత్తాంశం);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (దత్తాంశం->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#అయితే_ముగింపు
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (దత్తాంశం->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_గమ్యం_host(దత్తాంశం);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (దత్తాంశం->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#అయితే_ముగింపు
#అయితే_ముగింపు
  m->require_tls = దత్తాంశం->సమితి.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = దత్తాంశం->సమితి.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  తిరిగి Curl_ssl_filter_config_tmp_init(దత్తాంశం, needle->origin,
                                         &m->ssl_config,
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#లేకపోతే
                                         ఖాళీ);
#అయితే_ముగింపు
}

స్థిర శూన్య url_సరిపోలిక_destroy(నిర్మాణం url_conn_సరిపోలిక *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#అయితే_ముగింపు
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
స్థిర CURLcode url_కనుగొను_or_create_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                                        స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం connectdata *needle = ఖాళీ;
  నిర్మాణం url_conn_సరిపోలిక సరిపోలిక;
  bool సరిపోలిక_initialized = FALSE;
  CURLcode ఫలితం;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  ఫలితం = url_create_needle(దత్తాంశం, &needle);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  DEBUGASSERT(needle);

  ఫలితం = url_సరిపోలిక_init(దత్తాంశం, needle, &సరిపోలిక);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  సరిపోలిక_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#నిర్వచితం_కాకపోతే CURL_DISABLE_FILE
  ఒకవేళ(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool పూర్తి;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->నిరంతరం->connect_it);
    దత్తాంశం->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    దత్తాంశం->info.conn_protocol = needle->scheme->protocol & CURLPROTO_ముసుగు;
    ఫలితం = needle->scheme->నిరంతరం->connect_it(దత్తాంశం, &పూర్తి);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    /* Setup a "faked" transfer that will do nothing */
    ఫలితం = Curl_cpool_కూడు(దత్తాంశం, needle, 0, 0, pnow);
    Curl_attach_connection(దత్తాంశం, needle, TRUE);
    needle = ఖాళీ;
    ఒకవేళ(!ఫలితం) {
      /* Setup whatever necessary for a resumed transfer */
      ఫలితం = అమరిక_range(దత్తాంశం);
      ఒకవేళ(!ఫలితం) {
        Curl_xfer_అమరిక_nop(దత్తాంశం);
        ఫలితం = Curl_init_transfer(దత్తాంశం, దత్తాంశం->conn);
      }
    }

    ఒకవేళ(ఫలితం) {
      DEBUGASSERT(దత్తాంశం->conn->scheme->నిరంతరం->పూర్తి);
      /* we ignore the return code for the protocol-specific DONE */
      (శూన్య)దత్తాంశం->conn->scheme->నిరంతరం->పూర్తి(దత్తాంశం, ఫలితం, FALSE);
    }
    వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  ఒకవేళ((!దత్తాంశం->సమితి.reuse_fresh || దత్తాంశం->state.followlocation) &&
     !దత్తాంశం->సమితి.connect_only) {
    /* Ok, try to find and attach an existing one */
    ఒకవేళ(url_attach_existing(దత్తాంశం, needle, &సరిపోలిక, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      నిర్మాణం connectdata *conn = దత్తాంశం->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->బిట్లు.reuse = TRUE;
      url_conn_reuse_adjust(దత్తాంశం, needle);

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
      infof(దత్తాంశం, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#లేకపోతే
      infof(దత్తాంశం, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#అయితే_ముగింపు
    }
  }

  ఒకవేళ(!దత్తాంశం->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    ఒకవేళ(సరిపోలిక.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(దత్తాంశం, "Waiting on connection to negotiate possible multiplexing.");
      ఫలితం = CURLE_NO_CONNECTION_అందుబాటు;
      వెళ్ళు బహిర్గతం;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    ఫలితం = Curl_cpool_కూడు(దత్తాంశం, needle,
                            దత్తాంశం->multi->max_మొత్తం_connections,
                            దత్తాంశం->multi->max_host_connections,
                            pnow);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(దత్తాంశం, needle, TRUE);
    needle = ఖాళీ;

    /* Clone the SSL filter configs into the new connection. */
    ఫలితం = Curl_ssl_conn_config_clone(&సరిపోలిక.ssl_config,
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
                                        &సరిపోలిక.proxy_ssl_config,
#లేకపోతే
                                        ఖాళీ,
#అయితే_ముగింపు
                                        దత్తాంశం->conn);
    ఒకవేళ(ఫలితం) {
      DEBUGF(infof(దత్తాంశం, "Error: clone connection SSL config"));
      వెళ్ళు బహిర్గతం;
    }

#నిర్వచితమైతే USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    ఒకవేళ((దత్తాంశం->state.authhost.picked & CURLAUTH_NTLM) &&
       దత్తాంశం->state.authhost.పూర్తి) {
      infof(దత్తాంశం, "NTLM picked AND auth done set, clear picked");
      దత్తాంశం->state.authhost.picked = CURLAUTH_NONE;
      దత్తాంశం->state.authhost.పూర్తి = FALSE;
    }

    ఒకవేళ((దత్తాంశం->state.authproxy.picked & CURLAUTH_NTLM) &&
       దత్తాంశం->state.authproxy.పూర్తి) {
      infof(దత్తాంశం, "NTLM-proxy picked AND auth done set, clear picked");
      దత్తాంశం->state.authproxy.picked = CURLAUTH_NONE;
      దత్తాంశం->state.authproxy.పూర్తి = FALSE;
    }
#అయితే_ముగింపు
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  ఫలితం = Curl_init_transfer(దత్తాంశం, దత్తాంశం->conn);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* Setup whatever necessary for a resumed transfer */
  ఫలితం = అమరిక_range(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* persist the scheme and handler the transfer is using */
  దత్తాంశం->info.conn_scheme = దత్తాంశం->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  దత్తాంశం->info.conn_protocol = దత్తాంశం->conn->scheme->protocol & CURLPROTO_ముసుగు;
  దత్తాంశం->info.వాడిన_proxy =
#నిర్వచితమైతే CURL_DISABLE_PROXY
    0
#లేకపోతే
    (దత్తాంశం->conn->socks_proxy.peer || దత్తాంశం->conn->http_proxy.peer)
#అయితే_ముగింపు
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  ఫలితం = Curl_conn_ev_దత్తాంశం_అమరిక(దత్తాంశం);

బహిర్గతం:
  ఒకవేళ(సరిపోలిక_initialized)
    url_సరిపోలిక_destroy(&సరిపోలిక);
  ఒకవేళ(needle)
    Curl_conn_free(దత్తాంశం, needle);
  DEBUGASSERT(ఫలితం || దత్తాంశం->conn);
  తిరిగి ఫలితం;
}

CURLcode Curl_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *pconnected)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = ఖాళీ;
  స్థిరరాశి నిర్మాణం curltime *pnow = ఖాళీ;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&దత్తాంశం->req, దత్తాంశం);
  /* Determine the origin of the transfer and what credentials to use */
  ఫలితం = url_సమితి_దత్తాంశం_origin_and_creds(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(!దత్తాంశం->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(దత్తాంశం);
  Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_POSTQUEUE, *pnow);
  ఫలితం = url_కనుగొను_or_create_conn(దత్తాంశం, pnow);
  conn = దత్తాంశం->conn;
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(!దత్తాంశం->conn) { /* just make really sure */
    DEBUGASSERT(0);
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(conn->బిట్లు.reuse) {
    ఒకవేళ(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  లేకపోతే ఒకవేళ(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  లేకపోతే {
    ఫలితం = Curl_conn_అమరిక(దత్తాంశం, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_శీర్షికలు_init(దత్తాంశం);
    CURL_TRC_M(దత్తాంశం, "Curl_conn_setup() -> %d", (పూర్ణాంకం)ఫలితం);
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం == CURLE_NO_CONNECTION_అందుబాటు)
    DEBUGASSERT(!conn);

  ఒకవేళ(ఫలితం && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(దత్తాంశం);
    Curl_conn_close(దత్తాంశం, conn, TRUE);
  }

  తిరిగి ఫలితం;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం;

  ఒకవేళ(conn) {
    conn->బిట్లు.do_మరిన్ని = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    ఒకవేళ(దత్తాంశం->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      దత్తాంశం->state.wildcardmatch = FALSE;
  }

  దత్తాంశం->state.పూర్తి = FALSE; /* *_done() is not called yet */

  దత్తాంశం->req.no_body = దత్తాంశం->సమితి.opt_no_body;
  ఒకవేళ(దత్తాంశం->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    దత్తాంశం->state.httpreq = HTTPREQ_తల;

  ఫలితం = Curl_req_ప్రారంభం(&దత్తాంశం->req, దత్తాంశం);
  ఒకవేళ(!ఫలితం) {
    Curl_pgrsReset(దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

#ఒకవేళ defined(USE_HTTP2) || defined(USE_HTTP3)

శూన్య Curl_దత్తాంశం_ప్రాధాన్యత_clear_state(నిర్మాణం Curl_easy *దత్తాంశం)
{
  దత్తాంశం->state.బరువు = 0;
}

#అయితే_ముగింపు /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_సమితి(నిర్మాణం connectdata *conn, స్థిరరాశి అక్షరం *కీలకం,
                            శూన్య *meta_దత్తాంశం, Curl_meta_dtor *meta_dtor)
{
  ఒకవేళ(!Curl_hash_add2(&conn->meta_hash, కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1,
                     meta_దత్తాంశం, meta_dtor)) {
    meta_dtor(కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1, meta_దత్తాంశం);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLE_OK;
}

శూన్య Curl_conn_meta_remove(నిర్మాణం connectdata *conn, స్థిరరాశి అక్షరం *కీలకం)
{
  Curl_hash_delete(&conn->meta_hash, కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1);
}

శూన్య *Curl_conn_meta_get(నిర్మాణం connectdata *conn, స్థిరరాశి అక్షరం *కీలకం)
{
  తిరిగి Curl_hash_pick(&conn->meta_hash, కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1);
}

నిర్మాణం Curl_easy *Curl_get_admin(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_easy *admin;

  ఒకవేళ(!దత్తాంశం->mid) /* already an admin handle */
    admin = దత్తాంశం;
  లేకపోతే ఒకవేళ(దత్తాంశం->multi)
    admin = దత్తాంశం->multi->admin;
  లేకపోతే ఒకవేళ(దత్తాంశం->multi_easy)
    admin = దత్తాంశం->multi_easy->admin;
  లేకపోతే {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = దత్తాంశం;
  }
  ఒకవేళ(admin != దత్తాంశం) {
    admin->సమితి.conn_max_idle_ms = దత్తాంశం->సమితి.conn_max_idle_ms;
    admin->సమితి.conn_max_age_ms = దత్తాంశం->సమితి.conn_max_age_ms;
    admin->సమితి.upkeep_interval_ms = దత్తాంశం->సమితి.upkeep_interval_ms;
    admin->సమితి.timeout = దత్తాంశం->సమితి.timeout;
    admin->సమితి.server_response_timeout = దత్తాంశం->సమితి.server_response_timeout;
    admin->సమితి.no_signal = దత్తాంశం->సమితి.no_signal;
  }
  తిరిగి admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  ఒకవేళ(r1 && (r1 != CURLE_AGAIN))
    తిరిగి r1;
  ఒకవేళ(r2 && (r2 != CURLE_AGAIN))
    తిరిగి r2;
  తిరిగి r1;
}
