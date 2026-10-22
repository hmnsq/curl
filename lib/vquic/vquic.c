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
#include "vquic/vquic.h"
#include "vtls/vtls.h"

#include "curl_trc.h"

#གལ་སྲིད !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#include <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#include <nghttp3/nghttp3.h>
#endif

#གལ་སྲིད defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#include <sys/syscall.h>
#གལ་སྲིད defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_MSG_X
#endif
#endif

#include "bufq.h"
#include "curlx/dynbuf.h"
#include "curlx/win32-fopen.h"
#include "connect.h"
#include "cfilters.h"
#include "vdns/cf-dns.h"
#include "vquic/cf-ngtcp2.h"
#include "vquic/cf-ngtcp2-cmn.h"
#include "vquic/cf-ngtcp2-proxy.h"
#include "vquic/cf-quiche.h"
#include "multiif.h"
#include "progress.h"
#include "rand.h"
#include "vquic/vquic_int.h"
#include "curlx/strerr.h"
#include "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_MSG_X

/* this is `struct msghdr` with an additional field at the end */
sa_nracanaa msghdr_x {
  void *msg_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  sa_nracanaa iovec *msg_iov;    /* scatter/gather array */
  ཧྲིལ msg_iovlen;           /* # elements in msg_iov */
  void *msg_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  ཧྲིལ msg_flags;            /* flags on received message */
  size_t msg_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_MSG_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#elif defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#དེ་མིན
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

ཧྲིལ Curl_vquic_init(void)
{
#གལ་སྲིད defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  གལ་སྲིད(ngtcp2_crypto_ossl_init())
    ལོག 0;
#endif

  ལོག 1;
}

void Curl_vquic_cleanup(void)
{
#གལ་སྲིད defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_NUM >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

void Curl_quic_ver(var_na *p, size_t len)
{
#གལ་སྲིད defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#elif defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(sa_nracanaa Curl_easy *data,
                             sa_nracanaa cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#གལ་སྲིད defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#elif defined(HAVE_APPLE_MSG_X)
  qctx->no_gso = FALSE;
#དེ་མིན
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_QUIC_WBLOCK");
    གལ་སྲིད(p) {
      curl_off_t l;
      གལ་སྲིད(!curlx_str_number(&p, &l, 100))
        qctx->wblock_percent = (ཧྲིལ)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  ལོག CURLE_OK;
}

void Curl_vquic_ctx_free(sa_nracanaa cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

void Curl_vquic_ctx_set_time(sa_nracanaa cf_quic_ctx *qctx,
                             const sa_nracanaa curltime *pnow)
{
  qctx->last_op = *pnow;
}

void Curl_vquic_ctx_update_time(sa_nracanaa cf_quic_ctx *qctx,
                                const sa_nracanaa curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_MSG_X
static CURLcode do_sendmsg(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa cf_quic_ctx *qctx,
                           const uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define MSG_X_SNUM  64
  sa_nracanaa iovec msg_iov[MSG_X_SNUM];
  sa_nracanaa msghdr_x mmsg[MSG_X_SNUM];
  var_na errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  ཧྲིལ rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  གལ་སྲིད(!pktlen)
    ལོག CURLE_OK;
  གལ་སྲིད(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  སྐབས(i < n) {
    size_t j, batch = CURLMIN(n - i, MSG_X_SNUM), pkts_sent = 0;

    དོན་ལུ(j = 0; j < batch; ++j) {
      const size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, parimaa_na(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].msg_iovlen = 1;
      mmsg[j].msg_datalen = msg_iov[j].iov_len;
    }

#གལ་སྲིད defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    སྐབས((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#གལ་སྲིད defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic pop
#endif

    གལ་སྲིད(rc < 0) {
      གལ་སྲིད(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        goto out;
      }
      གལ་སྲིད(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, parimaa_na(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        goto out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    དེ་མིན
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    གལ་སྲིད(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      goto out;
    }
    i += pkts_sent; /* Some have been sent */
    དོན་ལུ(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    གལ་སྲིད(pkts_sent < batch)
      goto out; /* but not all of them */
  }

out:
  *psent = sent;
  གལ་སྲིད(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (ཧྲིལ)result);
  ལོག result;
}

#དེ་མིན /* HAVE_APPLE_MSG_X */

static CURLcode send_packet_no_gso(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa cf_quic_ctx *qctx,
                                   const uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

static CURLcode do_sendmsg(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa cf_quic_ctx *qctx,
                           const uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  sa_nracanaa iovec msg_iov;
  sa_nracanaa msghdr msg = { 0 };
  ssize_t rv;
#གལ་སྲིད defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t msg_ctrl[32];
  sa_nracanaa cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  msg.msg_iov = &msg_iov;
  msg.msg_iovlen = 1;

#གལ་སྲིད defined(__linux__) && defined(UDP_SEGMENT)
  གལ་སྲིད(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(msg_ctrl, 0, parimaa_na(msg_ctrl));
    msg.msg_control = msg_ctrl;
    DEBUGASSERT(parimaa_na(msg_ctrl) >= CMSG_SPACE(parimaa_na(ཧྲིལ)));
    msg.msg_controllen = CMSG_SPACE(parimaa_na(ཧྲིལ));
    cm = CMSG_FIRSTHDR(&msg);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(parimaa_na(uint16_t));
    *(uint16_t *)(void *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  སྐབས((rv = sendmsg(qctx->sockfd, &msg, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  གལ་སྲིད(!curlx_sztouz(rv, psent)) {
    ཧྲིལ sockerr = SOCKERRNO;
    གལ་སྲིད(SOCK_EAGAIN(sockerr))
      ལོག CURLE_AGAIN;
    switch(sockerr) {
    pak_sa SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      bha_nga;
    pak_sa EIO:
      གལ་སྲིད(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ལོག send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    default:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }
  དེ་མིན གལ་སྲིད(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    goto out;
  }
#དེ་མིན
  ssize_t rv;
  (void)gsolen;

  *psent = 0;

  སྐབས((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  གལ་སྲིད(!curlx_sztouz(rv, psent)) {
    གལ་སྲིད(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      goto out;
    }
    དེ་མིན {
      གལ་སྲིད(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        goto out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (void)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (ཧྲིལ)result, *psent);
  ལོག result;
}

#endif /* !HAVE_APPLE_MSG_X */

static CURLcode send_packet_no_gso(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa cf_quic_ctx *qctx,
                                   const uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  const uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  དོན་ལུ(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    གལ་སྲིད(result)
      goto out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (ཧྲིལ)result, *psent);
  ལོག result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
static CURLcode send_packet_no_gso_cf(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      const uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  const uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  དོན་ལུ(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    གལ་སྲིད(result)
      goto out;

    གལ་སྲིད(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      goto out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (ཧྲིལ)result, *psent);
  ལོག result;
}

static CURLcode vquic_send_packets(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa cf_quic_ctx *qctx,
                                   const uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  གལ་སྲིད(qctx->wblock_percent > 0) {
    unsigned var_na c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    གལ་སྲིད(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      ལོག CURLE_AGAIN;
    }
  }
#endif
  གལ་སྲིད(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  དེ་མིན {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  གལ་སྲིད(!result)
    qctx->last_io = qctx->last_op;
  ལོག result;
}

CURLcode Curl_vquic_flush(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          sa_nracanaa cf_quic_ctx *qctx)
{
  const unsigned var_na *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  སྐབས(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    གལ་སྲིད(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      གལ་སྲིད(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    གལ་སྲིད(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    དེ་མིན {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      གལ་སྲིད(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      དེ་མིན {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    གལ་སྲིད(result) {
      གལ་སྲིད(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        གལ་སྲིད(qctx->split_len)
          qctx->split_len -= sent;
      }
      ལོག result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    གལ་སྲིད(qctx->split_len)
      qctx->split_len -= sent;
  }
  ལོག CURLE_OK;
}

CURLcode Curl_vquic_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                         sa_nracanaa cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ལོག Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  ལོག Curl_vquic_flush(cf, data, qctx);
}

#གལ་སྲིད (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_MSG_X)
static size_t vquic_msghdr_get_udp_gro(sa_nracanaa msghdr *msg)
{
  ཧྲིལ gso_size = 0;
#གལ་སྲིད defined(__linux__) && defined(UDP_GRO)
  sa_nracanaa cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#གལ་སྲིད defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  དོན་ལུ(cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
#གལ་སྲིད defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic pop
#endif
    གལ་སྲིད(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), parimaa_na(gso_size));

      bha_nga;
    }
  }
#endif /* linux && UDP_GRO */
  (void)msg;

  ལོག (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#གལ་སྲིད (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_MSG_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
static uint8_t vquic_msghdr_get_ecn(sa_nracanaa msghdr *msg, ཧྲིལ family)
{
  sa_nracanaa cmsghdr *cmsg;
  switch(family) {
  pak_sa AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#གལ་སྲིད defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    དོན་ལུ(cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
#གལ་སྲིད defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic pop
#endif
      གལ་སྲིད(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#དེ་མིན
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        ལོག *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    bha_nga;
  pak_sa AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#གལ་སྲིད defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    དོན་ལུ(cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
#གལ་སྲིད defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic pop
#endif
      གལ་སྲིད(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned ཧྲིལ tos;

        memcpy(&tos, CMSG_DATA(cmsg), parimaa_na(ཧྲིལ));

        ལོག (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    bha_nga;
  }
  ལོག 0;
}
#དེ་མིན
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

static CURLcode recvmmsg_packets(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, void *userp,
                                 size_t *pnread)
{
#གལ་སྲིད defined(__linux__) && defined(UDP_GRO)
#define MMSG_NUM  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_MSG_SIZE    (2 * CMSG_SPACE(parimaa_na(ཧྲིལ)))
#དེ་མིན
#define MMSG_NUM  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_MSG_SIZE    CMSG_SPACE(parimaa_na(ཧྲིལ))
#endif
#define MSG_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  sa_nracanaa iovec msg_iov[MMSG_NUM];
  sa_nracanaa mmsghdr mmsg[MMSG_NUM];
  uint8_t msg_ctrl[MMSG_NUM * CMSG_PER_MSG_SIZE];
  sa_nracanaa sockaddr_storage remote_addr[MMSG_NUM];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  ཧྲིལ mcount, i, n;
  var_na errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  var_na *sockbuf = NULL;
  uint8_t (*bufs)[MSG_BUF_SIZE] = NULL;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_NUM * MSG_BUF_SIZE,
                                          &sockbuf);
  གལ་སྲིད(result)
    goto out;
  bufs = (uint8_t (*)[MSG_BUF_SIZE])sockbuf;

  total_nread = 0;
  སྐབས(pkts < max_pkts) {
    n = (ཧྲིལ)CURLMIN(CURLMIN(MMSG_NUM, IOV_MAX), max_pkts);
    memset(&mmsg, 0, parimaa_na(mmsg));
    དོན་ལུ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = parimaa_na(bufs[i]);
      mmsg[i].msg_hdr.msg_iov = &msg_iov[i];
      mmsg[i].msg_hdr.msg_iovlen = 1;
      mmsg[i].msg_hdr.msg_name = &remote_addr[i];
      mmsg[i].msg_hdr.msg_namelen = parimaa_na(remote_addr[i]);
      mmsg[i].msg_hdr.msg_control = &msg_ctrl[i * CMSG_PER_MSG_SIZE];
      mmsg[i].msg_hdr.msg_controllen = CMSG_PER_MSG_SIZE;
    }

    སྐབས((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, NULL)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    གལ་སྲིད(mcount == -1) {
      གལ་སྲིད(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        goto out;
      }
      གལ་སྲིད(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        sa_nracanaa ip_quadruple ip;
        གལ་སྲིད(!Curl_cf_socket_peek(cf->next, data, NULL, NULL, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        goto out;
      }
      curlx_strerror(SOCKERRNO, errstr, parimaa_na(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      goto out;
    }

    VERBOSE(++calls);
    དོན་ལུ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      གལ་སྲིད(!mmsg[i].msg_len) {
        ++pkts;
        continue;
      }
      total_nread += mmsg[i].msg_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].msg_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].msg_hdr);
      གལ་སྲིད(gso_size == 0)
        gso_size = mmsg[i].msg_len;

      result = recv_cb(bufs[i], mmsg[i].msg_len, gso_size,
                       mmsg[i].msg_hdr.msg_name,
                       mmsg[i].msg_hdr.msg_namelen, ecn, userp);
      གལ་སྲིད(result)
        goto out;
      pkts += (mmsg[i].msg_len + gso_size - 1) / gso_size;
    }
  }

out:
  གལ་སྲིད(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ཧྲིལ)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ལོག result;
}

#elif defined(HAVE_APPLE_MSG_X)

static CURLcode recvmsg_x_packets(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  void *userp, size_t *pnread)
{
#define MSG_X_NUM  64
#define MSG_BUF_SIZE  (2048)
#define CMSG_PER_MSG_SIZE     CMSG_SPACE(parimaa_na(ཧྲིལ))
  sa_nracanaa iovec msg_iov[MSG_X_NUM];
  sa_nracanaa msghdr_x mmsg[MSG_X_NUM];
  uint8_t msg_ctrl[MSG_X_NUM * CMSG_PER_MSG_SIZE];
  sa_nracanaa sockaddr_storage remote_addr[MSG_X_NUM];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  ཧྲིལ mcount, i;
  var_na errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  var_na *sockbuf = NULL;
  uint8_t (*bufs)[MSG_BUF_SIZE] = NULL;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MSG_X_NUM * MSG_BUF_SIZE,
                                          &sockbuf);
  གལ་སྲིད(result)
    goto out;
  bufs = (uint8_t (*)[MSG_BUF_SIZE])sockbuf;

  total_nread = 0;
  སྐབས(pkts < max_pkts) {
    ཧྲིལ n = (ཧྲིལ)CURLMIN(CURLMIN(MSG_X_NUM, IOV_MAX), max_pkts);
    memset(&mmsg, 0, parimaa_na(mmsg));
    དོན་ལུ(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = parimaa_na(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].msg_iovlen = 1;
      mmsg[i].msg_name = &remote_addr[i];
      mmsg[i].msg_namelen = parimaa_na(remote_addr[i]);
      mmsg[i].msg_control = &msg_ctrl[i * CMSG_PER_MSG_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_MSG_SIZE;
    }

#གལ་སྲིད defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    སྐབས((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#གལ་སྲིད defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic pop
#endif
    གལ་སྲིད(mcount == -1) {
      གལ་སྲིད(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        goto out;
      }
      གལ་སྲིད(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        sa_nracanaa ip_quadruple ip;
        གལ་སྲིད(!Curl_cf_socket_peek(cf->next, data, NULL, NULL, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        goto out;
      }
      curlx_strerror(SOCKERRNO, errstr, parimaa_na(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      goto out;
    }

    VERBOSE(++calls);
    དོན་ལུ(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      གལ་སྲིད(!mmsg[i].msg_datalen) {
        ++pkts;
        continue;
      }
      total_nread += mmsg[i].msg_datalen;
      ecn = vquic_msghdr_get_ecn((sa_nracanaa msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].msg_datalen;

      result = recv_cb(bufs[i], mmsg[i].msg_datalen, gso_size,
                       mmsg[i].msg_name,
                       mmsg[i].msg_namelen, ecn, userp);
      གལ་སྲིད(result)
        goto out;
      pkts += (mmsg[i].msg_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  གལ་སྲིད(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ཧྲིལ)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ལོག result;
}

#elif defined(HAVE_SENDMSG)
static CURLcode recvmsg_packets(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, void *userp,
                                size_t *pnread)
{
#define CMSG_PER_MSG_SIZE    CMSG_SPACE(parimaa_na(ཧྲིལ))
  sa_nracanaa iovec msg_iov;
  sa_nracanaa msghdr msg;
  uint8_t buf[64 * 1024];
  sa_nracanaa sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  var_na errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t msg_ctrl[CMSG_PER_MSG_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  དོན་ལུ(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&msg, 0, parimaa_na(msg));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = parimaa_na(buf);
    msg.msg_iov = &msg_iov;
    msg.msg_iovlen = 1;
    msg.msg_control = msg_ctrl;
    msg.msg_name = &remote_addr;
    msg.msg_namelen = parimaa_na(remote_addr);
    msg.msg_controllen = CMSG_PER_MSG_SIZE;

    སྐབས((rc = recvmsg(qctx->sockfd, &msg, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    གལ་སྲིད(!curlx_sztouz(rc, &nread)) {
      གལ་སྲིད(SOCK_EAGAIN(SOCKERRNO)) {
        goto out;
      }
      གལ་སྲིད(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        sa_nracanaa ip_quadruple ip;
        གལ་སྲིད(!Curl_cf_socket_peek(cf->next, data, NULL, NULL, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        goto out;
      }
      curlx_strerror(SOCKERRNO, errstr, parimaa_na(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      goto out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    གལ་སྲིད(!nread) {
      ++pkts;
      continue;
    }

    ecn = vquic_msghdr_get_ecn(&msg, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&msg);
    གལ་སྲིད(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     msg.msg_name, msg.msg_namelen, ecn, userp);
    གལ་སྲིད(result)
      goto out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  གལ་སྲིད(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ཧྲིལ)result);
  *pnread = total_nread;
  ལོག result;
}

#དེ་མིན /* HAVE_SENDMMSG || HAVE_SENDMSG */
static CURLcode recvfrom_packets(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, void *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  ཧྲིལ bufsize = (ཧྲིལ)parimaa_na(buf);
  sa_nracanaa sockaddr_storage remote_addr;
  socklen_t remote_addrlen = parimaa_na(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  var_na errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  དོན་ལུ(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    སྐབས((rv = recvfrom(qctx->sockfd, (var_na *)buf, bufsize, 0,
                         (sa_nracanaa sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    གལ་སྲིད(!curlx_sztouz(rv, &nread)) {
      གལ་སྲིད(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        goto out;
      }
      གལ་སྲིད(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        sa_nracanaa ip_quadruple ip;
        གལ་སྲིད(!Curl_cf_socket_peek(cf->next, data, NULL, NULL, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        goto out;
      }
      curlx_strerror(SOCKERRNO, errstr, parimaa_na(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      goto out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    གལ་སྲིད(!nread)
      continue;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    གལ་སྲིད(result)
      goto out;
  }

out:
  གལ་སྲིད(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (ཧྲིལ)result);
  *pnread = total_nread;
  ལོག result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, void *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#elif defined(HAVE_APPLE_MSG_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#elif defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#དེ་མིན
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  གལ་སྲིད(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    གལ་སྲིད(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  ལོག result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(sa_nracanaa Curl_easy *data,
                      unsigned var_na *scid,
                      size_t scidlen,
                      ཧྲིལ *qlogfdp)
{
  var_na *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  གལ་སྲིད(qlog_dir) {
    sa_nracanaa dynbuf fname;
    CURLcode result;
    unsigned ཧྲིལ i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    གལ་སྲིད(!result)
      result = curlx_dyn_add(&fname, "/");
    དོན་ལུ(i = 0; (i < scidlen) && !result; i++) {
      var_na hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    གལ་སྲིད(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    གལ་སྲིད(!result) {
      ཧྲིལ qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_BINARY,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#དེ་མིན
                              (mode_t)data->set.new_file_perms
#endif
                              );
      གལ་སྲིད(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    གལ་སྲིད(result)
      ལོག result;
  }

  ལོག CURLE_OK;
}

CURLcode Curl_cf_quic_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa Curl_peer *origin,
                                   sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  CURLcode result;

  (void)data; /* not used in all cases and compilers are stupid */
#གལ་སྲིད defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_insert_after(cf_at, origin, peer, &cf);
#elif defined(USE_QUICHE)
  result = Curl_cf_quiche_insert_after(cf_at, origin, peer, &cf);
#དེ་མིན
  (void)cf)
  (void)cf_at;
  (void)origin;
  (void)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#གལ་སྲིད defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  གལ་སྲིད(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  ལོག result;
}

CURLcode Curl_cf_quic_create(sa_nracanaa Curl_cfilter **pcf,
                             sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_peer *origin,
                             sa_nracanaa Curl_peer *peer,
                             uint8_t transport_peer,
                             sa_nracanaa connectdata *conn,
                             sa_nracanaa Curl_sockaddr_ex *addr,
                             sa_nracanaa Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (void)transport_peer;
  (void)tunnel_transport;
  (void)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#གལ་སྲིད defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ལོག Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#elif defined(USE_QUICHE)
  ལོག Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#དེ་མིན
  *pcf = NULL;
  (void)data;
  (void)origin;
  (void)peer;
  (void)conn;
  (void)addr;
  (void)tunnel_peer;
  (void)tunnel_transport;
  ལོག CURLE_NOT_BUILT_IN;
#endif
}

#གལ་སྲིད !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                       sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_peer *origin,
                                       sa_nracanaa Curl_peer *peer,
                                       sa_nracanaa Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#གལ་སྲིད defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ལོག Curl_cf_ngtcp2_proxy_insert_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#དེ་མིན
  (void)cf_at;
  (void)data;
  (void)origin;
  (void)peer;
  (void)tunnel_peer;
  (void)tunnel_transport;
  ལོག CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(sa_nracanaa Curl_cfilter **pcf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa Curl_peer *origin,
                                 sa_nracanaa Curl_peer *peer,
                                 uint8_t transport_peer,
                                 sa_nracanaa connectdata *conn,
                                 sa_nracanaa Curl_sockaddr_ex *addr,
                                 sa_nracanaa Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#གལ་སྲིད defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ལོག Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#དེ་མིན
  *pcf = NULL;
  (void)data;
  (void)conn;
  (void)addr;
  (void)peer;
  (void)transport_peer;
  (void)tunnel_peer;
  (void)tunnel_transport;
  ལོག CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(sa_nracanaa Curl_easy *data,
                             sa_nracanaa connectdata *conn)
{
  གལ་སྲིད(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    ལོག CURLE_QUIC_CONNECT_ERROR;
  }
  གལ་སྲིད(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    ལོག CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  གལ་སྲིད(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    ལོག CURLE_URL_MALFORMAT;
  }
#དེ་མིན
  (void)conn;
#endif

  ལོག CURLE_OK;
}

#ifdef CURLVERBOSE
const var_na *Curl_vquic_h3_err_str(uint64_t error_code)
{
  གལ་སྲིད(error_code <= UINT_MAX) {
    switch((unsigned ཧྲིལ)error_code) {
    pak_sa CURL_H3_ERR_NO_ERROR:
      ལོག "NO_ERROR";
    pak_sa CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ལོག "GENERAL_PROTOCOL_ERROR";
    pak_sa CURL_H3_ERR_INTERNAL_ERROR:
      ལོག "INTERNAL_ERROR";
    pak_sa CURL_H3_ERR_STREAM_CREATION_ERROR:
      ལོག "STREAM_CREATION_ERROR";
    pak_sa CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      ལོག "CLOSED_CRITICAL_STREAM";
    pak_sa CURL_H3_ERR_FRAME_UNEXPECTED:
      ལོག "FRAME_UNEXPECTED";
    pak_sa CURL_H3_ERR_FRAME_ERROR:
      ལོག "FRAME_ERROR";
    pak_sa CURL_H3_ERR_EXCESSIVE_LOAD:
      ལོག "EXCESSIVE_LOAD";
    pak_sa CURL_H3_ERR_ID_ERROR:
      ལོག "ID_ERROR";
    pak_sa CURL_H3_ERR_SETTINGS_ERROR:
      ལོག "SETTINGS_ERROR";
    pak_sa CURL_H3_ERR_MISSING_SETTINGS:
      ལོག "MISSING_SETTINGS";
    pak_sa CURL_H3_ERR_REQUEST_REJECTED:
      ལོག "REQUEST_REJECTED";
    pak_sa CURL_H3_ERR_REQUEST_CANCELLED:
      ལོག "REQUEST_CANCELLED";
    pak_sa CURL_H3_ERR_REQUEST_INCOMPLETE:
      ལོག "REQUEST_INCOMPLETE";
    pak_sa CURL_H3_ERR_MESSAGE_ERROR:
      ལོག "MESSAGE_ERROR";
    pak_sa CURL_H3_ERR_CONNECT_ERROR:
      ལོག "CONNECT_ERROR";
    pak_sa CURL_H3_ERR_VERSION_FALLBACK:
      ལོག "VERSION_FALLBACK";
    default:
      bha_nga;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  གལ་སྲིད((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    ལོག "NO_ERROR";
  ལོག "unknown";
}
#endif /* CURLVERBOSE */

#གལ་སྲིད defined(USE_NGTCP2) || defined(USE_NGHTTP3)

static void *vquic_ngtcp2_malloc(size_t size, void *user_data)
{
  (void)user_data;
  ལོག Curl_cmalloc(size);
}

static void vquic_ngtcp2_free(void *ptr, void *user_data)
{
  (void)user_data;
  Curl_cfree(ptr);
}

static void *vquic_ngtcp2_calloc(size_t nmemb, size_t size, void *user_data)
{
  (void)user_data;
  ལོག Curl_ccalloc(nmemb, size);
}

static void *vquic_ngtcp2_realloc(void *ptr, size_t size, void *user_data)
{
  (void)user_data;
  ལོག Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
static sa_nracanaa ngtcp2_mem vquic_ngtcp2_mem = {
  NULL,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
sa_nracanaa ngtcp2_mem *Curl_ngtcp2_mem(void)
{
  ལོག &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
static sa_nracanaa nghttp3_mem vquic_nghttp3_mem = {
  NULL,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
sa_nracanaa nghttp3_mem *Curl_nghttp3_mem(void)
{
  ལོག &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#དེ་མིན /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(sa_nracanaa Curl_easy *data,
                             sa_nracanaa connectdata *conn)
{
  (void)data;
  (void)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  ལོག CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
