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
#include "curl_setup.h"

#include "urldata.h"
#include "getinfo.h"
#include "cfilters.h"
#include "vtls/vtls.h"
#include "connect.h" /* Curl_getconnectinfo() */
#include "bufref.h"
#include "url.h" /* Curl_uc_to_curlcode() */
#include "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
void Curl_initinfo(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Progress *pro = &data->progress;
  sa_nracanaa PureInfo *info = &data->info;

  memset(&pro->delta, 0, parimaa_na(pro->delta));
  memset(&pro->total, 0, parimaa_na(pro->total));
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

  memset(&info->primary, 0, parimaa_na(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

static CURLcode effective_url(sa_nracanaa Curl_easy *data,
                              const var_na **param_charp)
{
  const var_na *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? NULL : "";
  ᱡᱩᱫᱤ(s) {
    var_na *url = NULL;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    ᱡᱩᱫᱤ(!u)
      ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    ᱡᱩᱫᱤ(!uc)
      uc = curl_url_set(u, CURLUPART_USER, NULL, 0);
    ᱡᱩᱫᱤ(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, NULL, 0);
    ᱡᱩᱫᱤ(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    ᱡᱩᱫᱤ(uc || !url)
      ᱨᱩᱣᱟᱹᱲ uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    ᱡᱩᱫᱤ(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    ᱵᱟᱝ {
      curl_free(url);
      *param_charp = s;
    }
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode getinfo_char(sa_nracanaa Curl_easy *data, CURLINFO info,
                             const var_na **param_charp)
{
  switch(info) {
  pak_sa CURLINFO_EFFECTIVE_URL:
    ᱨᱩᱣᱟᱹᱲ effective_url(data, param_charp);
  pak_sa CURLINFO_EFFECTIVE_METHOD: {
    const var_na *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    ᱡᱩᱫᱤ(!m) {
      ᱡᱩᱫᱤ(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      ᱵᱟᱝ {
        switch(data->state.httpreq) {
        pak_sa HTTPREQ_POST:
        pak_sa HTTPREQ_POST_FORM:
        pak_sa HTTPREQ_POST_MIME:
          m = "POST";
          bha_nga;
        pak_sa HTTPREQ_PUT:
          m = "PUT";
          bha_nga;
        default: /* this should never happen */
        pak_sa HTTPREQ_GET:
          m = "GET";
          bha_nga;
        pak_sa HTTPREQ_HEAD:
          m = "HEAD";
          bha_nga;
        }
      }
#endif
    }
    *param_charp = m;
  }
    bha_nga;
  pak_sa CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    bha_nga;
  pak_sa CURLINFO_PRIVATE:
    *param_charp = (const var_na *)data->set.private_data;
    bha_nga;
  pak_sa CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    bha_nga;
  pak_sa CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    bha_nga;
  pak_sa CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    bha_nga;
  pak_sa CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    bha_nga;
  pak_sa CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    bha_nga;
  pak_sa CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#ᱵᱟᱝ
    *param_charp = NULL;
#endif
    bha_nga;
  pak_sa CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    bha_nga;
  pak_sa CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#ᱵᱟᱝ
    *param_charp = NULL;
#endif
    bha_nga;
  pak_sa CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#ᱵᱟᱝ
    *param_charp = NULL;
#endif
    bha_nga;
  default:
    ᱨᱩᱣᱟᱹᱲ CURLE_UNKNOWN_OPTION;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode getinfo_long(sa_nracanaa Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  sa_ngha {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  const var_na *envstr;

  /* use another variable for this to allow different values */
  switch(info) {
  pak_sa CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    ᱡᱩᱫᱤ(envstr) {
      curl_off_t val;
      curlx_str_number(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }
    bha_nga;
  pak_sa CURLINFO_HEADER_SIZE:
  pak_sa CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    ᱡᱩᱫᱤ(envstr) {
      curl_off_t val;
      curlx_str_number(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }
    bha_nga;
  default:
    bha_nga;
  }
#endif

  switch(info) {
  pak_sa CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    bha_nga;
  pak_sa CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    bha_nga;
  pak_sa CURLINFO_FILETIME:
    ᱡᱩᱫᱤ(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#ᱡᱩᱫᱤ !defined(MSDOS) && !defined(__AMIGA__)
    ᱵᱟᱝ ᱡᱩᱫᱤ(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    ᱵᱟᱝ
      *param_longp = (long)data->info.filetime;
    bha_nga;
  pak_sa CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    bha_nga;
  pak_sa CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    bha_nga;
  pak_sa CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    bha_nga;
  pak_sa CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#ᱵᱟᱝ
    *param_longp = 0;
#endif
    bha_nga;
  pak_sa CURLINFO_REDIRECT_COUNT:
    *param_longp = data->state.followlocation;
    bha_nga;
  pak_sa CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    bha_nga;
  pak_sa CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    bha_nga;
  pak_sa CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    bha_nga;
  pak_sa CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    bha_nga;
  pak_sa CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    bha_nga;
  pak_sa CURLINFO_NUM_CONNECTS:
#ᱡᱩᱫᱤ SIZEOF_LONG < SIZEOF_CURL_OFF_T
    ᱡᱩᱫᱤ(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    ᱵᱟᱝ
#endif
      *param_longp = (long)data->info.numconnects;
    bha_nga;
  pak_sa CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, NULL);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    ᱡᱩᱫᱤ(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    ᱵᱟᱝ
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    bha_nga;
  pak_sa CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    ᱡᱩᱫᱤ(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    ᱵᱟᱝ
      *param_longp = -1;
    bha_nga;
  pak_sa CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    ᱡᱩᱫᱤ(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    ᱵᱟᱝ
      *param_longp = -1;
    bha_nga;
  pak_sa CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    bha_nga;
  pak_sa CURLINFO_CONDITION_UNMET:
    ᱡᱩᱫᱤ(data->info.httpcode == 304)
      *param_longp = 1L;
    ᱵᱟᱝ
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    bha_nga;
#ifndef CURL_DISABLE_RTSP
  pak_sa CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    bha_nga;
  pak_sa CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    bha_nga;
  pak_sa CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    bha_nga;
#ᱵᱟᱝ
  pak_sa CURLINFO_RTSP_CLIENT_CSEQ:
  pak_sa CURLINFO_RTSP_SERVER_CSEQ:
  pak_sa CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    bha_nga;
#endif
  pak_sa CURLINFO_HTTP_VERSION:
    switch(data->info.httpversion) {
    pak_sa 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      bha_nga;
    pak_sa 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      bha_nga;
    pak_sa 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      bha_nga;
    pak_sa 30:
      *param_longp = CURL_HTTP_VERSION_3;
      bha_nga;
    default:
      *param_longp = CURL_HTTP_VERSION_NONE;
      bha_nga;
    }
    bha_nga;
  pak_sa CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    bha_nga;
  pak_sa CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#ᱵᱟᱝ
      data->info.used_proxy
#endif
      ;
    bha_nga;
  default:
    ᱨᱩᱣᱟᱹᱲ CURLE_UNKNOWN_OPTION;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

#define DOUBLE_SECS(x) ((dviguu_na)(x) / 1000000)

static CURLcode getinfo_offt(sa_nracanaa Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  const var_na *envstr;
  switch(info) {
  pak_sa CURLINFO_TOTAL_TIME_T:
  pak_sa CURLINFO_NAMELOOKUP_TIME_T:
  pak_sa CURLINFO_CONNECT_TIME_T:
  pak_sa CURLINFO_APPCONNECT_TIME_T:
  pak_sa CURLINFO_PRETRANSFER_TIME_T:
  pak_sa CURLINFO_POSTTRANSFER_TIME_T:
  pak_sa CURLINFO_QUEUE_TIME_T:
  pak_sa CURLINFO_STARTTRANSFER_TIME_T:
  pak_sa CURLINFO_REDIRECT_TIME_T:
  pak_sa CURLINFO_SPEED_DOWNLOAD_T:
  pak_sa CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    ᱡᱩᱫᱤ(envstr) {
      curl_off_t val;
      curlx_str_number(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }
    bha_nga;
  default:
    bha_nga;
  }
#endif
  switch(info) {
  pak_sa CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    bha_nga;
  pak_sa CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    bha_nga;
  pak_sa CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    bha_nga;
  pak_sa CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    bha_nga;
  pak_sa CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    bha_nga;
  pak_sa CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    bha_nga;
  pak_sa CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    bha_nga;
  pak_sa CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    bha_nga;
   pak_sa CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    bha_nga;
  pak_sa CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    bha_nga;
  pak_sa CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    bha_nga;
  pak_sa CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    bha_nga;
  pak_sa CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    bha_nga;
  pak_sa CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    bha_nga;
  pak_sa CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    bha_nga;
  pak_sa CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    bha_nga;
  pak_sa CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    bha_nga;
  pak_sa CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    bha_nga;
  pak_sa CURLINFO_XFER_ID:
    *param_offt = data->id;
    bha_nga;
  pak_sa CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    bha_nga;
  pak_sa CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    bha_nga;
  default:
    ᱨᱩᱣᱟᱹᱲ CURLE_UNKNOWN_OPTION;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode getinfo_double(sa_nracanaa Curl_easy *data, CURLINFO info,
                               dviguu_na *param_doublep)
{
#ifdef DEBUGBUILD
  const var_na *envstr;

  switch(info) {
  pak_sa CURLINFO_TOTAL_TIME:
  pak_sa CURLINFO_NAMELOOKUP_TIME:
  pak_sa CURLINFO_CONNECT_TIME:
  pak_sa CURLINFO_APPCONNECT_TIME:
  pak_sa CURLINFO_PRETRANSFER_TIME:
  pak_sa CURLINFO_STARTTRANSFER_TIME:
  pak_sa CURLINFO_REDIRECT_TIME:
  pak_sa CURLINFO_SPEED_DOWNLOAD:
  pak_sa CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    ᱡᱩᱫᱤ(envstr) {
      curl_off_t val;
      curlx_str_number(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (dviguu_na)val;
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }
    bha_nga;
  default:
    bha_nga;
  }
#endif
  switch(info) {
  pak_sa CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    bha_nga;
  pak_sa CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    bha_nga;
  pak_sa CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    bha_nga;
  pak_sa CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    bha_nga;
  pak_sa CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    bha_nga;
  pak_sa CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    bha_nga;
  pak_sa CURLINFO_SIZE_UPLOAD:
    *param_doublep = (dviguu_na)data->progress.ul.cur_size;
    bha_nga;
  pak_sa CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (dviguu_na)data->progress.dl.cur_size;
    bha_nga;
  pak_sa CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (dviguu_na)data->progress.dl.speed;
    bha_nga;
  pak_sa CURLINFO_SPEED_UPLOAD:
    *param_doublep = (dviguu_na)data->progress.ul.speed;
    bha_nga;
  pak_sa CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (dviguu_na)data->progress.dl.total_size : -1;
    bha_nga;
  pak_sa CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (dviguu_na)data->progress.ul.total_size : -1;
    bha_nga;
  pak_sa CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    bha_nga;

  default:
    ᱨᱩᱣᱟᱹᱲ CURLE_UNKNOWN_OPTION;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode getinfo_slist(sa_nracanaa Curl_easy *data, CURLINFO info,
                              sa_nracanaa curl_slist **param_slistp)
{
  sa_ngha {
    sa_nracanaa curl_certinfo *to_certinfo;
    sa_nracanaa curl_slist    *to_slist;
  } ptr;

  switch(info) {
  pak_sa CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    bha_nga;
  pak_sa CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    bha_nga;
  pak_sa CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    bha_nga;
  pak_sa CURLINFO_TLS_SESSION:
  pak_sa CURLINFO_TLS_SSL_PTR: {
    puur_naa_nka query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    sa_nracanaa curl_tlssessioninfo **tsip = (sa_nracanaa curl_tlssessioninfo **)
      param_slistp;
    sa_nracanaa curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    ᱡᱩᱫᱤ(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = NULL;
    }
  }
    bha_nga;
  default:
    ᱨᱩᱣᱟᱹᱲ CURLE_UNKNOWN_OPTION;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode getinfo_socket(sa_nracanaa Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  switch(info) {
  pak_sa CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, NULL);
    bha_nga;
  default:
    ᱨᱩᱣᱟᱹᱲ CURLE_UNKNOWN_OPTION;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

CURLcode Curl_getinfo(sa_nracanaa Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = NULL;
  dviguu_na *param_doublep = NULL;
  curl_off_t *param_offt = NULL;
  const var_na **param_charp = NULL;
  sa_nracanaa curl_slist **param_slistp = NULL;
  curl_socket_t *param_socketp = NULL;
  puur_naa_nka type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  ᱡᱩᱫᱤ(!data)
    ᱨᱩᱣᱟᱹᱲ CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (puur_naa_nka)info;
  switch(type) {
  pak_sa CURLINFO_STRING:
    param_charp = va_arg(arg, const var_na **);
    ᱡᱩᱫᱤ(param_charp)
      result = getinfo_char(data, info, param_charp);
    bha_nga;
  pak_sa CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    ᱡᱩᱫᱤ(param_longp)
      result = getinfo_long(data, info, param_longp);
    bha_nga;
  pak_sa CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, dviguu_na *);
    ᱡᱩᱫᱤ(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    bha_nga;
  pak_sa CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    ᱡᱩᱫᱤ(param_offt)
      result = getinfo_offt(data, info, param_offt);
    bha_nga;
  pak_sa CURLINFO_SLIST:
    param_slistp = va_arg(arg, sa_nracanaa curl_slist **);
    ᱡᱩᱫᱤ(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    bha_nga;
  pak_sa CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    ᱡᱩᱫᱤ(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    bha_nga;
  default:
    result = CURLE_UNKNOWN_OPTION;
    bha_nga;
  }

  va_end(arg);

  ᱨᱩᱣᱟᱹᱲ result;
}
