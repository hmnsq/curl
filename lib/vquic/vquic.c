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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "vquic/vquic.h"
#অন্তর্ভাৱ "vtls/vtls.h"

#অন্তর্ভাৱ "curl_trc.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#অন্তর্ভাৱ <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#অন্তর্ভাৱ <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#অন্তর্ভাৱ <nghttp3/nghttp3.h>
#endif

#যদি defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#অন্তর্ভাৱ <sys/syscall.h>
#যদি defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_বাৰ্তা_X
#endif
#endif

#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "curlx/win32-fopen.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "vdns/cf-dns.h"
#অন্তর্ভাৱ "vquic/cf-ngtcp2.h"
#অন্তর্ভাৱ "vquic/cf-ngtcp2-cmn.h"
#অন্তর্ভাৱ "vquic/cf-ngtcp2-proxy.h"
#অন্তর্ভাৱ "vquic/cf-quiche.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "rand.h"
#অন্তর্ভাৱ "vquic/vquic_int.h"
#অন্তর্ভাৱ "curlx/strerr.h"
#অন্তর্ভাৱ "curlx/strparse.h"


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_বাৰ্তা_X

/* this is `struct msghdr` with an additional field at the end */
সংরচনা msghdr_x {
  শূন্য *বাৰ্তা_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  সংরচনা iovec *msg_iov;    /* scatter/gather array */
  পূর্ণাঙ্ক বাৰ্তা_iovlen;           /* # elements in msg_iov */
  শূন্য *বাৰ্তা_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  পূর্ণাঙ্ক বাৰ্তা_flags;            /* flags on received message */
  size_t বাৰ্তা_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_বাৰ্তা_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#নোচেদ্যদি defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#অন্যথা
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

পূর্ণাঙ্ক Curl_vquic_init(শূন্য)
{
#যদি defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  যদি(ngtcp2_crypto_ossl_init())
    ঘূৰাই 0;
#endif

  ঘূৰাই 1;
}

শূন্য Curl_vquic_cleanup(শূন্য)
{
#যদি defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_অঙ্ক >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

শূন্য Curl_quic_ver(ৱর্ণ *p, size_t len)
{
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#নোচেদ্যদি defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(সংরচনা Curl_easy *data,
                             সংরচনা cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#যদি defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#নোচেদ্যদি defined(HAVE_APPLE_বাৰ্তা_X)
  qctx->no_gso = FALSE;
#অন্যথা
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ধ্রুৱ ৱর্ণ *p = getenv("CURL_DBG_QUIC_WBLOCK");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 100))
        qctx->wblock_percent = (পূর্ণাঙ্ক)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  ঘূৰাই CURLE_OK;
}

