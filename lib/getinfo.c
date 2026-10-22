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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "getinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h" /* Curl_getconnectinfo() */
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h" /* Curl_uc_to_curlcode() */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
ਖਾਲੀ Curl_initinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ਤਰੱਕੀ_2 *pro = &ਅੰਕੜੇ->ਤਰੱਕੀ;
  ਬਣਤਰ ਸ਼ੁੱਧ_Info *info = &ਅੰਕੜੇ->info;

  memset(&pro->delta, 0, ਆਕਾਰ(pro->delta));
  memset(&pro->ਕੁੱਲ, 0, ਆਕਾਰ(pro->ਕੁੱਲ));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->ਸਿਰਲੇਖ_ਮਾਪ = 0;
  info->request_ਮਾਪ = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  memset(&info->primary, 0, ਆਕਾਰ(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->ਵਰਤਿਆ_proxy = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  Curl_ssl_free_certinfo(ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ CURLcode effective_url(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ **param_charp)
{
  ਅਚਲ ਅੱਖਰ *s = Curl_bufref_ptr(&ਅੰਕੜੇ->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ਨਿਰਾ : "";
  ਜੇ(s) {
    ਅੱਖਰ *url = ਨਿਰਾ;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    ਜੇ(!u)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    uc = curl_url_ਸੈੱਟ(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (ਅੰਕੜੇ->ਸੈੱਟ.ਰਸਤਾ_as_is ? CURLU_ਰਸਤਾ_AS_IS : 0));
    ਜੇ(!uc)
      uc = curl_url_ਸੈੱਟ(u, CURLUPART_USER, ਨਿਰਾ, 0);
    ਜੇ(!uc)
      uc = curl_url_ਸੈੱਟ(u, CURLUPART_PASSWORD, ਨਿਰਾ, 0);
    ਜੇ(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ);

    curl_url_cleanup(u);

    ਜੇ(uc || !url)
      ਵਾਪਸ uc ? Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc) : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(strcmp(url, s)) {
      /* they are different */
      curlx_free(ਅੰਕੜੇ->info.effective_url);
      ਅੰਕੜੇ->info.effective_url = url;
      *param_charp = url;
    }
    ਨਹੀਂਤਾਂ {
      curl_free(url);
      *param_charp = s;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode getinfo_char(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info,
                             ਅਚਲ ਅੱਖਰ **param_charp)
{
  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_EFFECTIVE_URL:
    ਵਾਪਸ effective_url(ਅੰਕੜੇ, param_charp);
  ਹਾਲਤ CURLINFO_EFFECTIVE_ਵਿਧੀ: {
    ਅਚਲ ਅੱਖਰ *m = CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST);
    ਜੇ(!m) {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.opt_no_body)
        m = "HEAD";
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
      ਨਹੀਂਤਾਂ {
        ਚੋਣ(ਅੰਕੜੇ->state.httpreq) {
        ਹਾਲਤ HTTPREQ_POST:
        ਹਾਲਤ HTTPREQ_POST_FORM:
        ਹਾਲਤ HTTPREQ_POST_MIME:
          m = "POST";
          ਰੋਕੋ;
        ਹਾਲਤ HTTPREQ_PUT:
          m = "PUT";
          ਰੋਕੋ;
        ਮੂਲ: /* this should never happen */
        ਹਾਲਤ HTTPREQ_GET:
          m = "GET";
          ਰੋਕੋ;
        ਹਾਲਤ HTTPREQ_ਸਿਰ:
          m = "HEAD";
          ਰੋਕੋ;
        }
      }
#ਜੇ_ਅੰਤ
    }
    *param_charp = m;
  }
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONTENT_TYPE:
    *param_charp = ਅੰਕੜੇ->info.contenttype;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਨਿੱਜੀ:
    *param_charp = (ਅਚਲ ਅੱਖਰ *)ਅੰਕੜੇ->ਸੈੱਟ.ਨਿੱਜੀ_ਅੰਕੜੇ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_FTP_ਇੰਦਰਾਜ_ਰਸਤਾ:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = ਅੰਕੜੇ->state.most_recent_ftp_entrypath;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = ਅੰਕੜੇ->info.wouldredirect;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&ਅੰਕੜੇ->state.referer);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = ਅੰਕੜੇ->info.primary.remote_ip;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = ਅੰਕੜੇ->info.primary.local_ip;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_RTSP_SESSION_ID:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(ਅੰਕੜੇ, STRING_RTSP_SESSION_ID);
#ਨਹੀਂਤਾਂ
    *param_charp = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_SCHEME:
    *param_charp = ਅੰਕੜੇ->info.conn_scheme;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CAPATH:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_ਰਸਤਾ
    *param_charp = CURL_CA_ਰਸਤਾ;
#ਨਹੀਂਤਾਂ
    *param_charp = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CAINFO:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#ਨਹੀਂਤਾਂ
    *param_charp = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode getinfo_long(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info,
                             ਲੰਮਾ *param_longp)
{
  curl_socket_t sockfd;

  ਸੰਘ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ਮੰਜ਼ਿਲ_ulong;
    ਲੰਮਾ          *ਮੰਜ਼ਿਲ_long;
  } lptr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਅਚਲ ਅੱਖਰ *envstr;

  /* use another variable for this to allow different values */
  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_LOCAL_ਪੋਰਟ:
    envstr = getenv("CURL_TIME");
    ਜੇ(envstr) {
      curl_off_t val;
      curlx_str_ਸੰਖਿਆ(&envstr, &val, ਸਮਾਂ_T_MAX);
      *param_longp = (ਲੰਮਾ)val;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਸਿਰਲੇਖ_ਮਾਪ:
  ਹਾਲਤ CURLINFO_REQUEST_ਮਾਪ:
    envstr = getenv("CURL_DEBUG_SIZE");
    ਜੇ(envstr) {
      curl_off_t val;
      curlx_str_ਸੰਖਿਆ(&envstr, &val, LONG_MAX);
      *param_longp = (ਲੰਮਾ)val;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
#ਜੇ_ਅੰਤ

  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_RESPONSE_ਕੋਡ:
    *param_longp = ਅੰਕੜੇ->info.httpcode;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_HTTP_CONNECTCODE:
    *param_longp = ਅੰਕੜੇ->info.httpproxycode;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_FILETIME:
    ਜੇ(ਅੰਕੜੇ->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#ਜੇ !defined(MSDOS) && !defined(__AMIGA__)
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
      *param_longp = (ਲੰਮਾ)ਅੰਕੜੇ->info.filetime;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਸਿਰਲੇਖ_ਮਾਪ:
    *param_longp = (ਲੰਮਾ)ਅੰਕੜੇ->info.ਸਿਰਲੇਖ_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_REQUEST_ਮਾਪ:
    *param_longp = (ਲੰਮਾ)ਅੰਕੜੇ->info.request_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_SSL_VERIFYRESULT:
    *param_longp = ਅੰਕੜੇ->ਸੈੱਟ.ssl.certverifyresult;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PROXY_SSL_VERIFYRESULT:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    *param_longp = ਅੰਕੜੇ->ਸੈੱਟ.proxy_ssl.certverifyresult;
#ਨਹੀਂਤਾਂ
    *param_longp = 0;
#ਜੇ_ਅੰਤ
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_REDIRECT_ਗਿਣਤੀ:
    *param_longp = ਅੰਕੜੇ->state.followlocation;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_HTTPAUTH_AVAIL:
    lptr.ਮੰਜ਼ਿਲ_long = param_longp;
    *lptr.ਮੰਜ਼ਿਲ_ulong = ਅੰਕੜੇ->info.httpauthavail;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PROXYAUTH_AVAIL:
    lptr.ਮੰਜ਼ਿਲ_long = param_longp;
    *lptr.ਮੰਜ਼ਿਲ_ulong = ਅੰਕੜੇ->info.proxyauthavail;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_HTTPAUTH_ਵਰਤਿਆ:
    lptr.ਮੰਜ਼ਿਲ_long = param_longp;
    *lptr.ਮੰਜ਼ਿਲ_ulong = ਅੰਕੜੇ->info.httpauthpicked;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PROXYAUTH_ਵਰਤਿਆ:
    lptr.ਮੰਜ਼ਿਲ_long = param_longp;
    *lptr.ਮੰਜ਼ਿਲ_ulong = ਅੰਕੜੇ->info.proxyauthpicked;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_OS_ERRNO:
    *param_longp = ਅੰਕੜੇ->state.os_errno;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਅੰਕ_CONNECTS:
#ਜੇ SIZEOF_LONG < SIZEOF_CURL_OFF_T
    ਜੇ(ਅੰਕੜੇ->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      *param_longp = (ਲੰਮਾ)ਅੰਕੜੇ->info.numconnects;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(ਅੰਕੜੇ, ਨਿਰਾ);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    ਜੇ(sockfd != CURL_SOCKET_BAD)
      *param_longp = (ਲੰਮਾ)sockfd;
    ਨਹੀਂਤਾਂ
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PRIMARY_ਪੋਰਟ:
    /* Return the (remote) port of the most recent (primary) connection */
    ਜੇ(CUR_IP_QUAD_HAS_PORTS(&ਅੰਕੜੇ->info.primary))
      *param_longp = ਅੰਕੜੇ->info.primary.remote_ਪੋਰਟ;
    ਨਹੀਂਤਾਂ
      *param_longp = -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_LOCAL_ਪੋਰਟ:
    /* Return the local port of the most recent (primary) connection */
    ਜੇ(CUR_IP_QUAD_HAS_PORTS(&ਅੰਕੜੇ->info.primary))
      *param_longp = ਅੰਕੜੇ->info.primary.local_ਪੋਰਟ;
    ਨਹੀਂਤਾਂ
      *param_longp = -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PROXY_ERROR:
    *param_longp = (ਲੰਮਾ)ਅੰਕੜੇ->info.pxcode;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONDITION_UNMET:
    ਜੇ(ਅੰਕੜੇ->info.httpcode == 304)
      *param_longp = 1L;
    ਨਹੀਂਤਾਂ
      /* return if the condition prevented the document to get transferred */
      *param_longp = ਅੰਕੜੇ->info.timecond ? 1L : 0L;
    ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ਹਾਲਤ CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_client_CSeq;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = ਅੰਕੜੇ->state.rtsp_ਅਗਲਾ_server_CSeq;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = ਅੰਕੜੇ->state.rtsp_CSeq_recv;
    ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
  ਹਾਲਤ CURLINFO_RTSP_CLIENT_CSEQ:
  ਹਾਲਤ CURLINFO_RTSP_SERVER_CSEQ:
  ਹਾਲਤ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURLINFO_HTTP_ਸੰਸਕਰਣ:
    ਚੋਣ(ਅੰਕੜੇ->info.httpversion) {
    ਹਾਲਤ 10:
      *param_longp = CURL_HTTP_ਸੰਸਕਰਣ_1_0;
      ਰੋਕੋ;
    ਹਾਲਤ 11:
      *param_longp = CURL_HTTP_ਸੰਸਕਰਣ_1_1;
      ਰੋਕੋ;
    ਹਾਲਤ 20:
      *param_longp = CURL_HTTP_ਸੰਸਕਰਣ_2_0;
      ਰੋਕੋ;
    ਹਾਲਤ 30:
      *param_longp = CURL_HTTP_ਸੰਸਕਰਣ_3;
      ਰੋਕੋ;
    ਮੂਲ:
      *param_longp = CURL_HTTP_ਸੰਸਕਰਣ_NONE;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PROTOCOL:
    *param_longp = (ਲੰਮਾ)ਅੰਕੜੇ->info.conn_protocol;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਵਰਤਿਆ_PROXY:
    *param_longp =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
      0
#ਨਹੀਂਤਾਂ
      ਅੰਕੜੇ->info.ਵਰਤਿਆ_proxy
#ਜੇ_ਅੰਤ
      ;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }

  ਵਾਪਸ CURLE_OK;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOUBLE_SECS(x) ((ਦੁੱਗਣਾ)(x) / 1000000)

ਸਥਿਰ CURLcode getinfo_offt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info,
                             curl_off_t *param_offt)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਅਚਲ ਅੱਖਰ *envstr;
  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_ਕੁੱਲ_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_NAMELOOKUP_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_CONNECT_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_APPCONNECT_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_PRETRANSFER_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_POSTTRANSFER_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_ਕਤਾਰ_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_STARTTRANSFER_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_REDIRECT_ਸਮਾਂ_T:
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_DOWNLOAD_T:
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    ਜੇ(envstr) {
      curl_off_t val;
      curlx_str_ਸੰਖਿਆ(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
#ਜੇ_ਅੰਤ
  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)ਅੰਕੜੇ->info.filetime;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਮਾਪ_DELIVERED:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.deliver;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਮਾਪ_UPLOAD_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ul.cur_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਮਾਪ_DOWNLOAD_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.dl.cur_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_DOWNLOAD_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਰਫਤਾਰ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_UPLOAD_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਰਫਤਾਰ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONTENT_ਲੰਮਾਈ_DOWNLOAD_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.dl_ਮਾਪ_ਗਿਆਤ ?
      ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਕੁੱਲ_ਮਾਪ : -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONTENT_ਲੰਮਾਈ_UPLOAD_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ul_ਮਾਪ_ਗਿਆਤ ?
      ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਕੁੱਲ_ਮਾਪ : -1;
    ਰੋਕੋ;
   ਹਾਲਤ CURLINFO_ਕੁੱਲ_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.spent_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_NAMELOOKUP_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.nslookup_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONNECT_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.connect_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_APPCONNECT_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.appconnect_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PRETRANSFER_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.pretransfer_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_POSTTRANSFER_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.posttransfer_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_STARTTRANSFER_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.starttransfer_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਕਤਾਰ_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.queued_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_REDIRECT_ਸਮਾਂ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startredirect_us;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_RETRY_AFTER:
    *param_offt = ਅੰਕੜੇ->info.retry_after;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_XFER_ID:
    *param_offt = ਅੰਕੜੇ->id;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONN_ID:
    *param_offt = ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_EARLYDATA_ਭੇਜਿਆ_T:
    *param_offt = ਅੰਕੜੇ->ਤਰੱਕੀ.earlydata_ਭੇਜਿਆ;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode getinfo_double(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info,
                               ਦੁੱਗਣਾ *param_doublep)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਅਚਲ ਅੱਖਰ *envstr;

  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_ਕੁੱਲ_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_NAMELOOKUP_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_CONNECT_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_APPCONNECT_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_PRETRANSFER_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_STARTTRANSFER_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_REDIRECT_ਸਮਾਂ:
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_DOWNLOAD:
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_UPLOAD:
    envstr = getenv("CURL_TIME");
    ਜੇ(envstr) {
      curl_off_t val;
      curlx_str_ਸੰਖਿਆ(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (ਦੁੱਗਣਾ)val;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
#ਜੇ_ਅੰਤ
  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_ਕੁੱਲ_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.spent_us);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_NAMELOOKUP_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.nslookup_us);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONNECT_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.connect_us);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_APPCONNECT_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.appconnect_us);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_PRETRANSFER_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.pretransfer_us);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_STARTTRANSFER_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.starttransfer_us);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਮਾਪ_UPLOAD:
    *param_doublep = (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.ul.cur_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਮਾਪ_DOWNLOAD:
    *param_doublep = (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.dl.cur_ਮਾਪ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_DOWNLOAD:
    *param_doublep = (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਰਫਤਾਰ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_ਰਫਤਾਰ_UPLOAD:
    *param_doublep = (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਰਫਤਾਰ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONTENT_ਲੰਮਾਈ_DOWNLOAD:
    *param_doublep = ਅੰਕੜੇ->ਤਰੱਕੀ.dl_ਮਾਪ_ਗਿਆਤ ?
      (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਕੁੱਲ_ਮਾਪ : -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CONTENT_ਲੰਮਾਈ_UPLOAD:
    *param_doublep = ਅੰਕੜੇ->ਤਰੱਕੀ.ul_ਮਾਪ_ਗਿਆਤ ?
      (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਕੁੱਲ_ਮਾਪ : -1;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_REDIRECT_ਸਮਾਂ:
    *param_doublep = DOUBLE_SECS(ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startredirect_us);
    ਰੋਕੋ;

  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode getinfo_slist(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info,
                              ਬਣਤਰ curl_slist **param_slistp)
{
  ਸੰਘ {
    ਬਣਤਰ curl_certinfo *ਮੰਜ਼ਿਲ_certinfo;
    ਬਣਤਰ curl_slist    *ਮੰਜ਼ਿਲ_slist;
  } ptr;

  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.ਮੰਜ਼ਿਲ_certinfo = &ਅੰਕੜੇ->info.certs;
    *param_slistp = ptr.ਮੰਜ਼ਿਲ_slist;
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_TLS_SESSION:
  ਹਾਲਤ CURLINFO_TLS_SSL_PTR: {
    ਪੂਰਨ query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ਬਣਤਰ curl_tlssessioninfo **tsip = (ਬਣਤਰ curl_tlssessioninfo **)
      param_slistp;
    ਬਣਤਰ curl_tlssessioninfo *tsi = &ਅੰਕੜੇ->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    ਜੇ(!Curl_conn_get_ssl_info(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ਨਿਰਾ;
    }
  }
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode getinfo_socket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  ਚੋਣ(info) {
  ਹਾਲਤ CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(ਅੰਕੜੇ, ਨਿਰਾ);
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNKNOWN_OPTION;
  }

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_getinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLINFO info, ...)
{
  va_list arg;
  ਲੰਮਾ *param_longp = ਨਿਰਾ;
  ਦੁੱਗਣਾ *param_doublep = ਨਿਰਾ;
  curl_off_t *param_offt = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ **param_charp = ਨਿਰਾ;
  ਬਣਤਰ curl_slist **param_slistp = ਨਿਰਾ;
  curl_socket_t *param_socketp = ਨਿਰਾ;
  ਪੂਰਨ type;
  CURLcode ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (ਪੂਰਨ)info;
  ਚੋਣ(type) {
  ਹਾਲਤ CURLINFO_STRING:
    param_charp = va_arg(arg, ਅਚਲ ਅੱਖਰ **);
    ਜੇ(param_charp)
      ਨਤੀਜਾ = getinfo_char(ਅੰਕੜੇ, info, param_charp);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_LONG:
    param_longp = va_arg(arg, ਲੰਮਾ *);
    ਜੇ(param_longp)
      ਨਤੀਜਾ = getinfo_long(ਅੰਕੜੇ, info, param_longp);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, ਦੁੱਗਣਾ *);
    ਜੇ(param_doublep)
      ਨਤੀਜਾ = getinfo_double(ਅੰਕੜੇ, info, param_doublep);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    ਜੇ(param_offt)
      ਨਤੀਜਾ = getinfo_offt(ਅੰਕੜੇ, info, param_offt);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_SLIST:
    param_slistp = va_arg(arg, ਬਣਤਰ curl_slist **);
    ਜੇ(param_slistp)
      ਨਤੀਜਾ = getinfo_slist(ਅੰਕੜੇ, info, param_slistp);
    ਰੋਕੋ;
  ਹਾਲਤ CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    ਜੇ(param_socketp)
      ਨਤੀਜਾ = getinfo_socket(ਅੰਕੜੇ, info, param_socketp);
    ਰੋਕੋ;
  ਮੂਲ:
    ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
    ਰੋਕੋ;
  }

  va_end(arg);

  ਵਾਪਸ ਨਤੀਜਾ;
}
