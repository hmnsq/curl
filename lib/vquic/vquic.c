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
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_UDP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/udp.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/ip.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP3
#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp3/nghttp3.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/syscall.h>
#ਜੇ defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_APPLE_ਸੁਨੇਹਾ_X
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2-cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-quiche.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NW_CHUNK_ਮਾਪ     (64 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NW_SEND_CHUNKS    1

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_APPLE_ਸੁਨੇਹਾ_X

/* this is `struct msghdr` with an additional field at the end */
ਬਣਤਰ msghdr_x {
  ਖਾਲੀ *ਸੁਨੇਹਾ_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ਬਣਤਰ iovec *msg_iov;    /* scatter/gather array */
  ਪੂਰਨ ਸੁਨੇਹਾ_iovlen;           /* # elements in msg_iov */
  ਖਾਲੀ *ਸੁਨੇਹਾ_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  ਪੂਰਨ ਸੁਨੇਹਾ_flags;            /* flags on received message */
  size_t ਸੁਨੇਹਾ_datalen;       /* byte length of buffer in msg_iov */
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_APPLE_ਸੁਨੇਹਾ_X
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VQUIC_SEND_ਵਿਧੀ   "sendmsg_x"
#ਜਾਂਜੇ defined(HAVE_SENDMSG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VQUIC_SEND_ਵਿਧੀ   "sendmsg"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VQUIC_SEND_ਵਿਧੀ   "send"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

ਪੂਰਨ Curl_vquic_init(ਖਾਲੀ)
{
#ਜੇ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  ਜੇ(ngtcp2_crypto_ossl_init())
    ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ 1;
}

ਖਾਲੀ Curl_vquic_cleanup(ਖਾਲੀ)
{
#ਜੇ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x011800)
  ngtcp2_crypto_ossl_free();
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_quic_ver(ਅੱਖਰ *p, size_t len)
{
#ਜੇ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#ਜਾਂਜੇ defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#ਜੇ_ਅੰਤ
}

CURLcode Curl_vquic_ctx_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_ਮਾਪ, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
#ਜੇ defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#ਜਾਂਜੇ defined(HAVE_APPLE_ਸੁਨੇਹਾ_X)
  qctx->no_gso = FALSE;
#ਨਹੀਂਤਾਂ
  qctx->no_gso = TRUE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_DBG_QUIC_WBLOCK");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, 100))
        qctx->wblock_percent = (ਪੂਰਨ)l;
    }
  }
#ਜੇ_ਅੰਤ
  Curl_vquic_ctx_ਸੈੱਟ_ਸਮਾਂ(qctx, Curl_pgrs_now(ਅੰਕੜੇ));

  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_vquic_ctx_free(ਬਣਤਰ cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

ਖਾਲੀ Curl_vquic_ctx_ਸੈੱਟ_ਸਮਾਂ(ਬਣਤਰ cf_quic_ctx *qctx,
                             ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  qctx->ਆਖਰੀ_op = *pnow;
}

ਖਾਲੀ Curl_vquic_ctx_update_ਸਮਾਂ(ਬਣਤਰ cf_quic_ctx *qctx,
                                ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  qctx->ਆਖਰੀ_op = *pnow;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_APPLE_ਸੁਨੇਹਾ_X
ਸਥਿਰ CURLcode do_sendmsg(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ cf_quic_ctx *qctx,
                           ਅਚਲ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_X_SNUM  64
  ਬਣਤਰ iovec msg_iov[ਸੁਨੇਹਾ_X_SNUM];
  ਬਣਤਰ msghdr_x mmsg[ਸੁਨੇਹਾ_X_SNUM];
  ਅੱਖਰ errstr[STRERROR_LEN];
  size_t n, i = 0, ਭੇਜਿਆ = 0;
  ਪੂਰਨ rc;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  ਜੇ(!pktlen)
    ਵਾਪਸ CURLE_OK;
  ਜੇ(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  ਜਦੋਂ(i < n) {
    size_t j, batch = CURLMIN(n - i, ਸੁਨੇਹਾ_X_SNUM), pkts_ਭੇਜਿਆ = 0;

    ਲਈ(j = 0; j < batch; ++j) {
      ਅਚਲ size_t ਵਿੱਥ = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + ਵਿੱਥ);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - ਵਿੱਥ);
      memset(&mmsg[j], 0, ਆਕਾਰ(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].ਸੁਨੇਹਾ_iovlen = 1;
      mmsg[j].ਸੁਨੇਹਾ_datalen = msg_iov[j].iov_len;
    }

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ
    ਜਦੋਂ((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

    ਜੇ(rc < 0) {
      ਜੇ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "egress, sendmsg_x -> EAGAIN");
        ਨਤੀਜਾ = ਭੇਜਿਆ ? CURLE_OK : CURLE_AGAIN;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, ਆਕਾਰ(errstr));
        failf(ਅੰਕੜੇ, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_ਭੇਜਿਆ = batch;
    }
    ਨਹੀਂਤਾਂ
      pkts_ਭੇਜਿਆ = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    ਜੇ(!pkts_ਭੇਜਿਆ) {  /* no packets of the current batch were sent */
      ਨਤੀਜਾ = ਭੇਜਿਆ ? CURLE_OK : CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    i += pkts_ਭੇਜਿਆ; /* Some have been sent */
    ਲਈ(j = 0; j < pkts_ਭੇਜਿਆ; ++j)
      ਭੇਜਿਆ += msg_iov[j].iov_len;
    ਜੇ(pkts_ਭੇਜਿਆ < batch)
      ਜਾਓ ਬਾਹਰਲਾ; /* but not all of them */
  }

ਬਾਹਰਲਾ:
  *psent = ਭੇਜਿਆ;
  ਜੇ(ਭੇਜਿਆ || ਨਤੀਜਾ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                ਭੇਜਿਆ, gsolen, n, calls, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਨਹੀਂਤਾਂ /* HAVE_APPLE_MSG_X */

ਸਥਿਰ CURLcode send_packet_no_gso(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ cf_quic_ctx *qctx,
                                   ਅਚਲ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ਸਥਿਰ CURLcode do_sendmsg(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ cf_quic_ctx *qctx,
                           ਅਚਲ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SENDMSG
  ਬਣਤਰ iovec msg_iov;
  ਬਣਤਰ msghdr ਸੁਨੇਹਾ = { 0 };
  ssize_t rv;
#ਜੇ defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t ਸੁਨੇਹਾ_ctrl[32];
  ਬਣਤਰ cmsghdr *cm;
#ਜੇ_ਅੰਤ

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  ਸੁਨੇਹਾ.msg_iov = &msg_iov;
  ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_iovlen = 1;

#ਜੇ defined(__linux__) && defined(UDP_SEGMENT)
  ਜੇ(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(ਸੁਨੇਹਾ_ctrl, 0, ਆਕਾਰ(ਸੁਨੇਹਾ_ctrl));
    ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_control = ਸੁਨੇਹਾ_ctrl;
    DEBUGASSERT(ਆਕਾਰ(ਸੁਨੇਹਾ_ctrl) >= CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ)));
    ਸੁਨੇਹਾ.msg_controllen = CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ));
    cm = CMSG_FIRSTHDR(&ਸੁਨੇਹਾ);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(ਆਕਾਰ(uint16_t));
    *(uint16_t *)(ਖਾਲੀ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#ਜੇ_ਅੰਤ

  ਜਦੋਂ((rv = sendmsg(qctx->sockfd, &ਸੁਨੇਹਾ, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  ਜੇ(!curlx_sztouz(rv, psent)) {
    ਪੂਰਨ sockerr = SOCKERRNO;
    ਜੇ(SOCK_EAGAIN(sockerr))
      ਵਾਪਸ CURLE_AGAIN;
    ਚੋਣ(sockerr) {
    ਹਾਲਤ SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      ਰੋਕੋ;
    ਹਾਲਤ EIO:
      ਜੇ(pktlen > gsolen) {
        /* GSO failure */
        infof(ਅੰਕੜੇ, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ਵਾਪਸ send_packet_no_gso(cf, ਅੰਕੜੇ, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(pktlen != *psent) {
    failf(ਅੰਕੜੇ, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਨਹੀਂਤਾਂ
  ssize_t rv;
  (ਖਾਲੀ)gsolen;

  *psent = 0;

  ਜਦੋਂ((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  ਜੇ(!curlx_sztouz(rv, psent)) {
    ਜੇ(SOCK_EAGAIN(SOCKERRNO)) {
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(SOCKERRNO != SOCKEMSGSIZE) {
        failf(ਅੰਕੜੇ, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#ਜੇ_ਅੰਤ
  (ਖਾਲੀ)cf;

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_ਵਿਧੀ, pktlen, gsolen, (ਪੂਰਨ)ਨਤੀਜਾ, *psent);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !HAVE_APPLE_MSG_X */

ਸਥਿਰ CURLcode send_packet_no_gso(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ cf_quic_ctx *qctx,
                                   ਅਚਲ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ਅਚਲ uint8_t *p, *ਸਿਰਾ = pkt + pktlen;
  size_t ਭੇਜਿਆ, len;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  ਲਈ(p = pkt; p < ਸਿਰਾ; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(ਸਿਰਾ - p));
    ਨਤੀਜਾ = do_sendmsg(cf, ਅੰਕੜੇ, qctx, p, len, len, &ਭੇਜਿਆ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    *psent += ਭੇਜਿਆ;
    VERBOSE(++calls);
  }
ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_ਵਿਧੀ, pktlen, gsolen, calls, (ਪੂਰਨ)ਨਤੀਜਾ, *psent);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ਸਥਿਰ CURLcode send_packet_no_gso_cf(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਅਚਲ uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ਅਚਲ uint8_t *p, *ਸਿਰਾ = pkt + pktlen;
  size_t ਭੇਜਿਆ, len;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  ਲਈ(p = pkt; p < ਸਿਰਾ; p += len) {
    len = CURLMIN(gsolen, (size_t)(ਸਿਰਾ - p));
    ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, p, len, FALSE, &ਭੇਜਿਆ);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਜੇ(ਭੇਜਿਆ != len) {
      /* We can only send the complete datagram, not parts. */
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    *psent += ਭੇਜਿਆ;
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (ਪੂਰਨ)ਨਤੀਜਾ, *psent);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode vquic_send_packets(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ cf_quic_ctx *qctx,
                                   ਅਚਲ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode ਨਤੀਜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  /* simulate network blocking/partial writes */
  ਜੇ(qctx->wblock_percent > 0) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c;
    *psent = 0;
    Curl_rand(ਅੰਕੜੇ, &c, 1);
    ਜੇ(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "vquic_flush() simulate EWOULDBLOCK");
      ਵਾਪਸ CURLE_AGAIN;
    }
  }
#ਜੇ_ਅੰਤ
  ਜੇ(qctx->no_gso && pktlen > gsolen) {
    ਨਤੀਜਾ = send_packet_no_gso(cf, ਅੰਕੜੇ, qctx, pkt, pktlen, gsolen, psent);
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = do_sendmsg(cf, ਅੰਕੜੇ, qctx, pkt, pktlen, gsolen, psent);
  }
  ਜੇ(!ਨਤੀਜਾ)
    qctx->ਆਖਰੀ_io = qctx->ਆਖਰੀ_op;
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_vquic_flush(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ cf_quic_ctx *qctx)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
  size_t blen, ਭੇਜਿਆ;
  CURLcode ਨਤੀਜਾ;
  size_t gsolen;

  ਜਦੋਂ(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    ਜੇ(qctx->ਵੰਡੋ_len) {
      gsolen = qctx->ਵੰਡੋ_gsolen;
      ਜੇ(blen > qctx->ਵੰਡੋ_len)
        blen = qctx->ਵੰਡੋ_len;
    }

    ਜੇ(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      ਨਤੀਜਾ = vquic_send_packets(cf, ਅੰਕੜੇ, qctx, buf, blen, gsolen, &ਭੇਜਿਆ);
    }
    ਨਹੀਂਤਾਂ {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      ਜੇ(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        ਨਤੀਜਾ = send_packet_no_gso_cf(cf, ਅੰਕੜੇ, buf, blen, gsolen, &ਭੇਜਿਆ);
      }
      ਨਹੀਂਤਾਂ {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, blen, FALSE, &ਭੇਜਿਆ);
      }
    }

    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, ਭੇਜਿਆ);
        ਜੇ(qctx->ਵੰਡੋ_len)
          qctx->ਵੰਡੋ_len -= ਭੇਜਿਆ;
      }
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    Curl_bufq_skip(&qctx->sendbuf, ਭੇਜਿਆ);
    ਜੇ(qctx->ਵੰਡੋ_len)
      qctx->ਵੰਡੋ_len -= ਭੇਜਿਆ;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_vquic_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ਵਾਪਸ Curl_vquic_flush(cf, ਅੰਕੜੇ, qctx);
}

CURLcode Curl_vquic_send_ਪੂਛ_ਵੰਡੋ(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ cf_quic_ctx *qctx, size_t gsolen,
                                    size_t ਪੂਛ_len, size_t ਪੂਛ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > ਪੂਛ_len);
  qctx->ਵੰਡੋ_len = Curl_bufq_len(&qctx->sendbuf) - ਪੂਛ_len;
  qctx->ਵੰਡੋ_gsolen = gsolen;
  qctx->gsolen = ਪੂਛ_gsolen;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->ਵੰਡੋ_len, qctx->ਵੰਡੋ_gsolen, ਪੂਛ_len, qctx->gsolen);
  ਵਾਪਸ Curl_vquic_flush(cf, ਅੰਕੜੇ, qctx);
}

#ਜੇ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_ਸੁਨੇਹਾ_X)
ਸਥਿਰ size_t vquic_msghdr_get_udp_gro(ਬਣਤਰ msghdr *ਸੁਨੇਹਾ)
{
  ਪੂਰਨ gso_ਮਾਪ = 0;
#ਜੇ defined(__linux__) && defined(UDP_GRO)
  ਬਣਤਰ cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#ਜੇ defined(__clang__) && !defined(__GLIBC__)
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wsign-compare"
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-align"
#ਜੇ_ਅੰਤ
  ਲਈ(cmsg = CMSG_FIRSTHDR(ਸੁਨੇਹਾ); cmsg; cmsg = CMSG_NXTHDR(ਸੁਨੇਹਾ, cmsg)) {
#ਜੇ defined(__clang__) && !defined(__GLIBC__)
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
    ਜੇ(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&gso_ਮਾਪ, CMSG_DATA(cmsg), ਆਕਾਰ(gso_ਮਾਪ));

      ਰੋਕੋ;
    }
  }
#ਜੇ_ਅੰਤ /* linux && UDP_GRO */
  (ਖਾਲੀ)ਸੁਨੇਹਾ;

  ਵਾਪਸ (size_t)gso_ਮਾਪ;
}
#ਜੇ_ਅੰਤ /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#ਜੇ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_ਸੁਨੇਹਾ_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_ਨਕਾਬ)
ਸਥਿਰ uint8_t vquic_msghdr_get_ecn(ਬਣਤਰ msghdr *ਸੁਨੇਹਾ, ਪੂਰਨ family)
{
  ਬਣਤਰ cmsghdr *cmsg;
  ਚੋਣ(family) {
  ਹਾਲਤ AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#ਜੇ defined(__clang__) && !defined(__GLIBC__)
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wsign-compare"
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-align"
#ਜੇ_ਅੰਤ
    ਲਈ(cmsg = CMSG_FIRSTHDR(ਸੁਨੇਹਾ); cmsg; cmsg = CMSG_NXTHDR(ਸੁਨੇਹਾ, cmsg)) {
#ਜੇ defined(__clang__) && !defined(__GLIBC__)
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
      ਜੇ(cmsg->cmsg_level == IPPROTO_IP &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#ਨਹੀਂਤਾਂ
          cmsg->cmsg_type == IP_TOS
#ਜੇ_ਅੰਤ
          && cmsg->cmsg_len) {
        ਵਾਪਸ *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_ਨਕਾਬ;
      }
    }
    ਰੋਕੋ;
  ਹਾਲਤ AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#ਜੇ defined(__clang__) && !defined(__GLIBC__)
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wsign-compare"
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-align"
#ਜੇ_ਅੰਤ
    ਲਈ(cmsg = CMSG_FIRSTHDR(ਸੁਨੇਹਾ); cmsg; cmsg = CMSG_NXTHDR(ਸੁਨੇਹਾ, cmsg)) {
#ਜੇ defined(__clang__) && !defined(__GLIBC__)
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
      ਜੇ(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ tos;

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&tos, CMSG_DATA(cmsg), ਆਕਾਰ(ਪੂਰਨ));

        ਵਾਪਸ (uint8_t)(tos & IPTOS_ECN_ਨਕਾਬ);
      }
    }
    ਰੋਕੋ;
  }
  ਵਾਪਸ 0;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vquic_msghdr_get_ecn(a, b)      0
#ਜੇ_ਅੰਤ /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SENDMMSG

ਸਥਿਰ CURLcode recvmmsg_packets(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ਖਾਲੀ *userp,
                                 size_t *pnread)
{
#ਜੇ defined(__linux__) && defined(UDP_GRO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MMSG_ਅੰਕ  16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UDP_GRO_CNT_MAX  64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ    (2 * CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ)))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MMSG_ਅੰਕ  64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UDP_GRO_CNT_MAX  1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ    CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ))
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_BUF_ਮਾਪ  (UDP_GRO_CNT_MAX * 1500)
  ਬਣਤਰ iovec msg_iov[MMSG_ਅੰਕ];
  ਬਣਤਰ mmsghdr mmsg[MMSG_ਅੰਕ];
  uint8_t ਸੁਨੇਹਾ_ctrl[MMSG_ਅੰਕ * CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ];
  ਬਣਤਰ sockaddr_storage remote_addr[MMSG_ਅੰਕ];
  size_t ਕੁੱਲ_nread = 0, pkts = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  size_t calls = 0;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ mcount, i, n;
  ਅੱਖਰ errstr[STRERROR_LEN];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t gso_ਮਾਪ;
  ਅੱਖਰ *sockbuf = ਨਿਰਾ;
  uint8_t (*bufs)[ਸੁਨੇਹਾ_BUF_ਮਾਪ] = ਨਿਰਾ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ਨਤੀਜਾ = Curl_multi_xfer_sockbuf_borrow(ਅੰਕੜੇ, MMSG_ਅੰਕ * ਸੁਨੇਹਾ_BUF_ਮਾਪ,
                                          &sockbuf);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  bufs = (uint8_t (*)[ਸੁਨੇਹਾ_BUF_ਮਾਪ])sockbuf;

  ਕੁੱਲ_nread = 0;
  ਜਦੋਂ(pkts < max_pkts) {
    n = (ਪੂਰਨ)CURLMIN(CURLMIN(MMSG_ਅੰਕ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ਆਕਾਰ(mmsg));
    ਲਈ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ਆਕਾਰ(bufs[i]);
      mmsg[i].ਸੁਨੇਹਾ_hdr.msg_iov = &msg_iov[i];
      mmsg[i].ਸੁਨੇਹਾ_hdr.ਸੁਨੇਹਾ_iovlen = 1;
      mmsg[i].ਸੁਨੇਹਾ_hdr.ਸੁਨੇਹਾ_name = &remote_addr[i];
      mmsg[i].ਸੁਨੇਹਾ_hdr.msg_namelen = ਆਕਾਰ(remote_addr[i]);
      mmsg[i].ਸੁਨੇਹਾ_hdr.ਸੁਨੇਹਾ_control = &ਸੁਨੇਹਾ_ctrl[i * CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ];
      mmsg[i].ਸੁਨੇਹਾ_hdr.msg_controllen = CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ;
    }

    ਜਦੋਂ((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ਨਿਰਾ)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ਜੇ(mcount == -1) {
      ਜੇ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "ingress, recvmmsg -> EAGAIN");
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ਬਣਤਰ ip_quadruple ip;
        ਜੇ(!Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਰਾ, ਨਿਰਾ, &ip))
          failf(ਅੰਕੜੇ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ਪੋਰਟ);
        ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      curlx_strerror(SOCKERRNO, errstr, ਆਕਾਰ(errstr));
      failf(ਅੰਕੜੇ, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    VERBOSE(++calls);
    ਲਈ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ਜੇ(!mmsg[i].ਸੁਨੇਹਾ_len) {
        ++pkts;
        ਜਾਰੀ;
      }
      ਕੁੱਲ_nread += mmsg[i].ਸੁਨੇਹਾ_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].ਸੁਨੇਹਾ_hdr, remote_addr[i].ss_family);
      gso_ਮਾਪ = vquic_msghdr_get_udp_gro(&mmsg[i].ਸੁਨੇਹਾ_hdr);
      ਜੇ(gso_ਮਾਪ == 0)
        gso_ਮਾਪ = mmsg[i].ਸੁਨੇਹਾ_len;

      ਨਤੀਜਾ = recv_cb(bufs[i], mmsg[i].ਸੁਨੇਹਾ_len, gso_ਮਾਪ,
                       mmsg[i].ਸੁਨੇਹਾ_hdr.ਸੁਨੇਹਾ_name,
                       mmsg[i].ਸੁਨੇਹਾ_hdr.msg_namelen, ecn, userp);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      pkts += (mmsg[i].ਸੁਨੇਹਾ_len + gso_ਮਾਪ - 1) / gso_ਮਾਪ;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਕੁੱਲ_nread || ਨਤੀਜਾ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ਕੁੱਲ_nread, pkts, calls, (ਪੂਰਨ)ਨਤੀਜਾ);
  Curl_multi_xfer_sockbuf_release(ਅੰਕੜੇ, sockbuf);
  *pnread = ਕੁੱਲ_nread;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜਾਂਜੇ defined(HAVE_APPLE_ਸੁਨੇਹਾ_X)

ਸਥਿਰ CURLcode recvmsg_x_packets(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  ਖਾਲੀ *userp, size_t *pnread)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_X_ਅੰਕ  64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੁਨੇਹਾ_BUF_ਮਾਪ  (2048)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ     CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ))
  ਬਣਤਰ iovec msg_iov[ਸੁਨੇਹਾ_X_ਅੰਕ];
  ਬਣਤਰ msghdr_x mmsg[ਸੁਨੇਹਾ_X_ਅੰਕ];
  uint8_t ਸੁਨੇਹਾ_ctrl[ਸੁਨੇਹਾ_X_ਅੰਕ * CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ];
  ਬਣਤਰ sockaddr_storage remote_addr[ਸੁਨੇਹਾ_X_ਅੰਕ];
  size_t ਕੁੱਲ_nread = 0, pkts = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  size_t calls = 0;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ mcount, i;
  ਅੱਖਰ errstr[STRERROR_LEN];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t gso_ਮਾਪ;
  ਅੱਖਰ *sockbuf = ਨਿਰਾ;
  uint8_t (*bufs)[ਸੁਨੇਹਾ_BUF_ਮਾਪ] = ਨਿਰਾ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ਨਤੀਜਾ = Curl_multi_xfer_sockbuf_borrow(ਅੰਕੜੇ, ਸੁਨੇਹਾ_X_ਅੰਕ * ਸੁਨੇਹਾ_BUF_ਮਾਪ,
                                          &sockbuf);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  bufs = (uint8_t (*)[ਸੁਨੇਹਾ_BUF_ਮਾਪ])sockbuf;

  ਕੁੱਲ_nread = 0;
  ਜਦੋਂ(pkts < max_pkts) {
    ਪੂਰਨ n = (ਪੂਰਨ)CURLMIN(CURLMIN(ਸੁਨੇਹਾ_X_ਅੰਕ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ਆਕਾਰ(mmsg));
    ਲਈ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ਆਕਾਰ(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].ਸੁਨੇਹਾ_iovlen = 1;
      mmsg[i].ਸੁਨੇਹਾ_name = &remote_addr[i];
      mmsg[i].msg_namelen = ਆਕਾਰ(remote_addr[i]);
      mmsg[i].ਸੁਨੇਹਾ_control = &ਸੁਨੇਹਾ_ctrl[i * CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ];
      mmsg[i].msg_controllen = CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ;
    }

#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
#ਜੇ_ਅੰਤ
    ਜਦੋਂ((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#ਜੇ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
    ਜੇ(mcount == -1) {
      ਜੇ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "ingress, recvmsg_x -> EAGAIN");
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ਬਣਤਰ ip_quadruple ip;
        ਜੇ(!Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਰਾ, ਨਿਰਾ, &ip))
          failf(ਅੰਕੜੇ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ਪੋਰਟ);
        ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      curlx_strerror(SOCKERRNO, errstr, ਆਕਾਰ(errstr));
      failf(ਅੰਕੜੇ, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    VERBOSE(++calls);
    ਲਈ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ਜੇ(!mmsg[i].ਸੁਨੇਹਾ_datalen) {
        ++pkts;
        ਜਾਰੀ;
      }
      ਕੁੱਲ_nread += mmsg[i].ਸੁਨੇਹਾ_datalen;
      ecn = vquic_msghdr_get_ecn((ਬਣਤਰ msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_ਮਾਪ = mmsg[i].ਸੁਨੇਹਾ_datalen;

      ਨਤੀਜਾ = recv_cb(bufs[i], mmsg[i].ਸੁਨੇਹਾ_datalen, gso_ਮਾਪ,
                       mmsg[i].ਸੁਨੇਹਾ_name,
                       mmsg[i].msg_namelen, ecn, userp);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      pkts += (mmsg[i].ਸੁਨੇਹਾ_datalen + gso_ਮਾਪ - 1) / gso_ਮਾਪ;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਕੁੱਲ_nread || ਨਤੀਜਾ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                ਕੁੱਲ_nread, pkts, calls, (ਪੂਰਨ)ਨਤੀਜਾ);
  Curl_multi_xfer_sockbuf_release(ਅੰਕੜੇ, sockbuf);
  *pnread = ਕੁੱਲ_nread;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜਾਂਜੇ defined(HAVE_SENDMSG)
ਸਥਿਰ CURLcode recvmsg_packets(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, ਖਾਲੀ *userp,
                                size_t *pnread)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ    CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ))
  ਬਣਤਰ iovec msg_iov;
  ਬਣਤਰ msghdr ਸੁਨੇਹਾ;
  uint8_t buf[64 * 1024];
  ਬਣਤਰ sockaddr_storage remote_addr;
  size_t ਕੁੱਲ_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  ਅੱਖਰ errstr[STRERROR_LEN];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uint8_t ਸੁਨੇਹਾ_ctrl[CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ];
  size_t gso_ਮਾਪ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ਲਈ(pkts = 0, ਕੁੱਲ_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&ਸੁਨੇਹਾ, 0, ਆਕਾਰ(ਸੁਨੇਹਾ));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = ਆਕਾਰ(buf);
    ਸੁਨੇਹਾ.msg_iov = &msg_iov;
    ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_iovlen = 1;
    ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_control = ਸੁਨੇਹਾ_ctrl;
    ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_name = &remote_addr;
    ਸੁਨੇਹਾ.msg_namelen = ਆਕਾਰ(remote_addr);
    ਸੁਨੇਹਾ.msg_controllen = CMSG_ਪ੍ਰਤੀ_ਸੁਨੇਹਾ_ਮਾਪ;

    ਜਦੋਂ((rc = recvmsg(qctx->sockfd, &ਸੁਨੇਹਾ, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ਜੇ(!curlx_sztouz(rc, &nread)) {
      ਜੇ(SOCK_EAGAIN(SOCKERRNO)) {
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ਬਣਤਰ ip_quadruple ip;
        ਜੇ(!Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਰਾ, ਨਿਰਾ, &ip))
          failf(ਅੰਕੜੇ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ਪੋਰਟ);
        ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      curlx_strerror(SOCKERRNO, errstr, ਆਕਾਰ(errstr));
      failf(ਅੰਕੜੇ, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਕੁੱਲ_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ਜੇ(!nread) {
      ++pkts;
      ਜਾਰੀ;
    }

    ecn = vquic_msghdr_get_ecn(&ਸੁਨੇਹਾ, remote_addr.ss_family);
    gso_ਮਾਪ = vquic_msghdr_get_udp_gro(&ਸੁਨੇਹਾ);
    ਜੇ(gso_ਮਾਪ == 0)
      gso_ਮਾਪ = nread;

    ਨਤੀਜਾ = recv_cb(buf, nread, gso_ਮਾਪ,
                     ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_name, ਸੁਨੇਹਾ.msg_namelen, ecn, userp);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    pkts += (nread + gso_ਮਾਪ - 1) / gso_ਮਾਪ;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਕੁੱਲ_nread || ਨਤੀਜਾ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ਕੁੱਲ_nread, pkts, calls, (ਪੂਰਨ)ਨਤੀਜਾ);
  *pnread = ਕੁੱਲ_nread;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਨਹੀਂਤਾਂ /* HAVE_SENDMMSG || HAVE_SENDMSG */
ਸਥਿਰ CURLcode recvfrom_packets(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ਖਾਲੀ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  ਪੂਰਨ bufsize = (ਪੂਰਨ)ਆਕਾਰ(buf);
  ਬਣਤਰ sockaddr_storage remote_addr;
  socklen_t remote_addrlen = ਆਕਾਰ(remote_addr);
  size_t ਕੁੱਲ_nread, pkts, calls = 0, nread;
  ssize_t rv;
  ਅੱਖਰ errstr[STRERROR_LEN];
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  ਲਈ(pkts = 0, ਕੁੱਲ_nread = 0; pkts < max_pkts;) {
    ਜਦੋਂ((rv = recvfrom(qctx->sockfd, (ਅੱਖਰ *)buf, bufsize, 0,
                         (ਬਣਤਰ sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ਜੇ(!curlx_sztouz(rv, &nread)) {
      ਜੇ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "ingress, recvfrom -> EAGAIN");
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ਬਣਤਰ ip_quadruple ip;
        ਜੇ(!Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਰਾ, ਨਿਰਾ, &ip))
          failf(ਅੰਕੜੇ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ਪੋਰਟ);
        ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      curlx_strerror(SOCKERRNO, errstr, ਆਕਾਰ(errstr));
      failf(ਅੰਕੜੇ, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ਜੇ(!nread)
      ਜਾਰੀ;

    ਕੁੱਲ_nread += nread;
    ਨਤੀਜਾ = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਕੁੱਲ_nread || ਨਤੀਜਾ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                ਕੁੱਲ_nread, pkts, calls, (ਪੂਰਨ)ਨਤੀਜਾ);
  *pnread = ਕੁੱਲ_nread;
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ਖਾਲੀ *userp)
{
  CURLcode ਨਤੀਜਾ;
  size_t nread = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SENDMMSG
  ਨਤੀਜਾ = recvmmsg_packets(cf, ਅੰਕੜੇ, qctx, max_pkts, recv_cb, userp, &nread);
#ਜਾਂਜੇ defined(HAVE_APPLE_ਸੁਨੇਹਾ_X)
  ਨਤੀਜਾ = recvmsg_x_packets(cf, ਅੰਕੜੇ, qctx, max_pkts, recv_cb, userp,
                             &nread);
#ਜਾਂਜੇ defined(HAVE_SENDMSG)
  ਨਤੀਜਾ = recvmsg_packets(cf, ਅੰਕੜੇ, qctx, max_pkts, recv_cb, userp, &nread);
#ਨਹੀਂਤਾਂ
  ਨਤੀਜਾ = recvfrom_packets(cf, ਅੰਕੜੇ, qctx, max_pkts, recv_cb, userp, &nread);
#ਜੇ_ਅੰਤ
  ਜੇ(!ਨਤੀਜਾ) {
    /* EAGAIN is no error, only count actual packets as a reply */
    ਜੇ(!qctx->got_ਪਹਿਲਾ_byte && nread) {
      qctx->got_ਪਹਿਲਾ_byte = TRUE;
      qctx->ਪਹਿਲਾ_byte_at = qctx->ਆਖਰੀ_op;
    }
    qctx->ਆਖਰੀ_io = qctx->ਆਖਰੀ_op;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *scid,
                      size_t scidlen,
                      ਪੂਰਨ *qlogfdp)
{
  ਅੱਖਰ *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  ਜੇ(qlog_dir) {
    ਬਣਤਰ dynbuf fname;
    CURLcode ਨਤੀਜਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&fname, qlog_dir);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&fname, "/");
    ਲਈ(i = 0; (i < scidlen) && !ਨਤੀਜਾ; i++) {
      ਅੱਖਰ hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&fname, hex);
    }
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&fname, ".sqlog");

    ਜੇ(!ਨਤੀਜਾ) {
      ਪੂਰਨ qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ਦੁਆਧਾਰੀ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
                              ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms & (_S_IREAD | _S_IWRITE)
#ਨਹੀਂਤਾਂ
                              (mode_t)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms
#ਜੇ_ਅੰਤ
                              );
      ਜੇ(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cf_quic_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ Curl_peer *origin,
                                   ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)ਅੰਕੜੇ; /* not used in all cases and compilers are stupid */
#ਜੇ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ਨਤੀਜਾ = Curl_cf_ngtcp2_ਪਾਓ_after(cf_at, origin, peer, &cf);
#ਜਾਂਜੇ defined(USE_QUICHE)
  ਨਤੀਜਾ = Curl_cf_quiche_ਪਾਓ_after(cf_at, origin, peer, &cf);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf)
  (ਖਾਲੀ)cf_at;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)peer;
  ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  ਜੇ(!ਨਤੀਜਾ && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    ਨਤੀਜਾ = Curl_conn_dns_ਜੋੜੋ_https_resolve(ਅੰਕੜੇ, cf->conn,
                                             cf->sockindex, origin);
  }
#ਜੇ_ਅੰਤ /* USE_HTTPSRR && USE_ECH */
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_quic_create(ਬਣਤਰ Curl_cfilter **pcf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_peer *origin,
                             ਬਣਤਰ Curl_peer *peer,
                             uint8_t transport_peer,
                             ਬਣਤਰ connectdata *conn,
                             ਬਣਤਰ Curl_sockaddr_ex *addr,
                             ਬਣਤਰ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (ਖਾਲੀ)transport_peer;
  (ਖਾਲੀ)tunnel_transport;
  (ਖਾਲੀ)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ਜੇ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ਵਾਪਸ Curl_cf_ngtcp2_create(pcf, ਅੰਕੜੇ, origin, peer, conn, addr);
#ਜਾਂਜੇ defined(USE_QUICHE)
  ਵਾਪਸ Curl_cf_quiche_create(pcf, ਅੰਕੜੇ, origin, peer, conn, addr);
#ਨਹੀਂਤਾਂ
  *pcf = ਨਿਰਾ;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)peer;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)addr;
  (ਖਾਲੀ)tunnel_peer;
  (ਖਾਲੀ)tunnel_transport;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

#ਜੇ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_peer *origin,
                                       ਬਣਤਰ Curl_peer *peer,
                                       ਬਣਤਰ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#ਜੇ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ਵਾਪਸ Curl_cf_ngtcp2_proxy_ਪਾਓ_after(cf_at, ਅੰਕੜੇ, origin, peer,
                                           tunnel_peer, tunnel_transport);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf_at;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)peer;
  (ਖਾਲੀ)tunnel_peer;
  (ਖਾਲੀ)tunnel_transport;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

CURLcode Curl_cf_h3_proxy_create(ਬਣਤਰ Curl_cfilter **pcf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_peer *origin,
                                 ਬਣਤਰ Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ਬਣਤਰ connectdata *conn,
                                 ਬਣਤਰ Curl_sockaddr_ex *addr,
                                 ਬਣਤਰ Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ਜੇ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ਵਾਪਸ Curl_cf_ngtcp2_proxy_create(pcf, ਅੰਕੜੇ, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#ਨਹੀਂਤਾਂ
  *pcf = ਨਿਰਾ;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)addr;
  (ਖਾਲੀ)peer;
  (ਖਾਲੀ)transport_peer;
  (ਖਾਲੀ)tunnel_peer;
  (ਖਾਲੀ)tunnel_transport;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn)
{
  ਜੇ(Curl_conn_get_ਪਹਿਲਾ_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(ਅੰਕੜੇ, "HTTP/3 cannot be used over UNIX domain sockets");
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }
  ਜੇ(!(ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(ਅੰਕੜੇ, "HTTP/3 requested for non-HTTPS URL");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(conn->socks_proxy.peer) {
    failf(ਅੰਕੜੇ, "HTTP/3 is not supported over a SOCKS proxy");
    ਵਾਪਸ CURLE_URL_MALFORMAT;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)conn;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਅਚਲ ਅੱਖਰ *Curl_vquic_h3_err_str(uint64_t error_ਕੋਡ)
{
  ਜੇ(error_ਕੋਡ <= UINT_MAX) {
    ਚੋਣ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)error_ਕੋਡ) {
    ਹਾਲਤ CURL_H3_ERR_NO_ERROR:
      ਵਾਪਸ "NO_ERROR";
    ਹਾਲਤ CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ਵਾਪਸ "GENERAL_PROTOCOL_ERROR";
    ਹਾਲਤ CURL_H3_ERR_ਅੰਦਰੂਨੀ_ERROR:
      ਵਾਪਸ "INTERNAL_ERROR";
    ਹਾਲਤ CURL_H3_ERR_ਧਾਰਾ_CREATION_ERROR:
      ਵਾਪਸ "STREAM_CREATION_ERROR";
    ਹਾਲਤ CURL_H3_ERR_CLOSED_CRITICAL_ਧਾਰਾ:
      ਵਾਪਸ "CLOSED_CRITICAL_STREAM";
    ਹਾਲਤ CURL_H3_ERR_ਚੌਖਟਾ_UNEXPECTED:
      ਵਾਪਸ "FRAME_UNEXPECTED";
    ਹਾਲਤ CURL_H3_ERR_ਚੌਖਟਾ_ERROR:
      ਵਾਪਸ "FRAME_ERROR";
    ਹਾਲਤ CURL_H3_ERR_EXCESSIVE_ਲੱਦੋ:
      ਵਾਪਸ "EXCESSIVE_LOAD";
    ਹਾਲਤ CURL_H3_ERR_ID_ERROR:
      ਵਾਪਸ "ID_ERROR";
    ਹਾਲਤ CURL_H3_ERR_SETTINGS_ERROR:
      ਵਾਪਸ "SETTINGS_ERROR";
    ਹਾਲਤ CURL_H3_ERR_MISSING_SETTINGS:
      ਵਾਪਸ "MISSING_SETTINGS";
    ਹਾਲਤ CURL_H3_ERR_REQUEST_REJECTED:
      ਵਾਪਸ "REQUEST_REJECTED";
    ਹਾਲਤ CURL_H3_ERR_REQUEST_CANCELLED:
      ਵਾਪਸ "REQUEST_CANCELLED";
    ਹਾਲਤ CURL_H3_ERR_REQUEST_INCOMPLETE:
      ਵਾਪਸ "REQUEST_INCOMPLETE";
    ਹਾਲਤ CURL_H3_ERR_ਸੁਨੇਹਾ_ERROR:
      ਵਾਪਸ "MESSAGE_ERROR";
    ਹਾਲਤ CURL_H3_ERR_CONNECT_ERROR:
      ਵਾਪਸ "CONNECT_ERROR";
    ਹਾਲਤ CURL_H3_ERR_ਸੰਸਕਰਣ_FALLBACK:
      ਵਾਪਸ "VERSION_FALLBACK";
    ਮੂਲ:
      ਰੋਕੋ;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  ਜੇ((error_ਕੋਡ >= 0x21) && !((error_ਕੋਡ - 0x21) % 0x1f))
    ਵਾਪਸ "NO_ERROR";
  ਵਾਪਸ "unknown";
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

#ਜੇ defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ਸਥਿਰ ਖਾਲੀ *vquic_ngtcp2_malloc(size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਵਾਪਸ Curl_cmalloc(ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ vquic_ngtcp2_free(ਖਾਲੀ *ptr, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  Curl_cfree(ptr);
}

ਸਥਿਰ ਖਾਲੀ *vquic_ngtcp2_calloc(size_t nmemb, size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਵਾਪਸ Curl_ccalloc(nmemb, ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ *vquic_ngtcp2_realloc(ਖਾਲੀ *ptr, size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਵਾਪਸ Curl_crealloc(ptr, ਮਾਪ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGTCP2
ਸਥਿਰ ਬਣਤਰ ngtcp2_mem vquic_ngtcp2_mem = {
  ਨਿਰਾ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ਬਣਤਰ ngtcp2_mem *Curl_ngtcp2_mem(ਖਾਲੀ)
{
  ਵਾਪਸ &vquic_ngtcp2_mem;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP3
ਸਥਿਰ ਬਣਤਰ nghttp3_mem vquic_nghttp3_mem = {
  ਨਿਰਾ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ਬਣਤਰ nghttp3_mem *Curl_nghttp3_mem(ਖਾਲੀ)
{
  ਵਾਪਸ &vquic_nghttp3_mem;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* USE_NGTCP2 || USE_NGHTTP3 */

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;
  DEBUGF(infof(ਅੰਕੜੇ, "QUIC is not supported in this build"));
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_HTTP3 */
