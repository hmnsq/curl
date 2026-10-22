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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "getinfo.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "connect.h" /* Curl_getconnectinfo() */
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "url.h" /* Curl_uc_to_curlcode() */
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
ಶೂನ್ಯ Curl_initinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ಪ್ರಗತಿ_2 *pro = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ;
  ರಚನೆ ಶುದ್ಧ_Info *info = &ದತ್ತಾಂಶ->info;

  memset(&pro->delta, 0, ಗಾತ್ರ(pro->delta));
  memset(&pro->ಒಟ್ಟು, 0, ಗಾತ್ರ(pro->ಒಟ್ಟು));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = 0;
  info->request_ಗಾತ್ರ = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  memset(&info->primary, 0, ಗಾತ್ರ(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->ಬಳಸಿದ_proxy = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  Curl_ssl_free_certinfo(ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ CURLcode effective_url(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **param_charp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ಖಾಲಿ : "";
  ಆದರೆ(s) {
    ಅಕ್ಷರ *url = ಖಾಲಿ;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    ಆದರೆ(!u)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    uc = curl_url_ಗಣ(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (ದತ್ತಾಂಶ->ಗಣ.ಮಾರ್ಗ_as_is ? CURLU_ಮಾರ್ಗ_AS_IS : 0));
    ಆದರೆ(!uc)
      uc = curl_url_ಗಣ(u, CURLUPART_USER, ಖಾಲಿ, 0);
    ಆದರೆ(!uc)
      uc = curl_url_ಗಣ(u, CURLUPART_PASSWORD, ಖಾಲಿ, 0);
    ಆದರೆ(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_ಖಾಲಿಗಳು);

    curl_url_cleanup(u);

    ಆದರೆ(uc || !url)
      ಹಿಂತಿರುಗಿ uc ? Curl_uc_ಗಮ್ಯ_curlcode(uc) : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(strcmp(url, s)) {
      /* they are different */
      curlx_free(ದತ್ತಾಂಶ->info.effective_url);
      ದತ್ತಾಂಶ->info.effective_url = url;
      *param_charp = url;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      curl_free(url);
      *param_charp = s;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_char(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **param_charp)
{
  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_EFFECTIVE_URL:
    ಹಿಂತಿರುಗಿ effective_url(ದತ್ತಾಂಶ, param_charp);
  ಸ್ಥಿತಿ CURLINFO_EFFECTIVE_ವಿಧಾನ: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *m = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST);
    ಆದರೆ(!m) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.opt_no_body)
        m = "HEAD";
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ(ದತ್ತಾಂಶ->state.httpreq) {
        ಸ್ಥಿತಿ HTTPREQ_POST:
        ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
        ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
          m = "POST";
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ HTTPREQ_PUT:
          m = "PUT";
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ: /* this should never happen */
        ಸ್ಥಿತಿ HTTPREQ_GET:
          m = "GET";
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ HTTPREQ_ತಲೆ:
          m = "HEAD";
          ನಿಲ್ಲಿಸು;
        }
      }
#ಆದರೆ_ಅಂತ್ಯ
    }
    *param_charp = m;
  }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONTENT_TYPE:
    *param_charp = ದತ್ತಾಂಶ->info.contenttype;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಖಾಸಗಿ:
    *param_charp = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ದತ್ತಾಂಶ->ಗಣ.ಖಾಸಗಿ_ದತ್ತಾಂಶ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_FTP_ನಮೂದು_ಮಾರ್ಗ:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = ದತ್ತಾಂಶ->info.wouldredirect;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.referer);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = ದತ್ತಾಂಶ->info.primary.remote_ip;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = ದತ್ತಾಂಶ->info.primary.local_ip;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_RTSP_SESSION_ID:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_RTSP_SESSION_ID);
#ಇಲ್ಲದಿದ್ದರೆ
    *param_charp = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_SCHEME:
    *param_charp = ದತ್ತಾಂಶ->info.conn_scheme;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CAPATH:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_ಮಾರ್ಗ
    *param_charp = CURL_CA_ಮಾರ್ಗ;
