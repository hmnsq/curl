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

#که_تعریف_شوی HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETINET_IP_H
#ضمن <netinet/ip.h>
#د_که_پای

#که_تعریف_شوی USE_NGHTTP3
#ضمن <nghttp3/nghttp3.h>
#د_که_پای

#إذا defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ضمن <sys/syscall.h>
#إذا defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#تعریف_کړه HAVE_APPLE_پیغام_X
#د_که_پای
#د_که_پای

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


#تعریف_کړه NW_CHUNK_اندازه     (64 * 1024)
#تعریف_کړه NW_SEND_CHUNKS    1

#که_تعریف_شوی HAVE_APPLE_پیغام_X

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
#د_که_پای

#که_تعریف_شوی CURLVERBOSE
#که_تعریف_شوی HAVE_APPLE_پیغام_X
#تعریف_کړه VQUIC_SEND_لاره   "sendmsg_x"
#وإلاإذا defined(HAVE_SENDMSG)
#تعریف_کړه VQUIC_SEND_لاره   "sendmsg"
#وإلا
#تعریف_کړه VQUIC_SEND_لاره   "send"
#د_که_پای
#د_که_پای

صحيح Curl_vquic_init(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_init())
    أرجع 0;
#د_که_پای

  أرجع 1;
}

فراغ Curl_vquic_cleanup(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_نسخه_عدد >= 0x011800)
  ngtcp2_crypto_ossl_free();
#د_که_پای
}

فراغ Curl_quic_ver(حرف *p, size_t len)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#وإلاإذا defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#د_که_پای
}

CURLcode Curl_vquic_ctx_init(بنية Curl_easy *معلومات,
                             بنية cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_اندازه, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
#إذا defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#وإلاإذا defined(HAVE_APPLE_پیغام_X)
  qctx->no_gso = FALSE;
#وإلا
  qctx->no_gso = TRUE;
#د_که_پای
#که_تعریف_شوی DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_QUIC_WBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, 100))
        qctx->wblock_percent = (صحيح)l;
    }
  }
#د_که_پای
  Curl_vquic_ctx_سېټ_وخت(qctx, Curl_pgrs_now(معلومات));

  أرجع CURLE_OK;
}

