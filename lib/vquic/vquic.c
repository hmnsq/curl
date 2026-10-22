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

#اگر_تعریف_شدہ HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_IP_H
#شامل <netinet/ip.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_NGHTTP3
#شامل <nghttp3/nghttp3.h>
#اگر_ختم

#اگر defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#شامل <sys/syscall.h>
#اگر defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#تعریف_کرو HAVE_APPLE_پیغام_X
#اگر_ختم
#اگر_ختم

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


#تعریف_کرو NW_CHUNK_سائز     (64 * 1024)
#تعریف_کرو NW_SEND_CHUNKS    1

#اگر_تعریف_شدہ HAVE_APPLE_پیغام_X

/* this is `struct msghdr` with an additional field at the end */
ساخت msghdr_x {
  خالی *پیغام_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  ساخت iovec *msg_iov;    /* scatter/gather array */
  صحیح پیغام_iovlen;           /* # elements in msg_iov */
  خالی *پیغام_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  صحیح پیغام_flags;            /* flags on received message */
  size_t پیغام_datalen;       /* byte length of buffer in msg_iov */
};
#اگر_ختم

#اگر_تعریف_شدہ CURLVERBOSE
#اگر_تعریف_شدہ HAVE_APPLE_پیغام_X
#تعریف_کرو VQUIC_SEND_طریقہ   "sendmsg_x"
#وگرنہ_اگر defined(HAVE_SENDMSG)
#تعریف_کرو VQUIC_SEND_طریقہ   "sendmsg"
#ورنہ
#تعریف_کرو VQUIC_SEND_طریقہ   "send"
#اگر_ختم
#اگر_ختم

صحیح Curl_vquic_init(خالی)
{
#اگر defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  اگر(ngtcp2_crypto_ossl_init())
    واپس 0;
#اگر_ختم

  واپس 1;
}

خالی Curl_vquic_cleanup(خالی)
{
#اگر defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_نسخہ_عدد >= 0x011800)
  ngtcp2_crypto_ossl_free();
#اگر_ختم
}

خالی Curl_quic_ver(حرف *p, size_t len)
{
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#وگرنہ_اگر defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#اگر_ختم
}

CURLcode Curl_vquic_ctx_init(ساخت Curl_easy *ڈیٹا,
                             ساخت cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_سائز, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
#اگر defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#وگرنہ_اگر defined(HAVE_APPLE_پیغام_X)
  qctx->no_gso = FALSE;
#ورنہ
  qctx->no_gso = TRUE;
#اگر_ختم
#اگر_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_QUIC_WBLOCK");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, 100))
        qctx->wblock_percent = (صحیح)l;
    }
  }
#اگر_ختم
  Curl_vquic_ctx_سیٹ_وقت(qctx, Curl_pgrs_now(ڈیٹا));

  واپس CURLE_OK;
}