শূন্য Curl_vquic_ctx_free(সংরচনা cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

শূন্য Curl_vquic_ctx_set_time(সংরচনা cf_quic_ctx *qctx,
                             ধ্রুৱ সংরচনা curltime *pnow)
{
  qctx->last_op = *pnow;
}

শূন্য Curl_vquic_ctx_update_time(সংরচনা cf_quic_ctx *qctx,
                                ধ্রুৱ সংরচনা curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_বাৰ্তা_X
স্থির CURLcode do_sendmsg(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           সংরচনা cf_quic_ctx *qctx,
                           ধ্রুৱ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define বাৰ্তা_X_SNUM  64
  সংরচনা iovec msg_iov[বাৰ্তা_X_SNUM];
  সংরচনা msghdr_x mmsg[বাৰ্তা_X_SNUM];
  ৱর্ণ errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  পূর্ণাঙ্ক rc;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  যদি(!pktlen)
    ঘূৰাই CURLE_OK;
  যদি(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  যেতিয়ালৈ(i < n) {
    size_t j, batch = CURLMIN(n - i, বাৰ্তা_X_SNUM), pkts_sent = 0;

    বাবে(j = 0; j < batch; ++j) {
      ধ্রুৱ size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, পরিমাণ(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].বাৰ্তা_iovlen = 1;
      mmsg[j].বাৰ্তা_datalen = msg_iov[j].iov_len;
    }

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    যেতিয়ালৈ((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic বেৰ_কৰো
#endif

    যদি(rc < 0) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        গচ্ছ out;
      }
      যদি(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, পরিমাণ(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        গচ্ছ out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    অন্যথা
      pkts_sent = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    যদি(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      গচ্ছ out;
    }
    i += pkts_sent; /* Some have been sent */
    বাবে(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    যদি(pkts_sent < batch)
      গচ্ছ out; /* but not all of them */
  }

out:
  *psent = sent;
  যদি(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

#অন্যথা /* HAVE_APPLE_MSG_X */

স্থির CURLcode send_packet_no_gso(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা cf_quic_ctx *qctx,
                                   ধ্রুৱ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

স্থির CURLcode do_sendmsg(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           সংরচনা cf_quic_ctx *qctx,
                           ধ্রুৱ uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  সংরচনা iovec msg_iov;
  সংরচনা msghdr বাৰ্তা = { 0 };
  ssize_t rv;
#যদি defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t বাৰ্তা_ctrl[32];
  সংরচনা cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  বাৰ্তা.msg_iov = &msg_iov;
  বাৰ্তা.বাৰ্তা_iovlen = 1;

#যদি defined(__linux__) && defined(UDP_SEGMENT)
  যদি(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(বাৰ্তা_ctrl, 0, পরিমাণ(বাৰ্তা_ctrl));
    বাৰ্তা.বাৰ্তা_control = বাৰ্তা_ctrl;
    DEBUGASSERT(পরিমাণ(বাৰ্তা_ctrl) >= CMSG_SPACE(পরিমাণ(পূর্ণাঙ্ক)));
    বাৰ্তা.msg_controllen = CMSG_SPACE(পরিমাণ(পূর্ণাঙ্ক));
    cm = CMSG_FIRSTHDR(&বাৰ্তা);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(পরিমাণ(uint16_t));
    *(uint16_t *)(শূন্য *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  যেতিয়ালৈ((rv = sendmsg(qctx->sockfd, &বাৰ্তা, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  যদি(!curlx_sztouz(rv, psent)) {
    পূর্ণাঙ্ক sockerr = SOCKERRNO;
    যদি(SOCK_EAGAIN(sockerr))
      ঘূৰাই CURLE_AGAIN;
    ৱিকল্প(sockerr) {
    পক্ষ SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      ভঙ্গ;
    পক্ষ EIO:
      যদি(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ঘূৰাই send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    স্ৱভাৱ:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }
  অন্যথা যদি(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
#অন্যথা
  ssize_t rv;
  (শূন্য)gsolen;

  *psent = 0;

  যেতিয়ালৈ((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  যদি(!curlx_sztouz(rv, psent)) {
    যদি(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      গচ্ছ out;
    }
    অন্যথা {
      যদি(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        গচ্ছ out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (শূন্য)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (পূর্ণাঙ্ক)result, *psent);
  ঘূৰাই result;
}

#endif /* !HAVE_APPLE_MSG_X */

স্থির CURLcode send_packet_no_gso(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা cf_quic_ctx *qctx,
                                   ধ্রুৱ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ধ্রুৱ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  বাবে(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    যদি(result)
      গচ্ছ out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (পূর্ণাঙ্ক)result, *psent);
  ঘূৰাই result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
স্থির CURLcode send_packet_no_gso_cf(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      ধ্রুৱ uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ধ্রুৱ uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  বাবে(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    যদি(result)
      গচ্ছ out;

    যদি(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (পূর্ণাঙ্ক)result, *psent);
  ঘূৰাই result;
}

স্থির CURLcode vquic_send_packets(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা cf_quic_ctx *qctx,
                                   ধ্রুৱ uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  যদি(qctx->wblock_percent > 0) {
    unsigned ৱর্ণ c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    যদি(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      ঘূৰাই CURLE_AGAIN;
    }
  }
#endif
  যদি(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  অন্যথা {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  যদি(!result)
    qctx->last_io = qctx->last_op;
  ঘূৰাই result;
}

CURLcode Curl_vquic_flush(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                          সংরচনা cf_quic_ctx *qctx)
{
  ধ্রুৱ unsigned ৱর্ণ *buf;
  size_t blen, sent;
  CURLcode result;
  size_t gsolen;

  যেতিয়ালৈ(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    যদি(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      যদি(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    যদি(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    অন্যথা {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      যদি(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      অন্যথা {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    যদি(result) {
      যদি(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        যদি(qctx->split_len)
          qctx->split_len -= sent;
      }
      ঘূৰাই result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    যদি(qctx->split_len)
      qctx->split_len -= sent;
  }
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_vquic_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                         সংরচনা cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ঘূৰাই Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  ঘূৰাই Curl_vquic_flush(cf, data, qctx);
}

#যদি (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_বাৰ্তা_X)
স্থির size_t vquic_msghdr_get_udp_gro(সংরচনা msghdr *বাৰ্তা)
{
  পূর্ণাঙ্ক gso_size = 0;
#যদি defined(__linux__) && defined(UDP_GRO)
  সংরচনা cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  বাবে(cmsg = CMSG_FIRSTHDR(বাৰ্তা); cmsg; cmsg = CMSG_NXTHDR(বাৰ্তা, cmsg)) {
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic বেৰ_কৰো
#endif
    যদি(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_size, CMSG_DATA(cmsg), পরিমাণ(gso_size));

      ভঙ্গ;
    }
  }
#endif /* linux && UDP_GRO */
  (শূন্য)বাৰ্তা;

  ঘূৰাই (size_t)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#যদি (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_বাৰ্তা_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
স্থির uint8_t vquic_msghdr_get_ecn(সংরচনা msghdr *বাৰ্তা, পূর্ণাঙ্ক family)
{
  সংরচনা cmsghdr *cmsg;
  ৱিকল্প(family) {
  পক্ষ AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    বাবে(cmsg = CMSG_FIRSTHDR(বাৰ্তা); cmsg; cmsg = CMSG_NXTHDR(বাৰ্তা, cmsg)) {
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic বেৰ_কৰো
#endif
      যদি(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#অন্যথা
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        ঘূৰাই *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    ভঙ্গ;
  পক্ষ AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    বাবে(cmsg = CMSG_FIRSTHDR(বাৰ্তা); cmsg; cmsg = CMSG_NXTHDR(বাৰ্তা, cmsg)) {
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic বেৰ_কৰো
#endif
      যদি(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned পূর্ণাঙ্ক tos;

        memcpy(&tos, CMSG_DATA(cmsg), পরিমাণ(পূর্ণাঙ্ক));

        ঘূৰাই (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    ভঙ্গ;
  }
  ঘূৰাই 0;
}
#অন্যথা
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

স্থির CURLcode recvmmsg_packets(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp,
                                 size_t *pnread)
{
#যদি defined(__linux__) && defined(UDP_GRO)
#define MMSG_অঙ্ক  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_বাৰ্তা_SIZE    (2 * CMSG_SPACE(পরিমাণ(পূর্ণাঙ্ক)))
#অন্যথা
#define MMSG_অঙ্ক  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_বাৰ্তা_SIZE    CMSG_SPACE(পরিমাণ(পূর্ণাঙ্ক))
#endif
#define বাৰ্তা_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  সংরচনা iovec msg_iov[MMSG_অঙ্ক];
  সংরচনা mmsghdr mmsg[MMSG_অঙ্ক];
  uint8_t বাৰ্তা_ctrl[MMSG_অঙ্ক * CMSG_PER_বাৰ্তা_SIZE];
  সংরচনা sockaddr_storage remote_addr[MMSG_অঙ্ক];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  পূর্ণাঙ্ক mcount, i, n;
  ৱর্ণ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  ৱর্ণ *sockbuf = রিক্ত;
  uint8_t (*bufs)[বাৰ্তা_BUF_SIZE] = রিক্ত;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_অঙ্ক * বাৰ্তা_BUF_SIZE,
                                          &sockbuf);
  যদি(result)
    গচ্ছ out;
  bufs = (uint8_t (*)[বাৰ্তা_BUF_SIZE])sockbuf;

  total_nread = 0;
  যেতিয়ালৈ(pkts < max_pkts) {
    n = (পূর্ণাঙ্ক)CURLMIN(CURLMIN(MMSG_অঙ্ক, IOV_MAX), max_pkts);
    memset(&mmsg, 0, পরিমাণ(mmsg));
    বাবে(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = পরিমাণ(bufs[i]);
      mmsg[i].বাৰ্তা_hdr.msg_iov = &msg_iov[i];
      mmsg[i].বাৰ্তা_hdr.বাৰ্তা_iovlen = 1;
      mmsg[i].বাৰ্তা_hdr.বাৰ্তা_name = &remote_addr[i];
      mmsg[i].বাৰ্তা_hdr.msg_namelen = পরিমাণ(remote_addr[i]);
      mmsg[i].বাৰ্তা_hdr.বাৰ্তা_control = &বাৰ্তা_ctrl[i * CMSG_PER_বাৰ্তা_SIZE];
      mmsg[i].বাৰ্তা_hdr.msg_controllen = CMSG_PER_বাৰ্তা_SIZE;
    }

    যেতিয়ালৈ((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, রিক্ত)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    যদি(mcount == -1) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        গচ্ছ out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        সংরচনা ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->next, data, রিক্ত, রিক্ত, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        গচ্ছ out;
      }
      curlx_strerror(SOCKERRNO, errstr, পরিমাণ(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }

    VERBOSE(++calls);
    বাবে(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      যদি(!mmsg[i].বাৰ্তা_len) {
        ++pkts;
        অনুৱর্ত;
      }
      total_nread += mmsg[i].বাৰ্তা_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].বাৰ্তা_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].বাৰ্তা_hdr);
      যদি(gso_size == 0)
        gso_size = mmsg[i].বাৰ্তা_len;

      result = recv_cb(bufs[i], mmsg[i].বাৰ্তা_len, gso_size,
                       mmsg[i].বাৰ্তা_hdr.বাৰ্তা_name,
                       mmsg[i].বাৰ্তা_hdr.msg_namelen, ecn, userp);
      যদি(result)
        গচ্ছ out;
      pkts += (mmsg[i].বাৰ্তা_len + gso_size - 1) / gso_size;
    }
  }

out:
  যদি(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (পূর্ণাঙ্ক)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ঘূৰাই result;
}

#নোচেদ্যদি defined(HAVE_APPLE_বাৰ্তা_X)

স্থির CURLcode recvmsg_x_packets(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  শূন্য *userp, size_t *pnread)
{
#define বাৰ্তা_X_অঙ্ক  64
#define বাৰ্তা_BUF_SIZE  (2048)
#define CMSG_PER_বাৰ্তা_SIZE     CMSG_SPACE(পরিমাণ(পূর্ণাঙ্ক))
  সংরচনা iovec msg_iov[বাৰ্তা_X_অঙ্ক];
  সংরচনা msghdr_x mmsg[বাৰ্তা_X_অঙ্ক];
  uint8_t বাৰ্তা_ctrl[বাৰ্তা_X_অঙ্ক * CMSG_PER_বাৰ্তা_SIZE];
  সংরচনা sockaddr_storage remote_addr[বাৰ্তা_X_অঙ্ক];
  size_t total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  পূর্ণাঙ্ক mcount, i;
  ৱর্ণ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  size_t gso_size;
  ৱর্ণ *sockbuf = রিক্ত;
  uint8_t (*bufs)[বাৰ্তা_BUF_SIZE] = রিক্ত;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, বাৰ্তা_X_অঙ্ক * বাৰ্তা_BUF_SIZE,
                                          &sockbuf);
  যদি(result)
    গচ্ছ out;
  bufs = (uint8_t (*)[বাৰ্তা_BUF_SIZE])sockbuf;

  total_nread = 0;
  যেতিয়ালৈ(pkts < max_pkts) {
    পূর্ণাঙ্ক n = (পূর্ণাঙ্ক)CURLMIN(CURLMIN(বাৰ্তা_X_অঙ্ক, IOV_MAX), max_pkts);
    memset(&mmsg, 0, পরিমাণ(mmsg));
    বাবে(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = পরিমাণ(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].বাৰ্তা_iovlen = 1;
      mmsg[i].বাৰ্তা_name = &remote_addr[i];
      mmsg[i].msg_namelen = পরিমাণ(remote_addr[i]);
      mmsg[i].বাৰ্তা_control = &বাৰ্তা_ctrl[i * CMSG_PER_বাৰ্তা_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_বাৰ্তা_SIZE;
    }

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    যেতিয়ালৈ((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic বেৰ_কৰো
#endif
    যদি(mcount == -1) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        গচ্ছ out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        সংরচনা ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->next, data, রিক্ত, রিক্ত, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        গচ্ছ out;
      }
      curlx_strerror(SOCKERRNO, errstr, পরিমাণ(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }

    VERBOSE(++calls);
    বাবে(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      যদি(!mmsg[i].বাৰ্তা_datalen) {
        ++pkts;
        অনুৱর্ত;
      }
      total_nread += mmsg[i].বাৰ্তা_datalen;
      ecn = vquic_msghdr_get_ecn((সংরচনা msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].বাৰ্তা_datalen;

      result = recv_cb(bufs[i], mmsg[i].বাৰ্তা_datalen, gso_size,
                       mmsg[i].বাৰ্তা_name,
                       mmsg[i].msg_namelen, ecn, userp);
      যদি(result)
        গচ্ছ out;
      pkts += (mmsg[i].বাৰ্তা_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  যদি(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (পূর্ণাঙ্ক)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  ঘূৰাই result;
}

#নোচেদ্যদি defined(HAVE_SENDMSG)
স্থির CURLcode recvmsg_packets(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                সংরচনা cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp,
                                size_t *pnread)
{
#define CMSG_PER_বাৰ্তা_SIZE    CMSG_SPACE(পরিমাণ(পূর্ণাঙ্ক))
  সংরচনা iovec msg_iov;
  সংরচনা msghdr বাৰ্তা;
  uint8_t buf[64 * 1024];
  সংরচনা sockaddr_storage remote_addr;
  size_t total_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  ৱর্ণ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t বাৰ্তা_ctrl[CMSG_PER_বাৰ্তা_SIZE];
  size_t gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  বাবে(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&বাৰ্তা, 0, পরিমাণ(বাৰ্তা));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = পরিমাণ(buf);
    বাৰ্তা.msg_iov = &msg_iov;
    বাৰ্তা.বাৰ্তা_iovlen = 1;
    বাৰ্তা.বাৰ্তা_control = বাৰ্তা_ctrl;
    বাৰ্তা.বাৰ্তা_name = &remote_addr;
    বাৰ্তা.msg_namelen = পরিমাণ(remote_addr);
    বাৰ্তা.msg_controllen = CMSG_PER_বাৰ্তা_SIZE;

    যেতিয়ালৈ((rc = recvmsg(qctx->sockfd, &বাৰ্তা, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    যদি(!curlx_sztouz(rc, &nread)) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        গচ্ছ out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        সংরচনা ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->next, data, রিক্ত, রিক্ত, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        গচ্ছ out;
      }
      curlx_strerror(SOCKERRNO, errstr, পরিমাণ(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    যদি(!nread) {
      ++pkts;
      অনুৱর্ত;
    }

    ecn = vquic_msghdr_get_ecn(&বাৰ্তা, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&বাৰ্তা);
    যদি(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     বাৰ্তা.বাৰ্তা_name, বাৰ্তা.msg_namelen, ecn, userp);
    যদি(result)
      গচ্ছ out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  যদি(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (পূর্ণাঙ্ক)result);
  *pnread = total_nread;
  ঘূৰাই result;
}

#অন্যথা /* HAVE_SENDMMSG || HAVE_SENDMSG */
স্থির CURLcode recvfrom_packets(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  পূর্ণাঙ্ক bufsize = (পূর্ণাঙ্ক)পরিমাণ(buf);
  সংরচনা sockaddr_storage remote_addr;
  socklen_t remote_addrlen = পরিমাণ(remote_addr);
  size_t total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  ৱর্ণ errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  বাবে(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    যেতিয়ালৈ((rv = recvfrom(qctx->sockfd, (ৱর্ণ *)buf, bufsize, 0,
                         (সংরচনা sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    যদি(!curlx_sztouz(rv, &nread)) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        গচ্ছ out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        সংরচনা ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->next, data, রিক্ত, রিক্ত, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        গচ্ছ out;
      }
      curlx_strerror(SOCKERRNO, errstr, পরিমাণ(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    যদি(!nread)
      অনুৱর্ত;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    যদি(result)
      গচ্ছ out;
  }

out:
  যদি(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (পূর্ণাঙ্ক)result);
  *pnread = total_nread;
  ঘূৰাই result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp)
{
  CURLcode result;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#নোচেদ্যদি defined(HAVE_APPLE_বাৰ্তা_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#নোচেদ্যদি defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#অন্যথা
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  যদি(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    যদি(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  ঘূৰাই result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(সংরচনা Curl_easy *data,
                      unsigned ৱর্ণ *scid,
                      size_t scidlen,
                      পূর্ণাঙ্ক *qlogfdp)
{
  ৱর্ণ *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  যদি(qlog_dir) {
    সংরচনা dynbuf fname;
    CURLcode result;
    unsigned পূর্ণাঙ্ক i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    যদি(!result)
      result = curlx_dyn_add(&fname, "/");
    বাবে(i = 0; (i < scidlen) && !result; i++) {
      ৱর্ণ hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    যদি(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    যদি(!result) {
      পূর্ণাঙ্ক qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_দ্বিমিক,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#অন্যথা
                              (mode_t)data->set.new_file_perms
#endif
                              );
      যদি(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    যদি(result)
      ঘূৰাই result;
  }

  ঘূৰাই CURLE_OK;
}

CURLcode Curl_cf_quic_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা Curl_peer *origin,
                                   সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  CURLcode result;

  (শূন্য)data; /* not used in all cases and compilers are stupid */
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_ঢোকাও_after(cf_at, origin, peer, &cf);
#নোচেদ্যদি defined(USE_QUICHE)
  result = Curl_cf_quiche_ঢোকাও_after(cf_at, origin, peer, &cf);
#অন্যথা
  (শূন্য)cf)
  (শূন্য)cf_at;
  (শূন্য)origin;
  (শূন্য)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#যদি defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  যদি(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  ঘূৰাই result;
}

CURLcode Curl_cf_quic_create(সংরচনা Curl_cfilter **pcf,
                             সংরচনা Curl_easy *data,
                             সংরচনা Curl_peer *origin,
                             সংরচনা Curl_peer *peer,
                             uint8_t transport_peer,
                             সংরচনা connectdata *conn,
                             সংরচনা Curl_sockaddr_ex *addr,
                             সংরচনা Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (শূন্য)transport_peer;
  (শূন্য)tunnel_transport;
  (শূন্য)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ঘূৰাই Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#নোচেদ্যদি defined(USE_QUICHE)
  ঘূৰাই Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#অন্যথা
  *pcf = রিক্ত;
  (শূন্য)data;
  (শূন্য)origin;
  (শূন্য)peer;
  (শূন্য)conn;
  (শূন্য)addr;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

#যদি !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা Curl_peer *origin,
                                       সংরচনা Curl_peer *peer,
                                       সংরচনা Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ঘূৰাই Curl_cf_ngtcp2_proxy_ঢোকাও_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#অন্যথা
  (শূন্য)cf_at;
  (শূন্য)data;
  (শূন্য)origin;
  (শূন্য)peer;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(সংরচনা Curl_cfilter **pcf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা Curl_peer *origin,
                                 সংরচনা Curl_peer *peer,
                                 uint8_t transport_peer,
                                 সংরচনা connectdata *conn,
                                 সংরচনা Curl_sockaddr_ex *addr,
                                 সংরচনা Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ঘূৰাই Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#অন্যথা
  *pcf = রিক্ত;
  (শূন্য)data;
  (শূন্য)conn;
  (শূন্য)addr;
  (শূন্য)peer;
  (শূন্য)transport_peer;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(সংরচনা Curl_easy *data,
                             সংরচনা connectdata *conn)
{
  যদি(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    ঘূৰাই CURLE_QUIC_CONNECT_ERROR;
  }
  যদি(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  যদি(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    ঘূৰাই CURLE_URL_MALFORMAT;
  }
#অন্যথা
  (শূন্য)conn;
#endif

  ঘূৰাই CURLE_OK;
}

#ifdef CURLVERBOSE
ধ্রুৱ ৱর্ণ *Curl_vquic_h3_err_str(uint64_t error_code)
{
  যদি(error_code <= UINT_MAX) {
    ৱিকল্প((unsigned পূর্ণাঙ্ক)error_code) {
    পক্ষ CURL_H3_ERR_NO_ERROR:
      ঘূৰাই "NO_ERROR";
    পক্ষ CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ঘূৰাই "GENERAL_PROTOCOL_ERROR";
    পক্ষ CURL_H3_ERR_INTERNAL_ERROR:
      ঘূৰাই "INTERNAL_ERROR";
    পক্ষ CURL_H3_ERR_STREAM_CREATION_ERROR:
      ঘূৰাই "STREAM_CREATION_ERROR";
    পক্ষ CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      ঘূৰাই "CLOSED_CRITICAL_STREAM";
    পক্ষ CURL_H3_ERR_FRAME_UNEXPECTED:
      ঘূৰাই "FRAME_UNEXPECTED";
    পক্ষ CURL_H3_ERR_FRAME_ERROR:
      ঘূৰাই "FRAME_ERROR";
    পক্ষ CURL_H3_ERR_EXCESSIVE_LOAD:
      ঘূৰাই "EXCESSIVE_LOAD";
    পক্ষ CURL_H3_ERR_ID_ERROR:
      ঘূৰাই "ID_ERROR";
    পক্ষ CURL_H3_ERR_SETTINGS_ERROR:
      ঘূৰাই "SETTINGS_ERROR";
    পক্ষ CURL_H3_ERR_MISSING_SETTINGS:
      ঘূৰাই "MISSING_SETTINGS";
    পক্ষ CURL_H3_ERR_REQUEST_REJECTED:
      ঘূৰাই "REQUEST_REJECTED";
    পক্ষ CURL_H3_ERR_REQUEST_CANCELLED:
      ঘূৰাই "REQUEST_CANCELLED";
    পক্ষ CURL_H3_ERR_REQUEST_INCOMPLETE:
      ঘূৰাই "REQUEST_INCOMPLETE";
    পক্ষ CURL_H3_ERR_MESSAGE_ERROR:
      ঘূৰাই "MESSAGE_ERROR";
    পক্ষ CURL_H3_ERR_CONNECT_ERROR:
      ঘূৰাই "CONNECT_ERROR";
    পক্ষ CURL_H3_ERR_VERSION_FALLBACK:
      ঘূৰাই "VERSION_FALLBACK";
    স্ৱভাৱ:
      ভঙ্গ;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  যদি((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    ঘূৰাই "NO_ERROR";
  ঘূৰাই "unknown";
}
#endif /* CURLVERBOSE */

#যদি defined(USE_NGTCP2) || defined(USE_NGHTTP3)

স্থির শূন্য *vquic_ngtcp2_malloc(size_t size, শূন্য *user_data)
{
  (শূন্য)user_data;
  ঘূৰাই Curl_cmalloc(size);
}

স্থির শূন্য vquic_ngtcp2_free(শূন্য *ptr, শূন্য *user_data)
{
  (শূন্য)user_data;
  Curl_cfree(ptr);
}

স্থির শূন্য *vquic_ngtcp2_calloc(size_t nmemb, size_t size, শূন্য *user_data)
{
  (শূন্য)user_data;
  ঘূৰাই Curl_ccalloc(nmemb, size);
}

স্থির শূন্য *vquic_ngtcp2_realloc(শূন্য *ptr, size_t size, শূন্য *user_data)
{
  (শূন্য)user_data;
  ঘূৰাই Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
স্থির সংরচনা ngtcp2_mem vquic_ngtcp2_mem = {
  রিক্ত,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
সংরচনা ngtcp2_mem *Curl_ngtcp2_mem(শূন্য)
{
  ঘূৰাই &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
স্থির সংরচনা nghttp3_mem vquic_nghttp3_mem = {
  রিক্ত,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
সংরচনা nghttp3_mem *Curl_nghttp3_mem(শূন্য)
{
  ঘূৰাই &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#অন্যথা /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(সংরচনা Curl_easy *data,
                             সংরচনা connectdata *conn)
{
  (শূন্য)data;
  (শূন্য)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  ঘূৰাই CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
