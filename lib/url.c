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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IPHLPAPI_H
#ಅನ್ತರ್ಭಾವ <Iphlpapi.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SOCKET
#ದೋಷ "We cannot compile without socket() support"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_IF_NAMETOINDEX) && defined(USE_WINSOCK)
#ಆದರೆ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 5)
#ಅನ್ತರ್ಭಾವ <wincrypt.h>  /* workaround for old mingw-w64 missing to include it */
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <iphlpapi.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vssh/vssh.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "http_digest.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "getinfo.h"
#ಅನ್ತರ್ಭಾವ "pop3.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "proxy.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "idn.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "setopt.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "peer.h"

/* Now for the protocols */
#ಅನ್ತರ್ಭಾವ "ftp.h"
#ಅನ್ತರ್ಭಾವ "dict.h"
#ಅನ್ತರ್ಭಾವ "telnet.h"
#ಅನ್ತರ್ಭಾವ "tftp.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"
#ಅನ್ತರ್ಭಾವ "imap.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "gopher.h"
#ಅನ್ತರ್ಭಾವ "mqtt.h"
#ಅನ್ತರ್ಭಾವ "rtsp.h"
#ಅನ್ತರ್ಭಾವ "smtp.h"
#ಅನ್ತರ್ಭಾವ "ws.h"

/* Some parts of the code (e.g. chunked encoding) assume this buffer has more
 * than a few bytes to play with. Do not let it become too small or bad things
 * will happen.
 */
#ಆದರೆ READBUFFER_ಗಾತ್ರ < READBUFFER_MIN
# error READBUFFER_ಗಾತ್ರ is too small
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ curl_prot_t get_protocol_family(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *s)
{
  DEBUGASSERT(s);
  DEBUGASSERT(s->family);
  ಹಿಂತಿರುಗಿ s->family;
}

ಶೂನ್ಯ Curl_freeset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* Free all dynamic strings stored in the data->set substructure. */
  ಎಣಿಕೆ dupblob j;

  CURL_EASY_STR_CLEAR0(ದತ್ತಾಂಶ, STRING_PASSWORD);
  CURL_EASY_STR_CLEAR0(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD);
  CURL_EASY_STR_CLEAR0(ದತ್ತಾಂಶ, STRING_BEARER);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  CURL_EASY_STR_CLEAR0(ದತ್ತಾಂಶ, STRING_PROXYPASSWORD);
  CURL_EASY_STR_CLEAR0(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD_PROXY);
#ಆದರೆ_ಅಂತ್ಯ
  Curl_u8_strset_clear(&ದತ್ತಾಂಶ->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು);
  curlx_safefree(ದತ್ತಾಂಶ->ಗಣ.str_copypostfields);

  ಗಾಗಿ(j = (ಎಣಿಕೆ dupblob)0; j < BLOB_ಕೊನೆಯ; j++) {
    curlx_safefree(ದತ್ತಾಂಶ->ಗಣ.blobs[j]);
  }

  Curl_bufref_free(&ದತ್ತಾಂಶ->state.referer);
  Curl_bufref_free(&ದತ್ತಾಂಶ->state.url);

#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(ದತ್ತಾಂಶ->ಗಣ.mimepostp);
  curlx_safefree(ದತ್ತಾಂಶ->ಗಣ.mimepostp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  curl_slist_free_all(ದತ್ತಾಂಶ->state.cookielist);
  ದತ್ತಾಂಶ->state.cookielist = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

/* free the URL pieces */
ಸ್ಥಿರ ಶೂನ್ಯ ಮೇಲೆ_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ urlpieces *ಮೇಲೆ = &ದತ್ತಾಂಶ->state.ಮೇಲೆ;
  curlx_safefree(ಮೇಲೆ->options);
  curlx_safefree(ಮೇಲೆ->ಮಾರ್ಗ);
  curlx_safefree(ಮೇಲೆ->query);
  curl_url_cleanup(ದತ್ತಾಂಶ->state.uh);
  ದತ್ತಾಂಶ->state.uh = ಖಾಲಿ;
}

/*
 * This is the internal function curl_easy_cleanup() calls. This should
 * cleanup and free all resources associated with this Curl_easy.
 *
 * We ignore SIGPIPE when this is called from curl_easy_cleanup.
 */
CURLcode Curl_close(ರಚನೆ Curl_easy **datap)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;

  ಆದರೆ(!datap || !*datap)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ದತ್ತಾಂಶ = *datap;
  *datap = ಖಾಲಿ;

  ಆದರೆ(!ದತ್ತಾಂಶ->state.ಆಂತರಿಕ && ದತ್ತಾಂಶ->multi) {
    /* This handle is still part of a multi handle, take care of this first
       and detach this handle from there.
       This detaches the connection. */
    Curl_multi_remove_handle(ದತ್ತಾಂಶ->multi, ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Detach connection if any is left. This should not be normal, but can be
       the case for example with CONNECT_ONLY + recv/send (test 556) */
    Curl_detach_connection(ದತ್ತಾಂಶ);
    ಆದರೆ(!ದತ್ತಾಂಶ->state.ಆಂತರಿಕ && ದತ್ತಾಂಶ->multi_easy) {
      /* when curl_easy_perform() is used, it creates its own multi handle to
         use and this is the one */
      curl_multi_cleanup(ದತ್ತಾಂಶ->multi_easy);
      ದತ್ತಾಂಶ->multi_easy = ಖಾಲಿ;
    }
  }
  DEBUGASSERT(!ದತ್ತಾಂಶ->conn || ದತ್ತಾಂಶ->state.ಆಂತರಿಕ);

  Curl_expire_clear_all(ದತ್ತಾಂಶ); /* shut off any timers left */

  ಆದರೆ(ದತ್ತಾಂಶ->state.rangestringalloc)
    curlx_free(ದತ್ತಾಂಶ->state.range);

  /* release any resolve information this transfer kept */
  Curl_resolv_destroy_all(ದತ್ತಾಂಶ);

  ದತ್ತಾಂಶ->ಗಣ.verbose = FALSE; /* no more calls to DEBUGFUNCTION */
  ದತ್ತಾಂಶ->magic = 0; /* force a clear AFTER the possibly enforced removal from
                    * the multi handle and async dns shutdown. The multi
                    * handle might check the magic and so might any
                    * DEBUGFUNCTION invoked for tracing */

  /* freed here in case DONE was not called */
  Curl_req_free(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);

  /* Close down all open SSL info and sessions */
  Curl_ssl_close_all(ದತ್ತಾಂಶ);
  Curl_peer_unlink(&ದತ್ತಾಂಶ->state.origin);
  Curl_peer_unlink(&ದತ್ತಾಂಶ->state.initial_origin);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  Curl_peer_unlink(&ದತ್ತಾಂಶ->state.rtsp_session_origin);
#ಆದರೆ_ಅಂತ್ಯ
  Curl_ssl_free_certinfo(ದತ್ತಾಂಶ);

  Curl_bufref_free(&ದತ್ತಾಂಶ->state.referer);

  ಮೇಲೆ_free(ದತ್ತಾಂಶ);
  curlx_dyn_free(&ದತ್ತಾಂಶ->state.headerb);
  Curl_flush_cookies(ದತ್ತಾಂಶ, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  Curl_altsvc_save(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->asi, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ALTSVC));
  Curl_altsvc_cleanup(&ದತ್ತಾಂಶ->asi);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  Curl_hsts_save(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->hsts, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_HSTS));
  ಆದರೆ(!ದತ್ತಾಂಶ->share || !ದತ್ತಾಂಶ->share->hsts)
    Curl_hsts_cleanup(&ದತ್ತಾಂಶ->hsts);
  curl_slist_free_all(ದತ್ತಾಂಶ->state.hstslist); /* clean up list */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
  Curl_http_auth_cleanup_digest(ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
  curlx_safefree(ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath);
  curlx_safefree(ದತ್ತಾಂಶ->info.contenttype);
  curlx_safefree(ದತ್ತಾಂಶ->info.wouldredirect);
  curlx_safefree(ದತ್ತಾಂಶ->info.effective_url);

  /* No longer a dirty share, if it exists */
  ಆದರೆ(Curl_share_easy_unlink(ದತ್ತಾಂಶ))
    DEBUGASSERT(0);

  Curl_hash_destroy(&ದತ್ತಾಂಶ->meta_hash);
  Curl_creds_unlink(&ದತ್ತಾಂಶ->state.creds);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  curlx_safefree(ದತ್ತಾಂಶ->state.rangeline);
  curlx_safefree(ದತ್ತಾಂಶ->state.http_host);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
  curlx_safefree(ದತ್ತಾಂಶ->req.cookiehost);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)
  Curl_mime_cleanpart(ದತ್ತಾಂಶ->state.formp);
  curlx_safefree(ದತ್ತಾಂಶ->state.formp);
#ಆದರೆ_ಅಂತ್ಯ

  /* destruct wildcard structures if it is needed */
  Curl_wildcard_dtor(&ದತ್ತಾಂಶ->wildcard);
  Curl_freeset(ದತ್ತಾಂಶ);
  Curl_ಶೀರ್ಷಿಕೆಗಳು_cleanup(ದತ್ತಾಂಶ);
  Curl_netrc_cleanup(&ದತ್ತಾಂಶ->state.netrc);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
  curlx_free(ದತ್ತಾಂಶ->state.envproxy);
