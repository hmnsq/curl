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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETINET_IP_H
#ضمن <netinet/ip.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NGHTTP3
#ضمن <nghttp3/nghttp3.h>
#جيڪڏهن_پورو

#إذا defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ضمن <sys/syscall.h>
#إذا defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#وصف_ڪريو HAVE_APPLE_نياپو_X
#جيڪڏهن_پورو
#جيڪڏهن_پورو

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


#وصف_ڪريو NW_CHUNK_سائيز     (64 * 1024)
#وصف_ڪريو NW_SEND_CHUNKS    1

#جيڪڏهن_وصف_ٿيل HAVE_APPLE_نياپو_X

/* this is `struct msghdr` with an additional field at the end */
بنية msghdr_x {
  فراغ *نياپو_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  بنية iovec *msg_iov;    /* scatter/gather array */
  صحيح نياپو_iovlen;           /* # elements in msg_iov */
  فراغ *نياپو_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  صحيح نياپو_flags;            /* flags on received message */
  size_t نياپو_datalen;       /* byte length of buffer in msg_iov */
};
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#جيڪڏهن_وصف_ٿيل HAVE_APPLE_نياپو_X
#وصف_ڪريو VQUIC_SEND_طريقو   "sendmsg_x"
#وإلاإذا defined(HAVE_SENDMSG)
#وصف_ڪريو VQUIC_SEND_طريقو   "sendmsg"
#وإلا
#وصف_ڪريو VQUIC_SEND_طريقو   "send"
#جيڪڏهن_پورو
#جيڪڏهن_پورو

صحيح Curl_vquic_init(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_init())
    أرجع 0;
#جيڪڏهن_پورو

  أرجع 1;
}

فراغ Curl_vquic_cleanup(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_نسخو_انگ >= 0x011800)
  ngtcp2_crypto_ossl_free();
#جيڪڏهن_پورو
}

فراغ Curl_quic_ver(حرف *p, size_t len)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#وإلاإذا defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#جيڪڏهن_پورو
}

CURLcode Curl_vquic_ctx_init(بنية Curl_easy *معلومات,
                             بنية cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_سائيز, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندي);
#إذا defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#وإلاإذا defined(HAVE_APPLE_نياپو_X)
  qctx->no_gso = FALSE;
#وإلا
  qctx->no_gso = TRUE;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_QUIC_WBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 100))
        qctx->wblock_percent = (صحيح)l;
    }
  }
#جيڪڏهن_پورو
  Curl_vquic_ctx_سيٽ_وقت(qctx, Curl_pgrs_now(معلومات));

  أرجع CURLE_OK;
}

