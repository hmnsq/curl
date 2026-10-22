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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "vquic/vquic.h"
#شامل "vtls/vtls.h"

#شامل "curl_trc.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#شامل <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#شامل <nghttp3/nghttp3.h>
#endif

#اگر defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#شامل <sys/syscall.h>
#اگر defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_پیام_X
#endif
#endif

#شامل "bufq.h"
#شامل "curlx/dynbuf.h"
#شامل "curlx/win32-fopen.h"
#شامل "connect.h"
#شامل "cfilters.h"
#شامل "vdns/cf-dns.h"
#شامل "vquic/cf-ngtcp2.h"
#شامل "vquic/cf-ngtcp2-cmn.h"
#شامل "vquic/cf-ngtcp2-proxy.h"
#شامل "vquic/cf-quiche.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "rand.h"
#شامل "vquic/vquic_int.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_پیام_X

/* this is `struct msghdr` with an additional field at the end */
ساختار msghdr_x {
  تهی *پیام_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ساختار iovec *msg_iov;    /* scatter/gather array */
  صحیح پیام_iovlen;           /* # elements in msg_iov */
  تهی *پیام_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  صحیح پیام_flags;            /* flags on received message */
  size_t پیام_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_پیام_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#وگرنهاگر defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#وگرنه
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

صحیح Curl_vquic_init(تهی)
{
#اگر defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  اگر(ngtcp2_crypto_ossl_init())
    بازگشت 0;
#endif

  بازگشت 1;
}

تهی Curl_vquic_cleanup(تهی)
{
#اگر defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_رقم >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

تهی Curl_quic_ver(نویسه *p, size_t len)
{
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#وگرنهاگر defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(ساختار Curl_easy *data,
                             ساختار cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#اگر defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#وگرنهاگر defined(HAVE_APPLE_پیام_X)
  qctx->no_gso = FALSE;
#وگرنه
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ثابت نویسه *p = getenv("CURL_DBG_QUIC_WBLOCK");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, 100))
        qctx->wblock_percent = (صحیح)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  بازگشت CURLE_OK;
}