خالی Curl_vquic_ctx_free(ساخت cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

خالی Curl_vquic_ctx_سیٹ_وقت(ساخت cf_quic_ctx *qctx,
                             ثابت ساخت curltime *pnow)
{
  qctx->آخری_op = *pnow;
}

خالی Curl_vquic_ctx_update_وقت(ساخت cf_quic_ctx *qctx,
                                ثابت ساخت curltime *pnow)
{
  qctx->آخری_op = *pnow;
}

#اگر_تعریف_شدہ HAVE_APPLE_پیغام_X
ساکن CURLcode do_sendmsg(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#تعریف_کرو پیغام_X_SNUM  64
  ساخت iovec msg_iov[پیغام_X_SNUM];
  ساخت msghdr_x mmsg[پیغام_X_SNUM];
  حرف errstr[STRERROR_LEN];
  size_t n, i = 0, بھیجا = 0;
  صحیح rc;
  CURLcode نتیجہ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  اگر(!pktlen)
    واپس CURLE_OK;
  اگر(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  جب(i < n) {
    size_t j, batch = CURLMIN(n - i, پیغام_X_SNUM), pkts_بھیجا = 0;

    برائے(j = 0; j < batch; ++j) {
      ثابت size_t انحراف = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + انحراف);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - انحراف);
      memset(&mmsg[j], 0, حجم(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].پیغام_iovlen = 1;
      mmsg[j].پیغام_datalen = msg_iov[j].iov_len;
    }

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم
    جب((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ہدایت GCC diagnostic نکالو
#اگر_ختم

    اگر(rc < 0) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ڈیٹا, cf, "egress, sendmsg_x -> EAGAIN");
        نتیجہ = بھیجا ? CURLE_OK : CURLE_AGAIN;
        جاؤ برآمد;
      }
      اگر(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
        failf(ڈیٹا, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        نتیجہ = CURLE_SEND_ERROR;
        جاؤ برآمد;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_بھیجا = batch;
    }
    ورنہ
      pkts_بھیجا = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    اگر(!pkts_بھیجا) {  /* no packets of the current batch were sent */
      نتیجہ = بھیجا ? CURLE_OK : CURLE_AGAIN;
      جاؤ برآمد;
    }
    i += pkts_بھیجا; /* Some have been sent */
    برائے(j = 0; j < pkts_بھیجا; ++j)
      بھیجا += msg_iov[j].iov_len;
    اگر(pkts_بھیجا < batch)
      جاؤ برآمد; /* but not all of them */
  }

برآمد:
  *psent = بھیجا;
  اگر(بھیجا || نتیجہ)
    CURL_TRC_CF(ڈیٹا, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                بھیجا, gsolen, n, calls, (صحیح)نتیجہ);
  واپس نتیجہ;
}

#ورنہ /* HAVE_APPLE_MSG_X */

ساکن CURLcode send_packet_no_gso(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

ساکن CURLcode do_sendmsg(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت cf_quic_ctx *qctx,
                           ثابت uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode نتیجہ = CURLE_OK;
#اگر_تعریف_شدہ HAVE_SENDMSG
  ساخت iovec msg_iov;
  ساخت msghdr پیغام = { 0 };
  ssize_t rv;
#اگر defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t پیغام_ctrl[32];
  ساخت cmsghdr *cm;
#اگر_ختم

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  پیغام.msg_iov = &msg_iov;
  پیغام.پیغام_iovlen = 1;

#اگر defined(__linux__) && defined(UDP_SEGMENT)
  اگر(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    memset(پیغام_ctrl, 0, حجم(پیغام_ctrl));
    پیغام.پیغام_control = پیغام_ctrl;
    DEBUGASSERT(حجم(پیغام_ctrl) >= CMSG_SPACE(حجم(صحیح)));
    پیغام.msg_controllen = CMSG_SPACE(حجم(صحیح));
    cm = CMSG_FIRSTHDR(&پیغام);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(حجم(uint16_t));
    *(uint16_t *)(خالی *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#اگر_ختم

  جب((rv = sendmsg(qctx->sockfd, &پیغام, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  اگر(!curlx_sztouz(rv, psent)) {
    صحیح sockerr = SOCKERRNO;
    اگر(SOCK_EAGAIN(sockerr))
      واپس CURLE_AGAIN;
    انتخاب(sockerr) {
    حالت SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      توڑو;
    حالت EIO:
      اگر(pktlen > gsolen) {
        /* GSO failure */
        infof(ڈیٹا, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        واپس send_packet_no_gso(cf, ڈیٹا, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    طےشدہ:
      failf(ڈیٹا, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }
  ورنہ اگر(pktlen != *psent) {
    failf(ڈیٹا, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
#ورنہ
  ssize_t rv;
  (خالی)gsolen;

  *psent = 0;

  جب((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  اگر(!curlx_sztouz(rv, psent)) {
    اگر(SOCK_EAGAIN(SOCKERRNO)) {
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    }
    ورنہ {
      اگر(SOCKERRNO != SOCKEMSGSIZE) {
        failf(ڈیٹا, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        نتیجہ = CURLE_SEND_ERROR;
        جاؤ برآمد;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#اگر_ختم
  (خالی)cf;

برآمد:
  CURL_TRC_CF(ڈیٹا, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_طریقہ, pktlen, gsolen, (صحیح)نتیجہ, *psent);
  واپس نتیجہ;
}

#اگر_ختم /* !HAVE_APPLE_MSG_X */

ساکن CURLcode send_packet_no_gso(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *انتہا = pkt + pktlen;
  size_t بھیجا, len;
  CURLcode نتیجہ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  برائے(p = pkt; p < انتہا; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(انتہا - p));
    نتیجہ = do_sendmsg(cf, ڈیٹا, qctx, p, len, len, &بھیجا);
    اگر(نتیجہ)
      جاؤ برآمد;
    *psent += بھیجا;
    VERBOSE(++calls);
  }
برآمد:
  CURL_TRC_CF(ڈیٹا, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_طریقہ, pktlen, gsolen, calls, (صحیح)نتیجہ, *psent);
  واپس نتیجہ;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
ساکن CURLcode send_packet_no_gso_cf(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ثابت uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ثابت uint8_t *p, *انتہا = pkt + pktlen;
  size_t بھیجا, len;
  CURLcode نتیجہ = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  برائے(p = pkt; p < انتہا; p += len) {
    len = CURLMIN(gsolen, (size_t)(انتہا - p));
    نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, p, len, FALSE, &بھیجا);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    اگر(نتیجہ)
      جاؤ برآمد;

    اگر(بھیجا != len) {
      /* We can only send the complete datagram, not parts. */
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    *psent += بھیجا;
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (صحیح)نتیجہ, *psent);
  واپس نتیجہ;
}

ساکن CURLcode vquic_send_packets(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت cf_quic_ctx *qctx,
                                   ثابت uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode نتیجہ;
#اگر_تعریف_شدہ DEBUGBUILD
  /* simulate network blocking/partial writes */
  اگر(qctx->wblock_percent > 0) {
    غیرعلامتی حرف c;
    *psent = 0;
    Curl_rand(ڈیٹا, &c, 1);
    اگر(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(ڈیٹا, cf, "vquic_flush() simulate EWOULDBLOCK");
      واپس CURLE_AGAIN;
    }
  }
#اگر_ختم
  اگر(qctx->no_gso && pktlen > gsolen) {
    نتیجہ = send_packet_no_gso(cf, ڈیٹا, qctx, pkt, pktlen, gsolen, psent);
  }
  ورنہ {
    نتیجہ = do_sendmsg(cf, ڈیٹا, qctx, pkt, pktlen, gsolen, psent);
  }
  اگر(!نتیجہ)
    qctx->آخری_io = qctx->آخری_op;
  واپس نتیجہ;
}

CURLcode Curl_vquic_flush(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          ساخت cf_quic_ctx *qctx)
{
  ثابت غیرعلامتی حرف *buf;
  size_t blen, بھیجا;
  CURLcode نتیجہ;
  size_t gsolen;

  جب(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    اگر(qctx->بانٹو_len) {
      gsolen = qctx->بانٹو_gsolen;
      اگر(blen > qctx->بانٹو_len)
        blen = qctx->بانٹو_len;
    }

    اگر(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      نتیجہ = vquic_send_packets(cf, ڈیٹا, qctx, buf, blen, gsolen, &بھیجا);
    }
    ورنہ {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      اگر(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        نتیجہ = send_packet_no_gso_cf(cf, ڈیٹا, buf, blen, gsolen, &بھیجا);
      }
      ورنہ {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, buf, blen, FALSE, &بھیجا);
      }
    }

    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, بھیجا);
        اگر(qctx->بانٹو_len)
          qctx->بانٹو_len -= بھیجا;
      }
      واپس نتیجہ;
    }
    Curl_bufq_skip(&qctx->sendbuf, بھیجا);
    اگر(qctx->بانٹو_len)
      qctx->بانٹو_len -= بھیجا;
  }
  واپس CURLE_OK;
}

CURLcode Curl_vquic_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                         ساخت cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  واپس Curl_vquic_flush(cf, ڈیٹا, qctx);
}

CURLcode Curl_vquic_send_دم_بانٹو(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت cf_quic_ctx *qctx, size_t gsolen,
                                    size_t دم_len, size_t دم_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > دم_len);
  qctx->بانٹو_len = Curl_bufq_len(&qctx->sendbuf) - دم_len;
  qctx->بانٹو_gsolen = gsolen;
  qctx->gsolen = دم_gsolen;
  CURL_TRC_CF(ڈیٹا, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->بانٹو_len, qctx->بانٹو_gsolen, دم_len, qctx->gsolen);
  واپس Curl_vquic_flush(cf, ڈیٹا, qctx);
}

#اگر (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_پیغام_X)
ساکن size_t vquic_msghdr_get_udp_gro(ساخت msghdr *پیغام)
{
  صحیح gso_سائز = 0;
#اگر defined(__linux__) && defined(UDP_GRO)
  ساخت cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#اگر defined(__clang__) && !defined(__GLIBC__)
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wsign-compare"
#ہدایت clang diagnostic ignored "-Wcast-align"
#اگر_ختم
  برائے(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#اگر defined(__clang__) && !defined(__GLIBC__)
#ہدایت clang diagnostic نکالو
#اگر_ختم
    اگر(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      حافظہ_نقل(&gso_سائز, CMSG_DATA(cmsg), حجم(gso_سائز));

      توڑو;
    }
  }
#اگر_ختم /* linux && UDP_GRO */
  (خالی)پیغام;

  واپس (size_t)gso_سائز;
}
#اگر_ختم /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#اگر (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_پیغام_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_نقاب)
ساکن uint8_t vquic_msghdr_get_ecn(ساخت msghdr *پیغام, صحیح family)
{
  ساخت cmsghdr *cmsg;
  انتخاب(family) {
  حالت AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#اگر defined(__clang__) && !defined(__GLIBC__)
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wsign-compare"
#ہدایت clang diagnostic ignored "-Wcast-align"
#اگر_ختم
    برائے(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#اگر defined(__clang__) && !defined(__GLIBC__)
#ہدایت clang diagnostic نکالو
#اگر_ختم
      اگر(cmsg->cmsg_level == IPPROTO_IP &&
#اگر_تعریف_شدہ __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#ورنہ
          cmsg->cmsg_type == IP_TOS
#اگر_ختم
          && cmsg->cmsg_len) {
        واپس *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_نقاب;
      }
    }
    توڑو;
  حالت AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#اگر defined(__clang__) && !defined(__GLIBC__)
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wsign-compare"
#ہدایت clang diagnostic ignored "-Wcast-align"
#اگر_ختم
    برائے(cmsg = CMSG_FIRSTHDR(پیغام); cmsg; cmsg = CMSG_NXTHDR(پیغام, cmsg)) {
#اگر defined(__clang__) && !defined(__GLIBC__)
#ہدایت clang diagnostic نکالو
#اگر_ختم
      اگر(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        غیرعلامتی صحیح tos;

        حافظہ_نقل(&tos, CMSG_DATA(cmsg), حجم(صحیح));

        واپس (uint8_t)(tos & IPTOS_ECN_نقاب);
      }
    }
    توڑو;
  }
  واپس 0;
}
#ورنہ
#تعریف_کرو vquic_msghdr_get_ecn(a, b)      0
#اگر_ختم /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#اگر_تعریف_شدہ HAVE_SENDMMSG

ساکن CURLcode recvmmsg_packets(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, خالی *userp,
                                 size_t *pnread)
{
#اگر defined(__linux__) && defined(UDP_GRO)
#تعریف_کرو MMSG_عدد  16
#تعریف_کرو UDP_GRO_CNT_MAX  64
#تعریف_کرو CMSG_فی_پیغام_سائز    (2 * CMSG_SPACE(حجم(صحیح)))
#ورنہ
#تعریف_کرو MMSG_عدد  64
#تعریف_کرو UDP_GRO_CNT_MAX  1
#تعریف_کرو CMSG_فی_پیغام_سائز    CMSG_SPACE(حجم(صحیح))
#اگر_ختم
#تعریف_کرو پیغام_BUF_سائز  (UDP_GRO_CNT_MAX * 1500)
  ساخت iovec msg_iov[MMSG_عدد];
  ساخت mmsghdr mmsg[MMSG_عدد];
  uint8_t پیغام_ctrl[MMSG_عدد * CMSG_فی_پیغام_سائز];
  ساخت sockaddr_storage remote_addr[MMSG_عدد];
  size_t کل_nread = 0, pkts = 0;
#اگر_تعریف_شدہ CURLVERBOSE
  size_t calls = 0;
#اگر_ختم
  صحیح mcount, i, n;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجہ = CURLE_OK;
  size_t gso_سائز;
  حرف *sockbuf = تہی;
  uint8_t (*bufs)[پیغام_BUF_سائز] = تہی;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  نتیجہ = Curl_multi_xfer_sockbuf_borrow(ڈیٹا, MMSG_عدد * پیغام_BUF_سائز,
                                          &sockbuf);
  اگر(نتیجہ)
    جاؤ برآمد;
  bufs = (uint8_t (*)[پیغام_BUF_سائز])sockbuf;

  کل_nread = 0;
  جب(pkts < max_pkts) {
    n = (صحیح)CURLMIN(CURLMIN(MMSG_عدد, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    برائے(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].پیغام_hdr.msg_iov = &msg_iov[i];
      mmsg[i].پیغام_hdr.پیغام_iovlen = 1;
      mmsg[i].پیغام_hdr.پیغام_name = &remote_addr[i];
      mmsg[i].پیغام_hdr.msg_namelen = حجم(remote_addr[i]);
      mmsg[i].پیغام_hdr.پیغام_control = &پیغام_ctrl[i * CMSG_فی_پیغام_سائز];
      mmsg[i].پیغام_hdr.msg_controllen = CMSG_فی_پیغام_سائز;
    }

    جب((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, تہی)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    اگر(mcount == -1) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ڈیٹا, cf, "ingress, recvmmsg -> EAGAIN");
        جاؤ برآمد;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساخت ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->اگلا, ڈیٹا, تہی, تہی, &ip))
          failf(ڈیٹا, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٹ);
        نتیجہ = CURLE_COULDNT_CONNECT;
        جاؤ برآمد;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(ڈیٹا, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }

    VERBOSE(++calls);
    برائے(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      اگر(!mmsg[i].پیغام_len) {
        ++pkts;
        جاری;
      }
      کل_nread += mmsg[i].پیغام_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].پیغام_hdr, remote_addr[i].ss_family);
      gso_سائز = vquic_msghdr_get_udp_gro(&mmsg[i].پیغام_hdr);
      اگر(gso_سائز == 0)
        gso_سائز = mmsg[i].پیغام_len;

      نتیجہ = recv_cb(bufs[i], mmsg[i].پیغام_len, gso_سائز,
                       mmsg[i].پیغام_hdr.پیغام_name,
                       mmsg[i].پیغام_hdr.msg_namelen, ecn, userp);
      اگر(نتیجہ)
        جاؤ برآمد;
      pkts += (mmsg[i].پیغام_len + gso_سائز - 1) / gso_سائز;
    }
  }

برآمد:
  اگر(کل_nread || نتیجہ)
    CURL_TRC_CF(ڈیٹا, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                کل_nread, pkts, calls, (صحیح)نتیجہ);
  Curl_multi_xfer_sockbuf_release(ڈیٹا, sockbuf);
  *pnread = کل_nread;
  واپس نتیجہ;
}

#وگرنہ_اگر defined(HAVE_APPLE_پیغام_X)

ساکن CURLcode recvmsg_x_packets(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  خالی *userp, size_t *pnread)
{
#تعریف_کرو پیغام_X_عدد  64
#تعریف_کرو پیغام_BUF_سائز  (2048)
#تعریف_کرو CMSG_فی_پیغام_سائز     CMSG_SPACE(حجم(صحیح))
  ساخت iovec msg_iov[پیغام_X_عدد];
  ساخت msghdr_x mmsg[پیغام_X_عدد];
  uint8_t پیغام_ctrl[پیغام_X_عدد * CMSG_فی_پیغام_سائز];
  ساخت sockaddr_storage remote_addr[پیغام_X_عدد];
  size_t کل_nread = 0, pkts = 0;
#اگر_تعریف_شدہ CURLVERBOSE
  size_t calls = 0;
#اگر_ختم
  صحیح mcount, i;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجہ = CURLE_OK;
  size_t gso_سائز;
  حرف *sockbuf = تہی;
  uint8_t (*bufs)[پیغام_BUF_سائز] = تہی;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  نتیجہ = Curl_multi_xfer_sockbuf_borrow(ڈیٹا, پیغام_X_عدد * پیغام_BUF_سائز,
                                          &sockbuf);
  اگر(نتیجہ)
    جاؤ برآمد;
  bufs = (uint8_t (*)[پیغام_BUF_سائز])sockbuf;

  کل_nread = 0;
  جب(pkts < max_pkts) {
    صحیح n = (صحیح)CURLMIN(CURLMIN(پیغام_X_عدد, IOV_MAX), max_pkts);
    memset(&mmsg, 0, حجم(mmsg));
    برائے(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = حجم(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].پیغام_iovlen = 1;
      mmsg[i].پیغام_name = &remote_addr[i];
      mmsg[i].msg_namelen = حجم(remote_addr[i]);
      mmsg[i].پیغام_control = &پیغام_ctrl[i * CMSG_فی_پیغام_سائز];
      mmsg[i].msg_controllen = CMSG_فی_پیغام_سائز;
    }

#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
#اگر_ختم
    جب((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#اگر defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#ہدایت GCC diagnostic نکالو
#اگر_ختم
    اگر(mcount == -1) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ڈیٹا, cf, "ingress, recvmsg_x -> EAGAIN");
        جاؤ برآمد;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساخت ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->اگلا, ڈیٹا, تہی, تہی, &ip))
          failf(ڈیٹا, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٹ);
        نتیجہ = CURLE_COULDNT_CONNECT;
        جاؤ برآمد;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(ڈیٹا, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }

    VERBOSE(++calls);
    برائے(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      اگر(!mmsg[i].پیغام_datalen) {
        ++pkts;
        جاری;
      }
      کل_nread += mmsg[i].پیغام_datalen;
      ecn = vquic_msghdr_get_ecn((ساخت msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_سائز = mmsg[i].پیغام_datalen;

      نتیجہ = recv_cb(bufs[i], mmsg[i].پیغام_datalen, gso_سائز,
                       mmsg[i].پیغام_name,
                       mmsg[i].msg_namelen, ecn, userp);
      اگر(نتیجہ)
        جاؤ برآمد;
      pkts += (mmsg[i].پیغام_datalen + gso_سائز - 1) / gso_سائز;
    }
  }

برآمد:
  اگر(کل_nread || نتیجہ)
    CURL_TRC_CF(ڈیٹا, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                کل_nread, pkts, calls, (صحیح)نتیجہ);
  Curl_multi_xfer_sockbuf_release(ڈیٹا, sockbuf);
  *pnread = کل_nread;
  واپس نتیجہ;
}

#وگرنہ_اگر defined(HAVE_SENDMSG)
ساکن CURLcode recvmsg_packets(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, خالی *userp,
                                size_t *pnread)
{
#تعریف_کرو CMSG_فی_پیغام_سائز    CMSG_SPACE(حجم(صحیح))
  ساخت iovec msg_iov;
  ساخت msghdr پیغام;
  uint8_t buf[64 * 1024];
  ساخت sockaddr_storage remote_addr;
  size_t کل_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجہ = CURLE_OK;
  uint8_t پیغام_ctrl[CMSG_فی_پیغام_سائز];
  size_t gso_سائز;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  برائے(pkts = 0, کل_nread = 0, calls = 0; pkts < max_pkts;) {
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
    پیغام.msg_controllen = CMSG_فی_پیغام_سائز;

    جب((rc = recvmsg(qctx->sockfd, &پیغام, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    اگر(!curlx_sztouz(rc, &nread)) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        جاؤ برآمد;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساخت ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->اگلا, ڈیٹا, تہی, تہی, &ip))
          failf(ڈیٹا, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٹ);
        نتیجہ = CURLE_COULDNT_CONNECT;
        جاؤ برآمد;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(ڈیٹا, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }

    کل_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    اگر(!nread) {
      ++pkts;
      جاری;
    }

    ecn = vquic_msghdr_get_ecn(&پیغام, remote_addr.ss_family);
    gso_سائز = vquic_msghdr_get_udp_gro(&پیغام);
    اگر(gso_سائز == 0)
      gso_سائز = nread;

    نتیجہ = recv_cb(buf, nread, gso_سائز,
                     پیغام.پیغام_name, پیغام.msg_namelen, ecn, userp);
    اگر(نتیجہ)
      جاؤ برآمد;
    pkts += (nread + gso_سائز - 1) / gso_سائز;
  }

برآمد:
  اگر(کل_nread || نتیجہ)
    CURL_TRC_CF(ڈیٹا, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                کل_nread, pkts, calls, (صحیح)نتیجہ);
  *pnread = کل_nread;
  واپس نتیجہ;
}

#ورنہ /* HAVE_SENDMMSG || HAVE_SENDMSG */
ساکن CURLcode recvfrom_packets(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, خالی *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  صحیح bufsize = (صحیح)حجم(buf);
  ساخت sockaddr_storage remote_addr;
  socklen_t remote_addrlen = حجم(remote_addr);
  size_t کل_nread, pkts, calls = 0, nread;
  ssize_t rv;
  حرف errstr[STRERROR_LEN];
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  برائے(pkts = 0, کل_nread = 0; pkts < max_pkts;) {
    جب((rv = recvfrom(qctx->sockfd, (حرف *)buf, bufsize, 0,
                         (ساخت sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    اگر(!curlx_sztouz(rv, &nread)) {
      اگر(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(ڈیٹا, cf, "ingress, recvfrom -> EAGAIN");
        جاؤ برآمد;
      }
      اگر(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        ساخت ip_quadruple ip;
        اگر(!Curl_cf_socket_peek(cf->اگلا, ڈیٹا, تہی, تہی, &ip))
          failf(ڈیٹا, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_پورٹ);
        نتیجہ = CURLE_COULDNT_CONNECT;
        جاؤ برآمد;
      }
      curlx_strerror(SOCKERRNO, errstr, حجم(errstr));
      failf(ڈیٹا, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    اگر(!nread)
      جاری;

    کل_nread += nread;
    نتیجہ = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

برآمد:
  اگر(کل_nread || نتیجہ)
    CURL_TRC_CF(ڈیٹا, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                کل_nread, pkts, calls, (صحیح)نتیجہ);
  *pnread = کل_nread;
  واپس نتیجہ;
}
#اگر_ختم /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, خالی *userp)
{
  CURLcode نتیجہ;
  size_t nread = 0;
#اگر_تعریف_شدہ HAVE_SENDMMSG
  نتیجہ = recvmmsg_packets(cf, ڈیٹا, qctx, max_pkts, recv_cb, userp, &nread);
#وگرنہ_اگر defined(HAVE_APPLE_پیغام_X)
  نتیجہ = recvmsg_x_packets(cf, ڈیٹا, qctx, max_pkts, recv_cb, userp,
                             &nread);
#وگرنہ_اگر defined(HAVE_SENDMSG)
  نتیجہ = recvmsg_packets(cf, ڈیٹا, qctx, max_pkts, recv_cb, userp, &nread);
#ورنہ
  نتیجہ = recvfrom_packets(cf, ڈیٹا, qctx, max_pkts, recv_cb, userp, &nread);
#اگر_ختم
  اگر(!نتیجہ) {
    /* EAGAIN is no error, only count actual packets as a reply */
    اگر(!qctx->got_پہلا_byte && nread) {
      qctx->got_پہلا_byte = TRUE;
      qctx->پہلا_byte_at = qctx->آخری_op;
    }
    qctx->آخری_io = qctx->آخری_op;
  }
  واپس نتیجہ;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(ساخت Curl_easy *ڈیٹا,
                      غیرعلامتی حرف *scid,
                      size_t scidlen,
                      صحیح *qlogfdp)
{
  حرف *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  اگر(qlog_dir) {
    ساخت dynbuf fname;
    CURLcode نتیجہ;
    غیرعلامتی صحیح i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    نتیجہ = curlx_dyn_جوڑو(&fname, qlog_dir);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_جوڑو(&fname, "/");
    برائے(i = 0; (i < scidlen) && !نتیجہ; i++) {
      حرف hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      نتیجہ = curlx_dyn_جوڑو(&fname, hex);
    }
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_جوڑو(&fname, ".sqlog");

    اگر(!نتیجہ) {
      صحیح qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_ثنائی,
#اگر_تعریف_شدہ _WIN32
                              ڈیٹا->سیٹ.new_file_perms & (_S_IREAD | _S_IWRITE)
#ورنہ
                              (mode_t)ڈیٹا->سیٹ.new_file_perms
#اگر_ختم
                              );
      اگر(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  واپس CURLE_OK;
}

CURLcode Curl_cf_quic_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_peer *origin,
                                   ساخت Curl_peer *peer)
{
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  (خالی)ڈیٹا; /* not used in all cases and compilers are stupid */
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  نتیجہ = Curl_cf_ngtcp2_داخل_کرو_after(cf_at, origin, peer, &cf);
#وگرنہ_اگر defined(USE_QUICHE)
  نتیجہ = Curl_cf_quiche_داخل_کرو_after(cf_at, origin, peer, &cf);
#ورنہ
  (خالی)cf)
  (خالی)cf_at;
  (خالی)origin;
  (خالی)peer;
  نتیجہ = CURLE_NOT_BUILT_اندر;
#اگر_ختم

#اگر defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  اگر(!نتیجہ && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    نتیجہ = Curl_conn_dns_جوڑو_https_resolve(ڈیٹا, cf->conn,
                                             cf->sockindex, origin);
  }
#اگر_ختم /* USE_HTTPSRR && USE_ECH */
  واپس نتیجہ;
}

CURLcode Curl_cf_quic_create(ساخت Curl_cfilter **pcf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_peer *origin,
                             ساخت Curl_peer *peer,
                             uint8_t transport_peer,
                             ساخت connectdata *conn,
                             ساخت Curl_sockaddr_ex *addr,
                             ساخت Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (خالی)transport_peer;
  (خالی)tunnel_transport;
  (خالی)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  واپس Curl_cf_ngtcp2_create(pcf, ڈیٹا, origin, peer, conn, addr);
#وگرنہ_اگر defined(USE_QUICHE)
  واپس Curl_cf_quiche_create(pcf, ڈیٹا, origin, peer, conn, addr);
#ورنہ
  *pcf = تہی;
  (خالی)ڈیٹا;
  (خالی)origin;
  (خالی)peer;
  (خالی)conn;
  (خالی)addr;
  (خالی)tunnel_peer;
  (خالی)tunnel_transport;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

#اگر !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_peer *origin,
                                       ساخت Curl_peer *peer,
                                       ساخت Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  واپس Curl_cf_ngtcp2_proxy_داخل_کرو_after(cf_at, ڈیٹا, origin, peer,
                                           tunnel_peer, tunnel_transport);
#ورنہ
  (خالی)cf_at;
  (خالی)ڈیٹا;
  (خالی)origin;
  (خالی)peer;
  (خالی)tunnel_peer;
  (خالی)tunnel_transport;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

CURLcode Curl_cf_h3_proxy_create(ساخت Curl_cfilter **pcf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_peer *origin,
                                 ساخت Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ساخت connectdata *conn,
                                 ساخت Curl_sockaddr_ex *addr,
                                 ساخت Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  واپس Curl_cf_ngtcp2_proxy_create(pcf, ڈیٹا, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#ورنہ
  *pcf = تہی;
  (خالی)ڈیٹا;
  (خالی)conn;
  (خالی)addr;
  (خالی)peer;
  (خالی)transport_peer;
  (خالی)tunnel_peer;
  (خالی)tunnel_transport;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

#اگر_ختم /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn)
{
  اگر(Curl_conn_get_پہلا_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(ڈیٹا, "HTTP/3 cannot be used over UNIX domain sockets");
    واپس CURLE_QUIC_CONNECT_ERROR;
  }
  اگر(!(ڈیٹا->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(ڈیٹا, "HTTP/3 requested for non-HTTPS URL");
    واپس CURLE_URL_MALFORMAT;
  }
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(conn->socks_proxy.peer) {
    failf(ڈیٹا, "HTTP/3 is not supported over a SOCKS proxy");
    واپس CURLE_URL_MALFORMAT;
  }
#ورنہ
  (خالی)conn;
#اگر_ختم

  واپس CURLE_OK;
}

#اگر_تعریف_شدہ CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_رمز)
{
  اگر(error_رمز <= UINT_MAX) {
    انتخاب((غیرعلامتی صحیح)error_رمز) {
    حالت CURL_H3_ERR_NO_ERROR:
      واپس "NO_ERROR";
    حالت CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      واپس "GENERAL_PROTOCOL_ERROR";
    حالت CURL_H3_ERR_اندرونی_ERROR:
      واپس "INTERNAL_ERROR";
    حالت CURL_H3_ERR_دھارا_CREATION_ERROR:
      واپس "STREAM_CREATION_ERROR";
    حالت CURL_H3_ERR_CLOSED_CRITICAL_دھارا:
      واپس "CLOSED_CRITICAL_STREAM";
    حالت CURL_H3_ERR_چوکھٹا_UNEXPECTED:
      واپس "FRAME_UNEXPECTED";
    حالت CURL_H3_ERR_چوکھٹا_ERROR:
      واپس "FRAME_ERROR";
    حالت CURL_H3_ERR_EXCESSIVE_بھرو:
      واپس "EXCESSIVE_LOAD";
    حالت CURL_H3_ERR_ID_ERROR:
      واپس "ID_ERROR";
    حالت CURL_H3_ERR_SETTINGS_ERROR:
      واپس "SETTINGS_ERROR";
    حالت CURL_H3_ERR_MISSING_SETTINGS:
      واپس "MISSING_SETTINGS";
    حالت CURL_H3_ERR_REQUEST_REJECTED:
      واپس "REQUEST_REJECTED";
    حالت CURL_H3_ERR_REQUEST_CANCELLED:
      واپس "REQUEST_CANCELLED";
    حالت CURL_H3_ERR_REQUEST_INCOMPLETE:
      واپس "REQUEST_INCOMPLETE";
    حالت CURL_H3_ERR_پیغام_ERROR:
      واپس "MESSAGE_ERROR";
    حالت CURL_H3_ERR_CONNECT_ERROR:
      واپس "CONNECT_ERROR";
    حالت CURL_H3_ERR_نسخہ_FALLBACK:
      واپس "VERSION_FALLBACK";
    طےشدہ:
      توڑو;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  اگر((error_رمز >= 0x21) && !((error_رمز - 0x21) % 0x1f))
    واپس "NO_ERROR";
  واپس "unknown";
}
#اگر_ختم /* CURLVERBOSE */

#اگر defined(USE_NGTCP2) || defined(USE_NGHTTP3)

ساکن خالی *vquic_ngtcp2_malloc(size_t سائز, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  واپس Curl_cmalloc(سائز);
}

ساکن خالی vquic_ngtcp2_free(خالی *ptr, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  Curl_cfree(ptr);
}

ساکن خالی *vquic_ngtcp2_calloc(size_t nmemb, size_t سائز, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  واپس Curl_ccalloc(nmemb, سائز);
}

ساکن خالی *vquic_ngtcp2_realloc(خالی *ptr, size_t سائز, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  واپس Curl_crealloc(ptr, سائز);
}

#اگر_تعریف_شدہ USE_NGTCP2
ساکن ساخت ngtcp2_mem vquic_ngtcp2_mem = {
  تہی,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ساخت ngtcp2_mem *Curl_ngtcp2_mem(خالی)
{
  واپس &vquic_ngtcp2_mem;
}
#اگر_ختم

#اگر_تعریف_شدہ USE_NGHTTP3
ساکن ساخت nghttp3_mem vquic_nghttp3_mem = {
  تہی,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
ساخت nghttp3_mem *Curl_nghttp3_mem(خالی)
{
  واپس &vquic_nghttp3_mem;
}
#اگر_ختم

#اگر_ختم /* USE_NGTCP2 || USE_NGHTTP3 */

#ورنہ /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn)
{
  (خالی)ڈیٹا;
  (خالی)conn;
  DEBUGF(infof(ڈیٹا, "QUIC is not supported in this build"));
  واپس CURLE_NOT_BUILT_اندر;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_HTTP3 */
