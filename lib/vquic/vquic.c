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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#यदि_परिभाषितम् HAVE_NETINET_UDP_H
#अन्तर्भाव <netinet/udp.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETINET_IP_H
#अन्तर्भाव <netinet/ip.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_NGHTTP3
#अन्तर्भाव <nghttp3/nghttp3.h>
#यदि_अन्तः

#यदि defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#अन्तर्भाव <sys/syscall.h>
#यदि defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#परिभाषय HAVE_APPLE_सन्देशः_X
#यदि_अन्तः
#यदि_अन्तः

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


#परिभाषय NW_CHUNK_आकार     (64 * 1024)
#परिभाषय NW_SEND_CHUNKS    1

#यदि_परिभाषितम् HAVE_APPLE_सन्देशः_X

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
#यदि_अन्तः

#यदि_परिभाषितम् CURLVERBOSE
#यदि_परिभाषितम् HAVE_APPLE_सन्देशः_X
#परिभाषय VQUIC_SEND_विधानम्   "sendmsg_x"
#नोचेद्यदि defined(HAVE_SENDMSG)
#परिभाषय VQUIC_SEND_विधानम्   "sendmsg"
#अन्यथा
#परिभाषय VQUIC_SEND_विधानम्   "send"
#यदि_अन्तः
#यदि_अन्तः

पूर्णाङ्क Curl_vquic_init(शून्य)
{
#यदि defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  यदि(ngtcp2_crypto_ossl_init())
    प्रत्यागम 0;
#यदि_अन्तः

  प्रत्यागम 1;
}

शून्य Curl_vquic_cleanup(शून्य)
{
#यदि defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_संस्करणम्_अङ्कः >= 0x011800)
  ngtcp2_crypto_ossl_free();
#यदि_अन्तः
}

शून्य Curl_quic_ver(वर्ण *p, size_t len)
{
#यदि defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#नोचेद्यदि defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#यदि_अन्तः
}

CURLcode Curl_vquic_ctx_init(संरचना Curl_easy *दत्त,
                             संरचना cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_आकार, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_सीमा);
#यदि defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#नोचेद्यदि defined(HAVE_APPLE_सन्देशः_X)
  qctx->no_gso = FALSE;
#अन्यथा
  qctx->no_gso = TRUE;
#यदि_अन्तः
#यदि_परिभाषितम् DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_QUIC_WBLOCK");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, 100))
        qctx->wblock_percent = (पूर्णाङ्क)l;
    }
  }
#यदि_अन्तः
  Curl_vquic_ctx_समुच्चयः_time(qctx, Curl_pgrs_now(दत्त));

  प्रत्यागम CURLE_OK;
}