فراغ Curl_vquic_ctx_free(بنية cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

فراغ Curl_vquic_ctx_سيٽ_وقت(بنية cf_quic_ctx *qctx,
                             ثابت بنية curltime *pnow)
{
  qctx->آخري_op = *pnow;
}

فراغ Curl_vquic_ctx_update_وقت(بنية cf_quic_ctx *qctx,
                                ثابت بنية curltime *pnow)
{
  qctx->آخري_op = *pnow;
}

#جيڪڏهن_وصف_ٿيل HAVE_APPLE_نياپو_X
ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#وصف_ڪريو نياپو_X_SNUM  64
  بنية iovec msg_iov[نياپو_X_SNUM];
  بنية msghdr_x mmsg[نياپو_X_SNUM];
  حرف errstr[STRERROR_LEN];
  size_t n, i = 0, موڪليل = 0;
  صحيح rc;
  CURLcode نتيجو = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  إذا(!pktlen)
    أرجع CURLE_OK;
  إذا(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  طالما(i < n) {
    size_t j, batch = CURLMIN(n - i, نياپو_X_SNUM), pkts_موڪليل = 0;

    لكل(j = 0; j < batch; ++j) {
      ثابت size_t وٿي = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + وٿي);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - وٿي);
      memset(&mmsg[j], 0, حجم(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].نياپو_iovlen = 1;
      mmsg[j].نياپو_datalen = msg_iov[j].iov_len;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wdeprecated-declarations"
#جيڪڏهن_پورو
    طالما((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

    إذا(rc < 0) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "egress, sendmsg_x -> EAGAIN");
        نتيجو = موڪليل ? CURLE_OK : CURLE_AGAIN;
        اذهب ٻاهريون;
      }
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
        failf(معلومات, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        نتيجو = CURLE_SEND_ERROR;
        اذهب ٻاهريون;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_موڪليل = batch;
    }
    وإلا
      pkts_موڪليل = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    إذا(!pkts_موڪليل) {  /* no packets of the current batch were sent */
      نتيجو = موڪليل ? CURLE_OK : CURLE_AGAIN;
      اذهب ٻاهريون;
    }
    i += pkts_موڪليل; /* Some have been sent */
    لكل(j = 0; j < pkts_موڪليل; ++j)
      موڪليل += msg_iov[j].iov_len;
    إذا(pkts_موڪليل < batch)
      اذهب ٻاهريون; /* but not all of them */
  }

ٻاهريون:
  *psent = موڪليل;
  إذا(موڪليل || نتيجو)
    CURL_TRC_CF(معلومات, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                موڪليل, gsolen, n, calls, (صحيح)نتيجو);
  أرجع نتيجو;
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
  CURLcode نتيجو = CURLE_OK;
#جيڪڏهن_وصف_ٿيل HAVE_SENDMSG
  بنية iovec msg_iov;
  بنية msghdr نياپو = { 0 };
  ssize_t rv;
#إذا defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t نياپو_ctrl[32];
  بنية cmsghdr *cm;
#جيڪڏهن_پورو

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  نياپو.msg_iov = &msg_iov;
  نياپو.نياپو_iovlen = 1;

#إذا defined(__linux__) && defined(UDP_SEGMENT)
  إذا(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(نياپو_ctrl, 0, حجم(نياپو_ctrl));
    نياپو.نياپو_control = نياپو_ctrl;
    DEBUGASSERT(حجم(نياپو_ctrl) >= CMSG_SPACE(حجم(صحيح)));
    نياپو.msg_controllen = CMSG_SPACE(حجم(صحيح));
    cm = CMSG_FIRSTHDR(&نياپو);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(حجم(uint16_t));
    *(uint16_t *)(فراغ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#جيڪڏهن_پورو

  طالما((rv = sendmsg(qctx->sockfd, &نياپو, 0)) == -1 && SOCKERRNO == SOCKEINTR)
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
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }
  وإلا إذا(pktlen != *psent) {
    failf(معلومات, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
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
      نتيجو = CURLE_AGAIN;
      اذهب ٻاهريون;
    }
    وإلا {
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        failf(معلومات, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        نتيجو = CURLE_SEND_ERROR;
        اذهب ٻاهريون;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#جيڪڏهن_پورو
  (فراغ)cf;

ٻاهريون:
  CURL_TRC_CF(معلومات, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_طريقو, pktlen, gsolen, (صحيح)نتيجو, *psent);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *پڇاڙي = pkt + pktlen;
  size_t موڪليل, len;
  CURLcode نتيجو = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  لكل(p = pkt; p < پڇاڙي; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(پڇاڙي - p));
    نتيجو = do_sendmsg(cf, معلومات, qctx, p, len, len, &موڪليل);
    إذا(نتيجو)
      اذهب ٻاهريون;
    *psent += موڪليل;
    VERBOSE(++calls);
  }
ٻاهريون:
  CURL_TRC_CF(معلومات, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_طريقو, pktlen, gsolen, calls, (صحيح)نتيجو, *psent);
  أرجع نتيجو;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ساكن CURLcode send_packet_no_gso_cf(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      ثابت uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *پڇاڙي = pkt + pktlen;
  size_t موڪليل, len;
  CURLcode نتيجو = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  لكل(p = pkt; p < پڇاڙي; p += len) {
    len = CURLMIN(gsolen, (size_t)(پڇاڙي - p));
    نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, p, len, FALSE, &موڪليل);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    إذا(نتيجو)
      اذهب ٻاهريون;

    إذا(موڪليل != len) {
      /* We can only send the complete datagram, not parts. */
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
    *psent += موڪليل;
  }

ٻاهريون:
  CURL_TRC_CF(معلومات, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (صحيح)نتيجو, *psent);
  أرجع نتيجو;
}

ساكن CURLcode vquic_send_packets(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode نتيجو;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  /* simulate network blocking/partial writes */
  إذا(qctx->wblock_percent > 0) {
    بنانشان حرف c;
    *psent = 0;
    Curl_rand(معلومات, &c, 1);
    إذا(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(معلومات, cf, "vquic_flush() simulate EWOULDBLOCK");
      أرجع CURLE_AGAIN;
    }
  }
#جيڪڏهن_پورو
  إذا(qctx->no_gso && pktlen > gsolen) {
    نتيجو = send_packet_no_gso(cf, معلومات, qctx, pkt, pktlen, gsolen, psent);
  }
  وإلا {
    نتيجو = do_sendmsg(cf, معلومات, qctx, pkt, pktlen, gsolen, psent);
  }
  إذا(!نتيجو)
    qctx->آخري_io = qctx->آخري_op;
  أرجع نتيجو;
}

CURLcode Curl_vquic_flush(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          بنية cf_quic_ctx *qctx)
{
  ثابت بنانشان حرف *buf;
  size_t blen, موڪليل;
  CURLcode نتيجو;
  size_t gsolen;

  طالما(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    إذا(qctx->ورهايو_len) {
      gsolen = qctx->ورهايو_gsolen;
      إذا(blen > qctx->ورهايو_len)
        blen = qctx->ورهايو_len;
    }

    إذا(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      نتيجو = vquic_send_packets(cf, معلومات, qctx, buf, blen, gsolen, &موڪليل);
    }
    وإلا {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      إذا(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        نتيجو = send_packet_no_gso_cf(cf, معلومات, buf, blen, gsolen, &موڪليل);
      }
      وإلا {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, buf, blen, FALSE, &موڪليل);
      }
    }

    إذا(نتيجو) {
      إذا(نتيجو == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, موڪليل);
        إذا(qctx->ورهايو_len)
          qctx->ورهايو_len -= موڪليل;
      }
      أرجع نتيجو;
    }
    Curl_bufq_skip(&qctx->sendbuf, موڪليل);
    إذا(qctx->ورهايو_len)
      qctx->ورهايو_len -= موڪليل;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_vquic_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         بنية cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  أرجع Curl_vquic_flush(cf, معلومات, qctx);
}

CURLcode Curl_vquic_send_پڇ_ورهايو(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية cf_quic_ctx *qctx, size_t gsolen,
                                    size_t پڇ_len, size_t پڇ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > پڇ_len);
  qctx->ورهايو_len = Curl_bufq_len(&qctx->sendbuf) - پڇ_len;
  qctx->ورهايو_gsolen = gsolen;
  qctx->gsolen = پڇ_gsolen;
  CURL_TRC_CF(معلومات, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->ورهايو_len, qctx->ورهايو_gsolen, پڇ_len, qctx->gsolen);
  أرجع Curl_vquic_flush(cf, معلومات, qctx);
}

#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_نياپو_X)
ساكن size_t vquic_msghdr_get_udp_gro(بنية msghdr *نياپو)
{
  صحيح gso_سائيز = 0;
#إذا defined(__linux__) && defined(UDP_GRO)
  بنية cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wsign-compare"
#هدايت clang diagnostic ignored "-Wcast-align"
#جيڪڏهن_پورو
  لكل(cmsg = CMSG_FIRSTHDR(نياپو); cmsg; cmsg = CMSG_NXTHDR(نياپو, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
    إذا(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      يادگيري_نقل(&gso_سائيز, CMSG_DATA(cmsg), حجم(gso_سائيز));

      اكسر;
    }
  }
#جيڪڏهن_پورو /* linux && UDP_GRO */
  (فراغ)نياپو;

  أرجع (size_t)gso_سائيز;
}
#جيڪڏهن_پورو /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_نياپو_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_نقاب)
ساكن uint8_t vquic_msghdr_get_ecn(بنية msghdr *نياپو, صحيح family)
{
  بنية cmsghdr *cmsg;
  بدل(family) {
  حالة AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wsign-compare"
#هدايت clang diagnostic ignored "-Wcast-align"
#جيڪڏهن_پورو
    لكل(cmsg = CMSG_FIRSTHDR(نياپو); cmsg; cmsg = CMSG_NXTHDR(نياپو, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
      إذا(cmsg->cmsg_level == IPPROTO_IP &&
#جيڪڏهن_وصف_ٿيل __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#وإلا
          cmsg->cmsg_type == IP_TOS
#جيڪڏهن_پورو
          && cmsg->cmsg_len) {
        أرجع *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_نقاب;
      }
    }
    اكسر;
  حالة AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wsign-compare"
#هدايت clang diagnostic ignored "-Wcast-align"
#جيڪڏهن_پورو
    لكل(cmsg = CMSG_FIRSTHDR(نياپو); cmsg; cmsg = CMSG_NXTHDR(نياپو, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
      إذا(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        بنانشان صحيح tos;

        يادگيري_نقل(&tos, CMSG_DATA(cmsg), حجم(صحيح));

        أرجع (uint8_t)(tos & IPTOS_ECN_نقاب);
      }
    }
    اكسر;
  }
  أرجع 0;
}
#وإلا
#وصف_ڪريو vquic_msghdr_get_ecn(a, b)      0
#جيڪڏهن_پورو /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#جيڪڏهن_وصف_ٿيل HAVE_SENDMMSG

ساكن CURLcode recvmmsg_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
#إذا defined(__linux__) && defined(UDP_GRO)
#وصف_ڪريو MMSG_انگ  16
#وصف_ڪريو UDP_GRO_CNT_MAX  64
#وصف_ڪريو CMSG_في_نياپو_سائيز    (2 * CMSG_SPACE(حجم(صحيح)))
#وإلا
#وصف_ڪريو MMSG_انگ  64
#وصف_ڪريو UDP_GRO_CNT_MAX  1
#وصف_ڪريو CMSG_في_نياپو_سائيز    CMSG_SPACE(حجم(صحيح))
#جيڪڏهن_پورو
#وصف_ڪريو نياپو_BUF_سائيز  (UDP_GRO_CNT_MAX * 1500)
  بنية iovec msg_iov[MMSG_انگ];
  بنية mmsghdr mmsg[MMSG_انگ];
  uint8_t نياپو_ctrl[MMSG_انگ * CMSG_في_نياپو_سائيز];
  بنية sockaddr_storage remote_addr[MMSG_انگ];
  size_t ڪل_nread = 0, pkts = 0;
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  size_t calls = 0;
#جيڪڏهن_پورو
  صحيح mcount, i, n;
  حرف errstr[STRERROR_LEN];
  CURLcode نتيجو = CURLE_OK;
  size_t gso_سائيز;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[نياپو_BUF_سائيز] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  نتيجو = Curl_multi_xfer_sockbuf_borrow(معلومات, MMSG_انگ * نياپو_BUF_سائيز,
                                          &sockbuf);
  إذا(نتيجو)
    اذهب ٻاهريون;
  bufs = (uint8_t (*)[نياپو_BUF_سائيز])sockbuf;

  ڪل_nread = 0;
  طالما(pkts < max_pkts) {
    n = (صحيح)CURLMIN(CURLMIN(MMSG_انگ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    لكل(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].نياپو_hdr.msg_iov = &msg_iov[i];
      mmsg[i].نياپو_hdr.نياپو_iovlen = 1;
      mmsg[i].نياپو_hdr.نياپو_name = &remote_addr[i];
      mmsg[i].نياپو_hdr.msg_namelen = حجم(remote_addr[i]);
      mmsg[i].نياپو_hdr.نياپو_control = &نياپو_ctrl[i * CMSG_في_نياپو_سائيز];
      mmsg[i].نياپو_hdr.msg_controllen = CMSG_في_نياپو_سائيز;
    }

    طالما((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, عدم)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "ingress, recvmmsg -> EAGAIN");
        اذهب ٻاهريون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->ايندڙ, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٽ);
        نتيجو = CURLE_COULDNT_CONNECT;
        اذهب ٻاهريون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      نتيجو = CURLE_RECV_ERROR;
      اذهب ٻاهريون;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].نياپو_len) {
        ++pkts;
        استمر;
      }
      ڪل_nread += mmsg[i].نياپو_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].نياپو_hdr, remote_addr[i].ss_family);
      gso_سائيز = vquic_msghdr_get_udp_gro(&mmsg[i].نياپو_hdr);
      إذا(gso_سائيز == 0)
        gso_سائيز = mmsg[i].نياپو_len;

      نتيجو = recv_cb(bufs[i], mmsg[i].نياپو_len, gso_سائيز,
                       mmsg[i].نياپو_hdr.نياپو_name,
                       mmsg[i].نياپو_hdr.msg_namelen, ecn, userp);
      إذا(نتيجو)
        اذهب ٻاهريون;
      pkts += (mmsg[i].نياپو_len + gso_سائيز - 1) / gso_سائيز;
    }
  }

ٻاهريون:
  إذا(ڪل_nread || نتيجو)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ڪل_nread, pkts, calls, (صحيح)نتيجو);
  Curl_multi_xfer_sockbuf_release(معلومات, sockbuf);
  *pnread = ڪل_nread;
  أرجع نتيجو;
}

