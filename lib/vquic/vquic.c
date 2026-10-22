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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "vtls/vtls.h"

#অন্তর্ভাব "curl_trc.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#অন্তর্ভাব <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#অন্তর্ভাব <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#অন্তর্ভাব <nghttp3/nghttp3.h>
#endif

#যদি defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#অন্তর্ভাব <sys/syscall.h>
#যদি defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_বার্তা_X
#endif
#endif

#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vquic/cf-ngtcp2.h"
#অন্তর্ভাব "vquic/cf-ngtcp2-cmn.h"
#অন্তর্ভাব "vquic/cf-ngtcp2-proxy.h"
#অন্তর্ভাব "vquic/cf-quiche.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "vquic/vquic_int.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strparse.h"


#define NW_CHUNK_আকার     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_বার্তা_X

/* this is `struct msghdr` with an additional field at the end */
কাঠামো msghdr_x {
  শূন্য *বার্তা_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  কাঠামো iovec *msg_iov;    /* scatter/gather array */
  পূর্ণসংখ্যা বার্তা_iovlen;           /* # elements in msg_iov */
  শূন্য *বার্তা_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  পূর্ণসংখ্যা বার্তা_flags;            /* flags on received message */
  size_t বার্তা_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_বার্তা_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#নতুবা_যদি defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#অন্যথা
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

পূর্ণসংখ্যা Curl_vquic_init(শূন্য)
{
#যদি defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  যদি(ngtcp2_crypto_ossl_init())
    ফেরত 0;
#endif

  ফেরত 1;
}

শূন্য Curl_vquic_cleanup(শূন্য)
{
#যদি defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_অঙ্ক >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

শূন্য Curl_quic_ver(অক্ষর *p, size_t len)
{
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#নতুবা_যদি defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(কাঠামো Curl_easy *তথ্য,
                             কাঠামো cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_আকার, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#যদি defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#নতুবা_যদি defined(HAVE_APPLE_বার্তা_X)
  qctx->no_gso = FALSE;
#অন্যথা
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    ধ্রুবক অক্ষর *p = getenv("CURL_DBG_QUIC_WBLOCK");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, 100))
        qctx->wblock_percent = (পূর্ণসংখ্যা)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(তথ্য));

  ফেরত CURLE_OK;
}