فراغ Curl_vquic_ctx_free(بنية cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

فراغ Curl_vquic_ctx_سېټ_وخت(بنية cf_quic_ctx *qctx,
                             ثابت بنية curltime *pnow)
{
  qctx->وروستی_op = *pnow;
}

فراغ Curl_vquic_ctx_update_وخت(بنية cf_quic_ctx *qctx,
                                ثابت بنية curltime *pnow)
{
  qctx->وروستی_op = *pnow;
}

#که_تعریف_شوی HAVE_APPLE_پیغام_X
ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#تعریف_کړه پیغام_X_SNUM  64
  بنية iovec msg_iov[پیغام_X_SNUM];
  بنية msghdr_x mmsg[پیغام_X_SNUM];
  حرف errstr[STRERROR_LEN];
  size_t n, i = 0, لېږل_شوی = 0;
  صحيح rc;
  CURLcode پایله = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  إذا(!pktlen)
    أرجع CURLE_OK;
  إذا(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  طالما(i < n) {
    size_t j, batch = CURLMIN(n - i, پیغام_X_SNUM), pkts_لېږل_شوی = 0;

    لكل(j = 0; j < batch; ++j) {
      ثابت size_t دننه_واټن = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + دننه_واټن);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - دننه_واټن);
      memset(&mmsg[j], 0, حجم(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].پیغام_iovlen = 1;
      mmsg[j].پیغام_datalen = msg_iov[j].iov_len;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wdeprecated-declarations"
#د_که_پای
    طالما((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

    إذا(rc < 0) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "egress, sendmsg_x -> EAGAIN");
        پایله = لېږل_شوی ? CURLE_OK : CURLE_AGAIN;
        اذهب بهر;
      }
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
        failf(معلومات, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        پایله = CURLE_SEND_ERROR;
        اذهب بهر;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_لېږل_شوی = batch;
    }
    وإلا
      pkts_لېږل_شوی = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    إذا(!pkts_لېږل_شوی) {  /* no packets of the current batch were sent */
      پایله = لېږل_شوی ? CURLE_OK : CURLE_AGAIN;
      اذهب بهر;
    }
    i += pkts_لېږل_شوی; /* Some have been sent */
    لكل(j = 0; j < pkts_لېږل_شوی; ++j)
      لېږل_شوی += msg_iov[j].iov_len;
    إذا(pkts_لېږل_شوی < batch)
      اذهب بهر; /* but not all of them */
  }

بهر:
  *psent = لېږل_شوی;
  إذا(لېږل_شوی || پایله)
    CURL_TRC_CF(معلومات, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                لېږل_شوی, gsolen, n, calls, (صحيح)پایله);
  أرجع پایله;
}

#وإلا /* HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode پایله = CURLE_OK;
#که_تعریف_شوی HAVE_SENDMSG
  بنية iovec msg_iov;
  بنية msghdr پیغام = { 0 };
  ssize_t rv;
#إذا defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t پیغام_ctrl[32];
  بنية cmsghdr *cm;
#د_که_پای

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
#د_که_پای

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
        infof(معلومات, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        أرجع send_packet_no_gso(cf, معلومات, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    افتراضي:
      failf(معلومات, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
  }
  وإلا إذا(pktlen != *psent) {
    failf(معلومات, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
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
      پایله = CURLE_AGAIN;
      اذهب بهر;
    }
    وإلا {
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        failf(معلومات, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        پایله = CURLE_SEND_ERROR;
        اذهب بهر;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#د_که_پای
  (فراغ)cf;

بهر:
  CURL_TRC_CF(معلومات, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_لاره, pktlen, gsolen, (صحيح)پایله, *psent);
  أرجع پایله;
}

#د_که_پای /* !HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *پای_ټکی = pkt + pktlen;
  size_t لېږل_شوی, len;
  CURLcode پایله = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  لكل(p = pkt; p < پای_ټکی; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(پای_ټکی - p));
    پایله = do_sendmsg(cf, معلومات, qctx, p, len, len, &لېږل_شوی);
    إذا(پایله)
      اذهب بهر;
    *psent += لېږل_شوی;
    VERBOSE(++calls);
  }
بهر:
  CURL_TRC_CF(معلومات, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_لاره, pktlen, gsolen, calls, (صحيح)پایله, *psent);
  أرجع پایله;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ساكن CURLcode send_packet_no_gso_cf(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      ثابت uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *پای_ټکی = pkt + pktlen;
  size_t لېږل_شوی, len;
  CURLcode پایله = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  لكل(p = pkt; p < پای_ټکی; p += len) {
    len = CURLMIN(gsolen, (size_t)(پای_ټکی - p));
    پایله = Curl_conn_cf_send(cf->بل, معلومات, p, len, FALSE, &لېږل_شوی);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    إذا(پایله)
      اذهب بهر;

    إذا(لېږل_شوی != len) {
      /* We can only send the complete datagram, not parts. */
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
    *psent += لېږل_شوی;
  }

بهر:
  CURL_TRC_CF(معلومات, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (صحيح)پایله, *psent);
  أرجع پایله;
}

ساكن CURLcode vquic_send_packets(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode پایله;
#که_تعریف_شوی DEBUGBUILD
  /* simulate network blocking/partial writes */
  إذا(qctx->wblock_percent > 0) {
    بینښه حرف c;
    *psent = 0;
    Curl_rand(معلومات, &c, 1);
    إذا(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(معلومات, cf, "vquic_flush() simulate EWOULDBLOCK");
      أرجع CURLE_AGAIN;
    }
  }
#د_که_پای
  إذا(qctx->no_gso && pktlen > gsolen) {
    پایله = send_packet_no_gso(cf, معلومات, qctx, pkt, pktlen, gsolen, psent);
  }
  وإلا {
    پایله = do_sendmsg(cf, معلومات, qctx, pkt, pktlen, gsolen, psent);
  }
  إذا(!پایله)
    qctx->وروستی_io = qctx->وروستی_op;
  أرجع پایله;
}

CURLcode Curl_vquic_flush(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          بنية cf_quic_ctx *qctx)
{
  ثابت بینښه حرف *buf;
  size_t blen, لېږل_شوی;
  CURLcode پایله;
  size_t gsolen;

  طالما(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    إذا(qctx->وېشل_len) {
      gsolen = qctx->وېشل_gsolen;
      إذا(blen > qctx->وېشل_len)
        blen = qctx->وېشل_len;
    }

    إذا(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      پایله = vquic_send_packets(cf, معلومات, qctx, buf, blen, gsolen, &لېږل_شوی);
    }
    وإلا {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      إذا(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        پایله = send_packet_no_gso_cf(cf, معلومات, buf, blen, gsolen, &لېږل_شوی);
      }
      وإلا {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        پایله = Curl_conn_cf_send(cf->بل, معلومات, buf, blen, FALSE, &لېږل_شوی);
      }
    }

    إذا(پایله) {
      إذا(پایله == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, لېږل_شوی);
        إذا(qctx->وېشل_len)
          qctx->وېشل_len -= لېږل_شوی;
      }
      أرجع پایله;
    }
    Curl_bufq_skip(&qctx->sendbuf, لېږل_شوی);
    إذا(qctx->وېشل_len)
      qctx->وېشل_len -= لېږل_شوی;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_vquic_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         بنية cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  أرجع Curl_vquic_flush(cf, معلومات, qctx);
}

CURLcode Curl_vquic_send_لکۍ_وېشل(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية cf_quic_ctx *qctx, size_t gsolen,
                                    size_t لکۍ_len, size_t لکۍ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > لکۍ_len);
  qctx->وېشل_len = Curl_bufq_len(&qctx->sendbuf) - لکۍ_len;
  qctx->وېشل_gsolen = gsolen;
  qctx->gsolen = لکۍ_gsolen;
  CURL_TRC_CF(معلومات, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->وېشل_len, qctx->وېشل_gsolen, لکۍ_len, qctx->gsolen);
  أرجع Curl_vquic_flush(cf, معلومات, qctx);
}

#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_پیغام_X)
ساكن size_t vquic_msghdr_get_udp_gro(بنية msghdr *پیغام)
{
  صحيح gso_اندازه = 0;
#إذا defined(__linux__) && defined(UDP_GRO)
  بنية cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wsign-compare"
#لارښوونه clang diagnostic ignored "-Wcast-align"
#د_که_پای
  لكل(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#لارښوونه clang diagnostic راوباسه
#د_که_پای
    إذا(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      حافظه_نقل(&gso_اندازه, CMSG_DATA(cmsg), حجم(gso_اندازه));

      اكسر;
    }
  }
#د_که_پای /* linux && UDP_GRO */
  (فراغ)پیغام;

  أرجع (size_t)gso_اندازه;
}
#د_که_پای /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_پیغام_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_ماسک)
ساكن uint8_t vquic_msghdr_get_ecn(بنية msghdr *پیغام, صحيح family)
{
  بنية cmsghdr *cmsg;
  بدل(family) {
  حالة AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wsign-compare"
#لارښوونه clang diagnostic ignored "-Wcast-align"
#د_که_پای
    لكل(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#لارښوونه clang diagnostic راوباسه
#د_که_پای
      إذا(cmsg->cmsg_level == IPPROTO_IP &&
#که_تعریف_شوی __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#وإلا
          cmsg->cmsg_type == IP_TOS
#د_که_پای
          && cmsg->cmsg_len) {
        أرجع *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_ماسک;
      }
    }
    اكسر;
  حالة AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wsign-compare"
#لارښوونه clang diagnostic ignored "-Wcast-align"
#د_که_پای
    لكل(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#لارښوونه clang diagnostic راوباسه
#د_که_پای
      إذا(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        بینښه صحيح tos;

        حافظه_نقل(&tos, CMSG_DATA(cmsg), حجم(صحيح));

        أرجع (uint8_t)(tos & IPTOS_ECN_ماسک);
      }
    }
    اكسر;
  }
  أرجع 0;
}
#وإلا
#تعریف_کړه vquic_msghdr_get_ecn(a, b)      0
#د_که_پای /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#که_تعریف_شوی HAVE_SENDMMSG

ساكن CURLcode recvmmsg_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
#إذا defined(__linux__) && defined(UDP_GRO)
#تعریف_کړه MMSG_عدد  16
#تعریف_کړه UDP_GRO_CNT_MAX  64
#تعریف_کړه CMSG_فی_پیغام_اندازه    (2 * CMSG_SPACE(حجم(صحيح)))
#وإلا
#تعریف_کړه MMSG_عدد  64
#تعریف_کړه UDP_GRO_CNT_MAX  1
#تعریف_کړه CMSG_فی_پیغام_اندازه    CMSG_SPACE(حجم(صحيح))
#د_که_پای
#تعریف_کړه پیغام_BUF_اندازه  (UDP_GRO_CNT_MAX * 1500)
  بنية iovec msg_iov[MMSG_عدد];
  بنية mmsghdr mmsg[MMSG_عدد];
  uint8_t پیغام_ctrl[MMSG_عدد * CMSG_فی_پیغام_اندازه];
  بنية sockaddr_storage remote_addr[MMSG_عدد];
  size_t ټول_nread = 0, pkts = 0;
#که_تعریف_شوی CURLVERBOSE
  size_t calls = 0;
#د_که_پای
  صحيح mcount, i, n;
  حرف errstr[STRERROR_LEN];
  CURLcode پایله = CURLE_OK;
  size_t gso_اندازه;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[پیغام_BUF_اندازه] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  پایله = Curl_multi_xfer_sockbuf_borrow(معلومات, MMSG_عدد * پیغام_BUF_اندازه,
                                          &sockbuf);
  إذا(پایله)
    اذهب بهر;
  bufs = (uint8_t (*)[پیغام_BUF_اندازه])sockbuf;

  ټول_nread = 0;
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
      mmsg[i].پیغام_hdr.پیغام_control = &پیغام_ctrl[i * CMSG_فی_پیغام_اندازه];
      mmsg[i].پیغام_hdr.msg_controllen = CMSG_فی_پیغام_اندازه;
    }

    طالما((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, عدم)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "ingress, recvmmsg -> EAGAIN");
        اذهب بهر;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بل, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورټ);
        پایله = CURLE_COULDNT_CONNECT;
        اذهب بهر;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].پیغام_len) {
        ++pkts;
        استمر;
      }
      ټول_nread += mmsg[i].پیغام_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].پیغام_hdr, remote_addr[i].ss_family);
      gso_اندازه = vquic_msghdr_get_udp_gro(&mmsg[i].پیغام_hdr);
      إذا(gso_اندازه == 0)
        gso_اندازه = mmsg[i].پیغام_len;

      پایله = recv_cb(bufs[i], mmsg[i].پیغام_len, gso_اندازه,
                       mmsg[i].پیغام_hdr.پیغام_name,
                       mmsg[i].پیغام_hdr.msg_namelen, ecn, userp);
      إذا(پایله)
        اذهب بهر;
      pkts += (mmsg[i].پیغام_len + gso_اندازه - 1) / gso_اندازه;
    }
  }

