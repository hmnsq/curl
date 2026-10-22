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

#اگر_تعریف_شده HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETINET_IP_H
#ضمن <netinet/ip.h>
#پایان_اگر

#اگر_تعریف_شده USE_NGHTTP3
#ضمن <nghttp3/nghttp3.h>
#پایان_اگر

#إذا defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#ضمن <sys/syscall.h>
#إذا defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#تعریف_کن HAVE_APPLE_پیام_X
#پایان_اگر
#پایان_اگر

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


#تعریف_کن NW_CHUNK_اندازه     (64 * 1024)
#تعریف_کن NW_SEND_CHUNKS    1

#اگر_تعریف_شده HAVE_APPLE_پیام_X

/* this is `struct msghdr` with an additional field at the end */
بنية msghdr_x {
  فراغ *پیام_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  بنية iovec *msg_iov;    /* scatter/gather array */
  صحيح پیام_iovlen;           /* # elements in msg_iov */
  فراغ *پیام_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  صحيح پیام_flags;            /* flags on received message */
  size_t پیام_datalen;       /* byte length of buffer in msg_iov */
};
#پایان_اگر

#اگر_تعریف_شده CURLVERBOSE
#اگر_تعریف_شده HAVE_APPLE_پیام_X
#تعریف_کن VQUIC_SEND_روش   "sendmsg_x"
#وإلاإذا defined(HAVE_SENDMSG)
#تعریف_کن VQUIC_SEND_روش   "sendmsg"
#وإلا
#تعریف_کن VQUIC_SEND_روش   "send"
#پایان_اگر
#پایان_اگر

صحيح Curl_vquic_init(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_init())
    أرجع 0;
#پایان_اگر

  أرجع 1;
}

فراغ Curl_vquic_cleanup(فراغ)
{
#إذا defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_نسخه_رقم >= 0x011800)
  ngtcp2_crypto_ossl_free();
#پایان_اگر
}

فراغ Curl_quic_ver(حرف *p, size_t len)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#وإلاإذا defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#پایان_اگر
}

CURLcode Curl_vquic_ctx_init(بنية Curl_easy *داده,
                             بنية cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_اندازه, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
#إذا defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#وإلاإذا defined(HAVE_APPLE_پیام_X)
  qctx->no_gso = FALSE;
#وإلا
  qctx->no_gso = TRUE;
#پایان_اگر
#اگر_تعریف_شده DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_QUIC_WBLOCK");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, 100))
        qctx->wblock_percent = (صحيح)l;
    }
  }
#پایان_اگر
  Curl_vquic_ctx_مجموعه_زمان(qctx, Curl_pgrs_now(داده));

  أرجع CURLE_OK;
}