শূন্য Curl_vquic_ctx_free(কাঠামো cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

শূন্য Curl_vquic_ctx_set_time(কাঠামো cf_quic_ctx *qctx,
                             ধ্রুবক কাঠামো curltime *pnow)
{
  qctx->last_op = *pnow;
}

শূন্য Curl_vquic_ctx_update_time(কাঠামো cf_quic_ctx *qctx,
                                ধ্রুবক কাঠামো curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_বার্তা_X
স্থির CURLcode do_sendmsg(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো cf_quic_ctx *qctx,
                           ধ্রুবক uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#define বার্তা_X_SNUM  64
  কাঠামো iovec msg_iov[বার্তা_X_SNUM];
  কাঠামো msghdr_x mmsg[বার্তা_X_SNUM];
  অক্ষর errstr[STRERROR_LEN];
  size_t n, i = 0, sent = 0;
  পূর্ণসংখ্যা rc;
  CURLcode ফলাফল = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  যদি(!pktlen)
    ফেরত CURLE_OK;
  যদি(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  যখন(i < n) {
    size_t j, batch = CURLMIN(n - i, বার্তা_X_SNUM), pkts_sent = 0;

    জন্য(j = 0; j < batch; ++j) {
      ধ্রুবক size_t offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      memset(&mmsg[j], 0, আকার(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].বার্তা_iovlen = 1;
      mmsg[j].বার্তা_datalen = msg_iov[j].iov_len;
    }

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    যখন((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic বের_করো
#endif

    যদি(rc < 0) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(তথ্য, cf, "egress, sendmsg_x -> EAGAIN");
        ফলাফল = sent ? CURLE_OK : CURLE_AGAIN;
        যাও out;
      }
      যদি(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, আকার(errstr));
        failf(তথ্য, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        ফলাফল = CURLE_SEND_ERROR;
        যাও out;
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
      ফলাফল = sent ? CURLE_OK : CURLE_AGAIN;
      যাও out;
    }
    i += pkts_sent; /* Some have been sent */
    জন্য(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    যদি(pkts_sent < batch)
      যাও out; /* but not all of them */
  }

out:
  *psent = sent;
  যদি(sent || ফলাফল)
    CURL_TRC_CF(তথ্য, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

#অন্যথা /* HAVE_APPLE_MSG_X */

স্থির CURLcode send_packet_no_gso(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো cf_quic_ctx *qctx,
                                   ধ্রুবক uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

স্থির CURLcode do_sendmsg(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো cf_quic_ctx *qctx,
                           ধ্রুবক uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode ফলাফল = CURLE_OK;
#ifdef HAVE_SENDMSG
  কাঠামো iovec msg_iov;
  কাঠামো msghdr বার্তা = { 0 };
  ssize_t rv;
#যদি defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t বার্তা_ctrl[32];
  কাঠামো cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  বার্তা.msg_iov = &msg_iov;
  বার্তা.বার্তা_iovlen = 1;

#যদি defined(__linux__) && defined(UDP_SEGMENT)
  যদি(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(বার্তা_ctrl, 0, আকার(বার্তা_ctrl));
    বার্তা.বার্তা_control = বার্তা_ctrl;
    DEBUGASSERT(আকার(বার্তা_ctrl) >= CMSG_SPACE(আকার(পূর্ণসংখ্যা)));
    বার্তা.msg_controllen = CMSG_SPACE(আকার(পূর্ণসংখ্যা));
    cm = CMSG_FIRSTHDR(&বার্তা);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(আকার(uint16_t));
    *(uint16_t *)(শূন্য *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  যখন((rv = sendmsg(qctx->sockfd, &বার্তা, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  যদি(!curlx_sztouz(rv, psent)) {
    পূর্ণসংখ্যা sockerr = SOCKERRNO;
    যদি(SOCK_EAGAIN(sockerr))
      ফেরত CURLE_AGAIN;
    নির্বাচন(sockerr) {
    ক্ষেত্র SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      বিরতি;
    ক্ষেত্র EIO:
      যদি(pktlen > gsolen) {
        /* GSO failure */
        infof(তথ্য, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        ফেরত send_packet_no_gso(cf, তথ্য, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    পূর্বনির্ধারিত:
      failf(তথ্য, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }
  অন্যথা যদি(pktlen != *psent) {
    failf(তথ্য, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
#অন্যথা
  ssize_t rv;
  (শূন্য)gsolen;

  *psent = 0;

  যখন((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  যদি(!curlx_sztouz(rv, psent)) {
    যদি(SOCK_EAGAIN(SOCKERRNO)) {
      ফলাফল = CURLE_AGAIN;
      যাও out;
    }
    অন্যথা {
      যদি(SOCKERRNO != SOCKEMSGSIZE) {
        failf(তথ্য, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        ফলাফল = CURLE_SEND_ERROR;
        যাও out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (শূন্য)cf;

out:
  CURL_TRC_CF(তথ্য, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (পূর্ণসংখ্যা)ফলাফল, *psent);
  ফেরত ফলাফল;
}

#endif /* !HAVE_APPLE_MSG_X */

স্থির CURLcode send_packet_no_gso(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো cf_quic_ctx *qctx,
                                   ধ্রুবক uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ধ্রুবক uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode ফলাফল = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  জন্য(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    ফলাফল = do_sendmsg(cf, তথ্য, qctx, p, len, len, &sent);
    যদি(ফলাফল)
      যাও out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(তথ্য, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (পূর্ণসংখ্যা)ফলাফল, *psent);
  ফেরত ফলাফল;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
স্থির CURLcode send_packet_no_gso_cf(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      ধ্রুবক uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ধ্রুবক uint8_t *p, *end = pkt + pktlen;
  size_t sent, len;
  CURLcode ফলাফল = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  জন্য(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (size_t)(end - p));
    ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    যদি(ফলাফল)
      যাও out;

    যদি(sent != len) {
      /* We can only send the complete datagram, not parts. */
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(তথ্য, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (পূর্ণসংখ্যা)ফলাফল, *psent);
  ফেরত ফলাফল;
}

স্থির CURLcode vquic_send_packets(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো cf_quic_ctx *qctx,
                                   ধ্রুবক uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode ফলাফল;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  যদি(qctx->wblock_percent > 0) {
    unsigned অক্ষর c;
    *psent = 0;
    Curl_rand(তথ্য, &c, 1);
    যদি(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(তথ্য, cf, "vquic_flush() simulate EWOULDBLOCK");
      ফেরত CURLE_AGAIN;
    }
  }
#endif
  যদি(qctx->no_gso && pktlen > gsolen) {
    ফলাফল = send_packet_no_gso(cf, তথ্য, qctx, pkt, pktlen, gsolen, psent);
  }
  অন্যথা {
    ফলাফল = do_sendmsg(cf, তথ্য, qctx, pkt, pktlen, gsolen, psent);
  }
  যদি(!ফলাফল)
    qctx->last_io = qctx->last_op;
  ফেরত ফলাফল;
}

CURLcode Curl_vquic_flush(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                          কাঠামো cf_quic_ctx *qctx)
{
  ধ্রুবক unsigned অক্ষর *buf;
  size_t blen, sent;
  CURLcode ফলাফল;
  size_t gsolen;

  যখন(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    যদি(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      যদি(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    যদি(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      ফলাফল = vquic_send_packets(cf, তথ্য, qctx, buf, blen, gsolen, &sent);
    }
    অন্যথা {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      যদি(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        ফলাফল = send_packet_no_gso_cf(cf, তথ্য, buf, blen, gsolen, &sent);
      }
      অন্যথা {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, buf, blen, FALSE, &sent);
      }
    }

    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        যদি(qctx->split_len)
          qctx->split_len -= sent;
      }
      ফেরত ফলাফল;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    যদি(qctx->split_len)
      qctx->split_len -= sent;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_vquic_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                         কাঠামো cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  ফেরত Curl_vquic_flush(cf, তথ্য, qctx);
}

CURLcode Curl_vquic_send_লেজ_split(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো cf_quic_ctx *qctx, size_t gsolen,
                                    size_t লেজ_len, size_t লেজ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > লেজ_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - লেজ_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = লেজ_gsolen;
  CURL_TRC_CF(তথ্য, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, লেজ_len, qctx->gsolen);
  ফেরত Curl_vquic_flush(cf, তথ্য, qctx);
}

#যদি (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_বার্তা_X)
স্থির size_t vquic_msghdr_get_udp_gro(কাঠামো msghdr *বার্তা)
{
  পূর্ণসংখ্যা gso_আকার = 0;
#যদি defined(__linux__) && defined(UDP_GRO)
  কাঠামো cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  জন্য(cmsg = CMSG_FIRSTHDR(বার্তা); cmsg; cmsg = CMSG_NXTHDR(বার্তা, cmsg)) {
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic বের_করো
#endif
    যদি(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      memcpy(&gso_আকার, CMSG_DATA(cmsg), আকার(gso_আকার));

      বিরতি;
    }
  }
#endif /* linux && UDP_GRO */
  (শূন্য)বার্তা;

  ফেরত (size_t)gso_আকার;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#যদি (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_বার্তা_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
স্থির uint8_t vquic_msghdr_get_ecn(কাঠামো msghdr *বার্তা, পূর্ণসংখ্যা family)
{
  কাঠামো cmsghdr *cmsg;
  নির্বাচন(family) {
  ক্ষেত্র AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    জন্য(cmsg = CMSG_FIRSTHDR(বার্তা); cmsg; cmsg = CMSG_NXTHDR(বার্তা, cmsg)) {
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic বের_করো
#endif
      যদি(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#অন্যথা
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        ফেরত *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    বিরতি;
  ক্ষেত্র AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    জন্য(cmsg = CMSG_FIRSTHDR(বার্তা); cmsg; cmsg = CMSG_NXTHDR(বার্তা, cmsg)) {
#যদি defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic বের_করো
#endif
      যদি(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        unsigned পূর্ণসংখ্যা tos;

        memcpy(&tos, CMSG_DATA(cmsg), আকার(পূর্ণসংখ্যা));

        ফেরত (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    বিরতি;
  }
  ফেরত 0;
}
#অন্যথা
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

স্থির CURLcode recvmmsg_packets(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp,
                                 size_t *pnread)
{
#যদি defined(__linux__) && defined(UDP_GRO)
#define MMSG_অঙ্ক  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_বার্তা_আকার    (2 * CMSG_SPACE(আকার(পূর্ণসংখ্যা)))
#অন্যথা
#define MMSG_অঙ্ক  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_বার্তা_আকার    CMSG_SPACE(আকার(পূর্ণসংখ্যা))
#endif
#define বার্তা_BUF_আকার  (UDP_GRO_CNT_MAX * 1500)
  কাঠামো iovec msg_iov[MMSG_অঙ্ক];
  কাঠামো mmsghdr mmsg[MMSG_অঙ্ক];
  uint8_t বার্তা_ctrl[MMSG_অঙ্ক * CMSG_PER_বার্তা_আকার];
  কাঠামো sockaddr_storage remote_addr[MMSG_অঙ্ক];
  size_t যোগ_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  পূর্ণসংখ্যা mcount, i, n;
  অক্ষর errstr[STRERROR_LEN];
  CURLcode ফলাফল = CURLE_OK;
  size_t gso_আকার;
  অক্ষর *sockbuf = ফাঁকা;
  uint8_t (*bufs)[বার্তা_BUF_আকার] = ফাঁকা;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ফলাফল = Curl_multi_xfer_sockbuf_borrow(তথ্য, MMSG_অঙ্ক * বার্তা_BUF_আকার,
                                          &sockbuf);
  যদি(ফলাফল)
    যাও out;
  bufs = (uint8_t (*)[বার্তা_BUF_আকার])sockbuf;

  যোগ_nread = 0;
  যখন(pkts < max_pkts) {
    n = (পূর্ণসংখ্যা)CURLMIN(CURLMIN(MMSG_অঙ্ক, IOV_MAX), max_pkts);
    memset(&mmsg, 0, আকার(mmsg));
    জন্য(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = আকার(bufs[i]);
      mmsg[i].বার্তা_hdr.msg_iov = &msg_iov[i];
      mmsg[i].বার্তা_hdr.বার্তা_iovlen = 1;
      mmsg[i].বার্তা_hdr.বার্তা_name = &remote_addr[i];
      mmsg[i].বার্তা_hdr.msg_namelen = আকার(remote_addr[i]);
      mmsg[i].বার্তা_hdr.বার্তা_control = &বার্তা_ctrl[i * CMSG_PER_বার্তা_আকার];
      mmsg[i].বার্তা_hdr.msg_controllen = CMSG_PER_বার্তা_আকার;
    }

    যখন((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, ফাঁকা)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    যদি(mcount == -1) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(তথ্য, cf, "ingress, recvmmsg -> EAGAIN");
        যাও out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        কাঠামো ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, ফাঁকা, ফাঁকা, &ip))
          failf(তথ্য, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        ফলাফল = CURLE_COULDNT_CONNECT;
        যাও out;
      }
      curlx_strerror(SOCKERRNO, errstr, আকার(errstr));
      failf(তথ্য, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }

    VERBOSE(++calls);
    জন্য(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      যদি(!mmsg[i].বার্তা_len) {
        ++pkts;
        চালু;
      }
      যোগ_nread += mmsg[i].বার্তা_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].বার্তা_hdr, remote_addr[i].ss_family);
      gso_আকার = vquic_msghdr_get_udp_gro(&mmsg[i].বার্তা_hdr);
      যদি(gso_আকার == 0)
        gso_আকার = mmsg[i].বার্তা_len;

      ফলাফল = recv_cb(bufs[i], mmsg[i].বার্তা_len, gso_আকার,
                       mmsg[i].বার্তা_hdr.বার্তা_name,
                       mmsg[i].বার্তা_hdr.msg_namelen, ecn, userp);
      যদি(ফলাফল)
        যাও out;
      pkts += (mmsg[i].বার্তা_len + gso_আকার - 1) / gso_আকার;
    }
  }

out:
  যদি(যোগ_nread || ফলাফল)
    CURL_TRC_CF(তথ্য, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                যোগ_nread, pkts, calls, (পূর্ণসংখ্যা)ফলাফল);
  Curl_multi_xfer_sockbuf_release(তথ্য, sockbuf);
  *pnread = যোগ_nread;
  ফেরত ফলাফল;
}

#নতুবা_যদি defined(HAVE_APPLE_বার্তা_X)

স্থির CURLcode recvmsg_x_packets(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  শূন্য *userp, size_t *pnread)
{
#define বার্তা_X_অঙ্ক  64
#define বার্তা_BUF_আকার  (2048)
#define CMSG_PER_বার্তা_আকার     CMSG_SPACE(আকার(পূর্ণসংখ্যা))
  কাঠামো iovec msg_iov[বার্তা_X_অঙ্ক];
  কাঠামো msghdr_x mmsg[বার্তা_X_অঙ্ক];
  uint8_t বার্তা_ctrl[বার্তা_X_অঙ্ক * CMSG_PER_বার্তা_আকার];
  কাঠামো sockaddr_storage remote_addr[বার্তা_X_অঙ্ক];
  size_t যোগ_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  size_t calls = 0;
#endif
  পূর্ণসংখ্যা mcount, i;
  অক্ষর errstr[STRERROR_LEN];
  CURLcode ফলাফল = CURLE_OK;
  size_t gso_আকার;
  অক্ষর *sockbuf = ফাঁকা;
  uint8_t (*bufs)[বার্তা_BUF_আকার] = ফাঁকা;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  ফলাফল = Curl_multi_xfer_sockbuf_borrow(তথ্য, বার্তা_X_অঙ্ক * বার্তা_BUF_আকার,
                                          &sockbuf);
  যদি(ফলাফল)
    যাও out;
  bufs = (uint8_t (*)[বার্তা_BUF_আকার])sockbuf;

  যোগ_nread = 0;
  যখন(pkts < max_pkts) {
    পূর্ণসংখ্যা n = (পূর্ণসংখ্যা)CURLMIN(CURLMIN(বার্তা_X_অঙ্ক, IOV_MAX), max_pkts);
    memset(&mmsg, 0, আকার(mmsg));
    জন্য(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = আকার(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].বার্তা_iovlen = 1;
      mmsg[i].বার্তা_name = &remote_addr[i];
      mmsg[i].msg_namelen = আকার(remote_addr[i]);
      mmsg[i].বার্তা_control = &বার্তা_ctrl[i * CMSG_PER_বার্তা_আকার];
      mmsg[i].msg_controllen = CMSG_PER_বার্তা_আকার;
    }

#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    যখন((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#যদি defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic বের_করো
#endif
    যদি(mcount == -1) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(তথ্য, cf, "ingress, recvmsg_x -> EAGAIN");
        যাও out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        কাঠামো ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, ফাঁকা, ফাঁকা, &ip))
          failf(তথ্য, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        ফলাফল = CURLE_COULDNT_CONNECT;
        যাও out;
      }
      curlx_strerror(SOCKERRNO, errstr, আকার(errstr));
      failf(তথ্য, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }

    VERBOSE(++calls);
    জন্য(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      যদি(!mmsg[i].বার্তা_datalen) {
        ++pkts;
        চালু;
      }
      যোগ_nread += mmsg[i].বার্তা_datalen;
      ecn = vquic_msghdr_get_ecn((কাঠামো msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_আকার = mmsg[i].বার্তা_datalen;

      ফলাফল = recv_cb(bufs[i], mmsg[i].বার্তা_datalen, gso_আকার,
                       mmsg[i].বার্তা_name,
                       mmsg[i].msg_namelen, ecn, userp);
      যদি(ফলাফল)
        যাও out;
      pkts += (mmsg[i].বার্তা_datalen + gso_আকার - 1) / gso_আকার;
    }
  }

out:
  যদি(যোগ_nread || ফলাফল)
    CURL_TRC_CF(তথ্য, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                যোগ_nread, pkts, calls, (পূর্ণসংখ্যা)ফলাফল);
  Curl_multi_xfer_sockbuf_release(তথ্য, sockbuf);
  *pnread = যোগ_nread;
  ফেরত ফলাফল;
}

#নতুবা_যদি defined(HAVE_SENDMSG)
স্থির CURLcode recvmsg_packets(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp,
                                size_t *pnread)
{
#define CMSG_PER_বার্তা_আকার    CMSG_SPACE(আকার(পূর্ণসংখ্যা))
  কাঠামো iovec msg_iov;
  কাঠামো msghdr বার্তা;
  uint8_t buf[64 * 1024];
  কাঠামো sockaddr_storage remote_addr;
  size_t যোগ_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  অক্ষর errstr[STRERROR_LEN];
  CURLcode ফলাফল = CURLE_OK;
  uint8_t বার্তা_ctrl[CMSG_PER_বার্তা_আকার];
  size_t gso_আকার;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  জন্য(pkts = 0, যোগ_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&বার্তা, 0, আকার(বার্তা));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = আকার(buf);
    বার্তা.msg_iov = &msg_iov;
    বার্তা.বার্তা_iovlen = 1;
    বার্তা.বার্তা_control = বার্তা_ctrl;
    বার্তা.বার্তা_name = &remote_addr;
    বার্তা.msg_namelen = আকার(remote_addr);
    বার্তা.msg_controllen = CMSG_PER_বার্তা_আকার;

    যখন((rc = recvmsg(qctx->sockfd, &বার্তা, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    যদি(!curlx_sztouz(rc, &nread)) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        যাও out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        কাঠামো ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, ফাঁকা, ফাঁকা, &ip))
          failf(তথ্য, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        ফলাফল = CURLE_COULDNT_CONNECT;
        যাও out;
      }
      curlx_strerror(SOCKERRNO, errstr, আকার(errstr));
      failf(তথ্য, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }

    যোগ_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    যদি(!nread) {
      ++pkts;
      চালু;
    }

    ecn = vquic_msghdr_get_ecn(&বার্তা, remote_addr.ss_family);
    gso_আকার = vquic_msghdr_get_udp_gro(&বার্তা);
    যদি(gso_আকার == 0)
      gso_আকার = nread;

    ফলাফল = recv_cb(buf, nread, gso_আকার,
                     বার্তা.বার্তা_name, বার্তা.msg_namelen, ecn, userp);
    যদি(ফলাফল)
      যাও out;
    pkts += (nread + gso_আকার - 1) / gso_আকার;
  }

out:
  যদি(যোগ_nread || ফলাফল)
    CURL_TRC_CF(তথ্য, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                যোগ_nread, pkts, calls, (পূর্ণসংখ্যা)ফলাফল);
  *pnread = যোগ_nread;
  ফেরত ফলাফল;
}

#অন্যথা /* HAVE_SENDMMSG || HAVE_SENDMSG */
স্থির CURLcode recvfrom_packets(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  পূর্ণসংখ্যা bufsize = (পূর্ণসংখ্যা)আকার(buf);
  কাঠামো sockaddr_storage remote_addr;
  socklen_t remote_addrlen = আকার(remote_addr);
  size_t যোগ_nread, pkts, calls = 0, nread;
  ssize_t rv;
  অক্ষর errstr[STRERROR_LEN];
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  জন্য(pkts = 0, যোগ_nread = 0; pkts < max_pkts;) {
    যখন((rv = recvfrom(qctx->sockfd, (অক্ষর *)buf, bufsize, 0,
                         (কাঠামো sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    যদি(!curlx_sztouz(rv, &nread)) {
      যদি(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(তথ্য, cf, "ingress, recvfrom -> EAGAIN");
        যাও out;
      }
      যদি(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        কাঠামো ip_quadruple ip;
        যদি(!Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, ফাঁকা, ফাঁকা, &ip))
          failf(তথ্য, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        ফলাফল = CURLE_COULDNT_CONNECT;
        যাও out;
      }
      curlx_strerror(SOCKERRNO, errstr, আকার(errstr));
      failf(তথ্য, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    যদি(!nread)
      চালু;

    যোগ_nread += nread;
    ফলাফল = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    যদি(ফলাফল)
      যাও out;
  }

out:
  যদি(যোগ_nread || ফলাফল)
    CURL_TRC_CF(তথ্য, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                যোগ_nread, pkts, calls, (পূর্ণসংখ্যা)ফলাফল);
  *pnread = যোগ_nread;
  ফেরত ফলাফল;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, শূন্য *userp)
{
  CURLcode ফলাফল;
  size_t nread = 0;
#ifdef HAVE_SENDMMSG
  ফলাফল = recvmmsg_packets(cf, তথ্য, qctx, max_pkts, recv_cb, userp, &nread);
#নতুবা_যদি defined(HAVE_APPLE_বার্তা_X)
  ফলাফল = recvmsg_x_packets(cf, তথ্য, qctx, max_pkts, recv_cb, userp,
                             &nread);
#নতুবা_যদি defined(HAVE_SENDMSG)
  ফলাফল = recvmsg_packets(cf, তথ্য, qctx, max_pkts, recv_cb, userp, &nread);
#অন্যথা
  ফলাফল = recvfrom_packets(cf, তথ্য, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  যদি(!ফলাফল) {
    /* EAGAIN is no error, only count actual packets as a reply */
    যদি(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  ফেরত ফলাফল;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(কাঠামো Curl_easy *তথ্য,
                      unsigned অক্ষর *scid,
                      size_t scidlen,
                      পূর্ণসংখ্যা *qlogfdp)
{
  অক্ষর *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  যদি(qlog_dir) {
    কাঠামো dynbuf fname;
    CURLcode ফলাফল;
    unsigned পূর্ণসংখ্যা i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    ফলাফল = curlx_dyn_add(&fname, qlog_dir);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_add(&fname, "/");
    জন্য(i = 0; (i < scidlen) && !ফলাফল; i++) {
      অক্ষর hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      ফলাফল = curlx_dyn_add(&fname, hex);
    }
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_add(&fname, ".sqlog");

    যদি(!ফলাফল) {
      পূর্ণসংখ্যা qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_দ্বিমিক,
#ifdef _WIN32
                              তথ্য->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#অন্যথা
                              (mode_t)তথ্য->set.new_file_perms
#endif
                              );
      যদি(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফেরত CURLE_OK;
}

CURLcode Curl_cf_quic_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_peer *origin,
                                   কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)তথ্য; /* not used in all cases and compilers are stupid */
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ফলাফল = Curl_cf_ngtcp2_ঢোকাও_after(cf_at, origin, peer, &cf);
#নতুবা_যদি defined(USE_QUICHE)
  ফলাফল = Curl_cf_quiche_ঢোকাও_after(cf_at, origin, peer, &cf);
#অন্যথা
  (শূন্য)cf)
  (শূন্য)cf_at;
  (শূন্য)origin;
  (শূন্য)peer;
  ফলাফল = CURLE_NOT_BUILT_IN;
#endif

#যদি defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  যদি(!ফলাফল && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    ফলাফল = Curl_conn_dns_add_https_resolve(তথ্য, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  ফেরত ফলাফল;
}

CURLcode Curl_cf_quic_create(কাঠামো Curl_cfilter **pcf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_peer *origin,
                             কাঠামো Curl_peer *peer,
                             uint8_t transport_peer,
                             কাঠামো connectdata *conn,
                             কাঠামো Curl_sockaddr_ex *addr,
                             কাঠামো Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (শূন্য)transport_peer;
  (শূন্য)tunnel_transport;
  (শূন্য)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ফেরত Curl_cf_ngtcp2_create(pcf, তথ্য, origin, peer, conn, addr);
#নতুবা_যদি defined(USE_QUICHE)
  ফেরত Curl_cf_quiche_create(pcf, তথ্য, origin, peer, conn, addr);
#অন্যথা
  *pcf = ফাঁকা;
  (শূন্য)তথ্য;
  (শূন্য)origin;
  (শূন্য)peer;
  (শূন্য)conn;
  (শূন্য)addr;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

#যদি !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_peer *origin,
                                       কাঠামো Curl_peer *peer,
                                       কাঠামো Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ফেরত Curl_cf_ngtcp2_proxy_ঢোকাও_after(cf_at, তথ্য, origin, peer,
                                           tunnel_peer, tunnel_transport);
#অন্যথা
  (শূন্য)cf_at;
  (শূন্য)তথ্য;
  (শূন্য)origin;
  (শূন্য)peer;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(কাঠামো Curl_cfilter **pcf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_peer *origin,
                                 কাঠামো Curl_peer *peer,
                                 uint8_t transport_peer,
                                 কাঠামো connectdata *conn,
                                 কাঠামো Curl_sockaddr_ex *addr,
                                 কাঠামো Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  ফেরত Curl_cf_ngtcp2_proxy_create(pcf, তথ্য, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#অন্যথা
  *pcf = ফাঁকা;
  (শূন্য)তথ্য;
  (শূন্য)conn;
  (শূন্য)addr;
  (শূন্য)peer;
  (শূন্য)transport_peer;
  (শূন্য)tunnel_peer;
  (শূন্য)tunnel_transport;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn)
{
  যদি(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(তথ্য, "HTTP/3 cannot be used over UNIX domain sockets");
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }
  যদি(!(তথ্য->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(তথ্য, "HTTP/3 requested for non-HTTPS URL");
    ফেরত CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  যদি(conn->socks_proxy.peer) {
    failf(তথ্য, "HTTP/3 is not supported over a SOCKS proxy");
    ফেরত CURLE_URL_MALFORMAT;
  }
#অন্যথা
  (শূন্য)conn;
#endif

  ফেরত CURLE_OK;
}

#ifdef CURLVERBOSE
ধ্রুবক অক্ষর *Curl_vquic_h3_err_str(uint64_t error_code)
{
  যদি(error_code <= UINT_MAX) {
    নির্বাচন((unsigned পূর্ণসংখ্যা)error_code) {
    ক্ষেত্র CURL_H3_ERR_NO_ERROR:
      ফেরত "NO_ERROR";
    ক্ষেত্র CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      ফেরত "GENERAL_PROTOCOL_ERROR";
    ক্ষেত্র CURL_H3_ERR_INTERNAL_ERROR:
      ফেরত "INTERNAL_ERROR";
    ক্ষেত্র CURL_H3_ERR_STREAM_CREATION_ERROR:
      ফেরত "STREAM_CREATION_ERROR";
    ক্ষেত্র CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      ফেরত "CLOSED_CRITICAL_STREAM";
    ক্ষেত্র CURL_H3_ERR_FRAME_UNEXPECTED:
      ফেরত "FRAME_UNEXPECTED";
    ক্ষেত্র CURL_H3_ERR_FRAME_ERROR:
      ফেরত "FRAME_ERROR";
    ক্ষেত্র CURL_H3_ERR_EXCESSIVE_LOAD:
      ফেরত "EXCESSIVE_LOAD";
    ক্ষেত্র CURL_H3_ERR_ID_ERROR:
      ফেরত "ID_ERROR";
    ক্ষেত্র CURL_H3_ERR_SETTINGS_ERROR:
      ফেরত "SETTINGS_ERROR";
    ক্ষেত্র CURL_H3_ERR_MISSING_SETTINGS:
      ফেরত "MISSING_SETTINGS";
    ক্ষেত্র CURL_H3_ERR_REQUEST_REJECTED:
      ফেরত "REQUEST_REJECTED";
    ক্ষেত্র CURL_H3_ERR_REQUEST_CANCELLED:
      ফেরত "REQUEST_CANCELLED";
    ক্ষেত্র CURL_H3_ERR_REQUEST_INCOMPLETE:
      ফেরত "REQUEST_INCOMPLETE";
    ক্ষেত্র CURL_H3_ERR_MESSAGE_ERROR:
      ফেরত "MESSAGE_ERROR";
    ক্ষেত্র CURL_H3_ERR_CONNECT_ERROR:
      ফেরত "CONNECT_ERROR";
    ক্ষেত্র CURL_H3_ERR_VERSION_FALLBACK:
      ফেরত "VERSION_FALLBACK";
    পূর্বনির্ধারিত:
      বিরতি;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  যদি((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    ফেরত "NO_ERROR";
  ফেরত "unknown";
}
#endif /* CURLVERBOSE */

#যদি defined(USE_NGTCP2) || defined(USE_NGHTTP3)

স্থির শূন্য *vquic_ngtcp2_malloc(size_t আকার_2, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  ফেরত Curl_cmalloc(আকার_2);
}

স্থির শূন্য vquic_ngtcp2_free(শূন্য *ptr, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  Curl_cfree(ptr);
}

স্থির শূন্য *vquic_ngtcp2_calloc(size_t nmemb, size_t আকার_2, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  ফেরত Curl_ccalloc(nmemb, আকার_2);
}

স্থির শূন্য *vquic_ngtcp2_realloc(শূন্য *ptr, size_t আকার_2, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  ফেরত Curl_crealloc(ptr, আকার_2);
}

#ifdef USE_NGTCP2
স্থির কাঠামো ngtcp2_mem vquic_ngtcp2_mem = {
  ফাঁকা,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
কাঠামো ngtcp2_mem *Curl_ngtcp2_mem(শূন্য)
{
  ফেরত &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
স্থির কাঠামো nghttp3_mem vquic_nghttp3_mem = {
  ফাঁকা,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
কাঠামো nghttp3_mem *Curl_nghttp3_mem(শূন্য)
{
  ফেরত &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#অন্যথা /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn)
{
  (শূন্য)তথ্য;
  (শূন্য)conn;
  DEBUGF(infof(তথ্য, "QUIC is not supported in this build"));
  ফেরত CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