بهر:
  إذا(ټول_nread || پایله)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ټول_nread, pkts, calls, (صحيح)پایله);
  Curl_multi_xfer_sockbuf_release(معلومات, sockbuf);
  *pnread = ټول_nread;
  أرجع پایله;
}

#وإلاإذا defined(HAVE_APPLE_پیغام_X)

ساكن CURLcode recvmsg_x_packets(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  فراغ *userp, size_t *pnread)
{
#تعریف_کړه پیغام_X_عدد  64
#تعریف_کړه پیغام_BUF_اندازه  (2048)
#تعریف_کړه CMSG_فی_پیغام_اندازه     CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov[پیغام_X_عدد];
  بنية msghdr_x mmsg[پیغام_X_عدد];
  uint8_t پیغام_ctrl[پیغام_X_عدد * CMSG_فی_پیغام_اندازه];
  بنية sockaddr_storage remote_addr[پیغام_X_عدد];
  size_t ټول_nread = 0, pkts = 0;
#که_تعریف_شوی CURLVERBOSE
  size_t calls = 0;
#د_که_پای
  صحيح mcount, i;
  حرف errstr[STRERROR_LEN];
  CURLcode پایله = CURLE_OK;
  size_t gso_اندازه;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[پیغام_BUF_اندازه] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  پایله = Curl_multi_xfer_sockbuf_borrow(معلومات, پیغام_X_عدد * پیغام_BUF_اندازه,
                                          &sockbuf);
  إذا(پایله)
    اذهب بهر;
  bufs = (uint8_t (*)[پیغام_BUF_اندازه])sockbuf;

  ټول_nread = 0;
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
      mmsg[i].پیغام_control = &پیغام_ctrl[i * CMSG_فی_پیغام_اندازه];
      mmsg[i].msg_controllen = CMSG_فی_پیغام_اندازه;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wdeprecated-declarations"
#د_که_پای
    طالما((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#لارښوونه GCC diagnostic راوباسه
#د_که_پای
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "ingress, recvmsg_x -> EAGAIN");
        اذهب بهر;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بل, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورټ);
        پایله = CURLE_COULDNT_CONNECT;
        اذهب بهر;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].پیغام_datalen) {
        ++pkts;
        استمر;
      }
      ټول_nread += mmsg[i].پیغام_datalen;
      ecn = vquic_msghdr_get_ecn((بنية msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_اندازه = mmsg[i].پیغام_datalen;

      پایله = recv_cb(bufs[i], mmsg[i].پیغام_datalen, gso_اندازه,
                       mmsg[i].پیغام_name,
                       mmsg[i].msg_namelen, ecn, userp);
      إذا(پایله)
        اذهب بهر;
      pkts += (mmsg[i].پیغام_datalen + gso_اندازه - 1) / gso_اندازه;
    }
  }

