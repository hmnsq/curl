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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "vquic/vquic.h"
#ضمن "vtls/vtls.h"

#ضمن "curl_trc.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#ضمن <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#ضمن <nghttp3/nghttp3.h>
#endif

#إذا defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ضمن <sys/syscall.h>
#إذا defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_پیغام_X
#endif
#endif

#ضمن "bufq.h"
#ضمن "curlx/dynbuf.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "connect.h"
#ضمن "cfilters.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vquic/cf-ngtcp2.h"
#ضمن "vquic/cf-ngtcp2-cmn.h"
#ضمن "vquic/cf-ngtcp2-proxy.h"
#ضمن "vquic/cf-quiche.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "rand.h"
#ضمن "vquic/vquic_int.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_پیغام_X

/* this is `struct msghdr` with an additional field at the end */
بنية msghdr_x {
  فراغ *پیغام_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  بنية iovec *msg_iov;    /* scatter/gather array */
  صحيح پیغام_iovlen;           /* # elements in msg_iov */
  فراغ *پیغام_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  صحيح پیغام_flags;            /* flags on received message */
  size_t پیغام_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_پیغام_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#وإلاإذا defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#وإلا
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

صحيح Curl_vquic_init(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_init())
    أرجع 0;
#endif

  أرجع 1;
}

فراغ Curl_vquic_cleanup(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_عدد >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

فراغ Curl_quic_ver(حرف *p, size_t len)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#وإلاإذا defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(بنية Curl_easy *data,
                             بنية cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#إذا defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#وإلاإذا defined(HAVE_APPLE_پیغام_X)
  qctx->no_gso = FALSE;
#وإلا
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_QUIC_WBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, 100))
        qctx->wblock_percent = (صحيح)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  أرجع CURLE_OK;
}

