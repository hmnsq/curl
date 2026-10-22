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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "getinfo.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "connect.h" /* Curl_getconnectinfo() */
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "url.h" /* Curl_uc_to_curlcode() */
#অন্তর্ভাব "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
শূন্য Curl_initinfo(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Progress *pro = &তথ্য->progress;
  কাঠামো PureInfo *info = &তথ্য->info;

  memset(&pro->delta, 0, আকার(pro->delta));
  memset(&pro->যোগ, 0, আকার(pro->যোগ));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->header_আকার = 0;
  info->request_আকার = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  memset(&info->primary, 0, আকার(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(তথ্য);
#endif
}

স্থির CURLcode effective_url(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর **param_charp)
{
  ধ্রুবক অক্ষর *s = Curl_bufref_ptr(&তথ্য->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ফাঁকা : "";
  যদি(s) {
    অক্ষর *url = ফাঁকা;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    যদি(!u)
      ফেরত CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (তথ্য->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    যদি(!uc)
      uc = curl_url_set(u, CURLUPART_USER, ফাঁকা, 0);
    যদি(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, ফাঁকা, 0);
    যদি(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    যদি(uc || !url)
      ফেরত uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    যদি(strcmp(url, s)) {
      /* they are different */
      curlx_free(তথ্য->info.effective_url);
      তথ্য->info.effective_url = url;
      *param_charp = url;
    }
    অন্যথা {
      curl_free(url);
      *param_charp = s;
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode getinfo_char(কাঠামো Curl_easy *তথ্য, CURLINFO info,
                             ধ্রুবক অক্ষর **param_charp)
{
  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_EFFECTIVE_URL:
    ফেরত effective_url(তথ্য, param_charp);
  ক্ষেত্র CURLINFO_EFFECTIVE_METHOD: {
    ধ্রুবক অক্ষর *m = CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST);
    যদি(!m) {
      যদি(তথ্য->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      অন্যথা {
        নির্বাচন(তথ্য->state.httpreq) {
        ক্ষেত্র HTTPREQ_POST:
        ক্ষেত্র HTTPREQ_POST_FORM:
        ক্ষেত্র HTTPREQ_POST_MIME:
          m = "POST";
          বিরতি;
        ক্ষেত্র HTTPREQ_PUT:
          m = "PUT";
          বিরতি;
        পূর্বনির্ধারিত: /* this should never happen */
        ক্ষেত্র HTTPREQ_GET:
          m = "GET";
          বিরতি;
        ক্ষেত্র HTTPREQ_শীর্ষ:
          m = "HEAD";
          বিরতি;
        }
      }
#endif
    }
    *param_charp = m;
  }
    বিরতি;
  ক্ষেত্র CURLINFO_CONTENT_TYPE:
    *param_charp = তথ্য->info.contenttype;
    বিরতি;
  ক্ষেত্র CURLINFO_PRIVATE:
    *param_charp = (ধ্রুবক অক্ষর *)তথ্য->set.private_তথ্য;
    বিরতি;
  ক্ষেত্র CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = তথ্য->state.most_recent_ftp_entrypath;
    বিরতি;
  ক্ষেত্র CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = তথ্য->info.wouldredirect;
    বিরতি;
  ক্ষেত্র CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&তথ্য->state.referer);
    বিরতি;
  ক্ষেত্র CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = তথ্য->info.primary.remote_ip;
    বিরতি;
  ক্ষেত্র CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = তথ্য->info.primary.local_ip;
    বিরতি;
  ক্ষেত্র CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(তথ্য, STRING_RTSP_SESSION_ID);
#অন্যথা
    *param_charp = ফাঁকা;
#endif
    বিরতি;
  ক্ষেত্র CURLINFO_SCHEME:
    *param_charp = তথ্য->info.conn_scheme;
    বিরতি;
  ক্ষেত্র CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#অন্যথা
    *param_charp = ফাঁকা;
#endif
    বিরতি;
  ক্ষেত্র CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#অন্যথা
    *param_charp = ফাঁকা;
#endif
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode getinfo_long(কাঠামো Curl_easy *তথ্য, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  সংঘ {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  ধ্রুবক অক্ষর *envstr;

  /* use another variable for this to allow different values */
  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      ফেরত CURLE_OK;
    }
    বিরতি;
  ক্ষেত্র CURLINFO_HEADER_আকার:
  ক্ষেত্র CURLINFO_REQUEST_আকার:
    envstr = getenv("CURL_DEBUG_SIZE");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      ফেরত CURLE_OK;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
#endif

  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_RESPONSE_CODE:
    *param_longp = তথ্য->info.httpcode;
    বিরতি;
  ক্ষেত্র CURLINFO_HTTP_CONNECTCODE:
    *param_longp = তথ্য->info.httpproxycode;
    বিরতি;
  ক্ষেত্র CURLINFO_FILETIME:
    যদি(তথ্য->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#যদি !defined(MSDOS) && !defined(__AMIGA__)
    অন্যথা যদি(তথ্য->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    অন্যথা
      *param_longp = (long)তথ্য->info.filetime;
    বিরতি;
  ক্ষেত্র CURLINFO_HEADER_আকার:
    *param_longp = (long)তথ্য->info.header_আকার;
    বিরতি;
  ক্ষেত্র CURLINFO_REQUEST_আকার:
    *param_longp = (long)তথ্য->info.request_আকার;
    বিরতি;
  ক্ষেত্র CURLINFO_SSL_VERIFYRESULT:
    *param_longp = তথ্য->set.ssl.certverifyresult;
    বিরতি;
  ক্ষেত্র CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = তথ্য->set.proxy_ssl.certverifyresult;
#অন্যথা
    *param_longp = 0;
#endif
    বিরতি;
  ক্ষেত্র CURLINFO_REDIRECT_গুনতি:
    *param_longp = তথ্য->state.followlocation;
    বিরতি;
  ক্ষেত্র CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = তথ্য->info.httpauthavail;
    বিরতি;
  ক্ষেত্র CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = তথ্য->info.proxyauthavail;
    বিরতি;
  ক্ষেত্র CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = তথ্য->info.httpauthpicked;
    বিরতি;
  ক্ষেত্র CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = তথ্য->info.proxyauthpicked;
    বিরতি;
  ক্ষেত্র CURLINFO_OS_ERRNO:
    *param_longp = তথ্য->state.os_errno;
    বিরতি;
  ক্ষেত্র CURLINFO_অঙ্ক_CONNECTS:
#যদি SIZEOF_LONG < SIZEOF_CURL_OFF_T
    যদি(তথ্য->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    অন্যথা
#endif
      *param_longp = (long)তথ্য->info.numconnects;
    বিরতি;
  ক্ষেত্র CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(তথ্য, ফাঁকা);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    যদি(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    অন্যথা
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    বিরতি;
  ক্ষেত্র CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    যদি(CUR_IP_QUAD_HAS_PORTS(&তথ্য->info.primary))
      *param_longp = তথ্য->info.primary.remote_port;
    অন্যথা
      *param_longp = -1;
    বিরতি;
  ক্ষেত্র CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    যদি(CUR_IP_QUAD_HAS_PORTS(&তথ্য->info.primary))
      *param_longp = তথ্য->info.primary.local_port;
    অন্যথা
      *param_longp = -1;
    বিরতি;
  ক্ষেত্র CURLINFO_PROXY_ERROR:
    *param_longp = (long)তথ্য->info.pxcode;
    বিরতি;
  ক্ষেত্র CURLINFO_CONDITION_UNMET:
    যদি(তথ্য->info.httpcode == 304)
      *param_longp = 1L;
    অন্যথা
      /* return if the condition prevented the document to get transferred */
      *param_longp = তথ্য->info.timecond ? 1L : 0L;
    বিরতি;
#ifndef CURL_DISABLE_RTSP
  ক্ষেত্র CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = তথ্য->state.rtsp_পরবর্তী_client_CSeq;
    বিরতি;
  ক্ষেত্র CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = তথ্য->state.rtsp_পরবর্তী_server_CSeq;
    বিরতি;
  ক্ষেত্র CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = তথ্য->state.rtsp_CSeq_recv;
    বিরতি;
#অন্যথা
  ক্ষেত্র CURLINFO_RTSP_CLIENT_CSEQ:
  ক্ষেত্র CURLINFO_RTSP_SERVER_CSEQ:
  ক্ষেত্র CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    বিরতি;
#endif
  ক্ষেত্র CURLINFO_HTTP_VERSION:
    নির্বাচন(তথ্য->info.httpversion) {
    ক্ষেত্র 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      বিরতি;
    ক্ষেত্র 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      বিরতি;
    ক্ষেত্র 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      বিরতি;
    ক্ষেত্র 30:
      *param_longp = CURL_HTTP_VERSION_3;
      বিরতি;
    পূর্বনির্ধারিত:
      *param_longp = CURL_HTTP_VERSION_NONE;
      বিরতি;
    }
    বিরতি;
  ক্ষেত্র CURLINFO_PROTOCOL:
    *param_longp = (long)তথ্য->info.conn_protocol;
    বিরতি;
  ক্ষেত্র CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#অন্যথা
      তথ্য->info.used_proxy
#endif
      ;
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }

  ফেরত CURLE_OK;
}

#define DOUBLE_SECS(x) ((দ্বিগুণ)(x) / 1000000)

স্থির CURLcode getinfo_offt(কাঠামো Curl_easy *তথ্য, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  ধ্রুবক অক্ষর *envstr;
  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_যোগ_TIME_T:
  ক্ষেত্র CURLINFO_NAMELOOKUP_TIME_T:
  ক্ষেত্র CURLINFO_CONNECT_TIME_T:
  ক্ষেত্র CURLINFO_APPCONNECT_TIME_T:
  ক্ষেত্র CURLINFO_PRETRANSFER_TIME_T:
  ক্ষেত্র CURLINFO_POSTTRANSFER_TIME_T:
  ক্ষেত্র CURLINFO_QUEUE_TIME_T:
  ক্ষেত্র CURLINFO_STARTTRANSFER_TIME_T:
  ক্ষেত্র CURLINFO_REDIRECT_TIME_T:
  ক্ষেত্র CURLINFO_SPEED_DOWNLOAD_T:
  ক্ষেত্র CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      ফেরত CURLE_OK;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
#endif
  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)তথ্য->info.filetime;
    বিরতি;
  ক্ষেত্র CURLINFO_আকার_DELIVERED:
    *param_offt = তথ্য->progress.deliver;
    বিরতি;
  ক্ষেত্র CURLINFO_আকার_UPLOAD_T:
    *param_offt = তথ্য->progress.ul.cur_আকার;
    বিরতি;
  ক্ষেত্র CURLINFO_আকার_DOWNLOAD_T:
    *param_offt = তথ্য->progress.dl.cur_আকার;
    বিরতি;
  ক্ষেত্র CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = তথ্য->progress.dl.speed;
    বিরতি;
  ক্ষেত্র CURLINFO_SPEED_UPLOAD_T:
    *param_offt = তথ্য->progress.ul.speed;
    বিরতি;
  ক্ষেত্র CURLINFO_CONTENT_দৈর্ঘ্য_DOWNLOAD_T:
    *param_offt = তথ্য->progress.dl_আকার_known ?
      তথ্য->progress.dl.যোগ_আকার : -1;
    বিরতি;
  ক্ষেত্র CURLINFO_CONTENT_দৈর্ঘ্য_UPLOAD_T:
    *param_offt = তথ্য->progress.ul_আকার_known ?
      তথ্য->progress.ul.যোগ_আকার : -1;
    বিরতি;
   ক্ষেত্র CURLINFO_যোগ_TIME_T:
    *param_offt = তথ্য->progress.যোগ.spent_us;
    বিরতি;
  ক্ষেত্র CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = তথ্য->progress.যোগ.nslookup_us;
    বিরতি;
  ক্ষেত্র CURLINFO_CONNECT_TIME_T:
    *param_offt = তথ্য->progress.যোগ.connect_us;
    বিরতি;
  ক্ষেত্র CURLINFO_APPCONNECT_TIME_T:
    *param_offt = তথ্য->progress.যোগ.appconnect_us;
    বিরতি;
  ক্ষেত্র CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = তথ্য->progress.যোগ.pretransfer_us;
    বিরতি;
  ক্ষেত্র CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = তথ্য->progress.যোগ.posttransfer_us;
    বিরতি;
  ক্ষেত্র CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = তথ্য->progress.যোগ.starttransfer_us;
    বিরতি;
  ক্ষেত্র CURLINFO_QUEUE_TIME_T:
    *param_offt = তথ্য->progress.যোগ.queued_us;
    বিরতি;
  ক্ষেত্র CURLINFO_REDIRECT_TIME_T:
    *param_offt = তথ্য->progress.delta.startredirect_us;
    বিরতি;
  ক্ষেত্র CURLINFO_RETRY_AFTER:
    *param_offt = তথ্য->info.retry_after;
    বিরতি;
  ক্ষেত্র CURLINFO_XFER_ID:
    *param_offt = তথ্য->id;
    বিরতি;
  ক্ষেত্র CURLINFO_CONN_ID:
    *param_offt = তথ্য->state.last_conn_id;
    বিরতি;
  ক্ষেত্র CURLINFO_EARLYDATA_SENT_T:
    *param_offt = তথ্য->progress.earlydata_sent;
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode getinfo_double(কাঠামো Curl_easy *তথ্য, CURLINFO info,
                               দ্বিগুণ *param_doublep)
{
#ifdef DEBUGBUILD
  ধ্রুবক অক্ষর *envstr;

  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_যোগ_TIME:
  ক্ষেত্র CURLINFO_NAMELOOKUP_TIME:
  ক্ষেত্র CURLINFO_CONNECT_TIME:
  ক্ষেত্র CURLINFO_APPCONNECT_TIME:
  ক্ষেত্র CURLINFO_PRETRANSFER_TIME:
  ক্ষেত্র CURLINFO_STARTTRANSFER_TIME:
  ক্ষেত্র CURLINFO_REDIRECT_TIME:
  ক্ষেত্র CURLINFO_SPEED_DOWNLOAD:
  ক্ষেত্র CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (দ্বিগুণ)val;
      ফেরত CURLE_OK;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
#endif
  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_যোগ_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.যোগ.spent_us);
    বিরতি;
  ক্ষেত্র CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.যোগ.nslookup_us);
    বিরতি;
  ক্ষেত্র CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.যোগ.connect_us);
    বিরতি;
  ক্ষেত্র CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.যোগ.appconnect_us);
    বিরতি;
  ক্ষেত্র CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.যোগ.pretransfer_us);
    বিরতি;
  ক্ষেত্র CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.যোগ.starttransfer_us);
    বিরতি;
  ক্ষেত্র CURLINFO_আকার_UPLOAD:
    *param_doublep = (দ্বিগুণ)তথ্য->progress.ul.cur_আকার;
    বিরতি;
  ক্ষেত্র CURLINFO_আকার_DOWNLOAD:
    *param_doublep = (দ্বিগুণ)তথ্য->progress.dl.cur_আকার;
    বিরতি;
  ক্ষেত্র CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (দ্বিগুণ)তথ্য->progress.dl.speed;
    বিরতি;
  ক্ষেত্র CURLINFO_SPEED_UPLOAD:
    *param_doublep = (দ্বিগুণ)তথ্য->progress.ul.speed;
    বিরতি;
  ক্ষেত্র CURLINFO_CONTENT_দৈর্ঘ্য_DOWNLOAD:
    *param_doublep = তথ্য->progress.dl_আকার_known ?
      (দ্বিগুণ)তথ্য->progress.dl.যোগ_আকার : -1;
    বিরতি;
  ক্ষেত্র CURLINFO_CONTENT_দৈর্ঘ্য_UPLOAD:
    *param_doublep = তথ্য->progress.ul_আকার_known ?
      (দ্বিগুণ)তথ্য->progress.ul.যোগ_আকার : -1;
    বিরতি;
  ক্ষেত্র CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(তথ্য->progress.delta.startredirect_us);
    বিরতি;

  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode getinfo_slist(কাঠামো Curl_easy *তথ্য, CURLINFO info,
                              কাঠামো curl_slist **param_slistp)
{
  সংঘ {
    কাঠামো curl_certinfo *to_certinfo;
    কাঠামো curl_slist    *to_slist;
  } ptr;

  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(তথ্য);
    বিরতি;
  ক্ষেত্র CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(তথ্য);
    বিরতি;
  ক্ষেত্র CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &তথ্য->info.certs;
    *param_slistp = ptr.to_slist;
    বিরতি;
  ক্ষেত্র CURLINFO_TLS_SESSION:
  ক্ষেত্র CURLINFO_TLS_SSL_PTR: {
    পূর্ণসংখ্যা query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    কাঠামো curl_tlssessioninfo **tsip = (কাঠামো curl_tlssessioninfo **)
      param_slistp;
    কাঠামো curl_tlssessioninfo *tsi = &তথ্য->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    যদি(!Curl_conn_get_ssl_info(তথ্য, তথ্য->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ফাঁকা;
    }
  }
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode getinfo_socket(কাঠামো Curl_easy *তথ্য, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  নির্বাচন(info) {
  ক্ষেত্র CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(তথ্য, ফাঁকা);
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_UNKNOWN_OPTION;
  }

  ফেরত CURLE_OK;
}

CURLcode Curl_getinfo(কাঠামো Curl_easy *তথ্য, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = ফাঁকা;
  দ্বিগুণ *param_doublep = ফাঁকা;
  curl_off_t *param_offt = ফাঁকা;
  ধ্রুবক অক্ষর **param_charp = ফাঁকা;
  কাঠামো curl_slist **param_slistp = ফাঁকা;
  curl_socket_t *param_socketp = ফাঁকা;
  পূর্ণসংখ্যা type;
  CURLcode ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!তথ্য)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (পূর্ণসংখ্যা)info;
  নির্বাচন(type) {
  ক্ষেত্র CURLINFO_STRING:
    param_charp = va_arg(arg, ধ্রুবক অক্ষর **);
    যদি(param_charp)
      ফলাফল = getinfo_char(তথ্য, info, param_charp);
    বিরতি;
  ক্ষেত্র CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    যদি(param_longp)
      ফলাফল = getinfo_long(তথ্য, info, param_longp);
    বিরতি;
  ক্ষেত্র CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, দ্বিগুণ *);
    যদি(param_doublep)
      ফলাফল = getinfo_double(তথ্য, info, param_doublep);
    বিরতি;
  ক্ষেত্র CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    যদি(param_offt)
      ফলাফল = getinfo_offt(তথ্য, info, param_offt);
    বিরতি;
  ক্ষেত্র CURLINFO_SLIST:
    param_slistp = va_arg(arg, কাঠামো curl_slist **);
    যদি(param_slistp)
      ফলাফল = getinfo_slist(তথ্য, info, param_slistp);
    বিরতি;
  ক্ষেত্র CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    যদি(param_socketp)
      ফলাফল = getinfo_socket(তথ্য, info, param_socketp);
    বিরতি;
  পূর্বনির্ধারিত:
    ফলাফল = CURLE_UNKNOWN_OPTION;
    বিরতি;
  }

  va_end(arg);

  ফেরত ফলাফল;
}