بهر:
  إذا(ټول_nread || پایله)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                ټول_nread, pkts, calls, (صحيح)پایله);
  Curl_multi_xfer_sockbuf_release(معلومات, sockbuf);
  *pnread = ټول_nread;
  أرجع پایله;
}

#وإلاإذا defined(HAVE_SENDMSG)
ساكن CURLcode recvmsg_packets(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                size_t *pnread)
{
#تعریف_کړه CMSG_فی_پیغام_اندازه    CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov;
  بنية msghdr پیغام;
  uint8_t buf[64 * 1024];
  بنية sockaddr_storage remote_addr;
  size_t ټول_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  حرف errstr[STRERROR_LEN];
  CURLcode پایله = CURLE_OK;
  uint8_t پیغام_ctrl[CMSG_فی_پیغام_اندازه];
  size_t gso_اندازه;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, ټول_nread = 0, calls = 0; pkts < max_pkts;) {
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
    پیغام.msg_controllen = CMSG_فی_پیغام_اندازه;

    طالما((rc = recvmsg(qctx->sockfd, &پیغام, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rc, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        اذهب بهر;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بل, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورټ);
        پایله = CURLE_COULDNT_CONNECT;
        اذهب بهر;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
    }

    ټول_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread) {
      ++pkts;
      استمر;
    }

    ecn = vquic_msghdr_get_ecn(&پیغام, remote_addr.ss_family);
    gso_اندازه = vquic_msghdr_get_udp_gro(&پیغام);
    إذا(gso_اندازه == 0)
      gso_اندازه = nread;

    پایله = recv_cb(buf, nread, gso_اندازه,
                     پیغام.پیغام_name, پیغام.msg_namelen, ecn, userp);
    إذا(پایله)
      اذهب بهر;
    pkts += (nread + gso_اندازه - 1) / gso_اندازه;
  }

