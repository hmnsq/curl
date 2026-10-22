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
ಶೂನ್ಯ Curl_initinfo(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Progress *pro = &data->progress;
  ಸಂರಚನಾ PureInfo *info = &data->info;

  memset(&pro->delta, 0, ಪರಿಮಾಣ(pro->delta));
  memset(&pro->total, 0, ಪರಿಮಾಣ(pro->total));
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

  memset(&info->primary, 0, ಪರಿಮಾಣ(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

ಸ್ಥಿರ CURLcode effective_url(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ **param_charp)
{
  ಧ್ರುವ ವರ್ಣ *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ರಿಕ್ತ : "";
  ಯದಿ(s) {
    ವರ್ಣ *url = ರಿಕ್ತ;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    ಯದಿ(!u)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    ಯದಿ(!uc)
      uc = curl_url_set(u, CURLUPART_USER, ರಿಕ್ತ, 0);
    ಯದಿ(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, ರಿಕ್ತ, 0);
    ಯದಿ(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    ಯದಿ(uc || !url)
      ಪ್ರತ್ಯಾಗಮ uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    ಯದಿ(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    ಅನ್ಯಥಾ {
      curl_free(url);
      *param_charp = s;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_char(ಸಂರಚನಾ Curl_easy *data, CURLINFO info,
                             ಧ್ರುವ ವರ್ಣ **param_charp)
{
  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_EFFECTIVE_URL:
    ಪ್ರತ್ಯಾಗಮ effective_url(data, param_charp);
  ಪಕ್ಷ CURLINFO_EFFECTIVE_METHOD: {
    ಧ್ರುವ ವರ್ಣ *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    ಯದಿ(!m) {
      ಯದಿ(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ(data->state.httpreq) {
        ಪಕ್ಷ HTTPREQ_POST:
        ಪಕ್ಷ HTTPREQ_POST_FORM:
        ಪಕ್ಷ HTTPREQ_POST_MIME:
          m = "POST";
          ಭಙ್ಗ;
        ಪಕ್ಷ HTTPREQ_PUT:
          m = "PUT";
          ಭಙ್ಗ;
        ಸ್ವಭಾವ: /* this should never happen */
        ಪಕ್ಷ HTTPREQ_GET:
          m = "GET";
          ಭಙ್ಗ;
        ಪಕ್ಷ HTTPREQ_HEAD:
          m = "HEAD";
          ಭಙ್ಗ;
        }
      }
#endif
    }
    *param_charp = m;
  }
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PRIVATE:
    *param_charp = (ಧ್ರುವ ವರ್ಣ *)data->set.private_data;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#ಅನ್ಯಥಾ
    *param_charp = ರಿಕ್ತ;
#endif
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#ಅನ್ಯಥಾ
    *param_charp = ರಿಕ್ತ;
#endif
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#ಅನ್ಯಥಾ
    *param_charp = ರಿಕ್ತ;
#endif
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_long(ಸಂರಚನಾ Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  ಸಙ್ಘ {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  ಧ್ರುವ ವರ್ಣ *envstr;

  /* use another variable for this to allow different values */
  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    ಯದಿ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_HEADER_SIZE:
  ಪಕ್ಷ CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    ಯದಿ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
#endif

  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_FILETIME:
    ಯದಿ(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#ಯದಿ !defined(MSDOS) && !defined(__AMIGA__)
    ಅನ್ಯಥಾ ಯದಿ(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    ಅನ್ಯಥಾ
      *param_longp = (long)data->info.filetime;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#ಅನ್ಯಥಾ
    *param_longp = 0;
#endif
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_REDIRECT_ಎಣಿಕೆ:
    *param_longp = data->state.followlocation;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_ಅಂಕೆ_CONNECTS:
#ಯದಿ SIZEOF_LONG < SIZEOF_CURL_OFF_T
    ಯದಿ(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    ಅನ್ಯಥಾ
#endif
      *param_longp = (long)data->info.numconnects;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, ರಿಕ್ತ);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    ಯದಿ(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    ಅನ್ಯಥಾ
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    ಯದಿ(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    ಅನ್ಯಥಾ
      *param_longp = -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    ಯದಿ(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    ಅನ್ಯಥಾ
      *param_longp = -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONDITION_UNMET:
    ಯದಿ(data->info.httpcode == 304)
      *param_longp = 1L;
    ಅನ್ಯಥಾ
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    ಭಙ್ಗ;
#ifndef CURL_DISABLE_RTSP
  ಪಕ್ಷ CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    ಭಙ್ಗ;
#ಅನ್ಯಥಾ
  ಪಕ್ಷ CURLINFO_RTSP_CLIENT_CSEQ:
  ಪಕ್ಷ CURLINFO_RTSP_SERVER_CSEQ:
  ಪಕ್ಷ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURLINFO_HTTP_VERSION:
    ವಿಕಲ್ಪ(data->info.httpversion) {
    ಪಕ್ಷ 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      ಭಙ್ಗ;
    ಪಕ್ಷ 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      ಭಙ್ಗ;
    ಪಕ್ಷ 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      ಭಙ್ಗ;
    ಪಕ್ಷ 30:
      *param_longp = CURL_HTTP_VERSION_3;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      *param_longp = CURL_HTTP_VERSION_NONE;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#ಅನ್ಯಥಾ
      data->info.used_proxy
#endif
      ;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#define DOUBLE_SECS(x) ((ದ್ವಿಗುಣ)(x) / 1000000)

ಸ್ಥಿರ CURLcode getinfo_offt(ಸಂರಚನಾ Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  ಧ್ರುವ ವರ್ಣ *envstr;
  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_TOTAL_TIME_T:
  ಪಕ್ಷ CURLINFO_NAMELOOKUP_TIME_T:
  ಪಕ್ಷ CURLINFO_CONNECT_TIME_T:
  ಪಕ್ಷ CURLINFO_APPCONNECT_TIME_T:
  ಪಕ್ಷ CURLINFO_PRETRANSFER_TIME_T:
  ಪಕ್ಷ CURLINFO_POSTTRANSFER_TIME_T:
  ಪಕ್ಷ CURLINFO_QUEUE_TIME_T:
  ಪಕ್ಷ CURLINFO_STARTTRANSFER_TIME_T:
  ಪಕ್ಷ CURLINFO_REDIRECT_TIME_T:
  ಪಕ್ಷ CURLINFO_SPEED_DOWNLOAD_T:
  ಪಕ್ಷ CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    ಯದಿ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
#endif
  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    ಭಙ್ಗ;
   ಪಕ್ಷ CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_XFER_ID:
    *param_offt = data->id;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_double(ಸಂರಚನಾ Curl_easy *data, CURLINFO info,
                               ದ್ವಿಗುಣ *param_doublep)
{
#ifdef DEBUGBUILD
  ಧ್ರುವ ವರ್ಣ *envstr;

  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_TOTAL_TIME:
  ಪಕ್ಷ CURLINFO_NAMELOOKUP_TIME:
  ಪಕ್ಷ CURLINFO_CONNECT_TIME:
  ಪಕ್ಷ CURLINFO_APPCONNECT_TIME:
  ಪಕ್ಷ CURLINFO_PRETRANSFER_TIME:
  ಪಕ್ಷ CURLINFO_STARTTRANSFER_TIME:
  ಪಕ್ಷ CURLINFO_REDIRECT_TIME:
  ಪಕ್ಷ CURLINFO_SPEED_DOWNLOAD:
  ಪಕ್ಷ CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    ಯದಿ(envstr) {
      curl_off_t val;
      curlx_str_ಸಂಖ್ಯೆ(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (ದ್ವಿಗುಣ)val;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
#endif
  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SIZE_UPLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)data->progress.ul.cur_size;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)data->progress.dl.cur_size;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)data->progress.dl.speed;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SPEED_UPLOAD:
    *param_doublep = (ದ್ವಿಗುಣ)data->progress.ul.speed;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (ದ್ವಿಗುಣ)data->progress.dl.total_size : -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (ದ್ವಿಗುಣ)data->progress.ul.total_size : -1;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_slist(ಸಂರಚನಾ Curl_easy *data, CURLINFO info,
                              ಸಂರಚನಾ curl_slist **param_slistp)
{
  ಸಙ್ಘ {
    ಸಂರಚನಾ curl_certinfo *to_certinfo;
    ಸಂರಚನಾ curl_slist    *to_slist;
  } ptr;

  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_TLS_SESSION:
  ಪಕ್ಷ CURLINFO_TLS_SSL_PTR: {
    ಪೂರ್ಣಾಙ್ಕ query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ಸಂರಚನಾ curl_tlssessioninfo **tsip = (ಸಂರಚನಾ curl_tlssessioninfo **)
      param_slistp;
    ಸಂರಚನಾ curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    ಯದಿ(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ರಿಕ್ತ;
    }
  }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode getinfo_socket(ಸಂರಚನಾ Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  ವಿಕಲ್ಪ(info) {
  ಪಕ್ಷ CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, ರಿಕ್ತ);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNKNOWN_OPTION;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_getinfo(ಸಂರಚನಾ Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = ರಿಕ್ತ;
  ದ್ವಿಗುಣ *param_doublep = ರಿಕ್ತ;
  curl_off_t *param_offt = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ **param_charp = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_slist **param_slistp = ರಿಕ್ತ;
  curl_socket_t *param_socketp = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (ಪೂರ್ಣಾಙ್ಕ)info;
  ವಿಕಲ್ಪ(type) {
  ಪಕ್ಷ CURLINFO_STRING:
    param_charp = va_arg(arg, ಧ್ರುವ ವರ್ಣ **);
    ಯದಿ(param_charp)
      result = getinfo_char(data, info, param_charp);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    ಯದಿ(param_longp)
      result = getinfo_long(data, info, param_longp);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, ದ್ವಿಗುಣ *);
    ಯದಿ(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    ಯದಿ(param_offt)
      result = getinfo_offt(data, info, param_offt);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SLIST:
    param_slistp = va_arg(arg, ಸಂರಚನಾ curl_slist **);
    ಯದಿ(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    ಯದಿ(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    result = CURLE_UNKNOWN_OPTION;
    ಭಙ್ಗ;
  }

  va_end(arg);

  ಪ್ರತ್ಯಾಗಮ result;
}