#وإلاإذا defined(HAVE_APPLE_نياپو_X)

ساكن CURLcode recvmsg_x_packets(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  فراغ *userp, size_t *pnread)
{
#وصف_ڪريو نياپو_X_انگ  64
#وصف_ڪريو نياپو_BUF_سائيز  (2048)
#وصف_ڪريو CMSG_في_نياپو_سائيز     CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov[نياپو_X_انگ];
  بنية msghdr_x mmsg[نياپو_X_انگ];
  uint8_t نياپو_ctrl[نياپو_X_انگ * CMSG_في_نياپو_سائيز];
  بنية sockaddr_storage remote_addr[نياپو_X_انگ];
  size_t ڪل_nread = 0, pkts = 0;
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  size_t calls = 0;
#جيڪڏهن_پورو
  صحيح mcount, i;
  حرف errstr[STRERROR_LEN];
  CURLcode نتيجو = CURLE_OK;
  size_t gso_سائيز;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[نياپو_BUF_سائيز] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  نتيجو = Curl_multi_xfer_sockbuf_borrow(معلومات, نياپو_X_انگ * نياپو_BUF_سائيز,
                                          &sockbuf);
  إذا(نتيجو)
    اذهب ٻاهريون;
  bufs = (uint8_t (*)[نياپو_BUF_سائيز])sockbuf;

  ڪل_nread = 0;
  طالما(pkts < max_pkts) {
    صحيح n = (صحيح)CURLMIN(CURLMIN(نياپو_X_انگ, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    لكل(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].نياپو_iovlen = 1;
      mmsg[i].نياپو_name = &remote_addr[i];
      mmsg[i].msg_namelen = حجم(remote_addr[i]);
      mmsg[i].نياپو_control = &نياپو_ctrl[i * CMSG_في_نياپو_سائيز];
      mmsg[i].msg_controllen = CMSG_في_نياپو_سائيز;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wdeprecated-declarations"
#جيڪڏهن_پورو
    طالما((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "ingress, recvmsg_x -> EAGAIN");
        اذهب ٻاهريون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->ايندڙ, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٽ);
        نتيجو = CURLE_COULDNT_CONNECT;
        اذهب ٻاهريون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      نتيجو = CURLE_RECV_ERROR;
      اذهب ٻاهريون;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].نياپو_datalen) {
        ++pkts;
        استمر;
      }
      ڪل_nread += mmsg[i].نياپو_datalen;
      ecn = vquic_msghdr_get_ecn((بنية msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_سائيز = mmsg[i].نياپو_datalen;

      نتيجو = recv_cb(bufs[i], mmsg[i].نياپو_datalen, gso_سائيز,
                       mmsg[i].نياپو_name,
                       mmsg[i].msg_namelen, ecn, userp);
      إذا(نتيجو)
        اذهب ٻاهريون;
      pkts += (mmsg[i].نياپو_datalen + gso_سائيز - 1) / gso_سائيز;
    }
  }

ٻاهريون:
  إذا(ڪل_nread || نتيجو)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                ڪل_nread, pkts, calls, (صحيح)نتيجو);
  Curl_multi_xfer_sockbuf_release(معلومات, sockbuf);
  *pnread = ڪل_nread;
  أرجع نتيجو;
}