بهر:
  إذا(ټول_nread || پایله)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ټول_nread, pkts, calls, (صحيح)پایله);
  *pnread = ټول_nread;
  أرجع پایله;
}

#وإلا /* HAVE_SENDMMSG || HAVE_SENDMSG */
ساكن CURLcode recvfrom_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  صحيح bufsize = (صحيح)حجم(buf);
  بنية sockaddr_storage remote_addr;
  socklen_t remote_addrlen = حجم(remote_addr);
  size_t ټول_nread, pkts, calls = 0, nread;
  ssize_t rv;
  حرف errstr[STRERROR_LEN];
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, ټول_nread = 0; pkts < max_pkts;) {
    طالما((rv = recvfrom(qctx->sockfd, (حرف *)buf, bufsize, 0,
                         (بنية sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "ingress, recvfrom -> EAGAIN");
        اذهب بهر;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بل, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورټ);
        پایله = CURLE_COULDNT_CONNECT;
        اذهب بهر;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread)
      استمر;

    ټول_nread += nread;
    پایله = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    إذا(پایله)
      اذهب بهر;
  }

بهر:
  إذا(ټول_nread || پایله)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                ټول_nread, pkts, calls, (صحيح)پایله);
  *pnread = ټول_nread;
  أرجع پایله;
}
#د_که_پای /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp)
{
  CURLcode پایله;
  size_t nread = 0;
#که_تعریف_شوی HAVE_SENDMMSG
  پایله = recvmmsg_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp, &nread);
