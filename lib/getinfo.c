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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "getinfo.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "connect.h" /* Curl_getconnectinfo() */
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "url.h" /* Curl_uc_to_curlcode() */
#അന്തര്ഭാവ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
ശൂന്യം Curl_initinfo(ഘടന Curl_easy *data)
{
  ഘടന Progress *pro = &data->progress;
  ഘടന PureInfo *info = &data->info;

  memset(&pro->delta, 0, വലുപ്പം(pro->delta));
  memset(&pro->total, 0, വലുപ്പം(pro->total));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->header_size = 0;
  info->request_size = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  memset(&info->primary, 0, വലുപ്പം(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

സ്ഥിതം CURLcode effective_url(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം **param_charp)
{
  സ്ഥിരം അക്ഷരം *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ശൂന്യ : "";
  എങ്കിൽ(s) {
    അക്ഷരം *url = ശൂന്യ;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    എങ്കിൽ(!u)
      തിരികെ CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    എങ്കിൽ(!uc)
      uc = curl_url_set(u, CURLUPART_USER, ശൂന്യ, 0);
    എങ്കിൽ(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, ശൂന്യ, 0);
    എങ്കിൽ(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    എങ്കിൽ(uc || !url)
      തിരികെ uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    അല്ലെങ്കിൽ {
      curl_free(url);
      *param_charp = s;
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode getinfo_char(ഘടന Curl_easy *data, CURLINFO info,
                             സ്ഥിരം അക്ഷരം **param_charp)
{
  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_EFFECTIVE_URL:
    തിരികെ effective_url(data, param_charp);
  അവസ്ഥ CURLINFO_EFFECTIVE_METHOD: {
    സ്ഥിരം അക്ഷരം *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    എങ്കിൽ(!m) {
      എങ്കിൽ(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      അല്ലെങ്കിൽ {
        തിരഞ്ഞെടുപ്പ്(data->state.httpreq) {
        അവസ്ഥ HTTPREQ_POST:
        അവസ്ഥ HTTPREQ_POST_FORM:
        അവസ്ഥ HTTPREQ_POST_MIME:
          m = "POST";
          നിർത്തുക;
        അവസ്ഥ HTTPREQ_PUT:
          m = "PUT";
          നിർത്തുക;
        സ്ഥിരസ്ഥിതി: /* this should never happen */
        അവസ്ഥ HTTPREQ_GET:
          m = "GET";
          നിർത്തുക;
        അവസ്ഥ HTTPREQ_HEAD:
          m = "HEAD";
          നിർത്തുക;
        }
      }
#endif
    }
    *param_charp = m;
  }
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PRIVATE:
    *param_charp = (സ്ഥിരം അക്ഷരം *)data->set.private_data;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#അല്ലെങ്കിൽ
    *param_charp = ശൂന്യ;
#endif
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#അല്ലെങ്കിൽ
    *param_charp = ശൂന്യ;
#endif
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#അല്ലെങ്കിൽ
    *param_charp = ശൂന്യ;
#endif
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode getinfo_long(ഘടന Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  സംഘം {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  സ്ഥിരം അക്ഷരം *envstr;

  /* use another variable for this to allow different values */
  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    എങ്കിൽ(envstr) {
      curl_off_t val;
      curlx_str_സംഖ്യ(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  അവസ്ഥ CURLINFO_HEADER_SIZE:
  അവസ്ഥ CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    എങ്കിൽ(envstr) {
      curl_off_t val;
      curlx_str_സംഖ്യ(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
#endif

  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_FILETIME:
    എങ്കിൽ(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#എങ്കിൽ !defined(MSDOS) && !defined(__AMIGA__)
    അല്ലെങ്കിൽ എങ്കിൽ(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    അല്ലെങ്കിൽ
      *param_longp = (long)data->info.filetime;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#അല്ലെങ്കിൽ
    *param_longp = 0;
#endif
    നിർത്തുക;
  അവസ്ഥ CURLINFO_REDIRECT_എണ്ണം:
    *param_longp = data->state.followlocation;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_അക്കം_CONNECTS:
#എങ്കിൽ SIZEOF_LONG < SIZEOF_CURL_OFF_T
    എങ്കിൽ(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    അല്ലെങ്കിൽ
#endif
      *param_longp = (long)data->info.numconnects;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, ശൂന്യ);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    എങ്കിൽ(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    അല്ലെങ്കിൽ
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    എങ്കിൽ(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    അല്ലെങ്കിൽ
      *param_longp = -1;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    എങ്കിൽ(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    അല്ലെങ്കിൽ
      *param_longp = -1;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONDITION_UNMET:
    എങ്കിൽ(data->info.httpcode == 304)
      *param_longp = 1L;
    അല്ലെങ്കിൽ
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    നിർത്തുക;
#ifndef CURL_DISABLE_RTSP
  അവസ്ഥ CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    നിർത്തുക;
#അല്ലെങ്കിൽ
  അവസ്ഥ CURLINFO_RTSP_CLIENT_CSEQ:
  അവസ്ഥ CURLINFO_RTSP_SERVER_CSEQ:
  അവസ്ഥ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    നിർത്തുക;
#endif
  അവസ്ഥ CURLINFO_HTTP_VERSION:
    തിരഞ്ഞെടുപ്പ്(data->info.httpversion) {
    അവസ്ഥ 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      നിർത്തുക;
    അവസ്ഥ 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      നിർത്തുക;
    അവസ്ഥ 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      നിർത്തുക;
    അവസ്ഥ 30:
      *param_longp = CURL_HTTP_VERSION_3;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      *param_longp = CURL_HTTP_VERSION_NONE;
      നിർത്തുക;
    }
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#അല്ലെങ്കിൽ
      data->info.used_proxy
#endif
      ;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }

  തിരികെ CURLE_OK;
}

#define DOUBLE_SECS(x) ((ഇരട്ട)(x) / 1000000)

സ്ഥിതം CURLcode getinfo_offt(ഘടന Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  സ്ഥിരം അക്ഷരം *envstr;
  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_TOTAL_TIME_T:
  അവസ്ഥ CURLINFO_NAMELOOKUP_TIME_T:
  അവസ്ഥ CURLINFO_CONNECT_TIME_T:
  അവസ്ഥ CURLINFO_APPCONNECT_TIME_T:
  അവസ്ഥ CURLINFO_PRETRANSFER_TIME_T:
  അവസ്ഥ CURLINFO_POSTTRANSFER_TIME_T:
  അവസ്ഥ CURLINFO_QUEUE_TIME_T:
  അവസ്ഥ CURLINFO_STARTTRANSFER_TIME_T:
  അവസ്ഥ CURLINFO_REDIRECT_TIME_T:
  അവസ്ഥ CURLINFO_SPEED_DOWNLOAD_T:
  അവസ്ഥ CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    എങ്കിൽ(envstr) {
      curl_off_t val;
      curlx_str_സംഖ്യ(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
#endif
  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    നിർത്തുക;
   അവസ്ഥ CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_XFER_ID:
    *param_offt = data->id;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode getinfo_double(ഘടന Curl_easy *data, CURLINFO info,
                               ഇരട്ട *param_doublep)
{
#ifdef DEBUGBUILD
  സ്ഥിരം അക്ഷരം *envstr;

  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_TOTAL_TIME:
  അവസ്ഥ CURLINFO_NAMELOOKUP_TIME:
  അവസ്ഥ CURLINFO_CONNECT_TIME:
  അവസ്ഥ CURLINFO_APPCONNECT_TIME:
  അവസ്ഥ CURLINFO_PRETRANSFER_TIME:
  അവസ്ഥ CURLINFO_STARTTRANSFER_TIME:
  അവസ്ഥ CURLINFO_REDIRECT_TIME:
  അവസ്ഥ CURLINFO_SPEED_DOWNLOAD:
  അവസ്ഥ CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    എങ്കിൽ(envstr) {
      curl_off_t val;
      curlx_str_സംഖ്യ(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (ഇരട്ട)val;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
#endif
  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SIZE_UPLOAD:
    *param_doublep = (ഇരട്ട)data->progress.ul.cur_size;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (ഇരട്ട)data->progress.dl.cur_size;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (ഇരട്ട)data->progress.dl.speed;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SPEED_UPLOAD:
    *param_doublep = (ഇരട്ട)data->progress.ul.speed;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (ഇരട്ട)data->progress.dl.total_size : -1;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (ഇരട്ട)data->progress.ul.total_size : -1;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode getinfo_slist(ഘടന Curl_easy *data, CURLINFO info,
                              ഘടന curl_slist **param_slistp)
{
  സംഘം {
    ഘടന curl_certinfo *to_certinfo;
    ഘടന curl_slist    *to_slist;
  } ptr;

  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    നിർത്തുക;
  അവസ്ഥ CURLINFO_TLS_SESSION:
  അവസ്ഥ CURLINFO_TLS_SSL_PTR: {
    പൂർണ്ണസംഖ്യ query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ഘടന curl_tlssessioninfo **tsip = (ഘടന curl_tlssessioninfo **)
      param_slistp;
    ഘടന curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    എങ്കിൽ(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ശൂന്യ;
    }
  }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode getinfo_socket(ഘടന Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  തിരഞ്ഞെടുപ്പ്(info) {
  അവസ്ഥ CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, ശൂന്യ);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNKNOWN_OPTION;
  }

  തിരികെ CURLE_OK;
}

CURLcode Curl_getinfo(ഘടന Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = ശൂന്യ;
  ഇരട്ട *param_doublep = ശൂന്യ;
  curl_off_t *param_offt = ശൂന്യ;
  സ്ഥിരം അക്ഷരം **param_charp = ശൂന്യ;
  ഘടന curl_slist **param_slistp = ശൂന്യ;
  curl_socket_t *param_socketp = ശൂന്യ;
  പൂർണ്ണസംഖ്യ type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(!data)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (പൂർണ്ണസംഖ്യ)info;
  തിരഞ്ഞെടുപ്പ്(type) {
  അവസ്ഥ CURLINFO_STRING:
    param_charp = va_arg(arg, സ്ഥിരം അക്ഷരം **);
    എങ്കിൽ(param_charp)
      result = getinfo_char(data, info, param_charp);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    എങ്കിൽ(param_longp)
      result = getinfo_long(data, info, param_longp);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, ഇരട്ട *);
    എങ്കിൽ(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    എങ്കിൽ(param_offt)
      result = getinfo_offt(data, info, param_offt);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SLIST:
    param_slistp = va_arg(arg, ഘടന curl_slist **);
    എങ്കിൽ(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    നിർത്തുക;
  അവസ്ഥ CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    എങ്കിൽ(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    result = CURLE_UNKNOWN_OPTION;
    നിർത്തുക;
  }

  va_end(arg);

  തിരികെ result;
}