#وإلاإذا defined(HAVE_SENDMSG)
ساكن CURLcode recvmsg_packets(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                size_t *pnread)
{
#وصف_ڪريو CMSG_في_نياپو_سائيز    CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov;
  بنية msghdr نياپو;
  uint8_t buf[64 * 1024];
  بنية sockaddr_storage remote_addr;
  size_t ڪل_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  حرف errstr[STRERROR_LEN];
  CURLcode نتيجو = CURLE_OK;
  uint8_t نياپو_ctrl[CMSG_في_نياپو_سائيز];
  size_t gso_سائيز;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, ڪل_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&نياپو, 0, حجم(نياپو));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = حجم(buf);
    نياپو.msg_iov = &msg_iov;
    نياپو.نياپو_iovlen = 1;
    نياپو.نياپو_control = نياپو_ctrl;
    نياپو.نياپو_name = &remote_addr;
    نياپو.msg_namelen = حجم(remote_addr);
    نياپو.msg_controllen = CMSG_في_نياپو_سائيز;

    طالما((rc = recvmsg(qctx->sockfd, &نياپو, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rc, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        اذهب ٻاهريون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->ايندڙ, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٽ);
        نتيجو = CURLE_COULDNT_CONNECT;
        اذهب ٻاهريون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      نتيجو = CURLE_RECV_ERROR;
      اذهب ٻاهريون;
    }

    ڪل_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread) {
      ++pkts;
      استمر;
    }

    ecn = vquic_msghdr_get_ecn(&نياپو, remote_addr.ss_family);
    gso_سائيز = vquic_msghdr_get_udp_gro(&نياپو);
    إذا(gso_سائيز == 0)
      gso_سائيز = nread;

    نتيجو = recv_cb(buf, nread, gso_سائيز,
                     نياپو.نياپو_name, نياپو.msg_namelen, ecn, userp);
    إذا(نتيجو)
      اذهب ٻاهريون;
    pkts += (nread + gso_سائيز - 1) / gso_سائيز;
  }