#وإلاإذا defined(HAVE_APPLE_پیغام_X)
  پایله = recvmsg_x_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp,
                             &nread);
#وإلاإذا defined(HAVE_SENDMSG)
  پایله = recvmsg_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp, &nread);
#وإلا
  پایله = recvfrom_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp, &nread);
#د_که_پای
  إذا(!پایله) {
    /* EAGAIN is no error, only count actual packets as a reply */
    إذا(!qctx->got_لومړی_byte && nread) {
      qctx->got_لومړی_byte = TRUE;
      qctx->لومړی_byte_at = qctx->وروستی_op;
    }
    qctx->وروستی_io = qctx->وروستی_op;
  }
  أرجع پایله;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(بنية Curl_easy *معلومات,
                      بینښه حرف *scid,
                      size_t scidlen,
                      صحيح *qlogfdp)
{
  حرف *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  إذا(qlog_dir) {
    بنية dynbuf fname;
    CURLcode پایله;
    بینښه صحيح i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    پایله = curlx_dyn_جمع_کړه(&fname, qlog_dir);
    إذا(!پایله)
      پایله = curlx_dyn_جمع_کړه(&fname, "/");
    لكل(i = 0; (i < scidlen) && !پایله; i++) {
      حرف hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      پایله = curlx_dyn_جمع_کړه(&fname, hex);
    }
    إذا(!پایله)
      پایله = curlx_dyn_جمع_کړه(&fname, ".sqlog");

    إذا(!پایله) {
      صحيح qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_دوګونی,
#که_تعریف_شوی _WIN32
                              معلومات->سېټ.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                              (mode_t)معلومات->سېټ.new_file_perms
#د_که_پای
                              );
      إذا(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    إذا(پایله)
      أرجع پایله;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_cf_quic_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                   بنية Curl_easy *معلومات,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = عدم;
  CURLcode پایله;

  (فراغ)معلومات; /* not used in all cases and compilers are stupid */
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  پایله = Curl_cf_ngtcp2_ورننباسه_after(cf_at, origin, peer, &cf);
#وإلاإذا defined(USE_QUICHE)
  پایله = Curl_cf_quiche_ورننباسه_after(cf_at, origin, peer, &cf);
#وإلا
  (فراغ)cf)
  (فراغ)cf_at;
  (فراغ)origin;
  (فراغ)peer;
  پایله = CURLE_NOT_BUILT_دننه;
#د_که_پای

#إذا defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  إذا(!پایله && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    پایله = Curl_conn_dns_جمع_کړه_https_resolve(معلومات, cf->conn,
                                             cf->sockindex, origin);
  }
#د_که_پای /* USE_HTTPSRR && USE_ECH */
  أرجع پایله;
}

CURLcode Curl_cf_quic_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *معلومات,
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
  أرجع Curl_cf_ngtcp2_create(pcf, معلومات, origin, peer, conn, addr);
#وإلاإذا defined(USE_QUICHE)
  أرجع Curl_cf_quiche_create(pcf, معلومات, origin, peer, conn, addr);
#وإلا
  *pcf = عدم;
  (فراغ)معلومات;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)conn;
  (فراغ)addr;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
}

