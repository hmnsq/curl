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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "vtls/vtls.h"

#અન્તર્ભાવ "curl_trc.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#અન્તર્ભાવ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#અન્તર્ભાવ <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#અન્તર્ભાવ <nghttp3/nghttp3.h>
#endif

#જો defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#અન્તર્ભાવ <sys/syscall.h>
#જો defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_સંદેશ_X
#endif
#endif

#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2-cmn.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2-proxy.h"
#અન્તર્ભાવ "vquic/cf-quiche.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "vquic/vquic_int.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_સંદેશ_X

/* this is `struct msghdr` with an additional field at the end */
સંરચના msghdr_x {
  શૂન્ય *સંદેશ_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  સંરચના iovec *msg_iov;    /* scatter/gather array */
  પૂર્ણાંક સંદેશ_iovlen;           /* # elements in msg_iov */
  શૂન્ય *સંદેશ_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  પૂર્ણાંક સંદેશ_flags;            /* flags on received message */
  size_t સંદેશ_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_સંદેશ_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#અથવાજો defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#નહીંતર
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

પૂર્ણાંક Curl_vquic_init(શૂન્ય)
{
#જો defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  જો(ngtcp2_crypto_ossl_init())
    પાછા 0;
#endif

  પાછા 1;
}

શૂન્ય Curl_vquic_cleanup(શૂન્ય)
{
#જો defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_અંક >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

શૂન્ય Curl_quic_ver(અક્ષર *p, size_t len)
{
#જો defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#અથવાજો defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(સંરચના Curl_easy *data,
                             સંરચના cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#જો defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#અથવાજો defined(HAVE_APPLE_સંદેશ_X)
  qctx->no_gso = FALSE;
#નહીંતર
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    અચલ અક્ષર *p = getenv("CURL_DBG_QUIC_WBLOCK");
    જો(p) {
      curl_off_t l;
      જો(!curlx_str_સંખ્યા(&p, &l, 100))
        qctx->wblock_percent = (પૂર્ણાંક)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  પાછા CURLE_OK;
}

શૂન્ય Curl_vquic_ctx_free(સંરચના cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

શૂન્ય Curl_vquic_ctx_set_time(સંરચના cf_quic_ctx *qctx,
                             અચલ સંરચના curltime *pnow)
{
  qctx->last_op = *pnow;
}

શૂન્ય Curl_vquic_ctx_update_time(સંરચના cf_quic_ctx *qctx,
                                અચલ સંરચના curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_સંદેશ_X
સ્થિર CURLcode do_sendmsg(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           સંરચના cf_quic_ctx *qctx,
                           અચલ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define સંદેશ_X_SNUM  64
  સંરચના iovec msg_iov[સંદેશ_X_SNUM];
  સંરચના msghdr_x mmsg[સંદેશ_X_SNUM];
  અક્ષર errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  પૂર્ણાંક rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  જો(!pktlen)
    પાછા CURLE_OK;
  જો(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  જ્યારે(i < n) {
    size_t j, batch = CURLMIN(n - i, સંદેશ_X_SNUM), pkts_sent = 0;

    માટે(j = 0; j < batch; ++j) {
      અચલ size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, કદ(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].સંદેશ_iovlen = 1;
      mmsg[j].સંદેશ_datalen = msg_iov[j].iov_len;
    }

#જો defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    જ્યારે((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#જો defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic કાઢો
#endif

    જો(rc < 0) {
      જો(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        જાઓ out;
      }
      જો(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, કદ(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        જાઓ out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    નહીંતર
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    જો(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      જાઓ out;
    }
    i += pkts_sent; /* Some have been sent */
    માટે(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    જો(pkts_sent < batch)
      જાઓ out; /* but not all of them */
  }

out:
  *psent = sent;
  જો(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (પૂર્ણાંક)result);
  પાછા result;
}

#નહીંતર /* HAVE_APPLE_MSG_X */

સ્થિર CURLcode send_packet_no_gso(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના cf_quic_ctx *qctx,
                                   અચલ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

સ્થિર CURLcode do_sendmsg(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           સંરચના cf_quic_ctx *qctx,
                           અચલ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  સંરચના iovec msg_iov;
  સંરચના msghdr સંદેશ = { 0 };
  ssize_t rv;
#જો defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t સંદેશ_ctrl[32];
  સંરચના cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  સંદેશ.msg_iov = &msg_iov;
  સંદેશ.સંદેશ_iovlen = 1;

#જો defined(__linux__) && defined(UDP_SEGMENT)
  જો(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(સંદેશ_ctrl, 0, કદ(સંદેશ_ctrl));
    સંદેશ.સંદેશ_control = સંદેશ_ctrl;
    DEBUGASSERT(કદ(સંદેશ_ctrl) >= CMSG_SPACE(કદ(પૂર્ણાંક)));
    સંદેશ.msg_controllen = CMSG_SPACE(કદ(પૂર્ણાંક));
    cm = CMSG_FIRSTHDR(&સંદેશ);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(કદ(uint16_t));
    *(uint16_t *)(શૂન્ય *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  જ્યારે((rv = sendmsg(qctx->sockfd, &સંદેશ, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  જો(!curlx_sztouz(rv, psent)) {
    પૂર્ણાંક sockerr = SOCKERRNO;
    જો(SOCK_EAGAIN(sockerr))
      પાછા CURLE_AGAIN;
    પસંદગી(sockerr) {
    સ્થિતિ SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      તોડો;
    સ્થિતિ EIO:
      જો(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        પાછા send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    મૂળભૂત:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }
  નહીંતર જો(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
#નહીંતર
  ssize_t rv;
  (શૂન્ય)gsolen;

  *psent = 0;

  જ્યારે((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  જો(!curlx_sztouz(rv, psent)) {
    જો(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      જાઓ out;
    }
    નહીંતર {
      જો(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        જાઓ out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (શૂન્ય)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (પૂર્ણાંક)result, *psent);
  પાછા result;
}

#endif /* !HAVE_APPLE_MSG_X */

સ્થિર CURLcode send_packet_no_gso(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના cf_quic_ctx *qctx,
                                   અચલ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  અચલ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  માટે(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    જો(result)
      જાઓ out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (પૂર્ણાંક)result, *psent);
  પાછા result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
સ્થિર CURLcode send_packet_no_gso_cf(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      અચલ uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  અચલ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  માટે(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    જો(result)
      જાઓ out;

    જો(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (પૂર્ણાંક)result, *psent);
  પાછા result;
}

સ્થિર CURLcode vquic_send_packets(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના cf_quic_ctx *qctx,
                                   અચલ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  જો(qctx->wblock_percent > 0) {
    unsigned અક્ષર c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    જો(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      પાછા CURLE_AGAIN;
    }
  }
#endif
  જો(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  નહીંતર {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  જો(!result)
    qctx->last_io = qctx->last_op;
  પાછા result;
}

CURLcode Curl_vquic_flush(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                          સંરચના cf_quic_ctx *qctx)
{
  અચલ unsigned અક્ષર *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  જ્યારે(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    જો(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      જો(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    જો(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    નહીંતર {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      જો(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      નહીંતર {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    જો(result) {
      જો(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        જો(qctx->split_len)
          qctx->split_len -= sent;
      }
      પાછા result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    જો(qctx->split_len)
      qctx->split_len -= sent;
  }
  પાછા CURLE_OK;
}

CURLcode Curl_vquic_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                         સંરચના cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  પાછા Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  પાછા Curl_vquic_flush(cf, data, qctx);
}

#જો (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_સંદેશ_X)
સ્થિર size_t vquic_msghdr_get_udp_gro(સંરચના msghdr *સંદેશ)
{
  પૂર્ણાંક gso_size = 0;
#જો defined(__linux__) && defined(UDP_GRO)
  સંરચના cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#જો defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  માટે(cmsg = CMSG_FIRSTHDR(સંદેશ); cmsg; cmsg = CMSG_NXTHDR(સંદેશ, cmsg)) {
#જો defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic કાઢો
#endif
    જો(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), કદ(gso_size));

      તોડો;
    }
  }
#endif /* linux && UDP_GRO */
  (શૂન્ય)સંદેશ;

  પાછા (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#જો (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_સંદેશ_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
સ્થિર uint8_t vquic_msghdr_get_ecn(સંરચના msghdr *સંદેશ, પૂર્ણાંક family)
{
  સંરચના cmsghdr *cmsg;
  પસંદગી(family) {
  સ્થિતિ AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#જો defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    માટે(cmsg = CMSG_FIRSTHDR(સંદેશ); cmsg; cmsg = CMSG_NXTHDR(સંદેશ, cmsg)) {
#જો defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic કાઢો
#endif
      જો(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#નહીંતર
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        પાછા *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    તોડો;
  સ્થિતિ AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#જો defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    માટે(cmsg = CMSG_FIRSTHDR(સંદેશ); cmsg; cmsg = CMSG_NXTHDR(સંદેશ, cmsg)) {
#જો defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic કાઢો
#endif
      જો(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned પૂર્ણાંક tos;

        memcpy(&tos, CMSG_DATA(cmsg), કદ(પૂર્ણાંક));

        પાછા (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    તોડો;
  }
  પાછા 0;
}
#નહીંતર
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

સ્થિર CURLcode recvmmsg_packets(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, શૂન્ય *userp,
                                 size_t *pnread)
{
#જો defined(__linux__) && defined(UDP_GRO)
#define MMSG_અંક  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_સંદેશ_SIZE    (2 * CMSG_SPACE(કદ(પૂર્ણાંક)))
#નહીંતર
#define MMSG_અંક  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_સંદેશ_SIZE    CMSG_SPACE(કદ(પૂર્ણાંક))
#endif
#define સંદેશ_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  સંરચના iovec msg_iov[MMSG_અંક];
  સંરચના mmsghdr mmsg[MMSG_અંક];
  uint8_t સંદેશ_ctrl[MMSG_અંક * CMSG_PER_સંદેશ_SIZE];
  સંરચના sockaddr_storage remote_addr[MMSG_અંક];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  પૂર્ણાંક mcount, i, n;
  અક્ષર errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  અક્ષર *sockbuf = ખાલી;
  uint8_t (*bufs)[સંદેશ_BUF_SIZE] = ખાલી;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_અંક * સંદેશ_BUF_SIZE,
                                          &sockbuf);
  જો(result)
    જાઓ out;
  bufs = (uint8_t (*)[સંદેશ_BUF_SIZE])sockbuf;

  total_nread = 0;
  જ્યારે(pkts < max_pkts) {
    n = (પૂર્ણાંક)CURLMIN(CURLMIN(MMSG_અંક, IOV_MAX), max_pkts);
    memset(&mmsg, 0, કદ(mmsg));
    માટે(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = કદ(bufs[i]);
      mmsg[i].સંદેશ_hdr.msg_iov = &msg_iov[i];
      mmsg[i].સંદેશ_hdr.સંદેશ_iovlen = 1;
      mmsg[i].સંદેશ_hdr.સંદેશ_name = &remote_addr[i];
      mmsg[i].સંદેશ_hdr.msg_namelen = કદ(remote_addr[i]);
      mmsg[i].સંદેશ_hdr.સંદેશ_control = &સંદેશ_ctrl[i * CMSG_PER_સંદેશ_SIZE];
      mmsg[i].સંદેશ_hdr.msg_controllen = CMSG_PER_સંદેશ_SIZE;
    }

    જ્યારે((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ખાલી)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    જો(mcount == -1) {
      જો(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        જાઓ out;
      }
      જો(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        સંરચના ip_quadruple ip;
        જો(!Curl_cf_socket_peek(cf->next, data, ખાલી, ખાલી, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        જાઓ out;
      }
      curlx_strerror(SOCKERRNO, errstr, કદ(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }

    VERBOSE(++calls);
    માટે(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      જો(!mmsg[i].સંદેશ_len) {
        ++pkts;
        ચાલુ;
      }
      total_nread += mmsg[i].સંદેશ_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].સંદેશ_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].સંદેશ_hdr);
      જો(gso_size == 0)
        gso_size = mmsg[i].સંદેશ_len;

      result = recv_cb(bufs[i], mmsg[i].સંદેશ_len, gso_size,
                       mmsg[i].સંદેશ_hdr.સંદેશ_name,
                       mmsg[i].સંદેશ_hdr.msg_namelen, ecn, userp);
      જો(result)
        જાઓ out;
      pkts += (mmsg[i].સંદેશ_len + gso_size - 1) / gso_size;
    }
  }

out:
  જો(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (પૂર્ણાંક)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  પાછા result;
}

#અથવાજો defined(HAVE_APPLE_સંદેશ_X)

સ્થિર CURLcode recvmsg_x_packets(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  શૂન્ય *userp, size_t *pnread)
{
#define સંદેશ_X_અંક  64
#define સંદેશ_BUF_SIZE  (2048)
#define CMSG_PER_સંદેશ_SIZE     CMSG_SPACE(કદ(પૂર્ણાંક))
  સંરચના iovec msg_iov[સંદેશ_X_અંક];
  સંરચના msghdr_x mmsg[સંદેશ_X_અંક];
  uint8_t સંદેશ_ctrl[સંદેશ_X_અંક * CMSG_PER_સંદેશ_SIZE];
  સંરચના sockaddr_storage remote_addr[સંદેશ_X_અંક];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  પૂર્ણાંક mcount, i;
  અક્ષર errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  અક્ષર *sockbuf = ખાલી;
  uint8_t (*bufs)[સંદેશ_BUF_SIZE] = ખાલી;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, સંદેશ_X_અંક * સંદેશ_BUF_SIZE,
                                          &sockbuf);
  જો(result)
    જાઓ out;
  bufs = (uint8_t (*)[સંદેશ_BUF_SIZE])sockbuf;

  total_nread = 0;
  જ્યારે(pkts < max_pkts) {
    પૂર્ણાંક n = (પૂર્ણાંક)CURLMIN(CURLMIN(સંદેશ_X_અંક, IOV_MAX), max_pkts);
    memset(&mmsg, 0, કદ(mmsg));
    માટે(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = કદ(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].સંદેશ_iovlen = 1;
      mmsg[i].સંદેશ_name = &remote_addr[i];
      mmsg[i].msg_namelen = કદ(remote_addr[i]);
      mmsg[i].સંદેશ_control = &સંદેશ_ctrl[i * CMSG_PER_સંદેશ_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_સંદેશ_SIZE;
    }

#જો defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    જ્યારે((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#જો defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic કાઢો
#endif
    જો(mcount == -1) {
      જો(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        જાઓ out;
      }
      જો(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        સંરચના ip_quadruple ip;
        જો(!Curl_cf_socket_peek(cf->next, data, ખાલી, ખાલી, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        જાઓ out;
      }
      curlx_strerror(SOCKERRNO, errstr, કદ(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }

    VERBOSE(++calls);
    માટે(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      જો(!mmsg[i].સંદેશ_datalen) {
        ++pkts;
        ચાલુ;
      }
      total_nread += mmsg[i].સંદેશ_datalen;
      ecn = vquic_msghdr_get_ecn((સંરચના msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].સંદેશ_datalen;

      result = recv_cb(bufs[i], mmsg[i].સંદેશ_datalen, gso_size,
                       mmsg[i].સંદેશ_name,
                       mmsg[i].msg_namelen, ecn, userp);
      જો(result)
        જાઓ out;
      pkts += (mmsg[i].સંદેશ_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  જો(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (પૂર્ણાંક)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  પાછા result;
}

#અથવાજો defined(HAVE_SENDMSG)
સ્થિર CURLcode recvmsg_packets(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, શૂન્ય *userp,
                                size_t *pnread)
{
#define CMSG_PER_સંદેશ_SIZE    CMSG_SPACE(કદ(પૂર્ણાંક))
  સંરચના iovec msg_iov;
  સંરચના msghdr સંદેશ;
  uint8_t buf[64 * 1024];
  સંરચના sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  અક્ષર errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t સંદેશ_ctrl[CMSG_PER_સંદેશ_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  માટે(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&સંદેશ, 0, કદ(સંદેશ));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = કદ(buf);
    સંદેશ.msg_iov = &msg_iov;
    સંદેશ.સંદેશ_iovlen = 1;
    સંદેશ.સંદેશ_control = સંદેશ_ctrl;
    સંદેશ.સંદેશ_name = &remote_addr;
    સંદેશ.msg_namelen = કદ(remote_addr);
    સંદેશ.msg_controllen = CMSG_PER_સંદેશ_SIZE;

    જ્યારે((rc = recvmsg(qctx->sockfd, &સંદેશ, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    જો(!curlx_sztouz(rc, &nread)) {
      જો(SOCK_EAGAIN(SOCKERRNO)) {
        જાઓ out;
      }
      જો(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        સંરચના ip_quadruple ip;
        જો(!Curl_cf_socket_peek(cf->next, data, ખાલી, ખાલી, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        જાઓ out;
      }
      curlx_strerror(SOCKERRNO, errstr, કદ(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    જો(!nread) {
      ++pkts;
      ચાલુ;
    }

    ecn = vquic_msghdr_get_ecn(&સંદેશ, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&સંદેશ);
    જો(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     સંદેશ.સંદેશ_name, સંદેશ.msg_namelen, ecn, userp);
    જો(result)
      જાઓ out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  જો(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (પૂર્ણાંક)result);
  *pnread = total_nread;
  પાછા result;
}

#નહીંતર /* HAVE_SENDMMSG || HAVE_SENDMSG */
સ્થિર CURLcode recvfrom_packets(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, શૂન્ય *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  પૂર્ણાંક bufsize = (પૂર્ણાંક)કદ(buf);
  સંરચના sockaddr_storage remote_addr;
  socklen_t remote_addrlen = કદ(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  અક્ષર errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  માટે(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    જ્યારે((rv = recvfrom(qctx->sockfd, (અક્ષર *)buf, bufsize, 0,
                         (સંરચના sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    જો(!curlx_sztouz(rv, &nread)) {
      જો(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        જાઓ out;
      }
      જો(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        સંરચના ip_quadruple ip;
        જો(!Curl_cf_socket_peek(cf->next, data, ખાલી, ખાલી, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        જાઓ out;
      }
      curlx_strerror(SOCKERRNO, errstr, કદ(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    જો(!nread)
      ચાલુ;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    જો(result)
      જાઓ out;
  }

out:
  જો(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (પૂર્ણાંક)result);
  *pnread = total_nread;
  પાછા result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, શૂન્ય *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#અથવાજો defined(HAVE_APPLE_સંદેશ_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#અથવાજો defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#નહીંતર
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  જો(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    જો(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  પાછા result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(સંરચના Curl_easy *data,
                      unsigned અક્ષર *scid,
                      size_t scidlen,
                      પૂર્ણાંક *qlogfdp)
{
  અક્ષર *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  જો(qlog_dir) {
    સંરચના dynbuf fname;
    CURLcode result;
    unsigned પૂર્ણાંક i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    જો(!result)
      result = curlx_dyn_add(&fname, "/");
    માટે(i = 0; (i < scidlen) && !result; i++) {
      અક્ષર hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    જો(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    જો(!result) {
      પૂર્ણાંક qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_દ્વિઅંકી,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#નહીંતર
                              (mode_t)data->set.new_file_perms
#endif
                              );
      જો(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    જો(result)
      પાછા result;
  }

  પાછા CURLE_OK;
}

CURLcode Curl_cf_quic_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                   સંરચના Curl_easy *data,
                                   સંરચના Curl_peer *origin,
                                   સંરચના Curl_peer *peer)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  (શૂન્ય)data; /* not used in all cases and compilers are stupid */
#જો defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_ઉમેરો_after(cf_at, origin, peer, &cf);
#અથવાજો defined(USE_QUICHE)
  result = Curl_cf_quiche_ઉમેરો_after(cf_at, origin, peer, &cf);
#નહીંતર
  (શૂન્ય)cf)
  (શૂન્ય)cf_at;
  (શૂન્ય)origin;
  (શૂન્ય)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#જો defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  જો(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  પાછા result;
}

CURLcode Curl_cf_quic_create(સંરચના Curl_cfilter **pcf,
                             સંરચના Curl_easy *data,
                             સંરચના Curl_peer *origin,
                             સંરચના Curl_peer *peer,
                             uint8_t transport_peer,
                             સંરચના connectdata *conn,
                             સંરચના Curl_sockaddr_ex *addr,
                             સંરચના Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (શૂન્ય)transport_peer;
  (શૂન્ય)tunnel_transport;
  (શૂન્ય)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#જો defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  પાછા Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#અથવાજો defined(USE_QUICHE)
  પાછા Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#નહીંતર
  *pcf = ખાલી;
  (શૂન્ય)data;
  (શૂન્ય)origin;
  (શૂન્ય)peer;
  (શૂન્ય)conn;
  (શૂન્ય)addr;
  (શૂન્ય)tunnel_peer;
  (શૂન્ય)tunnel_transport;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

#જો !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                       સંરચના Curl_easy *data,
                                       સંરચના Curl_peer *origin,
                                       સંરચના Curl_peer *peer,
                                       સંરચના Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#જો defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  પાછા Curl_cf_ngtcp2_proxy_ઉમેરો_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#નહીંતર
  (શૂન્ય)cf_at;
  (શૂન્ય)data;
  (શૂન્ય)origin;
  (શૂન્ય)peer;
  (શૂન્ય)tunnel_peer;
  (શૂન્ય)tunnel_transport;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(સંરચના Curl_cfilter **pcf,
                                 સંરચના Curl_easy *data,
                                 સંરચના Curl_peer *origin,
                                 સંરચના Curl_peer *peer,
                                 uint8_t transport_peer,
                                 સંરચના connectdata *conn,
                                 સંરચના Curl_sockaddr_ex *addr,
                                 સંરચના Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#જો defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  પાછા Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#નહીંતર
  *pcf = ખાલી;
  (શૂન્ય)data;
  (શૂન્ય)conn;
  (શૂન્ય)addr;
  (શૂન્ય)peer;
  (શૂન્ય)transport_peer;
  (શૂન્ય)tunnel_peer;
  (શૂન્ય)tunnel_transport;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(સંરચના Curl_easy *data,
                             સંરચના connectdata *conn)
{
  જો(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }
  જો(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    પાછા CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  જો(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    પાછા CURLE_URL_MALFORMAT;
  }
#નહીંતર
  (શૂન્ય)conn;
#endif

  પાછા CURLE_OK;
}

#ifdef CURLVERBOSE
અચલ અક્ષર *Curl_vquic_h3_err_str(uint64_t error_code)
{
  જો(error_code <= UINT_MAX) {
    પસંદગી((unsigned પૂર્ણાંક)error_code) {
    સ્થિતિ CURL_H3_ERR_NO_ERROR:
      પાછા "NO_ERROR";
    સ્થિતિ CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      પાછા "GENERAL_PROTOCOL_ERROR";
    સ્થિતિ CURL_H3_ERR_INTERNAL_ERROR:
      પાછા "INTERNAL_ERROR";
    સ્થિતિ CURL_H3_ERR_STREAM_CREATION_ERROR:
      પાછા "STREAM_CREATION_ERROR";
    સ્થિતિ CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      પાછા "CLOSED_CRITICAL_STREAM";
    સ્થિતિ CURL_H3_ERR_FRAME_UNEXPECTED:
      પાછા "FRAME_UNEXPECTED";
    સ્થિતિ CURL_H3_ERR_FRAME_ERROR:
      પાછા "FRAME_ERROR";
    સ્થિતિ CURL_H3_ERR_EXCESSIVE_LOAD:
      પાછા "EXCESSIVE_LOAD";
    સ્થિતિ CURL_H3_ERR_ID_ERROR:
      પાછા "ID_ERROR";
    સ્થિતિ CURL_H3_ERR_SETTINGS_ERROR:
      પાછા "SETTINGS_ERROR";
    સ્થિતિ CURL_H3_ERR_MISSING_SETTINGS:
      પાછા "MISSING_SETTINGS";
    સ્થિતિ CURL_H3_ERR_REQUEST_REJECTED:
      પાછા "REQUEST_REJECTED";
    સ્થિતિ CURL_H3_ERR_REQUEST_CANCELLED:
      પાછા "REQUEST_CANCELLED";
    સ્થિતિ CURL_H3_ERR_REQUEST_INCOMPLETE:
      પાછા "REQUEST_INCOMPLETE";
    સ્થિતિ CURL_H3_ERR_MESSAGE_ERROR:
      પાછા "MESSAGE_ERROR";
    સ્થિતિ CURL_H3_ERR_CONNECT_ERROR:
      પાછા "CONNECT_ERROR";
    સ્થિતિ CURL_H3_ERR_VERSION_FALLBACK:
      પાછા "VERSION_FALLBACK";
    મૂળભૂત:
      તોડો;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  જો((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    પાછા "NO_ERROR";
  પાછા "unknown";
}
#endif /* CURLVERBOSE */

#જો defined(USE_NGTCP2) || defined(USE_NGHTTP3)

સ્થિર શૂન્ય *vquic_ngtcp2_malloc(size_t size, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  પાછા Curl_cmalloc(size);
}

સ્થિર શૂન્ય vquic_ngtcp2_free(શૂન્ય *ptr, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  Curl_cfree(ptr);
}

સ્થિર શૂન્ય *vquic_ngtcp2_calloc(size_t nmemb, size_t size, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  પાછા Curl_ccalloc(nmemb, size);
}

સ્થિર શૂન્ય *vquic_ngtcp2_realloc(શૂન્ય *ptr, size_t size, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  પાછા Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
સ્થિર સંરચના ngtcp2_mem vquic_ngtcp2_mem = {
  ખાલી,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
સંરચના ngtcp2_mem *Curl_ngtcp2_mem(શૂન્ય)
{
  પાછા &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
સ્થિર સંરચના nghttp3_mem vquic_nghttp3_mem = {
  ખાલી,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
સંરચના nghttp3_mem *Curl_nghttp3_mem(શૂન્ય)
{
  પાછા &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#નહીંતર /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(સંરચના Curl_easy *data,
                             સંરચના connectdata *conn)
{
  (શૂન્ય)data;
  (શૂન્ય)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  પાછા CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
