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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "vtls/vtls.h"

#அந்தர்பாவ "curl_trc.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#அந்தர்பாவ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#அந்தர்பாவ <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#அந்தர்பாவ <nghttp3/nghttp3.h>
#endif

#எனில் defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#அந்தர்பாவ <sys/syscall.h>
#எனில் defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_செய்தி_X
#endif
#endif

#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vquic/cf-ngtcp2.h"
#அந்தர்பாவ "vquic/cf-ngtcp2-cmn.h"
#அந்தர்பாவ "vquic/cf-ngtcp2-proxy.h"
#அந்தர்பாவ "vquic/cf-quiche.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "vquic/vquic_int.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_செய்தி_X

/* this is `struct msghdr` with an additional field at the end */
கட்டமைப்பு msghdr_x {
  வெறுமை *செய்தி_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  கட்டமைப்பு iovec *msg_iov;    /* scatter/gather array */
  முழு செய்தி_iovlen;           /* # elements in msg_iov */
  வெறுமை *செய்தி_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  முழு செய்தி_flags;            /* flags on received message */
  size_t செய்தி_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_செய்தி_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#அல்லதெனில் defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#இல்லையெனில்
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

முழு Curl_vquic_init(வெறுமை)
{
#எனில் defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  எனில்(ngtcp2_crypto_ossl_init())
    திரும்பு 0;
#endif

  திரும்பு 1;
}

வெறுமை Curl_vquic_cleanup(வெறுமை)
{
#எனில் defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_இலக்கம் >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

வெறுமை Curl_quic_ver(எழுத்து *p, size_t len)
{
#எனில் defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#அல்லதெனில் defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#எனில் defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#அல்லதெனில் defined(HAVE_APPLE_செய்தி_X)
  qctx->no_gso = FALSE;
#இல்லையெனில்
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    மாறிலி எழுத்து *p = getenv("CURL_DBG_QUIC_WBLOCK");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, 100))
        qctx->wblock_percent = (முழு)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  திரும்பு CURLE_OK;
}