#إذا !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  أرجع Curl_cf_ngtcp2_proxy_ورننباسه_after(cf_at, معلومات, origin, peer,
                                           tunnel_peer, tunnel_transport);
#وإلا
  (فراغ)cf_at;
  (فراغ)معلومات;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
}

CURLcode Curl_cf_h3_proxy_create(بنية Curl_cfilter **pcf,
                                 بنية Curl_easy *معلومات,
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
  أرجع Curl_cf_ngtcp2_proxy_create(pcf, معلومات, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#وإلا
  *pcf = عدم;
  (فراغ)معلومات;
  (فراغ)conn;
  (فراغ)addr;
  (فراغ)peer;
  (فراغ)transport_peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
}

#د_که_پای /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *معلومات,
                             بنية connectdata *conn)
{
  إذا(Curl_conn_get_لومړی_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(معلومات, "HTTP/3 cannot be used over UNIX domain sockets");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  إذا(!(معلومات->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(معلومات, "HTTP/3 requested for non-HTTPS URL");
    أرجع CURLE_URL_MALFORMAT;
  }
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer) {
    failf(معلومات, "HTTP/3 is not supported over a SOCKS proxy");
    أرجع CURLE_URL_MALFORMAT;
  }
#وإلا
  (فراغ)conn;
#د_که_پای

  أرجع CURLE_OK;
}

#که_تعریف_شوی CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_کوډ)
{
  إذا(error_کوډ <= UINT_MAX) {
    بدل((بینښه صحيح)error_کوډ) {
    حالة CURL_H3_ERR_NO_ERROR:
      أرجع "NO_ERROR";
    حالة CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      أرجع "GENERAL_PROTOCOL_ERROR";
    حالة CURL_H3_ERR_دننی_ERROR:
      أرجع "INTERNAL_ERROR";
    حالة CURL_H3_ERR_بهیر_CREATION_ERROR:
      أرجع "STREAM_CREATION_ERROR";
    حالة CURL_H3_ERR_CLOSED_CRITICAL_بهیر:
      أرجع "CLOSED_CRITICAL_STREAM";
    حالة CURL_H3_ERR_چوکاټ_UNEXPECTED:
      أرجع "FRAME_UNEXPECTED";
    حالة CURL_H3_ERR_چوکاټ_ERROR:
      أرجع "FRAME_ERROR";
    حالة CURL_H3_ERR_EXCESSIVE_بارول:
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
    حالة CURL_H3_ERR_پیغام_ERROR:
      أرجع "MESSAGE_ERROR";
    حالة CURL_H3_ERR_CONNECT_ERROR:
      أرجع "CONNECT_ERROR";
    حالة CURL_H3_ERR_نسخه_FALLBACK:
      أرجع "VERSION_FALLBACK";
    افتراضي:
      اكسر;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  إذا((error_کوډ >= 0x21) && !((error_کوډ - 0x21) % 0x1f))
    أرجع "NO_ERROR";
  أرجع "unknown";
}
#د_که_پای /* CURLVERBOSE */

#إذا defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ساكن فراغ *vquic_ngtcp2_malloc(size_t اندازه, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_cmalloc(اندازه);
}

ساكن فراغ vquic_ngtcp2_free(فراغ *ptr, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  Curl_cfree(ptr);
}

ساكن فراغ *vquic_ngtcp2_calloc(size_t nmemb, size_t اندازه, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_ccalloc(nmemb, اندازه);
}

ساكن فراغ *vquic_ngtcp2_realloc(فراغ *ptr, size_t اندازه, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_crealloc(ptr, اندازه);
}

#که_تعریف_شوی USE_NGTCP2
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
#د_که_پای

#که_تعریف_شوی USE_NGHTTP3
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
#د_که_پای

#د_که_پای /* USE_NGTCP2 || USE_NGHTTP3 */

#وإلا /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *معلومات,
                             بنية connectdata *conn)
{
  (فراغ)معلومات;
  (فراغ)conn;
  DEBUGF(infof(معلومات, "QUIC is not supported in this build"));
  أرجع CURLE_NOT_BUILT_دننه;
}

#د_که_پای /* !CURL_DISABLE_HTTP && USE_HTTP3 */