ٻاهريون:
  إذا(ڪل_nread || نتيجو)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                ڪل_nread, pkts, calls, (صحيح)نتيجو);
  *pnread = ڪل_nread;
  أرجع نتيجو;
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
  size_t ڪل_nread, pkts, calls = 0, nread;
  ssize_t rv;
  حرف errstr[STRERROR_LEN];
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, ڪل_nread = 0; pkts < max_pkts;) {
    طالما((rv = recvfrom(qctx->sockfd, (حرف *)buf, bufsize, 0,
                         (بنية sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(معلومات, cf, "ingress, recvfrom -> EAGAIN");
        اذهب ٻاهريون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->ايندڙ, معلومات, عدم, عدم, &ip))
          failf(معلومات, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٽ);
        نتيجو = CURLE_COULDNT_CONNECT;
        اذهب ٻاهريون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(معلومات, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      نتيجو = CURLE_RECV_ERROR;
      اذهب ٻاهريون;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread)
      استمر;

    ڪل_nread += nread;
    نتيجو = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

ٻاهريون:
  إذا(ڪل_nread || نتيجو)
    CURL_TRC_CF(معلومات, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                ڪل_nread, pkts, calls, (صحيح)نتيجو);
  *pnread = ڪل_nread;
  أرجع نتيجو;
}
#جيڪڏهن_پورو /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp)
{
  CURLcode نتيجو;
  size_t nread = 0;
#جيڪڏهن_وصف_ٿيل HAVE_SENDMMSG
  نتيجو = recvmmsg_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp, &nread);
#وإلاإذا defined(HAVE_APPLE_نياپو_X)
  نتيجو = recvmsg_x_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp,
                             &nread);
#وإلاإذا defined(HAVE_SENDMSG)
  نتيجو = recvmsg_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp, &nread);
