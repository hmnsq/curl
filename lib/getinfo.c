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

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "getinfo.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "connect.h" /* Curl_getconnectinfo() */
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "url.h" /* Curl_uc_to_curlcode() */
#అన్తర్భావ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
శూన్య Curl_initinfo(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం పురోగతి_2 *pro = &దత్తాంశం->పురోగతి;
  నిర్మాణం శుద్ధ_Info *info = &దత్తాంశం->info;

  memset(&pro->delta, 0, పరిమాణం(pro->delta));
  memset(&pro->మొత్తం, 0, పరిమాణం(pro->మొత్తం));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->శీర్షిక_కొలత = 0;
  info->request_కొలత = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  memset(&info->primary, 0, పరిమాణం(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->వాడిన_proxy = 0;

#నిర్వచితమైతే USE_SSL
  Curl_ssl_free_certinfo(దత్తాంశం);
#అయితే_ముగింపు
}

స్థిర CURLcode effective_url(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం **param_charp)
{
  స్థిరరాశి అక్షరం *s = Curl_bufref_ptr(&దత్తాంశం->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ఖాళీ : "";
  ఒకవేళ(s) {
    అక్షరం *url = ఖాళీ;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    ఒకవేళ(!u)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    uc = curl_url_సమితి(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (దత్తాంశం->సమితి.మార్గం_as_is ? CURLU_మార్గం_AS_IS : 0));
    ఒకవేళ(!uc)
      uc = curl_url_సమితి(u, CURLUPART_USER, ఖాళీ, 0);
    ఒకవేళ(!uc)
      uc = curl_url_సమితి(u, CURLUPART_PASSWORD, ఖాళీ, 0);
    ఒకవేళ(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_ఖాళీలు);

    curl_url_cleanup(u);

    ఒకవేళ(uc || !url)
      తిరిగి uc ? Curl_uc_గమ్యం_curlcode(uc) : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(strcmp(url, s)) {
      /* they are different */
      curlx_free(దత్తాంశం->info.effective_url);
      దత్తాంశం->info.effective_url = url;
      *param_charp = url;
    }
    లేకపోతే {
      curl_free(url);
      *param_charp = s;
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode getinfo_char(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info,
                             స్థిరరాశి అక్షరం **param_charp)
{
  ఎంపిక(info) {
  స్థితి CURLINFO_EFFECTIVE_URL:
    తిరిగి effective_url(దత్తాంశం, param_charp);
  స్థితి CURLINFO_EFFECTIVE_పద్ధతి: {
    స్థిరరాశి అక్షరం *m = CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST);
    ఒకవేళ(!m) {
      ఒకవేళ(దత్తాంశం->సమితి.opt_no_body)
        m = "HEAD";
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
      లేకపోతే {
        ఎంపిక(దత్తాంశం->state.httpreq) {
        స్థితి HTTPREQ_POST:
        స్థితి HTTPREQ_POST_FORM:
        స్థితి HTTPREQ_POST_MIME:
          m = "POST";
          ఆపు;
        స్థితి HTTPREQ_PUT:
          m = "PUT";
          ఆపు;
        అప్రమేయ: /* this should never happen */
        స్థితి HTTPREQ_GET:
          m = "GET";
          ఆపు;
        స్థితి HTTPREQ_తల:
          m = "HEAD";
          ఆపు;
        }
      }
#అయితే_ముగింపు
    }
    *param_charp = m;
  }
    ఆపు;
  స్థితి CURLINFO_CONTENT_TYPE:
    *param_charp = దత్తాంశం->info.contenttype;
    ఆపు;
  స్థితి CURLINFO_వ్యక్తిగత:
    *param_charp = (స్థిరరాశి అక్షరం *)దత్తాంశం->సమితి.వ్యక్తిగత_దత్తాంశం;
    ఆపు;
  స్థితి CURLINFO_FTP_నమోదు_మార్గం:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = దత్తాంశం->state.most_recent_ftp_entrypath;
    ఆపు;
  స్థితి CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = దత్తాంశం->info.wouldredirect;
    ఆపు;
  స్థితి CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&దత్తాంశం->state.referer);
    ఆపు;
  స్థితి CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = దత్తాంశం->info.primary.remote_ip;
    ఆపు;
  స్థితి CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = దత్తాంశం->info.primary.local_ip;
    ఆపు;
  స్థితి CURLINFO_RTSP_SESSION_ID:
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(దత్తాంశం, STRING_RTSP_SESSION_ID);
#లేకపోతే
    *param_charp = ఖాళీ;
#అయితే_ముగింపు
    ఆపు;
  స్థితి CURLINFO_SCHEME:
    *param_charp = దత్తాంశం->info.conn_scheme;
    ఆపు;
  స్థితి CURLINFO_CAPATH:
#నిర్వచితమైతే CURL_CA_మార్గం
    *param_charp = CURL_CA_మార్గం;
#లేకపోతే
    *param_charp = ఖాళీ;
#అయితే_ముగింపు
    ఆపు;
  స్థితి CURLINFO_CAINFO:
#నిర్వచితమైతే CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#లేకపోతే
    *param_charp = ఖాళీ;
#అయితే_ముగింపు
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode getinfo_long(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info,
                             దీర్ఘ *param_longp)
{
  curl_socket_t sockfd;

  సంఘం {
    గుర్తులేని దీర్ఘ *గమ్యం_ulong;
    దీర్ఘ          *గమ్యం_long;
  } lptr;

#నిర్వచితమైతే DEBUGBUILD
  స్థిరరాశి అక్షరం *envstr;

  /* use another variable for this to allow different values */
  ఎంపిక(info) {
  స్థితి CURLINFO_LOCAL_పోర్టు:
    envstr = getenv("CURL_TIME");
    ఒకవేళ(envstr) {
      curl_off_t val;
      curlx_str_సంఖ్య(&envstr, &val, సమయం_T_MAX);
      *param_longp = (దీర్ఘ)val;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  స్థితి CURLINFO_శీర్షిక_కొలత:
  స్థితి CURLINFO_REQUEST_కొలత:
    envstr = getenv("CURL_DEBUG_SIZE");
    ఒకవేళ(envstr) {
      curl_off_t val;
      curlx_str_సంఖ్య(&envstr, &val, LONG_MAX);
      *param_longp = (దీర్ఘ)val;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
#అయితే_ముగింపు

  ఎంపిక(info) {
  స్థితి CURLINFO_RESPONSE_సంకేతం:
    *param_longp = దత్తాంశం->info.httpcode;
    ఆపు;
  స్థితి CURLINFO_HTTP_CONNECTCODE:
    *param_longp = దత్తాంశం->info.httpproxycode;
    ఆపు;
  స్థితి CURLINFO_FILETIME:
    ఒకవేళ(దత్తాంశం->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#ఒకవేళ !defined(MSDOS) && !defined(__AMIGA__)
    లేకపోతే ఒకవేళ(దత్తాంశం->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#అయితే_ముగింపు
    లేకపోతే
      *param_longp = (దీర్ఘ)దత్తాంశం->info.filetime;
    ఆపు;
  స్థితి CURLINFO_శీర్షిక_కొలత:
    *param_longp = (దీర్ఘ)దత్తాంశం->info.శీర్షిక_కొలత;
    ఆపు;
  స్థితి CURLINFO_REQUEST_కొలత:
    *param_longp = (దీర్ఘ)దత్తాంశం->info.request_కొలత;
    ఆపు;
  స్థితి CURLINFO_SSL_VERIFYRESULT:
    *param_longp = దత్తాంశం->సమితి.ssl.certverifyresult;
    ఆపు;
  స్థితి CURLINFO_PROXY_SSL_VERIFYRESULT:
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    *param_longp = దత్తాంశం->సమితి.proxy_ssl.certverifyresult;
#లేకపోతే
    *param_longp = 0;
#అయితే_ముగింపు
    ఆపు;
  స్థితి CURLINFO_REDIRECT_లెక్క:
    *param_longp = దత్తాంశం->state.followlocation;
    ఆపు;
  స్థితి CURLINFO_HTTPAUTH_AVAIL:
    lptr.గమ్యం_long = param_longp;
    *lptr.గమ్యం_ulong = దత్తాంశం->info.httpauthavail;
    ఆపు;
  స్థితి CURLINFO_PROXYAUTH_AVAIL:
    lptr.గమ్యం_long = param_longp;
    *lptr.గమ్యం_ulong = దత్తాంశం->info.proxyauthavail;
    ఆపు;
  స్థితి CURLINFO_HTTPAUTH_వాడిన:
    lptr.గమ్యం_long = param_longp;
    *lptr.గమ్యం_ulong = దత్తాంశం->info.httpauthpicked;
    ఆపు;
  స్థితి CURLINFO_PROXYAUTH_వాడిన:
    lptr.గమ్యం_long = param_longp;
    *lptr.గమ్యం_ulong = దత్తాంశం->info.proxyauthpicked;
    ఆపు;
  స్థితి CURLINFO_OS_ERRNO:
    *param_longp = దత్తాంశం->state.os_errno;
    ఆపు;
  స్థితి CURLINFO_అంకె_CONNECTS:
#ఒకవేళ SIZEOF_LONG < SIZEOF_CURL_OFF_T
    ఒకవేళ(దత్తాంశం->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    లేకపోతే
#అయితే_ముగింపు
      *param_longp = (దీర్ఘ)దత్తాంశం->info.numconnects;
    ఆపు;
  స్థితి CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(దత్తాంశం, ఖాళీ);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    ఒకవేళ(sockfd != CURL_SOCKET_BAD)
      *param_longp = (దీర్ఘ)sockfd;
    లేకపోతే
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    ఆపు;
  స్థితి CURLINFO_PRIMARY_పోర్టు:
    /* Return the (remote) port of the most recent (primary) connection */
    ఒకవేళ(CUR_IP_QUAD_HAS_PORTS(&దత్తాంశం->info.primary))
      *param_longp = దత్తాంశం->info.primary.remote_పోర్టు;
    లేకపోతే
      *param_longp = -1;
    ఆపు;
  స్థితి CURLINFO_LOCAL_పోర్టు:
    /* Return the local port of the most recent (primary) connection */
    ఒకవేళ(CUR_IP_QUAD_HAS_PORTS(&దత్తాంశం->info.primary))
      *param_longp = దత్తాంశం->info.primary.local_పోర్టు;
    లేకపోతే
      *param_longp = -1;
    ఆపు;
  స్థితి CURLINFO_PROXY_ERROR:
    *param_longp = (దీర్ఘ)దత్తాంశం->info.pxcode;
    ఆపు;
  స్థితి CURLINFO_CONDITION_UNMET:
    ఒకవేళ(దత్తాంశం->info.httpcode == 304)
      *param_longp = 1L;
    లేకపోతే
      /* return if the condition prevented the document to get transferred */
      *param_longp = దత్తాంశం->info.timecond ? 1L : 0L;
    ఆపు;
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  స్థితి CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = దత్తాంశం->state.rtsp_తదుపరి_client_CSeq;
    ఆపు;
  స్థితి CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = దత్తాంశం->state.rtsp_తదుపరి_server_CSeq;
    ఆపు;
  స్థితి CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = దత్తాంశం->state.rtsp_CSeq_recv;
    ఆపు;
#లేకపోతే
  స్థితి CURLINFO_RTSP_CLIENT_CSEQ:
  స్థితి CURLINFO_RTSP_SERVER_CSEQ:
  స్థితి CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURLINFO_HTTP_సంస్కరణ:
    ఎంపిక(దత్తాంశం->info.httpversion) {
    స్థితి 10:
      *param_longp = CURL_HTTP_సంస్కరణ_1_0;
      ఆపు;
    స్థితి 11:
      *param_longp = CURL_HTTP_సంస్కరణ_1_1;
      ఆపు;
    స్థితి 20:
      *param_longp = CURL_HTTP_సంస్కరణ_2_0;
      ఆపు;
    స్థితి 30:
      *param_longp = CURL_HTTP_సంస్కరణ_3;
      ఆపు;
    అప్రమేయ:
      *param_longp = CURL_HTTP_సంస్కరణ_NONE;
      ఆపు;
    }
    ఆపు;
  స్థితి CURLINFO_PROTOCOL:
    *param_longp = (దీర్ఘ)దత్తాంశం->info.conn_protocol;
    ఆపు;
  స్థితి CURLINFO_వాడిన_PROXY:
    *param_longp =
#నిర్వచితమైతే CURL_DISABLE_PROXY
      0
#లేకపోతే
      దత్తాంశం->info.వాడిన_proxy
#అయితే_ముగింపు
      ;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }

  తిరిగి CURLE_OK;
}

#నిర్వచించు DOUBLE_SECS(x) ((ద్విగుణ)(x) / 1000000)

స్థిర CURLcode getinfo_offt(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info,
                             curl_off_t *param_offt)
{
#నిర్వచితమైతే DEBUGBUILD
  స్థిరరాశి అక్షరం *envstr;
  ఎంపిక(info) {
  స్థితి CURLINFO_మొత్తం_సమయం_T:
  స్థితి CURLINFO_NAMELOOKUP_సమయం_T:
  స్థితి CURLINFO_CONNECT_సమయం_T:
  స్థితి CURLINFO_APPCONNECT_సమయం_T:
  స్థితి CURLINFO_PRETRANSFER_సమయం_T:
  స్థితి CURLINFO_POSTTRANSFER_సమయం_T:
  స్థితి CURLINFO_వరుస_సమయం_T:
  స్థితి CURLINFO_STARTTRANSFER_సమయం_T:
  స్థితి CURLINFO_REDIRECT_సమయం_T:
  స్థితి CURLINFO_వేగం_DOWNLOAD_T:
  స్థితి CURLINFO_వేగం_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    ఒకవేళ(envstr) {
      curl_off_t val;
      curlx_str_సంఖ్య(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
#అయితే_ముగింపు
  ఎంపిక(info) {
  స్థితి CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)దత్తాంశం->info.filetime;
    ఆపు;
  స్థితి CURLINFO_కొలత_DELIVERED:
    *param_offt = దత్తాంశం->పురోగతి.deliver;
    ఆపు;
  స్థితి CURLINFO_కొలత_UPLOAD_T:
    *param_offt = దత్తాంశం->పురోగతి.ul.cur_కొలత;
    ఆపు;
  స్థితి CURLINFO_కొలత_DOWNLOAD_T:
    *param_offt = దత్తాంశం->పురోగతి.dl.cur_కొలత;
    ఆపు;
  స్థితి CURLINFO_వేగం_DOWNLOAD_T:
    *param_offt = దత్తాంశం->పురోగతి.dl.వేగం;
    ఆపు;
  స్థితి CURLINFO_వేగం_UPLOAD_T:
    *param_offt = దత్తాంశం->పురోగతి.ul.వేగం;
    ఆపు;
  స్థితి CURLINFO_CONTENT_నిడివి_DOWNLOAD_T:
    *param_offt = దత్తాంశం->పురోగతి.dl_కొలత_తెలిసిన ?
      దత్తాంశం->పురోగతి.dl.మొత్తం_కొలత : -1;
    ఆపు;
  స్థితి CURLINFO_CONTENT_నిడివి_UPLOAD_T:
    *param_offt = దత్తాంశం->పురోగతి.ul_కొలత_తెలిసిన ?
      దత్తాంశం->పురోగతి.ul.మొత్తం_కొలత : -1;
    ఆపు;
   స్థితి CURLINFO_మొత్తం_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.spent_us;
    ఆపు;
  స్థితి CURLINFO_NAMELOOKUP_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.nslookup_us;
    ఆపు;
  స్థితి CURLINFO_CONNECT_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.connect_us;
    ఆపు;
  స్థితి CURLINFO_APPCONNECT_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.appconnect_us;
    ఆపు;
  స్థితి CURLINFO_PRETRANSFER_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.pretransfer_us;
    ఆపు;
  స్థితి CURLINFO_POSTTRANSFER_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.posttransfer_us;
    ఆపు;
  స్థితి CURLINFO_STARTTRANSFER_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.starttransfer_us;
    ఆపు;
  స్థితి CURLINFO_వరుస_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.మొత్తం.queued_us;
    ఆపు;
  స్థితి CURLINFO_REDIRECT_సమయం_T:
    *param_offt = దత్తాంశం->పురోగతి.delta.startredirect_us;
    ఆపు;
  స్థితి CURLINFO_RETRY_AFTER:
    *param_offt = దత్తాంశం->info.retry_after;
    ఆపు;
  స్థితి CURLINFO_XFER_ID:
    *param_offt = దత్తాంశం->id;
    ఆపు;
  స్థితి CURLINFO_CONN_ID:
    *param_offt = దత్తాంశం->state.చివరి_conn_id;
    ఆపు;
  స్థితి CURLINFO_EARLYDATA_పంపిన_T:
    *param_offt = దత్తాంశం->పురోగతి.earlydata_పంపిన;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode getinfo_double(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info,
                               ద్విగుణ *param_doublep)
{
#నిర్వచితమైతే DEBUGBUILD
  స్థిరరాశి అక్షరం *envstr;

  ఎంపిక(info) {
  స్థితి CURLINFO_మొత్తం_సమయం:
  స్థితి CURLINFO_NAMELOOKUP_సమయం:
  స్థితి CURLINFO_CONNECT_సమయం:
  స్థితి CURLINFO_APPCONNECT_సమయం:
  స్థితి CURLINFO_PRETRANSFER_సమయం:
  స్థితి CURLINFO_STARTTRANSFER_సమయం:
  స్థితి CURLINFO_REDIRECT_సమయం:
  స్థితి CURLINFO_వేగం_DOWNLOAD:
  స్థితి CURLINFO_వేగం_UPLOAD:
    envstr = getenv("CURL_TIME");
    ఒకవేళ(envstr) {
      curl_off_t val;
      curlx_str_సంఖ్య(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (ద్విగుణ)val;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
#అయితే_ముగింపు
  ఎంపిక(info) {
  స్థితి CURLINFO_మొత్తం_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.మొత్తం.spent_us);
    ఆపు;
  స్థితి CURLINFO_NAMELOOKUP_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.మొత్తం.nslookup_us);
    ఆపు;
  స్థితి CURLINFO_CONNECT_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.మొత్తం.connect_us);
    ఆపు;
  స్థితి CURLINFO_APPCONNECT_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.మొత్తం.appconnect_us);
    ఆపు;
  స్థితి CURLINFO_PRETRANSFER_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.మొత్తం.pretransfer_us);
    ఆపు;
  స్థితి CURLINFO_STARTTRANSFER_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.మొత్తం.starttransfer_us);
    ఆపు;
  స్థితి CURLINFO_కొలత_UPLOAD:
    *param_doublep = (ద్విగుణ)దత్తాంశం->పురోగతి.ul.cur_కొలత;
    ఆపు;
  స్థితి CURLINFO_కొలత_DOWNLOAD:
    *param_doublep = (ద్విగుణ)దత్తాంశం->పురోగతి.dl.cur_కొలత;
    ఆపు;
  స్థితి CURLINFO_వేగం_DOWNLOAD:
    *param_doublep = (ద్విగుణ)దత్తాంశం->పురోగతి.dl.వేగం;
    ఆపు;
  స్థితి CURLINFO_వేగం_UPLOAD:
    *param_doublep = (ద్విగుణ)దత్తాంశం->పురోగతి.ul.వేగం;
    ఆపు;
  స్థితి CURLINFO_CONTENT_నిడివి_DOWNLOAD:
    *param_doublep = దత్తాంశం->పురోగతి.dl_కొలత_తెలిసిన ?
      (ద్విగుణ)దత్తాంశం->పురోగతి.dl.మొత్తం_కొలత : -1;
    ఆపు;
  స్థితి CURLINFO_CONTENT_నిడివి_UPLOAD:
    *param_doublep = దత్తాంశం->పురోగతి.ul_కొలత_తెలిసిన ?
      (ద్విగుణ)దత్తాంశం->పురోగతి.ul.మొత్తం_కొలత : -1;
    ఆపు;
  స్థితి CURLINFO_REDIRECT_సమయం:
    *param_doublep = DOUBLE_SECS(దత్తాంశం->పురోగతి.delta.startredirect_us);
    ఆపు;

  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode getinfo_slist(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info,
                              నిర్మాణం curl_slist **param_slistp)
{
  సంఘం {
    నిర్మాణం curl_certinfo *గమ్యం_certinfo;
    నిర్మాణం curl_slist    *గమ్యం_slist;
  } ptr;

  ఎంపిక(info) {
  స్థితి CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(దత్తాంశం);
    ఆపు;
  స్థితి CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(దత్తాంశం);
    ఆపు;
  స్థితి CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.గమ్యం_certinfo = &దత్తాంశం->info.certs;
    *param_slistp = ptr.గమ్యం_slist;
    ఆపు;
  స్థితి CURLINFO_TLS_SESSION:
  స్థితి CURLINFO_TLS_SSL_PTR: {
    పూర్ణాంకం query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    నిర్మాణం curl_tlssessioninfo **tsip = (నిర్మాణం curl_tlssessioninfo **)
      param_slistp;
    నిర్మాణం curl_tlssessioninfo *tsi = &దత్తాంశం->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    ఒకవేళ(!Curl_conn_get_ssl_info(దత్తాంశం, దత్తాంశం->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ఖాళీ;
    }
  }
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode getinfo_socket(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  ఎంపిక(info) {
  స్థితి CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(దత్తాంశం, ఖాళీ);
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_UNKNOWN_OPTION;
  }

  తిరిగి CURLE_OK;
}

CURLcode Curl_getinfo(నిర్మాణం Curl_easy *దత్తాంశం, CURLINFO info, ...)
{
  va_list arg;
  దీర్ఘ *param_longp = ఖాళీ;
  ద్విగుణ *param_doublep = ఖాళీ;
  curl_off_t *param_offt = ఖాళీ;
  స్థిరరాశి అక్షరం **param_charp = ఖాళీ;
  నిర్మాణం curl_slist **param_slistp = ఖాళీ;
  curl_socket_t *param_socketp = ఖాళీ;
  పూర్ణాంకం type;
  CURLcode ఫలితం = CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (పూర్ణాంకం)info;
  ఎంపిక(type) {
  స్థితి CURLINFO_STRING:
    param_charp = va_arg(arg, స్థిరరాశి అక్షరం **);
    ఒకవేళ(param_charp)
      ఫలితం = getinfo_char(దత్తాంశం, info, param_charp);
    ఆపు;
  స్థితి CURLINFO_LONG:
    param_longp = va_arg(arg, దీర్ఘ *);
    ఒకవేళ(param_longp)
      ఫలితం = getinfo_long(దత్తాంశం, info, param_longp);
    ఆపు;
  స్థితి CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, ద్విగుణ *);
    ఒకవేళ(param_doublep)
      ఫలితం = getinfo_double(దత్తాంశం, info, param_doublep);
    ఆపు;
  స్థితి CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    ఒకవేళ(param_offt)
      ఫలితం = getinfo_offt(దత్తాంశం, info, param_offt);
    ఆపు;
  స్థితి CURLINFO_SLIST:
    param_slistp = va_arg(arg, నిర్మాణం curl_slist **);
    ఒకవేళ(param_slistp)
      ఫలితం = getinfo_slist(దత్తాంశం, info, param_slistp);
    ఆపు;
  స్థితి CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    ఒకవేళ(param_socketp)
      ఫలితం = getinfo_socket(దత్తాంశం, info, param_socketp);
    ఆపు;
  అప్రమేయ:
    ఫలితం = CURLE_UNKNOWN_OPTION;
    ఆపు;
  }

  va_end(arg);

  తిరిగి ఫలితం;
}