فراغ Curl_vquic_ctx_free(بنية cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

فراغ Curl_vquic_ctx_set_time(بنية cf_quic_ctx *qctx,
                             ثابت بنية curltime *pnow)
{
  qctx->last_op = *pnow;
}

فراغ Curl_vquic_ctx_update_time(بنية cf_quic_ctx *qctx,
                                ثابت بنية curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_پیغام_X
ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define پیغام_X_SNUM  64
  بنية iovec msg_iov[پیغام_X_SNUM];
  بنية msghdr_x mmsg[پیغام_X_SNUM];
  حرف errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  صحيح rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  إذا(!pktlen)
    أرجع CURLE_OK;
  إذا(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  طالما(i < n) {
    size_t j, batch = CURLMIN(n - i, پیغام_X_SNUM), pkts_sent = 0;

    لكل(j = 0; j < batch; ++j) {
      ثابت size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, حجم(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].پیغام_iovlen = 1;
      mmsg[j].پیغام_datalen = msg_iov[j].iov_len;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ڈالو
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    طالما((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic نکالو
#endif

    إذا(rc < 0) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        اذهب out;
      }
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        اذهب out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    وإلا
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    إذا(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      اذهب out;
    }
    i += pkts_sent; /* Some have been sent */
    لكل(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    إذا(pkts_sent < batch)
      اذهب out; /* but not all of them */
  }

out:
  *psent = sent;
  إذا(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (صحيح)result);
  أرجع result;
}

#وإلا /* HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  بنية iovec msg_iov;
  بنية msghdr پیغام = { 0 };
  ssize_t rv;
#إذا defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t پیغام_ctrl[32];
  بنية cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  پیغام.msg_iov = &msg_iov;
  پیغام.پیغام_iovlen = 1;

#إذا defined(__linux__) && defined(UDP_SEGMENT)
  إذا(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(پیغام_ctrl, 0, حجم(پیغام_ctrl));
    پیغام.پیغام_control = پیغام_ctrl;
    DEBUGASSERT(حجم(پیغام_ctrl) >= CMSG_SPACE(حجم(صحيح)));
    پیغام.msg_controllen = CMSG_SPACE(حجم(صحيح));
    cm = CMSG_FIRSTHDR(&پیغام);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(حجم(uint16_t));
    *(uint16_t *)(فراغ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  طالما((rv = sendmsg(qctx->sockfd, &پیغام, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  إذا(!curlx_sztouz(rv, psent)) {
    صحيح sockerr = SOCKERRNO;
    إذا(SOCK_EAGAIN(sockerr))
      أرجع CURLE_AGAIN;
    بدل(sockerr) {
    حالة SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      اكسر;
    حالة EIO:
      إذا(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        أرجع send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    افتراضي:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
  }
  وإلا إذا(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
#وإلا
  ssize_t rv;
  (فراغ)gsolen;

  *psent = 0;

  طالما((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  إذا(!curlx_sztouz(rv, psent)) {
    إذا(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      اذهب out;
    }
    وإلا {
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        اذهب out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (فراغ)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (صحيح)result, *psent);
  أرجع result;
}

#endif /* !HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  لكل(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    إذا(result)
      اذهب out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (صحيح)result, *psent);
  أرجع result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ساكن CURLcode send_packet_no_gso_cf(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      ثابت uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  لكل(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    إذا(result)
      اذهب out;

    إذا(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (صحيح)result, *psent);
  أرجع result;
}

ساكن CURLcode vquic_send_packets(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  إذا(qctx->wblock_percent > 0) {
    unsigned حرف c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    إذا(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      أرجع CURLE_AGAIN;
    }
  }
#endif
  إذا(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  وإلا {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  إذا(!result)
    qctx->last_io = qctx->last_op;
  أرجع result;
}

CURLcode Curl_vquic_flush(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                          بنية cf_quic_ctx *qctx)
{
  ثابت unsigned حرف *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  طالما(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    إذا(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      إذا(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    إذا(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    وإلا {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      إذا(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      وإلا {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    إذا(result) {
      إذا(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        إذا(qctx->split_len)
          qctx->split_len -= sent;
      }
      أرجع result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    إذا(qctx->split_len)
      qctx->split_len -= sent;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_vquic_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                         بنية cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  أرجع Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    بنية cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  أرجع Curl_vquic_flush(cf, data, qctx);
}

#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_پیغام_X)
ساكن size_t vquic_msghdr_get_udp_gro(بنية msghdr *پیغام)
{
  صحيح gso_size = 0;
#إذا defined(__linux__) && defined(UDP_GRO)
  بنية cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ڈالو
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  لكل(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic نکالو
#endif
    إذا(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), حجم(gso_size));

      اكسر;
    }
  }
#endif /* linux && UDP_GRO */
  (فراغ)پیغام;

  أرجع (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_پیغام_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
ساكن uint8_t vquic_msghdr_get_ecn(بنية msghdr *پیغام, صحيح family)
{
  بنية cmsghdr *cmsg;
  بدل(family) {
  حالة AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ڈالو
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    لكل(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic نکالو
#endif
      إذا(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#وإلا
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        أرجع *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    اكسر;
  حالة AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ڈالو
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    لكل(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic نکالو
#endif
      إذا(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned صحيح tos;

        memcpy(&tos, CMSG_DATA(cmsg), حجم(صحيح));

        أرجع (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    اكسر;
  }
  أرجع 0;
}
#وإلا
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

ساكن CURLcode recvmmsg_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
#إذا defined(__linux__) && defined(UDP_GRO)
#define MMSG_عدد  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_پیغام_SIZE    (2 * CMSG_SPACE(حجم(صحيح)))
#وإلا
#define MMSG_عدد  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_پیغام_SIZE    CMSG_SPACE(حجم(صحيح))
#endif
#define پیغام_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  بنية iovec msg_iov[MMSG_عدد];
  بنية mmsghdr mmsg[MMSG_عدد];
  uint8_t پیغام_ctrl[MMSG_عدد * CMSG_PER_پیغام_SIZE];
  بنية sockaddr_storage remote_addr[MMSG_عدد];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  صحيح mcount, i, n;
  حرف errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[پیغام_BUF_SIZE] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_عدد * پیغام_BUF_SIZE,
                                          &sockbuf);
  إذا(result)
    اذهب out;
  bufs = (uint8_t (*)[پیغام_BUF_SIZE])sockbuf;

  total_nread = 0;
  طالما(pkts < max_pkts) {
    n = (صحيح)CURLMIN(CURLMIN(MMSG_عدد, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    لكل(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].پیغام_hdr.msg_iov = &msg_iov[i];
      mmsg[i].پیغام_hdr.پیغام_iovlen = 1;
      mmsg[i].پیغام_hdr.پیغام_name = &remote_addr[i];
      mmsg[i].پیغام_hdr.msg_namelen = حجم(remote_addr[i]);
      mmsg[i].پیغام_hdr.پیغام_control = &پیغام_ctrl[i * CMSG_PER_پیغام_SIZE];
      mmsg[i].پیغام_hdr.msg_controllen = CMSG_PER_پیغام_SIZE;
    }

    طالما((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, عدم)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        اذهب out;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->next, data, عدم, عدم, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        اذهب out;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      اذهب out;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].پیغام_len) {
        ++pkts;
        استمر;
      }
      total_nread += mmsg[i].پیغام_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].پیغام_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].پیغام_hdr);
      إذا(gso_size == 0)
        gso_size = mmsg[i].پیغام_len;

      result = recv_cb(bufs[i], mmsg[i].پیغام_len, gso_size,
                       mmsg[i].پیغام_hdr.پیغام_name,
                       mmsg[i].پیغام_hdr.msg_namelen, ecn, userp);
      إذا(result)
        اذهب out;
      pkts += (mmsg[i].پیغام_len + gso_size - 1) / gso_size;
    }
  }

out:
  إذا(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحيح)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  أرجع result;
}

#وإلاإذا defined(HAVE_APPLE_پیغام_X)

ساكن CURLcode recvmsg_x_packets(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  فراغ *userp, size_t *pnread)
{
#define پیغام_X_عدد  64
#define پیغام_BUF_SIZE  (2048)
#define CMSG_PER_پیغام_SIZE     CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov[پیغام_X_عدد];
  بنية msghdr_x mmsg[پیغام_X_عدد];
  uint8_t پیغام_ctrl[پیغام_X_عدد * CMSG_PER_پیغام_SIZE];
  بنية sockaddr_storage remote_addr[پیغام_X_عدد];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  صحيح mcount, i;
  حرف errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[پیغام_BUF_SIZE] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, پیغام_X_عدد * پیغام_BUF_SIZE,
                                          &sockbuf);
  إذا(result)
    اذهب out;
  bufs = (uint8_t (*)[پیغام_BUF_SIZE])sockbuf;

  total_nread = 0;
  طالما(pkts < max_pkts) {
    صحيح n = (صحيح)CURLMIN(CURLMIN(پیغام_X_عدد, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    لكل(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].پیغام_iovlen = 1;
      mmsg[i].پیغام_name = &remote_addr[i];
      mmsg[i].msg_namelen = حجم(remote_addr[i]);
      mmsg[i].پیغام_control = &پیغام_ctrl[i * CMSG_PER_پیغام_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_پیغام_SIZE;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ڈالو
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    طالما((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic نکالو
#endif
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        اذهب out;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->next, data, عدم, عدم, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        اذهب out;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      اذهب out;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].پیغام_datalen) {
        ++pkts;
        استمر;
      }
      total_nread += mmsg[i].پیغام_datalen;
      ecn = vquic_msghdr_get_ecn((بنية msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].پیغام_datalen;

      result = recv_cb(bufs[i], mmsg[i].پیغام_datalen, gso_size,
                       mmsg[i].پیغام_name,
                       mmsg[i].msg_namelen, ecn, userp);
      إذا(result)
        اذهب out;
      pkts += (mmsg[i].پیغام_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  إذا(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحيح)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  أرجع result;
}

#وإلاإذا defined(HAVE_SENDMSG)
ساكن CURLcode recvmsg_packets(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                بنية cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                size_t *pnread)
{
#define CMSG_PER_پیغام_SIZE    CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov;
  بنية msghdr پیغام;
  uint8_t buf[64 * 1024];
  بنية sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  حرف errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t پیغام_ctrl[CMSG_PER_پیغام_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&پیغام, 0, حجم(پیغام));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = حجم(buf);
    پیغام.msg_iov = &msg_iov;
    پیغام.پیغام_iovlen = 1;
    پیغام.پیغام_control = پیغام_ctrl;
    پیغام.پیغام_name = &remote_addr;
    پیغام.msg_namelen = حجم(remote_addr);
    پیغام.msg_controllen = CMSG_PER_پیغام_SIZE;

    طالما((rc = recvmsg(qctx->sockfd, &پیغام, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rc, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        اذهب out;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->next, data, عدم, عدم, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        اذهب out;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      اذهب out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread) {
      ++pkts;
      استمر;
    }

    ecn = vquic_msghdr_get_ecn(&پیغام, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&پیغام);
    إذا(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     پیغام.پیغام_name, پیغام.msg_namelen, ecn, userp);
    إذا(result)
      اذهب out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  إذا(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحيح)result);
  *pnread = total_nread;
  أرجع result;
}

#وإلا /* HAVE_SENDMMSG || HAVE_SENDMSG */
ساكن CURLcode recvfrom_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  صحيح bufsize = (صحيح)حجم(buf);
  بنية sockaddr_storage remote_addr;
  socklen_t remote_addrlen = حجم(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  حرف errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    طالما((rv = recvfrom(qctx->sockfd, (حرف *)buf, bufsize, 0,
                         (بنية sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        اذهب out;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->next, data, عدم, عدم, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        اذهب out;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      اذهب out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread)
      استمر;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    إذا(result)
      اذهب out;
  }

out:
  إذا(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحيح)result);
  *pnread = total_nread;
  أرجع result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#وإلاإذا defined(HAVE_APPLE_پیغام_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#وإلاإذا defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#وإلا
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  إذا(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    إذا(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  أرجع result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(بنية Curl_easy *data,
                      unsigned حرف *scid,
                      size_t scidlen,
                      صحيح *qlogfdp)
{
  حرف *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  إذا(qlog_dir) {
    بنية dynbuf fname;
    CURLcode result;
    unsigned صحيح i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    إذا(!result)
      result = curlx_dyn_add(&fname, "/");
    لكل(i = 0; (i < scidlen) && !result; i++) {
      حرف hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    إذا(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    إذا(!result) {
      صحيح qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ثنائی,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                              (mode_t)data->set.new_file_perms
#endif
                              );
      إذا(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    إذا(result)
      أرجع result;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_cf_quic_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                   بنية Curl_easy *data,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  (فراغ)data; /* not used in all cases and compilers are stupid */
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_داخل_کرو_after(cf_at, origin, peer, &cf);
#وإلاإذا defined(USE_QUICHE)
  result = Curl_cf_quiche_داخل_کرو_after(cf_at, origin, peer, &cf);
#وإلا
  (فراغ)cf)
  (فراغ)cf_at;
  (فراغ)origin;
  (فراغ)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#إذا defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  إذا(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  أرجع result;
}

CURLcode Curl_cf_quic_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *data,
                             بنية Curl_peer *origin,
                             بنية Curl_peer *peer,
                             uint8_t transport_peer,
                             بنية connectdata *conn,
                             بنية Curl_sockaddr_ex *addr,
                             بنية Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (فراغ)transport_peer;
  (فراغ)tunnel_transport;
  (فراغ)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  أرجع Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#وإلاإذا defined(USE_QUICHE)
  أرجع Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#وإلا
  *pcf = عدم;
  (فراغ)data;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)conn;
  (فراغ)addr;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_IN;
#endif
}

#إذا !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *data,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  أرجع Curl_cf_ngtcp2_proxy_داخل_کرو_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#وإلا
  (فراغ)cf_at;
  (فراغ)data;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(بنية Curl_cfilter **pcf,
                                 بنية Curl_easy *data,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 uint8_t transport_peer,
                                 بنية connectdata *conn,
                                 بنية Curl_sockaddr_ex *addr,
                                 بنية Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  أرجع Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#وإلا
  *pcf = عدم;
  (فراغ)data;
  (فراغ)conn;
  (فراغ)addr;
  (فراغ)peer;
  (فراغ)transport_peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *data,
                             بنية connectdata *conn)
{
  إذا(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  إذا(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    أرجع CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    أرجع CURLE_URL_MALFORMAT;
  }
#وإلا
  (فراغ)conn;
#endif

  أرجع CURLE_OK;
}

#ifdef CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_code)
{
  إذا(error_code <= UINT_MAX) {
    بدل((unsigned صحيح)error_code) {
    حالة CURL_H3_ERR_NO_ERROR:
      أرجع "NO_ERROR";
    حالة CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      أرجع "GENERAL_PROTOCOL_ERROR";
    حالة CURL_H3_ERR_INTERNAL_ERROR:
      أرجع "INTERNAL_ERROR";
    حالة CURL_H3_ERR_STREAM_CREATION_ERROR:
      أرجع "STREAM_CREATION_ERROR";
    حالة CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      أرجع "CLOSED_CRITICAL_STREAM";
    حالة CURL_H3_ERR_FRAME_UNEXPECTED:
      أرجع "FRAME_UNEXPECTED";
    حالة CURL_H3_ERR_FRAME_ERROR:
      أرجع "FRAME_ERROR";
    حالة CURL_H3_ERR_EXCESSIVE_LOAD:
      أرجع "EXCESSIVE_LOAD";
    حالة CURL_H3_ERR_ID_ERROR:
      أرجع "ID_ERROR";
    حالة CURL_H3_ERR_SETTINGS_ERROR:
      أرجع "SETTINGS_ERROR";
    حالة CURL_H3_ERR_MISSING_SETTINGS:
      أرجع "MISSING_SETTINGS";
    حالة CURL_H3_ERR_REQUEST_REJECTED:
      أرجع "REQUEST_REJECTED";
    حالة CURL_H3_ERR_REQUEST_CANCELLED:
      أرجع "REQUEST_CANCELLED";
    حالة CURL_H3_ERR_REQUEST_INCOMPLETE:
      أرجع "REQUEST_INCOMPLETE";
    حالة CURL_H3_ERR_MESSAGE_ERROR:
      أرجع "MESSAGE_ERROR";
    حالة CURL_H3_ERR_CONNECT_ERROR:
      أرجع "CONNECT_ERROR";
    حالة CURL_H3_ERR_VERSION_FALLBACK:
      أرجع "VERSION_FALLBACK";
    افتراضي:
      اكسر;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  إذا((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    أرجع "NO_ERROR";
  أرجع "unknown";
}
#endif /* CURLVERBOSE */

#إذا defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ساكن فراغ *vquic_ngtcp2_malloc(size_t size, فراغ *user_data)
{
  (فراغ)user_data;
  أرجع Curl_cmalloc(size);
}

ساكن فراغ vquic_ngtcp2_free(فراغ *ptr, فراغ *user_data)
{
  (فراغ)user_data;
  Curl_cfree(ptr);
}

ساكن فراغ *vquic_ngtcp2_calloc(size_t nmemb, size_t size, فراغ *user_data)
{
  (فراغ)user_data;
  أرجع Curl_ccalloc(nmemb, size);
}

ساكن فراغ *vquic_ngtcp2_realloc(فراغ *ptr, size_t size, فراغ *user_data)
{
  (فراغ)user_data;
  أرجع Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
ساكن بنية ngtcp2_mem vquic_ngtcp2_mem = {
  عدم,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
بنية ngtcp2_mem *Curl_ngtcp2_mem(فراغ)
{
  أرجع &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
ساكن بنية nghttp3_mem vquic_nghttp3_mem = {
  عدم,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
بنية nghttp3_mem *Curl_nghttp3_mem(فراغ)
{
  أرجع &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#وإلا /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *data,
                             بنية connectdata *conn)
{
  (فراغ)data;
  (فراغ)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  أرجع CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