#وإلا
  نتيجو = recvfrom_packets(cf, معلومات, qctx, max_pkts, recv_cb, userp, &nread);
#جيڪڏهن_پورو
  إذا(!نتيجو) {
    /* EAGAIN is no error, only count actual packets as a reply */
    إذا(!qctx->got_پهريون_byte && nread) {
      qctx->got_پهريون_byte = TRUE;
      qctx->پهريون_byte_at = qctx->آخري_op;
    }
    qctx->آخري_io = qctx->آخري_op;
  }
  أرجع نتيجو;
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
                      بنانشان حرف *scid,
                      size_t scidlen,
                      صحيح *qlogfdp)
{
  حرف *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  إذا(qlog_dir) {
    بنية dynbuf fname;
    CURLcode نتيجو;
    بنانشان صحيح i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    نتيجو = curlx_dyn_جوڙيو(&fname, qlog_dir);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_جوڙيو(&fname, "/");
    لكل(i = 0; (i < scidlen) && !نتيجو; i++) {
      حرف hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      نتيجو = curlx_dyn_جوڙيو(&fname, hex);
    }
    إذا(!نتيجو)
      نتيجو = curlx_dyn_جوڙيو(&fname, ".sqlog");

    إذا(!نتيجو) {
      صحيح qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ٻٽو,
#جيڪڏهن_وصف_ٿيل _WIN32
                              معلومات->سيٽ.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                              (mode_t)معلومات->سيٽ.new_file_perms
#جيڪڏهن_پورو
                              );
      إذا(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_cf_quic_وجهو_after(بنية Curl_cfilter *cf_at,
                                   بنية Curl_easy *معلومات,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتيجو;

  (فراغ)معلومات; /* not used in all cases and compilers are stupid */
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  نتيجو = Curl_cf_ngtcp2_وجهو_after(cf_at, origin, peer, &cf);
#وإلاإذا defined(USE_QUICHE)
  نتيجو = Curl_cf_quiche_وجهو_after(cf_at, origin, peer, &cf);
#وإلا
  (فراغ)cf)
  (فراغ)cf_at;
  (فراغ)origin;
  (فراغ)peer;
  نتيجو = CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو

#إذا defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  إذا(!نتيجو && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    نتيجو = Curl_conn_dns_جوڙيو_https_resolve(معلومات, cf->conn,
                                             cf->sockindex, origin);
  }
#جيڪڏهن_پورو /* USE_HTTPSRR && USE_ECH */
  أرجع نتيجو;
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
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

#إذا !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  أرجع Curl_cf_ngtcp2_proxy_وجهو_after(cf_at, معلومات, origin, peer,
                                           tunnel_peer, tunnel_transport);
