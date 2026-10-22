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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IPHLPAPI_H
#ਅਨ੍ਤਰ੍ਭਾਵ <Iphlpapi.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_UN_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/un.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKET
#ਗਲਤੀ "We cannot compile without socket() support"
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ਜੇ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ਅਨ੍ਤਰ੍ਭਾਵ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <iphlpapi.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/vssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cookie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_digest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "getinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pop3.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "idn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "setopt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "altsvc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "headers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "peer.h"

/* Now for the protocols */
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dict.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "telnet.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ldap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/ssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "imap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gopher.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mqtt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rtsp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "smtp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#ਜੇ READBUFFER_ਮਾਪ < READBUFFER_MIN
# error READBUFFER_ਮਾਪ is too small
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ curl_prot_t get_protocol_family(ਅਚਲ ਬਣਤਰ Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  ਵਾਪਸ s->family;
}

ਖਾਲੀ Curl_freeset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  ਗਿਣਤੀ dupblob j;

  CURL_EASY_STR_CLEAR0(ਅੰਕੜੇ, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD);
  CURL_EASY_STR_CLEAR0(ਅੰਕੜੇ, STRING_BEARER);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(ਅੰਕੜੇ, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD_PROXY);
#ਜੇ_ਅੰਤ
  Curl_u8_strset_clear(&ਅੰਕੜੇ->ਸੈੱਟ.ਲੜੀਆਂ);
  curlx_safefree(ਅੰਕੜੇ->ਸੈੱਟ.str_copypostfields);

  ਲਈ(j = (ਗਿਣਤੀ dupblob)0; j < BLOB_ਆਖਰੀ; j++) {
    curlx_safefree(ਅੰਕੜੇ->ਸੈੱਟ.blobs[j]);
  }

  Curl_bufref_free(&ਅੰਕੜੇ->state.referer);
  Curl_bufref_free(&ਅੰਕੜੇ->state.url);

