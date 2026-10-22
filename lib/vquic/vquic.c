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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vtls/vtls.h"

#अन्तर्भाव "curl_trc.h"

#सचे !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#अन्तर्भाव <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#अन्तर्भाव <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#अन्तर्भाव <nghttp3/nghttp3.h>
#endif

#सचे defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#अन्तर्भाव <sys/syscall.h>
#सचे defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_सन्देशः_X
#endif
#endif

#अन्तर्भाव "bufq.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vquic/cf-ngtcp2.h"
#अन्तर्भाव "vquic/cf-ngtcp2-cmn.h"
#अन्तर्भाव "vquic/cf-ngtcp2-proxy.h"
#अन्तर्भाव "vquic/cf-quiche.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "vquic/vquic_int.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_सन्देशः_X

/* this is `struct msghdr` with an additional field at the end */
संरचना msghdr_x {
  शून्य *सन्देशः_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  संरचना iovec *msg_iov;    /* scatter/gather array */
  पूर्णाङ्क सन्देशः_iovlen;           /* # elements in msg_iov */
  शून्य *सन्देशः_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  पूर्णाङ्क सन्देशः_flags;            /* flags on received message */
  size_t सन्देशः_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_सन्देशः_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#नोचेद्यदि defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#अञ्ञथा
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

पूर्णाङ्क Curl_vquic_init(शून्य)
{
#सचे defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  सचे(ngtcp2_crypto_ossl_init())
    पटिनिवत्त 0;
#endif

  पटिनिवत्त 1;
}

शून्य Curl_vquic_cleanup(शून्य)
{
#सचे defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_अङ्कः >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

शून्य Curl_quic_ver(वर्ण *p, size_t len)
{
#सचे defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#नोचेद्यदि defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(संरचना Curl_easy *data,
                             संरचना cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#सचे defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#नोचेद्यदि defined(HAVE_APPLE_सन्देशः_X)
  qctx->no_gso = FALSE;
#अञ्ञथा
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_QUIC_WBLOCK");
    सचे(p) {
      curl_off_t l;
      सचे(!curlx_str_सङ्ख्या(&p, &l, 100))
        qctx->wblock_percent = (पूर्णाङ्क)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  पटिनिवत्त CURLE_OK;
}

शून्य Curl_vquic_ctx_free(संरचना cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

शून्य Curl_vquic_ctx_set_time(संरचना cf_quic_ctx *qctx,
                             ध्रुव संरचना curltime *pnow)
{
  qctx->last_op = *pnow;
}

शून्य Curl_vquic_ctx_update_time(संरचना cf_quic_ctx *qctx,
                                ध्रुव संरचना curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_सन्देशः_X
स्थिर CURLcode do_sendmsg(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना cf_quic_ctx *qctx,
                           ध्रुव uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define सन्देशः_X_SNUM  64
  संरचना iovec msg_iov[सन्देशः_X_SNUM];
  संरचना msghdr_x mmsg[सन्देशः_X_SNUM];
  वर्ण errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  पूर्णाङ्क rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  सचे(!pktlen)
    पटिनिवत्त CURLE_OK;
  सचे(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  याव(i < n) {
    size_t j, batch = CURLMIN(n - i, सन्देशः_X_SNUM), pkts_sent = 0;

    अत्थाय(j = 0; j < batch; ++j) {
      ध्रुव size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, परिमाण(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].सन्देशः_iovlen = 1;
      mmsg[j].सन्देशः_datalen = msg_iov[j].iov_len;
    }

#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    याव((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic निष्कासय
#endif

    सचे(rc < 0) {
      सचे(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        गच्छ out;
      }
      सचे(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        गच्छ out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    अञ्ञथा
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    सचे(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      गच्छ out;
    }
    i += pkts_sent; /* Some have been sent */
    अत्थाय(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    सचे(pkts_sent < batch)
      गच्छ out; /* but not all of them */
  }

out:
  *psent = sent;
  सचे(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (पूर्णाङ्क)result);
  पटिनिवत्त result;
}

#अञ्ञथा /* HAVE_APPLE_MSG_X */

स्थिर CURLcode send_packet_no_gso(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना cf_quic_ctx *qctx,
                                   ध्रुव uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

स्थिर CURLcode do_sendmsg(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना cf_quic_ctx *qctx,
                           ध्रुव uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  संरचना iovec msg_iov;
  संरचना msghdr सन्देशः = { 0 };
  ssize_t rv;
#सचे defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t सन्देशः_ctrl[32];
  संरचना cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  सन्देशः.msg_iov = &msg_iov;
  सन्देशः.सन्देशः_iovlen = 1;

#सचे defined(__linux__) && defined(UDP_SEGMENT)
  सचे(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(सन्देशः_ctrl, 0, परिमाण(सन्देशः_ctrl));
    सन्देशः.सन्देशः_control = सन्देशः_ctrl;
    DEBUGASSERT(परिमाण(सन्देशः_ctrl) >= CMSG_SPACE(परिमाण(पूर्णाङ्क)));
    सन्देशः.msg_controllen = CMSG_SPACE(परिमाण(पूर्णाङ्क));
    cm = CMSG_FIRSTHDR(&सन्देशः);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(परिमाण(uint16_t));
    *(uint16_t *)(शून्य *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  याव((rv = sendmsg(qctx->sockfd, &सन्देशः, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  सचे(!curlx_sztouz(rv, psent)) {
    पूर्णाङ्क sockerr = SOCKERRNO;
    सचे(SOCK_EAGAIN(sockerr))
      पटिनिवत्त CURLE_AGAIN;
    विकल्प(sockerr) {
    पक्ष SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      भङ्ग;
    पक्ष EIO:
      सचे(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        पटिनिवत्त send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    स्वभाव:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
  }
  अञ्ञथा सचे(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }
#अञ्ञथा
  ssize_t rv;
  (शून्य)gsolen;

  *psent = 0;

  याव((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  सचे(!curlx_sztouz(rv, psent)) {
    सचे(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      गच्छ out;
    }
    अञ्ञथा {
      सचे(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        गच्छ out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (शून्य)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (पूर्णाङ्क)result, *psent);
  पटिनिवत्त result;
}

#endif /* !HAVE_APPLE_MSG_X */

स्थिर CURLcode send_packet_no_gso(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना cf_quic_ctx *qctx,
                                   ध्रुव uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ध्रुव uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  अत्थाय(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    सचे(result)
      गच्छ out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (पूर्णाङ्क)result, *psent);
  पटिनिवत्त result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
स्थिर CURLcode send_packet_no_gso_cf(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      ध्रुव uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ध्रुव uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  अत्थाय(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    सचे(result)
      गच्छ out;

    सचे(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (पूर्णाङ्क)result, *psent);
  पटिनिवत्त result;
}

स्थिर CURLcode vquic_send_packets(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना cf_quic_ctx *qctx,
                                   ध्रुव uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  सचे(qctx->wblock_percent > 0) {
    unsigned वर्ण c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    सचे(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      पटिनिवत्त CURLE_AGAIN;
    }
  }
#endif
  सचे(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  अञ्ञथा {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  सचे(!result)
    qctx->last_io = qctx->last_op;
  पटिनिवत्त result;
}

CURLcode Curl_vquic_flush(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          संरचना cf_quic_ctx *qctx)
{
  ध्रुव unsigned वर्ण *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  याव(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    सचे(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      सचे(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    सचे(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    अञ्ञथा {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      सचे(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      अञ्ञथा {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    सचे(result) {
      सचे(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        सचे(qctx->split_len)
          qctx->split_len -= sent;
      }
      पटिनिवत्त result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    सचे(qctx->split_len)
      qctx->split_len -= sent;
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_vquic_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                         संरचना cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  पटिनिवत्त Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  पटिनिवत्त Curl_vquic_flush(cf, data, qctx);
}

#सचे (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_सन्देशः_X)
स्थिर size_t vquic_msghdr_get_udp_gro(संरचना msghdr *सन्देशः)
{
  पूर्णाङ्क gso_size = 0;
#सचे defined(__linux__) && defined(UDP_GRO)
  संरचना cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#सचे defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic प्रक्षिप
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  अत्थाय(cmsg = CMSG_FIRSTHDR(सन्देशः); cmsg; cmsg = CMSG_NXTHDR(सन्देशः, cmsg)) {
#सचे defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic निष्कासय
#endif
    सचे(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), परिमाण(gso_size));

      भङ्ग;
    }
  }
#endif /* linux && UDP_GRO */
  (शून्य)सन्देशः;

  पटिनिवत्त (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#सचे (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_सन्देशः_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
स्थिर uint8_t vquic_msghdr_get_ecn(संरचना msghdr *सन्देशः, पूर्णाङ्क family)
{
  संरचना cmsghdr *cmsg;
  विकल्प(family) {
  पक्ष AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#सचे defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic प्रक्षिप
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    अत्थाय(cmsg = CMSG_FIRSTHDR(सन्देशः); cmsg; cmsg = CMSG_NXTHDR(सन्देशः, cmsg)) {
#सचे defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic निष्कासय
#endif
      सचे(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#अञ्ञथा
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        पटिनिवत्त *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    भङ्ग;
  पक्ष AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#सचे defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic प्रक्षिप
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    अत्थाय(cmsg = CMSG_FIRSTHDR(सन्देशः); cmsg; cmsg = CMSG_NXTHDR(सन्देशः, cmsg)) {
#सचे defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic निष्कासय
#endif
      सचे(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned पूर्णाङ्क tos;

        memcpy(&tos, CMSG_DATA(cmsg), परिमाण(पूर्णाङ्क));

        पटिनिवत्त (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    भङ्ग;
  }
  पटिनिवत्त 0;
}
#अञ्ञथा
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

स्थिर CURLcode recvmmsg_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                 size_t *pnread)
{
#सचे defined(__linux__) && defined(UDP_GRO)
#define MMSG_अङ्कः  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_सन्देशः_SIZE    (2 * CMSG_SPACE(परिमाण(पूर्णाङ्क)))
#अञ्ञथा
#define MMSG_अङ्कः  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_सन्देशः_SIZE    CMSG_SPACE(परिमाण(पूर्णाङ्क))
#endif
#define सन्देशः_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  संरचना iovec msg_iov[MMSG_अङ्कः];
  संरचना mmsghdr mmsg[MMSG_अङ्कः];
  uint8_t सन्देशः_ctrl[MMSG_अङ्कः * CMSG_PER_सन्देशः_SIZE];
  संरचना sockaddr_storage remote_addr[MMSG_अङ्कः];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  पूर्णाङ्क mcount, i, n;
  वर्ण errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  वर्ण *sockbuf = रिक्त;
  uint8_t (*bufs)[सन्देशः_BUF_SIZE] = रिक्त;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_अङ्कः * सन्देशः_BUF_SIZE,
                                          &sockbuf);
  सचे(result)
    गच्छ out;
  bufs = (uint8_t (*)[सन्देशः_BUF_SIZE])sockbuf;

  total_nread = 0;
  याव(pkts < max_pkts) {
    n = (पूर्णाङ्क)CURLMIN(CURLMIN(MMSG_अङ्कः, IOV_MAX), max_pkts);
    memset(&mmsg, 0, परिमाण(mmsg));
    अत्थाय(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = परिमाण(bufs[i]);
      mmsg[i].सन्देशः_hdr.msg_iov = &msg_iov[i];
      mmsg[i].सन्देशः_hdr.सन्देशः_iovlen = 1;
      mmsg[i].सन्देशः_hdr.सन्देशः_name = &remote_addr[i];
      mmsg[i].सन्देशः_hdr.msg_namelen = परिमाण(remote_addr[i]);
      mmsg[i].सन्देशः_hdr.सन्देशः_control = &सन्देशः_ctrl[i * CMSG_PER_सन्देशः_SIZE];
      mmsg[i].सन्देशः_hdr.msg_controllen = CMSG_PER_सन्देशः_SIZE;
    }

    याव((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, रिक्त)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    सचे(mcount == -1) {
      सचे(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        गच्छ out;
      }
      सचे(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        सचे(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        गच्छ out;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      गच्छ out;
    }

    VERBOSE(++calls);
    अत्थाय(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      सचे(!mmsg[i].सन्देशः_len) {
        ++pkts;
        अनुवर्त;
      }
      total_nread += mmsg[i].सन्देशः_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].सन्देशः_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].सन्देशः_hdr);
      सचे(gso_size == 0)
        gso_size = mmsg[i].सन्देशः_len;

      result = recv_cb(bufs[i], mmsg[i].सन्देशः_len, gso_size,
                       mmsg[i].सन्देशः_hdr.सन्देशः_name,
                       mmsg[i].सन्देशः_hdr.msg_namelen, ecn, userp);
      सचे(result)
        गच्छ out;
      pkts += (mmsg[i].सन्देशः_len + gso_size - 1) / gso_size;
    }
  }

out:
  सचे(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णाङ्क)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  पटिनिवत्त result;
}

#नोचेद्यदि defined(HAVE_APPLE_सन्देशः_X)

स्थिर CURLcode recvmsg_x_packets(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  शून्य *userp, size_t *pnread)
{
#define सन्देशः_X_अङ्कः  64
#define सन्देशः_BUF_SIZE  (2048)
#define CMSG_PER_सन्देशः_SIZE     CMSG_SPACE(परिमाण(पूर्णाङ्क))
  संरचना iovec msg_iov[सन्देशः_X_अङ्कः];
  संरचना msghdr_x mmsg[सन्देशः_X_अङ्कः];
  uint8_t सन्देशः_ctrl[सन्देशः_X_अङ्कः * CMSG_PER_सन्देशः_SIZE];
  संरचना sockaddr_storage remote_addr[सन्देशः_X_अङ्कः];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  पूर्णाङ्क mcount, i;
  वर्ण errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  वर्ण *sockbuf = रिक्त;
  uint8_t (*bufs)[सन्देशः_BUF_SIZE] = रिक्त;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, सन्देशः_X_अङ्कः * सन्देशः_BUF_SIZE,
                                          &sockbuf);
  सचे(result)
    गच्छ out;
  bufs = (uint8_t (*)[सन्देशः_BUF_SIZE])sockbuf;

  total_nread = 0;
  याव(pkts < max_pkts) {
    पूर्णाङ्क n = (पूर्णाङ्क)CURLMIN(CURLMIN(सन्देशः_X_अङ्कः, IOV_MAX), max_pkts);
    memset(&mmsg, 0, परिमाण(mmsg));
    अत्थाय(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = परिमाण(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].सन्देशः_iovlen = 1;
      mmsg[i].सन्देशः_name = &remote_addr[i];
      mmsg[i].msg_namelen = परिमाण(remote_addr[i]);
      mmsg[i].सन्देशः_control = &सन्देशः_ctrl[i * CMSG_PER_सन्देशः_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_सन्देशः_SIZE;
    }

#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    याव((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#सचे defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic निष्कासय
#endif
    सचे(mcount == -1) {
      सचे(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        गच्छ out;
      }
      सचे(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        सचे(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        गच्छ out;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      गच्छ out;
    }

    VERBOSE(++calls);
    अत्थाय(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      सचे(!mmsg[i].सन्देशः_datalen) {
        ++pkts;
        अनुवर्त;
      }
      total_nread += mmsg[i].सन्देशः_datalen;
      ecn = vquic_msghdr_get_ecn((संरचना msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].सन्देशः_datalen;

      result = recv_cb(bufs[i], mmsg[i].सन्देशः_datalen, gso_size,
                       mmsg[i].सन्देशः_name,
                       mmsg[i].msg_namelen, ecn, userp);
      सचे(result)
        गच्छ out;
      pkts += (mmsg[i].सन्देशः_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  सचे(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णाङ्क)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  पटिनिवत्त result;
}

#नोचेद्यदि defined(HAVE_SENDMSG)
स्थिर CURLcode recvmsg_packets(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                size_t *pnread)
{
#define CMSG_PER_सन्देशः_SIZE    CMSG_SPACE(परिमाण(पूर्णाङ्क))
  संरचना iovec msg_iov;
  संरचना msghdr सन्देशः;
  uint8_t buf[64 * 1024];
  संरचना sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  वर्ण errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t सन्देशः_ctrl[CMSG_PER_सन्देशः_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  अत्थाय(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&सन्देशः, 0, परिमाण(सन्देशः));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = परिमाण(buf);
    सन्देशः.msg_iov = &msg_iov;
    सन्देशः.सन्देशः_iovlen = 1;
    सन्देशः.सन्देशः_control = सन्देशः_ctrl;
    सन्देशः.सन्देशः_name = &remote_addr;
    सन्देशः.msg_namelen = परिमाण(remote_addr);
    सन्देशः.msg_controllen = CMSG_PER_सन्देशः_SIZE;

    याव((rc = recvmsg(qctx->sockfd, &सन्देशः, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    सचे(!curlx_sztouz(rc, &nread)) {
      सचे(SOCK_EAGAIN(SOCKERRNO)) {
        गच्छ out;
      }
      सचे(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        सचे(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        गच्छ out;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      गच्छ out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    सचे(!nread) {
      ++pkts;
      अनुवर्त;
    }

    ecn = vquic_msghdr_get_ecn(&सन्देशः, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&सन्देशः);
    सचे(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     सन्देशः.सन्देशः_name, सन्देशः.msg_namelen, ecn, userp);
    सचे(result)
      गच्छ out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  सचे(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णाङ्क)result);
  *pnread = total_nread;
  पटिनिवत्त result;
}

#अञ्ञथा /* HAVE_SENDMMSG || HAVE_SENDMSG */
स्थिर CURLcode recvfrom_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  पूर्णाङ्क bufsize = (पूर्णाङ्क)परिमाण(buf);
  संरचना sockaddr_storage remote_addr;
  socklen_t remote_addrlen = परिमाण(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  वर्ण errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  अत्थाय(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    याव((rv = recvfrom(qctx->sockfd, (वर्ण *)buf, bufsize, 0,
                         (संरचना sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    सचे(!curlx_sztouz(rv, &nread)) {
      सचे(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        गच्छ out;
      }
      सचे(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        सचे(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        गच्छ out;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      गच्छ out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    सचे(!nread)
      अनुवर्त;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    सचे(result)
      गच्छ out;
  }

out:
  सचे(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णाङ्क)result);
  *pnread = total_nread;
  पटिनिवत्त result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#नोचेद्यदि defined(HAVE_APPLE_सन्देशः_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#नोचेद्यदि defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#अञ्ञथा
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  सचे(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    सचे(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  पटिनिवत्त result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(संरचना Curl_easy *data,
                      unsigned वर्ण *scid,
                      size_t scidlen,
                      पूर्णाङ्क *qlogfdp)
{
  वर्ण *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  सचे(qlog_dir) {
    संरचना dynbuf fname;
    CURLcode result;
    unsigned पूर्णाङ्क i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    सचे(!result)
      result = curlx_dyn_add(&fname, "/");
    अत्थाय(i = 0; (i < scidlen) && !result; i++) {
      वर्ण hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    सचे(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    सचे(!result) {
      पूर्णाङ्क qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_द्विमान,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#अञ्ञथा
                              (mode_t)data->set.new_file_perms
#endif
                              );
      सचे(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    सचे(result)
      पटिनिवत्त result;
  }

  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_cf_quic_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                   संरचना Curl_easy *data,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  (शून्य)data; /* not used in all cases and compilers are stupid */
#सचे defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_निवेशय_after(cf_at, origin, peer, &cf);
#नोचेद्यदि defined(USE_QUICHE)
  result = Curl_cf_quiche_निवेशय_after(cf_at, origin, peer, &cf);
#अञ्ञथा
  (शून्य)cf)
  (शून्य)cf_at;
  (शून्य)origin;
  (शून्य)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#सचे defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  सचे(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  पटिनिवत्त result;
}

CURLcode Curl_cf_quic_create(संरचना Curl_cfilter **pcf,
                             संरचना Curl_easy *data,
                             संरचना Curl_peer *origin,
                             संरचना Curl_peer *peer,
                             uint8_t transport_peer,
                             संरचना connectdata *conn,
                             संरचना Curl_sockaddr_ex *addr,
                             संरचना Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (शून्य)transport_peer;
  (शून्य)tunnel_transport;
  (शून्य)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#सचे defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  पटिनिवत्त Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#नोचेद्यदि defined(USE_QUICHE)
  पटिनिवत्त Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#अञ्ञथा
  *pcf = रिक्त;
  (शून्य)data;
  (शून्य)origin;
  (शून्य)peer;
  (शून्य)conn;
  (शून्य)addr;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
#endif
}

#सचे !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *data,
                                       संरचना Curl_peer *origin,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#सचे defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  पटिनिवत्त Curl_cf_ngtcp2_proxy_निवेशय_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#अञ्ञथा
  (शून्य)cf_at;
  (शून्य)data;
  (शून्य)origin;
  (शून्य)peer;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(संरचना Curl_cfilter **pcf,
                                 संरचना Curl_easy *data,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 uint8_t transport_peer,
                                 संरचना connectdata *conn,
                                 संरचना Curl_sockaddr_ex *addr,
                                 संरचना Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#सचे defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  पटिनिवत्त Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#अञ्ञथा
  *pcf = रिक्त;
  (शून्य)data;
  (शून्य)conn;
  (शून्य)addr;
  (शून्य)peer;
  (शून्य)transport_peer;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(संरचना Curl_easy *data,
                             संरचना connectdata *conn)
{
  सचे(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
  }
  सचे(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    पटिनिवत्त CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  सचे(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    पटिनिवत्त CURLE_URL_MALFORMAT;
  }
#अञ्ञथा
  (शून्य)conn;
#endif

  पटिनिवत्त CURLE_OK;
}

#ifdef CURLVERBOSE
ध्रुव वर्ण *Curl_vquic_h3_err_str(uint64_t error_code)
{
  सचे(error_code <= UINT_MAX) {
    विकल्प((unsigned पूर्णाङ्क)error_code) {
    पक्ष CURL_H3_ERR_NO_ERROR:
      पटिनिवत्त "NO_ERROR";
    पक्ष CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      पटिनिवत्त "GENERAL_PROTOCOL_ERROR";
    पक्ष CURL_H3_ERR_INTERNAL_ERROR:
      पटिनिवत्त "INTERNAL_ERROR";
    पक्ष CURL_H3_ERR_STREAM_CREATION_ERROR:
      पटिनिवत्त "STREAM_CREATION_ERROR";
    पक्ष CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      पटिनिवत्त "CLOSED_CRITICAL_STREAM";
    पक्ष CURL_H3_ERR_FRAME_UNEXPECTED:
      पटिनिवत्त "FRAME_UNEXPECTED";
    पक्ष CURL_H3_ERR_FRAME_ERROR:
      पटिनिवत्त "FRAME_ERROR";
    पक्ष CURL_H3_ERR_EXCESSIVE_LOAD:
      पटिनिवत्त "EXCESSIVE_LOAD";
    पक्ष CURL_H3_ERR_ID_ERROR:
      पटिनिवत्त "ID_ERROR";
    पक्ष CURL_H3_ERR_SETTINGS_ERROR:
      पटिनिवत्त "SETTINGS_ERROR";
    पक्ष CURL_H3_ERR_MISSING_SETTINGS:
      पटिनिवत्त "MISSING_SETTINGS";
    पक्ष CURL_H3_ERR_REQUEST_REJECTED:
      पटिनिवत्त "REQUEST_REJECTED";
    पक्ष CURL_H3_ERR_REQUEST_CANCELLED:
      पटिनिवत्त "REQUEST_CANCELLED";
    पक्ष CURL_H3_ERR_REQUEST_INCOMPLETE:
      पटिनिवत्त "REQUEST_INCOMPLETE";
    पक्ष CURL_H3_ERR_MESSAGE_ERROR:
      पटिनिवत्त "MESSAGE_ERROR";
    पक्ष CURL_H3_ERR_CONNECT_ERROR:
      पटिनिवत्त "CONNECT_ERROR";
    पक्ष CURL_H3_ERR_VERSION_FALLBACK:
      पटिनिवत्त "VERSION_FALLBACK";
    स्वभाव:
      भङ्ग;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  सचे((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    पटिनिवत्त "NO_ERROR";
  पटिनिवत्त "unknown";
}
#endif /* CURLVERBOSE */

#सचे defined(USE_NGTCP2) || defined(USE_NGHTTP3)

स्थिर शून्य *vquic_ngtcp2_malloc(size_t size, शून्य *user_data)
{
  (शून्य)user_data;
  पटिनिवत्त Curl_cmalloc(size);
}

स्थिर शून्य vquic_ngtcp2_free(शून्य *ptr, शून्य *user_data)
{
  (शून्य)user_data;
  Curl_cfree(ptr);
}

स्थिर शून्य *vquic_ngtcp2_calloc(size_t nmemb, size_t size, शून्य *user_data)
{
  (शून्य)user_data;
  पटिनिवत्त Curl_ccalloc(nmemb, size);
}

स्थिर शून्य *vquic_ngtcp2_realloc(शून्य *ptr, size_t size, शून्य *user_data)
{
  (शून्य)user_data;
  पटिनिवत्त Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
स्थिर संरचना ngtcp2_mem vquic_ngtcp2_mem = {
  रिक्त,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
संरचना ngtcp2_mem *Curl_ngtcp2_mem(शून्य)
{
  पटिनिवत्त &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
स्थिर संरचना nghttp3_mem vquic_nghttp3_mem = {
  रिक्त,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
संरचना nghttp3_mem *Curl_nghttp3_mem(शून्य)
{
  पटिनिवत्त &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#अञ्ञथा /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(संरचना Curl_easy *data,
                             संरचना connectdata *conn)
{
  (शून्य)data;
  (शून्य)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  पटिनिवत्त CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