تهی Curl_vquic_ctx_free(ساختار cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

تهی Curl_vquic_ctx_set_time(ساختار cf_quic_ctx *qctx,
                             ثابت ساختار curltime *pnow)
{
  qctx->last_op = *pnow;
}

تهی Curl_vquic_ctx_update_time(ساختار cf_quic_ctx *qctx,
                                ثابت ساختار curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_پیام_X
ایستا CURLcode do_sendmsg(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           ساختار cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define پیام_X_SNUM  64
  ساختار iovec msg_iov[پیام_X_SNUM];
  ساختار msghdr_x mmsg[پیام_X_SNUM];
  نویسه errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  صحیح rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  اگر(!pktlen)
    بازگشت CURLE_OK;
  اگر(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  وقتی(i < n) {
    size_t j, batch = CURLMIN(n - i, پیام_X_SNUM), pkts_sent = 0;

    برای(j = 0; j < batch; ++j) {
      ثابت size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, اندازه(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].پیام_iovlen = 1;
      mmsg[j].پیام_datalen = msg_iov[j].iov_len;
    }

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    وقتی((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic برداشت
#endif

    اگر(rc < 0) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        برو out;
      }
      اگر(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, اندازه(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        برو out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    وگرنه
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    اگر(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      برو out;
    }
    i += pkts_sent; /* Some have been sent */
    برای(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    اگر(pkts_sent < batch)
      برو out; /* but not all of them */
  }

out:
  *psent = sent;
  اگر(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (صحیح)result);
  بازگشت result;
}

#وگرنه /* HAVE_APPLE_MSG_X */

ایستا CURLcode send_packet_no_gso(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ایستا CURLcode do_sendmsg(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           ساختار cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  ساختار iovec msg_iov;
  ساختار msghdr پیام = { 0 };
  ssize_t rv;
#اگر defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t پیام_ctrl[32];
  ساختار cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  پیام.msg_iov = &msg_iov;
  پیام.پیام_iovlen = 1;

#اگر defined(__linux__) && defined(UDP_SEGMENT)
  اگر(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(پیام_ctrl, 0, اندازه(پیام_ctrl));
    پیام.پیام_control = پیام_ctrl;
    DEBUGASSERT(اندازه(پیام_ctrl) >= CMSG_SPACE(اندازه(صحیح)));
    پیام.msg_controllen = CMSG_SPACE(اندازه(صحیح));
    cm = CMSG_FIRSTHDR(&پیام);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(اندازه(uint16_t));
    *(uint16_t *)(تهی *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  وقتی((rv = sendmsg(qctx->sockfd, &پیام, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  اگر(!curlx_sztouz(rv, psent)) {
    صحیح sockerr = SOCKERRNO;
    اگر(SOCK_EAGAIN(sockerr))
      بازگشت CURLE_AGAIN;
    انتخاب(sockerr) {
    حالت SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      بشکن;
    حالت EIO:
      اگر(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        بازگشت send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    پیشفرض:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }
  وگرنه اگر(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    برو out;
  }
#وگرنه
  ssize_t rv;
  (تهی)gsolen;

  *psent = 0;

  وقتی((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  اگر(!curlx_sztouz(rv, psent)) {
    اگر(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      برو out;
    }
    وگرنه {
      اگر(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        برو out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (تهی)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (صحیح)result, *psent);
  بازگشت result;
}

#endif /* !HAVE_APPLE_MSG_X */

ایستا CURLcode send_packet_no_gso(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  برای(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    اگر(result)
      برو out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (صحیح)result, *psent);
  بازگشت result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ایستا CURLcode send_packet_no_gso_cf(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ثابت uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  برای(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    اگر(result)
      برو out;

    اگر(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      برو out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (صحیح)result, *psent);
  بازگشت result;
}

ایستا CURLcode vquic_send_packets(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  اگر(qctx->wblock_percent > 0) {
    unsigned نویسه c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    اگر(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      بازگشت CURLE_AGAIN;
    }
  }
#endif
  اگر(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  وگرنه {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  اگر(!result)
    qctx->last_io = qctx->last_op;
  بازگشت result;
}

CURLcode Curl_vquic_flush(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          ساختار cf_quic_ctx *qctx)
{
  ثابت unsigned نویسه *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  وقتی(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    اگر(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      اگر(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    اگر(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    وگرنه {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      اگر(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      وگرنه {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    اگر(result) {
      اگر(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        اگر(qctx->split_len)
          qctx->split_len -= sent;
      }
      بازگشت result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    اگر(qctx->split_len)
      qctx->split_len -= sent;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_vquic_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                         ساختار cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  بازگشت Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  بازگشت Curl_vquic_flush(cf, data, qctx);
}

#اگر (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_پیام_X)
ایستا size_t vquic_msghdr_get_udp_gro(ساختار msghdr *پیام)
{
  صحیح gso_size = 0;
#اگر defined(__linux__) && defined(UDP_GRO)
  ساختار cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#اگر defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  برای(cmsg = CMSG_FIRSTHDR(پیام); cmsg; cmsg = CMSG_NXTHDR(پیام, cmsg)) {
#اگر defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic برداشت
#endif
    اگر(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), اندازه(gso_size));

      بشکن;
    }
  }
#endif /* linux && UDP_GRO */
  (تهی)پیام;

  بازگشت (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#اگر (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_پیام_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
ایستا uint8_t vquic_msghdr_get_ecn(ساختار msghdr *پیام, صحیح family)
{
  ساختار cmsghdr *cmsg;
  انتخاب(family) {
  حالت AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#اگر defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    برای(cmsg = CMSG_FIRSTHDR(پیام); cmsg; cmsg = CMSG_NXTHDR(پیام, cmsg)) {
#اگر defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic برداشت
#endif
      اگر(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#وگرنه
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        بازگشت *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    بشکن;
  حالت AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#اگر defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    برای(cmsg = CMSG_FIRSTHDR(پیام); cmsg; cmsg = CMSG_NXTHDR(پیام, cmsg)) {
#اگر defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic برداشت
#endif
      اگر(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned صحیح tos;

        memcpy(&tos, CMSG_DATA(cmsg), اندازه(صحیح));

        بازگشت (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    بشکن;
  }
  بازگشت 0;
}
#وگرنه
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

ایستا CURLcode recvmmsg_packets(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, تهی *userp,
                                 size_t *pnread)
{
#اگر defined(__linux__) && defined(UDP_GRO)
#define MMSG_رقم  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_پیام_SIZE    (2 * CMSG_SPACE(اندازه(صحیح)))
#وگرنه
#define MMSG_رقم  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_پیام_SIZE    CMSG_SPACE(اندازه(صحیح))
#endif
#define پیام_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  ساختار iovec msg_iov[MMSG_رقم];
  ساختار mmsghdr mmsg[MMSG_رقم];
  uint8_t پیام_ctrl[MMSG_رقم * CMSG_PER_پیام_SIZE];
  ساختار sockaddr_storage remote_addr[MMSG_رقم];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  صحیح mcount, i, n;
  نویسه errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  نویسه *sockbuf = پوچ;
  uint8_t (*bufs)[پیام_BUF_SIZE] = پوچ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_رقم * پیام_BUF_SIZE,
                                          &sockbuf);
  اگر(result)
    برو out;
  bufs = (uint8_t (*)[پیام_BUF_SIZE])sockbuf;

  total_nread = 0;
  وقتی(pkts < max_pkts) {
    n = (صحیح)CURLMIN(CURLMIN(MMSG_رقم, IOV_MAX), max_pkts);
    memset(&mmsg, 0, اندازه(mmsg));
    برای(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = اندازه(bufs[i]);
      mmsg[i].پیام_hdr.msg_iov = &msg_iov[i];
      mmsg[i].پیام_hdr.پیام_iovlen = 1;
      mmsg[i].پیام_hdr.پیام_name = &remote_addr[i];
      mmsg[i].پیام_hdr.msg_namelen = اندازه(remote_addr[i]);
      mmsg[i].پیام_hdr.پیام_control = &پیام_ctrl[i * CMSG_PER_پیام_SIZE];
      mmsg[i].پیام_hdr.msg_controllen = CMSG_PER_پیام_SIZE;
    }

    وقتی((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, پوچ)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    اگر(mcount == -1) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        برو out;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساختار ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->next, data, پوچ, پوچ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        برو out;
      }
      curlx_strerror(SOCKERRNO, errstr, اندازه(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      برو out;
    }

    VERBOSE(++calls);
    برای(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      اگر(!mmsg[i].پیام_len) {
        ++pkts;
        ادامه;
      }
      total_nread += mmsg[i].پیام_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].پیام_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].پیام_hdr);
      اگر(gso_size == 0)
        gso_size = mmsg[i].پیام_len;

      result = recv_cb(bufs[i], mmsg[i].پیام_len, gso_size,
                       mmsg[i].پیام_hdr.پیام_name,
                       mmsg[i].پیام_hdr.msg_namelen, ecn, userp);
      اگر(result)
        برو out;
      pkts += (mmsg[i].پیام_len + gso_size - 1) / gso_size;
    }
  }

out:
  اگر(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحیح)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  بازگشت result;
}

#وگرنهاگر defined(HAVE_APPLE_پیام_X)

ایستا CURLcode recvmsg_x_packets(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  تهی *userp, size_t *pnread)
{
#define پیام_X_رقم  64
#define پیام_BUF_SIZE  (2048)
#define CMSG_PER_پیام_SIZE     CMSG_SPACE(اندازه(صحیح))
  ساختار iovec msg_iov[پیام_X_رقم];
  ساختار msghdr_x mmsg[پیام_X_رقم];
  uint8_t پیام_ctrl[پیام_X_رقم * CMSG_PER_پیام_SIZE];
  ساختار sockaddr_storage remote_addr[پیام_X_رقم];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  صحیح mcount, i;
  نویسه errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  نویسه *sockbuf = پوچ;
  uint8_t (*bufs)[پیام_BUF_SIZE] = پوچ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, پیام_X_رقم * پیام_BUF_SIZE,
                                          &sockbuf);
  اگر(result)
    برو out;
  bufs = (uint8_t (*)[پیام_BUF_SIZE])sockbuf;

  total_nread = 0;
  وقتی(pkts < max_pkts) {
    صحیح n = (صحیح)CURLMIN(CURLMIN(پیام_X_رقم, IOV_MAX), max_pkts);
    memset(&mmsg, 0, اندازه(mmsg));
    برای(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = اندازه(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].پیام_iovlen = 1;
      mmsg[i].پیام_name = &remote_addr[i];
      mmsg[i].msg_namelen = اندازه(remote_addr[i]);
      mmsg[i].پیام_control = &پیام_ctrl[i * CMSG_PER_پیام_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_پیام_SIZE;
    }

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    وقتی((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic برداشت
#endif
    اگر(mcount == -1) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        برو out;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساختار ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->next, data, پوچ, پوچ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        برو out;
      }
      curlx_strerror(SOCKERRNO, errstr, اندازه(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      برو out;
    }

    VERBOSE(++calls);
    برای(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      اگر(!mmsg[i].پیام_datalen) {
        ++pkts;
        ادامه;
      }
      total_nread += mmsg[i].پیام_datalen;
      ecn = vquic_msghdr_get_ecn((ساختار msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].پیام_datalen;

      result = recv_cb(bufs[i], mmsg[i].پیام_datalen, gso_size,
                       mmsg[i].پیام_name,
                       mmsg[i].msg_namelen, ecn, userp);
      اگر(result)
        برو out;
      pkts += (mmsg[i].پیام_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  اگر(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحیح)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  بازگشت result;
}

#وگرنهاگر defined(HAVE_SENDMSG)
ایستا CURLcode recvmsg_packets(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ساختار cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, تهی *userp,
                                size_t *pnread)
{
#define CMSG_PER_پیام_SIZE    CMSG_SPACE(اندازه(صحیح))
  ساختار iovec msg_iov;
  ساختار msghdr پیام;
  uint8_t buf[64 * 1024];
  ساختار sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  نویسه errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t پیام_ctrl[CMSG_PER_پیام_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  برای(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&پیام, 0, اندازه(پیام));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = اندازه(buf);
    پیام.msg_iov = &msg_iov;
    پیام.پیام_iovlen = 1;
    پیام.پیام_control = پیام_ctrl;
    پیام.پیام_name = &remote_addr;
    پیام.msg_namelen = اندازه(remote_addr);
    پیام.msg_controllen = CMSG_PER_پیام_SIZE;

    وقتی((rc = recvmsg(qctx->sockfd, &پیام, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    اگر(!curlx_sztouz(rc, &nread)) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        برو out;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساختار ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->next, data, پوچ, پوچ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        برو out;
      }
      curlx_strerror(SOCKERRNO, errstr, اندازه(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      برو out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    اگر(!nread) {
      ++pkts;
      ادامه;
    }

    ecn = vquic_msghdr_get_ecn(&پیام, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&پیام);
    اگر(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     پیام.پیام_name, پیام.msg_namelen, ecn, userp);
    اگر(result)
      برو out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  اگر(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحیح)result);
  *pnread = total_nread;
  بازگشت result;
}

#وگرنه /* HAVE_SENDMMSG || HAVE_SENDMSG */
ایستا CURLcode recvfrom_packets(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, تهی *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  صحیح bufsize = (صحیح)اندازه(buf);
  ساختار sockaddr_storage remote_addr;
  socklen_t remote_addrlen = اندازه(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  نویسه errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  برای(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    وقتی((rv = recvfrom(qctx->sockfd, (نویسه *)buf, bufsize, 0,
                         (ساختار sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    اگر(!curlx_sztouz(rv, &nread)) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        برو out;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساختار ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->next, data, پوچ, پوچ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        برو out;
      }
      curlx_strerror(SOCKERRNO, errstr, اندازه(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      برو out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    اگر(!nread)
      ادامه;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    اگر(result)
      برو out;
  }

out:
  اگر(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (صحیح)result);
  *pnread = total_nread;
  بازگشت result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, تهی *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#وگرنهاگر defined(HAVE_APPLE_پیام_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#وگرنهاگر defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#وگرنه
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  اگر(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    اگر(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  بازگشت result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ساختار Curl_easy *data,
                      unsigned نویسه *scid,
                      size_t scidlen,
                      صحیح *qlogfdp)
{
  نویسه *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  اگر(qlog_dir) {
    ساختار dynbuf fname;
    CURLcode result;
    unsigned صحیح i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    اگر(!result)
      result = curlx_dyn_add(&fname, "/");
    برای(i = 0; (i < scidlen) && !result; i++) {
      نویسه hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    اگر(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    اگر(!result) {
      صحیح qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_دودویی,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#وگرنه
                              (mode_t)data->set.new_file_perms
#endif
                              );
      اگر(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    اگر(result)
      بازگشت result;
  }

  بازگشت CURLE_OK;
}

CURLcode Curl_cf_quic_درج_after(ساختار Curl_cfilter *cf_at,
                                   ساختار Curl_easy *data,
                                   ساختار Curl_peer *origin,
                                   ساختار Curl_peer *peer)
{
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  (تهی)data; /* not used in all cases and compilers are stupid */
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_درج_after(cf_at, origin, peer, &cf);
#وگرنهاگر defined(USE_QUICHE)
  result = Curl_cf_quiche_درج_after(cf_at, origin, peer, &cf);
#وگرنه
  (تهی)cf)
  (تهی)cf_at;
  (تهی)origin;
  (تهی)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#اگر defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  اگر(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  بازگشت result;
}

CURLcode Curl_cf_quic_create(ساختار Curl_cfilter **pcf,
                             ساختار Curl_easy *data,
                             ساختار Curl_peer *origin,
                             ساختار Curl_peer *peer,
                             uint8_t transport_peer,
                             ساختار connectdata *conn,
                             ساختار Curl_sockaddr_ex *addr,
                             ساختار Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (تهی)transport_peer;
  (تهی)tunnel_transport;
  (تهی)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  بازگشت Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#وگرنهاگر defined(USE_QUICHE)
  بازگشت Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#وگرنه
  *pcf = پوچ;
  (تهی)data;
  (تهی)origin;
  (تهی)peer;
  (تهی)conn;
  (تهی)addr;
  (تهی)tunnel_peer;
  (تهی)tunnel_transport;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

#اگر !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_درج_after(ساختار Curl_cfilter *cf_at,
                                       ساختار Curl_easy *data,
                                       ساختار Curl_peer *origin,
                                       ساختار Curl_peer *peer,
                                       ساختار Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  بازگشت Curl_cf_ngtcp2_proxy_درج_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#وگرنه
  (تهی)cf_at;
  (تهی)data;
  (تهی)origin;
  (تهی)peer;
  (تهی)tunnel_peer;
  (تهی)tunnel_transport;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(ساختار Curl_cfilter **pcf,
                                 ساختار Curl_easy *data,
                                 ساختار Curl_peer *origin,
                                 ساختار Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ساختار connectdata *conn,
                                 ساختار Curl_sockaddr_ex *addr,
                                 ساختار Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  بازگشت Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#وگرنه
  *pcf = پوچ;
  (تهی)data;
  (تهی)conn;
  (تهی)addr;
  (تهی)peer;
  (تهی)transport_peer;
  (تهی)tunnel_peer;
  (تهی)tunnel_transport;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ساختار Curl_easy *data,
                             ساختار connectdata *conn)
{
  اگر(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }
  اگر(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    بازگشت CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  اگر(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    بازگشت CURLE_URL_MALFORMAT;
  }
#وگرنه
  (تهی)conn;
#endif

  بازگشت CURLE_OK;
}

#ifdef CURLVERBOSE
ثابت نویسه *Curl_vquic_h3_err_str(uint64_t error_code)
{
  اگر(error_code <= UINT_MAX) {
    انتخاب((unsigned صحیح)error_code) {
    حالت CURL_H3_ERR_NO_ERROR:
      بازگشت "NO_ERROR";
    حالت CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      بازگشت "GENERAL_PROTOCOL_ERROR";
    حالت CURL_H3_ERR_INTERNAL_ERROR:
      بازگشت "INTERNAL_ERROR";
    حالت CURL_H3_ERR_STREAM_CREATION_ERROR:
      بازگشت "STREAM_CREATION_ERROR";
    حالت CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      بازگشت "CLOSED_CRITICAL_STREAM";
    حالت CURL_H3_ERR_FRAME_UNEXPECTED:
      بازگشت "FRAME_UNEXPECTED";
    حالت CURL_H3_ERR_FRAME_ERROR:
      بازگشت "FRAME_ERROR";
    حالت CURL_H3_ERR_EXCESSIVE_LOAD:
      بازگشت "EXCESSIVE_LOAD";
    حالت CURL_H3_ERR_ID_ERROR:
      بازگشت "ID_ERROR";
    حالت CURL_H3_ERR_SETTINGS_ERROR:
      بازگشت "SETTINGS_ERROR";
    حالت CURL_H3_ERR_MISSING_SETTINGS:
      بازگشت "MISSING_SETTINGS";
    حالت CURL_H3_ERR_REQUEST_REJECTED:
      بازگشت "REQUEST_REJECTED";
    حالت CURL_H3_ERR_REQUEST_CANCELLED:
      بازگشت "REQUEST_CANCELLED";
    حالت CURL_H3_ERR_REQUEST_INCOMPLETE:
      بازگشت "REQUEST_INCOMPLETE";
    حالت CURL_H3_ERR_MESSAGE_ERROR:
      بازگشت "MESSAGE_ERROR";
    حالت CURL_H3_ERR_CONNECT_ERROR:
      بازگشت "CONNECT_ERROR";
    حالت CURL_H3_ERR_VERSION_FALLBACK:
      بازگشت "VERSION_FALLBACK";
    پیشفرض:
      بشکن;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  اگر((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    بازگشت "NO_ERROR";
  بازگشت "unknown";
}
#endif /* CURLVERBOSE */

#اگر defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ایستا تهی *vquic_ngtcp2_malloc(size_t size, تهی *user_data)
{
  (تهی)user_data;
  بازگشت Curl_cmalloc(size);
}

ایستا تهی vquic_ngtcp2_free(تهی *ptr, تهی *user_data)
{
  (تهی)user_data;
  Curl_cfree(ptr);
}

ایستا تهی *vquic_ngtcp2_calloc(size_t nmemb, size_t size, تهی *user_data)
{
  (تهی)user_data;
  بازگشت Curl_ccalloc(nmemb, size);
}

ایستا تهی *vquic_ngtcp2_realloc(تهی *ptr, size_t size, تهی *user_data)
{
  (تهی)user_data;
  بازگشت Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
ایستا ساختار ngtcp2_mem vquic_ngtcp2_mem = {
  پوچ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ساختار ngtcp2_mem *Curl_ngtcp2_mem(تهی)
{
  بازگشت &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
ایستا ساختار nghttp3_mem vquic_nghttp3_mem = {
  پوچ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ساختار nghttp3_mem *Curl_nghttp3_mem(تهی)
{
  بازگشت &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#وگرنه /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ساختار Curl_easy *data,
                             ساختار connectdata *conn)
{
  (تهی)data;
  (تهی)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  بازگشت CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
