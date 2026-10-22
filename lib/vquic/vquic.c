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
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"

#ಅನ್ತರ್ಭಾವ "curl_trc.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#ಅನ್ತರ್ಭಾವ <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>
#endif

#ಯದಿ defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ಅನ್ತರ್ಭಾವ <sys/syscall.h>
#ಯದಿ defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_ಸಂದೇಶ_X
#endif
#endif

#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2-cmn.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2-proxy.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-quiche.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic_int.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_ಸಂದೇಶ_X

/* this is `struct msghdr` with an additional field at the end */
ಸಂರಚನಾ msghdr_x {
  ಶೂನ್ಯ *ಸಂದೇಶ_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ಸಂರಚನಾ iovec *msg_iov;    /* scatter/gather array */
  ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_iovlen;           /* # elements in msg_iov */
  ಶೂನ್ಯ *ಸಂದೇಶ_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_flags;            /* flags on received message */
  size_t ಸಂದೇಶ_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_ಸಂದೇಶ_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#ನೋಚೇದ್ಯದಿ defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#ಅನ್ಯಥಾ
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

ಪೂರ್ಣಾಙ್ಕ Curl_vquic_init(ಶೂನ್ಯ)
{
#ಯದಿ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  ಯದಿ(ngtcp2_crypto_ossl_init())
    ಪ್ರತ್ಯಾಗಮ 0;
#endif

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ Curl_vquic_cleanup(ಶೂನ್ಯ)
{
#ಯದಿ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_ಅಂಕೆ >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

ಶೂನ್ಯ Curl_quic_ver(ವರ್ಣ *p, size_t len)
{
#ಯದಿ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#ನೋಚೇದ್ಯದಿ defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#ಯದಿ defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#ನೋಚೇದ್ಯದಿ defined(HAVE_APPLE_ಸಂದೇಶ_X)
  qctx->no_gso = FALSE;
#ಅನ್ಯಥಾ
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_DBG_QUIC_WBLOCK");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        qctx->wblock_percent = (ಪೂರ್ಣಾಙ್ಕ)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_vquic_ctx_free(ಸಂರಚನಾ cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

ಶೂನ್ಯ Curl_vquic_ctx_set_time(ಸಂರಚನಾ cf_quic_ctx *qctx,
                             ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  qctx->last_op = *pnow;
}

ಶೂನ್ಯ Curl_vquic_ctx_update_time(ಸಂರಚನಾ cf_quic_ctx *qctx,
                                ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_ಸಂದೇಶ_X
ಸ್ಥಿರ CURLcode do_sendmsg(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ cf_quic_ctx *qctx,
                           ಧ್ರುವ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define ಸಂದೇಶ_X_SNUM  64
  ಸಂರಚನಾ iovec msg_iov[ಸಂದೇಶ_X_SNUM];
  ಸಂರಚನಾ msghdr_x mmsg[ಸಂದೇಶ_X_SNUM];
  ವರ್ಣ errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  ಪೂರ್ಣಾಙ್ಕ rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  ಯದಿ(!pktlen)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  ಯಾವತ್(i < n) {
    size_t j, batch = CURLMIN(n - i, ಸಂದೇಶ_X_SNUM), pkts_sent = 0;

    ಪ್ರತಿ(j = 0; j < batch; ++j) {
      ಧ್ರುವ size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, ಪರಿಮಾಣ(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].ಸಂದೇಶ_iovlen = 1;
      mmsg[j].ಸಂದೇಶ_datalen = msg_iov[j].iov_len;
    }

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ಯಾವತ್((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ತೆಗೆ
#endif

    ಯದಿ(rc < 0) {
      ಯದಿ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        ಗಚ್ಛ out;
      }
      ಯದಿ(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, ಪರಿಮಾಣ(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        ಗಚ್ಛ out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    ಅನ್ಯಥಾ
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    ಯದಿ(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      ಗಚ್ಛ out;
    }
    i += pkts_sent; /* Some have been sent */
    ಪ್ರತಿ(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    ಯದಿ(pkts_sent < batch)
      ಗಚ್ಛ out; /* but not all of them */
  }

out:
  *psent = sent;
  ಯದಿ(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ಅನ್ಯಥಾ /* HAVE_APPLE_MSG_X */

ಸ್ಥಿರ CURLcode send_packet_no_gso(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ cf_quic_ctx *qctx,
                                   ಧ್ರುವ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ಸ್ಥಿರ CURLcode do_sendmsg(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ cf_quic_ctx *qctx,
                           ಧ್ರುವ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  ಸಂರಚನಾ iovec msg_iov;
  ಸಂರಚನಾ msghdr ಸಂದೇಶ = { 0 };
  ssize_t rv;
#ಯದಿ defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t ಸಂದೇಶ_ctrl[32];
  ಸಂರಚನಾ cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  ಸಂದೇಶ.msg_iov = &msg_iov;
  ಸಂದೇಶ.ಸಂದೇಶ_iovlen = 1;

#ಯದಿ defined(__linux__) && defined(UDP_SEGMENT)
  ಯದಿ(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(ಸಂದೇಶ_ctrl, 0, ಪರಿಮಾಣ(ಸಂದೇಶ_ctrl));
    ಸಂದೇಶ.ಸಂದೇಶ_control = ಸಂದೇಶ_ctrl;
    DEBUGASSERT(ಪರಿಮಾಣ(ಸಂದೇಶ_ctrl) >= CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)));
    ಸಂದೇಶ.msg_controllen = CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
    cm = CMSG_FIRSTHDR(&ಸಂದೇಶ);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(ಪರಿಮಾಣ(uint16_t));
    *(uint16_t *)(ಶೂನ್ಯ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  ಯಾವತ್((rv = sendmsg(qctx->sockfd, &ಸಂದೇಶ, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  ಯದಿ(!curlx_sztouz(rv, psent)) {
    ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
    ಯದಿ(SOCK_EAGAIN(sockerr))
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ವಿಕಲ್ಪ(sockerr) {
    ಪಕ್ಷ SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      ಭಙ್ಗ;
    ಪಕ್ಷ EIO:
      ಯದಿ(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ಪ್ರತ್ಯಾಗಮ send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    ಸ್ವಭಾವ:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
#ಅನ್ಯಥಾ
  ssize_t rv;
  (ಶೂನ್ಯ)gsolen;

  *psent = 0;

  ಯಾವತ್((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  ಯದಿ(!curlx_sztouz(rv, psent)) {
    ಯದಿ(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        ಗಚ್ಛ out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (ಶೂನ್ಯ)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (ಪೂರ್ಣಾಙ್ಕ)result, *psent);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !HAVE_APPLE_MSG_X */

ಸ್ಥಿರ CURLcode send_packet_no_gso(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ cf_quic_ctx *qctx,
                                   ಧ್ರುವ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ಧ್ರುವ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  ಪ್ರತಿ(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (ಪೂರ್ಣಾಙ್ಕ)result, *psent);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ಸ್ಥಿರ CURLcode send_packet_no_gso_cf(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಧ್ರುವ uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ಧ್ರುವ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  ಪ್ರತಿ(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    ಯದಿ(result)
      ಗಚ್ಛ out;

    ಯದಿ(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (ಪೂರ್ಣಾಙ್ಕ)result, *psent);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode vquic_send_packets(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ cf_quic_ctx *qctx,
                                   ಧ್ರುವ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  ಯದಿ(qctx->wblock_percent > 0) {
    unsigned ವರ್ಣ c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    ಯದಿ(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
  }
#endif
  ಯದಿ(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  ಅನ್ಯಥಾ {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  ಯದಿ(!result)
    qctx->last_io = qctx->last_op;
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_vquic_flush(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ cf_quic_ctx *qctx)
{
  ಧ್ರುವ unsigned ವರ್ಣ *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  ಯಾವತ್(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    ಯದಿ(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      ಯದಿ(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    ಯದಿ(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    ಅನ್ಯಥಾ {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      ಯದಿ(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      ಅನ್ಯಥಾ {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    ಯದಿ(result) {
      ಯದಿ(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        ಯದಿ(qctx->split_len)
          qctx->split_len -= sent;
      }
      ಪ್ರತ್ಯಾಗಮ result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    ಯದಿ(qctx->split_len)
      qctx->split_len -= sent;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_vquic_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ಪ್ರತ್ಯಾಗಮ Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  ಪ್ರತ್ಯಾಗಮ Curl_vquic_flush(cf, data, qctx);
}

#ಯದಿ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_ಸಂದೇಶ_X)
ಸ್ಥಿರ size_t vquic_msghdr_get_udp_gro(ಸಂರಚನಾ msghdr *ಸಂದೇಶ)
{
  ಪೂರ್ಣಾಙ್ಕ gso_size = 0;
#ಯದಿ defined(__linux__) && defined(UDP_GRO)
  ಸಂರಚನಾ cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#ಯದಿ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  ಪ್ರತಿ(cmsg = CMSG_FIRSTHDR(ಸಂದೇಶ); cmsg; cmsg = CMSG_NXTHDR(ಸಂದೇಶ, cmsg)) {
#ಯದಿ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ತೆಗೆ
#endif
    ಯದಿ(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), ಪರಿಮಾಣ(gso_size));

      ಭಙ್ಗ;
    }
  }
#endif /* linux && UDP_GRO */
  (ಶೂನ್ಯ)ಸಂದೇಶ;

  ಪ್ರತ್ಯಾಗಮ (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#ಯದಿ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_ಸಂದೇಶ_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
ಸ್ಥಿರ uint8_t vquic_msghdr_get_ecn(ಸಂರಚನಾ msghdr *ಸಂದೇಶ, ಪೂರ್ಣಾಙ್ಕ family)
{
  ಸಂರಚನಾ cmsghdr *cmsg;
  ವಿಕಲ್ಪ(family) {
  ಪಕ್ಷ AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#ಯದಿ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    ಪ್ರತಿ(cmsg = CMSG_FIRSTHDR(ಸಂದೇಶ); cmsg; cmsg = CMSG_NXTHDR(ಸಂದೇಶ, cmsg)) {
#ಯದಿ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ತೆಗೆ
#endif
      ಯದಿ(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#ಅನ್ಯಥಾ
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        ಪ್ರತ್ಯಾಗಮ *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#ಯದಿ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    ಪ್ರತಿ(cmsg = CMSG_FIRSTHDR(ಸಂದೇಶ); cmsg; cmsg = CMSG_NXTHDR(ಸಂದೇಶ, cmsg)) {
#ಯದಿ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ತೆಗೆ
#endif
      ಯದಿ(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned ಪೂರ್ಣಾಙ್ಕ tos;

        memcpy(&tos, CMSG_DATA(cmsg), ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));

        ಪ್ರತ್ಯಾಗಮ (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#ಅನ್ಯಥಾ
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

ಸ್ಥಿರ CURLcode recvmmsg_packets(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp,
                                 size_t *pnread)
{
#ಯದಿ defined(__linux__) && defined(UDP_GRO)
#define MMSG_ಅಂಕೆ  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_ಸಂದೇಶ_SIZE    (2 * CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)))
#ಅನ್ಯಥಾ
#define MMSG_ಅಂಕೆ  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_ಸಂದೇಶ_SIZE    CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))
#endif
#define ಸಂದೇಶ_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  ಸಂರಚನಾ iovec msg_iov[MMSG_ಅಂಕೆ];
  ಸಂರಚನಾ mmsghdr mmsg[MMSG_ಅಂಕೆ];
  uint8_t ಸಂದೇಶ_ctrl[MMSG_ಅಂಕೆ * CMSG_PER_ಸಂದೇಶ_SIZE];
  ಸಂರಚನಾ sockaddr_storage remote_addr[MMSG_ಅಂಕೆ];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  ಪೂರ್ಣಾಙ್ಕ mcount, i, n;
  ವರ್ಣ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  ವರ್ಣ *sockbuf = ರಿಕ್ತ;
  uint8_t (*bufs)[ಸಂದೇಶ_BUF_SIZE] = ರಿಕ್ತ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_ಅಂಕೆ * ಸಂದೇಶ_BUF_SIZE,
                                          &sockbuf);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  bufs = (uint8_t (*)[ಸಂದೇಶ_BUF_SIZE])sockbuf;

  total_nread = 0;
  ಯಾವತ್(pkts < max_pkts) {
    n = (ಪೂರ್ಣಾಙ್ಕ)CURLMIN(CURLMIN(MMSG_ಅಂಕೆ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ಪರಿಮಾಣ(mmsg));
    ಪ್ರತಿ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ಪರಿಮಾಣ(bufs[i]);
      mmsg[i].ಸಂದೇಶ_hdr.msg_iov = &msg_iov[i];
      mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_iovlen = 1;
      mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_name = &remote_addr[i];
      mmsg[i].ಸಂದೇಶ_hdr.msg_namelen = ಪರಿಮಾಣ(remote_addr[i]);
      mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_control = &ಸಂದೇಶ_ctrl[i * CMSG_PER_ಸಂದೇಶ_SIZE];
      mmsg[i].ಸಂದೇಶ_hdr.msg_controllen = CMSG_PER_ಸಂದೇಶ_SIZE;
    }

    ಯಾವತ್((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ರಿಕ್ತ)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ಯದಿ(mcount == -1) {
      ಯದಿ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        ಗಚ್ಛ out;
      }
      ಯದಿ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ಸಂರಚನಾ ip_quadruple ip;
        ಯದಿ(!Curl_cf_socket_peek(cf->next, data, ರಿಕ್ತ, ರಿಕ್ತ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ಗಚ್ಛ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ಪರಿಮಾಣ(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }

    VERBOSE(++calls);
    ಪ್ರತಿ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ಯದಿ(!mmsg[i].ಸಂದೇಶ_len) {
        ++pkts;
        ಅನುವರ್ತ;
      }
      total_nread += mmsg[i].ಸಂದೇಶ_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].ಸಂದೇಶ_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].ಸಂದೇಶ_hdr);
      ಯದಿ(gso_size == 0)
        gso_size = mmsg[i].ಸಂದೇಶ_len;

      result = recv_cb(bufs[i], mmsg[i].ಸಂದೇಶ_len, gso_size,
                       mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_name,
                       mmsg[i].ಸಂದೇಶ_hdr.msg_namelen, ecn, userp);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      pkts += (mmsg[i].ಸಂದೇಶ_len + gso_size - 1) / gso_size;
    }
  }

out:
  ಯದಿ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ಪೂರ್ಣಾಙ್ಕ)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ಪ್ರತ್ಯಾಗಮ result;
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_APPLE_ಸಂದೇಶ_X)

ಸ್ಥಿರ CURLcode recvmsg_x_packets(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  ಶೂನ್ಯ *userp, size_t *pnread)
{
#define ಸಂದೇಶ_X_ಅಂಕೆ  64
#define ಸಂದೇಶ_BUF_SIZE  (2048)
#define CMSG_PER_ಸಂದೇಶ_SIZE     CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))
  ಸಂರಚನಾ iovec msg_iov[ಸಂದೇಶ_X_ಅಂಕೆ];
  ಸಂರಚನಾ msghdr_x mmsg[ಸಂದೇಶ_X_ಅಂಕೆ];
  uint8_t ಸಂದೇಶ_ctrl[ಸಂದೇಶ_X_ಅಂಕೆ * CMSG_PER_ಸಂದೇಶ_SIZE];
  ಸಂರಚನಾ sockaddr_storage remote_addr[ಸಂದೇಶ_X_ಅಂಕೆ];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  ಪೂರ್ಣಾಙ್ಕ mcount, i;
  ವರ್ಣ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  ವರ್ಣ *sockbuf = ರಿಕ್ತ;
  uint8_t (*bufs)[ಸಂದೇಶ_BUF_SIZE] = ರಿಕ್ತ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, ಸಂದೇಶ_X_ಅಂಕೆ * ಸಂದೇಶ_BUF_SIZE,
                                          &sockbuf);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  bufs = (uint8_t (*)[ಸಂದೇಶ_BUF_SIZE])sockbuf;

  total_nread = 0;
  ಯಾವತ್(pkts < max_pkts) {
    ಪೂರ್ಣಾಙ್ಕ n = (ಪೂರ್ಣಾಙ್ಕ)CURLMIN(CURLMIN(ಸಂದೇಶ_X_ಅಂಕೆ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ಪರಿಮಾಣ(mmsg));
    ಪ್ರತಿ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ಪರಿಮಾಣ(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].ಸಂದೇಶ_iovlen = 1;
      mmsg[i].ಸಂದೇಶ_name = &remote_addr[i];
      mmsg[i].msg_namelen = ಪರಿಮಾಣ(remote_addr[i]);
      mmsg[i].ಸಂದೇಶ_control = &ಸಂದೇಶ_ctrl[i * CMSG_PER_ಸಂದೇಶ_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_ಸಂದೇಶ_SIZE;
    }

#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ಯಾವತ್((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#ಯದಿ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ತೆಗೆ
#endif
    ಯದಿ(mcount == -1) {
      ಯದಿ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        ಗಚ್ಛ out;
      }
      ಯದಿ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ಸಂರಚನಾ ip_quadruple ip;
        ಯದಿ(!Curl_cf_socket_peek(cf->next, data, ರಿಕ್ತ, ರಿಕ್ತ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ಗಚ್ಛ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ಪರಿಮಾಣ(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }

    VERBOSE(++calls);
    ಪ್ರತಿ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ಯದಿ(!mmsg[i].ಸಂದೇಶ_datalen) {
        ++pkts;
        ಅನುವರ್ತ;
      }
      total_nread += mmsg[i].ಸಂದೇಶ_datalen;
      ecn = vquic_msghdr_get_ecn((ಸಂರಚನಾ msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].ಸಂದೇಶ_datalen;

      result = recv_cb(bufs[i], mmsg[i].ಸಂದೇಶ_datalen, gso_size,
                       mmsg[i].ಸಂದೇಶ_name,
                       mmsg[i].msg_namelen, ecn, userp);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      pkts += (mmsg[i].ಸಂದೇಶ_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  ಯದಿ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ಪೂರ್ಣಾಙ್ಕ)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ಪ್ರತ್ಯಾಗಮ result;
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_SENDMSG)
ಸ್ಥಿರ CURLcode recvmsg_packets(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp,
                                size_t *pnread)
{
#define CMSG_PER_ಸಂದೇಶ_SIZE    CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))
  ಸಂರಚನಾ iovec msg_iov;
  ಸಂರಚನಾ msghdr ಸಂದೇಶ;
  uint8_t buf[64 * 1024];
  ಸಂರಚನಾ sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  ವರ್ಣ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t ಸಂದೇಶ_ctrl[CMSG_PER_ಸಂದೇಶ_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ಪ್ರತಿ(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&ಸಂದೇಶ, 0, ಪರಿಮಾಣ(ಸಂದೇಶ));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = ಪರಿಮಾಣ(buf);
    ಸಂದೇಶ.msg_iov = &msg_iov;
    ಸಂದೇಶ.ಸಂದೇಶ_iovlen = 1;
    ಸಂದೇಶ.ಸಂದೇಶ_control = ಸಂದೇಶ_ctrl;
    ಸಂದೇಶ.ಸಂದೇಶ_name = &remote_addr;
    ಸಂದೇಶ.msg_namelen = ಪರಿಮಾಣ(remote_addr);
    ಸಂದೇಶ.msg_controllen = CMSG_PER_ಸಂದೇಶ_SIZE;

    ಯಾವತ್((rc = recvmsg(qctx->sockfd, &ಸಂದೇಶ, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ಯದಿ(!curlx_sztouz(rc, &nread)) {
      ಯದಿ(SOCK_EAGAIN(SOCKERRNO)) {
        ಗಚ್ಛ out;
      }
      ಯದಿ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ಸಂರಚನಾ ip_quadruple ip;
        ಯದಿ(!Curl_cf_socket_peek(cf->next, data, ರಿಕ್ತ, ರಿಕ್ತ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ಗಚ್ಛ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ಪರಿಮಾಣ(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ಯದಿ(!nread) {
      ++pkts;
      ಅನುವರ್ತ;
    }

    ecn = vquic_msghdr_get_ecn(&ಸಂದೇಶ, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&ಸಂದೇಶ);
    ಯದಿ(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     ಸಂದೇಶ.ಸಂದೇಶ_name, ಸಂದೇಶ.msg_namelen, ecn, userp);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  ಯದಿ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ಪೂರ್ಣಾಙ್ಕ)result);
  *pnread = total_nread;
  ಪ್ರತ್ಯಾಗಮ result;
}

#ಅನ್ಯಥಾ /* HAVE_SENDMMSG || HAVE_SENDMSG */
ಸ್ಥಿರ CURLcode recvfrom_packets(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  ಪೂರ್ಣಾಙ್ಕ bufsize = (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf);
  ಸಂರಚನಾ sockaddr_storage remote_addr;
  socklen_t remote_addrlen = ಪರಿಮಾಣ(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  ವರ್ಣ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  ಪ್ರತಿ(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    ಯಾವತ್((rv = recvfrom(qctx->sockfd, (ವರ್ಣ *)buf, bufsize, 0,
                         (ಸಂರಚನಾ sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ಯದಿ(!curlx_sztouz(rv, &nread)) {
      ಯದಿ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        ಗಚ್ಛ out;
      }
      ಯದಿ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ಸಂರಚನಾ ip_quadruple ip;
        ಯದಿ(!Curl_cf_socket_peek(cf->next, data, ರಿಕ್ತ, ರಿಕ್ತ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        ಗಚ್ಛ out;
      }
      curlx_strerror(SOCKERRNO, errstr, ಪರಿಮಾಣ(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ಯದಿ(!nread)
      ಅನುವರ್ತ;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

out:
  ಯದಿ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ಪೂರ್ಣಾಙ್ಕ)result);
  *pnread = total_nread;
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#ನೋಚೇದ್ಯದಿ defined(HAVE_APPLE_ಸಂದೇಶ_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#ನೋಚೇದ್ಯದಿ defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#ಅನ್ಯಥಾ
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  ಯದಿ(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    ಯದಿ(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ಸಂರಚನಾ Curl_easy *data,
                      unsigned ವರ್ಣ *scid,
                      size_t scidlen,
                      ಪೂರ್ಣಾಙ್ಕ *qlogfdp)
{
  ವರ್ಣ *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  ಯದಿ(qlog_dir) {
    ಸಂರಚನಾ dynbuf fname;
    CURLcode result;
    unsigned ಪೂರ್ಣಾಙ್ಕ i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    ಯದಿ(!result)
      result = curlx_dyn_add(&fname, "/");
    ಪ್ರತಿ(i = 0; (i < scidlen) && !result; i++) {
      ವರ್ಣ hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    ಯದಿ(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    ಯದಿ(!result) {
      ಪೂರ್ಣಾಙ್ಕ qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ದ್ವಿಮಾನ,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#ಅನ್ಯಥಾ
                              (mode_t)data->set.new_file_perms
#endif
                              );
      ಯದಿ(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cf_quic_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_peer *origin,
                                   ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)data; /* not used in all cases and compilers are stupid */
#ಯದಿ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_ಸೇರಿಸು_after(cf_at, origin, peer, &cf);
#ನೋಚೇದ್ಯದಿ defined(USE_QUICHE)
  result = Curl_cf_quiche_ಸೇರಿಸು_after(cf_at, origin, peer, &cf);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf)
  (ಶೂನ್ಯ)cf_at;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#ಯದಿ defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  ಯದಿ(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_quic_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_peer *origin,
                             ಸಂರಚನಾ Curl_peer *peer,
                             uint8_t transport_peer,
                             ಸಂರಚನಾ connectdata *conn,
                             ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                             ಸಂರಚನಾ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (ಶೂನ್ಯ)transport_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  (ಶೂನ್ಯ)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ಯದಿ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#ನೋಚೇದ್ಯದಿ defined(USE_QUICHE)
  ಪ್ರತ್ಯಾಗಮ Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#ಅನ್ಯಥಾ
  *pcf = ರಿಕ್ತ;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)addr;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

#ಯದಿ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_peer *origin,
                                       ಸಂರಚನಾ Curl_peer *peer,
                                       ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#ಯದಿ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_proxy_ಸೇರಿಸು_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf_at;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_peer *origin,
                                 ಸಂರಚನಾ Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ಸಂರಚನಾ connectdata *conn,
                                 ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                                 ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ಯದಿ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#ಅನ್ಯಥಾ
  *pcf = ರಿಕ್ತ;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)addr;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)transport_peer;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn)
{
  ಯದಿ(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }
  ಯದಿ(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  ಯದಿ(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    ಪ್ರತ್ಯಾಗಮ CURLE_URL_MALFORMAT;
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)conn;
#endif

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef CURLVERBOSE
ಧ್ರುವ ವರ್ಣ *Curl_vquic_h3_err_str(uint64_t error_code)
{
  ಯದಿ(error_code <= UINT_MAX) {
    ವಿಕಲ್ಪ((unsigned ಪೂರ್ಣಾಙ್ಕ)error_code) {
    ಪಕ್ಷ CURL_H3_ERR_NO_ERROR:
      ಪ್ರತ್ಯಾಗಮ "NO_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ಪ್ರತ್ಯಾಗಮ "GENERAL_PROTOCOL_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_INTERNAL_ERROR:
      ಪ್ರತ್ಯಾಗಮ "INTERNAL_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_STREAM_CREATION_ERROR:
      ಪ್ರತ್ಯಾಗಮ "STREAM_CREATION_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      ಪ್ರತ್ಯಾಗಮ "CLOSED_CRITICAL_STREAM";
    ಪಕ್ಷ CURL_H3_ERR_FRAME_UNEXPECTED:
      ಪ್ರತ್ಯಾಗಮ "FRAME_UNEXPECTED";
    ಪಕ್ಷ CURL_H3_ERR_FRAME_ERROR:
      ಪ್ರತ್ಯಾಗಮ "FRAME_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_EXCESSIVE_LOAD:
      ಪ್ರತ್ಯಾಗಮ "EXCESSIVE_LOAD";
    ಪಕ್ಷ CURL_H3_ERR_ID_ERROR:
      ಪ್ರತ್ಯಾಗಮ "ID_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_SETTINGS_ERROR:
      ಪ್ರತ್ಯಾಗಮ "SETTINGS_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_MISSING_SETTINGS:
      ಪ್ರತ್ಯಾಗಮ "MISSING_SETTINGS";
    ಪಕ್ಷ CURL_H3_ERR_REQUEST_REJECTED:
      ಪ್ರತ್ಯಾಗಮ "REQUEST_REJECTED";
    ಪಕ್ಷ CURL_H3_ERR_REQUEST_CANCELLED:
      ಪ್ರತ್ಯಾಗಮ "REQUEST_CANCELLED";
    ಪಕ್ಷ CURL_H3_ERR_REQUEST_INCOMPLETE:
      ಪ್ರತ್ಯಾಗಮ "REQUEST_INCOMPLETE";
    ಪಕ್ಷ CURL_H3_ERR_MESSAGE_ERROR:
      ಪ್ರತ್ಯಾಗಮ "MESSAGE_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_CONNECT_ERROR:
      ಪ್ರತ್ಯಾಗಮ "CONNECT_ERROR";
    ಪಕ್ಷ CURL_H3_ERR_VERSION_FALLBACK:
      ಪ್ರತ್ಯಾಗಮ "VERSION_FALLBACK";
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  ಯದಿ((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    ಪ್ರತ್ಯಾಗಮ "NO_ERROR";
  ಪ್ರತ್ಯಾಗಮ "unknown";
}
#endif /* CURLVERBOSE */

#ಯದಿ defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ಸ್ಥಿರ ಶೂನ್ಯ *vquic_ngtcp2_malloc(size_t size, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಪ್ರತ್ಯಾಗಮ Curl_cmalloc(size);
}

ಸ್ಥಿರ ಶೂನ್ಯ vquic_ngtcp2_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  Curl_cfree(ptr);
}

ಸ್ಥಿರ ಶೂನ್ಯ *vquic_ngtcp2_calloc(size_t nmemb, size_t size, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಪ್ರತ್ಯಾಗಮ Curl_ccalloc(nmemb, size);
}

ಸ್ಥಿರ ಶೂನ್ಯ *vquic_ngtcp2_realloc(ಶೂನ್ಯ *ptr, size_t size, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಪ್ರತ್ಯಾಗಮ Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
ಸ್ಥಿರ ಸಂರಚನಾ ngtcp2_mem vquic_ngtcp2_mem = {
  ರಿಕ್ತ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ಸಂರಚನಾ ngtcp2_mem *Curl_ngtcp2_mem(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
ಸ್ಥಿರ ಸಂರಚನಾ nghttp3_mem vquic_nghttp3_mem = {
  ರಿಕ್ತ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ಸಂರಚನಾ nghttp3_mem *Curl_nghttp3_mem(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#ಅನ್ಯಥಾ /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
