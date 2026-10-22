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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "getinfo.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "connect.h" /* Curl_getconnectinfo() */
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "url.h" /* Curl_uc_to_curlcode() */
#अन्तर्भाव "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
शून्य Curl_initinfo(संरचना Curl_easy *दत्त)
{
  संरचना प्रगतिः_2 *pro = &दत्त->प्रगतिः;
  संरचना विशुद्धम्_Info *info = &दत्त->info;

  स्मृतिं_पूरय(&pro->delta, 0, परिमाण(pro->delta));
  स्मृतिं_पूरय(&pro->योग, 0, परिमाण(pro->योग));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->शीर्षकम्_आकार = 0;
  info->request_आकार = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  स्मृतिं_पूरय(&info->primary, 0, परिमाण(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->प्रयुक्तम्_proxy = 0;

#यदि_परिभाषितम् USE_SSL
  Curl_ssl_free_certinfo(दत्त);
#यदि_अन्तः
}

स्थिर CURLcode effective_url(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण **param_charp)
{
  ध्रुव वर्ण *s = Curl_bufref_ptr(&दत्त->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? रिक्त : "";
  यदि(s) {
    वर्ण *url = रिक्त;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    यदि(!u)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    uc = curl_url_समुच्चयः(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (दत्त->समुच्चयः.मार्गः_as_is ? CURLU_मार्गः_AS_IS : 0));
    यदि(!uc)
      uc = curl_url_समुच्चयः(u, CURLUPART_USER, रिक्त, 0);
    यदि(!uc)
      uc = curl_url_समुच्चयः(u, CURLUPART_PASSWORD, रिक्त, 0);
    यदि(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_रिक्तानि);

    curl_url_cleanup(u);

    यदि(uc || !url)
      प्रत्यागम uc ? Curl_uc_गन्तव्यम्_curlcode(uc) : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(वाक्यं_तुलय(url, s)) {
      /* they are different */
      curlx_free(दत्त->info.effective_url);
      दत्त->info.effective_url = url;
      *param_charp = url;
    }
    अन्यथा {
      curl_free(url);
      *param_charp = s;
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode getinfo_char(संरचना Curl_easy *दत्त, CURLINFO info,
                             ध्रुव वर्ण **param_charp)
{
  विकल्प(info) {
  पक्ष CURLINFO_EFFECTIVE_URL:
    प्रत्यागम effective_url(दत्त, param_charp);
  पक्ष CURLINFO_EFFECTIVE_विधानम्: {
    ध्रुव वर्ण *m = CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST);
    यदि(!m) {
      यदि(दत्त->समुच्चयः.opt_no_body)
        m = "HEAD";
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
      अन्यथा {
        विकल्प(दत्त->state.httpreq) {
        पक्ष HTTPREQ_POST:
        पक्ष HTTPREQ_POST_FORM:
        पक्ष HTTPREQ_POST_MIME:
          m = "POST";
          भङ्ग;
        पक्ष HTTPREQ_PUT:
          m = "PUT";
          भङ्ग;
        स्वभाव: /* this should never happen */
        पक्ष HTTPREQ_GET:
          m = "GET";
          भङ्ग;
        पक्ष HTTPREQ_शीर्ष:
          m = "HEAD";
          भङ्ग;
        }
      }
#यदि_अन्तः
    }
    *param_charp = m;
  }
    भङ्ग;
  पक्ष CURLINFO_CONTENT_TYPE:
    *param_charp = दत्त->info.contenttype;
    भङ्ग;
  पक्ष CURLINFO_वैयक्तिकम्:
    *param_charp = (ध्रुव वर्ण *)दत्त->समुच्चयः.वैयक्तिकम्_दत्त;
    भङ्ग;
  पक्ष CURLINFO_FTP_प्रविष्टिः_मार्गः:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = दत्त->state.most_recent_ftp_entrypath;
    भङ्ग;
  पक्ष CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = दत्त->info.wouldredirect;
    भङ्ग;
  पक्ष CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&दत्त->state.referer);
    भङ्ग;
  पक्ष CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = दत्त->info.primary.remote_ip;
    भङ्ग;
  पक्ष CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = दत्त->info.primary.local_ip;
    भङ्ग;
  पक्ष CURLINFO_RTSP_SESSION_ID:
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(दत्त, STRING_RTSP_SESSION_ID);
#अन्यथा
    *param_charp = रिक्त;
#यदि_अन्तः
    भङ्ग;
  पक्ष CURLINFO_SCHEME:
    *param_charp = दत्त->info.conn_scheme;
    भङ्ग;
  पक्ष CURLINFO_CAPATH:
#यदि_परिभाषितम् CURL_CA_मार्गः
    *param_charp = CURL_CA_मार्गः;
#अन्यथा
    *param_charp = रिक्त;
#यदि_अन्तः
    भङ्ग;
  पक्ष CURLINFO_CAINFO:
#यदि_परिभाषितम् CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#अन्यथा
    *param_charp = रिक्त;
#यदि_अन्तः
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode getinfo_long(संरचना Curl_easy *दत्त, CURLINFO info,
                             दीर्घ *param_longp)
{
  curl_socket_t sockfd;

  सङ्घ {
    अचिह्नित दीर्घ *गन्तव्यम्_ulong;
    दीर्घ          *गन्तव्यम्_long;
  } lptr;

#यदि_परिभाषितम् DEBUGBUILD
  ध्रुव वर्ण *envstr;

  /* use another variable for this to allow different values */
  विकल्प(info) {
  पक्ष CURLINFO_LOCAL_पत्तनम्:
    envstr = getenv("CURL_TIME");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, TIME_T_MAX);
      *param_longp = (दीर्घ)val;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  पक्ष CURLINFO_शीर्षकम्_आकार:
  पक्ष CURLINFO_REQUEST_आकार:
    envstr = getenv("CURL_DEBUG_SIZE");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, LONG_MAX);
      *param_longp = (दीर्घ)val;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
#यदि_अन्तः

  विकल्प(info) {
  पक्ष CURLINFO_RESPONSE_संहिता:
    *param_longp = दत्त->info.httpcode;
    भङ्ग;
  पक्ष CURLINFO_HTTP_CONNECTCODE:
    *param_longp = दत्त->info.httpproxycode;
    भङ्ग;
  पक्ष CURLINFO_FILETIME:
    यदि(दत्त->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#यदि !defined(MSDOS) && !defined(__AMIGA__)
    अन्यथा यदि(दत्त->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#यदि_अन्तः
    अन्यथा
      *param_longp = (दीर्घ)दत्त->info.filetime;
    भङ्ग;
  पक्ष CURLINFO_शीर्षकम्_आकार:
    *param_longp = (दीर्घ)दत्त->info.शीर्षकम्_आकार;
    भङ्ग;
  पक्ष CURLINFO_REQUEST_आकार:
    *param_longp = (दीर्घ)दत्त->info.request_आकार;
    भङ्ग;
  पक्ष CURLINFO_SSL_VERIFYRESULT:
    *param_longp = दत्त->समुच्चयः.ssl.certverifyresult;
    भङ्ग;
  पक्ष CURLINFO_PROXY_SSL_VERIFYRESULT:
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    *param_longp = दत्त->समुच्चयः.proxy_ssl.certverifyresult;
#अन्यथा
    *param_longp = 0;
#यदि_अन्तः
    भङ्ग;
  पक्ष CURLINFO_REDIRECT_गणनम्:
    *param_longp = दत्त->state.followlocation;
    भङ्ग;
  पक्ष CURLINFO_HTTPAUTH_AVAIL:
    lptr.गन्तव्यम्_long = param_longp;
    *lptr.गन्तव्यम्_ulong = दत्त->info.httpauthavail;
    भङ्ग;
  पक्ष CURLINFO_PROXYAUTH_AVAIL:
    lptr.गन्तव्यम्_long = param_longp;
    *lptr.गन्तव्यम्_ulong = दत्त->info.proxyauthavail;
    भङ्ग;
  पक्ष CURLINFO_HTTPAUTH_प्रयुक्तम्:
    lptr.गन्तव्यम्_long = param_longp;
    *lptr.गन्तव्यम्_ulong = दत्त->info.httpauthpicked;
    भङ्ग;
  पक्ष CURLINFO_PROXYAUTH_प्रयुक्तम्:
    lptr.गन्तव्यम्_long = param_longp;
    *lptr.गन्तव्यम्_ulong = दत्त->info.proxyauthpicked;
    भङ्ग;
  पक्ष CURLINFO_OS_ERRNO:
    *param_longp = दत्त->state.os_errno;
    भङ्ग;
  पक्ष CURLINFO_अङ्कः_CONNECTS:
#यदि SIZEOF_LONG < SIZEOF_CURL_OFF_T
    यदि(दत्त->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    अन्यथा
#यदि_अन्तः
      *param_longp = (दीर्घ)दत्त->info.numconnects;
    भङ्ग;
  पक्ष CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(दत्त, रिक्त);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    यदि(sockfd != CURL_SOCKET_BAD)
      *param_longp = (दीर्घ)sockfd;
    अन्यथा
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    भङ्ग;
  पक्ष CURLINFO_PRIMARY_पत्तनम्:
    /* Return the (remote) port of the most recent (primary) connection */
    यदि(CUR_IP_QUAD_HAS_PORTS(&दत्त->info.primary))
      *param_longp = दत्त->info.primary.remote_पत्तनम्;
    अन्यथा
      *param_longp = -1;
    भङ्ग;
  पक्ष CURLINFO_LOCAL_पत्तनम्:
    /* Return the local port of the most recent (primary) connection */
    यदि(CUR_IP_QUAD_HAS_PORTS(&दत्त->info.primary))
      *param_longp = दत्त->info.primary.local_पत्तनम्;
    अन्यथा
      *param_longp = -1;
    भङ्ग;
  पक्ष CURLINFO_PROXY_ERROR:
    *param_longp = (दीर्घ)दत्त->info.pxcode;
    भङ्ग;
  पक्ष CURLINFO_CONDITION_UNMET:
    यदि(दत्त->info.httpcode == 304)
      *param_longp = 1L;
    अन्यथा
      /* return if the condition prevented the document to get transferred */
      *param_longp = दत्त->info.timecond ? 1L : 0L;
    भङ्ग;
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  पक्ष CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = दत्त->state.rtsp_अग्र_client_CSeq;
    भङ्ग;
  पक्ष CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = दत्त->state.rtsp_अग्र_server_CSeq;
    भङ्ग;
  पक्ष CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = दत्त->state.rtsp_CSeq_recv;
    भङ्ग;
#अन्यथा
  पक्ष CURLINFO_RTSP_CLIENT_CSEQ:
  पक्ष CURLINFO_RTSP_SERVER_CSEQ:
  पक्ष CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    भङ्ग;
#यदि_अन्तः
  पक्ष CURLINFO_HTTP_संस्करणम्:
    विकल्प(दत्त->info.httpversion) {
    पक्ष 10:
      *param_longp = CURL_HTTP_संस्करणम्_1_0;
      भङ्ग;
    पक्ष 11:
      *param_longp = CURL_HTTP_संस्करणम्_1_1;
      भङ्ग;
    पक्ष 20:
      *param_longp = CURL_HTTP_संस्करणम्_2_0;
      भङ्ग;
    पक्ष 30:
      *param_longp = CURL_HTTP_संस्करणम्_3;
      भङ्ग;
    स्वभाव:
      *param_longp = CURL_HTTP_संस्करणम्_NONE;
      भङ्ग;
    }
    भङ्ग;
  पक्ष CURLINFO_PROTOCOL:
    *param_longp = (दीर्घ)दत्त->info.conn_protocol;
    भङ्ग;
  पक्ष CURLINFO_प्रयुक्तम्_PROXY:
    *param_longp =
#यदि_परिभाषितम् CURL_DISABLE_PROXY
      0
#अन्यथा
      दत्त->info.प्रयुक्तम्_proxy
#यदि_अन्तः
      ;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }

  प्रत्यागम CURLE_OK;
}

#परिभाषय DOUBLE_SECS(x) ((द्विगुण)(x) / 1000000)

स्थिर CURLcode getinfo_offt(संरचना Curl_easy *दत्त, CURLINFO info,
                             curl_off_t *param_offt)
{
#यदि_परिभाषितम् DEBUGBUILD
  ध्रुव वर्ण *envstr;
  विकल्प(info) {
  पक्ष CURLINFO_योग_TIME_T:
  पक्ष CURLINFO_NAMELOOKUP_TIME_T:
  पक्ष CURLINFO_CONNECT_TIME_T:
  पक्ष CURLINFO_APPCONNECT_TIME_T:
  पक्ष CURLINFO_PRETRANSFER_TIME_T:
  पक्ष CURLINFO_POSTTRANSFER_TIME_T:
  पक्ष CURLINFO_QUEUE_TIME_T:
  पक्ष CURLINFO_STARTTRANSFER_TIME_T:
  पक्ष CURLINFO_REDIRECT_TIME_T:
  पक्ष CURLINFO_जवः_DOWNLOAD_T:
  पक्ष CURLINFO_जवः_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
#यदि_अन्तः
  विकल्प(info) {
  पक्ष CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)दत्त->info.filetime;
    भङ्ग;
  पक्ष CURLINFO_आकार_DELIVERED:
    *param_offt = दत्त->प्रगतिः.deliver;
    भङ्ग;
  पक्ष CURLINFO_आकार_UPLOAD_T:
    *param_offt = दत्त->प्रगतिः.ul.cur_आकार;
    भङ्ग;
  पक्ष CURLINFO_आकार_DOWNLOAD_T:
    *param_offt = दत्त->प्रगतिः.dl.cur_आकार;
    भङ्ग;
  पक्ष CURLINFO_जवः_DOWNLOAD_T:
    *param_offt = दत्त->प्रगतिः.dl.जवः;
    भङ्ग;
  पक्ष CURLINFO_जवः_UPLOAD_T:
    *param_offt = दत्त->प्रगतिः.ul.जवः;
    भङ्ग;
  पक्ष CURLINFO_CONTENT_दीर्घता_DOWNLOAD_T:
    *param_offt = दत्त->प्रगतिः.dl_आकार_ज्ञातम् ?
      दत्त->प्रगतिः.dl.योग_आकार : -1;
    भङ्ग;
  पक्ष CURLINFO_CONTENT_दीर्घता_UPLOAD_T:
    *param_offt = दत्त->प्रगतिः.ul_आकार_ज्ञातम् ?
      दत्त->प्रगतिः.ul.योग_आकार : -1;
    भङ्ग;
   पक्ष CURLINFO_योग_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.spent_us;
    भङ्ग;
  पक्ष CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.nslookup_us;
    भङ्ग;
  पक्ष CURLINFO_CONNECT_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.connect_us;
    भङ्ग;
  पक्ष CURLINFO_APPCONNECT_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.appconnect_us;
    भङ्ग;
  पक्ष CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.pretransfer_us;
    भङ्ग;
  पक्ष CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.posttransfer_us;
    भङ्ग;
  पक्ष CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.starttransfer_us;
    भङ्ग;
  पक्ष CURLINFO_QUEUE_TIME_T:
    *param_offt = दत्त->प्रगतिः.योग.queued_us;
    भङ्ग;
  पक्ष CURLINFO_REDIRECT_TIME_T:
    *param_offt = दत्त->प्रगतिः.delta.startredirect_us;
    भङ्ग;
  पक्ष CURLINFO_RETRY_AFTER:
    *param_offt = दत्त->info.retry_after;
    भङ्ग;
  पक्ष CURLINFO_XFER_ID:
    *param_offt = दत्त->id;
    भङ्ग;
  पक्ष CURLINFO_CONN_ID:
    *param_offt = दत्त->state.चरमः_conn_id;
    भङ्ग;
  पक्ष CURLINFO_EARLYDATA_प्रेषितम्_T:
    *param_offt = दत्त->प्रगतिः.earlydata_प्रेषितम्;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode getinfo_double(संरचना Curl_easy *दत्त, CURLINFO info,
                               द्विगुण *param_doublep)
{
#यदि_परिभाषितम् DEBUGBUILD
  ध्रुव वर्ण *envstr;

  विकल्प(info) {
  पक्ष CURLINFO_योग_TIME:
  पक्ष CURLINFO_NAMELOOKUP_TIME:
  पक्ष CURLINFO_CONNECT_TIME:
  पक्ष CURLINFO_APPCONNECT_TIME:
  पक्ष CURLINFO_PRETRANSFER_TIME:
  पक्ष CURLINFO_STARTTRANSFER_TIME:
  पक्ष CURLINFO_REDIRECT_TIME:
  पक्ष CURLINFO_जवः_DOWNLOAD:
  पक्ष CURLINFO_जवः_UPLOAD:
    envstr = getenv("CURL_TIME");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (द्विगुण)val;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
#यदि_अन्तः
  विकल्प(info) {
  पक्ष CURLINFO_योग_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.योग.spent_us);
    भङ्ग;
  पक्ष CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.योग.nslookup_us);
    भङ्ग;
  पक्ष CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.योग.connect_us);
    भङ्ग;
  पक्ष CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.योग.appconnect_us);
    भङ्ग;
  पक्ष CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.योग.pretransfer_us);
    भङ्ग;
  पक्ष CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.योग.starttransfer_us);
    भङ्ग;
  पक्ष CURLINFO_आकार_UPLOAD:
    *param_doublep = (द्विगुण)दत्त->प्रगतिः.ul.cur_आकार;
    भङ्ग;
  पक्ष CURLINFO_आकार_DOWNLOAD:
    *param_doublep = (द्विगुण)दत्त->प्रगतिः.dl.cur_आकार;
    भङ्ग;
  पक्ष CURLINFO_जवः_DOWNLOAD:
    *param_doublep = (द्विगुण)दत्त->प्रगतिः.dl.जवः;
    भङ्ग;
  पक्ष CURLINFO_जवः_UPLOAD:
    *param_doublep = (द्विगुण)दत्त->प्रगतिः.ul.जवः;
    भङ्ग;
  पक्ष CURLINFO_CONTENT_दीर्घता_DOWNLOAD:
    *param_doublep = दत्त->प्रगतिः.dl_आकार_ज्ञातम् ?
      (द्विगुण)दत्त->प्रगतिः.dl.योग_आकार : -1;
    भङ्ग;
  पक्ष CURLINFO_CONTENT_दीर्घता_UPLOAD:
    *param_doublep = दत्त->प्रगतिः.ul_आकार_ज्ञातम् ?
      (द्विगुण)दत्त->प्रगतिः.ul.योग_आकार : -1;
    भङ्ग;
  पक्ष CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(दत्त->प्रगतिः.delta.startredirect_us);
    भङ्ग;

  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode getinfo_slist(संरचना Curl_easy *दत्त, CURLINFO info,
                              संरचना curl_slist **param_slistp)
{
  सङ्घ {
    संरचना curl_certinfo *गन्तव्यम्_certinfo;
    संरचना curl_slist    *गन्तव्यम्_slist;
  } ptr;

  विकल्प(info) {
  पक्ष CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(दत्त);
    भङ्ग;
  पक्ष CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(दत्त);
    भङ्ग;
  पक्ष CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.गन्तव्यम्_certinfo = &दत्त->info.certs;
    *param_slistp = ptr.गन्तव्यम्_slist;
    भङ्ग;
  पक्ष CURLINFO_TLS_SESSION:
  पक्ष CURLINFO_TLS_SSL_PTR: {
    पूर्णाङ्क query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    संरचना curl_tlssessioninfo **tsip = (संरचना curl_tlssessioninfo **)
      param_slistp;
    संरचना curl_tlssessioninfo *tsi = &दत्त->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    यदि(!Curl_conn_get_ssl_info(दत्त, दत्त->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = रिक्त;
    }
  }
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode getinfo_socket(संरचना Curl_easy *दत्त, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  विकल्प(info) {
  पक्ष CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(दत्त, रिक्त);
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_UNKNOWN_OPTION;
  }

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_getinfo(संरचना Curl_easy *दत्त, CURLINFO info, ...)
{
  va_list arg;
  दीर्घ *param_longp = रिक्त;
  द्विगुण *param_doublep = रिक्त;
  curl_off_t *param_offt = रिक्त;
  ध्रुव वर्ण **param_charp = रिक्त;
  संरचना curl_slist **param_slistp = रिक्त;
  curl_socket_t *param_socketp = रिक्त;
  पूर्णाङ्क type;
  CURLcode परिणाम = CURLE_BAD_FUNCTION_तर्कः;

  यदि(!दत्त)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (पूर्णाङ्क)info;
  विकल्प(type) {
  पक्ष CURLINFO_STRING:
    param_charp = va_arg(arg, ध्रुव वर्ण **);
    यदि(param_charp)
      परिणाम = getinfo_char(दत्त, info, param_charp);
    भङ्ग;
  पक्ष CURLINFO_LONG:
    param_longp = va_arg(arg, दीर्घ *);
    यदि(param_longp)
      परिणाम = getinfo_long(दत्त, info, param_longp);
    भङ्ग;
  पक्ष CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, द्विगुण *);
    यदि(param_doublep)
      परिणाम = getinfo_double(दत्त, info, param_doublep);
    भङ्ग;
  पक्ष CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    यदि(param_offt)
      परिणाम = getinfo_offt(दत्त, info, param_offt);
    भङ्ग;
  पक्ष CURLINFO_SLIST:
    param_slistp = va_arg(arg, संरचना curl_slist **);
    यदि(param_slistp)
      परिणाम = getinfo_slist(दत्त, info, param_slistp);
    भङ्ग;
  पक्ष CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    यदि(param_socketp)
      परिणाम = getinfo_socket(दत्त, info, param_socketp);
    भङ्ग;
  स्वभाव:
    परिणाम = CURLE_UNKNOWN_OPTION;
    भङ्ग;
  }

  va_end(arg);

  प्रत्यागम परिणाम;
}