#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(ਅੰਕੜੇ->ਸੈੱਟ.mimepostp);
  curlx_safefree(ਅੰਕੜੇ->ਸੈੱਟ.mimepostp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  curl_slist_free_all(ਅੰਕੜੇ->state.cookielist);
  ਅੰਕੜੇ->state.cookielist = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

/* free the URL pieces */
ਸਥਿਰ ਖਾਲੀ ਉੱਪਰ_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ urlpieces *ਉੱਪਰ = &ਅੰਕੜੇ->state.ਉੱਪਰ;
  curlx_safefree(ਉੱਪਰ->options);
  curlx_safefree(ਉੱਪਰ->ਰਸਤਾ);
  curlx_safefree(ਉੱਪਰ->query);
  curl_url_cleanup(ਅੰਕੜੇ->state.uh);
  ਅੰਕੜੇ->state.uh = ਨਿਰਾ;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(ਬਣਤਰ Curl_easy **datap)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;

  ਜੇ(!datap || !*datap)
    ਵਾਪਸ CURLE_OK;

  ਅੰਕੜੇ = *datap;
  *datap = ਨਿਰਾ;

  ਜੇ(!ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ && ਅੰਕੜੇ->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(ਅੰਕੜੇ->multi, ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(ਅੰਕੜੇ);
    ਜੇ(!ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ && ਅੰਕੜੇ->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(ਅੰਕੜੇ->multi_easy);
      ਅੰਕੜੇ->multi_easy = ਨਿਰਾ;
    }
  }
  DEBUGASSERT(!ਅੰਕੜੇ->conn || ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ);

  Curl_expire_clear_all(ਅੰਕੜੇ); /* shut off any timers left */

  ਜੇ(ਅੰਕੜੇ->state.rangestringalloc)
    curlx_free(ਅੰਕੜੇ->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(ਅੰਕੜੇ);

  ਅੰਕੜੇ->ਸੈੱਟ.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  ਅੰਕੜੇ->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(ਅੰਕੜੇ);
  Curl_peer_unlink(&ਅੰਕੜੇ->state.origin);
  Curl_peer_unlink(&ਅੰਕੜੇ->state.initial_origin);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  Curl_peer_unlink(&ਅੰਕੜੇ->state.rtsp_session_origin);
#ਜੇ_ਅੰਤ
  Curl_ssl_free_certinfo(ਅੰਕੜੇ);

  Curl_bufref_free(&ਅੰਕੜੇ->state.referer);

  ਉੱਪਰ_free(ਅੰਕੜੇ);
  curlx_dyn_free(&ਅੰਕੜੇ->state.headerb);
  Curl_flush_cookies(ਅੰਕੜੇ, TRUE);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  Curl_altsvc_save(ਅੰਕੜੇ, ਅੰਕੜੇ->asi, CURL_EASY_STR(ਅੰਕੜੇ, STRING_ALTSVC));
  Curl_altsvc_cleanup(&ਅੰਕੜੇ->asi);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  Curl_hsts_save(ਅੰਕੜੇ, ਅੰਕੜੇ->hsts, CURL_EASY_STR(ਅੰਕੜੇ, STRING_HSTS));
  ਜੇ(!ਅੰਕੜੇ->share || !ਅੰਕੜੇ->share->hsts)
    Curl_hsts_cleanup(&ਅੰਕੜੇ->hsts);
  curl_slist_free_all(ਅੰਕੜੇ->state.hstslist); /* clean up list */
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
  curlx_safefree(ਅੰਕੜੇ->state.most_recent_ftp_entrypath);
  curlx_safefree(ਅੰਕੜੇ->info.contenttype);
  curlx_safefree(ਅੰਕੜੇ->info.wouldredirect);
  curlx_safefree(ਅੰਕੜੇ->info.effective_url);

  /* No longer a dirty share, if it exists */
  ਜੇ(Curl_share_easy_unlink(ਅੰਕੜੇ))
    DEBUGASSERT(0);

  Curl_hash_destroy(&ਅੰਕੜੇ->meta_hash);
  Curl_creds_unlink(&ਅੰਕੜੇ->state.creds);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  curlx_safefree(ਅੰਕੜੇ->state.rangeline);
  curlx_safefree(ਅੰਕੜੇ->state.http_host);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
  curlx_safefree(ਅੰਕੜੇ->req.cookiehost);
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(ਅੰਕੜੇ->state.formp);
  curlx_safefree(ਅੰਕੜੇ->state.formp);
#ਜੇ_ਅੰਤ

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&ਅੰਕੜੇ->wildcard);
  Curl_freeset(ਅੰਕੜੇ);
  Curl_ਸਿਰਲੇਖਾਂ_cleanup(ਅੰਕੜੇ);
  Curl_netrc_cleanup(&ਅੰਕੜੇ->state.netrc);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
  curlx_free(ਅੰਕੜੇ->state.envproxy);
#ਜੇ_ਅੰਤ
  curlx_memzero(ਅੰਕੜੇ, ਆਕਾਰ(*ਅੰਕੜੇ));
  curlx_free(ਅੰਕੜੇ);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
ਖਾਲੀ Curl_init_userdefined(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ UserDefined *ਸੈੱਟ = &ਅੰਕੜੇ->ਸੈੱਟ;

  ਸੈੱਟ->ਬਾਹਰਲਾ = stdout;  /* default output to stdout */
  ਸੈੱਟ->ਵਿੱਚ_ਸੈੱਟ = stdin;  /* default input from stdin */
  ਸੈੱਟ->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&ਅੰਕੜੇ->ਸੈੱਟ.ਲੜੀਆਂ);

#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
  /* use fwrite as default function to store output */
  ਸੈੱਟ->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  ਸੈੱਟ->fread_func_ਸੈੱਟ = (curl_read_callback)fread;
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->is_fread_ਸੈੱਟ = 0;

  ਸੈੱਟ->seek_client = ZERO_NULL;

  ਸੈੱਟ->filesize = -1;        /* we do not know the size */
  ਸੈੱਟ->postfieldsize = -1;   /* unknown size */
  ਸੈੱਟ->maxredirs = 30;       /* sensible default */

  ਸੈੱਟ->ਵਿਧੀ = HTTPREQ_GET; /* Default HTTP request */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਸੈੱਟ->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਸੈੱਟ->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  ਸੈੱਟ->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  ਸੈੱਟ->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  ਸੈੱਟ->ftp_filemethod = FTPFILE_MULTICWD;
  ਸੈੱਟ->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  ਸੈੱਟ->ssl_ca_cache_timeout = 24 * 60 * 60;

  ਸੈੱਟ->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&ਅੰਕੜੇ->ਸੈੱਟ.ssl);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  Curl_ssl_config_init(&ਅੰਕੜੇ->ਸੈੱਟ.proxy_ssl);
  ਸੈੱਟ->proxyport = 0;
  ਸੈੱਟ->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  ਸੈੱਟ->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  ਸੈੱਟ->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  ਸੈੱਟ->doh_verifyhost = TRUE;
  ਸੈੱਟ->doh_verifypeer = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  /* defaults to any auth type */
  ਸੈੱਟ->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  ਸੈੱਟ->new_directory_perms = 0755; /* Default permissions */
#ਜੇ_ਅੰਤ

  ਸੈੱਟ->new_file_perms = 0644;    /* Default permissions */
  ਸੈੱਟ->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  ਸੈੱਟ->redir_protocols = CURLPROTO_REDIR;

#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  ਸੈੱਟ->socks5_gssapi_nec = FALSE;
#ਜੇ_ਅੰਤ

  /* set default minimum TLS version */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  Curl_setopt_SSLVERSION(ਅੰਕੜੇ, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(ਅੰਕੜੇ, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਸੈੱਟ->wildcard_enabled = FALSE;
  ਸੈੱਟ->chunk_bgn = ZERO_NULL;
  ਸੈੱਟ->chunk_ਸਿਰਾ = ZERO_NULL;
  ਸੈੱਟ->fnmatch = ZERO_NULL;
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->tcp_keepalive = FALSE;
  ਸੈੱਟ->tcp_keepintvl = 60;
  ਸੈੱਟ->tcp_keepidle = 60;
  ਸੈੱਟ->tcp_keepcnt = 9;
  ਸੈੱਟ->tcp_fastopen = FALSE;
  ਸੈੱਟ->tcp_nodelay = TRUE;
  ਸੈੱਟ->ssl_enable_alpn = TRUE;
  ਸੈੱਟ->expect_100_timeout = 1000L; /* Wait for a second by default. */
  ਸੈੱਟ->sep_ਸਿਰਲੇਖਾਂ = TRUE; /* separated header lists by default */
  ਸੈੱਟ->ਬਫਰ_ਮਾਪ = READBUFFER_ਮਾਪ;
  ਸੈੱਟ->upload_ਬਫਰ_ਮਾਪ = UPLOADBUFFER_DEFAULT;
  ਸੈੱਟ->upload_flags = CURLULFLAG_ਵੇਖਿਆ;
  ਸੈੱਟ->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  ਸੈੱਟ->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  ਸੈੱਟ->maxconnects = DEFAULT_CONNCACHE_ਮਾਪ; /* for easy handles */
  ਸੈੱਟ->conn_max_idle_ms = 118 * 1000;
  ਸੈੱਟ->conn_max_age_ms = 24 * 3600 * 1000;
  ਸੈੱਟ->http09_allowed = FALSE;
  ਸੈੱਟ->httpwant = CURL_HTTP_ਸੰਸਕਰਣ_NONE;
#ਜੇ defined(USE_HTTP2) || defined(USE_HTTP3)
  ਸੈੱਟ->ਭਾਰ = 0;
#ਜੇ_ਅੰਤ
  ਸੈੱਟ->quick_exit = 0L;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  ਸੈੱਟ->ws_raw_ਬਹੁਲਕ = FALSE;
  ਸੈੱਟ->ws_no_auto_pong = FALSE;
#ਜੇ_ਅੰਤ
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ਸਥਿਰ ਖਾਲੀ easy_meta_freeentry(ਖਾਲੀ *p)
{
  (ਖਾਲੀ)p;
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
CURLcode Curl_open(ਬਣਤਰ Curl_easy **curl)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  ਅੰਕੜੇ = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_easy));
  ਜੇ(!ਅੰਕੜੇ) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਅੰਕੜੇ->magic = CURLEASY_MAGIC_ਸੰਖਿਆ;
  /* most recent connection is not yet defined */
  ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1;
  ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  ਅੰਕੜੇ->id = -1;
  ਅੰਕੜੇ->mid = UINT32_MAX;
  ਅੰਕੜੇ->master_mid = UINT32_MAX;
  ਅੰਕੜੇ->ਤਰੱਕੀ.hide = TRUE;

  Curl_hash_init(&ਅੰਕੜੇ->meta_hash, 23, CURL_HASH_TYPE_ਬਾਈਟਾਂ,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_ਆਖਰੀ <= UINT8_MAX);
  Curl_u8_strset_init(&ਅੰਕੜੇ->ਸੈੱਟ.ਲੜੀਆਂ);
  curlx_dyn_init(&ਅੰਕੜੇ->state.headerb, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  Curl_bufref_init(&ਅੰਕੜੇ->state.url);
  Curl_bufref_init(&ਅੰਕੜੇ->state.referer);
  Curl_req_init(&ਅੰਕੜੇ->req);
  Curl_initinfo(ਅੰਕੜੇ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  Curl_llist_init(&ਅੰਕੜੇ->state.httphdrs, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
  Curl_netrc_init(&ਅੰਕੜੇ->state.netrc);
  Curl_init_userdefined(ਅੰਕੜੇ);

  *curl = ਅੰਕੜੇ;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_conn_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  ਜੇ(conn->scheme && conn->scheme->ਲੜੀਵਾਰ->disconnect &&
     !conn->ਬਿੱਟ.shutdown_handler)
    conn->scheme->ਲੜੀਵਾਰ->disconnect(ਅੰਕੜੇ, conn, TRUE);

  ਲਈ(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(ਅੰਕੜੇ, conn, i);
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#ਜੇ_ਅੰਤ
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->ਰਾਹੀਂ_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->ਰਾਹੀਂ_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
ਸਥਿਰ bool xfer_may_multiplex(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ ਬਣਤਰ connectdata *conn)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  ਜੇ((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->ਬਿੱਟ.protoconnstart || !conn->ਬਿੱਟ.close)) {

    ਜੇ(Curl_multiplex_wanted(ਅੰਕੜੇ->multi) &&
       (ਅੰਕੜੇ->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      ਵਾਪਸ TRUE;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਸਥਿਰ bool proxy_info_matches(ਅਚਲ ਬਣਤਰ proxy_info *ਅੰਕੜੇ,
                               ਅਚਲ ਬਣਤਰ proxy_info *needle)
{
  ਜੇ((ਅੰਕੜੇ->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(ਅੰਕੜੇ->peer, needle->peer) &&
     Curl_creds_same(ਅੰਕੜੇ->creds, needle->creds)) {
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
ਸਥਿਰ bool ssh_config_matches(ਬਣਤਰ connectdata *one,
                               ਬਣਤਰ connectdata *two)
{
  ਬਣਤਰ ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  ਵਾਪਸ sshc1 && sshc2 && Curl_safecmp(sshc1->priv_ਕੁੰਜੀ, sshc2->priv_ਕੁੰਜੀ) &&
         Curl_safecmp(sshc1->pub_ਕੁੰਜੀ, sshc2->pub_ਕੁੰਜੀ);
}
#ਜੇ_ਅੰਤ

ਬਣਤਰ url_conn_ਮੇਲ {
  ਬਣਤਰ connectdata *ਮਿਲਿਆ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ connectdata *needle;
  ਬਣਤਰ curltime now;
  ਬਣਤਰ ssl_filter_config ssl_config;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਬਣਤਰ ssl_filter_config proxy_ssl_config;
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(may_multiplex);
  ਦੋਆਧਾਰੀ_ਅੰਕ(want_ntlm_http);
  ਦੋਆਧਾਰੀ_ਅੰਕ(want_proxy_ntlm_http);
  ਦੋਆਧਾਰੀ_ਅੰਕ(want_nego_http);
  ਦੋਆਧਾਰੀ_ਅੰਕ(want_proxy_nego_http);
  ਦੋਆਧਾਰੀ_ਅੰਕ(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(wait_pipe);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵੇਖਿਆ_pending_conn);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵੇਖਿਆ_single_use_conn);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵੇਖਿਆ_multiplex_conn);
};

ਸਥਿਰ bool url_ਮੇਲ_connect_config(ਬਣਤਰ connectdata *conn,
                                     ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  ਜੇ(conn->ਬਿੱਟ.connect_only || conn->ਬਿੱਟ.close || conn->ਬਿੱਟ.no_reuse)
    ਵਾਪਸ FALSE;

  /* ip_version must match */
  ਜੇ(m->ਅੰਕੜੇ->ਸੈੱਟ.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->ਅੰਕੜੇ->ਸੈੱਟ.ipver != conn->ip_ਸੰਸਕਰਣ)
    ਵਾਪਸ FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  ਜੇ(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    ਵਾਪਸ FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  ਜੇ((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    ਵਾਪਸ FALSE;

  ਜੇ((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    ਵਾਪਸ FALSE;

  ਜੇ(!m->needle->ਰਾਹੀਂ_peer != !conn->ਰਾਹੀਂ_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    ਵਾਪਸ FALSE;

  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_fully_connected(ਬਣਤਰ connectdata *conn,
                                      ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->ਬਿੱਟ.upgrade_ਵਿੱਚ_ਤਰੱਕੀ) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    ਜੇ(m->may_multiplex) {
      m->ਵੇਖਿਆ_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->ਅੰਕੜੇ, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_multi(ਬਣਤਰ connectdata *conn,
                            ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    ਜੇ(conn->attached_multi != m->ਅੰਕੜੇ->multi)
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_multiplex_needs(ਬਣਤਰ connectdata *conn,
                                      ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(CONN_INUSE(conn)) {
    ਜੇ(!conn->ਬਿੱਟ.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->ਵੇਖਿਆ_single_use_conn = TRUE;
      ਵਾਪਸ FALSE;
    }
    m->ਵੇਖਿਆ_multiplex_conn = TRUE;
    ਜੇ(!m->may_multiplex || !url_ਮੇਲ_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_multiplex_limits(ਬਣਤਰ connectdata *conn,
                                       ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->ਬਿੱਟ.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    ਜੇ(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->ਅੰਕੜੇ->multi)) {
      infof(m->ਅੰਕੜੇ, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      ਵਾਪਸ FALSE;
    }
    ਜੇ(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->ਅੰਕੜੇ, conn, FIRSTSOCKET)) {
      infof(m->ਅੰਕੜੇ, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      ਵਾਪਸ FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->ਅੰਕੜੇ, "Multiplexed connection found");
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_ssl_use(ਬਣਤਰ connectdata *conn,
                              ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    ਜੇ(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    ਜੇ(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      ਵਾਪਸ FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    ਜੇ(!Curl_ssl_conn_config_ਮੇਲ(m->ਅੰਕੜੇ, &m->ssl_config, conn, FALSE))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    ਵਾਪਸ FALSE;
  ਵਾਪਸ TRUE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਸਥਿਰ bool url_ਮੇਲ_proxy_use(ਬਣਤਰ connectdata *conn,
                                ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(m->needle->ਬਿੱਟ.origin_is_proxy != conn->ਬਿੱਟ.origin_is_proxy)
    ਵਾਪਸ FALSE;

  ਜੇ(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    ਵਾਪਸ FALSE;

  ਜੇ(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    ਵਾਪਸ FALSE;

  ਜੇ(m->ਅੰਕੜੇ->ਸੈੱਟ.socks5_auth_only && !conn->ਬਿੱਟ.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->ਅੰਕੜੇ,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    ਵਾਪਸ FALSE;
  }

  ਜੇ(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    ਜੇ(!Curl_ssl_conn_config_ਮੇਲ(m->ਅੰਕੜੇ, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->ਅੰਕੜੇ,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      ਵਾਪਸ FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  ਵਾਪਸ TRUE;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ url_ਮੇਲ_proxy_use(c, m) ((ਖਾਲੀ)(c), (ਖਾਲੀ)(m), TRUE)
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
ਸਥਿਰ bool url_ਮੇਲ_http_multiplex(ਬਣਤਰ connectdata *conn,
                                     ਬਣਤਰ url_conn_ਮੇਲ *m,
                                     bool *pwait_pipe)
{
  ਜੇ(m->may_multiplex &&
     (m->ਅੰਕੜੇ->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_ਵੇਖਿਆ) {
    ਜੇ(m->ਅੰਕੜੇ->ਸੈੱਟ.pipewait) {
      infof(m->ਅੰਕੜੇ, "Server upgrade does not support multiplex yet, wait");
      m->ਮਿਲਿਆ = ਨਿਰਾ;
      *pwait_pipe = TRUE;
      ਵਾਪਸ TRUE; /* stop searching, we want to wait */
    }
    infof(m->ਅੰਕੜੇ, "Server upgrade cannot be used");
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_http_ਸੰਸਕਰਣ(ਬਣਤਰ connectdata *conn,
                                   ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  ਜੇ((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    ਚੋਣ(Curl_conn_http_ਸੰਸਕਰਣ(m->ਅੰਕੜੇ, conn)) {
    ਹਾਲਤ 30:
      ਜੇ(!(m->ਅੰਕੜੇ->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->ਅੰਕੜੇ, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        ਵਾਪਸ FALSE;
      }
      ਰੋਕੋ;
    ਹਾਲਤ 20:
      ਜੇ(!(m->ਅੰਕੜੇ->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->ਅੰਕੜੇ, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        ਵਾਪਸ FALSE;
      }
      ਰੋਕੋ;
    ਮੂਲ:
      ਜੇ(!(m->ਅੰਕੜੇ->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->ਅੰਕੜੇ, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        ਵਾਪਸ FALSE;
      }
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ TRUE;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ url_ਮੇਲ_http_multiplex(c, m, w) ((ਖਾਲੀ)(c), (ਖਾਲੀ)(m), TRUE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ url_ਮੇਲ_http_ਸੰਸਕਰਣ(c, m)      ((ਖਾਲੀ)(c), (ਖਾਲੀ)(m), TRUE)
#ਜੇ_ਅੰਤ

ਸਥਿਰ bool url_ਮੇਲ_proto_config(ਬਣਤਰ connectdata *conn,
                                   ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(!url_ਮੇਲ_http_ਸੰਸਕਰਣ(conn, m))
    ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਜੇ(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    ਜੇ(!ssh_config_matches(m->needle, conn))
      ਵਾਪਸ FALSE;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਨਹੀਂਤਾਂ ਜੇ(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    ਜੇ(!Curl_ftp_conns_ਮੇਲ(m->needle, conn))
      ਵਾਪਸ FALSE;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_auth(ਬਣਤਰ connectdata *conn,
                           ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(!Curl_creds_same(m->needle->creds, conn->creds)) {
    ਜੇ(m->needle->creds)
      ਵਾਪਸ FALSE;
    ਜੇ(!Curl_creds_same(m->ਅੰਕੜੇ->state.creds, conn->creds))
      ਵਾਪਸ FALSE;
  }
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  ਜੇ(m->needle->gssapi_delegation != conn->gssapi_delegation)
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool url_ਮੇਲ_destination(ਬਣਤਰ connectdata *conn,
                                  ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  /* Different connect-to peers never match */
  ਜੇ(!Curl_peer_same_destination(m->needle->ਰਾਹੀਂ_peer, conn->ਰਾਹੀਂ_peer))
    ਵਾਪਸ FALSE;

  ਜੇ(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    ਵਾਪਸ FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  ਵਾਪਸ Curl_peer_same_destination(m->needle->origin, conn->origin);
}

ਸਥਿਰ bool url_ਮੇਲ_ssl_config(ਬਣਤਰ connectdata *conn,
                                 ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  ਜੇ(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_ਮੇਲ(m->ਅੰਕੜੇ, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->ਅੰਕੜੇ, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
ਸਥਿਰ bool url_ਮੇਲ_auth_ntlm(ਬਣਤਰ connectdata *conn,
                                ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    ਜੇ(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->ਅੰਕੜੇ->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->ਅੰਕੜੇ->state.origin))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    ਜੇ(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->ਅੰਕੜੇ->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->ਅੰਕੜੇ->state.origin)))
      ਵਾਪਸ FALSE;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  ਜੇ(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    ਜੇ(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(m->want_proxy_ntlm_http) {
    ਜੇ(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ਵਾਪਸ FALSE;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ TRUE;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ url_ਮੇਲ_auth_ntlm(c, m) ((ਖਾਲੀ)(c), (ਖਾਲੀ)(m), TRUE)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
ਸਥਿਰ bool url_ਮੇਲ_auth_nego(ਬਣਤਰ connectdata *conn,
                                ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  ਜੇ(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    ਜੇ(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->ਅੰਕੜੇ->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->ਅੰਕੜੇ->state.origin))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    ਜੇ(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->ਅੰਕੜੇ->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->ਅੰਕੜੇ->state.origin)))
      ਵਾਪਸ FALSE;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  ਜੇ(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    ਜੇ(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(m->want_proxy_nego_http) {
    ਜੇ(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ਵਾਪਸ FALSE;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ TRUE;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ url_ਮੇਲ_auth_nego(c, m) ((ਖਾਲੀ)(c), (ਖਾਲੀ)(m), TRUE)
#ਜੇ_ਅੰਤ

ਸਥਿਰ cpool_ਮੇਲ_ਨਤੀਜਾ url_ਮੇਲ_conn(ਬਣਤਰ connectdata *conn,
                                         ਖਾਲੀ *userdata)
{
  ਬਣਤਰ url_conn_ਮੇਲ *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  ਜੇ(!url_ਮੇਲ_connect_config(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  /* match for destination and protocol? */
  ਜੇ(!url_ਮੇਲ_destination(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_fully_connected(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_multiplex_needs(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_ssl_use(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_proxy_use(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;
  ਜੇ(!url_ਮੇਲ_ssl_config(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_http_multiplex(conn, m, &wait_pipe))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_auth(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_proto_config(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_auth_ntlm(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_auth_nego(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  ਜੇ(!url_ਮੇਲ_multiplex_limits(conn, m))
    ਵਾਪਸ CPOOL_ਮੇਲ_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  ਜੇ(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->ਮਿਲਿਆ);
    m->wait_pipe = TRUE;
    ਵਾਪਸ CPOOL_ਮੇਲ_ਮਿਲਿਆ;
  }

  ਜੇ(m->ਅੰਕੜੇ->ਸੈੱਟ.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->ਅੰਕੜੇ, conn, &m->now);
    ਜੇ(age_ms > m->ਅੰਕੜੇ->ਸੈੱਟ.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      ਵਾਪਸ CPOOL_ਮੇਲ_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  ਜੇ(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->ਅੰਕੜੇ, &m->now)) {
    infof(m->ਅੰਕੜੇ, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    ਵਾਪਸ CPOOL_ਮੇਲ_CLOSE;
  }

  /* conn matches our needs. */
  m->ਮਿਲਿਆ = conn;
  ਵਾਪਸ CPOOL_ਮੇਲ_ਮਿਲਿਆ;
}

ਸਥਿਰ bool url_ਮੇਲ_ਨਤੀਜਾ(ਖਾਲੀ *userdata)
{
  ਬਣਤਰ url_conn_ਮੇਲ *ਮੇਲ = userdata;
  ਜੇ(ਮੇਲ->ਮਿਲਿਆ) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(ਮੇਲ->ਅੰਕੜੇ, ਮੇਲ->ਮਿਲਿਆ, TRUE);
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ->ਵੇਖਿਆ_single_use_conn && !ਮੇਲ->ਵੇਖਿਆ_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    ਮੇਲ->wait_pipe = FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਮੇਲ->ਵੇਖਿਆ_pending_conn && ਮੇਲ->ਅੰਕੜੇ->ਸੈੱਟ.pipewait) {
    infof(ਮੇਲ->ਅੰਕੜੇ,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    ਮੇਲ->wait_pipe = TRUE;
  }
  ਵਾਪਸ FALSE;
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
ਸਥਿਰ bool url_attach_existing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *needle,
                                ਬਣਤਰ url_conn_ਮੇਲ *m,
                                ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  ਵਾਪਸ Curl_cpool_ਲੱਭੋ(ਅੰਕੜੇ, needle->destination, TRUE, pnow,
                         url_ਮੇਲ_conn, url_ਮੇਲ_ਨਤੀਜਾ, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
ਸਥਿਰ ਬਣਤਰ connectdata *allocate_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ connectdata));
  ਜੇ(!conn)
    ਵਾਪਸ ਨਿਰਾ;

  /* and we setup a few fields in case we end up actually using this struct */
  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->recv_idx = 0; /* default for receiving transfer data */
  conn->send_idx = 0; /* default for sending transfer data */
  conn->connection_id = -1;    /* no ID */
  conn->cpid = UINT32_MAX;  /* not in connection pool yet */
  conn->attached_xfers = 0;
  conn->shutdown.ਸ਼ੁਰੂ_ms[FIRSTSOCKET] =
    conn->shutdown.ਸ਼ੁਰੂ_ms[SECONDARYSOCKET] = -1;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  conn->ਬਿੱਟ.ftp_use_epsv = ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_epsv;
  conn->ਬਿੱਟ.ftp_use_eprt = ਅੰਕੜੇ->ਸੈੱਟ.ftp_use_eprt;
#ਜੇ_ਅੰਤ
  conn->ip_ਸੰਸਕਰਣ = ਅੰਕੜੇ->ਸੈੱਟ.ipver;
  conn->ਬਿੱਟ.connect_only = (bool)ਅੰਕੜੇ->ਸੈੱਟ.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(ਅੰਕੜੇ, STRING_INTERFACE_OPT));
    ਜੇ(!conn->interface_opts)
      ਜਾਓ error;
  }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_BINDLOCAL
  conn->localportrange = ਅੰਕੜੇ->ਸੈੱਟ.localportrange;
  conn->localport = ਅੰਕੜੇ->ਸੈੱਟ.localport;
#ਜੇ_ਅੰਤ

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = ਅੰਕੜੇ->ਸੈੱਟ.fclosesocket;
  conn->closesocket_client = ਅੰਕੜੇ->ਸੈੱਟ.closesocket_client;
#ਜੇ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = ਅੰਕੜੇ->ਸੈੱਟ.gssapi_delegation;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ CURLcode url_ਸੈੱਟ_conn_scheme(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ਅਚਲ ਬਣਤਰ Curl_scheme *scheme = conn->origin->scheme;
  ਜੇ(scheme->ਲੜੀਵਾਰ &&
     (ਅੰਕੜੇ->ਸੈੱਟ.allowed_protocols & scheme->protocol) &&
     (!ਅੰਕੜੇ->state.this_is_a_follow ||
       (ਅੰਕੜੇ->ਸੈੱਟ.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(ਅੰਕੜੇ, "Protocol \"%s\" is not for transfers", scheme->name);
  ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "Protocol \"%s\" is disabled%s", scheme->name,
          ਅੰਕੜੇ->state.this_is_a_follow ? " (in redirect)" : "");
  ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_ਮੰਜ਼ਿਲ_curlcode(CURLUcode uc)
{
  ਚੋਣ(uc) {
  ਮੂਲ:
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  ਹਾਲਤ CURLUE_UNSUPPORTED_SCHEME:
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  ਹਾਲਤ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਹਾਲਤ CURLUE_USER_NOT_ALLOWED:
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
ਸਥਿਰ CURLcode hsts_upgrade(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             CURLU *uh,
                             uint16_t ਪੋਰਟ_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  ਜੇ(ਅੰਕੜੇ->hsts && (ਅੰਕੜੇ->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(ਅੰਕੜੇ->hsts, ਅੰਕੜੇ->state.origin)) {
    ਅੱਖਰ *url;
    CURLUcode uc;
    CURLcode ਨਤੀਜਾ;

    uc = curl_url_ਸੈੱਟ(uh, CURLUPART_SCHEME, "https", 0);
    ਜੇ(uc)
      ਵਾਪਸ Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    Curl_bufref_free(&ਅੰਕੜੇ->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    ਜੇ(uc)
      ਵਾਪਸ Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.url, url, 0, curl_free);

    ਨਤੀਜਾ = Curl_peer_from_url(uh, ਅੰਕੜੇ, ਪੋਰਟ_override, scope_id,
                                &ਅੰਕੜੇ->state.origin);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    infof(ਅੰਕੜੇ, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  ਵਾਪਸ CURLE_OK;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hsts_upgrade(x, y, z, a) CURLE_OK
#ਜੇ_ਅੰਤ

ਸਥਿਰ bool str_has_ctrl(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼)
{
  ਜੇ(ਨਿਵੇਸ਼) {
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *str = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਨਿਵੇਸ਼;
    ਜਦੋਂ(*str) {
      ਜੇ(*str < 0x20)
        ਵਾਪਸ TRUE;
      str++;
    }
  }
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ਸਥਿਰ CURLcode url_ਸੈੱਟ_ਅੰਕੜੇ_creds_netrc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ Curl_creds **pcreds)
{
  ਬਣਤਰ Curl_creds *ncreds_ਬਾਹਰਲਾ = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_netrc) { /* not CURL_NETRC_IGNORED */
    ਬਣਤਰ Curl_creds *ncreds_ਵਿੱਚ = ਨਿਰਾ;
    bool ਜਾਂਚ_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    ਜੇ(*pcreds) {
      ਚੋਣ((*pcreds)->ਸਰੋਤ) {
      ਹਾਲਤ CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        ਜਾਂਚ_netrc = FALSE;
        ਰੋਕੋ;
      ਹਾਲਤ CREDS_URL: /* only apply when netrc is not required */
        ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_ਵਿੱਚ = *pcreds;
        }
        ਨਹੀਂਤਾਂ ਜੇ(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_ਵਿੱਚ = *pcreds;
        }
        ਨਹੀਂਤਾਂ
          ਜਾਂਚ_netrc = FALSE;
        ਰੋਕੋ;
      ਮੂਲ: /* ignore credentials from other sources */
        ਰੋਕੋ;
      }
    }

    ਜੇ(!ਜਾਂਚ_netrc)
      ਜਾਓ ਬਾਹਰਲਾ;

    ret = Curl_netrc_ਜਾਂਚ(ਅੰਕੜੇ, &ਅੰਕੜੇ->state.netrc,
                          ਅੰਕੜੇ->state.origin->hostname,
                          Curl_creds_user(ncreds_ਵਿੱਚ),
                          CURL_EASY_STR(ਅੰਕੜੇ, STRING_NETRC_FILE),
                          &ncreds_ਬਾਹਰਲਾ);
    DEBUGASSERT(!ret || !ncreds_ਬਾਹਰਲਾ);
    ਜੇ(ret == NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ret && ((ret == NETRC_NO_ਮੇਲ) ||
                    (ਅੰਕੜੇ->ਸੈੱਟ.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(ਅੰਕੜੇ, "Could not find host %s in the %s file; using defaults",
            ਅੰਕੜੇ->state.origin->hostname,
            (CURL_EASY_STR(ਅੰਕੜੇ, STRING_NETRC_FILE) ?
             CURL_EASY_STR(ਅੰਕੜੇ, STRING_NETRC_FILE) : ".netrc"));
    }
    ਨਹੀਂਤਾਂ ਜੇ(ret) {
      ਅਚਲ ਅੱਖਰ *m = Curl_netrc_strerror(ret);
      failf(ਅੰਕੜੇ, ".netrc error: %s", m);
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ncreds_ਬਾਹਰਲਾ) {
      ਜੇ(!(ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_ਬਾਹਰਲਾ->user) ||
          str_has_ctrl(ncreds_ਬਾਹਰਲਾ->passwd))) {
        failf(ਅੰਕੜੇ, "control code detected in .netrc credentials");
        ਨਤੀਜਾ = CURLE_READ_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      CURL_TRC_M(ਅੰਕੜੇ, "netrc: using credentials for %s as %s",
                 ਅੰਕੜੇ->state.origin->hostname, ncreds_ਬਾਹਰਲਾ->user);
      ਨਤੀਜਾ = Curl_creds_ਮਿਲਾਓ(ncreds_ਬਾਹਰਲਾ->user, ncreds_ਬਾਹਰਲਾ->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      /* for updated strings, we update them in the URL */
      uc = curl_url_ਸੈੱਟ(ਅੰਕੜੇ->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      ਜੇ(!uc)
        uc = curl_url_ਸੈੱਟ(ਅੰਕੜੇ->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      ਜੇ(uc)
        ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    }
    ਨਹੀਂਤਾਂ
      DEBUGASSERT(0);
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  Curl_creds_trace(ਅੰਕੜੇ, ਅੰਕੜੇ->state.creds, "transfer credentials");
#ਜੇ_ਅੰਤ

ਬਾਹਰਲਾ:
  Curl_creds_unlink(&ncreds_ਬਾਹਰਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* CURL_DISABLE_NETRC */

ਸਥਿਰ CURLcode url_ਸੈੱਟ_ਅੰਕੜੇ_creds(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLU *uh)
{
  ਬਣਤਰ Curl_creds *newcreds = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ((CURL_EASY_STR(ਅੰਕੜੇ, STRING_USERNAME) ||
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_PASSWORD) ||
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_BEARER) ||
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_ਮੰਜ਼ਿਲ_origin(ਅੰਕੜੇ, ਅੰਕੜੇ->state.origin)) {
    ਨਤੀਜਾ = Curl_creds_create(CURL_EASY_STR(ਅੰਕੜੇ, STRING_USERNAME),
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_PASSWORD),
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_BEARER),
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(newcreds &&
       !(ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(ਅੰਕੜੇ, "control code detected in credentials");
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  ਜੇ(!newcreds || !Curl_creds_has_user(newcreds)) {
    ਅੱਖਰ *user = ਨਿਰਾ;
    ਅੱਖਰ *passwd = ਨਿਰਾ;
    ਅੱਖਰ *udecoded = ਨਿਰਾ;
    ਅੱਖਰ *pdecoded = ਨਿਰਾ;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    ਜੇ(uc && (uc != CURLUE_NO_USER))
      ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    ਜੇ(!ਨਤੀਜਾ) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      ਜੇ(uc && (uc != CURLUE_NO_PASSWORD))
        ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    }
    ਜੇ(!ਨਤੀਜਾ && user) {
      ਨਤੀਜਾ = Curl_urldecode(user, 0, &udecoded, ਨਿਰਾ,
                              (ਅੰਕੜੇ->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    ਜੇ(!ਨਤੀਜਾ && passwd) {
      ਨਤੀਜਾ = Curl_urldecode(passwd, 0, &pdecoded, ਨਿਰਾ,
                              (ਅੰਕੜੇ->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_creds_ਮਿਲਾਓ(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "error extracting credentials from URL");
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  ਨਤੀਜਾ = url_ਸੈੱਟ_ਅੰਕੜੇ_creds_netrc(ਅੰਕੜੇ, &newcreds);
#ਜੇ_ਅੰਤ /* CURL_DISABLE_NETRC */

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ && !Curl_creds_equal(ਅੰਕੜੇ->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&ਅੰਕੜੇ->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode url_ਸੈੱਟ_conn_origin_etc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_peer_link(&conn->origin, ਅੰਕੜੇ->state.origin);

  /* set the connection scheme */
  ਨਤੀਜਾ = url_ਸੈੱਟ_conn_scheme(ਅੰਕੜੇ, conn);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* set the connection options */
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(ਅੰਕੜੇ, STRING_OPTIONS));
    ਜੇ(!conn->options) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.ਉੱਪਰ.options) {
    conn->options = curlx_strdup(ਅੰਕੜੇ->state.ਉੱਪਰ.options);
    ਜੇ(!conn->options) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ਸਥਿਰ CURLcode ਤਿਆਰੀ_range(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ UrlState *s = &ਅੰਕੜੇ->state;
  s->resume_from = ਅੰਕੜੇ->ਸੈੱਟ.ਸੈੱਟ_resume_from;
  ਜੇ(s->resume_from || CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_RANGE)) {
    ਜੇ(s->rangestringalloc)
      curlx_free(s->range);

    ਜੇ(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    ਨਹੀਂਤਾਂ
      s->range = curlx_strdup(CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਸੈੱਟ_RANGE));

    ਜੇ(!s->range)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  ਨਹੀਂਤਾਂ
    s->use_range = FALSE; /* disable range download */

  ਵਾਪਸ CURLE_OK;
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
ਸਥਿਰ CURLcode ਤਿਆਰੀ_connection_internals(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_peer *peer = ਨਿਰਾ;

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->ਤਿਆਰੀ_connection) {
    CURLcode ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->ਤਿਆਰੀ_connection(ਅੰਕੜੇ, conn);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  ਜੇ(!peer)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  ਜੇ(peer->ipv6)
    ਜੇ(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->ਪੋਰਟ);
    ਨਹੀਂਤਾਂ
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->ਪੋਰਟ);
  ਨਹੀਂਤਾਂ
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->ਪੋਰਟ);
  ਜੇ(!conn->destination)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  Curl_strntolower(conn->destination, conn->destination,
                   ਲੜੀ_ਲੰਬਾਈ(conn->destination));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.scope_id)
    conn->scope_id = ਅੰਕੜੇ->ਸੈੱਟ.scope_id;
  ਨਹੀਂਤਾਂ {
    peer = Curl_conn_get_ਪਹਿਲਾ_peer(conn, FIRSTSOCKET);
    ਜੇ(!peer)
      ਵਾਪਸ CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_parse_login_details(ਅਚਲ ਅੱਖਰ *login, ਅਚਲ size_t len,
                                  ਅੱਖਰ **userp, ਅੱਖਰ **passwdp,
                                  ਅੱਖਰ **optionsp)
{
  ਅੱਖਰ *ubuf = ਨਿਰਾ;
  ਅੱਖਰ *pbuf = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *psep = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *osep = ਨਿਰਾ;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  ਜੇ(optionsp)
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
  ਜੇ(!ubuf)
    ਜਾਓ error;

  /* Clone the password portion buffer */
  ਜੇ(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    ਜੇ(!pbuf)
      ਜਾਓ error;
  }

  /* Allocate the options portion buffer */
  ਜੇ(optionsp) {
    ਅੱਖਰ *obuf = ਨਿਰਾ;
    ਜੇ(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      ਜੇ(!obuf)
        ਜਾਓ error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  ਵਾਪਸ CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

/*
 * Set the login details so they are available in the connection
 */
ਸਥਿਰ CURLcode url_ਸੈੱਟ_conn_login(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  ਜੇ((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, ਅੰਕੜੇ->state.origin);
    ਜੇ(ਅੰਕੜੇ->state.creds)
      Curl_creds_link(&conn->creds, ਅੰਕੜੇ->state.creds);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, CREDS_NONE, &conn->creds);
  }
  ਨਹੀਂਤਾਂ ਜੇ(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, ਅੰਕੜੇ->state.origin);
    Curl_creds_link(&conn->creds, ਅੰਕੜੇ->state.creds);
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
ਸਥਿਰ CURLcode parse_connect_ਮੰਜ਼ਿਲ_string(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਅਚਲ ਬਣਤਰ Curl_peer *dest,
                                        ਅਚਲ ਅੱਖਰ *conn_ਮੰਜ਼ਿਲ_ਸਤਰ,
                                        ਬਣਤਰ Curl_peer **pvia_dest)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ptr = conn_ਮੰਜ਼ਿਲ_ਸਤਰ;
  bool host_ਮੇਲ = FALSE;
  bool ਪੋਰਟ_ਮੇਲ = FALSE;

  *pvia_dest = ਨਿਰਾ;

  ਜੇ(*ptr == ':') {
    /* an empty hostname always matches */
    host_ਮੇਲ = TRUE;
    ptr++;
  }
  ਨਹੀਂਤਾਂ {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = ਲੜੀ_ਲੰਬਾਈ(dest->hostname);
    host_ਮੇਲ = curl_strnequal(ptr, dest->hostname, hlen);
    ਜੇ(!host_ਮੇਲ && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = ਲੜੀ_ਲੰਬਾਈ(dest->user_hostname);
      host_ਮੇਲ = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_ਮੇਲ = host_ਮੇਲ && ptr[hlen] == ':';
    ਜੇ(host_ਮੇਲ)
      ptr += hlen + 1;
  }

  ਜੇ(host_ਮੇਲ) {
    ਜੇ(*ptr == ':') {
      /* an empty port always matches */
      ਪੋਰਟ_ਮੇਲ = TRUE;
      ptr++;
    }
    ਨਹੀਂਤਾਂ {
      /* check whether the URL's port matches */
      ਅਚਲ ਅੱਖਰ *ptr_ਅਗਲਾ = strchr(ptr, ':');
      ਜੇ(ptr_ਅਗਲਾ) {
        curl_off_t ਪੋਰਟ_ਮੰਜ਼ਿਲ_ਮੇਲ;
        ਜੇ(!curlx_str_ਸੰਖਿਆ(&ptr, &ਪੋਰਟ_ਮੰਜ਼ਿਲ_ਮੇਲ, 0xffff) &&
           ((uint16_t)ਪੋਰਟ_ਮੰਜ਼ਿਲ_ਮੇਲ == dest->ਪੋਰਟ)) {
          ਪੋਰਟ_ਮੇਲ = TRUE;
        }
        ptr = ptr_ਅਗਲਾ + 1;
      }
    }
  }

  ਜੇ(host_ਮੇਲ && ਪੋਰਟ_ਮੇਲ && ptr && *ptr)
    ਨਤੀਜਾ = Curl_peer_from_connect_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, dest, ptr, pvia_dest);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ਸਥਿਰ CURLcode url_ਸੈੱਟ_conn_peer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ Curl_peer *origin = conn->origin;
  ਬਣਤਰ Curl_peer *ਰਾਹੀਂ_peer = ਨਿਰਾ;
  ਬਣਤਰ curl_slist *conn_ਮੰਜ਼ਿਲ_ਇੰਦਰਾਜ = ਅੰਕੜੇ->ਸੈੱਟ.connect_ਮੰਜ਼ਿਲ;

  DEBUGASSERT(!conn->ਰਾਹੀਂ_peer);
  Curl_peer_unlink(&conn->ਰਾਹੀਂ_peer);

  ਜਦੋਂ(conn_ਮੰਜ਼ਿਲ_ਇੰਦਰਾਜ && !ਰਾਹੀਂ_peer) {
    ਨਤੀਜਾ = parse_connect_ਮੰਜ਼ਿਲ_string(ਅੰਕੜੇ, origin, conn_ਮੰਜ਼ਿਲ_ਇੰਦਰਾਜ->ਅੰਕੜੇ,
                                     &ਰਾਹੀਂ_peer);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    conn_ਮੰਜ਼ਿਲ_ਇੰਦਰਾਜ = conn_ਮੰਜ਼ਿਲ_ਇੰਦਰਾਜ->ਅਗਲਾ;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  ਜੇ(ਅੰਕੜੇ->asi && !ਰਾਹੀਂ_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#ਨਹੀਂਤਾਂ
      0
#ਜੇ_ਅੰਤ
       )) {
    /* no connect_to match, try alt-svc! */
    ਗਿਣਤੀ alpnid srcalpnid = ALPN_none;
    bool ਪ੍ਰਾਪਤੀ = FALSE;
    ਬਣਤਰ altsvc *as = ਨਿਰਾ;
    ਪੂਰਨ allowed_alpns = ALPN_none;
    ਬਣਤਰ http_negotiation *neg = &ਅੰਕੜੇ->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(ਅੰਕੜੇ, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP3
    ਜੇ(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
    ਜੇ(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#ਜੇ_ਅੰਤ
    ਜੇ(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (ਪੂਰਨ)ਅੰਕੜੇ->asi->flags;

    DEBUGF(infof(ਅੰਕੜੇ, "check Alt-Svc for host '%s'", origin->hostname));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP3
    ਜੇ(!ਪ੍ਰਾਪਤੀ && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      ਪ੍ਰਾਪਤੀ = Curl_altsvc_lookup(ਅੰਕੜੇ->asi,
                               origin, ALPN_h3, /* from */
                               &as /*ਮੰਜ਼ਿਲ*/,
                               allowed_alpns, &same_dest);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
    ਜੇ(!ਪ੍ਰਾਪਤੀ && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_ਪੂਰਵ_knowledge) {
      srcalpnid = ALPN_h2;
      ਪ੍ਰਾਪਤੀ = Curl_altsvc_lookup(ਅੰਕੜੇ->asi,
                               origin, ALPN_h2, /* from */
                               &as /*ਮੰਜ਼ਿਲ*/,
                               allowed_alpns, &same_dest);
    }
#ਜੇ_ਅੰਤ
    ਜੇ(!ਪ੍ਰਾਪਤੀ && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      ਪ੍ਰਾਪਤੀ = Curl_altsvc_lookup(ਅੰਕੜੇ->asi,
                               origin, ALPN_h1, /* from */
                               &as /*ਮੰਜ਼ਿਲ*/,
                               allowed_alpns, &same_dest);
    }

    ਜੇ(ਪ੍ਰਾਪਤੀ && same_dest) {
      /* same destination, but more HTTPS version options */
      ਚੋਣ(as->dst.alpnid) {
      ਹਾਲਤ ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        ਰੋਕੋ;
      ਹਾਲਤ ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        ਰੋਕੋ;
      ਹਾਲਤ ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        ਰੋਕੋ;
      ਮੂਲ: /* should not be possible */
        ਰੋਕੋ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਪ੍ਰਾਪਤੀ) {
      ਨਤੀਜਾ = Curl_peer_create(ਅੰਕੜੇ, conn->origin->scheme,
                                as->dst.host, as->dst.ਪੋਰਟ,
                                &ਰਾਹੀਂ_peer);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      infof(ਅੰਕੜੇ, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->ਪੋਰਟ,
            Curl_alpnid2str(as->dst.alpnid),
            ਰਾਹੀਂ_peer->hostname, ਰਾਹੀਂ_peer->ਪੋਰਟ);
      conn->ਬਿੱਟ.altused = TRUE;
      ਜੇ(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        ਚੋਣ(as->dst.alpnid) {
        ਹਾਲਤ ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          ਰੋਕੋ;
        ਹਾਲਤ ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          ਰੋਕੋ;
        ਹਾਲਤ ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          ਰੋਕੋ;
        ਮੂਲ: /* should not be possible */
          ਰੋਕੋ;
        }
      }
    }
  }
#ਜੇ_ਅੰਤ

  ਜੇ(ਰਾਹੀਂ_peer)
    conn->ਰਾਹੀਂ_peer = ਰਾਹੀਂ_peer;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
ਸਥਿਰ ਖਾਲੀ url_conn_reuse_adjust(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *needle)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  ਜੇ(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#ਜੇ_ਅੰਤ

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
  Curl_peer_link(&conn->ਰਾਹੀਂ_peer, needle->ਰਾਹੀਂ_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->ਰਾਹੀਂ_peer2, needle->ਰਾਹੀਂ_peer2);
}

ਸਥਿਰ ਖਾਲੀ conn_meta_freeentry(ਖਾਲੀ *p)
{
  (ਖਾਲੀ)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

ਸਥਿਰ CURLcode url_create_needle(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata **pneedle)
{
  ਬਣਤਰ connectdata *needle = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool ਜਾਲ_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(ਅੰਕੜੇ);
  ਜੇ(!needle) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_ਬਾਈਟਾਂ,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  ਨਤੀਜਾ = url_ਸੈੱਟ_conn_origin_etc(ਅੰਕੜੇ, needle);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  DEBUGASSERT(needle->origin);
  ਜਾਲ_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  ਜੇ(ਜਾਲ_scheme && CURL_EASY_STR(ਅੰਕੜੇ, STRING_UNIX_SOCKET_ਰਸਤਾ)) {
    ਨਤੀਜਾ = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(ਅੰਕੜੇ, STRING_UNIX_SOCKET_ਰਸਤਾ),
      (bool)ਅੰਕੜੇ->ਸੈੱਟ.abstract_unix_socket, &needle->ਰਾਹੀਂ_peer);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ /* USE_UNIX_SOCKETS */

  ਜੇ(ਜਾਲ_scheme && !needle->ਰਾਹੀਂ_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    ਨਤੀਜਾ = url_ਸੈੱਟ_conn_peer(ਅੰਕੜੇ, needle);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  ਜੇ(Curl_peer_equal(needle->origin, needle->ਰਾਹੀਂ_peer)) {
    Curl_peer_unlink(&needle->ਰਾਹੀਂ_peer);
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  ਜੇ(ਜਾਲ_scheme &&
     (!needle->ਰਾਹੀਂ_peer || !needle->ਰਾਹੀਂ_peer->unix_socket)) {
    ਨਤੀਜਾ = Curl_proxy_init_conn(ਅੰਕੜੇ, needle);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */

  ਨਤੀਜਾ = url_ਸੈੱਟ_conn_login(ਅੰਕੜੇ, needle); /* default credentials */
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  ਨਤੀਜਾ = ਤਿਆਰੀ_connection_internals(ਅੰਕੜੇ, needle);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl_enable_alpn)
      needle->ਬਿੱਟ.tls_enable_alpn = TRUE;
  }

  ਜੇ(ਜਾਲ_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->ਬਿੱਟ.tcp_fastopen = ਅੰਕੜੇ->ਸੈੱਟ.tcp_fastopen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
    ਜੇ(Curl_conn_get_ਪਹਿਲਾ_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#ਜੇ_ਅੰਤ
  }

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  ਨਹੀਂਤਾਂ {
    *pneedle = ਨਿਰਾ;
    ਜੇ(needle)
      Curl_conn_free(ਅੰਕੜੇ, needle);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode url_ਸੈੱਟ_ਅੰਕੜੇ_origin_and_creds(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_ਸੈੱਟ_uh = (ਅੰਕੜੇ->ਸੈੱਟ.uh && !ਅੰਕੜੇ->state.this_is_a_follow);
  uint16_t ਪੋਰਟ_override = ਅੰਕੜੇ->state.allow_ਪੋਰਟ ? ਅੰਕੜੇ->ਸੈੱਟ.use_ਪੋਰਟ : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  ਜੇ(!Curl_bufref_ptr(&ਅੰਕੜੇ->state.url)) {
    ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਉੱਪਰ_free(ਅੰਕੜੇ); /* cleanup previous leftovers first */

  /* parse the URL */
  ਜੇ(use_ਸੈੱਟ_uh)
    uh = ਅੰਕੜੇ->state.uh = curl_url_dup(ਅੰਕੜੇ->ਸੈੱਟ.uh);
  ਨਹੀਂਤਾਂ
    uh = ਅੰਕੜੇ->state.uh = curl_url();
  ਜੇ(!uh) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(!use_ਸੈੱਟ_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    ਜੇ(curl_strequal("file", CURL_EASY_STR(ਅੰਕੜੇ, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&ਅੰਕੜੇ->state.url), ਨਿਰਾ, 0, TRUE))
      prepend_sheme = TRUE;
    ਨਹੀਂਤਾਂ {
      uc = curl_url_ਸੈੱਟ(uh, CURLUPART_URL, Curl_bufref_ptr(&ਅੰਕੜੇ->state.url),
                        (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (ਅੰਕੜੇ->ਸੈੱਟ.disallow_username_ਵਿੱਚ_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (ਅੰਕੜੇ->ਸੈੱਟ.ਰਸਤਾ_as_is ? CURLU_ਰਸਤਾ_AS_IS : 0));
      ਜੇ(uc) {
        failf(ਅੰਕੜੇ, "URL rejected: %s", curl_url_strerror(uc));
        ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
        ਜਾਓ ਬਾਹਰਲਾ;
      }

      ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        ਅੱਖਰ *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        ਜੇ(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        ਜੇ(uc && uc != CURLUE_NO_SCHEME) {
          ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
          ਜਾਓ ਬਾਹਰਲਾ;
        }
      }
    }

    ਜੇ(prepend_sheme) {
      ਅੱਖਰ *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(ਅੰਕੜੇ, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));
      ਜੇ(!url) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      uc = curl_url_ਸੈੱਟ(uh, CURLUPART_URL, url,
                        (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (ਅੰਕੜੇ->ਸੈੱਟ.disallow_username_ਵਿੱਚ_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (ਅੰਕੜੇ->ਸੈੱਟ.ਰਸਤਾ_as_is ? CURLU_ਰਸਤਾ_AS_IS : 0));
      curl_free(url);
      ਜੇ(uc) {
        failf(ਅੰਕੜੇ, "URL rejected: %s", curl_url_strerror(uc));
        ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    {
      ਅੱਖਰ *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ);
      ਜੇ(uc) {
        ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(prepend_sheme)
        infof(ਅੰਕੜੇ, "Applied default protocol '%s'",
              CURL_EASY_STR(ਅੰਕੜੇ, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_ਸੈੱਟ(&ਅੰਕੜੇ->state.url, newurl, 0, curl_free);
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  scope_id = ਅੰਕੜੇ->ਸੈੱਟ.scope_id;
#ਜੇ_ਅੰਤ

  /* `uh` is now as the connection should use it, probably. */
  ਨਤੀਜਾ = Curl_peer_from_url(uh, ਅੰਕੜੇ, ਪੋਰਟ_override, scope_id,
                              &ਅੰਕੜੇ->state.origin);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  /* The origin might get changed when HSTS applies */
  ਨਤੀਜਾ = hsts_upgrade(ਅੰਕੜੇ, uh, ਪੋਰਟ_override, scope_id);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  ਜੇ(!ਅੰਕੜੇ->state.initial_origin)
    Curl_peer_link(&ਅੰਕੜੇ->state.initial_origin, ਅੰਕੜੇ->state.origin);

  uc = curl_url_get(uh, CURLUPART_ਰਸਤਾ, &ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ, CURLU_URLENCODE);
  ਜੇ(uc) {
    ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &ਅੰਕੜੇ->state.ਉੱਪਰ.query,
                    CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ);
  ਜੇ(uc && (uc != CURLUE_NO_QUERY)) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &ਅੰਕੜੇ->state.ਉੱਪਰ.options,
                    CURLU_URLDECODE);
  ਜੇ(uc && (uc != CURLUE_NO_OPTIONS)) {
    ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = url_ਸੈੱਟ_ਅੰਕੜੇ_creds(ਅੰਕੜੇ, uh);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode url_ਮੇਲ_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *needle,
                               ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  memset(m, 0, ਆਕਾਰ(*m));
  m->ਅੰਕੜੇ = ਅੰਕੜੇ;
  m->needle = needle;
  m->now = *Curl_pgrs_now(ਅੰਕੜੇ);
  m->may_multiplex = xfer_may_multiplex(ਅੰਕੜੇ, needle);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  m->want_ntlm_http =
    (ਅੰਕੜੇ->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (ਅੰਕੜੇ->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (ਅੰਕੜੇ->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_ਮੰਜ਼ਿਲ_host(ਅੰਕੜੇ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (ਅੰਕੜੇ->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
  m->require_tls = ਅੰਕੜੇ->ਸੈੱਟ.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = ਅੰਕੜੇ->ਸੈੱਟ.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  ਵਾਪਸ Curl_ssl_filter_config_tmp_init(ਅੰਕੜੇ, needle->origin,
                                         &m->ssl_config,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#ਨਹੀਂਤਾਂ
                                         ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ url_ਮੇਲ_destroy(ਬਣਤਰ url_conn_ਮੇਲ *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#ਜੇ_ਅੰਤ
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ਸਥਿਰ CURLcode url_ਲੱਭੋ_or_create_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ connectdata *needle = ਨਿਰਾ;
  ਬਣਤਰ url_conn_ਮੇਲ ਮੇਲ;
  bool ਮੇਲ_initialized = FALSE;
  CURLcode ਨਤੀਜਾ;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  ਨਤੀਜਾ = url_create_needle(ਅੰਕੜੇ, &needle);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  DEBUGASSERT(needle);

  ਨਤੀਜਾ = url_ਮੇਲ_init(ਅੰਕੜੇ, needle, &ਮੇਲ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਮੇਲ_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FILE
  ਜੇ(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool ਪੂਰਾ;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->ਲੜੀਵਾਰ->connect_it);
    ਅੰਕੜੇ->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    ਅੰਕੜੇ->info.conn_protocol = needle->scheme->protocol & CURLPROTO_ਨਕਾਬ;
    ਨਤੀਜਾ = needle->scheme->ਲੜੀਵਾਰ->connect_it(ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    /* Setup a "faked" transfer that will do nothing */
    ਨਤੀਜਾ = Curl_cpool_ਜੋੜੋ(ਅੰਕੜੇ, needle, 0, 0, pnow);
    Curl_attach_connection(ਅੰਕੜੇ, needle, TRUE);
    needle = ਨਿਰਾ;
    ਜੇ(!ਨਤੀਜਾ) {
      /* Setup whatever necessary for a resumed transfer */
      ਨਤੀਜਾ = ਤਿਆਰੀ_range(ਅੰਕੜੇ);
      ਜੇ(!ਨਤੀਜਾ) {
        Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
        ਨਤੀਜਾ = Curl_init_transfer(ਅੰਕੜੇ, ਅੰਕੜੇ->conn);
      }
    }

    ਜੇ(ਨਤੀਜਾ) {
      DEBUGASSERT(ਅੰਕੜੇ->conn->scheme->ਲੜੀਵਾਰ->ਪੂਰਾ);
      /* we ignore the return code for the protocol-specific DONE */
      (ਖਾਲੀ)ਅੰਕੜੇ->conn->scheme->ਲੜੀਵਾਰ->ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, FALSE);
    }
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  ਜੇ((!ਅੰਕੜੇ->ਸੈੱਟ.reuse_fresh || ਅੰਕੜੇ->state.followlocation) &&
     !ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
    /* Ok, try to find and attach an existing one */
    ਜੇ(url_attach_existing(ਅੰਕੜੇ, needle, &ਮੇਲ, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->ਬਿੱਟ.reuse = TRUE;
      url_conn_reuse_adjust(ਅੰਕੜੇ, needle);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
      infof(ਅੰਕੜੇ, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#ਜੇ_ਅੰਤ
    }
  }

  ਜੇ(!ਅੰਕੜੇ->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    ਜੇ(ਮੇਲ.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(ਅੰਕੜੇ, "Waiting on connection to negotiate possible multiplexing.");
      ਨਤੀਜਾ = CURLE_NO_CONNECTION_ਉਪਲਬਧ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    ਨਤੀਜਾ = Curl_cpool_ਜੋੜੋ(ਅੰਕੜੇ, needle,
                            ਅੰਕੜੇ->multi->max_ਕੁੱਲ_connections,
                            ਅੰਕੜੇ->multi->max_host_connections,
                            pnow);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(ਅੰਕੜੇ, needle, TRUE);
    needle = ਨਿਰਾ;

    /* Clone the SSL filter configs into the new connection. */
    ਨਤੀਜਾ = Curl_ssl_conn_config_clone(&ਮੇਲ.ssl_config,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
                                        &ਮੇਲ.proxy_ssl_config,
#ਨਹੀਂਤਾਂ
                                        ਨਿਰਾ,
#ਜੇ_ਅੰਤ
                                        ਅੰਕੜੇ->conn);
    ਜੇ(ਨਤੀਜਾ) {
      DEBUGF(infof(ਅੰਕੜੇ, "Error: clone connection SSL config"));
      ਜਾਓ ਬਾਹਰਲਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    ਜੇ((ਅੰਕੜੇ->state.authhost.picked & CURLAUTH_NTLM) &&
       ਅੰਕੜੇ->state.authhost.ਪੂਰਾ) {
      infof(ਅੰਕੜੇ, "NTLM picked AND auth done set, clear picked");
      ਅੰਕੜੇ->state.authhost.picked = CURLAUTH_NONE;
      ਅੰਕੜੇ->state.authhost.ਪੂਰਾ = FALSE;
    }

    ਜੇ((ਅੰਕੜੇ->state.authproxy.picked & CURLAUTH_NTLM) &&
       ਅੰਕੜੇ->state.authproxy.ਪੂਰਾ) {
      infof(ਅੰਕੜੇ, "NTLM-proxy picked AND auth done set, clear picked");
      ਅੰਕੜੇ->state.authproxy.picked = CURLAUTH_NONE;
      ਅੰਕੜੇ->state.authproxy.ਪੂਰਾ = FALSE;
    }
#ਜੇ_ਅੰਤ
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  ਨਤੀਜਾ = Curl_init_transfer(ਅੰਕੜੇ, ਅੰਕੜੇ->conn);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Setup whatever necessary for a resumed transfer */
  ਨਤੀਜਾ = ਤਿਆਰੀ_range(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* persist the scheme and handler the transfer is using */
  ਅੰਕੜੇ->info.conn_scheme = ਅੰਕੜੇ->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  ਅੰਕੜੇ->info.conn_protocol = ਅੰਕੜੇ->conn->scheme->protocol & CURLPROTO_ਨਕਾਬ;
  ਅੰਕੜੇ->info.ਵਰਤਿਆ_proxy =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    0
#ਨਹੀਂਤਾਂ
    (ਅੰਕੜੇ->conn->socks_proxy.peer || ਅੰਕੜੇ->conn->http_proxy.peer)
#ਜੇ_ਅੰਤ
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  ਨਤੀਜਾ = Curl_conn_ev_ਅੰਕੜੇ_ਤਿਆਰੀ(ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  ਜੇ(ਮੇਲ_initialized)
    url_ਮੇਲ_destroy(&ਮੇਲ);
  ਜੇ(needle)
    Curl_conn_free(ਅੰਕੜੇ, needle);
  DEBUGASSERT(ਨਤੀਜਾ || ਅੰਕੜੇ->conn);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *pconnected)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਨਿਰਾ;
  ਅਚਲ ਬਣਤਰ curltime *pnow = ਨਿਰਾ;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
  /* Determine the origin of the transfer and what credentials to use */
  ਨਤੀਜਾ = url_ਸੈੱਟ_ਅੰਕੜੇ_origin_and_creds(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(!ਅੰਕੜੇ->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(ਅੰਕੜੇ);
  Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_POSTQUEUE, *pnow);
  ਨਤੀਜਾ = url_ਲੱਭੋ_or_create_conn(ਅੰਕੜੇ, pnow);
  conn = ਅੰਕੜੇ->conn;
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(!ਅੰਕੜੇ->conn) { /* just make really sure */
    DEBUGASSERT(0);
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(conn->ਬਿੱਟ.reuse) {
    ਜੇ(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_conn_ਤਿਆਰੀ(ਅੰਕੜੇ, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_ਸਿਰਲੇਖਾਂ_init(ਅੰਕੜੇ);
    CURL_TRC_M(ਅੰਕੜੇ, "Curl_conn_setup() -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ == CURLE_NO_CONNECTION_ਉਪਲਬਧ)
    DEBUGASSERT(!conn);

  ਜੇ(ਨਤੀਜਾ && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(ਅੰਕੜੇ);
    Curl_conn_close(ਅੰਕੜੇ, conn, TRUE);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(conn) {
    conn->ਬਿੱਟ.do_ਹੋਰ = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    ਜੇ(ਅੰਕੜੇ->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      ਅੰਕੜੇ->state.wildcardmatch = FALSE;
  }

  ਅੰਕੜੇ->state.ਪੂਰਾ = FALSE; /* *_done() is not called yet */

  ਅੰਕੜੇ->req.no_body = ਅੰਕੜੇ->ਸੈੱਟ.opt_no_body;
  ਜੇ(ਅੰਕੜੇ->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    ਅੰਕੜੇ->state.httpreq = HTTPREQ_ਸਿਰ;

  ਨਤੀਜਾ = Curl_req_ਸ਼ੁਰੂ(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ) {
    Curl_pgrsReset(ਅੰਕੜੇ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(USE_HTTP2) || defined(USE_HTTP3)

ਖਾਲੀ Curl_ਅੰਕੜੇ_ਤਰਜੀਹ_clear_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅੰਕੜੇ->state.ਭਾਰ = 0;
}

#ਜੇ_ਅੰਤ /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_ਸੈੱਟ(ਬਣਤਰ connectdata *conn, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                            ਖਾਲੀ *meta_ਅੰਕੜੇ, Curl_meta_dtor *meta_dtor)
{
  ਜੇ(!Curl_hash_add2(&conn->meta_hash, ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1,
                     meta_ਅੰਕੜੇ, meta_dtor)) {
    meta_dtor(ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1, meta_ਅੰਕੜੇ);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_conn_meta_remove(ਬਣਤਰ connectdata *conn, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ)
{
  Curl_hash_delete(&conn->meta_hash, ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1);
}

ਖਾਲੀ *Curl_conn_meta_get(ਬਣਤਰ connectdata *conn, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ)
{
  ਵਾਪਸ Curl_hash_pick(&conn->meta_hash, ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1);
}

ਬਣਤਰ Curl_easy *Curl_get_admin(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_easy *admin;

  ਜੇ(!ਅੰਕੜੇ->mid) /* already an admin handle */
    admin = ਅੰਕੜੇ;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->multi)
    admin = ਅੰਕੜੇ->multi->admin;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->multi_easy)
    admin = ਅੰਕੜੇ->multi_easy->admin;
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = ਅੰਕੜੇ;
  }
  ਜੇ(admin != ਅੰਕੜੇ) {
    admin->ਸੈੱਟ.conn_max_idle_ms = ਅੰਕੜੇ->ਸੈੱਟ.conn_max_idle_ms;
    admin->ਸੈੱਟ.conn_max_age_ms = ਅੰਕੜੇ->ਸੈੱਟ.conn_max_age_ms;
    admin->ਸੈੱਟ.upkeep_interval_ms = ਅੰਕੜੇ->ਸੈੱਟ.upkeep_interval_ms;
    admin->ਸੈੱਟ.timeout = ਅੰਕੜੇ->ਸੈੱਟ.timeout;
    admin->ਸੈੱਟ.server_response_timeout = ਅੰਕੜੇ->ਸੈੱਟ.server_response_timeout;
    admin->ਸੈੱਟ.no_signal = ਅੰਕੜੇ->ਸੈੱਟ.no_signal;
  }
  ਵਾਪਸ admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  ਜੇ(r1 && (r1 != CURLE_AGAIN))
    ਵਾਪਸ r1;
  ਜੇ(r2 && (r2 != CURLE_AGAIN))
    ਵਾਪਸ r2;
  ਵਾਪਸ r1;
}
