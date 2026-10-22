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
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "vtls/vtls.h"

#అన్తర్భావ "curl_trc.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#నిర్వచితమైతే HAVE_NETINET_UDP_H
#అన్తర్భావ <netinet/udp.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_IP_H
#అన్తర్భావ <netinet/ip.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NGHTTP3
#అన్తర్భావ <nghttp3/nghttp3.h>
#అయితే_ముగింపు

#ఒకవేళ defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#అన్తర్భావ <sys/syscall.h>
#ఒకవేళ defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#నిర్వచించు HAVE_APPLE_సందేశం_X
#అయితే_ముగింపు
#అయితే_ముగింపు

#అన్తర్భావ "bufq.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vquic/cf-ngtcp2.h"
#అన్తర్భావ "vquic/cf-ngtcp2-cmn.h"
#అన్తర్భావ "vquic/cf-ngtcp2-proxy.h"
#అన్తర్భావ "vquic/cf-quiche.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "vquic/vquic_int.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strparse.h"


#నిర్వచించు NW_CHUNK_కొలత     (64 * 1024)
#నిర్వచించు NW_SEND_CHUNKS    1

#నిర్వచితమైతే HAVE_APPLE_సందేశం_X

/* this is `struct msghdr` with an additional field at the end */
నిర్మాణం msghdr_x {
  శూన్య *సందేశం_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  నిర్మాణం iovec *msg_iov;    /* scatter/gather array */
  పూర్ణాంకం సందేశం_iovlen;           /* # elements in msg_iov */
  శూన్య *సందేశం_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  పూర్ణాంకం సందేశం_flags;            /* flags on received message */
  size_t సందేశం_datalen;       /* byte length of buffer in msg_iov */
};
#అయితే_ముగింపు

#నిర్వచితమైతే CURLVERBOSE
#నిర్వచితమైతే HAVE_APPLE_సందేశం_X
#నిర్వచించు VQUIC_SEND_పద్ధతి   "sendmsg_x"
#లేదావేళ defined(HAVE_SENDMSG)
#నిర్వచించు VQUIC_SEND_పద్ధతి   "sendmsg"
#లేకపోతే
#నిర్వచించు VQUIC_SEND_పద్ధతి   "send"
#అయితే_ముగింపు
#అయితే_ముగింపు

పూర్ణాంకం Curl_vquic_init(శూన్య)
{
#ఒకవేళ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  ఒకవేళ(ngtcp2_crypto_ossl_init())
    తిరిగి 0;
#అయితే_ముగింపు

  తిరిగి 1;
}

శూన్య Curl_vquic_cleanup(శూన్య)
{
#ఒకవేళ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_సంస్కరణ_అంకె >= 0x011800)
  ngtcp2_crypto_ossl_free();
#అయితే_ముగింపు
}

శూన్య Curl_quic_ver(అక్షరం *p, size_t len)
{
#ఒకవేళ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#లేదావేళ defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#అయితే_ముగింపు
}

CURLcode Curl_vquic_ctx_init(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_కొలత, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_హద్దు);
#ఒకవేళ defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#లేదావేళ defined(HAVE_APPLE_సందేశం_X)
  qctx->no_gso = FALSE;
#లేకపోతే
  qctx->no_gso = TRUE;
#అయితే_ముగింపు
#నిర్వచితమైతే DEBUGBUILD
  {
    స్థిరరాశి అక్షరం *p = getenv("CURL_DBG_QUIC_WBLOCK");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, 100))
        qctx->wblock_percent = (పూర్ణాంకం)l;
    }
  }
#అయితే_ముగింపు
  Curl_vquic_ctx_సమితి_సమయం(qctx, Curl_pgrs_now(దత్తాంశం));

  తిరిగి CURLE_OK;
}