शून्य Curl_vquic_ctx_free(संरचना cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

शून्य Curl_vquic_ctx_समुच्चयः_time(संरचना cf_quic_ctx *qctx,
                             ध्रुव संरचना curltime *pnow)
{
  qctx->चरमः_op = *pnow;
}

शून्य Curl_vquic_ctx_update_time(संरचना cf_quic_ctx *qctx,
                                ध्रुव संरचना curltime *pnow)
{
  qctx->चरमः_op = *pnow;
}

#यदि_परिभाषितम् HAVE_APPLE_सन्देशः_X
स्थिर CURLcode do_sendmsg(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           संरचना cf_quic_ctx *qctx,
                           ध्रुव uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
#परिभाषय सन्देशः_X_SNUM  64
  संरचना iovec msg_iov[सन्देशः_X_SNUM];
  संरचना msghdr_x mmsg[सन्देशः_X_SNUM];
  वर्ण errstr[STRERROR_LEN];
  size_t n, i = 0, प्रेषितम् = 0;
  पूर्णाङ्क rc;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;
  यदि(!pktlen)
    प्रत्यागम CURLE_OK;
  यदि(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  यावत्(i < n) {
    size_t j, batch = CURLMIN(n - i, सन्देशः_X_SNUM), pkts_प्रेषितम् = 0;

    प्रति(j = 0; j < batch; ++j) {
      ध्रुव size_t विस्थापनम् = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + विस्थापनम्);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - विस्थापनम्);
      स्मृतिं_पूरय(&mmsg[j], 0, परिमाण(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].सन्देशः_iovlen = 1;
      mmsg[j].सन्देशः_datalen = msg_iov[j].iov_len;
    }

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
#यदि_अन्तः
    यावत्((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

    यदि(rc < 0) {
      यदि(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(दत्त, cf, "egress, sendmsg_x -> EAGAIN");
        परिणाम = प्रेषितम् ? CURLE_OK : CURLE_AGAIN;
        गच्छ बहिर्गमनम्;
      }
      यदि(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
        failf(दत्त, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        परिणाम = CURLE_SEND_ERROR;
        गच्छ बहिर्गमनम्;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_प्रेषितम् = batch;
    }
    अन्यथा
      pkts_प्रेषितम् = CURLMIN((size_t)rc, batch);

    VERBOSE(++calls);
    यदि(!pkts_प्रेषितम्) {  /* no packets of the current batch were sent */
      परिणाम = प्रेषितम् ? CURLE_OK : CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    }
    i += pkts_प्रेषितम्; /* Some have been sent */
    प्रति(j = 0; j < pkts_प्रेषितम्; ++j)
      प्रेषितम् += msg_iov[j].iov_len;
    यदि(pkts_प्रेषितम् < batch)
      गच्छ बहिर्गमनम्; /* but not all of them */
  }

बहिर्गमनम्:
  *psent = प्रेषितम्;
  यदि(प्रेषितम् || परिणाम)
    CURL_TRC_CF(दत्त, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                प्रेषितम्, gsolen, n, calls, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

#अन्यथा /* HAVE_APPLE_MSG_X */

स्थिर CURLcode send_packet_no_gso(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना cf_quic_ctx *qctx,
                                   ध्रुव uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent);

स्थिर CURLcode do_sendmsg(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           संरचना cf_quic_ctx *qctx,
                           ध्रुव uint8_t *pkt, size_t pktlen, size_t gsolen,
                           size_t *psent)
{
  CURLcode परिणाम = CURLE_OK;
#यदि_परिभाषितम् HAVE_SENDMSG
  संरचना iovec msg_iov;
  संरचना msghdr सन्देशः = { 0 };
  ssize_t rv;
#यदि defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t सन्देशः_ctrl[32];
  संरचना cmsghdr *cm;
#यदि_अन्तः

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  सन्देशः.msg_iov = &msg_iov;
  सन्देशः.सन्देशः_iovlen = 1;

#यदि defined(__linux__) && defined(UDP_SEGMENT)
  यदि(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    स्मृतिं_पूरय(सन्देशः_ctrl, 0, परिमाण(सन्देशः_ctrl));
    सन्देशः.सन्देशः_control = सन्देशः_ctrl;
    DEBUGASSERT(परिमाण(सन्देशः_ctrl) >= CMSG_SPACE(परिमाण(पूर्णाङ्क)));
    सन्देशः.msg_controllen = CMSG_SPACE(परिमाण(पूर्णाङ्क));
    cm = CMSG_FIRSTHDR(&सन्देशः);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(परिमाण(uint16_t));
    *(uint16_t *)(शून्य *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#यदि_अन्तः

  यावत्((rv = sendmsg(qctx->sockfd, &सन्देशः, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  यदि(!curlx_sztouz(rv, psent)) {
    पूर्णाङ्क sockerr = SOCKERRNO;
    यदि(SOCK_EAGAIN(sockerr))
      प्रत्यागम CURLE_AGAIN;
    विकल्प(sockerr) {
    पक्ष SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      भङ्ग;
    पक्ष EIO:
      यदि(pktlen > gsolen) {
        /* GSO failure */
        infof(दत्त, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        प्रत्यागम send_packet_no_gso(cf, दत्त, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    स्वभाव:
      failf(दत्त, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा यदि(pktlen != *psent) {
    failf(दत्त, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
#अन्यथा
  ssize_t rv;
  (शून्य)gsolen;

  *psent = 0;

  यावत्((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  यदि(!curlx_sztouz(rv, psent)) {
    यदि(SOCK_EAGAIN(SOCKERRNO)) {
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा {
      यदि(SOCKERRNO != SOCKEMSGSIZE) {
        failf(दत्त, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        परिणाम = CURLE_SEND_ERROR;
        गच्छ बहिर्गमनम्;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#यदि_अन्तः
  (शून्य)cf;

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_विधानम्, pktlen, gsolen, (पूर्णाङ्क)परिणाम, *psent);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !HAVE_APPLE_MSG_X */

स्थिर CURLcode send_packet_no_gso(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना cf_quic_ctx *qctx,
                                   ध्रुव uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  ध्रुव uint8_t *p, *अन्तः = pkt + pktlen;
  size_t प्रेषितम्, len;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  प्रति(p = pkt; p < अन्तः; p += gsolen) {
    len = CURLMIN(gsolen, (size_t)(अन्तः - p));
    परिणाम = do_sendmsg(cf, दत्त, qctx, p, len, len, &प्रेषितम्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    *psent += प्रेषितम्;
    VERBOSE(++calls);
  }
बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_विधानम्, pktlen, gsolen, calls, (पूर्णाङ्क)परिणाम, *psent);
  प्रत्यागम परिणाम;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
स्थिर CURLcode send_packet_no_gso_cf(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      ध्रुव uint8_t *pkt, size_t pktlen,
                                      size_t gsolen, size_t *psent)
{
  ध्रुव uint8_t *p, *अन्तः = pkt + pktlen;
  size_t प्रेषितम्, len;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(size_t calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  प्रति(p = pkt; p < अन्तः; p += len) {
    len = CURLMIN(gsolen, (size_t)(अन्तः - p));
    परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, p, len, FALSE, &प्रेषितम्);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    यदि(प्रेषितम् != len) {
      /* We can only send the complete datagram, not parts. */
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    *psent += प्रेषितम्;
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (पूर्णाङ्क)परिणाम, *psent);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode vquic_send_packets(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना cf_quic_ctx *qctx,
                                   ध्रुव uint8_t *pkt, size_t pktlen,
                                   size_t gsolen, size_t *psent)
{
  CURLcode परिणाम;
#यदि_परिभाषितम् DEBUGBUILD
  /* simulate network blocking/partial writes */
  यदि(qctx->wblock_percent > 0) {
    अचिह्नित वर्ण c;
    *psent = 0;
    Curl_rand(दत्त, &c, 1);
    यदि(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(दत्त, cf, "vquic_flush() simulate EWOULDBLOCK");
      प्रत्यागम CURLE_AGAIN;
    }
  }
#यदि_अन्तः
  यदि(qctx->no_gso && pktlen > gsolen) {
    परिणाम = send_packet_no_gso(cf, दत्त, qctx, pkt, pktlen, gsolen, psent);
  }
  अन्यथा {
    परिणाम = do_sendmsg(cf, दत्त, qctx, pkt, pktlen, gsolen, psent);
  }
  यदि(!परिणाम)
    qctx->चरमः_io = qctx->चरमः_op;
  प्रत्यागम परिणाम;
}

CURLcode Curl_vquic_flush(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          संरचना cf_quic_ctx *qctx)
{
  ध्रुव अचिह्नित वर्ण *buf;
  size_t blen, प्रेषितम्;
  CURLcode परिणाम;
  size_t gsolen;

  यावत्(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    यदि(qctx->विभज_len) {
      gsolen = qctx->विभज_gsolen;
      यदि(blen > qctx->विभज_len)
        blen = qctx->विभज_len;
    }

    यदि(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      परिणाम = vquic_send_packets(cf, दत्त, qctx, buf, blen, gsolen, &प्रेषितम्);
    }
    अन्यथा {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      यदि(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        परिणाम = send_packet_no_gso_cf(cf, दत्त, buf, blen, gsolen, &प्रेषितम्);
      }
      अन्यथा {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, buf, blen, FALSE, &प्रेषितम्);
      }
    }

    यदि(परिणाम) {
      यदि(परिणाम == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, प्रेषितम्);
        यदि(qctx->विभज_len)
          qctx->विभज_len -= प्रेषितम्;
      }
      प्रत्यागम परिणाम;
    }
    Curl_bufq_skip(&qctx->sendbuf, प्रेषितम्);
    यदि(qctx->विभज_len)
      qctx->विभज_len -= प्रेषितम्;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_vquic_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                         संरचना cf_quic_ctx *qctx, size_t gsolen)
{
  qctx->gsolen = gsolen;
  प्रत्यागम Curl_vquic_flush(cf, दत्त, qctx);
}

CURLcode Curl_vquic_send_पुच्छ_विभज(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना cf_quic_ctx *qctx, size_t gsolen,
                                    size_t पुच्छ_len, size_t पुच्छ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > पुच्छ_len);
  qctx->विभज_len = Curl_bufq_len(&qctx->sendbuf) - पुच्छ_len;
  qctx->विभज_gsolen = gsolen;
  qctx->gsolen = पुच्छ_gsolen;
  CURL_TRC_CF(दत्त, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->विभज_len, qctx->विभज_gsolen, पुच्छ_len, qctx->gsolen);
  प्रत्यागम Curl_vquic_flush(cf, दत्त, qctx);
}

#यदि (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_सन्देशः_X)
स्थिर size_t vquic_msghdr_get_udp_gro(संरचना msghdr *सन्देशः)
{
  पूर्णाङ्क gso_आकार = 0;
#यदि defined(__linux__) && defined(UDP_GRO)
  संरचना cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#यदि defined(__clang__) && !defined(__GLIBC__)
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wsign-compare"
#निर्देशः clang diagnostic ignored "-Wcast-align"
#यदि_अन्तः
  प्रति(cmsg = CMSG_FIRSTHDR(सन्देशः); cmsg; cmsg = CMSG_NXTHDR(सन्देशः, cmsg)) {
#यदि defined(__clang__) && !defined(__GLIBC__)
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
    यदि(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      स्मृतिं_प्रतिलिख(&gso_आकार, CMSG_DATA(cmsg), परिमाण(gso_आकार));

      भङ्ग;
    }
  }
#यदि_अन्तः /* linux && UDP_GRO */
  (शून्य)सन्देशः;

  प्रत्यागम (size_t)gso_आकार;
}
#यदि_अन्तः /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#यदि (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_सन्देशः_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_आवरणम्)
स्थिर uint8_t vquic_msghdr_get_ecn(संरचना msghdr *सन्देशः, पूर्णाङ्क family)
{
  संरचना cmsghdr *cmsg;
  विकल्प(family) {
  पक्ष AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#यदि defined(__clang__) && !defined(__GLIBC__)
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wsign-compare"
#निर्देशः clang diagnostic ignored "-Wcast-align"
#यदि_अन्तः
    प्रति(cmsg = CMSG_FIRSTHDR(सन्देशः); cmsg; cmsg = CMSG_NXTHDR(सन्देशः, cmsg)) {
#यदि defined(__clang__) && !defined(__GLIBC__)
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
      यदि(cmsg->cmsg_level == IPPROTO_IP &&
#यदि_परिभाषितम् __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#अन्यथा
          cmsg->cmsg_type == IP_TOS
#यदि_अन्तः
          && cmsg->cmsg_len) {
        प्रत्यागम *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_आवरणम्;
      }
    }
    भङ्ग;
  पक्ष AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#यदि defined(__clang__) && !defined(__GLIBC__)
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wsign-compare"
#निर्देशः clang diagnostic ignored "-Wcast-align"
#यदि_अन्तः
    प्रति(cmsg = CMSG_FIRSTHDR(सन्देशः); cmsg; cmsg = CMSG_NXTHDR(सन्देशः, cmsg)) {
#यदि defined(__clang__) && !defined(__GLIBC__)
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
      यदि(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        अचिह्नित पूर्णाङ्क tos;

        स्मृतिं_प्रतिलिख(&tos, CMSG_DATA(cmsg), परिमाण(पूर्णाङ्क));

        प्रत्यागम (uint8_t)(tos & IPTOS_ECN_आवरणम्);
      }
    }
    भङ्ग;
  }
  प्रत्यागम 0;
}
#अन्यथा
#परिभाषय vquic_msghdr_get_ecn(a, b)      0
#यदि_अन्तः /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#यदि_परिभाषितम् HAVE_SENDMMSG

स्थिर CURLcode recvmmsg_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                 size_t *pnread)
{
#यदि defined(__linux__) && defined(UDP_GRO)
#परिभाषय MMSG_अङ्कः  16
#परिभाषय UDP_GRO_CNT_MAX  64
#परिभाषय CMSG_प्रत्येकम्_सन्देशः_आकार    (2 * CMSG_SPACE(परिमाण(पूर्णाङ्क)))
#अन्यथा
#परिभाषय MMSG_अङ्कः  64
#परिभाषय UDP_GRO_CNT_MAX  1
#परिभाषय CMSG_प्रत्येकम्_सन्देशः_आकार    CMSG_SPACE(परिमाण(पूर्णाङ्क))
#यदि_अन्तः
#परिभाषय सन्देशः_BUF_आकार  (UDP_GRO_CNT_MAX * 1500)
  संरचना iovec msg_iov[MMSG_अङ्कः];
  संरचना mmsghdr mmsg[MMSG_अङ्कः];
  uint8_t सन्देशः_ctrl[MMSG_अङ्कः * CMSG_प्रत्येकम्_सन्देशः_आकार];
  संरचना sockaddr_storage remote_addr[MMSG_अङ्कः];
  size_t योग_nread = 0, pkts = 0;
#यदि_परिभाषितम् CURLVERBOSE
  size_t calls = 0;
#यदि_अन्तः
  पूर्णाङ्क mcount, i, n;
  वर्ण errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;
  size_t gso_आकार;
  वर्ण *sockbuf = रिक्त;
  uint8_t (*bufs)[सन्देशः_BUF_आकार] = रिक्त;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  परिणाम = Curl_multi_xfer_sockbuf_borrow(दत्त, MMSG_अङ्कः * सन्देशः_BUF_आकार,
                                          &sockbuf);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  bufs = (uint8_t (*)[सन्देशः_BUF_आकार])sockbuf;

  योग_nread = 0;
  यावत्(pkts < max_pkts) {
    n = (पूर्णाङ्क)CURLMIN(CURLMIN(MMSG_अङ्कः, IOV_MAX), max_pkts);
    स्मृतिं_पूरय(&mmsg, 0, परिमाण(mmsg));
    प्रति(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = परिमाण(bufs[i]);
      mmsg[i].सन्देशः_hdr.msg_iov = &msg_iov[i];
      mmsg[i].सन्देशः_hdr.सन्देशः_iovlen = 1;
      mmsg[i].सन्देशः_hdr.सन्देशः_name = &remote_addr[i];
      mmsg[i].सन्देशः_hdr.msg_namelen = परिमाण(remote_addr[i]);
      mmsg[i].सन्देशः_hdr.सन्देशः_control = &सन्देशः_ctrl[i * CMSG_प्रत्येकम्_सन्देशः_आकार];
      mmsg[i].सन्देशः_hdr.msg_controllen = CMSG_प्रत्येकम्_सन्देशः_आकार;
    }

    यावत्((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, रिक्त)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    यदि(mcount == -1) {
      यदि(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(दत्त, cf, "ingress, recvmmsg -> EAGAIN");
        गच्छ बहिर्गमनम्;
      }
      यदि(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        यदि(!Curl_cf_socket_peek(cf->अग्र, दत्त, रिक्त, रिक्त, &ip))
          failf(दत्त, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_पत्तनम्);
        परिणाम = CURLE_COULDNT_CONNECT;
        गच्छ बहिर्गमनम्;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(दत्त, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }

    VERBOSE(++calls);
    प्रति(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      यदि(!mmsg[i].सन्देशः_len) {
        ++pkts;
        अनुवर्त;
      }
      योग_nread += mmsg[i].सन्देशः_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].सन्देशः_hdr, remote_addr[i].ss_family);
      gso_आकार = vquic_msghdr_get_udp_gro(&mmsg[i].सन्देशः_hdr);
      यदि(gso_आकार == 0)
        gso_आकार = mmsg[i].सन्देशः_len;

      परिणाम = recv_cb(bufs[i], mmsg[i].सन्देशः_len, gso_आकार,
                       mmsg[i].सन्देशः_hdr.सन्देशः_name,
                       mmsg[i].सन्देशः_hdr.msg_namelen, ecn, userp);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      pkts += (mmsg[i].सन्देशः_len + gso_आकार - 1) / gso_आकार;
    }
  }

बहिर्गमनम्:
  यदि(योग_nread || परिणाम)
    CURL_TRC_CF(दत्त, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णाङ्क)परिणाम);
  Curl_multi_xfer_sockbuf_release(दत्त, sockbuf);
  *pnread = योग_nread;
  प्रत्यागम परिणाम;
}

#नोचेद्यदि defined(HAVE_APPLE_सन्देशः_X)

स्थिर CURLcode recvmsg_x_packets(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना cf_quic_ctx *qctx,
                                  size_t max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  शून्य *userp, size_t *pnread)
{
#परिभाषय सन्देशः_X_अङ्कः  64
#परिभाषय सन्देशः_BUF_आकार  (2048)
#परिभाषय CMSG_प्रत्येकम्_सन्देशः_आकार     CMSG_SPACE(परिमाण(पूर्णाङ्क))
  संरचना iovec msg_iov[सन्देशः_X_अङ्कः];
  संरचना msghdr_x mmsg[सन्देशः_X_अङ्कः];
  uint8_t सन्देशः_ctrl[सन्देशः_X_अङ्कः * CMSG_प्रत्येकम्_सन्देशः_आकार];
  संरचना sockaddr_storage remote_addr[सन्देशः_X_अङ्कः];
  size_t योग_nread = 0, pkts = 0;
#यदि_परिभाषितम् CURLVERBOSE
  size_t calls = 0;
#यदि_अन्तः
  पूर्णाङ्क mcount, i;
  वर्ण errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;
  size_t gso_आकार;
  वर्ण *sockbuf = रिक्त;
  uint8_t (*bufs)[सन्देशः_BUF_आकार] = रिक्त;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  परिणाम = Curl_multi_xfer_sockbuf_borrow(दत्त, सन्देशः_X_अङ्कः * सन्देशः_BUF_आकार,
                                          &sockbuf);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  bufs = (uint8_t (*)[सन्देशः_BUF_आकार])sockbuf;

  योग_nread = 0;
  यावत्(pkts < max_pkts) {
    पूर्णाङ्क n = (पूर्णाङ्क)CURLMIN(CURLMIN(सन्देशः_X_अङ्कः, IOV_MAX), max_pkts);
    स्मृतिं_पूरय(&mmsg, 0, परिमाण(mmsg));
    प्रति(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = परिमाण(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].सन्देशः_iovlen = 1;
      mmsg[i].सन्देशः_name = &remote_addr[i];
      mmsg[i].msg_namelen = परिमाण(remote_addr[i]);
      mmsg[i].सन्देशः_control = &सन्देशः_ctrl[i * CMSG_प्रत्येकम्_सन्देशः_आकार];
      mmsg[i].msg_controllen = CMSG_प्रत्येकम्_सन्देशः_आकार;
    }

#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
#यदि_अन्तः
    यावत्((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#यदि defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
    यदि(mcount == -1) {
      यदि(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(दत्त, cf, "ingress, recvmsg_x -> EAGAIN");
        गच्छ बहिर्गमनम्;
      }
      यदि(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        यदि(!Curl_cf_socket_peek(cf->अग्र, दत्त, रिक्त, रिक्त, &ip))
          failf(दत्त, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_पत्तनम्);
        परिणाम = CURLE_COULDNT_CONNECT;
        गच्छ बहिर्गमनम्;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(दत्त, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }

    VERBOSE(++calls);
    प्रति(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      यदि(!mmsg[i].सन्देशः_datalen) {
        ++pkts;
        अनुवर्त;
      }
      योग_nread += mmsg[i].सन्देशः_datalen;
      ecn = vquic_msghdr_get_ecn((संरचना msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_आकार = mmsg[i].सन्देशः_datalen;

      परिणाम = recv_cb(bufs[i], mmsg[i].सन्देशः_datalen, gso_आकार,
                       mmsg[i].सन्देशः_name,
                       mmsg[i].msg_namelen, ecn, userp);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      pkts += (mmsg[i].सन्देशः_datalen + gso_आकार - 1) / gso_आकार;
    }
  }

बहिर्गमनम्:
  यदि(योग_nread || परिणाम)
    CURL_TRC_CF(दत्त, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णाङ्क)परिणाम);
  Curl_multi_xfer_sockbuf_release(दत्त, sockbuf);
  *pnread = योग_nread;
  प्रत्यागम परिणाम;
}

#नोचेद्यदि defined(HAVE_SENDMSG)
स्थिर CURLcode recvmsg_packets(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                संरचना cf_quic_ctx *qctx,
                                size_t max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                size_t *pnread)
{
#परिभाषय CMSG_प्रत्येकम्_सन्देशः_आकार    CMSG_SPACE(परिमाण(पूर्णाङ्क))
  संरचना iovec msg_iov;
  संरचना msghdr सन्देशः;
  uint8_t buf[64 * 1024];
  संरचना sockaddr_storage remote_addr;
  size_t योग_nread, pkts, calls;
  ssize_t rc;
  size_t nread;
  वर्ण errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;
  uint8_t सन्देशः_ctrl[CMSG_प्रत्येकम्_सन्देशः_आकार];
  size_t gso_आकार;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  प्रति(pkts = 0, योग_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    स्मृतिं_पूरय(&सन्देशः, 0, परिमाण(सन्देशः));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = परिमाण(buf);
    सन्देशः.msg_iov = &msg_iov;
    सन्देशः.सन्देशः_iovlen = 1;
    सन्देशः.सन्देशः_control = सन्देशः_ctrl;
    सन्देशः.सन्देशः_name = &remote_addr;
    सन्देशः.msg_namelen = परिमाण(remote_addr);
    सन्देशः.msg_controllen = CMSG_प्रत्येकम्_सन्देशः_आकार;

    यावत्((rc = recvmsg(qctx->sockfd, &सन्देशः, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    यदि(!curlx_sztouz(rc, &nread)) {
      यदि(SOCK_EAGAIN(SOCKERRNO)) {
        गच्छ बहिर्गमनम्;
      }
      यदि(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        यदि(!Curl_cf_socket_peek(cf->अग्र, दत्त, रिक्त, रिक्त, &ip))
          failf(दत्त, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_पत्तनम्);
        परिणाम = CURLE_COULDNT_CONNECT;
        गच्छ बहिर्गमनम्;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(दत्त, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }

    योग_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    यदि(!nread) {
      ++pkts;
      अनुवर्त;
    }

    ecn = vquic_msghdr_get_ecn(&सन्देशः, remote_addr.ss_family);
    gso_आकार = vquic_msghdr_get_udp_gro(&सन्देशः);
    यदि(gso_आकार == 0)
      gso_आकार = nread;

    परिणाम = recv_cb(buf, nread, gso_आकार,
                     सन्देशः.सन्देशः_name, सन्देशः.msg_namelen, ecn, userp);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    pkts += (nread + gso_आकार - 1) / gso_आकार;
  }

बहिर्गमनम्:
  यदि(योग_nread || परिणाम)
    CURL_TRC_CF(दत्त, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णाङ्क)परिणाम);
  *pnread = योग_nread;
  प्रत्यागम परिणाम;
}

#अन्यथा /* HAVE_SENDMMSG || HAVE_SENDMSG */
स्थिर CURLcode recvfrom_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                 size_t *pnread)
{
  uint8_t buf[64 * 1024];
  पूर्णाङ्क bufsize = (पूर्णाङ्क)परिमाण(buf);
  संरचना sockaddr_storage remote_addr;
  socklen_t remote_addrlen = परिमाण(remote_addr);
  size_t योग_nread, pkts, calls = 0, nread;
  ssize_t rv;
  वर्ण errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  प्रति(pkts = 0, योग_nread = 0; pkts < max_pkts;) {
    यावत्((rv = recvfrom(qctx->sockfd, (वर्ण *)buf, bufsize, 0,
                         (संरचना sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    यदि(!curlx_sztouz(rv, &nread)) {
      यदि(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(दत्त, cf, "ingress, recvfrom -> EAGAIN");
        गच्छ बहिर्गमनम्;
      }
      यदि(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        यदि(!Curl_cf_socket_peek(cf->अग्र, दत्त, रिक्त, रिक्त, &ip))
          failf(दत्त, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_पत्तनम्);
        परिणाम = CURLE_COULDNT_CONNECT;
        गच्छ बहिर्गमनम्;
      }
      curlx_strerror(SOCKERRNO, errstr, परिमाण(errstr));
      failf(दत्त, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    यदि(!nread)
      अनुवर्त;

    योग_nread += nread;
    परिणाम = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

बहिर्गमनम्:
  यदि(योग_nread || परिणाम)
    CURL_TRC_CF(दत्त, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णाङ्क)परिणाम);
  *pnread = योग_nread;
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp)
{
  CURLcode परिणाम;
  size_t nread = 0;
#यदि_परिभाषितम् HAVE_SENDMMSG
  परिणाम = recvmmsg_packets(cf, दत्त, qctx, max_pkts, recv_cb, userp, &nread);
#नोचेद्यदि defined(HAVE_APPLE_सन्देशः_X)
  परिणाम = recvmsg_x_packets(cf, दत्त, qctx, max_pkts, recv_cb, userp,
                             &nread);
#नोचेद्यदि defined(HAVE_SENDMSG)
  परिणाम = recvmsg_packets(cf, दत्त, qctx, max_pkts, recv_cb, userp, &nread);
#अन्यथा
  परिणाम = recvfrom_packets(cf, दत्त, qctx, max_pkts, recv_cb, userp, &nread);
#यदि_अन्तः
  यदि(!परिणाम) {
    /* EAGAIN is no error, only count actual packets as a reply */
    यदि(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->चरमः_op;
    }
    qctx->चरमः_io = qctx->चरमः_op;
  }
  प्रत्यागम परिणाम;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(संरचना Curl_easy *दत्त,
                      अचिह्नित वर्ण *scid,
                      size_t scidlen,
                      पूर्णाङ्क *qlogfdp)
{
  वर्ण *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  यदि(qlog_dir) {
    संरचना dynbuf fname;
    CURLcode परिणाम;
    अचिह्नित पूर्णाङ्क i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    परिणाम = curlx_dyn_सङ्कलय(&fname, qlog_dir);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_सङ्कलय(&fname, "/");
    प्रति(i = 0; (i < scidlen) && !परिणाम; i++) {
      वर्ण hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      परिणाम = curlx_dyn_सङ्कलय(&fname, hex);
    }
    यदि(!परिणाम)
      परिणाम = curlx_dyn_सङ्कलय(&fname, ".sqlog");

    यदि(!परिणाम) {
      पूर्णाङ्क qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_द्विमान,
#यदि_परिभाषितम् _WIN32
                              दत्त->समुच्चयः.new_file_perms & (_S_IREAD | _S_IWRITE)
#अन्यथा
                              (mode_t)दत्त->समुच्चयः.new_file_perms
#यदि_अन्तः
                              );
      यदि(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cf_quic_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                   संरचना Curl_easy *दत्त,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode परिणाम;

  (शून्य)दत्त; /* not used in all cases and compilers are stupid */
#यदि defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  परिणाम = Curl_cf_ngtcp2_निवेशय_after(cf_at, origin, peer, &cf);
#नोचेद्यदि defined(USE_QUICHE)
  परिणाम = Curl_cf_quiche_निवेशय_after(cf_at, origin, peer, &cf);
#अन्यथा
  (शून्य)cf)
  (शून्य)cf_at;
  (शून्य)origin;
  (शून्य)peer;
  परिणाम = CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः

#यदि defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  यदि(!परिणाम && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    परिणाम = Curl_conn_dns_सङ्कलय_https_resolve(दत्त, cf->conn,
                                             cf->sockindex, origin);
  }
#यदि_अन्तः /* USE_HTTPSRR && USE_ECH */
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_quic_create(संरचना Curl_cfilter **pcf,
                             संरचना Curl_easy *दत्त,
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
#यदि defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  प्रत्यागम Curl_cf_ngtcp2_create(pcf, दत्त, origin, peer, conn, addr);
#नोचेद्यदि defined(USE_QUICHE)
  प्रत्यागम Curl_cf_quiche_create(pcf, दत्त, origin, peer, conn, addr);
#अन्यथा
  *pcf = रिक्त;
  (शून्य)दत्त;
  (शून्य)origin;
  (शून्य)peer;
  (शून्य)conn;
  (शून्य)addr;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

#यदि !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *दत्त,
                                       संरचना Curl_peer *origin,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#यदि defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  प्रत्यागम Curl_cf_ngtcp2_proxy_निवेशय_after(cf_at, दत्त, origin, peer,
                                           tunnel_peer, tunnel_transport);
#अन्यथा
  (शून्य)cf_at;
  (शून्य)दत्त;
  (शून्य)origin;
  (शून्य)peer;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

CURLcode Curl_cf_h3_proxy_create(संरचना Curl_cfilter **pcf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 uint8_t transport_peer,
                                 संरचना connectdata *conn,
                                 संरचना Curl_sockaddr_ex *addr,
                                 संरचना Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#यदि defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  प्रत्यागम Curl_cf_ngtcp2_proxy_create(pcf, दत्त, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#अन्यथा
  *pcf = रिक्त;
  (शून्य)दत्त;
  (शून्य)conn;
  (शून्य)addr;
  (शून्य)peer;
  (शून्य)transport_peer;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(संरचना Curl_easy *दत्त,
                             संरचना connectdata *conn)
{
  यदि(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(दत्त, "HTTP/3 cannot be used over UNIX domain sockets");
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }
  यदि(!(दत्त->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(दत्त, "HTTP/3 requested for non-HTTPS URL");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(conn->socks_proxy.peer) {
    failf(दत्त, "HTTP/3 is not supported over a SOCKS proxy");
    प्रत्यागम CURLE_URL_MALFORMAT;
  }
#अन्यथा
  (शून्य)conn;
#यदि_अन्तः

  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् CURLVERBOSE
ध्रुव वर्ण *Curl_vquic_h3_err_str(uint64_t error_संहिता)
{
  यदि(error_संहिता <= UINT_MAX) {
    विकल्प((अचिह्नित पूर्णाङ्क)error_संहिता) {
    पक्ष CURL_H3_ERR_NO_ERROR:
      प्रत्यागम "NO_ERROR";
    पक्ष CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      प्रत्यागम "GENERAL_PROTOCOL_ERROR";
    पक्ष CURL_H3_ERR_आन्तरिकम्_ERROR:
      प्रत्यागम "INTERNAL_ERROR";
    पक्ष CURL_H3_ERR_प्रवाहः_CREATION_ERROR:
      प्रत्यागम "STREAM_CREATION_ERROR";
    पक्ष CURL_H3_ERR_CLOSED_CRITICAL_प्रवाहः:
      प्रत्यागम "CLOSED_CRITICAL_STREAM";
    पक्ष CURL_H3_ERR_FRAME_UNEXPECTED:
      प्रत्यागम "FRAME_UNEXPECTED";
    पक्ष CURL_H3_ERR_FRAME_ERROR:
      प्रत्यागम "FRAME_ERROR";
    पक्ष CURL_H3_ERR_EXCESSIVE_आरोपय:
      प्रत्यागम "EXCESSIVE_LOAD";
    पक्ष CURL_H3_ERR_ID_ERROR:
      प्रत्यागम "ID_ERROR";
    पक्ष CURL_H3_ERR_SETTINGS_ERROR:
      प्रत्यागम "SETTINGS_ERROR";
    पक्ष CURL_H3_ERR_MISSING_SETTINGS:
      प्रत्यागम "MISSING_SETTINGS";
    पक्ष CURL_H3_ERR_REQUEST_REJECTED:
      प्रत्यागम "REQUEST_REJECTED";
    पक्ष CURL_H3_ERR_REQUEST_CANCELLED:
      प्रत्यागम "REQUEST_CANCELLED";
    पक्ष CURL_H3_ERR_REQUEST_INCOMPLETE:
      प्रत्यागम "REQUEST_INCOMPLETE";
    पक्ष CURL_H3_ERR_सन्देशः_ERROR:
      प्रत्यागम "MESSAGE_ERROR";
    पक्ष CURL_H3_ERR_CONNECT_ERROR:
      प्रत्यागम "CONNECT_ERROR";
    पक्ष CURL_H3_ERR_संस्करणम्_FALLBACK:
      प्रत्यागम "VERSION_FALLBACK";
    स्वभाव:
      भङ्ग;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  यदि((error_संहिता >= 0x21) && !((error_संहिता - 0x21) % 0x1f))
    प्रत्यागम "NO_ERROR";
  प्रत्यागम "unknown";
}
#यदि_अन्तः /* CURLVERBOSE */

#यदि defined(USE_NGTCP2) || defined(USE_NGHTTP3)

स्थिर शून्य *vquic_ngtcp2_malloc(size_t आकार, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  प्रत्यागम Curl_cmalloc(आकार);
}

स्थिर शून्य vquic_ngtcp2_free(शून्य *ptr, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  Curl_cfree(ptr);
}

स्थिर शून्य *vquic_ngtcp2_calloc(size_t nmemb, size_t आकार, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  प्रत्यागम Curl_ccalloc(nmemb, आकार);
}

स्थिर शून्य *vquic_ngtcp2_realloc(शून्य *ptr, size_t आकार, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  प्रत्यागम Curl_crealloc(ptr, आकार);
}

#यदि_परिभाषितम् USE_NGTCP2
स्थिर संरचना ngtcp2_mem vquic_ngtcp2_mem = {
  रिक्त,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
संरचना ngtcp2_mem *Curl_ngtcp2_mem(शून्य)
{
  प्रत्यागम &vquic_ngtcp2_mem;
}
#यदि_अन्तः

#यदि_परिभाषितम् USE_NGHTTP3
स्थिर संरचना nghttp3_mem vquic_nghttp3_mem = {
  रिक्त,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
संरचना nghttp3_mem *Curl_nghttp3_mem(शून्य)
{
  प्रत्यागम &vquic_nghttp3_mem;
}
#यदि_अन्तः

#यदि_अन्तः /* USE_NGTCP2 || USE_NGHTTP3 */

#अन्यथा /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(संरचना Curl_easy *दत्त,
                             संरचना connectdata *conn)
{
  (शून्य)दत्त;
  (शून्य)conn;
  DEBUGF(infof(दत्त, "QUIC is not supported in this build"));
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_HTTP3 */