வெறுமை Curl_vquic_ctx_free(கட்டமைப்பு cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

வெறுமை Curl_vquic_ctx_set_time(கட்டமைப்பு cf_quic_ctx *qctx,
                             மாறிலி கட்டமைப்பு curltime *pnow)
{
  qctx->last_op = *pnow;
}

வெறுமை Curl_vquic_ctx_update_time(கட்டமைப்பு cf_quic_ctx *qctx,
                                மாறிலி கட்டமைப்பு curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_செய்தி_X
நிலையான CURLcode do_sendmsg(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு cf_quic_ctx *qctx,
                           மாறிலி uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define செய்தி_X_SNUM  64
  கட்டமைப்பு iovec msg_iov[செய்தி_X_SNUM];
  கட்டமைப்பு msghdr_x mmsg[செய்தி_X_SNUM];
  எழுத்து errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  முழு rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  எனில்(!pktlen)
    திரும்பு CURLE_OK;
  எனில்(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  போது(i < n) {
    size_t j, batch = CURLMIN(n - i, செய்தி_X_SNUM), pkts_sent = 0;

    க்கு(j = 0; j < batch; ++j) {
      மாறிலி size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, அளவு(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].செய்தி_iovlen = 1;
      mmsg[j].செய்தி_datalen = msg_iov[j].iov_len;
    }

#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    போது((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic எடு
#endif

    எனில்(rc < 0) {
      எனில்(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        செல் out;
      }
      எனில்(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, அளவு(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        செல் out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    இல்லையெனில்
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    எனில்(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      செல் out;
    }
    i += pkts_sent; /* Some have been sent */
    க்கு(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    எனில்(pkts_sent < batch)
      செல் out; /* but not all of them */
  }

out:
  *psent = sent;
  எனில்(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (முழு)result);
  திரும்பு result;
}

#இல்லையெனில் /* HAVE_APPLE_MSG_X */

நிலையான CURLcode send_packet_no_gso(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு cf_quic_ctx *qctx,
                                   மாறிலி uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

நிலையான CURLcode do_sendmsg(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு cf_quic_ctx *qctx,
                           மாறிலி uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  கட்டமைப்பு iovec msg_iov;
  கட்டமைப்பு msghdr செய்தி = { 0 };
  ssize_t rv;
#எனில் defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t செய்தி_ctrl[32];
  கட்டமைப்பு cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  செய்தி.msg_iov = &msg_iov;
  செய்தி.செய்தி_iovlen = 1;

#எனில் defined(__linux__) && defined(UDP_SEGMENT)
  எனில்(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(செய்தி_ctrl, 0, அளவு(செய்தி_ctrl));
    செய்தி.செய்தி_control = செய்தி_ctrl;
    DEBUGASSERT(அளவு(செய்தி_ctrl) >= CMSG_SPACE(அளவு(முழு)));
    செய்தி.msg_controllen = CMSG_SPACE(அளவு(முழு));
    cm = CMSG_FIRSTHDR(&செய்தி);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(அளவு(uint16_t));
    *(uint16_t *)(வெறுமை *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  போது((rv = sendmsg(qctx->sockfd, &செய்தி, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  எனில்(!curlx_sztouz(rv, psent)) {
    முழு sockerr = SOCKERRNO;
    எனில்(SOCK_EAGAIN(sockerr))
      திரும்பு CURLE_AGAIN;
    தேர்வு(sockerr) {
    நிலை SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      நிறுத்து;
    நிலை EIO:
      எனில்(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        திரும்பு send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    இயல்பு:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }
  இல்லையெனில் எனில்(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    செல் out;
  }
#இல்லையெனில்
  ssize_t rv;
  (வெறுமை)gsolen;

  *psent = 0;

  போது((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  எனில்(!curlx_sztouz(rv, psent)) {
    எனில்(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      செல் out;
    }
    இல்லையெனில் {
      எனில்(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        செல் out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (வெறுமை)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (முழு)result, *psent);
  திரும்பு result;
}

#endif /* !HAVE_APPLE_MSG_X */

நிலையான CURLcode send_packet_no_gso(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு cf_quic_ctx *qctx,
                                   மாறிலி uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  மாறிலி uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  க்கு(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    எனில்(result)
      செல் out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (முழு)result, *psent);
  திரும்பு result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
நிலையான CURLcode send_packet_no_gso_cf(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      மாறிலி uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  மாறிலி uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  க்கு(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    எனில்(result)
      செல் out;

    எனில்(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (முழு)result, *psent);
  திரும்பு result;
}

நிலையான CURLcode vquic_send_packets(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு cf_quic_ctx *qctx,
                                   மாறிலி uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  எனில்(qctx->wblock_percent > 0) {
    unsigned எழுத்து c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    எனில்(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      திரும்பு CURLE_AGAIN;
    }
  }
#endif
  எனில்(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  இல்லையெனில் {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  எனில்(!result)
    qctx->last_io = qctx->last_op;
  திரும்பு result;
}

CURLcode Curl_vquic_flush(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு cf_quic_ctx *qctx)
{
  மாறிலி unsigned எழுத்து *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  போது(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    எனில்(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      எனில்(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    எனில்(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    இல்லையெனில் {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      எனில்(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      இல்லையெனில் {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    எனில்(result) {
      எனில்(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        எனில்(qctx->split_len)
          qctx->split_len -= sent;
      }
      திரும்பு result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    எனில்(qctx->split_len)
      qctx->split_len -= sent;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_vquic_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  திரும்பு Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  திரும்பு Curl_vquic_flush(cf, data, qctx);
}

#எனில் (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_செய்தி_X)
நிலையான size_t vquic_msghdr_get_udp_gro(கட்டமைப்பு msghdr *செய்தி)
{
  முழு gso_size = 0;
#எனில் defined(__linux__) && defined(UDP_GRO)
  கட்டமைப்பு cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#எனில் defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  க்கு(cmsg = CMSG_FIRSTHDR(செய்தி); cmsg; cmsg = CMSG_NXTHDR(செய்தி, cmsg)) {
#எனில் defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic எடு
#endif
    எனில்(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), அளவு(gso_size));

      நிறுத்து;
    }
  }
#endif /* linux && UDP_GRO */
  (வெறுமை)செய்தி;

  திரும்பு (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#எனில் (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_செய்தி_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
நிலையான uint8_t vquic_msghdr_get_ecn(கட்டமைப்பு msghdr *செய்தி, முழு family)
{
  கட்டமைப்பு cmsghdr *cmsg;
  தேர்வு(family) {
  நிலை AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#எனில் defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    க்கு(cmsg = CMSG_FIRSTHDR(செய்தி); cmsg; cmsg = CMSG_NXTHDR(செய்தி, cmsg)) {
#எனில் defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic எடு
#endif
      எனில்(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#இல்லையெனில்
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        திரும்பு *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    நிறுத்து;
  நிலை AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#எனில் defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    க்கு(cmsg = CMSG_FIRSTHDR(செய்தி); cmsg; cmsg = CMSG_NXTHDR(செய்தி, cmsg)) {
#எனில் defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic எடு
#endif
      எனில்(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned முழு tos;

        memcpy(&tos, CMSG_DATA(cmsg), அளவு(முழு));

        திரும்பு (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    நிறுத்து;
  }
  திரும்பு 0;
}
#இல்லையெனில்
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

நிலையான CURLcode recvmmsg_packets(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, வெறுமை *userp,
                                 size_t *pnread)
{
#எனில் defined(__linux__) && defined(UDP_GRO)
#define MMSG_இலக்கம்  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_செய்தி_SIZE    (2 * CMSG_SPACE(அளவு(முழு)))
#இல்லையெனில்
#define MMSG_இலக்கம்  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_செய்தி_SIZE    CMSG_SPACE(அளவு(முழு))
#endif
#define செய்தி_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  கட்டமைப்பு iovec msg_iov[MMSG_இலக்கம்];
  கட்டமைப்பு mmsghdr mmsg[MMSG_இலக்கம்];
  uint8_t செய்தி_ctrl[MMSG_இலக்கம் * CMSG_PER_செய்தி_SIZE];
  கட்டமைப்பு sockaddr_storage remote_addr[MMSG_இலக்கம்];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  முழு mcount, i, n;
  எழுத்து errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  எழுத்து *sockbuf = வெற்று;
  uint8_t (*bufs)[செய்தி_BUF_SIZE] = வெற்று;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_இலக்கம் * செய்தி_BUF_SIZE,
                                          &sockbuf);
  எனில்(result)
    செல் out;
  bufs = (uint8_t (*)[செய்தி_BUF_SIZE])sockbuf;

  total_nread = 0;
  போது(pkts < max_pkts) {
    n = (முழு)CURLMIN(CURLMIN(MMSG_இலக்கம், IOV_MAX), max_pkts);
    memset(&mmsg, 0, அளவு(mmsg));
    க்கு(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = அளவு(bufs[i]);
      mmsg[i].செய்தி_hdr.msg_iov = &msg_iov[i];
      mmsg[i].செய்தி_hdr.செய்தி_iovlen = 1;
      mmsg[i].செய்தி_hdr.செய்தி_name = &remote_addr[i];
      mmsg[i].செய்தி_hdr.msg_namelen = அளவு(remote_addr[i]);
      mmsg[i].செய்தி_hdr.செய்தி_control = &செய்தி_ctrl[i * CMSG_PER_செய்தி_SIZE];
      mmsg[i].செய்தி_hdr.msg_controllen = CMSG_PER_செய்தி_SIZE;
    }

    போது((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, வெற்று)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    எனில்(mcount == -1) {
      எனில்(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        செல் out;
      }
      எனில்(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        கட்டமைப்பு ip_quadruple ip;
        எனில்(!Curl_cf_socket_peek(cf->next, data, வெற்று, வெற்று, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        செல் out;
      }
      curlx_strerror(SOCKERRNO, errstr, அளவு(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      செல் out;
    }

    VERBOSE(++calls);
    க்கு(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      எனில்(!mmsg[i].செய்தி_len) {
        ++pkts;
        தொடர்;
      }
      total_nread += mmsg[i].செய்தி_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].செய்தி_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].செய்தி_hdr);
      எனில்(gso_size == 0)
        gso_size = mmsg[i].செய்தி_len;

      result = recv_cb(bufs[i], mmsg[i].செய்தி_len, gso_size,
                       mmsg[i].செய்தி_hdr.செய்தி_name,
                       mmsg[i].செய்தி_hdr.msg_namelen, ecn, userp);
      எனில்(result)
        செல் out;
      pkts += (mmsg[i].செய்தி_len + gso_size - 1) / gso_size;
    }
  }

out:
  எனில்(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (முழு)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  திரும்பு result;
}

#அல்லதெனில் defined(HAVE_APPLE_செய்தி_X)

நிலையான CURLcode recvmsg_x_packets(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  வெறுமை *userp, size_t *pnread)
{
#define செய்தி_X_இலக்கம்  64
#define செய்தி_BUF_SIZE  (2048)
#define CMSG_PER_செய்தி_SIZE     CMSG_SPACE(அளவு(முழு))
  கட்டமைப்பு iovec msg_iov[செய்தி_X_இலக்கம்];
  கட்டமைப்பு msghdr_x mmsg[செய்தி_X_இலக்கம்];
  uint8_t செய்தி_ctrl[செய்தி_X_இலக்கம் * CMSG_PER_செய்தி_SIZE];
  கட்டமைப்பு sockaddr_storage remote_addr[செய்தி_X_இலக்கம்];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  முழு mcount, i;
  எழுத்து errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  எழுத்து *sockbuf = வெற்று;
  uint8_t (*bufs)[செய்தி_BUF_SIZE] = வெற்று;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, செய்தி_X_இலக்கம் * செய்தி_BUF_SIZE,
                                          &sockbuf);
  எனில்(result)
    செல் out;
  bufs = (uint8_t (*)[செய்தி_BUF_SIZE])sockbuf;

  total_nread = 0;
  போது(pkts < max_pkts) {
    முழு n = (முழு)CURLMIN(CURLMIN(செய்தி_X_இலக்கம், IOV_MAX), max_pkts);
    memset(&mmsg, 0, அளவு(mmsg));
    க்கு(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = அளவு(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].செய்தி_iovlen = 1;
      mmsg[i].செய்தி_name = &remote_addr[i];
      mmsg[i].msg_namelen = அளவு(remote_addr[i]);
      mmsg[i].செய்தி_control = &செய்தி_ctrl[i * CMSG_PER_செய்தி_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_செய்தி_SIZE;
    }

#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    போது((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#எனில் defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic எடு
#endif
    எனில்(mcount == -1) {
      எனில்(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        செல் out;
      }
      எனில்(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        கட்டமைப்பு ip_quadruple ip;
        எனில்(!Curl_cf_socket_peek(cf->next, data, வெற்று, வெற்று, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        செல் out;
      }
      curlx_strerror(SOCKERRNO, errstr, அளவு(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      செல் out;
    }

    VERBOSE(++calls);
    க்கு(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      எனில்(!mmsg[i].செய்தி_datalen) {
        ++pkts;
        தொடர்;
      }
      total_nread += mmsg[i].செய்தி_datalen;
      ecn = vquic_msghdr_get_ecn((கட்டமைப்பு msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].செய்தி_datalen;

      result = recv_cb(bufs[i], mmsg[i].செய்தி_datalen, gso_size,
                       mmsg[i].செய்தி_name,
                       mmsg[i].msg_namelen, ecn, userp);
      எனில்(result)
        செல் out;
      pkts += (mmsg[i].செய்தி_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  எனில்(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (முழு)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  திரும்பு result;
}

#அல்லதெனில் defined(HAVE_SENDMSG)
நிலையான CURLcode recvmsg_packets(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, வெறுமை *userp,
                                size_t *pnread)
{
#define CMSG_PER_செய்தி_SIZE    CMSG_SPACE(அளவு(முழு))
  கட்டமைப்பு iovec msg_iov;
  கட்டமைப்பு msghdr செய்தி;
  uint8_t buf[64 * 1024];
  கட்டமைப்பு sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  எழுத்து errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t செய்தி_ctrl[CMSG_PER_செய்தி_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  க்கு(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&செய்தி, 0, அளவு(செய்தி));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = அளவு(buf);
    செய்தி.msg_iov = &msg_iov;
    செய்தி.செய்தி_iovlen = 1;
    செய்தி.செய்தி_control = செய்தி_ctrl;
    செய்தி.செய்தி_name = &remote_addr;
    செய்தி.msg_namelen = அளவு(remote_addr);
    செய்தி.msg_controllen = CMSG_PER_செய்தி_SIZE;

    போது((rc = recvmsg(qctx->sockfd, &செய்தி, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    எனில்(!curlx_sztouz(rc, &nread)) {
      எனில்(SOCK_EAGAIN(SOCKERRNO)) {
        செல் out;
      }
      எனில்(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        கட்டமைப்பு ip_quadruple ip;
        எனில்(!Curl_cf_socket_peek(cf->next, data, வெற்று, வெற்று, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        செல் out;
      }
      curlx_strerror(SOCKERRNO, errstr, அளவு(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      செல் out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    எனில்(!nread) {
      ++pkts;
      தொடர்;
    }

    ecn = vquic_msghdr_get_ecn(&செய்தி, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&செய்தி);
    எனில்(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     செய்தி.செய்தி_name, செய்தி.msg_namelen, ecn, userp);
    எனில்(result)
      செல் out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  எனில்(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (முழு)result);
  *pnread = total_nread;
  திரும்பு result;
}

#இல்லையெனில் /* HAVE_SENDMMSG || HAVE_SENDMSG */
நிலையான CURLcode recvfrom_packets(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, வெறுமை *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  முழு bufsize = (முழு)அளவு(buf);
  கட்டமைப்பு sockaddr_storage remote_addr;
  socklen_t remote_addrlen = அளவு(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  எழுத்து errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  க்கு(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    போது((rv = recvfrom(qctx->sockfd, (எழுத்து *)buf, bufsize, 0,
                         (கட்டமைப்பு sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    எனில்(!curlx_sztouz(rv, &nread)) {
      எனில்(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        செல் out;
      }
      எனில்(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        கட்டமைப்பு ip_quadruple ip;
        எனில்(!Curl_cf_socket_peek(cf->next, data, வெற்று, வெற்று, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        செல் out;
      }
      curlx_strerror(SOCKERRNO, errstr, அளவு(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      செல் out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    எனில்(!nread)
      தொடர்;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    எனில்(result)
      செல் out;
  }

out:
  எனில்(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (முழு)result);
  *pnread = total_nread;
  திரும்பு result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, வெறுமை *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#அல்லதெனில் defined(HAVE_APPLE_செய்தி_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#அல்லதெனில் defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#இல்லையெனில்
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  எனில்(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    எனில்(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  திரும்பு result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(கட்டமைப்பு Curl_easy *data,
                      unsigned எழுத்து *scid,
                      size_t scidlen,
                      முழு *qlogfdp)
{
  எழுத்து *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  எனில்(qlog_dir) {
    கட்டமைப்பு dynbuf fname;
    CURLcode result;
    unsigned முழு i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    எனில்(!result)
      result = curlx_dyn_add(&fname, "/");
    க்கு(i = 0; (i < scidlen) && !result; i++) {
      எழுத்து hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    எனில்(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    எனில்(!result) {
      முழு qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_இருமம்,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#இல்லையெனில்
                              (mode_t)data->set.new_file_perms
#endif
                              );
      எனில்(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    எனில்(result)
      திரும்பு result;
  }

  திரும்பு CURLE_OK;
}

CURLcode Curl_cf_quic_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_peer *origin,
                                   கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  (வெறுமை)data; /* not used in all cases and compilers are stupid */
#எனில் defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_செருகு_after(cf_at, origin, peer, &cf);
#அல்லதெனில் defined(USE_QUICHE)
  result = Curl_cf_quiche_செருகு_after(cf_at, origin, peer, &cf);
#இல்லையெனில்
  (வெறுமை)cf)
  (வெறுமை)cf_at;
  (வெறுமை)origin;
  (வெறுமை)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#எனில் defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  எனில்(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  திரும்பு result;
}

CURLcode Curl_cf_quic_create(கட்டமைப்பு Curl_cfilter **pcf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_peer *origin,
                             கட்டமைப்பு Curl_peer *peer,
                             uint8_t transport_peer,
                             கட்டமைப்பு connectdata *conn,
                             கட்டமைப்பு Curl_sockaddr_ex *addr,
                             கட்டமைப்பு Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (வெறுமை)transport_peer;
  (வெறுமை)tunnel_transport;
  (வெறுமை)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#எனில் defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  திரும்பு Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#அல்லதெனில் defined(USE_QUICHE)
  திரும்பு Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#இல்லையெனில்
  *pcf = வெற்று;
  (வெறுமை)data;
  (வெறுமை)origin;
  (வெறுமை)peer;
  (வெறுமை)conn;
  (வெறுமை)addr;
  (வெறுமை)tunnel_peer;
  (வெறுமை)tunnel_transport;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

#எனில் !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_peer *origin,
                                       கட்டமைப்பு Curl_peer *peer,
                                       கட்டமைப்பு Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#எனில் defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  திரும்பு Curl_cf_ngtcp2_proxy_செருகு_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#இல்லையெனில்
  (வெறுமை)cf_at;
  (வெறுமை)data;
  (வெறுமை)origin;
  (வெறுமை)peer;
  (வெறுமை)tunnel_peer;
  (வெறுமை)tunnel_transport;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(கட்டமைப்பு Curl_cfilter **pcf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_peer *origin,
                                 கட்டமைப்பு Curl_peer *peer,
                                 uint8_t transport_peer,
                                 கட்டமைப்பு connectdata *conn,
                                 கட்டமைப்பு Curl_sockaddr_ex *addr,
                                 கட்டமைப்பு Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#எனில் defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  திரும்பு Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#இல்லையெனில்
  *pcf = வெற்று;
  (வெறுமை)data;
  (வெறுமை)conn;
  (வெறுமை)addr;
  (வெறுமை)peer;
  (வெறுமை)transport_peer;
  (வெறுமை)tunnel_peer;
  (வெறுமை)tunnel_transport;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn)
{
  எனில்(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }
  எனில்(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    திரும்பு CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  எனில்(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    திரும்பு CURLE_URL_MALFORMAT;
  }
#இல்லையெனில்
  (வெறுமை)conn;
#endif

  திரும்பு CURLE_OK;
}

#ifdef CURLVERBOSE
மாறிலி எழுத்து *Curl_vquic_h3_err_str(uint64_t error_code)
{
  எனில்(error_code <= UINT_MAX) {
    தேர்வு((unsigned முழு)error_code) {
    நிலை CURL_H3_ERR_NO_ERROR:
      திரும்பு "NO_ERROR";
    நிலை CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      திரும்பு "GENERAL_PROTOCOL_ERROR";
    நிலை CURL_H3_ERR_INTERNAL_ERROR:
      திரும்பு "INTERNAL_ERROR";
    நிலை CURL_H3_ERR_STREAM_CREATION_ERROR:
      திரும்பு "STREAM_CREATION_ERROR";
    நிலை CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      திரும்பு "CLOSED_CRITICAL_STREAM";
    நிலை CURL_H3_ERR_FRAME_UNEXPECTED:
      திரும்பு "FRAME_UNEXPECTED";
    நிலை CURL_H3_ERR_FRAME_ERROR:
      திரும்பு "FRAME_ERROR";
    நிலை CURL_H3_ERR_EXCESSIVE_LOAD:
      திரும்பு "EXCESSIVE_LOAD";
    நிலை CURL_H3_ERR_ID_ERROR:
      திரும்பு "ID_ERROR";
    நிலை CURL_H3_ERR_SETTINGS_ERROR:
      திரும்பு "SETTINGS_ERROR";
    நிலை CURL_H3_ERR_MISSING_SETTINGS:
      திரும்பு "MISSING_SETTINGS";
    நிலை CURL_H3_ERR_REQUEST_REJECTED:
      திரும்பு "REQUEST_REJECTED";
    நிலை CURL_H3_ERR_REQUEST_CANCELLED:
      திரும்பு "REQUEST_CANCELLED";
    நிலை CURL_H3_ERR_REQUEST_INCOMPLETE:
      திரும்பு "REQUEST_INCOMPLETE";
    நிலை CURL_H3_ERR_MESSAGE_ERROR:
      திரும்பு "MESSAGE_ERROR";
    நிலை CURL_H3_ERR_CONNECT_ERROR:
      திரும்பு "CONNECT_ERROR";
    நிலை CURL_H3_ERR_VERSION_FALLBACK:
      திரும்பு "VERSION_FALLBACK";
    இயல்பு:
      நிறுத்து;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  எனில்((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    திரும்பு "NO_ERROR";
  திரும்பு "unknown";
}
#endif /* CURLVERBOSE */

#எனில் defined(USE_NGTCP2) || defined(USE_NGHTTP3)

நிலையான வெறுமை *vquic_ngtcp2_malloc(size_t size, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  திரும்பு Curl_cmalloc(size);
}

நிலையான வெறுமை vquic_ngtcp2_free(வெறுமை *ptr, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  Curl_cfree(ptr);
}

நிலையான வெறுமை *vquic_ngtcp2_calloc(size_t nmemb, size_t size, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  திரும்பு Curl_ccalloc(nmemb, size);
}

நிலையான வெறுமை *vquic_ngtcp2_realloc(வெறுமை *ptr, size_t size, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  திரும்பு Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
நிலையான கட்டமைப்பு ngtcp2_mem vquic_ngtcp2_mem = {
  வெற்று,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
கட்டமைப்பு ngtcp2_mem *Curl_ngtcp2_mem(வெறுமை)
{
  திரும்பு &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
நிலையான கட்டமைப்பு nghttp3_mem vquic_nghttp3_mem = {
  வெற்று,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
கட்டமைப்பு nghttp3_mem *Curl_nghttp3_mem(வெறுமை)
{
  திரும்பு &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#இல்லையெனில் /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn)
{
  (வெறுமை)data;
  (வெறுமை)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  திரும்பு CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