శూన్య Curl_vquic_ctx_free(నిర్మాణం cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

శూన్య Curl_vquic_ctx_సమితి_సమయం(నిర్మాణం cf_quic_ctx *qctx,
                             స్థిరరాశి నిర్మాణం curltime *pnow)
{
  qctx->చివరి_op = *pnow;
}

శూన్య Curl_vquic_ctx_update_సమయం(నిర్మాణం cf_quic_ctx *qctx,
                                స్థిరరాశి నిర్మాణం curltime *pnow)
{
  qctx->చివరి_op = *pnow;
}

#నిర్వచితమైతే HAVE_APPLE_సందేశం_X
స్థిర CURLcode do_sendmsg(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం cf_quic_ctx *qctx,
                           స్థిరరాశి uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#నిర్వచించు సందేశం_X_SNUM  64
  నిర్మాణం iovec msg_iov[సందేశం_X_SNUM];
  నిర్మాణం msghdr_x mmsg[సందేశం_X_SNUM];
  అక్షరం errstr[STRERROR_LEN];
  size_t n, i = 0, పంపిన = 0;
  పూర్ణాంకం rc;
  CURLcode ఫలితం = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  ఒకవేళ(!pktlen)
    తిరిగి CURLE_OK;
  ఒకవేళ(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  అయితే(i < n) {
    size_t j, batch = CURLMIN(n - i, సందేశం_X_SNUM), pkts_పంపిన = 0;

    కొరకు(j = 0; j < batch; ++j) {
      స్థిరరాశి size_t స్థానభ్రంశం = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + స్థానభ్రంశం);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - స్థానభ్రంశం);
      memset(&mmsg[j], 0, పరిమాణం(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].సందేశం_iovlen = 1;
      mmsg[j].సందేశం_datalen = msg_iov[j].iov_len;
    }

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు
    అయితే((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

    ఒకవేళ(rc < 0) {
      ఒకవేళ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(దత్తాంశం, cf, "egress, sendmsg_x -> EAGAIN");
        ఫలితం = పంపిన ? CURLE_OK : CURLE_AGAIN;
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, పరిమాణం(errstr));
        failf(దత్తాంశం, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        ఫలితం = CURLE_SEND_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_పంపిన = batch;
    }
    లేకపోతే
      pkts_పంపిన = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    ఒకవేళ(!pkts_పంపిన) {  /* no packets of the current batch were sent */
      ఫలితం = పంపిన ? CURLE_OK : CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    }
    i += pkts_పంపిన; /* Some have been sent */
    కొరకు(j = 0; j < pkts_పంపిన; ++j)
      పంపిన += msg_iov[j].iov_len;
    ఒకవేళ(pkts_పంపిన < batch)
      వెళ్ళు బహిర్గతం; /* but not all of them */
  }

బహిర్గతం:
  *psent = పంపిన;
  ఒకవేళ(పంపిన || ఫలితం)
    CURL_TRC_CF(దత్తాంశం, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                పంపిన, gsolen, n, calls, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

#లేకపోతే /* HAVE_APPLE_MSG_X */

స్థిర CURLcode send_packet_no_gso(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం cf_quic_ctx *qctx,
                                   స్థిరరాశి uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

స్థిర CURLcode do_sendmsg(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం cf_quic_ctx *qctx,
                           స్థిరరాశి uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode ఫలితం = CURLE_OK;
#నిర్వచితమైతే HAVE_SENDMSG
  నిర్మాణం iovec msg_iov;
  నిర్మాణం msghdr సందేశం = { 0 };
  ssize_t rv;
#ఒకవేళ defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t సందేశం_ctrl[32];
  నిర్మాణం cmsghdr *cm;
#అయితే_ముగింపు

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  సందేశం.msg_iov = &msg_iov;
  సందేశం.సందేశం_iovlen = 1;

#ఒకవేళ defined(__linux__) && defined(UDP_SEGMENT)
  ఒకవేళ(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(సందేశం_ctrl, 0, పరిమాణం(సందేశం_ctrl));
    సందేశం.సందేశం_control = సందేశం_ctrl;
    DEBUGASSERT(పరిమాణం(సందేశం_ctrl) >= CMSG_SPACE(పరిమాణం(పూర్ణాంకం)));
    సందేశం.msg_controllen = CMSG_SPACE(పరిమాణం(పూర్ణాంకం));
    cm = CMSG_FIRSTHDR(&సందేశం);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(పరిమాణం(uint16_t));
    *(uint16_t *)(శూన్య *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#అయితే_ముగింపు

  అయితే((rv = sendmsg(qctx->sockfd, &సందేశం, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  ఒకవేళ(!curlx_sztouz(rv, psent)) {
    పూర్ణాంకం sockerr = SOCKERRNO;
    ఒకవేళ(SOCK_EAGAIN(sockerr))
      తిరిగి CURLE_AGAIN;
    ఎంపిక(sockerr) {
    స్థితి SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      ఆపు;
    స్థితి EIO:
      ఒకవేళ(pktlen > gsolen) {
        /* GSO failure */
        infof(దత్తాంశం, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        తిరిగి send_packet_no_gso(cf, దత్తాంశం, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    అప్రమేయ:
      failf(దత్తాంశం, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే ఒకవేళ(pktlen != *psent) {
    failf(దత్తాంశం, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
#లేకపోతే
  ssize_t rv;
  (శూన్య)gsolen;

  *psent = 0;

  అయితే((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  ఒకవేళ(!curlx_sztouz(rv, psent)) {
    ఒకవేళ(SOCK_EAGAIN(SOCKERRNO)) {
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      ఒకవేళ(SOCKERRNO != SOCKEMSGSIZE) {
        failf(దత్తాంశం, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        ఫలితం = CURLE_SEND_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#అయితే_ముగింపు
  (శూన్య)cf;

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_పద్ధతి, pktlen, gsolen, (పూర్ణాంకం)ఫలితం, *psent);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !HAVE_APPLE_MSG_X */

స్థిర CURLcode send_packet_no_gso(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం cf_quic_ctx *qctx,
                                   స్థిరరాశి uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  స్థిరరాశి uint8_t *p, *చివర = pkt + pktlen;
  size_t పంపిన, len;
  CURLcode ఫలితం = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  కొరకు(p = pkt; p < చివర; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(చివర - p));
    ఫలితం = do_sendmsg(cf, దత్తాంశం, qctx, p, len, len, &పంపిన);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    *psent += పంపిన;
    VERBOSE(++calls);
  }
బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_పద్ధతి, pktlen, gsolen, calls, (పూర్ణాంకం)ఫలితం, *psent);
  తిరిగి ఫలితం;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
స్థిర CURLcode send_packet_no_gso_cf(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      స్థిరరాశి uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  స్థిరరాశి uint8_t *p, *చివర = pkt + pktlen;
  size_t పంపిన, len;
  CURLcode ఫలితం = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  కొరకు(p = pkt; p < చివర; p += len) {
    len = CURLMIN(gsolen, (size_t)(చివర - p));
    ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, p, len, FALSE, &పంపిన);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఒకవేళ(పంపిన != len) {
      /* We can only send the complete datagram, not parts. */
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    *psent += పంపిన;
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (పూర్ణాంకం)ఫలితం, *psent);
  తిరిగి ఫలితం;
}

స్థిర CURLcode vquic_send_packets(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం cf_quic_ctx *qctx,
                                   స్థిరరాశి uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode ఫలితం;
#నిర్వచితమైతే DEBUGBUILD
  /* simulate network blocking/partial writes */
  ఒకవేళ(qctx->wblock_percent > 0) {
    గుర్తులేని అక్షరం c;
    *psent = 0;
    Curl_rand(దత్తాంశం, &c, 1);
    ఒకవేళ(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(దత్తాంశం, cf, "vquic_flush() simulate EWOULDBLOCK");
      తిరిగి CURLE_AGAIN;
    }
  }
#అయితే_ముగింపు
  ఒకవేళ(qctx->no_gso && pktlen > gsolen) {
    ఫలితం = send_packet_no_gso(cf, దత్తాంశం, qctx, pkt, pktlen, gsolen, psent);
  }
  లేకపోతే {
    ఫలితం = do_sendmsg(cf, దత్తాంశం, qctx, pkt, pktlen, gsolen, psent);
  }
  ఒకవేళ(!ఫలితం)
    qctx->చివరి_io = qctx->చివరి_op;
  తిరిగి ఫలితం;
}

CURLcode Curl_vquic_flush(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం cf_quic_ctx *qctx)
{
  స్థిరరాశి గుర్తులేని అక్షరం *buf;
  size_t blen, పంపిన;
  CURLcode ఫలితం;
  size_t gsolen;

  అయితే(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    ఒకవేళ(qctx->విభజించు_len) {
      gsolen = qctx->విభజించు_gsolen;
      ఒకవేళ(blen > qctx->విభజించు_len)
        blen = qctx->విభజించు_len;
    }

    ఒకవేళ(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      ఫలితం = vquic_send_packets(cf, దత్తాంశం, qctx, buf, blen, gsolen, &పంపిన);
    }
    లేకపోతే {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      ఒకవేళ(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        ఫలితం = send_packet_no_gso_cf(cf, దత్తాంశం, buf, blen, gsolen, &పంపిన);
      }
      లేకపోతే {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, buf, blen, FALSE, &పంపిన);
      }
    }

    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, పంపిన);
        ఒకవేళ(qctx->విభజించు_len)
          qctx->విభజించు_len -= పంపిన;
      }
      తిరిగి ఫలితం;
    }
    Curl_bufq_skip(&qctx->sendbuf, పంపిన);
    ఒకవేళ(qctx->విభజించు_len)
      qctx->విభజించు_len -= పంపిన;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_vquic_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  తిరిగి Curl_vquic_flush(cf, దత్తాంశం, qctx);
}

CURLcode Curl_vquic_send_తోక_విభజించు(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం cf_quic_ctx *qctx, size_t gsolen,
                                    size_t తోక_len, size_t తోక_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > తోక_len);
  qctx->విభజించు_len = Curl_bufq_len(&qctx->sendbuf) - తోక_len;
  qctx->విభజించు_gsolen = gsolen;
  qctx->gsolen = తోక_gsolen;
  CURL_TRC_CF(దత్తాంశం, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->విభజించు_len, qctx->విభజించు_gsolen, తోక_len, qctx->gsolen);
  తిరిగి Curl_vquic_flush(cf, దత్తాంశం, qctx);
}

#ఒకవేళ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_సందేశం_X)
స్థిర size_t vquic_msghdr_get_udp_gro(నిర్మాణం msghdr *సందేశం)
{
  పూర్ణాంకం gso_కొలత = 0;
#ఒకవేళ defined(__linux__) && defined(UDP_GRO)
  నిర్మాణం cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#ఒకవేళ defined(__clang__) && !defined(__GLIBC__)
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wsign-compare"
#సూచన clang diagnostic ignored "-Wcast-align"
#అయితే_ముగింపు
  కొరకు(cmsg = CMSG_FIRSTHDR(సందేశం); cmsg; cmsg = CMSG_NXTHDR(సందేశం, cmsg)) {
#ఒకవేళ defined(__clang__) && !defined(__GLIBC__)
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
    ఒకవేళ(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      జ్ఞాపకం_నకలు(&gso_కొలత, CMSG_DATA(cmsg), పరిమాణం(gso_కొలత));

      ఆపు;
    }
  }
#అయితే_ముగింపు /* linux && UDP_GRO */
  (శూన్య)సందేశం;

  తిరిగి (size_t)gso_కొలత;
}
#అయితే_ముగింపు /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#ఒకవేళ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_సందేశం_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_ముసుగు)
స్థిర uint8_t vquic_msghdr_get_ecn(నిర్మాణం msghdr *సందేశం, పూర్ణాంకం family)
{
  నిర్మాణం cmsghdr *cmsg;
  ఎంపిక(family) {
  స్థితి AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#ఒకవేళ defined(__clang__) && !defined(__GLIBC__)
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wsign-compare"
#సూచన clang diagnostic ignored "-Wcast-align"
#అయితే_ముగింపు
    కొరకు(cmsg = CMSG_FIRSTHDR(సందేశం); cmsg; cmsg = CMSG_NXTHDR(సందేశం, cmsg)) {
#ఒకవేళ defined(__clang__) && !defined(__GLIBC__)
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
      ఒకవేళ(cmsg->cmsg_level == IPPROTO_IP &&
#నిర్వచితమైతే __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#లేకపోతే
          cmsg->cmsg_type == IP_TOS
#అయితే_ముగింపు
          && cmsg->cmsg_len) {
        తిరిగి *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_ముసుగు;
      }
    }
    ఆపు;
  స్థితి AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#ఒకవేళ defined(__clang__) && !defined(__GLIBC__)
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wsign-compare"
#సూచన clang diagnostic ignored "-Wcast-align"
#అయితే_ముగింపు
    కొరకు(cmsg = CMSG_FIRSTHDR(సందేశం); cmsg; cmsg = CMSG_NXTHDR(సందేశం, cmsg)) {
#ఒకవేళ defined(__clang__) && !defined(__GLIBC__)
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
      ఒకవేళ(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        గుర్తులేని పూర్ణాంకం tos;

        జ్ఞాపకం_నకలు(&tos, CMSG_DATA(cmsg), పరిమాణం(పూర్ణాంకం));

        తిరిగి (uint8_t)(tos & IPTOS_ECN_ముసుగు);
      }
    }
    ఆపు;
  }
  తిరిగి 0;
}
#లేకపోతే
#నిర్వచించు vquic_msghdr_get_ecn(a, b)      0
#అయితే_ముగింపు /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#నిర్వచితమైతే HAVE_SENDMMSG

స్థిర CURLcode recvmmsg_packets(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, శూన్య *userp,
                                 size_t *pnread)
{
#ఒకవేళ defined(__linux__) && defined(UDP_GRO)
#నిర్వచించు MMSG_అంకె  16
#నిర్వచించు UDP_GRO_CNT_MAX  64
#నిర్వచించు CMSG_చొప్పున_సందేశం_కొలత    (2 * CMSG_SPACE(పరిమాణం(పూర్ణాంకం)))
#లేకపోతే
#నిర్వచించు MMSG_అంకె  64
#నిర్వచించు UDP_GRO_CNT_MAX  1
#నిర్వచించు CMSG_చొప్పున_సందేశం_కొలత    CMSG_SPACE(పరిమాణం(పూర్ణాంకం))
#అయితే_ముగింపు
#నిర్వచించు సందేశం_BUF_కొలత  (UDP_GRO_CNT_MAX * 1500)
  నిర్మాణం iovec msg_iov[MMSG_అంకె];
  నిర్మాణం mmsghdr mmsg[MMSG_అంకె];
  uint8_t సందేశం_ctrl[MMSG_అంకె * CMSG_చొప్పున_సందేశం_కొలత];
  నిర్మాణం sockaddr_storage remote_addr[MMSG_అంకె];
  size_t మొత్తం_nread = 0, pkts = 0;
#నిర్వచితమైతే CURLVERBOSE
  size_t calls = 0;
#అయితే_ముగింపు
  పూర్ణాంకం mcount, i, n;
  అక్షరం errstr[STRERROR_LEN];
  CURLcode ఫలితం = CURLE_OK;
  size_t gso_కొలత;
  అక్షరం *sockbuf = ఖాళీ;
  uint8_t (*bufs)[సందేశం_BUF_కొలత] = ఖాళీ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ఫలితం = Curl_multi_xfer_sockbuf_borrow(దత్తాంశం, MMSG_అంకె * సందేశం_BUF_కొలత,
                                          &sockbuf);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  bufs = (uint8_t (*)[సందేశం_BUF_కొలత])sockbuf;

  మొత్తం_nread = 0;
  అయితే(pkts < max_pkts) {
    n = (పూర్ణాంకం)CURLMIN(CURLMIN(MMSG_అంకె, IOV_MAX), max_pkts);
    memset(&mmsg, 0, పరిమాణం(mmsg));
    కొరకు(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = పరిమాణం(bufs[i]);
      mmsg[i].సందేశం_hdr.msg_iov = &msg_iov[i];
      mmsg[i].సందేశం_hdr.సందేశం_iovlen = 1;
      mmsg[i].సందేశం_hdr.సందేశం_name = &remote_addr[i];
      mmsg[i].సందేశం_hdr.msg_namelen = పరిమాణం(remote_addr[i]);
      mmsg[i].సందేశం_hdr.సందేశం_control = &సందేశం_ctrl[i * CMSG_చొప్పున_సందేశం_కొలత];
      mmsg[i].సందేశం_hdr.msg_controllen = CMSG_చొప్పున_సందేశం_కొలత;
    }

    అయితే((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ఖాళీ)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ఒకవేళ(mcount == -1) {
      ఒకవేళ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(దత్తాంశం, cf, "ingress, recvmmsg -> EAGAIN");
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        నిర్మాణం ip_quadruple ip;
        ఒకవేళ(!Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, ఖాళీ, ఖాళీ, &ip))
          failf(దత్తాంశం, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_పోర్టు);
        ఫలితం = CURLE_COULDNT_CONNECT;
        వెళ్ళు బహిర్గతం;
      }
      curlx_strerror(SOCKERRNO, errstr, పరిమాణం(errstr));
      failf(దత్తాంశం, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    VERBOSE(++calls);
    కొరకు(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ఒకవేళ(!mmsg[i].సందేశం_len) {
        ++pkts;
        కొనసాగు;
      }
      మొత్తం_nread += mmsg[i].సందేశం_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].సందేశం_hdr, remote_addr[i].ss_family);
      gso_కొలత = vquic_msghdr_get_udp_gro(&mmsg[i].సందేశం_hdr);
      ఒకవేళ(gso_కొలత == 0)
        gso_కొలత = mmsg[i].సందేశం_len;

      ఫలితం = recv_cb(bufs[i], mmsg[i].సందేశం_len, gso_కొలత,
                       mmsg[i].సందేశం_hdr.సందేశం_name,
                       mmsg[i].సందేశం_hdr.msg_namelen, ecn, userp);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      pkts += (mmsg[i].సందేశం_len + gso_కొలత - 1) / gso_కొలత;
    }
  }

బహిర్గతం:
  ఒకవేళ(మొత్తం_nread || ఫలితం)
    CURL_TRC_CF(దత్తాంశం, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                మొత్తం_nread, pkts, calls, (పూర్ణాంకం)ఫలితం);
  Curl_multi_xfer_sockbuf_release(దత్తాంశం, sockbuf);
  *pnread = మొత్తం_nread;
  తిరిగి ఫలితం;
}

#లేదావేళ defined(HAVE_APPLE_సందేశం_X)

స్థిర CURLcode recvmsg_x_packets(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  శూన్య *userp, size_t *pnread)
{
#నిర్వచించు సందేశం_X_అంకె  64
#నిర్వచించు సందేశం_BUF_కొలత  (2048)
#నిర్వచించు CMSG_చొప్పున_సందేశం_కొలత     CMSG_SPACE(పరిమాణం(పూర్ణాంకం))
  నిర్మాణం iovec msg_iov[సందేశం_X_అంకె];
  నిర్మాణం msghdr_x mmsg[సందేశం_X_అంకె];
  uint8_t సందేశం_ctrl[సందేశం_X_అంకె * CMSG_చొప్పున_సందేశం_కొలత];
  నిర్మాణం sockaddr_storage remote_addr[సందేశం_X_అంకె];
  size_t మొత్తం_nread = 0, pkts = 0;
#నిర్వచితమైతే CURLVERBOSE
  size_t calls = 0;
#అయితే_ముగింపు
  పూర్ణాంకం mcount, i;
  అక్షరం errstr[STRERROR_LEN];
  CURLcode ఫలితం = CURLE_OK;
  size_t gso_కొలత;
  అక్షరం *sockbuf = ఖాళీ;
  uint8_t (*bufs)[సందేశం_BUF_కొలత] = ఖాళీ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ఫలితం = Curl_multi_xfer_sockbuf_borrow(దత్తాంశం, సందేశం_X_అంకె * సందేశం_BUF_కొలత,
                                          &sockbuf);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  bufs = (uint8_t (*)[సందేశం_BUF_కొలత])sockbuf;

  మొత్తం_nread = 0;
  అయితే(pkts < max_pkts) {
    పూర్ణాంకం n = (పూర్ణాంకం)CURLMIN(CURLMIN(సందేశం_X_అంకె, IOV_MAX), max_pkts);
    memset(&mmsg, 0, పరిమాణం(mmsg));
    కొరకు(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = పరిమాణం(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].సందేశం_iovlen = 1;
      mmsg[i].సందేశం_name = &remote_addr[i];
      mmsg[i].msg_namelen = పరిమాణం(remote_addr[i]);
      mmsg[i].సందేశం_control = &సందేశం_ctrl[i * CMSG_చొప్పున_సందేశం_కొలత];
      mmsg[i].msg_controllen = CMSG_చొప్పున_సందేశం_కొలత;
    }

#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
#అయితే_ముగింపు
    అయితే((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#ఒకవేళ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
    ఒకవేళ(mcount == -1) {
      ఒకవేళ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(దత్తాంశం, cf, "ingress, recvmsg_x -> EAGAIN");
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        నిర్మాణం ip_quadruple ip;
        ఒకవేళ(!Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, ఖాళీ, ఖాళీ, &ip))
          failf(దత్తాంశం, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_పోర్టు);
        ఫలితం = CURLE_COULDNT_CONNECT;
        వెళ్ళు బహిర్గతం;
      }
      curlx_strerror(SOCKERRNO, errstr, పరిమాణం(errstr));
      failf(దత్తాంశం, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    VERBOSE(++calls);
    కొరకు(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ఒకవేళ(!mmsg[i].సందేశం_datalen) {
        ++pkts;
        కొనసాగు;
      }
      మొత్తం_nread += mmsg[i].సందేశం_datalen;
      ecn = vquic_msghdr_get_ecn((నిర్మాణం msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_కొలత = mmsg[i].సందేశం_datalen;

      ఫలితం = recv_cb(bufs[i], mmsg[i].సందేశం_datalen, gso_కొలత,
                       mmsg[i].సందేశం_name,
                       mmsg[i].msg_namelen, ecn, userp);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      pkts += (mmsg[i].సందేశం_datalen + gso_కొలత - 1) / gso_కొలత;
    }
  }

బహిర్గతం:
  ఒకవేళ(మొత్తం_nread || ఫలితం)
    CURL_TRC_CF(దత్తాంశం, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                మొత్తం_nread, pkts, calls, (పూర్ణాంకం)ఫలితం);
  Curl_multi_xfer_sockbuf_release(దత్తాంశం, sockbuf);
  *pnread = మొత్తం_nread;
  తిరిగి ఫలితం;
}

#లేదావేళ defined(HAVE_SENDMSG)
స్థిర CURLcode recvmsg_packets(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, శూన్య *userp,
                                size_t *pnread)
{
#నిర్వచించు CMSG_చొప్పున_సందేశం_కొలత    CMSG_SPACE(పరిమాణం(పూర్ణాంకం))
  నిర్మాణం iovec msg_iov;
  నిర్మాణం msghdr సందేశం;
  uint8_t buf[64 * 1024];
  నిర్మాణం sockaddr_storage remote_addr;
  size_t మొత్తం_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  అక్షరం errstr[STRERROR_LEN];
  CURLcode ఫలితం = CURLE_OK;
  uint8_t సందేశం_ctrl[CMSG_చొప్పున_సందేశం_కొలత];
  size_t gso_కొలత;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  కొరకు(pkts = 0, మొత్తం_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&సందేశం, 0, పరిమాణం(సందేశం));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = పరిమాణం(buf);
    సందేశం.msg_iov = &msg_iov;
    సందేశం.సందేశం_iovlen = 1;
    సందేశం.సందేశం_control = సందేశం_ctrl;
    సందేశం.సందేశం_name = &remote_addr;
    సందేశం.msg_namelen = పరిమాణం(remote_addr);
    సందేశం.msg_controllen = CMSG_చొప్పున_సందేశం_కొలత;

    అయితే((rc = recvmsg(qctx->sockfd, &సందేశం, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ఒకవేళ(!curlx_sztouz(rc, &nread)) {
      ఒకవేళ(SOCK_EAGAIN(SOCKERRNO)) {
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        నిర్మాణం ip_quadruple ip;
        ఒకవేళ(!Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, ఖాళీ, ఖాళీ, &ip))
          failf(దత్తాంశం, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_పోర్టు);
        ఫలితం = CURLE_COULDNT_CONNECT;
        వెళ్ళు బహిర్గతం;
      }
      curlx_strerror(SOCKERRNO, errstr, పరిమాణం(errstr));
      failf(దత్తాంశం, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    మొత్తం_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ఒకవేళ(!nread) {
      ++pkts;
      కొనసాగు;
    }

    ecn = vquic_msghdr_get_ecn(&సందేశం, remote_addr.ss_family);
    gso_కొలత = vquic_msghdr_get_udp_gro(&సందేశం);
    ఒకవేళ(gso_కొలత == 0)
      gso_కొలత = nread;

    ఫలితం = recv_cb(buf, nread, gso_కొలత,
                     సందేశం.సందేశం_name, సందేశం.msg_namelen, ecn, userp);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    pkts += (nread + gso_కొలత - 1) / gso_కొలత;
  }

బహిర్గతం:
  ఒకవేళ(మొత్తం_nread || ఫలితం)
    CURL_TRC_CF(దత్తాంశం, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                మొత్తం_nread, pkts, calls, (పూర్ణాంకం)ఫలితం);
  *pnread = మొత్తం_nread;
  తిరిగి ఫలితం;
}

#లేకపోతే /* HAVE_SENDMMSG || HAVE_SENDMSG */
స్థిర CURLcode recvfrom_packets(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, శూన్య *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  పూర్ణాంకం bufsize = (పూర్ణాంకం)పరిమాణం(buf);
  నిర్మాణం sockaddr_storage remote_addr;
  socklen_t remote_addrlen = పరిమాణం(remote_addr);
  size_t మొత్తం_nread, pkts, calls = 0, nread;
  ssize_t rv;
  అక్షరం errstr[STRERROR_LEN];
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  కొరకు(pkts = 0, మొత్తం_nread = 0; pkts < max_pkts;) {
    అయితే((rv = recvfrom(qctx->sockfd, (అక్షరం *)buf, bufsize, 0,
                         (నిర్మాణం sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ఒకవేళ(!curlx_sztouz(rv, &nread)) {
      ఒకవేళ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(దత్తాంశం, cf, "ingress, recvfrom -> EAGAIN");
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        నిర్మాణం ip_quadruple ip;
        ఒకవేళ(!Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, ఖాళీ, ఖాళీ, &ip))
          failf(దత్తాంశం, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_పోర్టు);
        ఫలితం = CURLE_COULDNT_CONNECT;
        వెళ్ళు బహిర్గతం;
      }
      curlx_strerror(SOCKERRNO, errstr, పరిమాణం(errstr));
      failf(దత్తాంశం, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ఒకవేళ(!nread)
      కొనసాగు;

    మొత్తం_nread += nread;
    ఫలితం = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

బహిర్గతం:
  ఒకవేళ(మొత్తం_nread || ఫలితం)
    CURL_TRC_CF(దత్తాంశం, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                మొత్తం_nread, pkts, calls, (పూర్ణాంకం)ఫలితం);
  *pnread = మొత్తం_nread;
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, శూన్య *userp)
{
  CURLcode ఫలితం;
  size_t nread = 0;
#నిర్వచితమైతే HAVE_SENDMMSG
  ఫలితం = recvmmsg_packets(cf, దత్తాంశం, qctx, max_pkts, recv_cb, userp, &nread);
#లేదావేళ defined(HAVE_APPLE_సందేశం_X)
  ఫలితం = recvmsg_x_packets(cf, దత్తాంశం, qctx, max_pkts, recv_cb, userp,
                             &nread);
#లేదావేళ defined(HAVE_SENDMSG)
  ఫలితం = recvmsg_packets(cf, దత్తాంశం, qctx, max_pkts, recv_cb, userp, &nread);
#లేకపోతే
  ఫలితం = recvfrom_packets(cf, దత్తాంశం, qctx, max_pkts, recv_cb, userp, &nread);
#అయితే_ముగింపు
  ఒకవేళ(!ఫలితం) {
    /* EAGAIN is no error, only count actual packets as a reply */
    ఒకవేళ(!qctx->got_మొదటి_byte && nread) {
      qctx->got_మొదటి_byte = TRUE;
      qctx->మొదటి_byte_at = qctx->చివరి_op;
    }
    qctx->చివరి_io = qctx->చివరి_op;
  }
  తిరిగి ఫలితం;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(నిర్మాణం Curl_easy *దత్తాంశం,
                      గుర్తులేని అక్షరం *scid,
                      size_t scidlen,
                      పూర్ణాంకం *qlogfdp)
{
  అక్షరం *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  ఒకవేళ(qlog_dir) {
    నిర్మాణం dynbuf fname;
    CURLcode ఫలితం;
    గుర్తులేని పూర్ణాంకం i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    ఫలితం = curlx_dyn_కూడు(&fname, qlog_dir);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_కూడు(&fname, "/");
    కొరకు(i = 0; (i < scidlen) && !ఫలితం; i++) {
      అక్షరం hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      ఫలితం = curlx_dyn_కూడు(&fname, hex);
    }
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_కూడు(&fname, ".sqlog");

    ఒకవేళ(!ఫలితం) {
      పూర్ణాంకం qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ద్విమాన,
#నిర్వచితమైతే _WIN32
                              దత్తాంశం->సమితి.new_file_perms & (_S_IREAD | _S_IWRITE)
#లేకపోతే
                              (mode_t)దత్తాంశం->సమితి.new_file_perms
#అయితే_ముగింపు
                              );
      ఒకవేళ(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  తిరిగి CURLE_OK;
}

CURLcode Curl_cf_quic_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_peer *origin,
                                   నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం; /* not used in all cases and compilers are stupid */
#ఒకవేళ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ఫలితం = Curl_cf_ngtcp2_చొప్పించు_after(cf_at, origin, peer, &cf);
#లేదావేళ defined(USE_QUICHE)
  ఫలితం = Curl_cf_quiche_చొప్పించు_after(cf_at, origin, peer, &cf);
#లేకపోతే
  (శూన్య)cf)
  (శూన్య)cf_at;
  (శూన్య)origin;
  (శూన్య)peer;
  ఫలితం = CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు

#ఒకవేళ defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  ఒకవేళ(!ఫలితం && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    ఫలితం = Curl_conn_dns_కూడు_https_resolve(దత్తాంశం, cf->conn,
                                             cf->sockindex, origin);
  }
#అయితే_ముగింపు /* USE_HTTPSRR && USE_ECH */
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_quic_create(నిర్మాణం Curl_cfilter **pcf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_peer *origin,
                             నిర్మాణం Curl_peer *peer,
                             uint8_t transport_peer,
                             నిర్మాణం connectdata *conn,
                             నిర్మాణం Curl_sockaddr_ex *addr,
                             నిర్మాణం Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (శూన్య)transport_peer;
  (శూన్య)tunnel_transport;
  (శూన్య)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ఒకవేళ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  తిరిగి Curl_cf_ngtcp2_create(pcf, దత్తాంశం, origin, peer, conn, addr);
#లేదావేళ defined(USE_QUICHE)
  తిరిగి Curl_cf_quiche_create(pcf, దత్తాంశం, origin, peer, conn, addr);
#లేకపోతే
  *pcf = ఖాళీ;
  (శూన్య)దత్తాంశం;
  (శూన్య)origin;
  (శూన్య)peer;
  (శూన్య)conn;
  (శూన్య)addr;
  (శూన్య)tunnel_peer;
  (శూన్య)tunnel_transport;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

#ఒకవేళ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_peer *origin,
                                       నిర్మాణం Curl_peer *peer,
                                       నిర్మాణం Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#ఒకవేళ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  తిరిగి Curl_cf_ngtcp2_proxy_చొప్పించు_after(cf_at, దత్తాంశం, origin, peer,
                                           tunnel_peer, tunnel_transport);
#లేకపోతే
  (శూన్య)cf_at;
  (శూన్య)దత్తాంశం;
  (శూన్య)origin;
  (శూన్య)peer;
  (శూన్య)tunnel_peer;
  (శూన్య)tunnel_transport;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

CURLcode Curl_cf_h3_proxy_create(నిర్మాణం Curl_cfilter **pcf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_peer *origin,
                                 నిర్మాణం Curl_peer *peer,
                                 uint8_t transport_peer,
                                 నిర్మాణం connectdata *conn,
                                 నిర్మాణం Curl_sockaddr_ex *addr,
                                 నిర్మాణం Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ఒకవేళ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  తిరిగి Curl_cf_ngtcp2_proxy_create(pcf, దత్తాంశం, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#లేకపోతే
  *pcf = ఖాళీ;
  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
  (శూన్య)addr;
  (శూన్య)peer;
  (శూన్య)transport_peer;
  (శూన్య)tunnel_peer;
  (శూన్య)tunnel_transport;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn)
{
  ఒకవేళ(Curl_conn_get_మొదటి_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(దత్తాంశం, "HTTP/3 cannot be used over UNIX domain sockets");
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }
  ఒకవేళ(!(దత్తాంశం->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(దత్తాంశం, "HTTP/3 requested for non-HTTPS URL");
    తిరిగి CURLE_URL_MALFORMAT;
  }
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(conn->socks_proxy.peer) {
    failf(దత్తాంశం, "HTTP/3 is not supported over a SOCKS proxy");
    తిరిగి CURLE_URL_MALFORMAT;
  }
#లేకపోతే
  (శూన్య)conn;
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే CURLVERBOSE
స్థిరరాశి అక్షరం *Curl_vquic_h3_err_str(uint64_t error_సంకేతం)
{
  ఒకవేళ(error_సంకేతం <= UINT_MAX) {
    ఎంపిక((గుర్తులేని పూర్ణాంకం)error_సంకేతం) {
    స్థితి CURL_H3_ERR_NO_ERROR:
      తిరిగి "NO_ERROR";
    స్థితి CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      తిరిగి "GENERAL_PROTOCOL_ERROR";
    స్థితి CURL_H3_ERR_అంతర్గత_ERROR:
      తిరిగి "INTERNAL_ERROR";
    స్థితి CURL_H3_ERR_ప్రవాహం_CREATION_ERROR:
      తిరిగి "STREAM_CREATION_ERROR";
    స్థితి CURL_H3_ERR_CLOSED_CRITICAL_ప్రవాహం:
      తిరిగి "CLOSED_CRITICAL_STREAM";
    స్థితి CURL_H3_ERR_చట్రం_UNEXPECTED:
      తిరిగి "FRAME_UNEXPECTED";
    స్థితి CURL_H3_ERR_చట్రం_ERROR:
      తిరిగి "FRAME_ERROR";
    స్థితి CURL_H3_ERR_EXCESSIVE_ఎక్కించు:
      తిరిగి "EXCESSIVE_LOAD";
    స్థితి CURL_H3_ERR_ID_ERROR:
      తిరిగి "ID_ERROR";
    స్థితి CURL_H3_ERR_SETTINGS_ERROR:
      తిరిగి "SETTINGS_ERROR";
    స్థితి CURL_H3_ERR_MISSING_SETTINGS:
      తిరిగి "MISSING_SETTINGS";
    స్థితి CURL_H3_ERR_REQUEST_REJECTED:
      తిరిగి "REQUEST_REJECTED";
    స్థితి CURL_H3_ERR_REQUEST_CANCELLED:
      తిరిగి "REQUEST_CANCELLED";
    స్థితి CURL_H3_ERR_REQUEST_INCOMPLETE:
      తిరిగి "REQUEST_INCOMPLETE";
    స్థితి CURL_H3_ERR_సందేశం_ERROR:
      తిరిగి "MESSAGE_ERROR";
    స్థితి CURL_H3_ERR_CONNECT_ERROR:
      తిరిగి "CONNECT_ERROR";
    స్థితి CURL_H3_ERR_సంస్కరణ_FALLBACK:
      తిరిగి "VERSION_FALLBACK";
    అప్రమేయ:
      ఆపు;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  ఒకవేళ((error_సంకేతం >= 0x21) && !((error_సంకేతం - 0x21) % 0x1f))
    తిరిగి "NO_ERROR";
  తిరిగి "unknown";
}
#అయితే_ముగింపు /* CURLVERBOSE */

#ఒకవేళ defined(USE_NGTCP2) || defined(USE_NGHTTP3)

స్థిర శూన్య *vquic_ngtcp2_malloc(size_t కొలత, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  తిరిగి Curl_cmalloc(కొలత);
}

స్థిర శూన్య vquic_ngtcp2_free(శూన్య *ptr, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  Curl_cfree(ptr);
}

స్థిర శూన్య *vquic_ngtcp2_calloc(size_t nmemb, size_t కొలత, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  తిరిగి Curl_ccalloc(nmemb, కొలత);
}

స్థిర శూన్య *vquic_ngtcp2_realloc(శూన్య *ptr, size_t కొలత, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  తిరిగి Curl_crealloc(ptr, కొలత);
}

#నిర్వచితమైతే USE_NGTCP2
స్థిర నిర్మాణం ngtcp2_mem vquic_ngtcp2_mem = {
  ఖాళీ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
నిర్మాణం ngtcp2_mem *Curl_ngtcp2_mem(శూన్య)
{
  తిరిగి &vquic_ngtcp2_mem;
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NGHTTP3
స్థిర నిర్మాణం nghttp3_mem vquic_nghttp3_mem = {
  ఖాళీ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
నిర్మాణం nghttp3_mem *Curl_nghttp3_mem(శూన్య)
{
  తిరిగి &vquic_nghttp3_mem;
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_NGTCP2 || USE_NGHTTP3 */

#లేకపోతే /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
  DEBUGF(infof(దత్తాంశం, "QUIC is not supported in this build"));
  తిరిగి CURLE_NOT_BUILT_లోపల;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_HTTP3 */
