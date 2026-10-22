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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IP_H
#ಅನ್ತರ್ಭಾವ <netinet/ip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP3
#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ಅನ್ತರ್ಭಾವ <sys/syscall.h>
#ಆದರೆ defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_APPLE_ಸಂದೇಶ_X
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

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


#ವ್ಯಾಖ್ಯಾನಿಸು NW_CHUNK_ಗಾತ್ರ     (64 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು NW_SEND_CHUNKS    1

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_APPLE_ಸಂದೇಶ_X

/* this is `struct msghdr` with an additional field at the end */
ರಚನೆ msghdr_x {
  ಶೂನ್ಯ *ಸಂದೇಶ_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ರಚನೆ iovec *msg_iov;    /* scatter/gather array */
  ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_iovlen;           /* # elements in msg_iov */
  ಶೂನ್ಯ *ಸಂದೇಶ_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_flags;            /* flags on received message */
  size_t ಸಂದೇಶ_datalen;       /* byte length of buffer in msg_iov */
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_APPLE_ಸಂದೇಶ_X
#ವ್ಯಾಖ್ಯಾನಿಸು VQUIC_SEND_ವಿಧಾನ   "sendmsg_x"
#ಅಥವಾಆದರೆ defined(HAVE_SENDMSG)
#ವ್ಯಾಖ್ಯಾನಿಸು VQUIC_SEND_ವಿಧಾನ   "sendmsg"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು VQUIC_SEND_ವಿಧಾನ   "send"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ Curl_vquic_init(ಶೂನ್ಯ)
{
#ಆದರೆ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  ಆದರೆ(ngtcp2_crypto_ossl_init())
    ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ Curl_vquic_cleanup(ಶೂನ್ಯ)
{
#ಆದರೆ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x011800)
  ngtcp2_crypto_ossl_free();
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_quic_ver(ಅಕ್ಷರ *p, size_t len)
{
#ಆದರೆ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#ಅಥವಾಆದರೆ defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#ಆದರೆ_ಅಂತ್ಯ
}

CURLcode Curl_vquic_ctx_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_ಗಾತ್ರ, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
#ಆದರೆ defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#ಅಥವಾಆದರೆ defined(HAVE_APPLE_ಸಂದೇಶ_X)
  qctx->no_gso = FALSE;
#ಇಲ್ಲದಿದ್ದರೆ
  qctx->no_gso = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_DBG_QUIC_WBLOCK");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, 100))
        qctx->wblock_percent = (ಪೂರ್ಣಾಂಕ)l;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  Curl_vquic_ctx_ಗಣ_ಸಮಯ(qctx, Curl_pgrs_now(ದತ್ತಾಂಶ));

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_vquic_ctx_free(ರಚನೆ cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

ಶೂನ್ಯ Curl_vquic_ctx_ಗಣ_ಸಮಯ(ರಚನೆ cf_quic_ctx *qctx,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  qctx->ಕೊನೆಯ_op = *pnow;
}

ಶೂನ್ಯ Curl_vquic_ctx_update_ಸಮಯ(ರಚನೆ cf_quic_ctx *qctx,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  qctx->ಕೊನೆಯ_op = *pnow;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_APPLE_ಸಂದೇಶ_X
ಸ್ಥಿರ CURLcode do_sendmsg(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ cf_quic_ctx *qctx,
                           ಸ್ಥಿರಾಂಕ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_X_SNUM  64
  ರಚನೆ iovec msg_iov[ಸಂದೇಶ_X_SNUM];
  ರಚನೆ msghdr_x mmsg[ಸಂದೇಶ_X_SNUM];
  ಅಕ್ಷರ errstr[STRERROR_LEN];
  size_t n, i = 0, ಕಳುಹಿಸಿದ = 0;
  ಪೂರ್ಣಾಂಕ rc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  ಆದರೆ(!pktlen)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  ಆಗ(i < n) {
    size_t j, batch = CURLMIN(n - i, ಸಂದೇಶ_X_SNUM), pkts_ಕಳುಹಿಸಿದ = 0;

    ಗಾಗಿ(j = 0; j < batch; ++j) {
      ಸ್ಥಿರಾಂಕ size_t ಸ್ಥಾನಾಂತರ = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + ಸ್ಥಾನಾಂತರ);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - ಸ್ಥಾನಾಂತರ);
      memset(&mmsg[j], 0, ಗಾತ್ರ(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].ಸಂದೇಶ_iovlen = 1;
      mmsg[j].ಸಂದೇಶ_datalen = msg_iov[j].iov_len;
    }

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ
    ಆಗ((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(rc < 0) {
      ಆದರೆ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "egress, sendmsg_x -> EAGAIN");
        ಫಲಿತಾಂಶ = ಕಳುಹಿಸಿದ ? CURLE_OK : CURLE_AGAIN;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, ಗಾತ್ರ(errstr));
        failf(ದತ್ತಾಂಶ, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_ಕಳುಹಿಸಿದ = batch;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      pkts_ಕಳುಹಿಸಿದ = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    ಆದರೆ(!pkts_ಕಳುಹಿಸಿದ) {  /* no packets of the current batch were sent */
      ಫಲಿತಾಂಶ = ಕಳುಹಿಸಿದ ? CURLE_OK : CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    i += pkts_ಕಳುಹಿಸಿದ; /* Some have been sent */
    ಗಾಗಿ(j = 0; j < pkts_ಕಳುಹಿಸಿದ; ++j)
      ಕಳುಹಿಸಿದ += msg_iov[j].iov_len;
    ಆದರೆ(pkts_ಕಳುಹಿಸಿದ < batch)
      ಹೋಗು ಹೊರಹೋಗು; /* but not all of them */
  }

ಹೊರಹೋಗು:
  *psent = ಕಳುಹಿಸಿದ;
  ಆದರೆ(ಕಳುಹಿಸಿದ || ಫಲಿತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                ಕಳುಹಿಸಿದ, gsolen, n, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_APPLE_MSG_X */

ಸ್ಥಿರ CURLcode send_packet_no_gso(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ cf_quic_ctx *qctx,
                                   ಸ್ಥಿರಾಂಕ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ಸ್ಥಿರ CURLcode do_sendmsg(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ cf_quic_ctx *qctx,
                           ಸ್ಥಿರಾಂಕ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SENDMSG
  ರಚನೆ iovec msg_iov;
  ರಚನೆ msghdr ಸಂದೇಶ = { 0 };
  ssize_t rv;
#ಆದರೆ defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t ಸಂದೇಶ_ctrl[32];
  ರಚನೆ cmsghdr *cm;
#ಆದರೆ_ಅಂತ್ಯ

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  ಸಂದೇಶ.msg_iov = &msg_iov;
  ಸಂದೇಶ.ಸಂದೇಶ_iovlen = 1;

#ಆದರೆ defined(__linux__) && defined(UDP_SEGMENT)
  ಆದರೆ(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(ಸಂದೇಶ_ctrl, 0, ಗಾತ್ರ(ಸಂದೇಶ_ctrl));
    ಸಂದೇಶ.ಸಂದೇಶ_control = ಸಂದೇಶ_ctrl;
    DEBUGASSERT(ಗಾತ್ರ(ಸಂದೇಶ_ctrl) >= CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)));
    ಸಂದೇಶ.msg_controllen = CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
    cm = CMSG_FIRSTHDR(&ಸಂದೇಶ);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(ಗಾತ್ರ(uint16_t));
    *(uint16_t *)(ಶೂನ್ಯ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆಗ((rv = sendmsg(qctx->sockfd, &ಸಂದೇಶ, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  ಆದರೆ(!curlx_sztouz(rv, psent)) {
    ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
    ಆದರೆ(SOCK_EAGAIN(sockerr))
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಆಯ್ಕೆ(sockerr) {
    ಸ್ಥಿತಿ SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ EIO:
      ಆದರೆ(pktlen > gsolen) {
        /* GSO failure */
        infof(ದತ್ತಾಂಶ, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ಹಿಂತಿರುಗಿ send_packet_no_gso(cf, ದತ್ತಾಂಶ, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pktlen != *psent) {
    failf(ದತ್ತಾಂಶ, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ssize_t rv;
  (ಶೂನ್ಯ)gsolen;

  *psent = 0;

  ಆಗ((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  ಆದರೆ(!curlx_sztouz(rv, psent)) {
    ಆದರೆ(SOCK_EAGAIN(SOCKERRNO)) {
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(SOCKERRNO != SOCKEMSGSIZE) {
        failf(ದತ್ತಾಂಶ, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  (ಶೂನ್ಯ)cf;

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_ವಿಧಾನ, pktlen, gsolen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *psent);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_APPLE_MSG_X */

ಸ್ಥಿರ CURLcode send_packet_no_gso(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ cf_quic_ctx *qctx,
                                   ಸ್ಥಿರಾಂಕ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ಸ್ಥಿರಾಂಕ uint8_t *p, *ಕೊನೆ = pkt + pktlen;
  size_t ಕಳುಹಿಸಿದ, len;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  ಗಾಗಿ(p = pkt; p < ಕೊನೆ; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(ಕೊನೆ - p));
    ಫಲಿತಾಂಶ = do_sendmsg(cf, ದತ್ತಾಂಶ, qctx, p, len, len, &ಕಳುಹಿಸಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    *psent += ಕಳುಹಿಸಿದ;
    VERBOSE(++calls);
  }
ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_ವಿಧಾನ, pktlen, gsolen, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *psent);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ಸ್ಥಿರ CURLcode send_packet_no_gso_cf(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ಸ್ಥಿರಾಂಕ uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ಸ್ಥಿರಾಂಕ uint8_t *p, *ಕೊನೆ = pkt + pktlen;
  size_t ಕಳುಹಿಸಿದ, len;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  ಗಾಗಿ(p = pkt; p < ಕೊನೆ; p += len) {
    len = CURLMIN(gsolen, (size_t)(ಕೊನೆ - p));
    ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, p, len, FALSE, &ಕಳುಹಿಸಿದ);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಆದರೆ(ಕಳುಹಿಸಿದ != len) {
      /* We can only send the complete datagram, not parts. */
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    *psent += ಕಳುಹಿಸಿದ;
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *psent);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode vquic_send_packets(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ cf_quic_ctx *qctx,
                                   ಸ್ಥಿರಾಂಕ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  /* simulate network blocking/partial writes */
  ಆದರೆ(qctx->wblock_percent > 0) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c;
    *psent = 0;
    Curl_rand(ದತ್ತಾಂಶ, &c, 1);
    ಆದರೆ(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "vquic_flush() simulate EWOULDBLOCK");
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(qctx->no_gso && pktlen > gsolen) {
    ಫಲಿತಾಂಶ = send_packet_no_gso(cf, ದತ್ತಾಂಶ, qctx, pkt, pktlen, gsolen, psent);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = do_sendmsg(cf, ದತ್ತಾಂಶ, qctx, pkt, pktlen, gsolen, psent);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    qctx->ಕೊನೆಯ_io = qctx->ಕೊನೆಯ_op;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_vquic_flush(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ cf_quic_ctx *qctx)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
  size_t blen, ಕಳುಹಿಸಿದ;
  CURLcode ಫಲಿತಾಂಶ;
  size_t gsolen;

  ಆಗ(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    ಆದರೆ(qctx->ವಿಭಜಿಸು_len) {
      gsolen = qctx->ವಿಭಜಿಸು_gsolen;
      ಆದರೆ(blen > qctx->ವಿಭಜಿಸು_len)
        blen = qctx->ವಿಭಜಿಸು_len;
    }

    ಆದರೆ(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      ಫಲಿತಾಂಶ = vquic_send_packets(cf, ದತ್ತಾಂಶ, qctx, buf, blen, gsolen, &ಕಳುಹಿಸಿದ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      ಆದರೆ(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        ಫಲಿತಾಂಶ = send_packet_no_gso_cf(cf, ದತ್ತಾಂಶ, buf, blen, gsolen, &ಕಳುಹಿಸಿದ);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, blen, FALSE, &ಕಳುಹಿಸಿದ);
      }
    }

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, ಕಳುಹಿಸಿದ);
        ಆದರೆ(qctx->ವಿಭಜಿಸು_len)
          qctx->ವಿಭಜಿಸು_len -= ಕಳುಹಿಸಿದ;
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    Curl_bufq_skip(&qctx->sendbuf, ಕಳುಹಿಸಿದ);
    ಆದರೆ(qctx->ವಿಭಜಿಸು_len)
      qctx->ವಿಭಜಿಸು_len -= ಕಳುಹಿಸಿದ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_vquic_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ಹಿಂತಿರುಗಿ Curl_vquic_flush(cf, ದತ್ತಾಂಶ, qctx);
}

CURLcode Curl_vquic_send_ಬಾಲ_ವಿಭಜಿಸು(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ cf_quic_ctx *qctx, size_t gsolen,
                                    size_t ಬಾಲ_len, size_t ಬಾಲ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > ಬಾಲ_len);
  qctx->ವಿಭಜಿಸು_len = Curl_bufq_len(&qctx->sendbuf) - ಬಾಲ_len;
  qctx->ವಿಭಜಿಸು_gsolen = gsolen;
  qctx->gsolen = ಬಾಲ_gsolen;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->ವಿಭಜಿಸು_len, qctx->ವಿಭಜಿಸು_gsolen, ಬಾಲ_len, qctx->gsolen);
  ಹಿಂತಿರುಗಿ Curl_vquic_flush(cf, ದತ್ತಾಂಶ, qctx);
}

#ಆದರೆ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_ಸಂದೇಶ_X)
ಸ್ಥಿರ size_t vquic_msghdr_get_udp_gro(ರಚನೆ msghdr *ಸಂದೇಶ)
{
  ಪೂರ್ಣಾಂಕ gso_ಗಾತ್ರ = 0;
#ಆದರೆ defined(__linux__) && defined(UDP_GRO)
  ರಚನೆ cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#ಆದರೆ defined(__clang__) && !defined(__GLIBC__)
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wsign-compare"
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-align"
#ಆದರೆ_ಅಂತ್ಯ
  ಗಾಗಿ(cmsg = CMSG_FIRSTHDR(ಸಂದೇಶ); cmsg; cmsg = CMSG_NXTHDR(ಸಂದೇಶ, cmsg)) {
#ಆದರೆ defined(__clang__) && !defined(__GLIBC__)
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      ಸ್ಮರಣೆ_ನಕಲು(&gso_ಗಾತ್ರ, CMSG_DATA(cmsg), ಗಾತ್ರ(gso_ಗಾತ್ರ));

      ನಿಲ್ಲಿಸು;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* linux && UDP_GRO */
  (ಶೂನ್ಯ)ಸಂದೇಶ;

  ಹಿಂತಿರುಗಿ (size_t)gso_ಗಾತ್ರ;
}
#ಆದರೆ_ಅಂತ್ಯ /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#ಆದರೆ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_ಸಂದೇಶ_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_ಮುಖವಾಡ)
ಸ್ಥಿರ uint8_t vquic_msghdr_get_ecn(ರಚನೆ msghdr *ಸಂದೇಶ, ಪೂರ್ಣಾಂಕ family)
{
  ರಚನೆ cmsghdr *cmsg;
  ಆಯ್ಕೆ(family) {
  ಸ್ಥಿತಿ AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#ಆದರೆ defined(__clang__) && !defined(__GLIBC__)
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wsign-compare"
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-align"
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ(cmsg = CMSG_FIRSTHDR(ಸಂದೇಶ); cmsg; cmsg = CMSG_NXTHDR(ಸಂದೇಶ, cmsg)) {
#ಆದರೆ defined(__clang__) && !defined(__GLIBC__)
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(cmsg->cmsg_level == IPPROTO_IP &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#ಇಲ್ಲದಿದ್ದರೆ
          cmsg->cmsg_type == IP_TOS
#ಆದರೆ_ಅಂತ್ಯ
          && cmsg->cmsg_len) {
        ಹಿಂತಿರುಗಿ *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_ಮುಖವಾಡ;
      }
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#ಆದರೆ defined(__clang__) && !defined(__GLIBC__)
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wsign-compare"
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-align"
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ(cmsg = CMSG_FIRSTHDR(ಸಂದೇಶ); cmsg; cmsg = CMSG_NXTHDR(ಸಂದೇಶ, cmsg)) {
#ಆದರೆ defined(__clang__) && !defined(__GLIBC__)
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ tos;

        ಸ್ಮರಣೆ_ನಕಲು(&tos, CMSG_DATA(cmsg), ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));

        ಹಿಂತಿರುಗಿ (uint8_t)(tos & IPTOS_ECN_ಮುಖವಾಡ);
      }
    }
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು vquic_msghdr_get_ecn(a, b)      0
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SENDMMSG

ಸ್ಥಿರ CURLcode recvmmsg_packets(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp,
                                 size_t *pnread)
{
#ಆದರೆ defined(__linux__) && defined(UDP_GRO)
#ವ್ಯಾಖ್ಯಾನಿಸು MMSG_ಅಂಕೆ  16
#ವ್ಯಾಖ್ಯಾನಿಸು UDP_GRO_CNT_MAX  64
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ    (2 * CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು MMSG_ಅಂಕೆ  64
#ವ್ಯಾಖ್ಯಾನಿಸು UDP_GRO_CNT_MAX  1
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ    CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_BUF_ಗಾತ್ರ  (UDP_GRO_CNT_MAX * 1500)
  ರಚನೆ iovec msg_iov[MMSG_ಅಂಕೆ];
  ರಚನೆ mmsghdr mmsg[MMSG_ಅಂಕೆ];
  uint8_t ಸಂದೇಶ_ctrl[MMSG_ಅಂಕೆ * CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ];
  ರಚನೆ sockaddr_storage remote_addr[MMSG_ಅಂಕೆ];
  size_t ಒಟ್ಟು_nread = 0, pkts = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  size_t calls = 0;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ mcount, i, n;
  ಅಕ್ಷರ errstr[STRERROR_LEN];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t gso_ಗಾತ್ರ;
  ಅಕ್ಷರ *sockbuf = ಖಾಲಿ;
  uint8_t (*bufs)[ಸಂದೇಶ_BUF_ಗಾತ್ರ] = ಖಾಲಿ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ಫಲಿತಾಂಶ = Curl_multi_xfer_sockbuf_borrow(ದತ್ತಾಂಶ, MMSG_ಅಂಕೆ * ಸಂದೇಶ_BUF_ಗಾತ್ರ,
                                          &sockbuf);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  bufs = (uint8_t (*)[ಸಂದೇಶ_BUF_ಗಾತ್ರ])sockbuf;

  ಒಟ್ಟು_nread = 0;
  ಆಗ(pkts < max_pkts) {
    n = (ಪೂರ್ಣಾಂಕ)CURLMIN(CURLMIN(MMSG_ಅಂಕೆ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ಗಾತ್ರ(mmsg));
    ಗಾಗಿ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ಗಾತ್ರ(bufs[i]);
      mmsg[i].ಸಂದೇಶ_hdr.msg_iov = &msg_iov[i];
      mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_iovlen = 1;
      mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_name = &remote_addr[i];
      mmsg[i].ಸಂದೇಶ_hdr.msg_namelen = ಗಾತ್ರ(remote_addr[i]);
      mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_control = &ಸಂದೇಶ_ctrl[i * CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ];
      mmsg[i].ಸಂದೇಶ_hdr.msg_controllen = CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ;
    }

    ಆಗ((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ಖಾಲಿ)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ಆದರೆ(mcount == -1) {
      ಆದರೆ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ingress, recvmmsg -> EAGAIN");
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ರಚನೆ ip_quadruple ip;
        ಆದರೆ(!Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಖಾಲಿ, ಖಾಲಿ, &ip))
          failf(ದತ್ತಾಂಶ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ಪೋರ್ಟ್);
        ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      curlx_strerror(SOCKERRNO, errstr, ಗಾತ್ರ(errstr));
      failf(ದತ್ತಾಂಶ, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    VERBOSE(++calls);
    ಗಾಗಿ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ಆದರೆ(!mmsg[i].ಸಂದೇಶ_len) {
        ++pkts;
        ಮುಂದುವರಿ;
      }
      ಒಟ್ಟು_nread += mmsg[i].ಸಂದೇಶ_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].ಸಂದೇಶ_hdr, remote_addr[i].ss_family);
      gso_ಗಾತ್ರ = vquic_msghdr_get_udp_gro(&mmsg[i].ಸಂದೇಶ_hdr);
      ಆದರೆ(gso_ಗಾತ್ರ == 0)
        gso_ಗಾತ್ರ = mmsg[i].ಸಂದೇಶ_len;

      ಫಲಿತಾಂಶ = recv_cb(bufs[i], mmsg[i].ಸಂದೇಶ_len, gso_ಗಾತ್ರ,
                       mmsg[i].ಸಂದೇಶ_hdr.ಸಂದೇಶ_name,
                       mmsg[i].ಸಂದೇಶ_hdr.msg_namelen, ecn, userp);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      pkts += (mmsg[i].ಸಂದೇಶ_len + gso_ಗಾತ್ರ - 1) / gso_ಗಾತ್ರ;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಒಟ್ಟು_nread || ಫಲಿತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ಒಟ್ಟು_nread, pkts, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  Curl_multi_xfer_sockbuf_release(ದತ್ತಾಂಶ, sockbuf);
  *pnread = ಒಟ್ಟು_nread;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಅಥವಾಆದರೆ defined(HAVE_APPLE_ಸಂದೇಶ_X)

ಸ್ಥಿರ CURLcode recvmsg_x_packets(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  ಶೂನ್ಯ *userp, size_t *pnread)
{
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_X_ಅಂಕೆ  64
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂದೇಶ_BUF_ಗಾತ್ರ  (2048)
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ     CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))
  ರಚನೆ iovec msg_iov[ಸಂದೇಶ_X_ಅಂಕೆ];
  ರಚನೆ msghdr_x mmsg[ಸಂದೇಶ_X_ಅಂಕೆ];
  uint8_t ಸಂದೇಶ_ctrl[ಸಂದೇಶ_X_ಅಂಕೆ * CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ];
  ರಚನೆ sockaddr_storage remote_addr[ಸಂದೇಶ_X_ಅಂಕೆ];
  size_t ಒಟ್ಟು_nread = 0, pkts = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  size_t calls = 0;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ mcount, i;
  ಅಕ್ಷರ errstr[STRERROR_LEN];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t gso_ಗಾತ್ರ;
  ಅಕ್ಷರ *sockbuf = ಖಾಲಿ;
  uint8_t (*bufs)[ಸಂದೇಶ_BUF_ಗಾತ್ರ] = ಖಾಲಿ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ಫಲಿತಾಂಶ = Curl_multi_xfer_sockbuf_borrow(ದತ್ತಾಂಶ, ಸಂದೇಶ_X_ಅಂಕೆ * ಸಂದೇಶ_BUF_ಗಾತ್ರ,
                                          &sockbuf);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  bufs = (uint8_t (*)[ಸಂದೇಶ_BUF_ಗಾತ್ರ])sockbuf;

  ಒಟ್ಟು_nread = 0;
  ಆಗ(pkts < max_pkts) {
    ಪೂರ್ಣಾಂಕ n = (ಪೂರ್ಣಾಂಕ)CURLMIN(CURLMIN(ಸಂದೇಶ_X_ಅಂಕೆ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, ಗಾತ್ರ(mmsg));
    ಗಾಗಿ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = ಗಾತ್ರ(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].ಸಂದೇಶ_iovlen = 1;
      mmsg[i].ಸಂದೇಶ_name = &remote_addr[i];
      mmsg[i].msg_namelen = ಗಾತ್ರ(remote_addr[i]);
      mmsg[i].ಸಂದೇಶ_control = &ಸಂದೇಶ_ctrl[i * CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ];
      mmsg[i].msg_controllen = CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ;
    }

#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
#ಆದರೆ_ಅಂತ್ಯ
    ಆಗ((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#ಆದರೆ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(mcount == -1) {
      ಆದರೆ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ingress, recvmsg_x -> EAGAIN");
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ರಚನೆ ip_quadruple ip;
        ಆದರೆ(!Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಖಾಲಿ, ಖಾಲಿ, &ip))
          failf(ದತ್ತಾಂಶ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ಪೋರ್ಟ್);
        ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      curlx_strerror(SOCKERRNO, errstr, ಗಾತ್ರ(errstr));
      failf(ದತ್ತಾಂಶ, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    VERBOSE(++calls);
    ಗಾಗಿ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      ಆದರೆ(!mmsg[i].ಸಂದೇಶ_datalen) {
        ++pkts;
        ಮುಂದುವರಿ;
      }
      ಒಟ್ಟು_nread += mmsg[i].ಸಂದೇಶ_datalen;
      ecn = vquic_msghdr_get_ecn((ರಚನೆ msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_ಗಾತ್ರ = mmsg[i].ಸಂದೇಶ_datalen;

      ಫಲಿತಾಂಶ = recv_cb(bufs[i], mmsg[i].ಸಂದೇಶ_datalen, gso_ಗಾತ್ರ,
                       mmsg[i].ಸಂದೇಶ_name,
                       mmsg[i].msg_namelen, ecn, userp);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      pkts += (mmsg[i].ಸಂದೇಶ_datalen + gso_ಗಾತ್ರ - 1) / gso_ಗಾತ್ರ;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಒಟ್ಟು_nread || ಫಲಿತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                ಒಟ್ಟು_nread, pkts, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  Curl_multi_xfer_sockbuf_release(ದತ್ತಾಂಶ, sockbuf);
  *pnread = ಒಟ್ಟು_nread;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಅಥವಾಆದರೆ defined(HAVE_SENDMSG)
ಸ್ಥಿರ CURLcode recvmsg_packets(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp,
                                size_t *pnread)
{
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ    CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))
  ರಚನೆ iovec msg_iov;
  ರಚನೆ msghdr ಸಂದೇಶ;
  uint8_t buf[64 * 1024];
  ರಚನೆ sockaddr_storage remote_addr;
  size_t ಒಟ್ಟು_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  ಅಕ್ಷರ errstr[STRERROR_LEN];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uint8_t ಸಂದೇಶ_ctrl[CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ];
  size_t gso_ಗಾತ್ರ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ಗಾಗಿ(pkts = 0, ಒಟ್ಟು_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&ಸಂದೇಶ, 0, ಗಾತ್ರ(ಸಂದೇಶ));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = ಗಾತ್ರ(buf);
    ಸಂದೇಶ.msg_iov = &msg_iov;
    ಸಂದೇಶ.ಸಂದೇಶ_iovlen = 1;
    ಸಂದೇಶ.ಸಂದೇಶ_control = ಸಂದೇಶ_ctrl;
    ಸಂದೇಶ.ಸಂದೇಶ_name = &remote_addr;
    ಸಂದೇಶ.msg_namelen = ಗಾತ್ರ(remote_addr);
    ಸಂದೇಶ.msg_controllen = CMSG_ಪ್ರತಿ_ಸಂದೇಶ_ಗಾತ್ರ;

    ಆಗ((rc = recvmsg(qctx->sockfd, &ಸಂದೇಶ, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ಆದರೆ(!curlx_sztouz(rc, &nread)) {
      ಆದರೆ(SOCK_EAGAIN(SOCKERRNO)) {
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ರಚನೆ ip_quadruple ip;
        ಆದರೆ(!Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಖಾಲಿ, ಖಾಲಿ, &ip))
          failf(ದತ್ತಾಂಶ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ಪೋರ್ಟ್);
        ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      curlx_strerror(SOCKERRNO, errstr, ಗಾತ್ರ(errstr));
      failf(ದತ್ತಾಂಶ, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಒಟ್ಟು_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ಆದರೆ(!nread) {
      ++pkts;
      ಮುಂದುವರಿ;
    }

    ecn = vquic_msghdr_get_ecn(&ಸಂದೇಶ, remote_addr.ss_family);
    gso_ಗಾತ್ರ = vquic_msghdr_get_udp_gro(&ಸಂದೇಶ);
    ಆದರೆ(gso_ಗಾತ್ರ == 0)
      gso_ಗಾತ್ರ = nread;

    ಫಲಿತಾಂಶ = recv_cb(buf, nread, gso_ಗಾತ್ರ,
                     ಸಂದೇಶ.ಸಂದೇಶ_name, ಸಂದೇಶ.msg_namelen, ecn, userp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    pkts += (nread + gso_ಗಾತ್ರ - 1) / gso_ಗಾತ್ರ;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಒಟ್ಟು_nread || ಫಲಿತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ಒಟ್ಟು_nread, pkts, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  *pnread = ಒಟ್ಟು_nread;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_SENDMMSG || HAVE_SENDMSG */
ಸ್ಥಿರ CURLcode recvfrom_packets(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  ಪೂರ್ಣಾಂಕ bufsize = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf);
  ರಚನೆ sockaddr_storage remote_addr;
  socklen_t remote_addrlen = ಗಾತ್ರ(remote_addr);
  size_t ಒಟ್ಟು_nread, pkts, calls = 0, nread;
  ssize_t rv;
  ಅಕ್ಷರ errstr[STRERROR_LEN];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  ಗಾಗಿ(pkts = 0, ಒಟ್ಟು_nread = 0; pkts < max_pkts;) {
    ಆಗ((rv = recvfrom(qctx->sockfd, (ಅಕ್ಷರ *)buf, bufsize, 0,
                         (ರಚನೆ sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    ಆದರೆ(!curlx_sztouz(rv, &nread)) {
      ಆದರೆ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ingress, recvfrom -> EAGAIN");
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ರಚನೆ ip_quadruple ip;
        ಆದರೆ(!Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಖಾಲಿ, ಖಾಲಿ, &ip))
          failf(ದತ್ತಾಂಶ, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_ಪೋರ್ಟ್);
        ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      curlx_strerror(SOCKERRNO, errstr, ಗಾತ್ರ(errstr));
      failf(ದತ್ತಾಂಶ, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    ಆದರೆ(!nread)
      ಮುಂದುವರಿ;

    ಒಟ್ಟು_nread += nread;
    ಫಲಿತಾಂಶ = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಒಟ್ಟು_nread || ಫಲಿತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                ಒಟ್ಟು_nread, pkts, calls, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  *pnread = ಒಟ್ಟು_nread;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ಶೂನ್ಯ *userp)
{
  CURLcode ಫಲಿತಾಂಶ;
  size_t nread = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SENDMMSG
  ಫಲಿತಾಂಶ = recvmmsg_packets(cf, ದತ್ತಾಂಶ, qctx, max_pkts, recv_cb, userp, &nread);
#ಅಥವಾಆದರೆ defined(HAVE_APPLE_ಸಂದೇಶ_X)
  ಫಲಿತಾಂಶ = recvmsg_x_packets(cf, ದತ್ತಾಂಶ, qctx, max_pkts, recv_cb, userp,
                             &nread);
#ಅಥವಾಆದರೆ defined(HAVE_SENDMSG)
  ಫಲಿತಾಂಶ = recvmsg_packets(cf, ದತ್ತಾಂಶ, qctx, max_pkts, recv_cb, userp, &nread);
#ಇಲ್ಲದಿದ್ದರೆ
  ಫಲಿತಾಂಶ = recvfrom_packets(cf, ದತ್ತಾಂಶ, qctx, max_pkts, recv_cb, userp, &nread);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    /* EAGAIN is no error, only count actual packets as a reply */
    ಆದರೆ(!qctx->got_ಮೊದಲ_byte && nread) {
      qctx->got_ಮೊದಲ_byte = TRUE;
      qctx->ಮೊದಲ_byte_at = qctx->ಕೊನೆಯ_op;
    }
    qctx->ಕೊನೆಯ_io = qctx->ಕೊನೆಯ_op;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *scid,
                      size_t scidlen,
                      ಪೂರ್ಣಾಂಕ *qlogfdp)
{
  ಅಕ್ಷರ *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  ಆದರೆ(qlog_dir) {
    ರಚನೆ dynbuf fname;
    CURLcode ಫಲಿತಾಂಶ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&fname, qlog_dir);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&fname, "/");
    ಗಾಗಿ(i = 0; (i < scidlen) && !ಫಲಿತಾಂಶ; i++) {
      ಅಕ್ಷರ hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&fname, hex);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&fname, ".sqlog");

    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಪೂರ್ಣಾಂಕ qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ದ್ವಿಮಾನ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
                              ದತ್ತಾಂಶ->ಗಣ.new_file_perms & (_S_IREAD | _S_IWRITE)
#ಇಲ್ಲದಿದ್ದರೆ
                              (mode_t)ದತ್ತಾಂಶ->ಗಣ.new_file_perms
#ಆದರೆ_ಅಂತ್ಯ
                              );
      ಆದರೆ(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cf_quic_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ Curl_peer *origin,
                                   ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ; /* not used in all cases and compilers are stupid */
#ಆದರೆ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಸೇರಿಸು_after(cf_at, origin, peer, &cf);
#ಅಥವಾಆದರೆ defined(USE_QUICHE)
  ಫಲಿತಾಂಶ = Curl_cf_quiche_ಸೇರಿಸು_after(cf_at, origin, peer, &cf);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf)
  (ಶೂನ್ಯ)cf_at;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)peer;
  ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  ಆದರೆ(!ಫಲಿತಾಂಶ && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    ಫಲಿತಾಂಶ = Curl_conn_dns_ಕೂಡಿಸು_https_resolve(ದತ್ತಾಂಶ, cf->conn,
                                             cf->sockindex, origin);
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR && USE_ECH */
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_quic_create(ರಚನೆ Curl_cfilter **pcf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_peer *origin,
                             ರಚನೆ Curl_peer *peer,
                             uint8_t transport_peer,
                             ರಚನೆ connectdata *conn,
                             ರಚನೆ Curl_sockaddr_ex *addr,
                             ರಚನೆ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (ಶೂನ್ಯ)transport_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  (ಶೂನ್ಯ)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ಆದರೆ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_create(pcf, ದತ್ತಾಂಶ, origin, peer, conn, addr);
#ಅಥವಾಆದರೆ defined(USE_QUICHE)
  ಹಿಂತಿರುಗಿ Curl_cf_quiche_create(pcf, ದತ್ತಾಂಶ, origin, peer, conn, addr);
#ಇಲ್ಲದಿದ್ದರೆ
  *pcf = ಖಾಲಿ;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)addr;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_peer *origin,
                                       ರಚನೆ Curl_peer *peer,
                                       ರಚನೆ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#ಆದರೆ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_proxy_ಸೇರಿಸು_after(cf_at, ದತ್ತಾಂಶ, origin, peer,
                                           tunnel_peer, tunnel_transport);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf_at;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

CURLcode Curl_cf_h3_proxy_create(ರಚನೆ Curl_cfilter **pcf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_peer *origin,
                                 ರಚನೆ Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ರಚನೆ connectdata *conn,
                                 ರಚನೆ Curl_sockaddr_ex *addr,
                                 ರಚನೆ Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#ಆದರೆ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_proxy_create(pcf, ದತ್ತಾಂಶ, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#ಇಲ್ಲದಿದ್ದರೆ
  *pcf = ಖಾಲಿ;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)addr;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)transport_peer;
  (ಶೂನ್ಯ)tunnel_peer;
  (ಶೂನ್ಯ)tunnel_transport;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn)
{
  ಆದರೆ(Curl_conn_get_ಮೊದಲ_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(ದತ್ತಾಂಶ, "HTTP/3 cannot be used over UNIX domain sockets");
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }
  ಆದರೆ(!(ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(ದತ್ತಾಂಶ, "HTTP/3 requested for non-HTTPS URL");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(conn->socks_proxy.peer) {
    failf(ದತ್ತಾಂಶ, "HTTP/3 is not supported over a SOCKS proxy");
    ಹಿಂತಿರುಗಿ CURLE_URL_MALFORMAT;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)conn;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_vquic_h3_err_str(uint64_t error_ಸಂಕೇತ)
{
  ಆದರೆ(error_ಸಂಕೇತ <= UINT_MAX) {
    ಆಯ್ಕೆ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)error_ಸಂಕೇತ) {
    ಸ್ಥಿತಿ CURL_H3_ERR_NO_ERROR:
      ಹಿಂತಿರುಗಿ "NO_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ಹಿಂತಿರುಗಿ "GENERAL_PROTOCOL_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_ಆಂತರಿಕ_ERROR:
      ಹಿಂತಿರುಗಿ "INTERNAL_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_ಹರಿವು_CREATION_ERROR:
      ಹಿಂತಿರುಗಿ "STREAM_CREATION_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_CLOSED_CRITICAL_ಹರಿವು:
      ಹಿಂತಿರುಗಿ "CLOSED_CRITICAL_STREAM";
    ಸ್ಥಿತಿ CURL_H3_ERR_ಚೌಕಟ್ಟು_UNEXPECTED:
      ಹಿಂತಿರುಗಿ "FRAME_UNEXPECTED";
    ಸ್ಥಿತಿ CURL_H3_ERR_ಚೌಕಟ್ಟು_ERROR:
      ಹಿಂತಿರುಗಿ "FRAME_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_EXCESSIVE_ತುಂಬು:
      ಹಿಂತಿರುಗಿ "EXCESSIVE_LOAD";
    ಸ್ಥಿತಿ CURL_H3_ERR_ID_ERROR:
      ಹಿಂತಿರುಗಿ "ID_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_SETTINGS_ERROR:
      ಹಿಂತಿರುಗಿ "SETTINGS_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_MISSING_SETTINGS:
      ಹಿಂತಿರುಗಿ "MISSING_SETTINGS";
    ಸ್ಥಿತಿ CURL_H3_ERR_REQUEST_REJECTED:
      ಹಿಂತಿರುಗಿ "REQUEST_REJECTED";
    ಸ್ಥಿತಿ CURL_H3_ERR_REQUEST_CANCELLED:
      ಹಿಂತಿರುಗಿ "REQUEST_CANCELLED";
    ಸ್ಥಿತಿ CURL_H3_ERR_REQUEST_INCOMPLETE:
      ಹಿಂತಿರುಗಿ "REQUEST_INCOMPLETE";
    ಸ್ಥಿತಿ CURL_H3_ERR_ಸಂದೇಶ_ERROR:
      ಹಿಂತಿರುಗಿ "MESSAGE_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_CONNECT_ERROR:
      ಹಿಂತಿರುಗಿ "CONNECT_ERROR";
    ಸ್ಥಿತಿ CURL_H3_ERR_ಆವೃತ್ತಿ_FALLBACK:
      ಹಿಂತಿರುಗಿ "VERSION_FALLBACK";
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  ಆದರೆ((error_ಸಂಕೇತ >= 0x21) && !((error_ಸಂಕೇತ - 0x21) % 0x1f))
    ಹಿಂತಿರುಗಿ "NO_ERROR";
  ಹಿಂತಿರುಗಿ "unknown";
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

#ಆದರೆ defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ಸ್ಥಿರ ಶೂನ್ಯ *vquic_ngtcp2_malloc(size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ Curl_cmalloc(ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ vquic_ngtcp2_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  Curl_cfree(ptr);
}

ಸ್ಥಿರ ಶೂನ್ಯ *vquic_ngtcp2_calloc(size_t nmemb, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ Curl_ccalloc(nmemb, ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ *vquic_ngtcp2_realloc(ಶೂನ್ಯ *ptr, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ Curl_crealloc(ptr, ಗಾತ್ರ_2);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGTCP2
ಸ್ಥಿರ ರಚನೆ ngtcp2_mem vquic_ngtcp2_mem = {
  ಖಾಲಿ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ರಚನೆ ngtcp2_mem *Curl_ngtcp2_mem(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ &vquic_ngtcp2_mem;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP3
ಸ್ಥಿರ ರಚನೆ nghttp3_mem vquic_nghttp3_mem = {
  ಖಾಲಿ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ರಚನೆ nghttp3_mem *Curl_nghttp3_mem(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ &vquic_nghttp3_mem;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* USE_NGTCP2 || USE_NGHTTP3 */

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;
  DEBUGF(infof(ದತ್ತಾಂಶ, "QUIC is not supported in this build"));
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_HTTP3 */
