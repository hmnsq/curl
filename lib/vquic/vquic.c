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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "vtls/vtls.h"

#അന്തര്ഭാവ "curl_trc.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#അന്തര്ഭാവ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#അന്തര്ഭാവ <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#അന്തര്ഭാവ <nghttp3/nghttp3.h>
#endif

#എങ്കിൽ defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#അന്തര്ഭാവ <sys/syscall.h>
#എങ്കിൽ defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_സന്ദേശം_X
#endif
#endif

#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2-cmn.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2-proxy.h"
#അന്തര്ഭാവ "vquic/cf-quiche.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "vquic/vquic_int.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_സന്ദേശം_X

/* this is `struct msghdr` with an additional field at the end */
ഘടന msghdr_x {
  ശൂന്യം *സന്ദേശം_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ഘടന iovec *msg_iov;    /* scatter/gather array */
  പൂർണ്ണസംഖ്യ സന്ദേശം_iovlen;           /* # elements in msg_iov */
  ശൂന്യം *സന്ദേശം_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  പൂർണ്ണസംഖ്യ സന്ദേശം_flags;            /* flags on received message */
  size_t സന്ദേശം_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_സന്ദേശം_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#അല്ലെങ്കിൽ
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

പൂർണ്ണസംഖ്യ Curl_vquic_init(ശൂന്യം)
{
#എങ്കിൽ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  എങ്കിൽ(ngtcp2_crypto_ossl_init())
    തിരികെ 0;
#endif

  തിരികെ 1;
}

ശൂന്യം Curl_vquic_cleanup(ശൂന്യം)
{
#എങ്കിൽ defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_അക്കം >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

ശൂന്യം Curl_quic_ver(അക്ഷരം *p, size_t len)
{
#എങ്കിൽ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(ഘടന Curl_easy *data,
                             ഘടന cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#എങ്കിൽ defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_APPLE_സന്ദേശം_X)
  qctx->no_gso = FALSE;
#അല്ലെങ്കിൽ
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_DBG_QUIC_WBLOCK");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, 100))
        qctx->wblock_percent = (പൂർണ്ണസംഖ്യ)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_vquic_ctx_free(ഘടന cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

ശൂന്യം Curl_vquic_ctx_set_time(ഘടന cf_quic_ctx *qctx,
                             സ്ഥിരം ഘടന curltime *pnow)
{
  qctx->last_op = *pnow;
}

ശൂന്യം Curl_vquic_ctx_update_time(ഘടന cf_quic_ctx *qctx,
                                സ്ഥിരം ഘടന curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_സന്ദേശം_X
സ്ഥിതം CURLcode do_sendmsg(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           ഘടന cf_quic_ctx *qctx,
                           സ്ഥിരം uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define സന്ദേശം_X_SNUM  64
  ഘടന iovec msg_iov[സന്ദേശം_X_SNUM];
  ഘടന msghdr_x mmsg[സന്ദേശം_X_SNUM];
  അക്ഷരം errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  പൂർണ്ണസംഖ്യ rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  എങ്കിൽ(!pktlen)
    തിരികെ CURLE_OK;
  എങ്കിൽ(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  ആകുമ്പോൾ(i < n) {
    size_t j, batch = CURLMIN(n - i, സന്ദേശം_X_SNUM), pkts_sent = 0;

    വേണ്ടി(j = 0; j < batch; ++j) {
      സ്ഥിരം size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, വലുപ്പം(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].സന്ദേശം_iovlen = 1;
      mmsg[j].സന്ദേശം_datalen = msg_iov[j].iov_len;
    }

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ആകുമ്പോൾ((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic എടുക്കുക
#endif

    എങ്കിൽ(rc < 0) {
      എങ്കിൽ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        പോകുക out;
      }
      എങ്കിൽ(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, വലുപ്പം(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        പോകുക out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    അല്ലെങ്കിൽ
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    എങ്കിൽ(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      പോകുക out;
    }
    i += pkts_sent; /* Some have been sent */
    വേണ്ടി(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    എങ്കിൽ(pkts_sent < batch)
      പോകുക out; /* but not all of them */
  }

out:
  *psent = sent;
  എങ്കിൽ(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

#അല്ലെങ്കിൽ /* HAVE_APPLE_MSG_X */

സ്ഥിതം CURLcode send_packet_no_gso(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന cf_quic_ctx *qctx,
                                   സ്ഥിരം uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

സ്ഥിതം CURLcode do_sendmsg(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           ഘടന cf_quic_ctx *qctx,
                           സ്ഥിരം uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  ഘടന iovec msg_iov;
  ഘടന msghdr സന്ദേശം = { 0 };
  ssize_t rv;
#എങ്കിൽ defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t സന്ദേശം_ctrl[32];
  ഘടന cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  സന്ദേശം.msg_iov = &msg_iov;
  സന്ദേശം.സന്ദേശം_iovlen = 1;

#എങ്കിൽ defined(__linux__) && defined(UDP_SEGMENT)
  എങ്കിൽ(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(സന്ദേശം_ctrl, 0, വലുപ്പം(സന്ദേശം_ctrl));
    സന്ദേശം.സന്ദേശം_control = സന്ദേശം_ctrl;
    DEBUGASSERT(വലുപ്പം(സന്ദേശം_ctrl) >= CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ)));
    സന്ദേശം.msg_controllen = CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ));
    cm = CMSG_FIRSTHDR(&സന്ദേശം);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(വലുപ്പം(uint16_t));
    *(uint16_t *)(ശൂന്യം *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  ആകുമ്പോൾ((rv = sendmsg(qctx->sockfd, &സന്ദേശം, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  എങ്കിൽ(!curlx_sztouz(rv, psent)) {
    പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
    എങ്കിൽ(SOCK_EAGAIN(sockerr))
      തിരികെ CURLE_AGAIN;
    തിരഞ്ഞെടുപ്പ്(sockerr) {
    അവസ്ഥ SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      നിർത്തുക;
    അവസ്ഥ EIO:
      എങ്കിൽ(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        തിരികെ send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    സ്ഥിരസ്ഥിതി:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }
#അല്ലെങ്കിൽ
  ssize_t rv;
  (ശൂന്യം)gsolen;

  *psent = 0;

  ആകുമ്പോൾ((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  എങ്കിൽ(!curlx_sztouz(rv, psent)) {
    എങ്കിൽ(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      പോകുക out;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        പോകുക out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (ശൂന്യം)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (പൂർണ്ണസംഖ്യ)result, *psent);
  തിരികെ result;
}

#endif /* !HAVE_APPLE_MSG_X */

സ്ഥിതം CURLcode send_packet_no_gso(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന cf_quic_ctx *qctx,
                                   സ്ഥിരം uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  സ്ഥിരം uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  വേണ്ടി(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    എങ്കിൽ(result)
      പോകുക out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (പൂർണ്ണസംഖ്യ)result, *psent);
  തിരികെ result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
സ്ഥിതം CURLcode send_packet_no_gso_cf(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      സ്ഥിരം uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  സ്ഥിരം uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  വേണ്ടി(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    എങ്കിൽ(result)
      പോകുക out;

    എങ്കിൽ(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (പൂർണ്ണസംഖ്യ)result, *psent);
  തിരികെ result;
}

സ്ഥിതം CURLcode vquic_send_packets(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന cf_quic_ctx *qctx,
                                   സ്ഥിരം uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  എങ്കിൽ(qctx->wblock_percent > 0) {
    unsigned അക്ഷരം c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    എങ്കിൽ(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      തിരികെ CURLE_AGAIN;
    }
  }
#endif
  എങ്കിൽ(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  അല്ലെങ്കിൽ {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  എങ്കിൽ(!result)
    qctx->last_io = qctx->last_op;
  തിരികെ result;
}

CURLcode Curl_vquic_flush(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                          ഘടന cf_quic_ctx *qctx)
{
  സ്ഥിരം unsigned അക്ഷരം *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  ആകുമ്പോൾ(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    എങ്കിൽ(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      എങ്കിൽ(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    എങ്കിൽ(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    അല്ലെങ്കിൽ {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      എങ്കിൽ(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      അല്ലെങ്കിൽ {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        എങ്കിൽ(qctx->split_len)
          qctx->split_len -= sent;
      }
      തിരികെ result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    എങ്കിൽ(qctx->split_len)
      qctx->split_len -= sent;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_vquic_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                         ഘടന cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  തിരികെ Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  തിരികെ Curl_vquic_flush(cf, data, qctx);
}

#എങ്കിൽ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_സന്ദേശം_X)
സ്ഥിതം size_t vquic_msghdr_get_udp_gro(ഘടന msghdr *സന്ദേശം)
{
  പൂർണ്ണസംഖ്യ gso_size = 0;
#എങ്കിൽ defined(__linux__) && defined(UDP_GRO)
  ഘടന cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#എങ്കിൽ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  വേണ്ടി(cmsg = CMSG_FIRSTHDR(സന്ദേശം); cmsg; cmsg = CMSG_NXTHDR(സന്ദേശം, cmsg)) {
#എങ്കിൽ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic എടുക്കുക
#endif
    എങ്കിൽ(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), വലുപ്പം(gso_size));

      നിർത്തുക;
    }
  }
#endif /* linux && UDP_GRO */
  (ശൂന്യം)സന്ദേശം;

  തിരികെ (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#എങ്കിൽ (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_സന്ദേശം_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
സ്ഥിതം uint8_t vquic_msghdr_get_ecn(ഘടന msghdr *സന്ദേശം, പൂർണ്ണസംഖ്യ family)
{
  ഘടന cmsghdr *cmsg;
  തിരഞ്ഞെടുപ്പ്(family) {
  അവസ്ഥ AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#എങ്കിൽ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    വേണ്ടി(cmsg = CMSG_FIRSTHDR(സന്ദേശം); cmsg; cmsg = CMSG_NXTHDR(സന്ദേശം, cmsg)) {
#എങ്കിൽ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic എടുക്കുക
#endif
      എങ്കിൽ(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#അല്ലെങ്കിൽ
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        തിരികെ *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    നിർത്തുക;
  അവസ്ഥ AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#എങ്കിൽ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    വേണ്ടി(cmsg = CMSG_FIRSTHDR(സന്ദേശം); cmsg; cmsg = CMSG_NXTHDR(സന്ദേശം, cmsg)) {
#എങ്കിൽ defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic എടുക്കുക
#endif
      എങ്കിൽ(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned പൂർണ്ണസംഖ്യ tos;

        memcpy(&tos, CMSG_DATA(cmsg), വലുപ്പം(പൂർണ്ണസംഖ്യ));

        തിരികെ (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    നിർത്തുക;
  }
  തിരികെ 0;
}
#അല്ലെങ്കിൽ
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

സ്ഥിതം CURLcode recvmmsg_packets(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ശൂന്യം *userp,
                                 size_t *pnread)
{
#എങ്കിൽ defined(__linux__) && defined(UDP_GRO)
#define MMSG_അക്കം  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_സന്ദേശം_SIZE    (2 * CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ)))
#അല്ലെങ്കിൽ
#define MMSG_അക്കം  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_സന്ദേശം_SIZE    CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ))
#endif
#define സന്ദേശം_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  ഘടന iovec msg_iov[MMSG_അക്കം];
  ഘടന mmsghdr mmsg[MMSG_അക്കം];
  uint8_t സന്ദേശം_ctrl[MMSG_അക്കം * CMSG_PER_സന്ദേശം_SIZE];
  ഘടന sockaddr_storage remote_addr[MMSG_അക്കം];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  പൂർണ്ണസംഖ്യ mcount, i, n;
  അക്ഷരം errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  അക്ഷരം *sockbuf = ശൂന്യ;
  uint8_t (*bufs)[സന്ദേശം_BUF_SIZE] = ശൂന്യ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_അക്കം * സന്ദേശം_BUF_SIZE,
                                          &sockbuf);
  എങ്കിൽ(result)
    പോകുക out;
  bufs = (uint8_t (*)[സന്ദേശം_BUF_SIZE])sockbuf;

  total_nread = 0;
  ആകുമ്പോൾ(pkts < max_pkts) {
    n = (പൂർണ്ണസംഖ്യ)CURLMIN(CURLMIN(MMSG_അക്കം, IOV_MAX), max_pkts);
    memset(&mmsg, 0, വലുപ്പം(mmsg));
    വേണ്ടി(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = വലുപ്പം(bufs[i]);
      mmsg[i].സന്ദേശം_hdr.msg_iov = &msg_iov[i];
      mmsg[i].സന്ദേശം_hdr.സന്ദേശം_iovlen = 1;
      mmsg[i].സന്ദേശം_hdr.സന്ദേശം_name = &remote_addr[i];
      mmsg[i].സന്ദേശം_hdr.msg_namelen = വലുപ്പം(remote_addr[i]);
      mmsg[i].സന്ദേശം_hdr.സന്ദേശം_control = &സന്ദേശം_ctrl[i * CMSG_PER_സന്ദേശം_SIZE];
      mmsg[i].സന്ദേശം_hdr.msg_controllen = CMSG_PER_സന്ദേശം_SIZE;
    }

    ആകുമ്പോൾ((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ശൂന്യ)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    എങ്കിൽ(mcount == -1) {
      എങ്കിൽ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        പോകുക out;
      }
      എങ്കിൽ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ഘടന ip_quadruple ip;
        എങ്കിൽ(!Curl_cf_socket_peek(cf->next, data, ശൂന്യ, ശൂന്യ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        പോകുക out;
      }
      curlx_strerror(SOCKERRNO, errstr, വലുപ്പം(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }

    VERBOSE(++calls);
    വേണ്ടി(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      എങ്കിൽ(!mmsg[i].സന്ദേശം_len) {
        ++pkts;
        തുടരുക;
      }
      total_nread += mmsg[i].സന്ദേശം_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].സന്ദേശം_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].സന്ദേശം_hdr);
      എങ്കിൽ(gso_size == 0)
        gso_size = mmsg[i].സന്ദേശം_len;

      result = recv_cb(bufs[i], mmsg[i].സന്ദേശം_len, gso_size,
                       mmsg[i].സന്ദേശം_hdr.സന്ദേശം_name,
                       mmsg[i].സന്ദേശം_hdr.msg_namelen, ecn, userp);
      എങ്കിൽ(result)
        പോകുക out;
      pkts += (mmsg[i].സന്ദേശം_len + gso_size - 1) / gso_size;
    }
  }

out:
  എങ്കിൽ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (പൂർണ്ണസംഖ്യ)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  തിരികെ result;
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_APPLE_സന്ദേശം_X)

സ്ഥിതം CURLcode recvmsg_x_packets(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  ശൂന്യം *userp, size_t *pnread)
{
#define സന്ദേശം_X_അക്കം  64
#define സന്ദേശം_BUF_SIZE  (2048)
#define CMSG_PER_സന്ദേശം_SIZE     CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ))
  ഘടന iovec msg_iov[സന്ദേശം_X_അക്കം];
  ഘടന msghdr_x mmsg[സന്ദേശം_X_അക്കം];
  uint8_t സന്ദേശം_ctrl[സന്ദേശം_X_അക്കം * CMSG_PER_സന്ദേശം_SIZE];
  ഘടന sockaddr_storage remote_addr[സന്ദേശം_X_അക്കം];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  പൂർണ്ണസംഖ്യ mcount, i;
  അക്ഷരം errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  അക്ഷരം *sockbuf = ശൂന്യ;
  uint8_t (*bufs)[സന്ദേശം_BUF_SIZE] = ശൂന്യ;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, സന്ദേശം_X_അക്കം * സന്ദേശം_BUF_SIZE,
                                          &sockbuf);
  എങ്കിൽ(result)
    പോകുക out;
  bufs = (uint8_t (*)[സന്ദേശം_BUF_SIZE])sockbuf;

  total_nread = 0;
  ആകുമ്പോൾ(pkts < max_pkts) {
    പൂർണ്ണസംഖ്യ n = (പൂർണ്ണസംഖ്യ)CURLMIN(CURLMIN(സന്ദേശം_X_അക്കം, IOV_MAX), max_pkts);
    memset(&mmsg, 0, വലുപ്പം(mmsg));
    വേണ്ടി(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = വലുപ്പം(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].സന്ദേശം_iovlen = 1;
      mmsg[i].സന്ദേശം_name = &remote_addr[i];
      mmsg[i].msg_namelen = വലുപ്പം(remote_addr[i]);
      mmsg[i].സന്ദേശം_control = &സന്ദേശം_ctrl[i * CMSG_PER_സന്ദേശം_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_സന്ദേശം_SIZE;
    }

#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ആകുമ്പോൾ((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#എങ്കിൽ defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic എടുക്കുക
#endif
    എങ്കിൽ(mcount == -1) {
      എങ്കിൽ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        പോകുക out;
      }
      എങ്കിൽ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ഘടന ip_quadruple ip;
        എങ്കിൽ(!Curl_cf_socket_peek(cf->next, data, ശൂന്യ, ശൂന്യ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        പോകുക out;
      }
      curlx_strerror(SOCKERRNO, errstr, വലുപ്പം(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }

    VERBOSE(++calls);
    വേണ്ടി(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      എങ്കിൽ(!mmsg[i].സന്ദേശം_datalen) {
        ++pkts;
        തുടരുക;
      }
      total_nread += mmsg[i].സന്ദേശം_datalen;
      ecn = vquic_msghdr_get_ecn((ഘടന msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].സന്ദേശം_datalen;

      result = recv_cb(bufs[i], mmsg[i].സന്ദേശം_datalen, gso_size,
                       mmsg[i].സന്ദേശം_name,
                       mmsg[i].msg_namelen, ecn, userp);
      എങ്കിൽ(result)
        പോകുക out;
      pkts += (mmsg[i].സന്ദേശം_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  എങ്കിൽ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (പൂർണ്ണസംഖ്യ)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  തിരികെ result;
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SENDMSG)
സ്ഥിതം CURLcode recvmsg_packets(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, ശൂന്യം *userp,
                                size_t *pnread)
{
#define CMSG_PER_സന്ദേശം_SIZE    CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ))
  ഘടന iovec msg_iov;
  ഘടന msghdr സന്ദേശം;
  uint8_t buf[64 * 1024];
  ഘടന sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  അക്ഷരം errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t സന്ദേശം_ctrl[CMSG_PER_സന്ദേശം_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  വേണ്ടി(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&സന്ദേശം, 0, വലുപ്പം(സന്ദേശം));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = വലുപ്പം(buf);
    സന്ദേശം.msg_iov = &msg_iov;
    സന്ദേശം.സന്ദേശം_iovlen = 1;
    സന്ദേശം.സന്ദേശം_control = സന്ദേശം_ctrl;
    സന്ദേശം.സന്ദേശം_name = &remote_addr;
    സന്ദേശം.msg_namelen = വലുപ്പം(remote_addr);
    സന്ദേശം.msg_controllen = CMSG_PER_സന്ദേശം_SIZE;

    ആകുമ്പോൾ((rc = recvmsg(qctx->sockfd, &സന്ദേശം, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    എങ്കിൽ(!curlx_sztouz(rc, &nread)) {
      എങ്കിൽ(SOCK_EAGAIN(SOCKERRNO)) {
        പോകുക out;
      }
      എങ്കിൽ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ഘടന ip_quadruple ip;
        എങ്കിൽ(!Curl_cf_socket_peek(cf->next, data, ശൂന്യ, ശൂന്യ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        പോകുക out;
      }
      curlx_strerror(SOCKERRNO, errstr, വലുപ്പം(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    എങ്കിൽ(!nread) {
      ++pkts;
      തുടരുക;
    }

    ecn = vquic_msghdr_get_ecn(&സന്ദേശം, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&സന്ദേശം);
    എങ്കിൽ(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     സന്ദേശം.സന്ദേശം_name, സന്ദേശം.msg_namelen, ecn, userp);
    എങ്കിൽ(result)
      പോകുക out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  എങ്കിൽ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (പൂർണ്ണസംഖ്യ)result);
  *pnread = total_nread;
  തിരികെ result;
}

#അല്ലെങ്കിൽ /* HAVE_SENDMMSG || HAVE_SENDMSG */
സ്ഥിതം CURLcode recvfrom_packets(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ശൂന്യം *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  പൂർണ്ണസംഖ്യ bufsize = (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf);
  ഘടന sockaddr_storage remote_addr;
  socklen_t remote_addrlen = വലുപ്പം(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  അക്ഷരം errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  വേണ്ടി(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    ആകുമ്പോൾ((rv = recvfrom(qctx->sockfd, (അക്ഷരം *)buf, bufsize, 0,
                         (ഘടന sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    എങ്കിൽ(!curlx_sztouz(rv, &nread)) {
      എങ്കിൽ(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        പോകുക out;
      }
      എങ്കിൽ(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ഘടന ip_quadruple ip;
        എങ്കിൽ(!Curl_cf_socket_peek(cf->next, data, ശൂന്യ, ശൂന്യ, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        പോകുക out;
      }
      curlx_strerror(SOCKERRNO, errstr, വലുപ്പം(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    എങ്കിൽ(!nread)
      തുടരുക;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    എങ്കിൽ(result)
      പോകുക out;
  }

out:
  എങ്കിൽ(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (പൂർണ്ണസംഖ്യ)result);
  *pnread = total_nread;
  തിരികെ result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, ശൂന്യം *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_APPLE_സന്ദേശം_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#അല്ലെങ്കിൽ
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  എങ്കിൽ(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    എങ്കിൽ(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  തിരികെ result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ഘടന Curl_easy *data,
                      unsigned അക്ഷരം *scid,
                      size_t scidlen,
                      പൂർണ്ണസംഖ്യ *qlogfdp)
{
  അക്ഷരം *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  എങ്കിൽ(qlog_dir) {
    ഘടന dynbuf fname;
    CURLcode result;
    unsigned പൂർണ്ണസംഖ്യ i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    എങ്കിൽ(!result)
      result = curlx_dyn_add(&fname, "/");
    വേണ്ടി(i = 0; (i < scidlen) && !result; i++) {
      അക്ഷരം hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    എങ്കിൽ(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    എങ്കിൽ(!result) {
      പൂർണ്ണസംഖ്യ qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ദ്വിമാന,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#അല്ലെങ്കിൽ
                              (mode_t)data->set.new_file_perms
#endif
                              );
      എങ്കിൽ(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  തിരികെ CURLE_OK;
}

CURLcode Curl_cf_quic_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                   ഘടന Curl_easy *data,
                                   ഘടന Curl_peer *origin,
                                   ഘടന Curl_peer *peer)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)data; /* not used in all cases and compilers are stupid */
#എങ്കിൽ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_തിരുകുക_after(cf_at, origin, peer, &cf);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_QUICHE)
  result = Curl_cf_quiche_തിരുകുക_after(cf_at, origin, peer, &cf);
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf)
  (ശൂന്യം)cf_at;
  (ശൂന്യം)origin;
  (ശൂന്യം)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#എങ്കിൽ defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  എങ്കിൽ(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  തിരികെ result;
}

CURLcode Curl_cf_quic_create(ഘടന Curl_cfilter **pcf,
                             ഘടന Curl_easy *data,
                             ഘടന Curl_peer *origin,
                             ഘടന Curl_peer *peer,
                             uint8_t transport_peer,
                             ഘടന connectdata *conn,
                             ഘടന Curl_sockaddr_ex *addr,
                             ഘടന Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (ശൂന്യം)transport_peer;
  (ശൂന്യം)tunnel_transport;
  (ശൂന്യം)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#എങ്കിൽ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  തിരികെ Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_QUICHE)
  തിരികെ Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#അല്ലെങ്കിൽ
  *pcf = ശൂന്യ;
  (ശൂന്യം)data;
  (ശൂന്യം)origin;
  (ശൂന്യം)peer;
  (ശൂന്യം)conn;
  (ശൂന്യം)addr;
  (ശൂന്യം)tunnel_peer;
  (ശൂന്യം)tunnel_transport;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                       ഘടന Curl_easy *data,
                                       ഘടന Curl_peer *origin,
                                       ഘടന Curl_peer *peer,
                                       ഘടന Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#എങ്കിൽ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  തിരികെ Curl_cf_ngtcp2_proxy_തിരുകുക_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf_at;
  (ശൂന്യം)data;
  (ശൂന്യം)origin;
  (ശൂന്യം)peer;
  (ശൂന്യം)tunnel_peer;
  (ശൂന്യം)tunnel_transport;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(ഘടന Curl_cfilter **pcf,
                                 ഘടന Curl_easy *data,
                                 ഘടന Curl_peer *origin,
                                 ഘടന Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ഘടന connectdata *conn,
                                 ഘടന Curl_sockaddr_ex *addr,
                                 ഘടന Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#എങ്കിൽ defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  തിരികെ Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#അല്ലെങ്കിൽ
  *pcf = ശൂന്യ;
  (ശൂന്യം)data;
  (ശൂന്യം)conn;
  (ശൂന്യം)addr;
  (ശൂന്യം)peer;
  (ശൂന്യം)transport_peer;
  (ശൂന്യം)tunnel_peer;
  (ശൂന്യം)tunnel_transport;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ഘടന Curl_easy *data,
                             ഘടന connectdata *conn)
{
  എങ്കിൽ(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }
  എങ്കിൽ(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    തിരികെ CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    തിരികെ CURLE_URL_MALFORMAT;
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)conn;
#endif

  തിരികെ CURLE_OK;
}

#ifdef CURLVERBOSE
സ്ഥിരം അക്ഷരം *Curl_vquic_h3_err_str(uint64_t error_code)
{
  എങ്കിൽ(error_code <= UINT_MAX) {
    തിരഞ്ഞെടുപ്പ്((unsigned പൂർണ്ണസംഖ്യ)error_code) {
    അവസ്ഥ CURL_H3_ERR_NO_ERROR:
      തിരികെ "NO_ERROR";
    അവസ്ഥ CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      തിരികെ "GENERAL_PROTOCOL_ERROR";
    അവസ്ഥ CURL_H3_ERR_INTERNAL_ERROR:
      തിരികെ "INTERNAL_ERROR";
    അവസ്ഥ CURL_H3_ERR_STREAM_CREATION_ERROR:
      തിരികെ "STREAM_CREATION_ERROR";
    അവസ്ഥ CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      തിരികെ "CLOSED_CRITICAL_STREAM";
    അവസ്ഥ CURL_H3_ERR_FRAME_UNEXPECTED:
      തിരികെ "FRAME_UNEXPECTED";
    അവസ്ഥ CURL_H3_ERR_FRAME_ERROR:
      തിരികെ "FRAME_ERROR";
    അവസ്ഥ CURL_H3_ERR_EXCESSIVE_LOAD:
      തിരികെ "EXCESSIVE_LOAD";
    അവസ്ഥ CURL_H3_ERR_ID_ERROR:
      തിരികെ "ID_ERROR";
    അവസ്ഥ CURL_H3_ERR_SETTINGS_ERROR:
      തിരികെ "SETTINGS_ERROR";
    അവസ്ഥ CURL_H3_ERR_MISSING_SETTINGS:
      തിരികെ "MISSING_SETTINGS";
    അവസ്ഥ CURL_H3_ERR_REQUEST_REJECTED:
      തിരികെ "REQUEST_REJECTED";
    അവസ്ഥ CURL_H3_ERR_REQUEST_CANCELLED:
      തിരികെ "REQUEST_CANCELLED";
    അവസ്ഥ CURL_H3_ERR_REQUEST_INCOMPLETE:
      തിരികെ "REQUEST_INCOMPLETE";
    അവസ്ഥ CURL_H3_ERR_MESSAGE_ERROR:
      തിരികെ "MESSAGE_ERROR";
    അവസ്ഥ CURL_H3_ERR_CONNECT_ERROR:
      തിരികെ "CONNECT_ERROR";
    അവസ്ഥ CURL_H3_ERR_VERSION_FALLBACK:
      തിരികെ "VERSION_FALLBACK";
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  എങ്കിൽ((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    തിരികെ "NO_ERROR";
  തിരികെ "unknown";
}
#endif /* CURLVERBOSE */

#എങ്കിൽ defined(USE_NGTCP2) || defined(USE_NGHTTP3)

സ്ഥിതം ശൂന്യം *vquic_ngtcp2_malloc(size_t size, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  തിരികെ Curl_cmalloc(size);
}

സ്ഥിതം ശൂന്യം vquic_ngtcp2_free(ശൂന്യം *ptr, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  Curl_cfree(ptr);
}

സ്ഥിതം ശൂന്യം *vquic_ngtcp2_calloc(size_t nmemb, size_t size, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  തിരികെ Curl_ccalloc(nmemb, size);
}

സ്ഥിതം ശൂന്യം *vquic_ngtcp2_realloc(ശൂന്യം *ptr, size_t size, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  തിരികെ Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
സ്ഥിതം ഘടന ngtcp2_mem vquic_ngtcp2_mem = {
  ശൂന്യ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ഘടന ngtcp2_mem *Curl_ngtcp2_mem(ശൂന്യം)
{
  തിരികെ &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
സ്ഥിതം ഘടന nghttp3_mem vquic_nghttp3_mem = {
  ശൂന്യ,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ഘടന nghttp3_mem *Curl_nghttp3_mem(ശൂന്യം)
{
  തിരികെ &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#അല്ലെങ്കിൽ /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ഘടന Curl_easy *data,
                             ഘടന connectdata *conn)
{
  (ശൂന്യം)data;
  (ശൂന്യം)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  തിരികെ CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