فراغ Curl_vquic_ctx_free(بنية cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

فراغ Curl_vquic_ctx_مجموعه_زمان(بنية cf_quic_ctx *qctx,
                             ثابت بنية curltime *pnow)
{
  qctx->آخرین_op = *pnow;
}

فراغ Curl_vquic_ctx_update_زمان(بنية cf_quic_ctx *qctx,
                                ثابت بنية curltime *pnow)
{
  qctx->آخرین_op = *pnow;
}

#اگر_تعریف_شده HAVE_APPLE_پیام_X
ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#تعریف_کن پیام_X_SNUM  64
  بنية iovec msg_iov[پیام_X_SNUM];
  بنية msghdr_x mmsg[پیام_X_SNUM];
  حرف errstr[STRERROR_LEN];
  size_t n, i = 0, فرستاده = 0;
  صحيح rc;
  CURLcode نتیجه = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  إذا(!pktlen)
    أرجع CURLE_OK;
  إذا(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  طالما(i < n) {
    size_t j, batch = CURLMIN(n - i, پیام_X_SNUM), pkts_فرستاده = 0;

    لكل(j = 0; j < batch; ++j) {
      ثابت size_t جابه_جایی = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + جابه_جایی);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - جابه_جایی);
      memset(&mmsg[j], 0, حجم(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].پیام_iovlen = 1;
      mmsg[j].پیام_datalen = msg_iov[j].iov_len;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wdeprecated-declarations"
#پایان_اگر
    طالما((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#رهنمود GCC diagnostic برداشت
#پایان_اگر

    إذا(rc < 0) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(داده, cf, "egress, sendmsg_x -> EAGAIN");
        نتیجه = فرستاده ? CURLE_OK : CURLE_AGAIN;
        اذهب بیرون;
      }
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
        failf(داده, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        نتیجه = CURLE_SEND_ERROR;
        اذهب بیرون;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_فرستاده = batch;
    }
    وإلا
      pkts_فرستاده = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    إذا(!pkts_فرستاده) {  /* no packets of the current batch were sent */
      نتیجه = فرستاده ? CURLE_OK : CURLE_AGAIN;
      اذهب بیرون;
    }
    i += pkts_فرستاده; /* Some have been sent */
    لكل(j = 0; j < pkts_فرستاده; ++j)
      فرستاده += msg_iov[j].iov_len;
    إذا(pkts_فرستاده < batch)
      اذهب بیرون; /* but not all of them */
  }

بیرون:
  *psent = فرستاده;
  إذا(فرستاده || نتیجه)
    CURL_TRC_CF(داده, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                فرستاده, gsolen, n, calls, (صحيح)نتیجه);
  أرجع نتیجه;
}

#وإلا /* HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ساكن CURLcode do_sendmsg(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           بنية cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode نتیجه = CURLE_OK;
#اگر_تعریف_شده HAVE_SENDMSG
  بنية iovec msg_iov;
  بنية msghdr پیام = { 0 };
  ssize_t rv;
#إذا defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t پیام_ctrl[32];
  بنية cmsghdr *cm;
#پایان_اگر

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  پیام.msg_iov = &msg_iov;
  پیام.پیام_iovlen = 1;

#إذا defined(__linux__) && defined(UDP_SEGMENT)
  إذا(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(پیام_ctrl, 0, حجم(پیام_ctrl));
    پیام.پیام_control = پیام_ctrl;
    DEBUGASSERT(حجم(پیام_ctrl) >= CMSG_SPACE(حجم(صحيح)));
    پیام.msg_controllen = CMSG_SPACE(حجم(صحيح));
    cm = CMSG_FIRSTHDR(&پیام);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(حجم(uint16_t));
    *(uint16_t *)(فراغ *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#پایان_اگر

  طالما((rv = sendmsg(qctx->sockfd, &پیام, 0)) == -1 && SOCKERRNO == SOCKEINTR)
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
        infof(داده, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        أرجع send_packet_no_gso(cf, داده, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    افتراضي:
      failf(داده, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }
  وإلا إذا(pktlen != *psent) {
    failf(داده, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
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
      نتیجه = CURLE_AGAIN;
      اذهب بیرون;
    }
    وإلا {
      إذا(SOCKERRNO != SOCKEMSGSIZE) {
        failf(داده, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        نتیجه = CURLE_SEND_ERROR;
        اذهب بیرون;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#پایان_اگر
  (فراغ)cf;

بیرون:
  CURL_TRC_CF(داده, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_روش, pktlen, gsolen, (صحيح)نتیجه, *psent);
  أرجع نتیجه;
}

#پایان_اگر /* !HAVE_APPLE_MSG_X */

ساكن CURLcode send_packet_no_gso(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *انتها = pkt + pktlen;
  size_t فرستاده, len;
  CURLcode نتیجه = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  لكل(p = pkt; p < انتها; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(انتها - p));
    نتیجه = do_sendmsg(cf, داده, qctx, p, len, len, &فرستاده);
    إذا(نتیجه)
      اذهب بیرون;
    *psent += فرستاده;
    VERBOSE(++calls);
  }
بیرون:
  CURL_TRC_CF(داده, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_روش, pktlen, gsolen, calls, (صحيح)نتیجه, *psent);
  أرجع نتیجه;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ساكن CURLcode send_packet_no_gso_cf(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      ثابت uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *انتها = pkt + pktlen;
  size_t فرستاده, len;
  CURLcode نتیجه = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  لكل(p = pkt; p < انتها; p += len) {
    len = CURLMIN(gsolen, (size_t)(انتها - p));
    نتیجه = Curl_conn_cf_send(cf->بعدی, داده, p, len, FALSE, &فرستاده);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    إذا(نتیجه)
      اذهب بیرون;

    إذا(فرستاده != len) {
      /* We can only send the complete datagram, not parts. */
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    *psent += فرستاده;
  }

بیرون:
  CURL_TRC_CF(داده, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (صحيح)نتیجه, *psent);
  أرجع نتیجه;
}

ساكن CURLcode vquic_send_packets(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode نتیجه;
#اگر_تعریف_شده DEBUGBUILD
  /* simulate network blocking/partial writes */
  إذا(qctx->wblock_percent > 0) {
    بیعلامت حرف c;
    *psent = 0;
    Curl_rand(داده, &c, 1);
    إذا(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(داده, cf, "vquic_flush() simulate EWOULDBLOCK");
      أرجع CURLE_AGAIN;
    }
  }
#پایان_اگر
  إذا(qctx->no_gso && pktlen > gsolen) {
    نتیجه = send_packet_no_gso(cf, داده, qctx, pkt, pktlen, gsolen, psent);
  }
  وإلا {
    نتیجه = do_sendmsg(cf, داده, qctx, pkt, pktlen, gsolen, psent);
  }
  إذا(!نتیجه)
    qctx->آخرین_io = qctx->آخرین_op;
  أرجع نتیجه;
}

CURLcode Curl_vquic_flush(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                          بنية cf_quic_ctx *qctx)
{
  ثابت بیعلامت حرف *buf;
  size_t blen, فرستاده;
  CURLcode نتیجه;
  size_t gsolen;

  طالما(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    إذا(qctx->تقسیم_len) {
      gsolen = qctx->تقسیم_gsolen;
      إذا(blen > qctx->تقسیم_len)
        blen = qctx->تقسیم_len;
    }

    إذا(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      نتیجه = vquic_send_packets(cf, داده, qctx, buf, blen, gsolen, &فرستاده);
    }
    وإلا {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      إذا(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        نتیجه = send_packet_no_gso_cf(cf, داده, buf, blen, gsolen, &فرستاده);
      }
      وإلا {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        نتیجه = Curl_conn_cf_send(cf->بعدی, داده, buf, blen, FALSE, &فرستاده);
      }
    }

    إذا(نتیجه) {
      إذا(نتیجه == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, فرستاده);
        إذا(qctx->تقسیم_len)
          qctx->تقسیم_len -= فرستاده;
      }
      أرجع نتیجه;
    }
    Curl_bufq_skip(&qctx->sendbuf, فرستاده);
    إذا(qctx->تقسیم_len)
      qctx->تقسیم_len -= فرستاده;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_vquic_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                         بنية cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  أرجع Curl_vquic_flush(cf, داده, qctx);
}

CURLcode Curl_vquic_send_دم_تقسیم(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    بنية cf_quic_ctx *qctx, size_t gsolen,
                                    size_t دم_len, size_t دم_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > دم_len);
  qctx->تقسیم_len = Curl_bufq_len(&qctx->sendbuf) - دم_len;
  qctx->تقسیم_gsolen = gsolen;
  qctx->gsolen = دم_gsolen;
  CURL_TRC_CF(داده, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->تقسیم_len, qctx->تقسیم_gsolen, دم_len, qctx->gsolen);
  أرجع Curl_vquic_flush(cf, داده, qctx);
}

#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_پیام_X)
ساكن size_t vquic_msghdr_get_udp_gro(بنية msghdr *پیام)
{
  صحيح gso_اندازه = 0;
#إذا defined(__linux__) && defined(UDP_GRO)
  بنية cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wsign-compare"
#رهنمود clang diagnostic ignored "-Wcast-align"
#پایان_اگر
  لكل(cmsg = CMSG_FIRSTHDR(پیام); cmsg; cmsg = CMSG_NXTHDR(پیام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#رهنمود clang diagnostic برداشت
#پایان_اگر
    إذا(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      کپی_حافظه(&gso_اندازه, CMSG_DATA(cmsg), حجم(gso_اندازه));

      اكسر;
    }
  }
#پایان_اگر /* linux && UDP_GRO */
  (فراغ)پیام;

  أرجع (size_t)gso_اندازه;
}
#پایان_اگر /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#إذا (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_پیام_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_ماسک)
ساكن uint8_t vquic_msghdr_get_ecn(بنية msghdr *پیام, صحيح family)
{
  بنية cmsghdr *cmsg;
  بدل(family) {
  حالة AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wsign-compare"
#رهنمود clang diagnostic ignored "-Wcast-align"
#پایان_اگر
    لكل(cmsg = CMSG_FIRSTHDR(پیام); cmsg; cmsg = CMSG_NXTHDR(پیام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#رهنمود clang diagnostic برداشت
#پایان_اگر
      إذا(cmsg->cmsg_level == IPPROTO_IP &&
#اگر_تعریف_شده __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#وإلا
          cmsg->cmsg_type == IP_TOS
#پایان_اگر
          && cmsg->cmsg_len) {
        أرجع *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_ماسک;
      }
    }
    اكسر;
  حالة AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#إذا defined(__clang__) && !defined(__GLIBC__)
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wsign-compare"
#رهنمود clang diagnostic ignored "-Wcast-align"
#پایان_اگر
    لكل(cmsg = CMSG_FIRSTHDR(پیام); cmsg; cmsg = CMSG_NXTHDR(پیام, cmsg)) {
#إذا defined(__clang__) && !defined(__GLIBC__)
#رهنمود clang diagnostic برداشت
#پایان_اگر
      إذا(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        بیعلامت صحيح tos;

        کپی_حافظه(&tos, CMSG_DATA(cmsg), حجم(صحيح));

        أرجع (uint8_t)(tos & IPTOS_ECN_ماسک);
      }
    }
    اكسر;
  }
  أرجع 0;
}
#وإلا
#تعریف_کن vquic_msghdr_get_ecn(a, b)      0
#پایان_اگر /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#اگر_تعریف_شده HAVE_SENDMMSG

ساكن CURLcode recvmmsg_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
#إذا defined(__linux__) && defined(UDP_GRO)
#تعریف_کن MMSG_رقم  16
#تعریف_کن UDP_GRO_CNT_MAX  64
#تعریف_کن CMSG_در_هر_پیام_اندازه    (2 * CMSG_SPACE(حجم(صحيح)))
#وإلا
#تعریف_کن MMSG_رقم  64
#تعریف_کن UDP_GRO_CNT_MAX  1
#تعریف_کن CMSG_در_هر_پیام_اندازه    CMSG_SPACE(حجم(صحيح))
#پایان_اگر
#تعریف_کن پیام_BUF_اندازه  (UDP_GRO_CNT_MAX * 1500)
  بنية iovec msg_iov[MMSG_رقم];
  بنية mmsghdr mmsg[MMSG_رقم];
  uint8_t پیام_ctrl[MMSG_رقم * CMSG_در_هر_پیام_اندازه];
  بنية sockaddr_storage remote_addr[MMSG_رقم];
  size_t مجموع_nread = 0, pkts = 0;
#اگر_تعریف_شده CURLVERBOSE
  size_t calls = 0;
#پایان_اگر
  صحيح mcount, i, n;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجه = CURLE_OK;
  size_t gso_اندازه;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[پیام_BUF_اندازه] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  نتیجه = Curl_multi_xfer_sockbuf_borrow(داده, MMSG_رقم * پیام_BUF_اندازه,
                                          &sockbuf);
  إذا(نتیجه)
    اذهب بیرون;
  bufs = (uint8_t (*)[پیام_BUF_اندازه])sockbuf;

  مجموع_nread = 0;
  طالما(pkts < max_pkts) {
    n = (صحيح)CURLMIN(CURLMIN(MMSG_رقم, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    لكل(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].پیام_hdr.msg_iov = &msg_iov[i];
      mmsg[i].پیام_hdr.پیام_iovlen = 1;
      mmsg[i].پیام_hdr.پیام_name = &remote_addr[i];
      mmsg[i].پیام_hdr.msg_namelen = حجم(remote_addr[i]);
      mmsg[i].پیام_hdr.پیام_control = &پیام_ctrl[i * CMSG_در_هر_پیام_اندازه];
      mmsg[i].پیام_hdr.msg_controllen = CMSG_در_هر_پیام_اندازه;
    }

    طالما((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, عدم)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(داده, cf, "ingress, recvmmsg -> EAGAIN");
        اذهب بیرون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بعدی, داده, عدم, عدم, &ip))
          failf(داده, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورت);
        نتیجه = CURLE_COULDNT_CONNECT;
        اذهب بیرون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(داده, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      نتیجه = CURLE_RECV_ERROR;
      اذهب بیرون;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].پیام_len) {
        ++pkts;
        استمر;
      }
      مجموع_nread += mmsg[i].پیام_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].پیام_hdr, remote_addr[i].ss_family);
      gso_اندازه = vquic_msghdr_get_udp_gro(&mmsg[i].پیام_hdr);
      إذا(gso_اندازه == 0)
        gso_اندازه = mmsg[i].پیام_len;

      نتیجه = recv_cb(bufs[i], mmsg[i].پیام_len, gso_اندازه,
                       mmsg[i].پیام_hdr.پیام_name,
                       mmsg[i].پیام_hdr.msg_namelen, ecn, userp);
      إذا(نتیجه)
        اذهب بیرون;
      pkts += (mmsg[i].پیام_len + gso_اندازه - 1) / gso_اندازه;
    }
  }

بیرون:
  إذا(مجموع_nread || نتیجه)
    CURL_TRC_CF(داده, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                مجموع_nread, pkts, calls, (صحيح)نتیجه);
  Curl_multi_xfer_sockbuf_release(داده, sockbuf);
  *pnread = مجموع_nread;
  أرجع نتیجه;
}

#وإلاإذا defined(HAVE_APPLE_پیام_X)

ساكن CURLcode recvmsg_x_packets(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  فراغ *userp, size_t *pnread)
{
#تعریف_کن پیام_X_رقم  64
#تعریف_کن پیام_BUF_اندازه  (2048)
#تعریف_کن CMSG_در_هر_پیام_اندازه     CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov[پیام_X_رقم];
  بنية msghdr_x mmsg[پیام_X_رقم];
  uint8_t پیام_ctrl[پیام_X_رقم * CMSG_در_هر_پیام_اندازه];
  بنية sockaddr_storage remote_addr[پیام_X_رقم];
  size_t مجموع_nread = 0, pkts = 0;
#اگر_تعریف_شده CURLVERBOSE
  size_t calls = 0;
#پایان_اگر
  صحيح mcount, i;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجه = CURLE_OK;
  size_t gso_اندازه;
  حرف *sockbuf = عدم;
  uint8_t (*bufs)[پیام_BUF_اندازه] = عدم;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  نتیجه = Curl_multi_xfer_sockbuf_borrow(داده, پیام_X_رقم * پیام_BUF_اندازه,
                                          &sockbuf);
  إذا(نتیجه)
    اذهب بیرون;
  bufs = (uint8_t (*)[پیام_BUF_اندازه])sockbuf;

  مجموع_nread = 0;
  طالما(pkts < max_pkts) {
    صحيح n = (صحيح)CURLMIN(CURLMIN(پیام_X_رقم, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    لكل(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].پیام_iovlen = 1;
      mmsg[i].پیام_name = &remote_addr[i];
      mmsg[i].msg_namelen = حجم(remote_addr[i]);
      mmsg[i].پیام_control = &پیام_ctrl[i * CMSG_در_هر_پیام_اندازه];
      mmsg[i].msg_controllen = CMSG_در_هر_پیام_اندازه;
    }

#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wdeprecated-declarations"
#پایان_اگر
    طالما((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#إذا defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#رهنمود GCC diagnostic برداشت
#پایان_اگر
    إذا(mcount == -1) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(داده, cf, "ingress, recvmsg_x -> EAGAIN");
        اذهب بیرون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بعدی, داده, عدم, عدم, &ip))
          failf(داده, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورت);
        نتیجه = CURLE_COULDNT_CONNECT;
        اذهب بیرون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(داده, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      نتیجه = CURLE_RECV_ERROR;
      اذهب بیرون;
    }

    VERBOSE(++calls);
    لكل(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      إذا(!mmsg[i].پیام_datalen) {
        ++pkts;
        استمر;
      }
      مجموع_nread += mmsg[i].پیام_datalen;
      ecn = vquic_msghdr_get_ecn((بنية msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_اندازه = mmsg[i].پیام_datalen;

      نتیجه = recv_cb(bufs[i], mmsg[i].پیام_datalen, gso_اندازه,
                       mmsg[i].پیام_name,
                       mmsg[i].msg_namelen, ecn, userp);
      إذا(نتیجه)
        اذهب بیرون;
      pkts += (mmsg[i].پیام_datalen + gso_اندازه - 1) / gso_اندازه;
    }
  }

بیرون:
  إذا(مجموع_nread || نتیجه)
    CURL_TRC_CF(داده, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                مجموع_nread, pkts, calls, (صحيح)نتیجه);
  Curl_multi_xfer_sockbuf_release(داده, sockbuf);
  *pnread = مجموع_nread;
  أرجع نتیجه;
}

#وإلاإذا defined(HAVE_SENDMSG)
ساكن CURLcode recvmsg_packets(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                بنية cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                size_t *pnread)
{
#تعریف_کن CMSG_در_هر_پیام_اندازه    CMSG_SPACE(حجم(صحيح))
  بنية iovec msg_iov;
  بنية msghdr پیام;
  uint8_t buf[64 * 1024];
  بنية sockaddr_storage remote_addr;
  size_t مجموع_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجه = CURLE_OK;
  uint8_t پیام_ctrl[CMSG_در_هر_پیام_اندازه];
  size_t gso_اندازه;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, مجموع_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    memset(&پیام, 0, حجم(پیام));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = حجم(buf);
    پیام.msg_iov = &msg_iov;
    پیام.پیام_iovlen = 1;
    پیام.پیام_control = پیام_ctrl;
    پیام.پیام_name = &remote_addr;
    پیام.msg_namelen = حجم(remote_addr);
    پیام.msg_controllen = CMSG_در_هر_پیام_اندازه;

    طالما((rc = recvmsg(qctx->sockfd, &پیام, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rc, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        اذهب بیرون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بعدی, داده, عدم, عدم, &ip))
          failf(داده, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورت);
        نتیجه = CURLE_COULDNT_CONNECT;
        اذهب بیرون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(داده, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      نتیجه = CURLE_RECV_ERROR;
      اذهب بیرون;
    }

    مجموع_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread) {
      ++pkts;
      استمر;
    }

    ecn = vquic_msghdr_get_ecn(&پیام, remote_addr.ss_family);
    gso_اندازه = vquic_msghdr_get_udp_gro(&پیام);
    إذا(gso_اندازه == 0)
      gso_اندازه = nread;

    نتیجه = recv_cb(buf, nread, gso_اندازه,
                     پیام.پیام_name, پیام.msg_namelen, ecn, userp);
    إذا(نتیجه)
      اذهب بیرون;
    pkts += (nread + gso_اندازه - 1) / gso_اندازه;
  }

بیرون:
  إذا(مجموع_nread || نتیجه)
    CURL_TRC_CF(داده, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                مجموع_nread, pkts, calls, (صحيح)نتیجه);
  *pnread = مجموع_nread;
  أرجع نتیجه;
}

#وإلا /* HAVE_SENDMMSG || HAVE_SENDMSG */
ساكن CURLcode recvfrom_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  صحيح bufsize = (صحيح)حجم(buf);
  بنية sockaddr_storage remote_addr;
  socklen_t remote_addrlen = حجم(remote_addr);
  size_t مجموع_nread, pkts, calls = 0, nread;
  ssize_t rv;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  لكل(pkts = 0, مجموع_nread = 0; pkts < max_pkts;) {
    طالما((rv = recvfrom(qctx->sockfd, (حرف *)buf, bufsize, 0,
                         (بنية sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(داده, cf, "ingress, recvfrom -> EAGAIN");
        اذهب بیرون;
      }
      إذا(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        بنية ip_quadruple ip;
        إذا(!Curl_cf_socket_peek(cf->بعدی, داده, عدم, عدم, &ip))
          failf(داده, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورت);
        نتیجه = CURLE_COULDNT_CONNECT;
        اذهب بیرون;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(داده, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      نتیجه = CURLE_RECV_ERROR;
      اذهب بیرون;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    إذا(!nread)
      استمر;

    مجموع_nread += nread;
    نتیجه = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    إذا(نتیجه)
      اذهب بیرون;
  }

بیرون:
  إذا(مجموع_nread || نتیجه)
    CURL_TRC_CF(داده, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                مجموع_nread, pkts, calls, (صحيح)نتیجه);
  *pnread = مجموع_nread;
  أرجع نتیجه;
}
#پایان_اگر /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, فراغ *userp)
{
  CURLcode نتیجه;
  size_t nread = 0;
#اگر_تعریف_شده HAVE_SENDMMSG
  نتیجه = recvmmsg_packets(cf, داده, qctx, max_pkts, recv_cb, userp, &nread);
#وإلاإذا defined(HAVE_APPLE_پیام_X)
  نتیجه = recvmsg_x_packets(cf, داده, qctx, max_pkts, recv_cb, userp,
                             &nread);
#وإلاإذا defined(HAVE_SENDMSG)
  نتیجه = recvmsg_packets(cf, داده, qctx, max_pkts, recv_cb, userp, &nread);
#وإلا
  نتیجه = recvfrom_packets(cf, داده, qctx, max_pkts, recv_cb, userp, &nread);
#پایان_اگر
  إذا(!نتیجه) {
    /* EAGAIN is no error, only count actual packets as a reply */
    إذا(!qctx->got_اولی_byte && nread) {
      qctx->got_اولی_byte = TRUE;
      qctx->اولی_byte_at = qctx->آخرین_op;
    }
    qctx->آخرین_io = qctx->آخرین_op;
  }
  أرجع نتیجه;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(بنية Curl_easy *داده,
                      بیعلامت حرف *scid,
                      size_t scidlen,
                      صحيح *qlogfdp)
{
  حرف *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  إذا(qlog_dir) {
    بنية dynbuf fname;
    CURLcode نتیجه;
    بیعلامت صحيح i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    نتیجه = curlx_dyn_جمع_کن(&fname, qlog_dir);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_جمع_کن(&fname, "/");
    لكل(i = 0; (i < scidlen) && !نتیجه; i++) {
      حرف hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      نتیجه = curlx_dyn_جمع_کن(&fname, hex);
    }
    إذا(!نتیجه)
      نتیجه = curlx_dyn_جمع_کن(&fname, ".sqlog");

    إذا(!نتیجه) {
      صحيح qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_دودویی,
#اگر_تعریف_شده _WIN32
                              داده->مجموعه.new_file_perms & (_S_IREAD | _S_IWRITE)
#وإلا
                              (mode_t)داده->مجموعه.new_file_perms
#پایان_اگر
                              );
      إذا(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_cf_quic_درج_after(بنية Curl_cfilter *cf_at,
                                   بنية Curl_easy *داده,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجه;

  (فراغ)داده; /* not used in all cases and compilers are stupid */
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  نتیجه = Curl_cf_ngtcp2_درج_after(cf_at, origin, peer, &cf);
#وإلاإذا defined(USE_QUICHE)
  نتیجه = Curl_cf_quiche_درج_after(cf_at, origin, peer, &cf);
#وإلا
  (فراغ)cf)
  (فراغ)cf_at;
  (فراغ)origin;
  (فراغ)peer;
  نتیجه = CURLE_NOT_BUILT_درون;
#پایان_اگر

#إذا defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  إذا(!نتیجه && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    نتیجه = Curl_conn_dns_جمع_کن_https_resolve(داده, cf->conn,
                                             cf->sockindex, origin);
  }
#پایان_اگر /* USE_HTTPSRR && USE_ECH */
  أرجع نتیجه;
}

CURLcode Curl_cf_quic_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *داده,
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
  أرجع Curl_cf_ngtcp2_create(pcf, داده, origin, peer, conn, addr);
#وإلاإذا defined(USE_QUICHE)
  أرجع Curl_cf_quiche_create(pcf, داده, origin, peer, conn, addr);
#وإلا
  *pcf = عدم;
  (فراغ)داده;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)conn;
  (فراغ)addr;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

#إذا !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_درج_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *داده,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#إذا defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  أرجع Curl_cf_ngtcp2_proxy_درج_after(cf_at, داده, origin, peer,
                                           tunnel_peer, tunnel_transport);
#وإلا
  (فراغ)cf_at;
  (فراغ)داده;
  (فراغ)origin;
  (فراغ)peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

CURLcode Curl_cf_h3_proxy_create(بنية Curl_cfilter **pcf,
                                 بنية Curl_easy *داده,
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
  أرجع Curl_cf_ngtcp2_proxy_create(pcf, داده, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#وإلا
  *pcf = عدم;
  (فراغ)داده;
  (فراغ)conn;
  (فراغ)addr;
  (فراغ)peer;
  (فراغ)transport_peer;
  (فراغ)tunnel_peer;
  (فراغ)tunnel_transport;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

#پایان_اگر /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *داده,
                             بنية connectdata *conn)
{
  إذا(Curl_conn_get_اولی_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(داده, "HTTP/3 cannot be used over UNIX domain sockets");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  إذا(!(داده->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(داده, "HTTP/3 requested for non-HTTPS URL");
    أرجع CURLE_URL_MALFORMAT;
  }
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(conn->socks_proxy.peer) {
    failf(داده, "HTTP/3 is not supported over a SOCKS proxy");
    أرجع CURLE_URL_MALFORMAT;
  }
#وإلا
  (فراغ)conn;
#پایان_اگر

  أرجع CURLE_OK;
}

#اگر_تعریف_شده CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_کد)
{
  إذا(error_کد <= UINT_MAX) {
    بدل((بیعلامت صحيح)error_کد) {
    حالة CURL_H3_ERR_NO_ERROR:
      أرجع "NO_ERROR";
    حالة CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      أرجع "GENERAL_PROTOCOL_ERROR";
    حالة CURL_H3_ERR_داخلی_ERROR:
      أرجع "INTERNAL_ERROR";
    حالة CURL_H3_ERR_جریان_CREATION_ERROR:
      أرجع "STREAM_CREATION_ERROR";
    حالة CURL_H3_ERR_CLOSED_CRITICAL_جریان:
      أرجع "CLOSED_CRITICAL_STREAM";
    حالة CURL_H3_ERR_قاب_UNEXPECTED:
      أرجع "FRAME_UNEXPECTED";
    حالة CURL_H3_ERR_قاب_ERROR:
      أرجع "FRAME_ERROR";
    حالة CURL_H3_ERR_EXCESSIVE_بارگیری:
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
    حالة CURL_H3_ERR_پیام_ERROR:
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
  إذا((error_کد >= 0x21) && !((error_کد - 0x21) % 0x1f))
    أرجع "NO_ERROR";
  أرجع "unknown";
}
#پایان_اگر /* CURLVERBOSE */

#إذا defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ساكن فراغ *vquic_ngtcp2_malloc(size_t اندازه, فراغ *user_داده)
{
  (فراغ)user_داده;
  أرجع Curl_cmalloc(اندازه);
}

ساكن فراغ vquic_ngtcp2_free(فراغ *ptr, فراغ *user_داده)
{
  (فراغ)user_داده;
  Curl_cfree(ptr);
}

ساكن فراغ *vquic_ngtcp2_calloc(size_t nmemb, size_t اندازه, فراغ *user_داده)
{
  (فراغ)user_داده;
  أرجع Curl_ccalloc(nmemb, اندازه);
}

ساكن فراغ *vquic_ngtcp2_realloc(فراغ *ptr, size_t اندازه, فراغ *user_داده)
{
  (فراغ)user_داده;
  أرجع Curl_crealloc(ptr, اندازه);
}

#اگر_تعریف_شده USE_NGTCP2
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
#پایان_اگر

#اگر_تعریف_شده USE_NGHTTP3
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
#پایان_اگر

#پایان_اگر /* USE_NGTCP2 || USE_NGHTTP3 */

#وإلا /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *داده,
                             بنية connectdata *conn)
{
  (فراغ)داده;
  (فراغ)conn;
  DEBUGF(infof(داده, "QUIC is not supported in this build"));
  أرجع CURLE_NOT_BUILT_درون;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && USE_HTTP3 */
