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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"

#ଅନ୍ତର୍ଭାଵ "curl_trc.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#ଅନ୍ତର୍ଭାଵ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#ଅନ୍ତର୍ଭାଵ <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#ଅନ୍ତର୍ଭାଵ <nghttp3/nghttp3.h>
#endif

#ଯଦି defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ଅନ୍ତର୍ଭାଵ <sys/syscall.h>
#ଯଦି defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_ସନ୍ଦେଶ_X
#endif
#endif

#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-ngtcp2.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-ngtcp2-cmn.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-ngtcp2-proxy.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-quiche.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic_int.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_ସନ୍ଦେଶ_X

/* this is `struct msghdr` with an additional field at the end */
ସଂରଚନା msghdr_x {
  ଶୂନ୍ୟ *ସନ୍ଦେଶ_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ସଂରଚନା iovec *msg_iov;    /* scatter/gather array */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_iovlen;           /* # elements in msg_iov */
  ଶୂନ୍ୟ *ସନ୍ଦେଶ_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_flags;            /* flags on received message */
  size_t ସନ୍ଦେଶ_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_ସନ୍ଦେଶ_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#ନଚେତଯଦି defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#ଅନ୍ୟଥା
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_vquic_init(ଶୂନ୍ୟ)
{
#ଯଦି defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  ଯଦି(ngtcp2_crypto_ossl_init())
    ଫେରନ୍ତୁ 0;
#endif

  ଫେରନ୍ତୁ 1;
}

ଶୂନ୍ୟ Curl_vquic_cleanup(ଶୂନ୍ୟ)
{
#ଯଦି defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_ଅଙ୍କ >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

ଶୂନ୍ୟ Curl_quic_ver(ଅକ୍ଷର *p, size_t len)
{
#ଯଦି defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#ନଚେତଯଦି defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#ଯଦି defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#ନଚେତଯଦି defined(HAVE_APPLE_ସନ୍ଦେଶ_X)
  qctx->no_gso = FALSE;
#ଅନ୍ୟଥା
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_DBG_QUIC_WBLOCK");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, 100))
        qctx->wblock_percent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_vquic_ctx_free(ସଂରଚନା cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

ଶୂନ୍ୟ Curl_vquic_ctx_set_time(ସଂରଚନା cf_quic_ctx *qctx,
                             ଅଚଳ ସଂରଚନା curltime *pnow)
{
  qctx->last_op = *pnow;
}

ଶୂନ୍ୟ Curl_vquic_ctx_update_time(ସଂରଚନା cf_quic_ctx *qctx,
                                ଅଚଳ ସଂରଚନା curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_ସନ୍ଦେଶ_X
ସ୍ଥିର CURLcode do_sendmsg(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା cf_quic_ctx *qctx,
                           ଅଚଳ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define ସନ୍ଦେଶ_X_SNUM  64
  ସଂରଚନା iovec msg_iov[ସନ୍ଦେଶ_X_SNUM];
  ସଂରଚନା msghdr_x mmsg[ସନ୍ଦେଶ_X_SNUM];
  ଅକ୍ଷର errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  ଯଦି(!pktlen)
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  ଯେତେବେଳେ(i < n) {
    size_t j, batch = CURLMIN(n - i, ସନ୍ଦେଶ_X_SNUM), pkts_sent = 0;

    ପାଇଁ(j = 0; j < batch; ++j) {
      ଅଚଳ size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, ଆକାର(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].ସନ୍ଦେଶ_iovlen = 1;
      mmsg[j].ସନ୍ଦେଶ_datalen = msg_iov[j].iov_len;
    }

#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ଯେତେବେଳେ((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic କାଢ଼
#endif

    ଯଦି(rc < 0) {
      ଯଦି(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        ଯାଅ out;
      }
      ଯଦି(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, ଆକାର(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        ଯାଅ out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    ଅନ୍ୟଥା
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    ଯଦି(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      ଯାଅ out;
    }
    i += pkts_sent; /* Some have been sent */
    ପାଇଁ(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    ଯଦି(pkts_sent < batch)
      ଯାଅ out; /* but not all of them */
  }

out:
  *psent = sent;
  ଯଦି(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

#ଅନ୍ୟଥା /* HAVE_APPLE_MSG_X */

ସ୍ଥିର CURLcode send_packet_no_gso(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା cf_quic_ctx *qctx,
                                   ଅଚଳ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ସ୍ଥିର CURLcode do_sendmsg(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା cf_quic_ctx *qctx,
                           ଅଚଳ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  ସଂରଚନା iovec msg_iov;
  ସଂରଚନା msghdr ସନ୍ଦେଶ = { 0 };
  ssize_t rv;
#ଯଦି defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t ସନ୍ଦେଶ_ctrl[32];
  ସଂରଚନା cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  ସନ୍ଦେଶ.msg_iov = &msg_iov;
  ସନ୍ଦେଶ.ସନ୍ଦେଶ_iovlen = 1;

#ଯଦି defined(__linux__) && defined(UDP_SEGMENT)
  ଯଦି(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(ସନ୍ଦେଶ_ctrl, 0, ଆକାର(ସନ୍ଦେଶ_ctrl));
    ସନ୍ଦେଶ.ସନ୍ଦେଶ_control = ସନ୍ଦେଶ_ctrl;
    DEBUGASSERT(ଆକାର(ସନ୍ଦେଶ_ctrl) >= CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)));
    ସନ୍ଦେଶ.msg_controllen = CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
    cm = CMSG_FIRSTHDR(&ସନ୍ଦେଶ);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(ଆକାର(uint16_t));
    *(uint16_t *)(ଶୂନ୍ୟ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  ଯେତେବେଳେ((rv = sendmsg(qctx->sockfd, &ସନ୍ଦେଶ, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  ଯଦି(!curlx_sztouz(rv, psent)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
    ଯଦି(SOCK_EAGAIN(sockerr))
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ବାଛ(sockerr) {
    ସ୍ଥିତି SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି EIO:
      ଯଦି(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ଫେରନ୍ତୁ send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା ଯଦି(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }
#ଅନ୍ୟଥା
  ssize_t rv;
  (ଶୂନ୍ୟ)gsolen;

  *psent = 0;

  ଯେତେବେଳେ((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  ଯଦି(!curlx_sztouz(rv, psent)) {
    ଯଦି(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      ଯଦି(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        ଯାଅ out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (ଶୂନ୍ୟ)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *psent);
  ଫେରନ୍ତୁ result;
}

#endif /* !HAVE_APPLE_MSG_X */

ସ୍ଥିର CURLcode send_packet_no_gso(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା cf_quic_ctx *qctx,
                                   ଅଚଳ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ଅଚଳ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  ପାଇଁ(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    ଯଦି(result)
      ଯାଅ out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *psent);
  ଫେରନ୍ତୁ result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ସ୍ଥିର CURLcode send_packet_no_gso_cf(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ଅଚଳ uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ଅଚଳ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  ପାଇଁ(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    ଯଦି(result)
      ଯାଅ out;

    ଯଦି(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *psent);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode vquic_send_packets(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା cf_quic_ctx *qctx,
                                   ଅଚଳ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  ଯଦି(qctx->wblock_percent > 0) {
    unsigned ଅକ୍ଷର c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    ଯଦି(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
  }
#endif
  ଯଦି(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  ଅନ୍ୟଥା {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  ଯଦି(!result)
    qctx->last_io = qctx->last_op;
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_vquic_flush(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା cf_quic_ctx *qctx)
{
  ଅଚଳ unsigned ଅକ୍ଷର *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  ଯେତେବେଳେ(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    ଯଦି(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      ଯଦି(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    ଯଦି(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    ଅନ୍ୟଥା {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      ଯଦି(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      ଅନ୍ୟଥା {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    ଯଦି(result) {
      ଯଦି(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        ଯଦି(qctx->split_len)
          qctx->split_len -= sent;
      }
      ଫେରନ୍ତୁ result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    ଯଦି(qctx->split_len)
      qctx->split_len -= sent;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_vquic_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ଫେରନ୍ତୁ Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  ଫେରନ୍ତୁ Curl_vquic_flush(cf, data, qctx);
}

#ଯଦି (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_ସନ୍ଦେଶ_X)
ସ୍ଥିର size_t vquic_msghdr_get_udp_gro(ସଂରଚନା msghdr *ସନ୍ଦେଶ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା gso_size = 0;
#ଯଦି defined(__linux__) && defined(UDP_GRO)
  ସଂରଚନା cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#ଯଦି defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  ପାଇଁ(cmsg = CMSG_FIRSTHDR(ସନ୍ଦେଶ); cmsg; cmsg = CMSG_NXTHDR(ସନ୍ଦେଶ, cmsg)) {
#ଯଦି defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic କାଢ଼
#endif
    ଯଦି(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), ଆକାର(gso_size));

      ଭାଙ୍ଗ;
    }
  }
#endif /* linux && UDP_GRO */
  (ଶୂନ୍ୟ)ସନ୍ଦେଶ;

  ଫେରନ୍ତୁ (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#ଯଦି (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_ସନ୍ଦେଶ_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
ସ୍ଥିର uint8_t vquic_msghdr_get_ecn(ସଂରଚନା msghdr *ସନ୍ଦେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା family)
{
  ସଂରଚନା cmsghdr *cmsg;
  ବାଛ(family) {
  ସ୍ଥିତି AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#ଯଦି defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    ପାଇଁ(cmsg = CMSG_FIRSTHDR(ସନ୍ଦେଶ); cmsg; cmsg = CMSG_NXTHDR(ସନ୍ଦେଶ, cmsg)) {
#ଯଦି defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic କାଢ଼
#endif
      ଯଦି(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#ଅନ୍ୟଥା
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        ଫେରନ୍ତୁ *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#ଯଦି defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    ପାଇଁ(cmsg = CMSG_FIRSTHDR(ସନ୍ଦେଶ); cmsg; cmsg = CMSG_NXTHDR(ସନ୍ଦେଶ, cmsg)) {
#ଯଦି defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic କାଢ଼
#endif
      ଯଦି(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା tos;

        memcpy(&tos, CMSG_DATA(cmsg), ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

        ଫେରନ୍ତୁ (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ 0;
}
#ଅନ୍ୟଥା
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

ସ୍ଥିର CURLcode recvmmsg_packets(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ଶୂନ୍ୟ *userp,
                                 size_t *pnread)
{
#ଯଦି defined(__linux__) && defined(UDP_GRO)
#define MMSG_ଅଙ୍କ  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_ସନ୍ଦେଶ_SIZE    (2 * CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)))
#ଅନ୍ୟଥା
#define MMSG_ଅଙ୍କ  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_ସନ୍ଦେଶ_SIZE    CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))
#endif
#define ସନ୍ଦେଶ_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  ସଂରଚନା iovec msg_iov[MMSG_ଅଙ୍କ];
  ସଂରଚନା mmsghdr mmsg[MMSG_ଅଙ୍କ];
  uint8_t ସନ୍ଦେଶ_ctrl[MMSG_ଅଙ୍କ * CMSG_PER_ସନ୍ଦେଶ_SIZE];
  ସଂରଚନା sockaddr_storage remote_addr[MMSG_ଅଙ୍କ];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mcount, i, n;
  ଅକ୍ଷର errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  ଅକ୍ଷର *sockbuf = ଖାଲି;
  uint8_t (*bufs)[ସନ୍ଦେଶ_BUF_SIZE] = ଖାଲି;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_ଅଙ୍କ * ସନ୍ଦେଶ_BUF_SIZE,
                                          &sockbuf);
  ଯଦି(result)
    ଯାଅ out;
  bufs = (uint8_t (*)[ସନ୍ଦେଶ_BUF_SIZE])sockbuf;

  total_nread = 0;
  ଯେତେବେଳେ(pkts < max_pkts) {
    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURLMIN(CURLMIN(MMSG_ଅଙ୍କ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ଆକାର(mmsg));
    ପାଇଁ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ଆକାର(bufs[i]);
      mmsg[i].ସନ୍ଦେଶ_hdr.msg_iov = &msg_iov[i];
      mmsg[i].ସନ୍ଦେଶ_hdr.ସନ୍ଦେଶ_iovlen = 1;
      mmsg[i].ସନ୍ଦେଶ_hdr.ସନ୍ଦେଶ_name = &remote_addr[i];
      mmsg[i].ସନ୍ଦେଶ_hdr.msg_namelen = ଆକାର(remote_addr[i]);
      mmsg[i].ସନ୍ଦେଶ_hdr.ସନ୍ଦେଶ_control = &ସନ୍ଦେଶ_ctrl[i * CMSG_PER_ସନ୍ଦେଶ_SIZE];
      mmsg[i].ସନ୍ଦେଶ_hdr.msg_controllen = CMSG_PER_ସନ୍ଦେଶ_SIZE;
    }

    ଯେତେବେଳେ((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ଖାଲି)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ଯଦି(mcount == -1) {
      ଯଦି(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        ଯାଅ out;
      }
      ଯଦି(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ସଂରଚନା ip_quadruple ip;
        ଯଦି(!Curl_cf_socket_peek(cf->next, data, ଖାଲି, ଖାଲି, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ଯାଅ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ଆକାର(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ଯାଅ out;
    }

    VERBOSE(++calls);
    ପାଇଁ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ଯଦି(!mmsg[i].ସନ୍ଦେଶ_len) {
        ++pkts;
        ଜାରି;
      }
      total_nread += mmsg[i].ସନ୍ଦେଶ_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].ସନ୍ଦେଶ_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].ସନ୍ଦେଶ_hdr);
      ଯଦି(gso_size == 0)
        gso_size = mmsg[i].ସନ୍ଦେଶ_len;

      result = recv_cb(bufs[i], mmsg[i].ସନ୍ଦେଶ_len, gso_size,
                       mmsg[i].ସନ୍ଦେଶ_hdr.ସନ୍ଦେଶ_name,
                       mmsg[i].ସନ୍ଦେଶ_hdr.msg_namelen, ecn, userp);
      ଯଦି(result)
        ଯାଅ out;
      pkts += (mmsg[i].ସନ୍ଦେଶ_len + gso_size - 1) / gso_size;
    }
  }

out:
  ଯଦି(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ଫେରନ୍ତୁ result;
}

#ନଚେତଯଦି defined(HAVE_APPLE_ସନ୍ଦେଶ_X)

ସ୍ଥିର CURLcode recvmsg_x_packets(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  ଶୂନ୍ୟ *userp, size_t *pnread)
{
#define ସନ୍ଦେଶ_X_ଅଙ୍କ  64
#define ସନ୍ଦେଶ_BUF_SIZE  (2048)
#define CMSG_PER_ସନ୍ଦେଶ_SIZE     CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))
  ସଂରଚନା iovec msg_iov[ସନ୍ଦେଶ_X_ଅଙ୍କ];
  ସଂରଚନା msghdr_x mmsg[ସନ୍ଦେଶ_X_ଅଙ୍କ];
  uint8_t ସନ୍ଦେଶ_ctrl[ସନ୍ଦେଶ_X_ଅଙ୍କ * CMSG_PER_ସନ୍ଦେଶ_SIZE];
  ସଂରଚନା sockaddr_storage remote_addr[ସନ୍ଦେଶ_X_ଅଙ୍କ];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mcount, i;
  ଅକ୍ଷର errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  ଅକ୍ଷର *sockbuf = ଖାଲି;
  uint8_t (*bufs)[ସନ୍ଦେଶ_BUF_SIZE] = ଖାଲି;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, ସନ୍ଦେଶ_X_ଅଙ୍କ * ସନ୍ଦେଶ_BUF_SIZE,
                                          &sockbuf);
  ଯଦି(result)
    ଯାଅ out;
  bufs = (uint8_t (*)[ସନ୍ଦେଶ_BUF_SIZE])sockbuf;

  total_nread = 0;
  ଯେତେବେଳେ(pkts < max_pkts) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURLMIN(CURLMIN(ସନ୍ଦେଶ_X_ଅଙ୍କ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ଆକାର(mmsg));
    ପାଇଁ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ଆକାର(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].ସନ୍ଦେଶ_iovlen = 1;
      mmsg[i].ସନ୍ଦେଶ_name = &remote_addr[i];
      mmsg[i].msg_namelen = ଆକାର(remote_addr[i]);
      mmsg[i].ସନ୍ଦେଶ_control = &ସନ୍ଦେଶ_ctrl[i * CMSG_PER_ସନ୍ଦେଶ_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_ସନ୍ଦେଶ_SIZE;
    }

#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ଯେତେବେଳେ((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#ଯଦି defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic କାଢ଼
#endif
    ଯଦି(mcount == -1) {
      ଯଦି(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        ଯାଅ out;
      }
      ଯଦି(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ସଂରଚନା ip_quadruple ip;
        ଯଦି(!Curl_cf_socket_peek(cf->next, data, ଖାଲି, ଖାଲି, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ଯାଅ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ଆକାର(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ଯାଅ out;
    }

    VERBOSE(++calls);
    ପାଇଁ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ଯଦି(!mmsg[i].ସନ୍ଦେଶ_datalen) {
        ++pkts;
        ଜାରି;
      }
      total_nread += mmsg[i].ସନ୍ଦେଶ_datalen;
      ecn = vquic_msghdr_get_ecn((ସଂରଚନା msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].ସନ୍ଦେଶ_datalen;

      result = recv_cb(bufs[i], mmsg[i].ସନ୍ଦେଶ_datalen, gso_size,
                       mmsg[i].ସନ୍ଦେଶ_name,
                       mmsg[i].msg_namelen, ecn, userp);
      ଯଦି(result)
        ଯାଅ out;
      pkts += (mmsg[i].ସନ୍ଦେଶ_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  ଯଦି(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ଫେରନ୍ତୁ result;
}

#ନଚେତଯଦି defined(HAVE_SENDMSG)
ସ୍ଥିର CURLcode recvmsg_packets(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, ଶୂନ୍ୟ *userp,
                                size_t *pnread)
{
#define CMSG_PER_ସନ୍ଦେଶ_SIZE    CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))
  ସଂରଚନା iovec msg_iov;
  ସଂରଚନା msghdr ସନ୍ଦେଶ;
  uint8_t buf[64 * 1024];
  ସଂରଚନା sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  ଅକ୍ଷର errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t ସନ୍ଦେଶ_ctrl[CMSG_PER_ସନ୍ଦେଶ_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ପାଇଁ(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&ସନ୍ଦେଶ, 0, ଆକାର(ସନ୍ଦେଶ));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = ଆକାର(buf);
    ସନ୍ଦେଶ.msg_iov = &msg_iov;
    ସନ୍ଦେଶ.ସନ୍ଦେଶ_iovlen = 1;
    ସନ୍ଦେଶ.ସନ୍ଦେଶ_control = ସନ୍ଦେଶ_ctrl;
    ସନ୍ଦେଶ.ସନ୍ଦେଶ_name = &remote_addr;
    ସନ୍ଦେଶ.msg_namelen = ଆକାର(remote_addr);
    ସନ୍ଦେଶ.msg_controllen = CMSG_PER_ସନ୍ଦେଶ_SIZE;

    ଯେତେବେଳେ((rc = recvmsg(qctx->sockfd, &ସନ୍ଦେଶ, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ଯଦି(!curlx_sztouz(rc, &nread)) {
      ଯଦି(SOCK_EAGAIN(SOCKERRNO)) {
        ଯାଅ out;
      }
      ଯଦି(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ସଂରଚନା ip_quadruple ip;
        ଯଦି(!Curl_cf_socket_peek(cf->next, data, ଖାଲି, ଖାଲି, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ଯାଅ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ଆକାର(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ଯାଅ out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ଯଦି(!nread) {
      ++pkts;
      ଜାରି;
    }

    ecn = vquic_msghdr_get_ecn(&ସନ୍ଦେଶ, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&ସନ୍ଦେଶ);
    ଯଦି(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     ସନ୍ଦେଶ.ସନ୍ଦେଶ_name, ସନ୍ଦେଶ.msg_namelen, ecn, userp);
    ଯଦି(result)
      ଯାଅ out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  ଯଦି(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  *pnread = total_nread;
  ଫେରନ୍ତୁ result;
}

#ଅନ୍ୟଥା /* HAVE_SENDMMSG || HAVE_SENDMSG */
ସ୍ଥିର CURLcode recvfrom_packets(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ଶୂନ୍ୟ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf);
  ସଂରଚନା sockaddr_storage remote_addr;
  socklen_t remote_addrlen = ଆକାର(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  ଅକ୍ଷର errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  ପାଇଁ(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    ଯେତେବେଳେ((rv = recvfrom(qctx->sockfd, (ଅକ୍ଷର *)buf, bufsize, 0,
                         (ସଂରଚନା sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ଯଦି(!curlx_sztouz(rv, &nread)) {
      ଯଦି(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        ଯାଅ out;
      }
      ଯଦି(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ସଂରଚନା ip_quadruple ip;
        ଯଦି(!Curl_cf_socket_peek(cf->next, data, ଖାଲି, ଖାଲି, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ଯାଅ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ଆକାର(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ଯାଅ out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ଯଦି(!nread)
      ଜାରି;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    ଯଦି(result)
      ଯାଅ out;
  }

out:
  ଯଦି(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  *pnread = total_nread;
  ଫେରନ୍ତୁ result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ଶୂନ୍ୟ *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#ନଚେତଯଦି defined(HAVE_APPLE_ସନ୍ଦେଶ_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#ନଚେତଯଦି defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#ଅନ୍ୟଥା
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  ଯଦି(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    ଯଦି(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  ଫେରନ୍ତୁ result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ସଂରଚନା Curl_easy *data,
                      unsigned ଅକ୍ଷର *scid,
                      size_t scidlen,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା *qlogfdp)
{
  ଅକ୍ଷର *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  ଯଦି(qlog_dir) {
    ସଂରଚନା dynbuf fname;
    CURLcode result;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    ଯଦି(!result)
      result = curlx_dyn_add(&fname, "/");
    ପାଇଁ(i = 0; (i < scidlen) && !result; i++) {
      ଅକ୍ଷର hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    ଯଦି(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    ଯଦି(!result) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ଦ୍ୱିମିକ,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#ଅନ୍ୟଥା
                              (mode_t)data->set.new_file_perms
#endif
                              );
      ଯଦି(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cf_quic_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_peer *origin,
                                   ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)data; /* not used in all cases and compilers are stupid */
#ଯଦି defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_ଭର୍ତ୍ତି_after(cf_at, origin, peer, &cf);
#ନଚେତଯଦି defined(USE_QUICHE)
  result = Curl_cf_quiche_ଭର୍ତ୍ତି_after(cf_at, origin, peer, &cf);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf)
  (ଶୂନ୍ୟ)cf_at;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#ଯଦି defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  ଯଦି(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_quic_create(ସଂରଚନା Curl_cfilter **pcf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_peer *origin,
                             ସଂରଚନା Curl_peer *peer,
                             uint8_t transport_peer,
                             ସଂରଚନା connectdata *conn,
                             ସଂରଚନା Curl_sockaddr_ex *addr,
                             ସଂରଚନା Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (ଶୂନ୍ୟ)transport_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  (ଶୂନ୍ୟ)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ଯଦି defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ଫେରନ୍ତୁ Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#ନଚେତଯଦି defined(USE_QUICHE)
  ଫେରନ୍ତୁ Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#ଅନ୍ୟଥା
  *pcf = ଖାଲି;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)peer;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)addr;
  (ଶୂନ୍ୟ)tunnel_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

#ଯଦି !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_peer *origin,
                                       ସଂରଚନା Curl_peer *peer,
                                       ସଂରଚନା Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#ଯଦି defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ଫେରନ୍ତୁ Curl_cf_ngtcp2_proxy_ଭର୍ତ୍ତି_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf_at;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)origin;
  (ଶୂନ୍ୟ)peer;
  (ଶୂନ୍ୟ)tunnel_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(ସଂରଚନା Curl_cfilter **pcf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_peer *origin,
                                 ସଂରଚନା Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ସଂରଚନା connectdata *conn,
                                 ସଂରଚନା Curl_sockaddr_ex *addr,
                                 ସଂରଚନା Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ଯଦି defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ଫେରନ୍ତୁ Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#ଅନ୍ୟଥା
  *pcf = ଖାଲି;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)addr;
  (ଶୂନ୍ୟ)peer;
  (ଶୂନ୍ୟ)transport_peer;
  (ଶୂନ୍ୟ)tunnel_peer;
  (ଶୂନ୍ୟ)tunnel_transport;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn)
{
  ଯଦି(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }
  ଯଦି(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  ଯଦି(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    ଫେରନ୍ତୁ CURLE_URL_MALFORMAT;
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)conn;
#endif

  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef CURLVERBOSE
ଅଚଳ ଅକ୍ଷର *Curl_vquic_h3_err_str(uint64_t error_code)
{
  ଯଦି(error_code <= UINT_MAX) {
    ବାଛ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)error_code) {
    ସ୍ଥିତି CURL_H3_ERR_NO_ERROR:
      ଫେରନ୍ତୁ "NO_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ଫେରନ୍ତୁ "GENERAL_PROTOCOL_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_INTERNAL_ERROR:
      ଫେରନ୍ତୁ "INTERNAL_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_STREAM_CREATION_ERROR:
      ଫେରନ୍ତୁ "STREAM_CREATION_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      ଫେରନ୍ତୁ "CLOSED_CRITICAL_STREAM";
    ସ୍ଥିତି CURL_H3_ERR_FRAME_UNEXPECTED:
      ଫେରନ୍ତୁ "FRAME_UNEXPECTED";
    ସ୍ଥିତି CURL_H3_ERR_FRAME_ERROR:
      ଫେରନ୍ତୁ "FRAME_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_EXCESSIVE_LOAD:
      ଫେରନ୍ତୁ "EXCESSIVE_LOAD";
    ସ୍ଥିତି CURL_H3_ERR_ID_ERROR:
      ଫେରନ୍ତୁ "ID_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_SETTINGS_ERROR:
      ଫେରନ୍ତୁ "SETTINGS_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_MISSING_SETTINGS:
      ଫେରନ୍ତୁ "MISSING_SETTINGS";
    ସ୍ଥିତି CURL_H3_ERR_REQUEST_REJECTED:
      ଫେରନ୍ତୁ "REQUEST_REJECTED";
    ସ୍ଥିତି CURL_H3_ERR_REQUEST_CANCELLED:
      ଫେରନ୍ତୁ "REQUEST_CANCELLED";
    ସ୍ଥିତି CURL_H3_ERR_REQUEST_INCOMPLETE:
      ଫେରନ୍ତୁ "REQUEST_INCOMPLETE";
    ସ୍ଥିତି CURL_H3_ERR_MESSAGE_ERROR:
      ଫେରନ୍ତୁ "MESSAGE_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_CONNECT_ERROR:
      ଫେରନ୍ତୁ "CONNECT_ERROR";
    ସ୍ଥିତି CURL_H3_ERR_VERSION_FALLBACK:
      ଫେରନ୍ତୁ "VERSION_FALLBACK";
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  ଯଦି((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    ଫେରନ୍ତୁ "NO_ERROR";
  ଫେରନ୍ତୁ "unknown";
}
#endif /* CURLVERBOSE */

#ଯଦି defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ସ୍ଥିର ଶୂନ୍ୟ *vquic_ngtcp2_malloc(size_t size, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଫେରନ୍ତୁ Curl_cmalloc(size);
}

ସ୍ଥିର ଶୂନ୍ୟ vquic_ngtcp2_free(ଶୂନ୍ୟ *ptr, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  Curl_cfree(ptr);
}

ସ୍ଥିର ଶୂନ୍ୟ *vquic_ngtcp2_calloc(size_t nmemb, size_t size, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଫେରନ୍ତୁ Curl_ccalloc(nmemb, size);
}

ସ୍ଥିର ଶୂନ୍ୟ *vquic_ngtcp2_realloc(ଶୂନ୍ୟ *ptr, size_t size, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଫେରନ୍ତୁ Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
ସ୍ଥିର ସଂରଚନା ngtcp2_mem vquic_ngtcp2_mem = {
  ଖାଲି,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ସଂରଚନା ngtcp2_mem *Curl_ngtcp2_mem(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
ସ୍ଥିର ସଂରଚନା nghttp3_mem vquic_nghttp3_mem = {
  ଖାଲି,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ସଂରଚନା nghttp3_mem *Curl_nghttp3_mem(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#ଅନ୍ୟଥା /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