#وإلا
  (فراغ)cf_at;
  (فراغ)معلومات;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
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
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *معلومات,
                             بنية connectdata *conn)
{
  إذا(Curl_conn_get_پهريون_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(معلومات, "HTTP/3 cannot be used over UNIX domain sockets");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  إذا(!(معلومات->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(معلومات, "HTTP/3 requested for non-HTTPS URL");
    أرجع CURLE_URL_MALFORMAT;
  }
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer) {
    failf(معلومات, "HTTP/3 is not supported over a SOCKS proxy");
    أرجع CURLE_URL_MALFORMAT;
  }
#وإلا
  (فراغ)conn;
#جيڪڏهن_پورو

  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_ڪوڊ)
{
  إذا(error_ڪوڊ <= UINT_MAX) {
    بدل((بنانشان صحيح)error_ڪوڊ) {
    حالة CURL_H3_ERR_NO_ERROR:
      أرجع "NO_ERROR";
    حالة CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      أرجع "GENERAL_PROTOCOL_ERROR";
    حالة CURL_H3_ERR_اندروني_ERROR:
      أرجع "INTERNAL_ERROR";
    حالة CURL_H3_ERR_وهڪرو_CREATION_ERROR:
      أرجع "STREAM_CREATION_ERROR";
    حالة CURL_H3_ERR_CLOSED_CRITICAL_وهڪرو:
      أرجع "CLOSED_CRITICAL_STREAM";
    حالة CURL_H3_ERR_چوکٺو_UNEXPECTED:
      أرجع "FRAME_UNEXPECTED";
    حالة CURL_H3_ERR_چوکٺو_ERROR:
      أرجع "FRAME_ERROR";
    حالة CURL_H3_ERR_EXCESSIVE_ڀريو:
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
    حالة CURL_H3_ERR_پيغام_ERROR:
      أرجع "MESSAGE_ERROR";
    حالة CURL_H3_ERR_CONNECT_ERROR:
      أرجع "CONNECT_ERROR";
    حالة CURL_H3_ERR_نسخو_FALLBACK:
      أرجع "VERSION_FALLBACK";
    افتراضي:
      اكسر;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  إذا((error_ڪوڊ >= 0x21) && !((error_ڪوڊ - 0x21) % 0x1f))
    أرجع "NO_ERROR";
  أرجع "unknown";
}
#جيڪڏهن_پورو /* CURLVERBOSE */

#إذا defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ساكن فراغ *vquic_ngtcp2_malloc(size_t سائيز, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_cmalloc(سائيز);
}

ساكن فراغ vquic_ngtcp2_free(فراغ *ptr, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  Curl_cfree(ptr);
}

ساكن فراغ *vquic_ngtcp2_calloc(size_t nmemb, size_t سائيز, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_ccalloc(nmemb, سائيز);
}

ساكن فراغ *vquic_ngtcp2_realloc(فراغ *ptr, size_t سائيز, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_crealloc(ptr, سائيز);
}

#جيڪڏهن_وصف_ٿيل USE_NGTCP2
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NGHTTP3
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
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* USE_NGTCP2 || USE_NGHTTP3 */

#وإلا /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *معلومات,
                             بنية connectdata *conn)
{
  (فراغ)معلومات;
  (فراغ)conn;
  DEBUGF(infof(معلومات, "QUIC is not supported in this build"));
  أرجع CURLE_NOT_BUILT_اندريون;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && USE_HTTP3 */