#ಆದರೆ_ಅಂತ್ಯ
  curlx_memzero(ದತ್ತಾಂಶ, ಗಾತ್ರ(*ದತ್ತಾಂಶ));
  curlx_free(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Initialize the UserDefined fields within a Curl_easy.
 * This may be safely called on a new or existing Curl_easy.
 */
ಶೂನ್ಯ Curl_init_userdefined(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ UserDefined *ಗಣ = &ದತ್ತಾಂಶ->ಗಣ;

  ಗಣ->ಹೊರಹೋಗು = stdout;  /* default output to stdout */
  ಗಣ->ಒಳ_ಗಣ = stdin;  /* default input from stdin */
  ಗಣ->err = stderr;  /* default stderr to stderr */

  Curl_u8_strset_init(&ದತ್ತಾಂಶ->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು);

#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
  /* use fwrite as default function to store output */
  ಗಣ->fwrite_func = (curl_write_callback)fwrite;

  /* use fread as default function to read input */
  ಗಣ->fread_func_ಗಣ = (curl_read_callback)fread;
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->is_fread_ಗಣ = 0;

  ಗಣ->seek_client = ZERO_NULL;

  ಗಣ->filesize = -1;        /* we do not know the size */
  ಗಣ->postfieldsize = -1;   /* unknown size */
  ಗಣ->maxredirs = 30;       /* sensible default */

  ಗಣ->ವಿಧಾನ = HTTPREQ_GET; /* Default HTTP request */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಗಣ->rtspreq = RTSPREQ_OPTIONS; /* Default RTSP request */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಗಣ->ftp_use_epsv = TRUE;   /* FTP defaults to EPSV operations */
  ಗಣ->ftp_use_eprt = TRUE;   /* FTP defaults to EPRT operations */
  ಗಣ->ftp_use_pret = FALSE;  /* mainly useful for drftpd servers */
  ಗಣ->ftp_filemethod = FTPFILE_MULTICWD;
  ಗಣ->ftp_skip_ip = TRUE;    /* skip PASV IP by default */
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->dns_cache_timeout_ms = 60000; /* Timeout every 60 seconds by default */

  /* Timeout every 24 hours by default */
  ಗಣ->ssl_ca_cache_timeout = 24 * 60 * 60;

  ಗಣ->httpauth = CURLAUTH_BASIC;  /* defaults to basic */

  Curl_ssl_config_init(&ದತ್ತಾಂಶ->ಗಣ.ssl);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  Curl_ssl_config_init(&ದತ್ತಾಂಶ->ಗಣ.proxy_ssl);
  ಗಣ->proxyport = 0;
  ಗಣ->proxytype = CURLPROXY_HTTP; /* defaults to HTTP proxy */
  ಗಣ->proxyauth = CURLAUTH_BASIC; /* defaults to basic */
  /* SOCKS5 proxy auth defaults to username/password + GSS-API */
  ಗಣ->socks5auth = CURLAUTH_BASIC | CURLAUTH_GSSAPI;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  ಗಣ->doh_verifyhost = TRUE;
  ಗಣ->doh_verifypeer = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  /* defaults to any auth type */
  ಗಣ->ssh_auth_types = CURLSSH_AUTH_DEFAULT;
  ಗಣ->new_directory_perms = 0755; /* Default permissions */
#ಆದರೆ_ಅಂತ್ಯ

  ಗಣ->new_file_perms = 0644;    /* Default permissions */
  ಗಣ->allowed_protocols = (curl_prot_t)CURLPROTO_64ALL;
  ಗಣ->redir_protocols = CURLPROTO_REDIR;

#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /*
   * disallow unprotected protection negotiation NEC reference implementation
   * seem not to follow rfc1961 section 4.3/4.4
   */
  ಗಣ->socks5_gssapi_nec = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

  /* set default minimum TLS version */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  Curl_setopt_SSLVERSION(ದತ್ತಾಂಶ, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  Curl_setopt_SSLVERSION(ದತ್ತಾಂಶ, CURLOPT_PROXY_SSLVERSION,
                         CURL_SSLVERSION_DEFAULT);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಗಣ->wildcard_enabled = FALSE;
  ಗಣ->chunk_bgn = ZERO_NULL;
  ಗಣ->chunk_ಕೊನೆ = ZERO_NULL;
  ಗಣ->fnmatch = ZERO_NULL;
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->tcp_keepalive = FALSE;
  ಗಣ->tcp_keepintvl = 60;
  ಗಣ->tcp_keepidle = 60;
  ಗಣ->tcp_keepcnt = 9;
  ಗಣ->tcp_fastopen = FALSE;
  ಗಣ->tcp_nodelay = TRUE;
  ಗಣ->ssl_enable_alpn = TRUE;
  ಗಣ->expect_100_timeout = 1000L; /* Wait for a second by default. */
  ಗಣ->sep_ಶೀರ್ಷಿಕೆಗಳು = TRUE; /* separated header lists by default */
  ಗಣ->ಬಫರ್_ಗಾತ್ರ = READBUFFER_ಗಾತ್ರ;
  ಗಣ->upload_ಬಫರ್_ಗಾತ್ರ = UPLOADBUFFER_DEFAULT;
  ಗಣ->upload_flags = CURLULFLAG_ನೋಡಿದ;
  ಗಣ->happy_eyeballs_timeout = CURL_HET_DEFAULT;
  ಗಣ->upkeep_interval_ms = CURL_UPKEEP_INTERVAL_DEFAULT;
  ಗಣ->maxconnects = DEFAULT_CONNCACHE_ಗಾತ್ರ; /* for easy handles */
  ಗಣ->conn_max_idle_ms = 118 * 1000;
  ಗಣ->conn_max_age_ms = 24 * 3600 * 1000;
  ಗಣ->http09_allowed = FALSE;
  ಗಣ->httpwant = CURL_HTTP_ಆವೃತ್ತಿ_NONE;
#ಆದರೆ defined(USE_HTTP2) || defined(USE_HTTP3)
  ಗಣ->ತೂಕ = 0;
#ಆದರೆ_ಅಂತ್ಯ
  ಗಣ->quick_exit = 0L;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  ಗಣ->ws_raw_ಬಹುಲಕ = FALSE;
  ಗಣ->ws_no_auto_pong = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/* easy->meta_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ಸ್ಥಿರ ಶೂನ್ಯ easy_meta_freeentry(ಶೂನ್ಯ *p)
{
  (ಶೂನ್ಯ)p;
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
CURLcode Curl_open(ರಚನೆ Curl_easy **curl)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;

  /* simple start-up: alloc the struct, init it with zeroes and return */
  ದತ್ತಾಂಶ = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_easy));
  ಆದರೆ(!ದತ್ತಾಂಶ) {
    /* this is a serious error */
    DEBUGF(curl_mfprintf(stderr, "Error: calloc of Curl_easy failed\n"));
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ದತ್ತಾಂಶ->magic = CURLEASY_MAGIC_ಸಂಖ್ಯೆ;
  /* most recent connection is not yet defined */
  ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1;
  ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
  /* and not assigned an id yet */
  ದತ್ತಾಂಶ->id = -1;
  ದತ್ತಾಂಶ->mid = UINT32_MAX;
  ದತ್ತಾಂಶ->master_mid = UINT32_MAX;
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide = TRUE;

  Curl_hash_init(&ದತ್ತಾಂಶ->meta_hash, 23, CURL_HASH_TYPE_ಬೈಟುಗಳು,
                 easy_meta_freeentry);
  DEBUGASSERT(STRING_ಕೊನೆಯ <= UINT8_MAX);
  Curl_u8_strset_init(&ದತ್ತಾಂಶ->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು);
  curlx_dyn_init(&ದತ್ತಾಂಶ->state.headerb, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  Curl_bufref_init(&ದತ್ತಾಂಶ->state.url);
  Curl_bufref_init(&ದತ್ತಾಂಶ->state.referer);
  Curl_req_init(&ದತ್ತಾಂಶ->req);
  Curl_initinfo(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  Curl_llist_init(&ದತ್ತಾಂಶ->state.httphdrs, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
  Curl_netrc_init(&ದತ್ತಾಂಶ->state.netrc);
  Curl_init_userdefined(ದತ್ತಾಂಶ);

  *curl = ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_conn_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn)
{
  int8_t i;

  DEBUGASSERT(conn);

  ಆದರೆ(conn->scheme && conn->scheme->ಸತತ->disconnect &&
     !conn->ಬಿಟ್ಟುಗಳು.shutdown_handler)
    conn->scheme->ಸತತ->disconnect(ದತ್ತಾಂಶ, conn, TRUE);

  ಗಾಗಿ(i = 0; i < (int8_t)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    Curl_conn_cf_discard_all(ದತ್ತಾಂಶ, conn, i);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  Curl_peer_unlink(&conn->http_proxy.peer);
  Curl_peer_unlink(&conn->socks_proxy.peer);
  Curl_creds_unlink(&conn->http_proxy.creds);
  Curl_creds_unlink(&conn->socks_proxy.creds);
#ಆದರೆ_ಅಂತ್ಯ
  Curl_creds_unlink(&conn->creds);
  Curl_peer_unlink(&conn->creds_origin);
  curlx_safefree(conn->options);
  curlx_safefree(conn->interface_opts);
  Curl_ssl_conn_config_cleanup(conn);

  curlx_safefree(conn->destination);
  Curl_hash_destroy(&conn->meta_hash);
  Curl_peer_unlink(&conn->origin);
  Curl_peer_unlink(&conn->ಮೂಲಕ_peer);
  Curl_peer_unlink(&conn->origin2);
  Curl_peer_unlink(&conn->ಮೂಲಕ_peer2);

  curlx_free(conn); /* free all the connection oriented data */
}

/*
 * xfer_may_multiplex()
 *
 * Return a TRUE, iff the transfer can be done over an (appropriate)
 * multiplexed connection.
 */
ಸ್ಥಿರ bool xfer_may_multiplex(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ connectdata *conn)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  /* If an HTTP protocol and multiplexing is enabled */
  ಆದರೆ((conn->scheme->protocol & PROTO_FAMILY_HTTP) &&
     (!conn->ಬಿಟ್ಟುಗಳು.protoconnstart || !conn->ಬಿಟ್ಟುಗಳು.close)) {

    ಆದರೆ(Curl_multiplex_wanted(ದತ್ತಾಂಶ->multi) &&
       (ದತ್ತಾಂಶ->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)))
      /* allows HTTP/2 or newer */
      ಹಿಂತಿರುಗಿ TRUE;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಸ್ಥಿರ bool proxy_info_matches(ಸ್ಥಿರಾಂಕ ರಚನೆ proxy_info *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ proxy_info *needle)
{
  ಆದರೆ((ದತ್ತಾಂಶ->proxytype == needle->proxytype) &&
     Curl_peer_same_destination(ದತ್ತಾಂಶ->peer, needle->peer) &&
     Curl_creds_same(ದತ್ತಾಂಶ->creds, needle->creds)) {
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
ಸ್ಥಿರ bool ssh_config_matches(ರಚನೆ connectdata *one,
                               ರಚನೆ connectdata *two)
{
  ರಚನೆ ssh_conn *sshc1, *sshc2;

  sshc1 = Curl_conn_meta_get(one, CURL_META_SSH_CONN);
  sshc2 = Curl_conn_meta_get(two, CURL_META_SSH_CONN);
  ಹಿಂತಿರುಗಿ sshc1 && sshc2 && Curl_safecmp(sshc1->priv_ಕೀಲಿ, sshc2->priv_ಕೀಲಿ) &&
         Curl_safecmp(sshc1->pub_ಕೀಲಿ, sshc2->pub_ಕೀಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ url_conn_ಹೊಂದಿಕೆ {
  ರಚನೆ connectdata *ಸಿಕ್ಕಿತು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ connectdata *needle;
  ರಚನೆ curltime now;
  ರಚನೆ ssl_filter_config ssl_config;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ರಚನೆ ssl_filter_config proxy_ssl_config;
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(may_multiplex);
  ಬಿಟ್(want_ntlm_http);
  ಬಿಟ್(want_proxy_ntlm_http);
  ಬಿಟ್(want_nego_http);
  ಬಿಟ್(want_proxy_nego_http);
  ಬಿಟ್(may_tls); /* May upgrade clear-text connection to TLS, can only reuse
                 * connections that have matching TLS configuration.
                 * Always TRUE if `req_tls` is TRUE. */
  ಬಿಟ್(require_tls); /* Requires TLS use from a clear-text start, can only
                 * reuse connections that have TLS. */
  ಬಿಟ್(wait_pipe);
  ಬಿಟ್(ನೋಡಿದ_pending_conn);
  ಬಿಟ್(ನೋಡಿದ_single_use_conn);
  ಬಿಟ್(ನೋಡಿದ_multiplex_conn);
};

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_connect_config(ರಚನೆ connectdata *conn,
                                     ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  /* connect-only or to-be-closed connections will not be reused */
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.connect_only || conn->ಬಿಟ್ಟುಗಳು.close || conn->ಬಿಟ್ಟುಗಳು.no_reuse)
    ಹಿಂತಿರುಗಿ FALSE;

  /* ip_version must match */
  ಆದರೆ(m->ದತ್ತಾಂಶ->ಗಣ.ipver != CURL_IPRESOLVE_WHATEVER &&
     m->ದತ್ತಾಂಶ->ಗಣ.ipver != conn->ip_ಆವೃತ್ತಿ)
    ಹಿಂತಿರುಗಿ FALSE;

  /* When needle has specific local bind settings, those have to match.
   * If it does not, any other connection might do when it comes to ports. */
  ಆದರೆ(m->needle->localport &&
     ((conn->localport != m->needle->localport) ||
      (conn->localportrange != m->needle->localportrange)))
    ಹಿಂತಿರುಗಿ FALSE;
  /* For interface related bind options, we require a full match
   * as connection might go totally different ways (VPNs, for example). */
  ಆದರೆ((m->needle->interface_opts != conn->interface_opts) &&
     (!m->needle->interface_opts || !conn->interface_opts ||
      strcmp(conn->interface_opts, m->needle->interface_opts)))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ((m->needle->interface_opts || m->needle->localport) &&
    /* If we are bound to a specific local end (IP+port), we must not reuse a
       random other one, although if we did not ask for a particular one we
       can reuse one that was bound. */
    ((conn->localport != m->needle->localport) ||
     (conn->localportrange != m->needle->localportrange) ||
     (m->needle->interface_opts &&
      (!conn->interface_opts ||
       strcmp(conn->interface_opts, m->needle->interface_opts)))))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(!m->needle->ಮೂಲಕ_peer != !conn->ಮೂಲಕ_peer)
    /* do not mix connections that use the "connect to host" feature and
     * connections that do not use this feature */
    ಹಿಂತಿರುಗಿ FALSE;

  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_fully_connected(ರಚನೆ connectdata *conn,
                                      ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(!Curl_conn_is_connected(conn, FIRSTSOCKET) ||
     conn->ಬಿಟ್ಟುಗಳು.upgrade_ಒಳ_ಪ್ರಗತಿ) {
    /* Not yet connected, or a protocol upgrade is in progress. The later
     * happens for HTTP/2 Upgrade: requests that need a response. */
    ಆದರೆ(m->may_multiplex) {
      m->ನೋಡಿದ_pending_conn = TRUE;
      /* Do not pick a connection that has not connected yet */
      infof(m->ದತ್ತಾಂಶ, "Connection #%" FMT_OFF_T
            " is not open enough, cannot reuse", conn->connection_id);
    }
    /* Do not pick a connection that has not connected yet */
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_multi(ರಚನೆ connectdata *conn,
                            ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(CONN_INUSE(conn)) {
    DEBUGASSERT(conn->attached_multi);
    ಆದರೆ(conn->attached_multi != m->ದತ್ತಾಂಶ->multi)
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_multiplex_needs(ರಚನೆ connectdata *conn,
                                      ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(CONN_INUSE(conn)) {
    ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.multiplex) {
      /* conn busy and conn cannot take more transfers */
      m->ನೋಡಿದ_single_use_conn = TRUE;
      ಹಿಂತಿರುಗಿ FALSE;
    }
    m->ನೋಡಿದ_multiplex_conn = TRUE;
    ಆದರೆ(!m->may_multiplex || !url_ಹೊಂದಿಕೆ_multi(conn, m))
      /* conn busy and transfer cannot be multiplexed */
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_multiplex_limits(ರಚನೆ connectdata *conn,
                                       ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(CONN_INUSE(conn) && m->may_multiplex) {
    DEBUGASSERT(conn->ಬಿಟ್ಟುಗಳು.multiplex);
    /* If multiplexed, make sure we do not go over concurrency limit */
    ಆದರೆ(conn->attached_xfers >=
            Curl_multi_max_concurrent_streams(m->ದತ್ತಾಂಶ->multi)) {
      infof(m->ದತ್ತಾಂಶ, "connection #%" FMT_OFF_T " reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      ಹಿಂತಿರುಗಿ FALSE;
    }
    ಆದರೆ(conn->attached_xfers >=
       Curl_conn_get_max_concurrent(m->ದತ್ತಾಂಶ, conn, FIRSTSOCKET)) {
      infof(m->ದತ್ತಾಂಶ, "connection #%" FMT_OFF_T "reached "
            "MAX_CONCURRENT_STREAMS(%u), skip",
            conn->connection_id, conn->attached_xfers);
      ಹಿಂತಿರುಗಿ FALSE;
    }
    /* When not multiplexed, we have a match here! */
    infof(m->ದತ್ತಾಂಶ, "Multiplexed connection found");
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_ssl_use(ರಚನೆ connectdata *conn,
                              ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(m->needle->scheme->flags & PROTOPT_SSL) {
    /* We are looking for SSL, if `conn` does not do it, not a match. */
    ಆದರೆ(!Curl_conn_is_ssl(conn, FIRSTSOCKET))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    /* If the protocol does not allow reuse of SSL connections OR
       is of another protocol family, not a match. */
    ಆದರೆ(!(m->needle->scheme->flags & PROTOPT_SSL_REUSE) ||
       (get_protocol_family(conn->scheme) != m->needle->scheme->protocol))
      ಹಿಂತಿರುಗಿ FALSE;
    /* We may reuse this as an auto-TLS upgrade, but only if the SSL
     * config parameters match. */
    ಆದರೆ(!Curl_ssl_conn_config_ಹೊಂದಿಕೆ(m->ದತ್ತಾಂಶ, &m->ssl_config, conn, FALSE))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(m->require_tls)
    /* a clear-text STARTTLS protocol with required TLS */
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_proxy_use(ರಚನೆ connectdata *conn,
                                ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(m->needle->ಬಿಟ್ಟುಗಳು.origin_is_proxy != conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy)
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(!proxy_info_matches(&m->needle->socks_proxy, &conn->socks_proxy))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(!proxy_info_matches(&m->needle->http_proxy, &conn->http_proxy))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(m->ದತ್ತಾಂಶ->ಗಣ.socks5_auth_only && !conn->ಬಿಟ್ಟುಗಳು.socks5_authenticated &&
     ((conn->socks_proxy.proxytype == CURLPROXY_SOCKS5) ||
      (conn->socks_proxy.proxytype == CURLPROXY_SOCKS5_HOSTNAME))) {
    DEBUGF(infof(m->ದತ್ತಾಂಶ,
                 "Connection #%" FMT_OFF_T
                 " was not SOCKS5 authenticated, cannot reuse",
                 conn->connection_id));
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಆದರೆ(CURL_PROXY_IS_HTTPS(m->needle->http_proxy.proxytype)) {
    /* https proxies come in different types, http/1.1, h2, ... */
    /* match SSL config to proxy */
    ಆದರೆ(!Curl_ssl_conn_config_ಹೊಂದಿಕೆ(m->ದತ್ತಾಂಶ, &m->proxy_ssl_config,
                                   conn, TRUE)) {
      DEBUGF(infof(m->ದತ್ತಾಂಶ,
                   "Connection #%" FMT_OFF_T
                   " has different SSL proxy parameters, cannot reuse",
                   conn->connection_id));
      ಹಿಂತಿರುಗಿ FALSE;
    }
    /* the SSL config to the server, which may apply here is checked
     * further below */
  }
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು url_ಹೊಂದಿಕೆ_proxy_use(c, m) ((ಶೂನ್ಯ)(c), (ಶೂನ್ಯ)(m), TRUE)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_http_multiplex(ರಚನೆ connectdata *conn,
                                     ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m,
                                     bool *pwait_pipe)
{
  ಆದರೆ(m->may_multiplex &&
     (m->ದತ್ತಾಂಶ->state.http_neg.allowed & (CURL_HTTP_V2x | CURL_HTTP_V3x)) &&
     (m->needle->scheme->protocol & CURLPROTO_HTTP) &&
     !conn->httpversion_ನೋಡಿದ) {
    ಆದರೆ(m->ದತ್ತಾಂಶ->ಗಣ.pipewait) {
      infof(m->ದತ್ತಾಂಶ, "Server upgrade does not support multiplex yet, wait");
      m->ಸಿಕ್ಕಿತು = ಖಾಲಿ;
      *pwait_pipe = TRUE;
      ಹಿಂತಿರುಗಿ TRUE; /* stop searching, we want to wait */
    }
    infof(m->ದತ್ತಾಂಶ, "Server upgrade cannot be used");
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_http_ಆವೃತ್ತಿ(ರಚನೆ connectdata *conn,
                                   ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  /* If looking for HTTP and the HTTP versions allowed do not include
   * the HTTP version of conn, continue looking. */
  ಆದರೆ((m->needle->scheme->protocol & PROTO_FAMILY_HTTP)) {
    ಆಯ್ಕೆ(Curl_conn_http_ಆವೃತ್ತಿ(m->ದತ್ತಾಂಶ, conn)) {
    ಸ್ಥಿತಿ 30:
      ಆದರೆ(!(m->ದತ್ತಾಂಶ->state.http_neg.allowed & CURL_HTTP_V3x)) {
        DEBUGF(infof(m->ದತ್ತಾಂಶ, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h3", conn->connection_id));
        ಹಿಂತಿರುಗಿ FALSE;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 20:
      ಆದರೆ(!(m->ದತ್ತಾಂಶ->state.http_neg.allowed & CURL_HTTP_V2x)) {
        DEBUGF(infof(m->ದತ್ತಾಂಶ, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h2", conn->connection_id));
        ಹಿಂತಿರುಗಿ FALSE;
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಆದರೆ(!(m->ದತ್ತಾಂಶ->state.http_neg.allowed & CURL_HTTP_V1x)) {
        DEBUGF(infof(m->ದತ್ತಾಂಶ, "not reusing conn #%" CURL_FORMAT_CURL_OFF_T
                     ", we do not want h1", conn->connection_id));
        ಹಿಂತಿರುಗಿ FALSE;
      }
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು url_ಹೊಂದಿಕೆ_http_multiplex(c, m, w) ((ಶೂನ್ಯ)(c), (ಶೂನ್ಯ)(m), TRUE)
#ವ್ಯಾಖ್ಯಾನಿಸು url_ಹೊಂದಿಕೆ_http_ಆವೃತ್ತಿ(c, m)      ((ಶೂನ್ಯ)(c), (ಶೂನ್ಯ)(m), TRUE)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_proto_config(ರಚನೆ connectdata *conn,
                                   ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(!url_ಹೊಂದಿಕೆ_http_ಆವೃತ್ತಿ(conn, m))
    ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  ಆದರೆ(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_SSH) {
    ಆದರೆ(!ssh_config_matches(m->needle, conn))
      ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(get_protocol_family(m->needle->scheme) & PROTO_FAMILY_FTP) {
    ಆದರೆ(!Curl_ftp_conns_ಹೊಂದಿಕೆ(m->needle, conn))
      ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_auth(ರಚನೆ connectdata *conn,
                           ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(!Curl_creds_same(m->needle->creds, conn->creds)) {
    ಆದರೆ(m->needle->creds)
      ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ(!Curl_creds_same(m->ದತ್ತಾಂಶ->state.creds, conn->creds))
      ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  /* GSS delegation differences do not actually affect every connection and
     auth method, but this check takes precaution before efficiency */
  ಆದರೆ(m->needle->gssapi_delegation != conn->gssapi_delegation)
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_destination(ರಚನೆ connectdata *conn,
                                  ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  /* Different connect-to peers never match */
  ಆದರೆ(!Curl_peer_same_destination(m->needle->ಮೂಲಕ_peer, conn->ಮೂಲಕ_peer))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(m->needle->origin->scheme != conn->origin->scheme &&
    /* `needle` and `conn` not having the same scheme.
     * This is allowed for the same family *if* conn is using TLS.
     * - IMAP+STARTTLS works for IMAPS.
     * - IMAPS works for IMAP. */
     get_protocol_family(conn->origin->scheme) !=
     m->needle->scheme->protocol)
    ಹಿಂತಿರುಗಿ FALSE;

  /* Scheme mismatch is acceptable, compare hostname/port */
  ಹಿಂತಿರುಗಿ Curl_peer_same_destination(m->needle->origin, conn->origin);
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_ssl_config(ರಚನೆ connectdata *conn,
                                 ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  /* If talking/upgrading to TLS, conn needs to use the same SSL options. */
  ಆದರೆ(((m->needle->scheme->flags & PROTOPT_SSL) || m->may_tls) &&
     !Curl_ssl_conn_config_ಹೊಂದಿಕೆ(m->ದತ್ತಾಂಶ, &m->ssl_config, conn, FALSE)) {
    DEBUGF(infof(m->ದತ್ತಾಂಶ, "Connection #%" FMT_OFF_T
                 " has different SSL parameters, cannot reuse",
                 conn->connection_id));
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_auth_ntlm(ರಚನೆ connectdata *conn,
                                ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(conn->http_ntlm_state != NTLMSTATE_NONE) {
    /* Connection is using NTLM. We cannot reuse if transfer
     * has different Auth input parameters. */
    ಆದರೆ(!m->want_ntlm_http ||
       !Curl_creds_same(conn->creds, m->ದತ್ತಾಂಶ->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->ದತ್ತಾಂಶ->state.origin))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(m->want_ntlm_http) {
    /* Transfer wants NTLM, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    ಆದರೆ(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->ದತ್ತಾಂಶ->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->ದತ್ತಾಂಶ->state.origin)))
      ಹಿಂತಿರುಗಿ FALSE;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  /* Same for Proxy NTLM authentication */
  ಆದರೆ(conn->proxy_ntlm_state != NTLMSTATE_NONE) {
    ಆದರೆ(!m->want_proxy_ntlm_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(m->want_proxy_ntlm_http) {
    ಆದರೆ(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು url_ಹೊಂದಿಕೆ_auth_ntlm(c, m) ((ಶೂನ್ಯ)(c), (ಶೂನ್ಯ)(m), TRUE)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_auth_nego(ರಚನೆ connectdata *conn,
                                ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  ಆದರೆ(conn->http_negotiate_state != CURL_NEGO_AUTH_NONE) {
    /* Connection is using Negotiate. We cannot reuse if transfer
     * has different Auth input parameters. */
    ಆದರೆ(!m->want_nego_http ||
       !Curl_creds_same(conn->creds, m->ದತ್ತಾಂಶ->state.creds) ||
       !Curl_peer_equal(conn->creds_origin, m->ದತ್ತಾಂಶ->state.origin))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(m->want_nego_http) {
    /* Transfer wants Negotiate, connection is not using it.
     * Do not reuse when connection credentials state is bound to an origin
     * and it or creds differ. */
    ಆದರೆ(conn->creds_origin &&
       (!Curl_creds_same(conn->creds, m->ದತ್ತಾಂಶ->state.creds) ||
        !Curl_peer_equal(conn->creds_origin, m->ದತ್ತಾಂಶ->state.origin)))
      ಹಿಂತಿರುಗಿ FALSE;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  /* Same for Proxy Negotiate authentication */
  ಆದರೆ(conn->proxy_negotiate_state != CURL_NEGO_AUTH_NONE) {
    ಆದರೆ(!m->want_proxy_nego_http ||
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(m->want_proxy_nego_http) {
    ಆದರೆ(conn->http_proxy.creds &&
       !Curl_creds_same(m->needle->http_proxy.creds, conn->http_proxy.creds))
      ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು url_ಹೊಂದಿಕೆ_auth_nego(c, m) ((ಶೂನ್ಯ)(c), (ಶೂನ್ಯ)(m), TRUE)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ cpool_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ url_ಹೊಂದಿಕೆ_conn(ರಚನೆ connectdata *conn,
                                         ಶೂನ್ಯ *userdata)
{
  ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m = userdata;
  bool wait_pipe = FALSE;

  /* general connect config setting match? */
  ಆದರೆ(!url_ಹೊಂದಿಕೆ_connect_config(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  /* match for destination and protocol? */
  ಆದರೆ(!url_ಹೊಂದಿಕೆ_destination(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_fully_connected(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_multiplex_needs(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_ssl_use(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_proxy_use(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;
  ಆದರೆ(!url_ಹೊಂದಿಕೆ_ssl_config(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_http_multiplex(conn, m, &wait_pipe))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_auth(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_proto_config(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_auth_ntlm(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_auth_nego(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  ಆದರೆ(!url_ಹೊಂದಿಕೆ_multiplex_limits(conn, m))
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CONT;

  /* The connection matches all conditions, but do we want to use it? */
  ಆದರೆ(wait_pipe) {
    /* The connection fits, but it's multiplex state has not been determined
     * yet. Put the transfer into PENDING and wait for conn state change. */
    DEBUGASSERT(!m->ಸಿಕ್ಕಿತು);
    m->wait_pipe = TRUE;
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು;
  }

  ಆದರೆ(m->ದತ್ತಾಂಶ->ಗಣ.conn_max_age_ms > 0) {
    timediff_t age_ms = Curl_cpool_conn_age_ms(m->ದತ್ತಾಂಶ, conn, &m->now);
    ಆದರೆ(age_ms > m->ದತ್ತಾಂಶ->ಗಣ.conn_max_age_ms) {
      /* Transfer is looking for a younger connection. */
      ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_TOO_OLD;
    }
  }

  /* If we are going to pick an idle connection, do an extra
   * health check before we reuse it. */
  ಆದರೆ(!CONN_INUSE(conn) &&
     !Curl_cpool_conn_seems_healthy(conn, m->ದತ್ತಾಂಶ, &m->now)) {
    infof(m->ದತ್ತಾಂಶ, "Connection %" FMT_OFF_T " seems to be dead, terminating",
          conn->connection_id);
    ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_CLOSE;
  }

  /* conn matches our needs. */
  m->ಸಿಕ್ಕಿತು = conn;
  ಹಿಂತಿರುಗಿ CPOOL_ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು;
}

ಸ್ಥಿರ bool url_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ(ಶೂನ್ಯ *userdata)
{
  ರಚನೆ url_conn_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ = userdata;
  ಆದರೆ(ಹೊಂದಿಕೆ->ಸಿಕ್ಕಿತು) {
    /* Attach it now while still under lock, so the connection does
     * no longer appear idle and can be reaped. */
    Curl_attach_connection(ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ, ಹೊಂದಿಕೆ->ಸಿಕ್ಕಿತು, TRUE);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ->ನೋಡಿದ_single_use_conn && !ಹೊಂದಿಕೆ->ನೋಡಿದ_multiplex_conn) {
    /* We have seen a single-use, existing connection to the destination and
     * no multiplexed one. It seems safe to assume that the server does
     * not support multiplexing. */
    ಹೊಂದಿಕೆ->wait_pipe = FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊಂದಿಕೆ->ನೋಡಿದ_pending_conn && ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ->ಗಣ.pipewait) {
    infof(ಹೊಂದಿಕೆ->ದತ್ತಾಂಶ,
          "Found pending candidate for reuse and CURLOPT_PIPEWAIT is set");
    ಹೊಂದಿಕೆ->wait_pipe = TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
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
ಸ್ಥಿರ bool url_attach_existing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *needle,
                                ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  /* Find a connection in the pool that matches what "data + needle"
   * requires. If a suitable candidate is found, it is attached to "data". */
  ಹಿಂತಿರುಗಿ Curl_cpool_ಕಂಡುಹಿಡಿ(ದತ್ತಾಂಶ, needle->destination, TRUE, pnow,
                         url_ಹೊಂದಿಕೆ_conn, url_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ, m);
}

/*
 * Allocate and initialize a new connectdata object.
 */
ಸ್ಥಿರ ರಚನೆ connectdata *allocate_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ connectdata));
  ಆದರೆ(!conn)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* and we setup a few fields in case we end up actually using this struct */
  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->recv_idx = 0; /* default for receiving transfer data */
  conn->send_idx = 0; /* default for sending transfer data */
  conn->connection_id = -1;    /* no ID */
  conn->cpid = UINT32_MAX;  /* not in connection pool yet */
  conn->attached_xfers = 0;
  conn->shutdown.ಆರಂಭ_ms[FIRSTSOCKET] =
    conn->shutdown.ಆರಂಭ_ms[SECONDARYSOCKET] = -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  conn->ಬಿಟ್ಟುಗಳು.ftp_use_epsv = ದತ್ತಾಂಶ->ಗಣ.ftp_use_epsv;
  conn->ಬಿಟ್ಟುಗಳು.ftp_use_eprt = ದತ್ತಾಂಶ->ಗಣ.ftp_use_eprt;
#ಆದರೆ_ಅಂತ್ಯ
  conn->ip_ಆವೃತ್ತಿ = ದತ್ತಾಂಶ->ಗಣ.ipver;
  conn->ಬಿಟ್ಟುಗಳು.connect_only = (bool)ದತ್ತಾಂಶ->ಗಣ.connect_only;
  conn->transport_wanted = TRNSPRT_TCP; /* most of them are TCP streams */

  /* Store the local bind parameters that will be used for this connection */
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_INTERFACE_OPT)) {
    conn->interface_opts =
      curlx_strdup(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_INTERFACE_OPT));
    ಆದರೆ(!conn->interface_opts)
      ಹೋಗು error;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_BINDLOCAL
  conn->localportrange = ದತ್ತಾಂಶ->ಗಣ.localportrange;
  conn->localport = ದತ್ತಾಂಶ->ಗಣ.localport;
#ಆದರೆ_ಅಂತ್ಯ

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = ದತ್ತಾಂಶ->ಗಣ.fclosesocket;
  conn->closesocket_client = ದತ್ತಾಂಶ->ಗಣ.closesocket_client;
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  conn->gssapi_delegation = ದತ್ತಾಂಶ->ಗಣ.gssapi_delegation;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ conn;
error:

  curlx_free(conn->interface_opts);
  curlx_free(conn);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ CURLcode url_ಗಣ_conn_scheme(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn)
{
  /* URL scheme is usable for connection when it is
   * - allowed
   * - not from a redirect or an allowed redirect protocol */
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme = conn->origin->scheme;
  ಆದರೆ(scheme->ಸತತ &&
     (ದತ್ತಾಂಶ->ಗಣ.allowed_protocols & scheme->protocol) &&
     (!ದತ್ತಾಂಶ->state.this_is_a_follow ||
       (ದತ್ತಾಂಶ->ಗಣ.redir_protocols & scheme->protocol))) {
    conn->scheme = scheme;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ(scheme->flags & PROTOPT_NO_TRANSFER)
    failf(ದತ್ತಾಂಶ, "Protocol \"%s\" is not for transfers", scheme->name);
  ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "Protocol \"%s\" is disabled%s", scheme->name,
          ದತ್ತಾಂಶ->state.this_is_a_follow ? " (in redirect)" : "");
  ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
}

CURLcode Curl_uc_ಗಮ್ಯ_curlcode(CURLUcode uc)
{
  ಆಯ್ಕೆ(uc) {
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  ಸ್ಥಿತಿ CURLUE_UNSUPPORTED_SCHEME:
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  ಸ್ಥಿತಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಥಿತಿ CURLUE_USER_NOT_ALLOWED:
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
ಸ್ಥಿರ CURLcode hsts_upgrade(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             CURLU *uh,
                             uint16_t ಪೋರ್ಟ್_override,
                             uint32_t scope_id)
{
  /* HSTS upgrade */
  ಆದರೆ(ದತ್ತಾಂಶ->hsts && (ದತ್ತಾಂಶ->state.origin->scheme == &Curl_scheme_http) &&
     Curl_hsts_applies(ದತ್ತಾಂಶ->hsts, ದತ್ತಾಂಶ->state.origin)) {
    ಅಕ್ಷರ *url;
    CURLUcode uc;
    CURLcode ಫಲಿತಾಂಶ;

    uc = curl_url_ಗಣ(uh, CURLUPART_SCHEME, "https", 0);
    ಆದರೆ(uc)
      ಹಿಂತಿರುಗಿ Curl_uc_ಗಮ್ಯ_curlcode(uc);
    Curl_bufref_free(&ದತ್ತಾಂಶ->state.url);
    /* after update, get the updated version */
    uc = curl_url_get(uh, CURLUPART_URL, &url, 0);
    ಆದರೆ(uc)
      ಹಿಂತಿರುಗಿ Curl_uc_ಗಮ್ಯ_curlcode(uc);
    Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.url, url, 0, curl_free);

    ಫಲಿತಾಂಶ = Curl_peer_from_url(uh, ದತ್ತಾಂಶ, ಪೋರ್ಟ್_override, scope_id,
                                &ದತ್ತಾಂಶ->state.origin);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    infof(ದತ್ತಾಂಶ, "Switched from HTTP to HTTPS due to HSTS => %s", url);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು hsts_upgrade(x, y, z, a) CURLE_OK
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ bool str_has_ctrl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು)
{
  ಆದರೆ(ಒಳಹರಿವು) {
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *str = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಒಳಹರಿವು;
    ಆಗ(*str) {
      ಆದರೆ(*str < 0x20)
        ಹಿಂತಿರುಗಿ TRUE;
      str++;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
/*
 * Override the login details from the URL with that in the CURLOPT_USERPWD
 * option or a .netrc file, if applicable.
 */
ಸ್ಥಿರ CURLcode url_ಗಣ_ದತ್ತಾಂಶ_creds_netrc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ Curl_creds **pcreds)
{
  ರಚನೆ Curl_creds *ncreds_ಹೊರಹೋಗು = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_netrc) { /* not CURL_NETRC_IGNORED */
    ರಚನೆ Curl_creds *ncreds_ಒಳ = ಖಾಲಿ;
    bool ಪರಿಶೀಲನೆ_netrc = TRUE;
    NETRCcode ret;
    CURLUcode uc;

    ಆದರೆ(*pcreds) {
      ಆಯ್ಕೆ((*pcreds)->ಮೂಲ) {
      ಸ್ಥಿತಿ CREDS_OPTION:
        /* we never override credentials set via CURLOPT_*, leave. */
        ಪರಿಶೀಲನೆ_netrc = FALSE;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CREDS_URL: /* only apply when netrc is not required */
        ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_netrc == CURL_NETRC_REQUIRED) {
          /* We ignore password from URL */
          ncreds_ಒಳ = *pcreds;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_creds_has_user(*pcreds) ||
                !Curl_creds_has_passwd(*pcreds)) {
          /* We use netrc to complete what is missing */
          ncreds_ಒಳ = *pcreds;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಪರಿಶೀಲನೆ_netrc = FALSE;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* ignore credentials from other sources */
        ನಿಲ್ಲಿಸು;
      }
    }

    ಆದರೆ(!ಪರಿಶೀಲನೆ_netrc)
      ಹೋಗು ಹೊರಹೋಗು;

    ret = Curl_netrc_ಪರಿಶೀಲನೆ(ದತ್ತಾಂಶ, &ದತ್ತಾಂಶ->state.netrc,
                          ದತ್ತಾಂಶ->state.origin->hostname,
                          Curl_creds_user(ncreds_ಒಳ),
                          CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_NETRC_FILE),
                          &ncreds_ಹೊರಹೋಗು);
    DEBUGASSERT(!ret || !ncreds_ಹೊರಹೋಗು);
    ಆದರೆ(ret == NETRC_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret && ((ret == NETRC_NO_ಹೊಂದಿಕೆ) ||
                    (ದತ್ತಾಂಶ->ಗಣ.use_netrc == CURL_NETRC_OPTIONAL))) {
      infof(ದತ್ತಾಂಶ, "Could not find host %s in the %s file; using defaults",
            ದತ್ತಾಂಶ->state.origin->hostname,
            (CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_NETRC_FILE) ?
             CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_NETRC_FILE) : ".netrc"));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *m = Curl_netrc_strerror(ret);
      failf(ದತ್ತಾಂಶ, ".netrc error: %s", m);
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ncreds_ಹೊರಹೋಗು) {
      ಆದರೆ(!(ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
         /* if the protocol cannot handle control codes in credentials, make
            sure there are none */
         (str_has_ctrl(ncreds_ಹೊರಹೋಗು->user) ||
          str_has_ctrl(ncreds_ಹೊರಹೋಗು->passwd))) {
        failf(ದತ್ತಾಂಶ, "control code detected in .netrc credentials");
        ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      CURL_TRC_M(ದತ್ತಾಂಶ, "netrc: using credentials for %s as %s",
                 ದತ್ತಾಂಶ->state.origin->hostname, ncreds_ಹೊರಹೋಗು->user);
      ಫಲಿತಾಂಶ = Curl_creds_ವಿಲೀನ(ncreds_ಹೊರಹೋಗು->user, ncreds_ಹೊರಹೋಗು->passwd,
                                *pcreds, CREDS_NETRC, pcreds);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      /* for updated strings, we update them in the URL */
      uc = curl_url_ಗಣ(ದತ್ತಾಂಶ->state.uh, CURLUPART_USER,
                        Curl_creds_user(*pcreds), CURLU_URLENCODE);
      ಆದರೆ(!uc)
        uc = curl_url_ಗಣ(ದತ್ತಾಂಶ->state.uh, CURLUPART_PASSWORD,
                          Curl_creds_passwd(*pcreds),
                          CURLU_URLENCODE);
      ಆದರೆ(uc)
        ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      DEBUGASSERT(0);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  Curl_creds_trace(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.creds, "transfer credentials");
#ಆದರೆ_ಅಂತ್ಯ

ಹೊರಹೋಗು:
  Curl_creds_unlink(&ncreds_ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_NETRC */

ಸ್ಥಿರ CURLcode url_ಗಣ_ದತ್ತಾಂಶ_creds(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLU *uh)
{
  ರಚನೆ Curl_creds *newcreds = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ((CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_USERNAME) ||
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PASSWORD) ||
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_BEARER) ||
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SASL_AUTHZID) ||
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SERVICE_NAME)) &&
     Curl_auth_allowed_ಗಮ್ಯ_origin(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.origin)) {
    ಫಲಿತಾಂಶ = Curl_creds_create(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_USERNAME),
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PASSWORD),
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_BEARER),
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SASL_AUTHZID),
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SERVICE_NAME),
                               CREDS_OPTION, &newcreds);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(newcreds &&
       !(ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_USERPWDCTRL) &&
       (str_has_ctrl(Curl_creds_user(newcreds)) ||
        str_has_ctrl(Curl_creds_passwd(newcreds)))) {
      /* if the protocol cannot handle control codes in credentials, make
         sure there are none */
      failf(ದತ್ತಾಂಶ, "control code detected in credentials");
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  /* Extract credentials from the URL only if there are none OR
   * if no CURLOPT_USER was set. */
  ಆದರೆ(!newcreds || !Curl_creds_has_user(newcreds)) {
    ಅಕ್ಷರ *user = ಖಾಲಿ;
    ಅಕ್ಷರ *passwd = ಖಾಲಿ;
    ಅಕ್ಷರ *udecoded = ಖಾಲಿ;
    ಅಕ್ಷರ *pdecoded = ಖಾಲಿ;
    CURLUcode uc;

    uc = curl_url_get(uh, CURLUPART_USER, &user, 0);
    ಆದರೆ(uc && (uc != CURLUE_NO_USER))
      ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      uc = curl_url_get(uh, CURLUPART_PASSWORD, &passwd, 0);
      ಆದರೆ(uc && (uc != CURLUE_NO_PASSWORD))
        ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ && user) {
      ಫಲಿತಾಂಶ = Curl_urldecode(user, 0, &udecoded, ಖಾಲಿ,
                              (ದತ್ತಾಂಶ->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ && passwd) {
      ಫಲಿತಾಂಶ = Curl_urldecode(passwd, 0, &pdecoded, ಖಾಲಿ,
                              (ದತ್ತಾಂಶ->state.origin->scheme->flags &
                               PROTOPT_USERPWDCTRL) ?
                              REJECT_ZERO : REJECT_CTRL);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_creds_ವಿಲೀನ(udecoded, pdecoded, newcreds,
                                CREDS_URL, &newcreds);

    curlx_free(udecoded);
    curlx_free(pdecoded);
    curlx_free(passwd);
    curlx_free(user);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "error extracting credentials from URL");
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_NETRC
  /* Check for overridden login details and set them accordingly so that
     they are known when protocol->setup_connection is called! */
  ಫಲಿತಾಂಶ = url_ಗಣ_ದತ್ತಾಂಶ_creds_netrc(ದತ್ತಾಂಶ, &newcreds);
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_NETRC */

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ && !Curl_creds_equal(ದತ್ತಾಂಶ->state.creds, newcreds)) {
    /* Do we have more things to trigger on credentials change? */
    Curl_creds_link(&ದತ್ತಾಂಶ->state.creds, newcreds);
  }
  Curl_creds_unlink(&newcreds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode url_ಗಣ_conn_origin_etc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_peer_link(&conn->origin, ದತ್ತಾಂಶ->state.origin);

  /* set the connection scheme */
  ಫಲಿತಾಂಶ = url_ಗಣ_conn_scheme(ದತ್ತಾಂಶ, conn);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* set the connection options */
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_OPTIONS)) {
    conn->options = curlx_strdup(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_OPTIONS));
    ಆದರೆ(!conn->options) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.ಮೇಲೆ.options) {
    conn->options = curlx_strdup(ದತ್ತಾಂಶ->state.ಮೇಲೆ.options);
    ಆದರೆ(!conn->options) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * If we are doing a resumed transfer, we need to setup our stuff
 * properly.
 */
ಸ್ಥಿರ CURLcode ಸಿದ್ಧತೆ_range(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ UrlState *s = &ದತ್ತಾಂಶ->state;
  s->resume_from = ದತ್ತಾಂಶ->ಗಣ.ಗಣ_resume_from;
  ಆದರೆ(s->resume_from || CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_RANGE)) {
    ಆದರೆ(s->rangestringalloc)
      curlx_free(s->range);

    ಆದರೆ(s->resume_from)
      s->range = curl_maprintf("%" FMT_OFF_T "-", s->resume_from);
    ಇಲ್ಲದಿದ್ದರೆ
      s->range = curlx_strdup(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಗಣ_RANGE));

    ಆದರೆ(!s->range)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    s->rangestringalloc = TRUE;

    /* tell ourselves to fetch this range */
    s->use_range = TRUE;        /* enable range download */
  }
  ಇಲ್ಲದಿದ್ದರೆ
    s->use_range = FALSE; /* disable range download */

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಸ್ಥಿರ CURLcode ಸಿದ್ಧತೆ_connection_internals(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_peer *peer = ಖಾಲಿ;

  ಆದರೆ(conn->scheme->ಸತತ->ಸಿದ್ಧತೆ_connection) {
    CURLcode ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->ಸಿದ್ಧತೆ_connection(ದತ್ತಾಂಶ, conn);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Now create the destination name */
  peer = Curl_conn_get_destination(conn, FIRSTSOCKET);
  ಆದರೆ(!peer)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* IPv6 addresses with a scope_id (0 is default == global) have a
   * printable representation with a '%<scope_id>' suffix. */
  ಆದರೆ(peer->ipv6)
    ಆದರೆ(peer->scopeid)
      conn->destination = curl_maprintf("[%s%%%u]:%u",
        peer->hostname, peer->scopeid, peer->ಪೋರ್ಟ್);
    ಇಲ್ಲದಿದ್ದರೆ
      conn->destination = curl_maprintf("[%s]:%u",
        peer->hostname, peer->ಪೋರ್ಟ್);
  ಇಲ್ಲದಿದ್ದರೆ
    conn->destination = curl_maprintf("%s:%u", peer->hostname, peer->ಪೋರ್ಟ್);
  ಆದರೆ(!conn->destination)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  Curl_strntolower(conn->destination, conn->destination,
                   ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(conn->destination));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.scope_id)
    conn->scope_id = ದತ್ತಾಂಶ->ಗಣ.scope_id;
  ಇಲ್ಲದಿದ್ದರೆ {
    peer = Curl_conn_get_ಮೊದಲ_peer(conn, FIRSTSOCKET);
    ಆದರೆ(!peer)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    conn->scope_id = peer->scopeid;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_parse_login_details(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *login, ಸ್ಥಿರಾಂಕ size_t len,
                                  ಅಕ್ಷರ **userp, ಅಕ್ಷರ **passwdp,
                                  ಅಕ್ಷರ **optionsp)
{
  ಅಕ್ಷರ *ubuf = ಖಾಲಿ;
  ಅಕ್ಷರ *pbuf = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *psep = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *osep = ಖಾಲಿ;
  size_t ulen;
  size_t plen;
  size_t olen;

  DEBUGASSERT(userp);
  DEBUGASSERT(passwdp);

  /* Attempt to find the password separator */
  psep = memchr(login, ':', len);

  /* Attempt to find the options separator */
  ಆದರೆ(optionsp)
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
  ಆದರೆ(!ubuf)
    ಹೋಗು error;

  /* Clone the password portion buffer */
  ಆದರೆ(psep) {
    pbuf = curlx_memdup0(&psep[1], plen);
    ಆದರೆ(!pbuf)
      ಹೋಗು error;
  }

  /* Allocate the options portion buffer */
  ಆದರೆ(optionsp) {
    ಅಕ್ಷರ *obuf = ಖಾಲಿ;
    ಆದರೆ(olen) {
      obuf = curlx_memdup0(&osep[1], olen);
      ಆದರೆ(!obuf)
        ಹೋಗು error;
    }
    *optionsp = obuf;
  }
  *userp = ubuf;
  *passwdp = pbuf;
  ಹಿಂತಿರುಗಿ CURLE_OK;
error:
  curlx_free(ubuf);
  curlx_free(pbuf);
  ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}

/*
 * Set the login details so they are available in the connection
 */
ಸ್ಥಿರ CURLcode url_ಗಣ_conn_login(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ connectdata *conn)
{
  /* If our protocol needs a password and we have none, use the defaults */
  ಆದರೆ((conn->scheme->flags & PROTOPT_NEEDSPWD) && !conn->creds) {
    Curl_peer_link(&conn->creds_origin, ದತ್ತಾಂಶ->state.origin);
    ಆದರೆ(ದತ್ತಾಂಶ->state.creds)
      Curl_creds_link(&conn->creds, ದತ್ತಾಂಶ->state.creds);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ Curl_creds_create(CURL_DEFAULT_USER, CURL_DEFAULT_PASSWORD,
                               ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, CREDS_NONE, &conn->creds);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(conn->scheme->flags & PROTOPT_CREDSPERREQUEST)) {
    /* for protocols that do not handle credentials per request,
     * the connection credentials are set by the initial transfer. */
    Curl_peer_link(&conn->creds_origin, ದತ್ತಾಂಶ->state.origin);
    Curl_creds_link(&conn->creds, ದತ್ತಾಂಶ->state.creds);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Parses one "connect to" string in the form:
 * "HOST:PORT:CONNECT-TO-HOST:CONNECT-TO-PORT".
 */
ಸ್ಥಿರ CURLcode parse_connect_ಗಮ್ಯ_string(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_peer *dest,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *conn_ಗಮ್ಯ_ಸಾಲು,
                                        ರಚನೆ Curl_peer **pvia_dest)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = conn_ಗಮ್ಯ_ಸಾಲು;
  bool host_ಹೊಂದಿಕೆ = FALSE;
  bool ಪೋರ್ಟ್_ಹೊಂದಿಕೆ = FALSE;

  *pvia_dest = ಖಾಲಿ;

  ಆದರೆ(*ptr == ':') {
    /* an empty hostname always matches */
    host_ಹೊಂದಿಕೆ = TRUE;
    ptr++;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* check whether the URL's hostname matches. Use the URL hostname
     * when it was an IPv6 address. Otherwise use the connection's hostname
     * that has IDN conversion. */
    size_t hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(dest->hostname);
    host_ಹೊಂದಿಕೆ = curl_strnequal(ptr, dest->hostname, hlen);
    ಆದರೆ(!host_ಹೊಂದಿಕೆ && (dest->user_hostname != dest->hostname)) {
      /* hostname was normalized, could be IPv6 or IDN */
      hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(dest->user_hostname);
      host_ಹೊಂದಿಕೆ = curl_strnequal(ptr, dest->user_hostname, hlen);
    }
    host_ಹೊಂದಿಕೆ = host_ಹೊಂದಿಕೆ && ptr[hlen] == ':';
    ಆದರೆ(host_ಹೊಂದಿಕೆ)
      ptr += hlen + 1;
  }

  ಆದರೆ(host_ಹೊಂದಿಕೆ) {
    ಆದರೆ(*ptr == ':') {
      /* an empty port always matches */
      ಪೋರ್ಟ್_ಹೊಂದಿಕೆ = TRUE;
      ptr++;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* check whether the URL's port matches */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr_ಮುಂದಿನ = strchr(ptr, ':');
      ಆದರೆ(ptr_ಮುಂದಿನ) {
        curl_off_t ಪೋರ್ಟ್_ಗಮ್ಯ_ಹೊಂದಿಕೆ;
        ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&ptr, &ಪೋರ್ಟ್_ಗಮ್ಯ_ಹೊಂದಿಕೆ, 0xffff) &&
           ((uint16_t)ಪೋರ್ಟ್_ಗಮ್ಯ_ಹೊಂದಿಕೆ == dest->ಪೋರ್ಟ್)) {
          ಪೋರ್ಟ್_ಹೊಂದಿಕೆ = TRUE;
        }
        ptr = ptr_ಮುಂದಿನ + 1;
      }
    }
  }

  ಆದರೆ(host_ಹೊಂದಿಕೆ && ಪೋರ್ಟ್_ಹೊಂದಿಕೆ && ptr && *ptr)
    ಫಲಿತಾಂಶ = Curl_peer_from_connect_ಗಮ್ಯ(ದತ್ತಾಂಶ, dest, ptr, pvia_dest);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* With `conn->origin` known, determine if we should talk to that
 * directly or via another peer. This is the result of inspecting
 * the "connect to" slist and "alt-svc" settings. */
ಸ್ಥಿರ CURLcode url_ಗಣ_conn_peer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ Curl_peer *origin = conn->origin;
  ರಚನೆ Curl_peer *ಮೂಲಕ_peer = ಖಾಲಿ;
  ರಚನೆ curl_slist *conn_ಗಮ್ಯ_ನಮೂದು = ದತ್ತಾಂಶ->ಗಣ.connect_ಗಮ್ಯ;

  DEBUGASSERT(!conn->ಮೂಲಕ_peer);
  Curl_peer_unlink(&conn->ಮೂಲಕ_peer);

  ಆಗ(conn_ಗಮ್ಯ_ನಮೂದು && !ಮೂಲಕ_peer) {
    ಫಲಿತಾಂಶ = parse_connect_ಗಮ್ಯ_string(ದತ್ತಾಂಶ, origin, conn_ಗಮ್ಯ_ನಮೂದು->ದತ್ತಾಂಶ,
                                     &ಮೂಲಕ_peer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    conn_ಗಮ್ಯ_ನಮೂದು = conn_ಗಮ್ಯ_ನಮೂದು->ಮುಂದಿನ;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  ಆದರೆ(ದತ್ತಾಂಶ->asi && !ಮೂಲಕ_peer &&
     ((conn->scheme->protocol == CURLPROTO_HTTPS) ||
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
      /* allow debug builds to circumvent the HTTPS restriction */
      getenv("CURL_ALTSVC_HTTP")
#ಇಲ್ಲದಿದ್ದರೆ
      0
#ಆದರೆ_ಅಂತ್ಯ
       )) {
    /* no connect_to match, try alt-svc! */
    ಎಣಿಕೆ alpnid srcalpnid = ALPN_none;
    bool ದೊರಕಿತು = FALSE;
    ರಚನೆ altsvc *as = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ allowed_alpns = ALPN_none;
    ರಚನೆ http_negotiation *neg = &ದತ್ತಾಂಶ->state.http_neg;
    bool same_dest = FALSE;

    DEBUGF(infof(ದತ್ತಾಂಶ, "Alt-svc check wanted=%x, allowed=%x",
                 neg->wanted, neg->allowed));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP3
    ಆದರೆ(neg->allowed & CURL_HTTP_V3x)
      allowed_alpns |= ALPN_h3;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
    ಆದರೆ(neg->allowed & CURL_HTTP_V2x)
      allowed_alpns |= ALPN_h2;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(neg->allowed & CURL_HTTP_V1x)
      allowed_alpns |= ALPN_h1;
    allowed_alpns &= (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->asi->flags;

    DEBUGF(infof(ದತ್ತಾಂಶ, "check Alt-Svc for host '%s'", origin->hostname));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP3
    ಆದರೆ(!ದೊರಕಿತು && (neg->wanted & CURL_HTTP_V3x)) {
      srcalpnid = ALPN_h3;
      ದೊರಕಿತು = Curl_altsvc_lookup(ದತ್ತಾಂಶ->asi,
                               origin, ALPN_h3, /* from */
                               &as /*ಗಮ್ಯ*/,
                               allowed_alpns, &same_dest);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
    ಆದರೆ(!ದೊರಕಿತು && (neg->wanted & CURL_HTTP_V2x) &&
       !neg->h2_ಪೂರ್ವ_knowledge) {
      srcalpnid = ALPN_h2;
      ದೊರಕಿತು = Curl_altsvc_lookup(ದತ್ತಾಂಶ->asi,
                               origin, ALPN_h2, /* from */
                               &as /*ಗಮ್ಯ*/,
                               allowed_alpns, &same_dest);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!ದೊರಕಿತು && (neg->wanted & CURL_HTTP_V1x) &&
       !neg->only_10) {
      srcalpnid = ALPN_h1;
      ದೊರಕಿತು = Curl_altsvc_lookup(ದತ್ತಾಂಶ->asi,
                               origin, ALPN_h1, /* from */
                               &as /*ಗಮ್ಯ*/,
                               allowed_alpns, &same_dest);
    }

    ಆದರೆ(ದೊರಕಿತು && same_dest) {
      /* same destination, but more HTTPS version options */
      ಆಯ್ಕೆ(as->dst.alpnid) {
      ಸ್ಥಿತಿ ALPN_h1:
        neg->wanted |= CURL_HTTP_V1x;
        neg->preferred = CURL_HTTP_V1x;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPN_h2:
        neg->wanted |= CURL_HTTP_V2x;
        neg->preferred = CURL_HTTP_V2x;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPN_h3:
        neg->wanted |= CURL_HTTP_V3x;
        neg->preferred = CURL_HTTP_V3x;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* should not be possible */
        ನಿಲ್ಲಿಸು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದೊರಕಿತು) {
      ಫಲಿತಾಂಶ = Curl_peer_create(ದತ್ತಾಂಶ, conn->origin->scheme,
                                as->dst.host, as->dst.ಪೋರ್ಟ್,
                                &ಮೂಲಕ_peer);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      infof(ದತ್ತಾಂಶ, "Alt-svc connecting from [%s]%s:%u to [%s]%s:%u",
            Curl_alpnid2str(srcalpnid), origin->hostname, origin->ಪೋರ್ಟ್,
            Curl_alpnid2str(as->dst.alpnid),
            ಮೂಲಕ_peer->hostname, ಮೂಲಕ_peer->ಪೋರ್ಟ್);
      conn->ಬಿಟ್ಟುಗಳು.altused = TRUE;
      ಆದರೆ(srcalpnid != as->dst.alpnid) {
        /* protocol version switch */
        ಆಯ್ಕೆ(as->dst.alpnid) {
        ಸ್ಥಿತಿ ALPN_h1:
          neg->wanted = neg->allowed = CURL_HTTP_V1x;
          neg->only_10 = FALSE;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ ALPN_h2:
          neg->wanted = neg->allowed = CURL_HTTP_V2x;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ ALPN_h3:
          conn->transport_wanted = TRNSPRT_QUIC;
          neg->wanted = neg->allowed = CURL_HTTP_V3x;
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ: /* should not be possible */
          ನಿಲ್ಲಿಸು;
        }
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(ಮೂಲಕ_peer)
    conn->ಮೂಲಕ_peer = ಮೂಲಕ_peer;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Adjust reused connection settings to the transfer/needle.
 */
ಸ್ಥಿರ ಶೂನ್ಯ url_conn_reuse_adjust(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *needle)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  /* get the user+password information from the needle since it may
   * be new for this request even when we reuse conn */
  ಆದರೆ(needle->creds) {
    /* use the new username and password though */
    Curl_creds_link(&conn->creds, needle->creds);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  /* use the new proxy username and proxy password though */
  Curl_creds_link(&conn->http_proxy.creds, needle->http_proxy.creds);
  Curl_creds_link(&conn->socks_proxy.creds, needle->socks_proxy.creds);
#ಆದರೆ_ಅಂತ್ಯ

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
  Curl_peer_link(&conn->ಮೂಲಕ_peer, needle->ಮೂಲಕ_peer);
  Curl_peer_link(&conn->origin2, needle->origin2);
  Curl_peer_link(&conn->ಮೂಲಕ_peer2, needle->ಮೂಲಕ_peer2);
}

ಸ್ಥಿರ ಶೂನ್ಯ conn_meta_freeentry(ಶೂನ್ಯ *p)
{
  (ಶೂನ್ಯ)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

ಸ್ಥಿರ CURLcode url_create_needle(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata **pneedle)
{
  ರಚನೆ connectdata *needle = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool ಜಾಲ_scheme = TRUE; /* almost all are */

  /* Allocate a temporary connection data struct (needle) and fill in for
     comparison purposes. */
  needle = allocate_conn(ದತ್ತಾಂಶ);
  ಆದರೆ(!needle) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Do the unfailable inits first, before checks that may early return */
  Curl_hash_init(&needle->meta_hash, 23, CURL_HASH_TYPE_ಬೈಟುಗಳು,
                 conn_meta_freeentry);

  /*************************************************************
   * Determine `conn->origin` and populate `data->state.up` and
   * other URL related properties.
   *************************************************************/
  ಫಲಿತಾಂಶ = url_ಗಣ_conn_origin_etc(ದತ್ತಾಂಶ, needle);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  DEBUGASSERT(needle->origin);
  ಜಾಲ_scheme = !(needle->origin->scheme->flags & PROTOPT_NONETWORK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  /*************************************************************
   * Set UDS first. It overrides "via_peer" and proxy settings.
   *************************************************************/
  ಆದರೆ(ಜಾಲ_scheme && CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_UNIX_SOCKET_ಮಾರ್ಗ)) {
    ಫಲಿತಾಂಶ = Curl_peer_uds_create(
      needle->origin->scheme, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_UNIX_SOCKET_ಮಾರ್ಗ),
      (bool)ದತ್ತಾಂಶ->ಗಣ.abstract_unix_socket, &needle->ಮೂಲಕ_peer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_UNIX_SOCKETS */

  ಆದರೆ(ಜಾಲ_scheme && !needle->ಮೂಲಕ_peer) {
    /*************************************************************
     * If the `via_peer` is not already set (via UDS above),
     * determine if we talk to `conn->origin` directly or use
     * `conn->via_peer` using "connect to" and "alt-svc" properties.
     *************************************************************/
    ಫಲಿತಾಂಶ = url_ಗಣ_conn_peer(ದತ್ತಾಂಶ, needle);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /*************************************************************
   * Check whether the host and the "connect to host" are equal.
   * Do this after the hostnames have been IDN-converted and
   * before initializing the proxy.
   *************************************************************/
  ಆದರೆ(Curl_peer_equal(needle->origin, needle->ಮೂಲಕ_peer)) {
    Curl_peer_unlink(&needle->ಮೂಲಕ_peer);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  /* Going via a unix socket ignores any proxy settings */
  ಆದರೆ(ಜಾಲ_scheme &&
     (!needle->ಮೂಲಕ_peer || !needle->ಮೂಲಕ_peer->unix_socket)) {
    ಫಲಿತಾಂಶ = Curl_proxy_init_conn(ದತ್ತಾಂಶ, needle);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */

  ಫಲಿತಾಂಶ = url_ಗಣ_conn_login(ದತ್ತಾಂಶ, needle); /* default credentials */
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  ಫಲಿತಾಂಶ = ಸಿದ್ಧತೆ_connection_internals(ದತ್ತಾಂಶ, needle);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(needle->scheme->flags & PROTOPT_ALPN) {
    /* The protocol wants it, so set the bits if enabled in the easy handle
       (default) */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl_enable_alpn)
      needle->ಬಿಟ್ಟುಗಳು.tls_enable_alpn = TRUE;
  }

  ಆದರೆ(ಜಾಲ_scheme) {
    /* Setup callbacks for network connections */
    needle->recv[FIRSTSOCKET] = Curl_cf_recv;
    needle->send[FIRSTSOCKET] = Curl_cf_send;
    needle->recv[SECONDARYSOCKET] = Curl_cf_recv;
    needle->send[SECONDARYSOCKET] = Curl_cf_send;
    needle->ಬಿಟ್ಟುಗಳು.tcp_fastopen = ದತ್ತಾಂಶ->ಗಣ.tcp_fastopen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
    ಆದರೆ(Curl_conn_get_ಮೊದಲ_peer(needle, FIRSTSOCKET)->unix_socket)
      needle->transport_wanted = TRNSPRT_UNIX;
#ಆದರೆ_ಅಂತ್ಯ
  }

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    DEBUGASSERT(needle);
    DEBUGASSERT(needle->origin);
    *pneedle = needle;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    *pneedle = ಖಾಲಿ;
    ಆದರೆ(needle)
      Curl_conn_free(ದತ್ತಾಂಶ, needle);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode url_ಗಣ_ದತ್ತಾಂಶ_origin_and_creds(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  CURLU *uh;
  CURLUcode uc = CURLUE_OK;
  bool use_ಗಣ_uh = (ದತ್ತಾಂಶ->ಗಣ.uh && !ದತ್ತಾಂಶ->state.this_is_a_follow);
  uint16_t ಪೋರ್ಟ್_override = ದತ್ತಾಂಶ->state.allow_ಪೋರ್ಟ್ ? ದತ್ತಾಂಶ->ಗಣ.use_ಪೋರ್ಟ್ : 0;
  uint32_t scope_id = 0;

  /*************************************************************
   * Check input data
   *************************************************************/
  ಆದರೆ(!Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url)) {
    ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಮೇಲೆ_free(ದತ್ತಾಂಶ); /* cleanup previous leftovers first */

  /* parse the URL */
  ಆದರೆ(use_ಗಣ_uh)
    uh = ದತ್ತಾಂಶ->state.uh = curl_url_dup(ದತ್ತಾಂಶ->ಗಣ.uh);
  ಇಲ್ಲದಿದ್ದರೆ
    uh = ದತ್ತಾಂಶ->state.uh = curl_url();
  ಆದರೆ(!uh) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(!use_ಗಣ_uh) {
    bool prepend_sheme = FALSE;
    /* special-case when 'file' is default as a normal path is not a valid URL
       without the 'file' scheme present */
    ಆದರೆ(curl_strequal("file", CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DEFAULT_PROTOCOL)) &&
       !Curl_is_absolute_url(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url), ಖಾಲಿ, 0, TRUE))
      prepend_sheme = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ {
      uc = curl_url_ಗಣ(uh, CURLUPART_URL, Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url),
                        (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (ದತ್ತಾಂಶ->ಗಣ.disallow_username_ಒಳ_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (ದತ್ತಾಂಶ->ಗಣ.ಮಾರ್ಗ_as_is ? CURLU_ಮಾರ್ಗ_AS_IS : 0));
      ಆದರೆ(uc) {
        failf(ದತ್ತಾಂಶ, "URL rejected: %s", curl_url_strerror(uc));
        ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
        ಹೋಗು ಹೊರಹೋಗು;
      }

      ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DEFAULT_PROTOCOL)) {
        /* check if we there was a scheme provided */
        ಅಕ್ಷರ *scheme;
        uc = curl_url_get(uh, CURLUPART_SCHEME, &scheme,
                          CURLU_NO_GUESS_SCHEME);
        ಆದರೆ(uc == CURLUE_NO_SCHEME)
          prepend_sheme = TRUE;
        curl_free(scheme);
        ಆದರೆ(uc && uc != CURLUE_NO_SCHEME) {
          ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
          ಹೋಗು ಹೊರಹೋಗು;
        }
      }
    }

    ಆದರೆ(prepend_sheme) {
      ಅಕ್ಷರ *url = curl_maprintf("%s://%s",
                                CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DEFAULT_PROTOCOL),
                                Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));
      ಆದರೆ(!url) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      uc = curl_url_ಗಣ(uh, CURLUPART_URL, url,
                        (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)
                        CURLU_GUESS_SCHEME |
                        CURLU_NON_SUPPORT_SCHEME |
                        (ದತ್ತಾಂಶ->ಗಣ.disallow_username_ಒಳ_url ?
                         CURLU_DISALLOW_USER : 0) |
                        (ದತ್ತಾಂಶ->ಗಣ.ಮಾರ್ಗ_as_is ? CURLU_ಮಾರ್ಗ_AS_IS : 0));
      curl_free(url);
      ಆದರೆ(uc) {
        failf(ದತ್ತಾಂಶ, "URL rejected: %s", curl_url_strerror(uc));
        ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    {
      ಅಕ್ಷರ *newurl;
      /* after it was parsed, and possibly given a new scheme, get the
         normalized version */
      uc = curl_url_get(uh, CURLUPART_URL, &newurl, CURLU_GET_ಖಾಲಿಗಳು);
      ಆದರೆ(uc) {
        ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(prepend_sheme)
        infof(ದತ್ತಾಂಶ, "Applied default protocol '%s'",
              CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DEFAULT_PROTOCOL));
      Curl_bufref_ಗಣ(&ದತ್ತಾಂಶ->state.url, newurl, 0, curl_free);
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  scope_id = ದತ್ತಾಂಶ->ಗಣ.scope_id;
#ಆದರೆ_ಅಂತ್ಯ

  /* `uh` is now as the connection should use it, probably. */
  ಫಲಿತಾಂಶ = Curl_peer_from_url(uh, ದತ್ತಾಂಶ, ಪೋರ್ಟ್_override, scope_id,
                              &ದತ್ತಾಂಶ->state.origin);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  /* The origin might get changed when HSTS applies */
  ಫಲಿತಾಂಶ = hsts_upgrade(ದತ್ತಾಂಶ, uh, ಪೋರ್ಟ್_override, scope_id);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* When the transfers initial_origin is not set, this is the initial
   * request. Remember this starting point. */
  ಆದರೆ(!ದತ್ತಾಂಶ->state.initial_origin)
    Curl_peer_link(&ದತ್ತಾಂಶ->state.initial_origin, ದತ್ತಾಂಶ->state.origin);

  uc = curl_url_get(uh, CURLUPART_ಮಾರ್ಗ, &ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ, CURLU_URLENCODE);
  ಆದರೆ(uc) {
    ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    ಹೋಗು ಹೊರಹೋಗು;
  }
  uc = curl_url_get(uh, CURLUPART_QUERY, &ದತ್ತಾಂಶ->state.ಮೇಲೆ.query,
                    CURLU_GET_ಖಾಲಿಗಳು);
  ಆದರೆ(uc && (uc != CURLUE_NO_QUERY)) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  uc = curl_url_get(uh, CURLUPART_OPTIONS, &ದತ್ತಾಂಶ->state.ಮೇಲೆ.options,
                    CURLU_URLDECODE);
  ಆದರೆ(uc && (uc != CURLUE_NO_OPTIONS)) {
    ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = url_ಗಣ_ದತ್ತಾಂಶ_creds(ದತ್ತಾಂಶ, uh);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode url_ಹೊಂದಿಕೆ_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *needle,
                               ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  memset(m, 0, ಗಾತ್ರ(*m));
  m->ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  m->needle = needle;
  m->now = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  m->may_multiplex = xfer_may_multiplex(ದತ್ತಾಂಶ, needle);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  m->want_ntlm_http =
    (ದತ್ತಾಂಶ->state.authhost.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  m->want_proxy_ntlm_http =
    needle->http_proxy.creds &&
    (ದತ್ತಾಂಶ->state.authproxy.want & CURLAUTH_NTLM) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_SPNEGO)
  m->want_nego_http =
    (ದತ್ತಾಂಶ->state.authhost.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP) &&
    Curl_auth_allowed_ಗಮ್ಯ_host(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  m->want_proxy_nego_http =
    needle->http_proxy.creds &&
    (ದತ್ತಾಂಶ->state.authproxy.want & CURLAUTH_NEGOTIATE) &&
    (needle->scheme->protocol & PROTO_FAMILY_HTTP);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
  m->require_tls = ದತ್ತಾಂಶ->ಗಣ.use_ssl >= CURLUSESSL_CONTROL;
  m->may_tls = ದತ್ತಾಂಶ->ಗಣ.use_ssl > CURLUSESSL_NONE;

  /* Get a shallow setup of filter configs for connection cache matching */
  ಹಿಂತಿರುಗಿ Curl_ssl_filter_config_tmp_init(ದತ್ತಾಂಶ, needle->origin,
                                         &m->ssl_config,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
                                         &m->proxy_ssl_config);
#ಇಲ್ಲದಿದ್ದರೆ
                                         ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ url_ಹೊಂದಿಕೆ_destroy(ರಚನೆ url_conn_ಹೊಂದಿಕೆ *m)
{
  Curl_ssl_config_cleanup(&m->ssl_config);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  Curl_ssl_config_cleanup(&m->proxy_ssl_config);
#ಆದರೆ_ಅಂತ್ಯ
}

/**
 * Find an existing connection for the transfer or create a new one.
 * Returns
 * - CURLE_OK on success with a connection attached to data
 * - CURLE_NO_CONNECTION_AVAILABLE when connection limits apply or when
 *   a suitable connection has not determined its multiplex capability.
 * - a fatal error
 */
ಸ್ಥಿರ CURLcode url_ಕಂಡುಹಿಡಿ_or_create_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ connectdata *needle = ಖಾಲಿ;
  ರಚನೆ url_conn_ಹೊಂದಿಕೆ ಹೊಂದಿಕೆ;
  bool ಹೊಂದಿಕೆ_initialized = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  /* create the template connection for transfer data. Use this needle to
   * find an existing connection or, if none exists, convert needle
   * to a full connection and attach it to data. */
  ಫಲಿತಾಂಶ = url_create_needle(ದತ್ತಾಂಶ, &needle);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  DEBUGASSERT(needle);

  ಫಲಿತಾಂಶ = url_ಹೊಂದಿಕೆ_init(ದತ್ತಾಂಶ, needle, &ಹೊಂದಿಕೆ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಹೊಂದಿಕೆ_initialized = TRUE;

  /***********************************************************************
   * file: is a special case in that it does not need a network connection
   ***********************************************************************/
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FILE
  ಆದರೆ(needle->scheme->flags & PROTOPT_NONETWORK) {
    bool ಮುಗಿದ;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(needle->scheme->ಸತತ->connect_it);
    ದತ್ತಾಂಶ->info.conn_scheme = needle->scheme->name;
    /* conn_protocol can only provide "old" protocols */
    ದತ್ತಾಂಶ->info.conn_protocol = needle->scheme->protocol & CURLPROTO_ಮುಖವಾಡ;
    ಫಲಿತಾಂಶ = needle->scheme->ಸತತ->connect_it(ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    /* Setup a "faked" transfer that will do nothing */
    ಫಲಿತಾಂಶ = Curl_cpool_ಕೂಡಿಸು(ದತ್ತಾಂಶ, needle, 0, 0, pnow);
    Curl_attach_connection(ದತ್ತಾಂಶ, needle, TRUE);
    needle = ಖಾಲಿ;
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      /* Setup whatever necessary for a resumed transfer */
      ಫಲಿತಾಂಶ = ಸಿದ್ಧತೆ_range(ದತ್ತಾಂಶ);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
        ಫಲಿತಾಂಶ = Curl_init_transfer(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn);
      }
    }

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      DEBUGASSERT(ದತ್ತಾಂಶ->conn->scheme->ಸತತ->ಮುಗಿದ);
      /* we ignore the return code for the protocol-specific DONE */
      (ಶೂನ್ಯ)ದತ್ತಾಂಶ->conn->scheme->ಸತತ->ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, FALSE);
    }
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  /*************************************************************
   * Reuse of existing connection is not allowed when
   * - connect_only is set or
   * - reuse_fresh is set and this is not a follow-up request
   *   (like with HTTP followlocation)
   *************************************************************/
  ಆದರೆ((!ದತ್ತಾಂಶ->ಗಣ.reuse_fresh || ದತ್ತಾಂಶ->state.followlocation) &&
     !ದತ್ತಾಂಶ->ಗಣ.connect_only) {
    /* Ok, try to find and attach an existing one */
    ಆದರೆ(url_attach_existing(ದತ್ತಾಂಶ, needle, &ಹೊಂದಿಕೆ, pnow)) {
      /* We attached an existing connection for this transfer. Copy
       * over transfer specific properties over from needle. */
      ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
      VERBOSE(bool tls_upgraded =
        (!(needle->origin->scheme->flags & PROTOPT_SSL) &&
         Curl_conn_is_ssl(conn, FIRSTSOCKET)));

      DEBUGASSERT(conn);
      conn->ಬಿಟ್ಟುಗಳು.reuse = TRUE;
      url_conn_reuse_adjust(ದತ್ತಾಂಶ, needle);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
      infof(ದತ್ತಾಂಶ, "Reusing existing %s: connection%s with %s %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            (conn->socks_proxy.peer || conn->http_proxy.peer) ?
            "proxy" : "host",
            conn->socks_proxy.peer ? conn->socks_proxy.peer->user_hostname :
            conn->http_proxy.peer ? conn->http_proxy.peer->user_hostname :
            conn->origin->hostname);
#ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "Reusing existing %s: connection%s with host %s",
            conn->origin->scheme->name,
            tls_upgraded ? " (upgraded to SSL)" : "",
            conn->origin->hostname);
#ಆದರೆ_ಅಂತ್ಯ
    }
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
    /* We have not attached an existing connection. `needle` may become
     * the connection to use, if
     * - we are not waiting on an existing connection result
     * - the total/host limits on active connections allow it
     */
    ಆದರೆ(ಹೊಂದಿಕೆ.wait_pipe) {
      /* There is a connection that *might* become usable for multiplexing
         "soon", and we wait for that */
      infof(ದತ್ತಾಂಶ, "Waiting on connection to negotiate possible multiplexing.");
      ಫಲಿತಾಂಶ = CURLE_NO_CONNECTION_ಲಭ್ಯ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    /* Add needle to conn pool, observing the limits.
     * Initializes connection_id.
     * May fail due to limits with CURLE_NO_CONNECTION_AVAILABLE. */
    ಫಲಿತಾಂಶ = Curl_cpool_ಕೂಡಿಸು(ದತ್ತಾಂಶ, needle,
                            ದತ್ತಾಂಶ->multi->max_ಒಟ್ಟು_connections,
                            ದತ್ತಾಂಶ->multi->max_host_connections,
                            pnow);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    /* All fine, attach it and forget needle */
    Curl_attach_connection(ದತ್ತಾಂಶ, needle, TRUE);
    needle = ಖಾಲಿ;

    /* Clone the SSL filter configs into the new connection. */
    ಫಲಿತಾಂಶ = Curl_ssl_conn_config_clone(&ಹೊಂದಿಕೆ.ssl_config,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
                                        &ಹೊಂದಿಕೆ.proxy_ssl_config,
#ಇಲ್ಲದಿದ್ದರೆ
                                        ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
                                        ದತ್ತಾಂಶ->conn);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "Error: clone connection SSL config"));
      ಹೋಗು ಹೊರಹೋಗು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
    /* If NTLM is requested in a part of this connection, make sure we do not
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    ಆದರೆ((ದತ್ತಾಂಶ->state.authhost.picked & CURLAUTH_NTLM) &&
       ದತ್ತಾಂಶ->state.authhost.ಮುಗಿದ) {
      infof(ದತ್ತಾಂಶ, "NTLM picked AND auth done set, clear picked");
      ದತ್ತಾಂಶ->state.authhost.picked = CURLAUTH_NONE;
      ದತ್ತಾಂಶ->state.authhost.ಮುಗಿದ = FALSE;
    }

    ಆದರೆ((ದತ್ತಾಂಶ->state.authproxy.picked & CURLAUTH_NTLM) &&
       ದತ್ತಾಂಶ->state.authproxy.ಮುಗಿದ) {
      infof(ದತ್ತಾಂಶ, "NTLM-proxy picked AND auth done set, clear picked");
      ದತ್ತಾಂಶ->state.authproxy.picked = CURLAUTH_NONE;
      ದತ್ತಾಂಶ->state.authproxy.ಮುಗಿದ = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  /* COMMON setup code for reused and new connections from here on. */

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  ಫಲಿತಾಂಶ = Curl_init_transfer(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Setup whatever necessary for a resumed transfer */
  ಫಲಿತಾಂಶ = ಸಿದ್ಧತೆ_range(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* persist the scheme and handler the transfer is using */
  ದತ್ತಾಂಶ->info.conn_scheme = ದತ್ತಾಂಶ->conn->scheme->name;
  /* conn_protocol can only provide "old" protocols */
  ದತ್ತಾಂಶ->info.conn_protocol = ದತ್ತಾಂಶ->conn->scheme->protocol & CURLPROTO_ಮುಖವಾಡ;
  ದತ್ತಾಂಶ->info.ಬಳಸಿದ_proxy =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_PROXY
    0
#ಇಲ್ಲದಿದ್ದರೆ
    (ದತ್ತಾಂಶ->conn->socks_proxy.peer || ದತ್ತಾಂಶ->conn->http_proxy.peer)
#ಆದರೆ_ಅಂತ್ಯ
    ;

  /* Lastly, inform connection filters that a new transfer is attached */
  ಫಲಿತಾಂಶ = Curl_conn_ev_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  ಆದರೆ(ಹೊಂದಿಕೆ_initialized)
    url_ಹೊಂದಿಕೆ_destroy(&ಹೊಂದಿಕೆ);
  ಆದರೆ(needle)
    Curl_conn_free(ದತ್ತಾಂಶ, needle);
  DEBUGASSERT(ಫಲಿತಾಂಶ || ದತ್ತಾಂಶ->conn);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *pconnected)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = ಖಾಲಿ;
  *pconnected = FALSE;

  /* Set the request to virgin state based on transfer settings */
  Curl_req_hard_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
  /* Determine the origin of the transfer and what credentials to use */
  ಫಲಿತಾಂಶ = url_ಗಣ_ದತ್ತಾಂಶ_origin_and_creds(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(!ದತ್ತಾಂಶ->state.origin) { /* just make really sure */
    DEBUGASSERT(0);
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Get or create a connection for the transfer. */
  pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
  Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_POSTQUEUE, *pnow);
  ಫಲಿತಾಂಶ = url_ಕಂಡುಹಿಡಿ_or_create_conn(ದತ್ತಾಂಶ, pnow);
  conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(!ದತ್ತಾಂಶ->conn) { /* just make really sure */
    DEBUGASSERT(0);
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.reuse) {
    ಆದರೆ(conn->attached_xfers > 1)
      /* multiplexed */
      *pconnected = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn->scheme->flags & PROTOPT_NONETWORK) {
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_NAMELOOKUP);
    *pconnected = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_conn_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ, conn, FIRSTSOCKET, CURL_CF_SSL_DEFAULT);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_ಶೀರ್ಷಿಕೆಗಳು_init(ದತ್ತಾಂಶ);
    CURL_TRC_M(ದತ್ತಾಂಶ, "Curl_conn_setup() -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_NO_CONNECTION_ಲಭ್ಯ)
    DEBUGASSERT(!conn);

  ಆದರೆ(ಫಲಿತಾಂಶ && conn) {
    /* We are not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_detach_connection(ದತ್ತಾಂಶ);
    Curl_conn_close(ದತ್ತಾಂಶ, conn, TRUE);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_init_transfer() is called each time before the transfer starts - to
 * prepare for a transfer, sometimes multiple times on the same Curl_easy.
 * Make sure nothing in here depends on stuff that is setup dynamically for
 * the transfer.
 *
 * Allow this function to get called with 'conn' set to NULL.
 */

CURLcode Curl_init_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(conn) {
    conn->ಬಿಟ್ಟುಗಳು.do_ಇನ್ನಷ್ಟು = FALSE; /* by default there is no curl_do_more() to
                                   use */
    /* if the protocol used does not support wildcards, switch it off */
    ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch &&
       !(conn->scheme->flags & PROTOPT_WILDCARD))
      ದತ್ತಾಂಶ->state.wildcardmatch = FALSE;
  }

  ದತ್ತಾಂಶ->state.ಮುಗಿದ = FALSE; /* *_done() is not called yet */

  ದತ್ತಾಂಶ->req.no_body = ದತ್ತಾಂಶ->ಗಣ.opt_no_body;
  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    ದತ್ತಾಂಶ->state.httpreq = HTTPREQ_ತಲೆ;

  ಫಲಿತಾಂಶ = Curl_req_ಆರಂಭ(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    Curl_pgrsReset(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ defined(USE_HTTP2) || defined(USE_HTTP3)

ಶೂನ್ಯ Curl_ದತ್ತಾಂಶ_ಆದ್ಯತೆ_clear_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ದತ್ತಾಂಶ->state.ತೂಕ = 0;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTP2 || USE_HTTP3 */

CURLcode Curl_conn_meta_ಗಣ(ರಚನೆ connectdata *conn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                            ಶೂನ್ಯ *meta_ದತ್ತಾಂಶ, Curl_meta_dtor *meta_dtor)
{
  ಆದರೆ(!Curl_hash_add2(&conn->meta_hash, ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1,
                     meta_ದತ್ತಾಂಶ, meta_dtor)) {
    meta_dtor(ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1, meta_ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_conn_meta_remove(ರಚನೆ connectdata *conn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ)
{
  Curl_hash_delete(&conn->meta_hash, ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1);
}

ಶೂನ್ಯ *Curl_conn_meta_get(ರಚನೆ connectdata *conn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ)
{
  ಹಿಂತಿರುಗಿ Curl_hash_pick(&conn->meta_hash, ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1);
}

ರಚನೆ Curl_easy *Curl_get_admin(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_easy *admin;

  ಆದರೆ(!ದತ್ತಾಂಶ->mid) /* already an admin handle */
    admin = ದತ್ತಾಂಶ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->multi)
    admin = ದತ್ತಾಂಶ->multi->admin;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->multi_easy)
    admin = ದತ್ತಾಂಶ->multi_easy->admin;
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(0); /* we do not want this. does it happen? */
    admin = ದತ್ತಾಂಶ;
  }
  ಆದರೆ(admin != ದತ್ತಾಂಶ) {
    admin->ಗಣ.conn_max_idle_ms = ದತ್ತಾಂಶ->ಗಣ.conn_max_idle_ms;
    admin->ಗಣ.conn_max_age_ms = ದತ್ತಾಂಶ->ಗಣ.conn_max_age_ms;
    admin->ಗಣ.upkeep_interval_ms = ದತ್ತಾಂಶ->ಗಣ.upkeep_interval_ms;
    admin->ಗಣ.timeout = ದತ್ತಾಂಶ->ಗಣ.timeout;
    admin->ಗಣ.server_response_timeout = ದತ್ತಾಂಶ->ಗಣ.server_response_timeout;
    admin->ಗಣ.no_signal = ದತ್ತಾಂಶ->ಗಣ.no_signal;
  }
  ಹಿಂತಿರುಗಿ admin;
}

CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2)
{
  ಆದರೆ(r1 && (r1 != CURLE_AGAIN))
    ಹಿಂತಿರುಗಿ r1;
  ಆದರೆ(r2 && (r2 != CURLE_AGAIN))
    ಹಿಂತಿರುಗಿ r2;
  ಹಿಂತಿರುಗಿ r1;
}