#ಇಲ್ಲದಿದ್ದರೆ
    *param_charp = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CAINFO:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#ಇಲ್ಲದಿದ್ದರೆ
    *param_charp = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_long(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info,
                             ದೀರ್ಘ *param_longp)
{
  curl_socket_t sockfd;

  ಒಕ್ಕೂಟ {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಗಮ್ಯ_ulong;
    ದೀರ್ಘ          *ಗಮ್ಯ_long;
  } lptr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *envstr;

  /* use another variable for this to allow different values */
  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_LOCAL_ಪೋರ್ಟ್:
    envstr = getenv("CURL_TIME");
    ಆದರೆ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, ಸಮಯ_T_MAX);
      *param_longp = (ದೀರ್ಘ)val;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ:
  ಸ್ಥಿತಿ CURLINFO_REQUEST_ಗಾತ್ರ:
    envstr = getenv("CURL_DEBUG_SIZE");
    ಆದರೆ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, LONG_MAX);
      *param_longp = (ದೀರ್ಘ)val;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_RESPONSE_ಸಂಕೇತ:
    *param_longp = ದತ್ತಾಂಶ->info.httpcode;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_HTTP_CONNECTCODE:
    *param_longp = ದತ್ತಾಂಶ->info.httpproxycode;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_FILETIME:
    ಆದರೆ(ದತ್ತಾಂಶ->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#ಆದರೆ !defined(MSDOS) && !defined(__AMIGA__)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
      *param_longp = (ದೀರ್ಘ)ದತ್ತಾಂಶ->info.filetime;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ:
    *param_longp = (ದೀರ್ಘ)ದತ್ತಾಂಶ->info.ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_REQUEST_ಗಾತ್ರ:
    *param_longp = (ದೀರ್ಘ)ದತ್ತಾಂಶ->info.request_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_SSL_VERIFYRESULT:
    *param_longp = ದತ್ತಾಂಶ->ಗಣ.ssl.certverifyresult;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PROXY_SSL_VERIFYRESULT:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    *param_longp = ದತ್ತಾಂಶ->ಗಣ.proxy_ssl.certverifyresult;
#ಇಲ್ಲದಿದ್ದರೆ
    *param_longp = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_REDIRECT_ಎಣಿಕೆ:
    *param_longp = ದತ್ತಾಂಶ->state.followlocation;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_HTTPAUTH_AVAIL:
    lptr.ಗಮ್ಯ_long = param_longp;
    *lptr.ಗಮ್ಯ_ulong = ದತ್ತಾಂಶ->info.httpauthavail;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PROXYAUTH_AVAIL:
    lptr.ಗಮ್ಯ_long = param_longp;
    *lptr.ಗಮ್ಯ_ulong = ದತ್ತಾಂಶ->info.proxyauthavail;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_HTTPAUTH_ಬಳಸಿದ:
    lptr.ಗಮ್ಯ_long = param_longp;
    *lptr.ಗಮ್ಯ_ulong = ದತ್ತಾಂಶ->info.httpauthpicked;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PROXYAUTH_ಬಳಸಿದ:
    lptr.ಗಮ್ಯ_long = param_longp;
    *lptr.ಗಮ್ಯ_ulong = ದತ್ತಾಂಶ->info.proxyauthpicked;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_OS_ERRNO:
    *param_longp = ದತ್ತಾಂಶ->state.os_errno;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಅಂಕೆ_CONNECTS:
#ಆದರೆ SIZEOF_LONG < SIZEOF_CURL_OFF_T
    ಆದರೆ(ದತ್ತಾಂಶ->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      *param_longp = (ದೀರ್ಘ)ದತ್ತಾಂಶ->info.numconnects;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(ದತ್ತಾಂಶ, ಖಾಲಿ);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    ಆದರೆ(sockfd != CURL_SOCKET_BAD)
      *param_longp = (ದೀರ್ಘ)sockfd;
    ಇಲ್ಲದಿದ್ದರೆ
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PRIMARY_ಪೋರ್ಟ್:
    /* Return the (remote) port of the most recent (primary) connection */
    ಆದರೆ(CUR_IP_QUAD_HAS_PORTS(&ದತ್ತಾಂಶ->info.primary))
      *param_longp = ದತ್ತಾಂಶ->info.primary.remote_ಪೋರ್ಟ್;
    ಇಲ್ಲದಿದ್ದರೆ
      *param_longp = -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_LOCAL_ಪೋರ್ಟ್:
    /* Return the local port of the most recent (primary) connection */
    ಆದರೆ(CUR_IP_QUAD_HAS_PORTS(&ದತ್ತಾಂಶ->info.primary))
      *param_longp = ದತ್ತಾಂಶ->info.primary.local_ಪೋರ್ಟ್;
    ಇಲ್ಲದಿದ್ದರೆ
      *param_longp = -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PROXY_ERROR:
    *param_longp = (ದೀರ್ಘ)ದತ್ತಾಂಶ->info.pxcode;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONDITION_UNMET:
    ಆದರೆ(ದತ್ತಾಂಶ->info.httpcode == 304)
      *param_longp = 1L;
    ಇಲ್ಲದಿದ್ದರೆ
      /* return if the condition prevented the document to get transferred */
      *param_longp = ದತ್ತಾಂಶ->info.timecond ? 1L : 0L;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  ಸ್ಥಿತಿ CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_client_CSeq;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = ದತ್ತಾಂಶ->state.rtsp_ಮುಂದಿನ_server_CSeq;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = ದತ್ತಾಂಶ->state.rtsp_CSeq_recv;
    ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿತಿ CURLINFO_RTSP_CLIENT_CSEQ:
  ಸ್ಥಿತಿ CURLINFO_RTSP_SERVER_CSEQ:
  ಸ್ಥಿತಿ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURLINFO_HTTP_ಆವೃತ್ತಿ:
    ಆಯ್ಕೆ(ದತ್ತಾಂಶ->info.httpversion) {
    ಸ್ಥಿತಿ 10:
      *param_longp = CURL_HTTP_ಆವೃತ್ತಿ_1_0;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 11:
      *param_longp = CURL_HTTP_ಆವೃತ್ತಿ_1_1;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 20:
      *param_longp = CURL_HTTP_ಆವೃತ್ತಿ_2_0;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 30:
      *param_longp = CURL_HTTP_ಆವೃತ್ತಿ_3;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      *param_longp = CURL_HTTP_ಆವೃತ್ತಿ_NONE;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PROTOCOL:
    *param_longp = (ದೀರ್ಘ)ದತ್ತಾಂಶ->info.conn_protocol;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಬಳಸಿದ_PROXY:
    *param_longp =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_PROXY
      0
#ಇಲ್ಲದಿದ್ದರೆ
      ದತ್ತಾಂಶ->info.ಬಳಸಿದ_proxy
#ಆದರೆ_ಅಂತ್ಯ
      ;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿಸು DOUBLE_SECS(x) ((ದ್ವಿಗುಣ)(x) / 1000000)

ಸ್ಥಿರ CURLcode getinfo_offt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info,
                             curl_off_t *param_offt)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *envstr;
  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_ಒಟ್ಟು_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_NAMELOOKUP_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_CONNECT_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_APPCONNECT_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_PRETRANSFER_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_POSTTRANSFER_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_ಸರತಿ_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_STARTTRANSFER_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_REDIRECT_ಸಮಯ_T:
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_DOWNLOAD_T:
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    ಆದರೆ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)ದತ್ತಾಂಶ->info.filetime;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಗಾತ್ರ_DELIVERED:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.deliver;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಗಾತ್ರ_UPLOAD_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.cur_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಗಾತ್ರ_DOWNLOAD_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.cur_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_DOWNLOAD_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ವೇಗ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_UPLOAD_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ವೇಗ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONTENT_ಉದ್ದಳತೆ_DOWNLOAD_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl_ಗಾತ್ರ_ತಿಳಿದ ?
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ಒಟ್ಟು_ಗಾತ್ರ : -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONTENT_ಉದ್ದಳತೆ_UPLOAD_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul_ಗಾತ್ರ_ತಿಳಿದ ?
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ಒಟ್ಟು_ಗಾತ್ರ : -1;
    ನಿಲ್ಲಿಸು;
   ಸ್ಥಿತಿ CURLINFO_ಒಟ್ಟು_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.spent_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_NAMELOOKUP_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.nslookup_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONNECT_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.connect_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_APPCONNECT_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.appconnect_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PRETRANSFER_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.pretransfer_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_POSTTRANSFER_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.posttransfer_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_STARTTRANSFER_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.starttransfer_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಸರತಿ_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.queued_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_REDIRECT_ಸಮಯ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startredirect_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_RETRY_AFTER:
    *param_offt = ದತ್ತಾಂಶ->info.retry_after;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_XFER_ID:
    *param_offt = ದತ್ತಾಂಶ->id;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONN_ID:
    *param_offt = ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_EARLYDATA_ಕಳುಹಿಸಿದ_T:
    *param_offt = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.earlydata_ಕಳುಹಿಸಿದ;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_double(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info,
                               ದ್ವಿಗುಣ *param_doublep)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *envstr;

  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_ಒಟ್ಟು_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_NAMELOOKUP_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_CONNECT_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_APPCONNECT_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_PRETRANSFER_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_STARTTRANSFER_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_REDIRECT_ಸಮಯ:
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_DOWNLOAD:
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_UPLOAD:
    envstr = getenv("CURL_TIME");
    ಆದರೆ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (ದ್ವಿಗುಣ)val;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_ಒಟ್ಟು_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.spent_us);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_NAMELOOKUP_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.nslookup_us);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONNECT_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.connect_us);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_APPCONNECT_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.appconnect_us);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_PRETRANSFER_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.pretransfer_us);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_STARTTRANSFER_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.starttransfer_us);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಗಾತ್ರ_UPLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.cur_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ಗಾತ್ರ_DOWNLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.cur_ಗಾತ್ರ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_DOWNLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ವೇಗ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_ವೇಗ_UPLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ವೇಗ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONTENT_ಉದ್ದಳತೆ_DOWNLOAD:
    *param_doublep = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl_ಗಾತ್ರ_ತಿಳಿದ ?
      (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ಒಟ್ಟು_ಗಾತ್ರ : -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CONTENT_ಉದ್ದಳತೆ_UPLOAD:
    *param_doublep = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul_ಗಾತ್ರ_ತಿಳಿದ ?
      (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ಒಟ್ಟು_ಗಾತ್ರ : -1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_REDIRECT_ಸಮಯ:
    *param_doublep = DOUBLE_SECS(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startredirect_us);
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_slist(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info,
                              ರಚನೆ curl_slist **param_slistp)
{
  ಒಕ್ಕೂಟ {
    ರಚನೆ curl_certinfo *ಗಮ್ಯ_certinfo;
    ರಚನೆ curl_slist    *ಗಮ್ಯ_slist;
  } ptr;

  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.ಗಮ್ಯ_certinfo = &ದತ್ತಾಂಶ->info.certs;
    *param_slistp = ptr.ಗಮ್ಯ_slist;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_TLS_SESSION:
  ಸ್ಥಿತಿ CURLINFO_TLS_SSL_PTR: {
    ಪೂರ್ಣಾಂಕ query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ರಚನೆ curl_tlssessioninfo **tsip = (ರಚನೆ curl_tlssessioninfo **)
      param_slistp;
    ರಚನೆ curl_tlssessioninfo *tsi = &ದತ್ತಾಂಶ->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    ಆದರೆ(!Curl_conn_get_ssl_info(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ಖಾಲಿ;
    }
  }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  ಆಯ್ಕೆ(info) {
  ಸ್ಥಿತಿ CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(ದತ್ತಾಂಶ, ಖಾಲಿ);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNKNOWN_OPTION;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_getinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLINFO info, ...)
{
  va_list arg;
  ದೀರ್ಘ *param_longp = ಖಾಲಿ;
  ದ್ವಿಗುಣ *param_doublep = ಖಾಲಿ;
  curl_off_t *param_offt = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **param_charp = ಖಾಲಿ;
  ರಚನೆ curl_slist **param_slistp = ಖಾಲಿ;
  curl_socket_t *param_socketp = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ type;
  CURLcode ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (ಪೂರ್ಣಾಂಕ)info;
  ಆಯ್ಕೆ(type) {
  ಸ್ಥಿತಿ CURLINFO_STRING:
    param_charp = va_arg(arg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **);
    ಆದರೆ(param_charp)
      ಫಲಿತಾಂಶ = getinfo_char(ದತ್ತಾಂಶ, info, param_charp);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_LONG:
    param_longp = va_arg(arg, ದೀರ್ಘ *);
    ಆದರೆ(param_longp)
      ಫಲಿತಾಂಶ = getinfo_long(ದತ್ತಾಂಶ, info, param_longp);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, ದ್ವಿಗುಣ *);
    ಆದರೆ(param_doublep)
      ಫಲಿತಾಂಶ = getinfo_double(ದತ್ತಾಂಶ, info, param_doublep);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    ಆದರೆ(param_offt)
      ಫಲಿತಾಂಶ = getinfo_offt(ದತ್ತಾಂಶ, info, param_offt);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_SLIST:
    param_slistp = va_arg(arg, ರಚನೆ curl_slist **);
    ಆದರೆ(param_slistp)
      ಫಲಿತಾಂಶ = getinfo_slist(ದತ್ತಾಂಶ, info, param_slistp);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    ಆದರೆ(param_socketp)
      ಫಲಿತಾಂಶ = getinfo_socket(ದತ್ತಾಂಶ, info, param_socketp);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
    ನಿಲ್ಲಿಸು;
  }

  va_end(arg);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
