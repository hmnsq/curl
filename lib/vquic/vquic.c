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

#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#ifdef HAVE_NETINET_UDP_H
#अन्तर्भाव <netinet/udp.h>
#endif
#ifdef HAVE_NETINET_IP_H
#अन्तर्भाव <netinet/ip.h>
#endif

#ifdef USE_NGHTTP3
#अन्तर्भाव <nghttp3/nghttp3.h>
#endif

#जर defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#अन्तर्भाव <sys/syscall.h>
#जर defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#define HAVE_APPLE_संदेश_X
#endif
#endif

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


#define NW_CHUNK_SIZE     (64 * 1024)
#define NW_SEND_CHUNKS    1

#ifdef HAVE_APPLE_संदेश_X

/* this is `struct msghdr` with an additional field at the end */
संरचना msghdr_x {
  शून्य *संदेश_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  संरचना iovec *msg_iov;    /* scatter/gather array */
  पूर्णांक संदेश_iovlen;           /* # elements in msg_iov */
  शून्य *संदेश_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  पूर्णांक संदेश_flags;            /* flags on received message */
  माप_प्रकार संदेश_datalen;       /* byte length of buffer in msg_iov */
};
#endif

#ifdef CURLVERBOSE
#ifdef HAVE_APPLE_संदेश_X
#define VQUIC_SEND_METHOD   "sendmsg_x"
#किंवाजर defined(HAVE_SENDMSG)
#define VQUIC_SEND_METHOD   "sendmsg"
#अन्यथा
#define VQUIC_SEND_METHOD   "send"
#endif
#endif

पूर्णांक Curl_vquic_init(शून्य)
{
#जर defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  जर(ngtcp2_crypto_ossl_init())
    परत 0;
#endif

  परत 1;
}

शून्य Curl_vquic_cleanup(शून्य)
{
#जर defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_अंक >= 0x011800)
  ngtcp2_crypto_ossl_free();
#endif
}

शून्य Curl_quic_ver(अक्षर *p, माप_प्रकार len)
{
#जर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#किंवाजर defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#endif
}

CURLcode Curl_vquic_ctx_init(संरचना Curl_easy *data,
                             संरचना cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_SIZE, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#जर defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#किंवाजर defined(HAVE_APPLE_संदेश_X)
  qctx->no_gso = FALSE;
#अन्यथा
  qctx->no_gso = TRUE;
#endif
#ifdef DEBUGBUILD
  {
    स्थिरांक अक्षर *p = दो_पर्या("CURL_DBG_QUIC_WBLOCK");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, 100))
        qctx->wblock_percent = (पूर्णांक)l;
    }
  }
#endif
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(data));

  परत CURLE_OK;
}

शून्य Curl_vquic_ctx_free(संरचना cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

शून्य Curl_vquic_ctx_set_time(संरचना cf_quic_ctx *qctx,
                             स्थिरांक संरचना curltime *pnow)
{
  qctx->last_op = *pnow;
}

शून्य Curl_vquic_ctx_update_time(संरचना cf_quic_ctx *qctx,
                                स्थिरांक संरचना curltime *pnow)
{
  qctx->last_op = *pnow;
}

#ifdef HAVE_APPLE_संदेश_X
स्थिर CURLcode do_sendmsg(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना cf_quic_ctx *qctx,
                           स्थिरांक uint8_t *pkt, माप_प्रकार pktlen, माप_प्रकार gsolen,
                           माप_प्रकार *psent)
{
#define संदेश_X_SNUM  64
  संरचना iovec msg_iov[संदेश_X_SNUM];
  संरचना msghdr_x mmsg[संदेश_X_SNUM];
  अक्षर errstr[STRERROR_LEN];
  माप_प्रकार n, i = 0, sent = 0;
  पूर्णांक rc;
  CURLcode result = CURLE_OK;
  VERBOSE(माप_प्रकार calls = 0);

  *psent = 0;
  जर(!pktlen)
    परत CURLE_OK;
  जर(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  पर्यंत(i < n) {
    माप_प्रकार j, batch = CURLMIN(n - i, संदेश_X_SNUM), pkts_sent = 0;

    साठी(j = 0; j < batch; ++j) {
      स्थिरांक माप_प्रकार offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      स_रखो(&mmsg[j], 0, आकार(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].संदेश_iovlen = 1;
      mmsg[j].संदेश_datalen = msg_iov[j].iov_len;
    }

#जर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    पर्यंत((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#जर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic काढा
#endif

    जर(rc < 0) {
      जर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "egress, sendmsg_x -> EAGAIN");
        result = sent ? CURLE_OK : CURLE_AGAIN;
        जा out;
      }
      जर(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, आकार(errstr));
        failf(data, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        result = CURLE_SEND_ERROR;
        जा out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    अन्यथा
      pkts_sent = CURLMIN((माप_प्रकार)rc, batch);

    VERBOSE(++calls);
    जर(!pkts_sent) {  /* no packets of the current batch were sent */
      result = sent ? CURLE_OK : CURLE_AGAIN;
      जा out;
    }
    i += pkts_sent; /* Some have been sent */
    साठी(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    जर(pkts_sent < batch)
      जा out; /* but not all of them */
  }

out:
  *psent = sent;
  जर(sent || result)
    CURL_TRC_CF(data, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (पूर्णांक)result);
  परत result;
}

#अन्यथा /* HAVE_APPLE_MSG_X */

स्थिर CURLcode send_packet_no_gso(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना cf_quic_ctx *qctx,
                                   स्थिरांक uint8_t *pkt, माप_प्रकार pktlen,
                                   माप_प्रकार gsolen, माप_प्रकार *psent);

स्थिर CURLcode do_sendmsg(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना cf_quic_ctx *qctx,
                           स्थिरांक uint8_t *pkt, माप_प्रकार pktlen, माप_प्रकार gsolen,
                           माप_प्रकार *psent)
{
  CURLcode result = CURLE_OK;
#ifdef HAVE_SENDMSG
  संरचना iovec msg_iov;
  संरचना msghdr संदेश = { 0 };
  ssize_t rv;
#जर defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t संदेश_ctrl[32];
  संरचना cmsghdr *cm;
#endif

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  संदेश.msg_iov = &msg_iov;
  संदेश.संदेश_iovlen = 1;

#जर defined(__linux__) && defined(UDP_SEGMENT)
  जर(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    स_रखो(संदेश_ctrl, 0, आकार(संदेश_ctrl));
    संदेश.संदेश_control = संदेश_ctrl;
    DEBUGASSERT(आकार(संदेश_ctrl) >= CMSG_SPACE(आकार(पूर्णांक)));
    संदेश.msg_controllen = CMSG_SPACE(आकार(पूर्णांक));
    cm = CMSG_FIRSTHDR(&संदेश);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(आकार(uint16_t));
    *(uint16_t *)(शून्य *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#endif

  पर्यंत((rv = sendmsg(qctx->sockfd, &संदेश, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  जर(!curlx_sztouz(rv, psent)) {
    पूर्णांक sockerr = SOCKERRNO;
    जर(SOCK_EAGAIN(sockerr))
      परत CURLE_AGAIN;
    निवड(sockerr) {
    स्थिती SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      थांबा;
    स्थिती EIO:
      जर(pktlen > gsolen) {
        /* GSO failure */
        infof(data, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        परत send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    पूर्वनिर्धारित:
      failf(data, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      result = CURLE_SEND_ERROR;
      जा out;
    }
  }
  अन्यथा जर(pktlen != *psent) {
    failf(data, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    result = CURLE_SEND_ERROR;
    जा out;
  }
#अन्यथा
  ssize_t rv;
  (शून्य)gsolen;

  *psent = 0;

  पर्यंत((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  जर(!curlx_sztouz(rv, psent)) {
    जर(SOCK_EAGAIN(SOCKERRNO)) {
      result = CURLE_AGAIN;
      जा out;
    }
    अन्यथा {
      जर(SOCKERRNO != SOCKEMSGSIZE) {
        failf(data, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        result = CURLE_SEND_ERROR;
        जा out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#endif
  (शून्य)cf;

out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (पूर्णांक)result, *psent);
  परत result;
}

#endif /* !HAVE_APPLE_MSG_X */

स्थिर CURLcode send_packet_no_gso(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना cf_quic_ctx *qctx,
                                   स्थिरांक uint8_t *pkt, माप_प्रकार pktlen,
                                   माप_प्रकार gsolen, माप_प्रकार *psent)
{
  स्थिरांक uint8_t *p, *end = pkt + pktlen;
  माप_प्रकार sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(माप_प्रकार calls = 0);

  *psent = 0;

  साठी(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (माप_प्रकार)(end - p));
    result = do_sendmsg(cf, data, qctx, p, len, len, &sent);
    जर(result)
      जा out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(data, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (पूर्णांक)result, *psent);
  परत result;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
स्थिर CURLcode send_packet_no_gso_cf(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      स्थिरांक uint8_t *pkt, माप_प्रकार pktlen,
                                      माप_प्रकार gsolen, माप_प्रकार *psent)
{
  स्थिरांक uint8_t *p, *end = pkt + pktlen;
  माप_प्रकार sent, len;
  CURLcode result = CURLE_OK;
  VERBOSE(माप_प्रकार calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  साठी(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (माप_प्रकार)(end - p));
    result = Curl_conn_cf_send(cf->next, data, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    जर(result)
      जा out;

    जर(sent != len) {
      /* We can only send the complete datagram, not parts. */
      result = CURLE_SEND_ERROR;
      जा out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(data, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (पूर्णांक)result, *psent);
  परत result;
}

स्थिर CURLcode vquic_send_packets(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना cf_quic_ctx *qctx,
                                   स्थिरांक uint8_t *pkt, माप_प्रकार pktlen,
                                   माप_प्रकार gsolen, माप_प्रकार *psent)
{
  CURLcode result;
#ifdef DEBUGBUILD
  /* simulate network blocking/partial writes */
  जर(qctx->wblock_percent > 0) {
    अचिन्हित अक्षर c;
    *psent = 0;
    Curl_rand(data, &c, 1);
    जर(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(data, cf, "vquic_flush() simulate EWOULDBLOCK");
      परत CURLE_AGAIN;
    }
  }
#endif
  जर(qctx->no_gso && pktlen > gsolen) {
    result = send_packet_no_gso(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  अन्यथा {
    result = do_sendmsg(cf, data, qctx, pkt, pktlen, gsolen, psent);
  }
  जर(!result)
    qctx->last_io = qctx->last_op;
  परत result;
}

CURLcode Curl_vquic_flush(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          संरचना cf_quic_ctx *qctx)
{
  स्थिरांक अचिन्हित अक्षर *buf;
  माप_प्रकार blen, sent;
  CURLcode result;
  माप_प्रकार gsolen;

  पर्यंत(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    जर(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      जर(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    जर(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      result = vquic_send_packets(cf, data, qctx, buf, blen, gsolen, &sent);
    }
    अन्यथा {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      जर(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        result = send_packet_no_gso_cf(cf, data, buf, blen, gsolen, &sent);
      }
      अन्यथा {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &sent);
      }
    }

    जर(result) {
      जर(result == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        जर(qctx->split_len)
          qctx->split_len -= sent;
      }
      परत result;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    जर(qctx->split_len)
      qctx->split_len -= sent;
  }
  परत CURLE_OK;
}

CURLcode Curl_vquic_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                         संरचना cf_quic_ctx *qctx, माप_प्रकार gsolen)
{
  qctx->gsolen = gsolen;
  परत Curl_vquic_flush(cf, data, qctx);
}

CURLcode Curl_vquic_send_tail_split(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना cf_quic_ctx *qctx, माप_प्रकार gsolen,
                                    माप_प्रकार tail_len, माप_प्रकार tail_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > tail_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - tail_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = tail_gsolen;
  CURL_TRC_CF(data, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, tail_len, qctx->gsolen);
  परत Curl_vquic_flush(cf, data, qctx);
}

#जर (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_संदेश_X)
स्थिर माप_प्रकार vquic_msghdr_get_udp_gro(संरचना msghdr *संदेश)
{
  पूर्णांक gso_size = 0;
#जर defined(__linux__) && defined(UDP_GRO)
  संरचना cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#जर defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
  साठी(cmsg = CMSG_FIRSTHDR(संदेश); cmsg; cmsg = CMSG_NXTHDR(संदेश, cmsg)) {
#जर defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic काढा
#endif
    जर(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      स_नकल(&gso_size, CMSG_DATA(cmsg), आकार(gso_size));

      थांबा;
    }
  }
#endif /* linux && UDP_GRO */
  (शून्य)संदेश;

  परत (माप_प्रकार)gso_size;
}
#endif /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#जर (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_संदेश_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
स्थिर uint8_t vquic_msghdr_get_ecn(संरचना msghdr *संदेश, पूर्णांक family)
{
  संरचना cmsghdr *cmsg;
  निवड(family) {
  स्थिती AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#जर defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    साठी(cmsg = CMSG_FIRSTHDR(संदेश); cmsg; cmsg = CMSG_NXTHDR(संदेश, cmsg)) {
#जर defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic काढा
#endif
      जर(cmsg->cmsg_level == IPPROTO_IP &&
#ifdef __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#अन्यथा
          cmsg->cmsg_type == IP_TOS
#endif
          && cmsg->cmsg_len) {
        परत *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    थांबा;
  स्थिती AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#जर defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wcast-align"
#endif
    साठी(cmsg = CMSG_FIRSTHDR(संदेश); cmsg; cmsg = CMSG_NXTHDR(संदेश, cmsg)) {
#जर defined(__clang__) && !defined(__GLIBC__)
#pragma clang diagnostic काढा
#endif
      जर(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        अचिन्हित पूर्णांक tos;

        स_नकल(&tos, CMSG_DATA(cmsg), आकार(पूर्णांक));

        परत (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    थांबा;
  }
  परत 0;
}
#अन्यथा
#define vquic_msghdr_get_ecn(a, b)      0
#endif /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#ifdef HAVE_SENDMMSG

स्थिर CURLcode recvmmsg_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                 माप_प्रकार *pnread)
{
#जर defined(__linux__) && defined(UDP_GRO)
#define MMSG_अंक  16
#define UDP_GRO_CNT_MAX  64
#define CMSG_PER_संदेश_SIZE    (2 * CMSG_SPACE(आकार(पूर्णांक)))
#अन्यथा
#define MMSG_अंक  64
#define UDP_GRO_CNT_MAX  1
#define CMSG_PER_संदेश_SIZE    CMSG_SPACE(आकार(पूर्णांक))
#endif
#define संदेश_BUF_SIZE  (UDP_GRO_CNT_MAX * 1500)
  संरचना iovec msg_iov[MMSG_अंक];
  संरचना mmsghdr mmsg[MMSG_अंक];
  uint8_t संदेश_ctrl[MMSG_अंक * CMSG_PER_संदेश_SIZE];
  संरचना sockaddr_storage remote_addr[MMSG_अंक];
  माप_प्रकार total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  माप_प्रकार calls = 0;
#endif
  पूर्णांक mcount, i, n;
  अक्षर errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  माप_प्रकार gso_size;
  अक्षर *sockbuf = रिक्त;
  uint8_t (*bufs)[संदेश_BUF_SIZE] = रिक्त;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, MMSG_अंक * संदेश_BUF_SIZE,
                                          &sockbuf);
  जर(result)
    जा out;
  bufs = (uint8_t (*)[संदेश_BUF_SIZE])sockbuf;

  total_nread = 0;
  पर्यंत(pkts < max_pkts) {
    n = (पूर्णांक)CURLMIN(CURLMIN(MMSG_अंक, IOV_MAX), max_pkts);
    स_रखो(&mmsg, 0, आकार(mmsg));
    साठी(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = आकार(bufs[i]);
      mmsg[i].संदेश_hdr.msg_iov = &msg_iov[i];
      mmsg[i].संदेश_hdr.संदेश_iovlen = 1;
      mmsg[i].संदेश_hdr.संदेश_name = &remote_addr[i];
      mmsg[i].संदेश_hdr.msg_namelen = आकार(remote_addr[i]);
      mmsg[i].संदेश_hdr.संदेश_control = &संदेश_ctrl[i * CMSG_PER_संदेश_SIZE];
      mmsg[i].संदेश_hdr.msg_controllen = CMSG_PER_संदेश_SIZE;
    }

    पर्यंत((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, रिक्त)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    जर(mcount == -1) {
      जर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmmsg -> EAGAIN");
        जा out;
      }
      जर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        जर(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        जा out;
      }
      curlx_strerror(SOCKERRNO, errstr, आकार(errstr));
      failf(data, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      जा out;
    }

    VERBOSE(++calls);
    साठी(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      जर(!mmsg[i].संदेश_len) {
        ++pkts;
        सुरूठेवा;
      }
      total_nread += mmsg[i].संदेश_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].संदेश_hdr, remote_addr[i].ss_family);
      gso_size = vquic_msghdr_get_udp_gro(&mmsg[i].संदेश_hdr);
      जर(gso_size == 0)
        gso_size = mmsg[i].संदेश_len;

      result = recv_cb(bufs[i], mmsg[i].संदेश_len, gso_size,
                       mmsg[i].संदेश_hdr.संदेश_name,
                       mmsg[i].संदेश_hdr.msg_namelen, ecn, userp);
      जर(result)
        जा out;
      pkts += (mmsg[i].संदेश_len + gso_size - 1) / gso_size;
    }
  }

out:
  जर(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णांक)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  परत result;
}

#किंवाजर defined(HAVE_APPLE_संदेश_X)

स्थिर CURLcode recvmsg_x_packets(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना cf_quic_ctx *qctx,
                                  माप_प्रकार max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  शून्य *userp, माप_प्रकार *pnread)
{
#define संदेश_X_अंक  64
#define संदेश_BUF_SIZE  (2048)
#define CMSG_PER_संदेश_SIZE     CMSG_SPACE(आकार(पूर्णांक))
  संरचना iovec msg_iov[संदेश_X_अंक];
  संरचना msghdr_x mmsg[संदेश_X_अंक];
  uint8_t संदेश_ctrl[संदेश_X_अंक * CMSG_PER_संदेश_SIZE];
  संरचना sockaddr_storage remote_addr[संदेश_X_अंक];
  माप_प्रकार total_nread = 0, pkts = 0;
#ifdef CURLVERBOSE
  माप_प्रकार calls = 0;
#endif
  पूर्णांक mcount, i;
  अक्षर errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  माप_प्रकार gso_size;
  अक्षर *sockbuf = रिक्त;
  uint8_t (*bufs)[संदेश_BUF_SIZE] = रिक्त;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  result = Curl_multi_xfer_sockbuf_borrow(data, संदेश_X_अंक * संदेश_BUF_SIZE,
                                          &sockbuf);
  जर(result)
    जा out;
  bufs = (uint8_t (*)[संदेश_BUF_SIZE])sockbuf;

  total_nread = 0;
  पर्यंत(pkts < max_pkts) {
    पूर्णांक n = (पूर्णांक)CURLMIN(CURLMIN(संदेश_X_अंक, IOV_MAX), max_pkts);
    स_रखो(&mmsg, 0, आकार(mmsg));
    साठी(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = आकार(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].संदेश_iovlen = 1;
      mmsg[i].संदेश_name = &remote_addr[i];
      mmsg[i].msg_namelen = आकार(remote_addr[i]);
      mmsg[i].संदेश_control = &संदेश_ctrl[i * CMSG_PER_संदेश_SIZE];
      mmsg[i].msg_controllen = CMSG_PER_संदेश_SIZE;
    }

#जर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    पर्यंत((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#जर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#pragma GCC diagnostic काढा
#endif
    जर(mcount == -1) {
      जर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvmsg_x -> EAGAIN");
        जा out;
      }
      जर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        जर(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        जा out;
      }
      curlx_strerror(SOCKERRNO, errstr, आकार(errstr));
      failf(data, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      जा out;
    }

    VERBOSE(++calls);
    साठी(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      जर(!mmsg[i].संदेश_datalen) {
        ++pkts;
        सुरूठेवा;
      }
      total_nread += mmsg[i].संदेश_datalen;
      ecn = vquic_msghdr_get_ecn((संरचना msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_size = mmsg[i].संदेश_datalen;

      result = recv_cb(bufs[i], mmsg[i].संदेश_datalen, gso_size,
                       mmsg[i].संदेश_name,
                       mmsg[i].msg_namelen, ecn, userp);
      जर(result)
        जा out;
      pkts += (mmsg[i].संदेश_datalen + gso_size - 1) / gso_size;
    }
  }

out:
  जर(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णांक)result);
  Curl_multi_xfer_sockbuf_release(data, sockbuf);
  *pnread = total_nread;
  परत result;
}

#किंवाजर defined(HAVE_SENDMSG)
स्थिर CURLcode recvmsg_packets(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना cf_quic_ctx *qctx,
                                माप_प्रकार max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                माप_प्रकार *pnread)
{
#define CMSG_PER_संदेश_SIZE    CMSG_SPACE(आकार(पूर्णांक))
  संरचना iovec msg_iov;
  संरचना msghdr संदेश;
  uint8_t buf[64 * 1024];
  संरचना sockaddr_storage remote_addr;
  माप_प्रकार total_nread, pkts, calls;
  ssize_t rc;
  माप_प्रकार nread;
  अक्षर errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;
  uint8_t संदेश_ctrl[CMSG_PER_संदेश_SIZE];
  माप_प्रकार gso_size;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  साठी(pkts = 0, total_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    स_रखो(&संदेश, 0, आकार(संदेश));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = आकार(buf);
    संदेश.msg_iov = &msg_iov;
    संदेश.संदेश_iovlen = 1;
    संदेश.संदेश_control = संदेश_ctrl;
    संदेश.संदेश_name = &remote_addr;
    संदेश.msg_namelen = आकार(remote_addr);
    संदेश.msg_controllen = CMSG_PER_संदेश_SIZE;

    पर्यंत((rc = recvmsg(qctx->sockfd, &संदेश, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    जर(!curlx_sztouz(rc, &nread)) {
      जर(SOCK_EAGAIN(SOCKERRNO)) {
        जा out;
      }
      जर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        जर(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        जा out;
      }
      curlx_strerror(SOCKERRNO, errstr, आकार(errstr));
      failf(data, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      जा out;
    }

    total_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    जर(!nread) {
      ++pkts;
      सुरूठेवा;
    }

    ecn = vquic_msghdr_get_ecn(&संदेश, remote_addr.ss_family);
    gso_size = vquic_msghdr_get_udp_gro(&संदेश);
    जर(gso_size == 0)
      gso_size = nread;

    result = recv_cb(buf, nread, gso_size,
                     संदेश.संदेश_name, संदेश.msg_namelen, ecn, userp);
    जर(result)
      जा out;
    pkts += (nread + gso_size - 1) / gso_size;
  }

out:
  जर(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णांक)result);
  *pnread = total_nread;
  परत result;
}

#अन्यथा /* HAVE_SENDMMSG || HAVE_SENDMSG */
स्थिर CURLcode recvfrom_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp,
                                 माप_प्रकार *pnread)
{
  uint8_t buf[64 * 1024];
  पूर्णांक bufsize = (पूर्णांक)आकार(buf);
  संरचना sockaddr_storage remote_addr;
  socklen_t remote_addrlen = आकार(remote_addr);
  माप_प्रकार total_nread, pkts, calls = 0, nread;
  ssize_t rv;
  अक्षर errstr[STRERROR_LEN];
  CURLcode result = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  साठी(pkts = 0, total_nread = 0; pkts < max_pkts;) {
    पर्यंत((rv = recvfrom(qctx->sockfd, (अक्षर *)buf, bufsize, 0,
                         (संरचना sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    जर(!curlx_sztouz(rv, &nread)) {
      जर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(data, cf, "ingress, recvfrom -> EAGAIN");
        जा out;
      }
      जर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        संरचना ip_quadruple ip;
        जर(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
          failf(data, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        result = CURLE_COULDNT_CONNECT;
        जा out;
      }
      curlx_strerror(SOCKERRNO, errstr, आकार(errstr));
      failf(data, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      result = CURLE_RECV_ERROR;
      जा out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    जर(!nread)
      सुरूठेवा;

    total_nread += nread;
    result = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    जर(result)
      जा out;
  }

out:
  जर(total_nread || result)
    CURL_TRC_CF(data, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                total_nread, pkts, calls, (पूर्णांक)result);
  *pnread = total_nread;
  परत result;
}
#endif /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, शून्य *userp)
{
  CURLcode result;
  माप_प्रकार nread = 0;
#ifdef HAVE_SENDMMSG
  result = recvmmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#किंवाजर defined(HAVE_APPLE_संदेश_X)
  result = recvmsg_x_packets(cf, data, qctx, max_pkts, recv_cb, userp,
                             &nread);
#किंवाजर defined(HAVE_SENDMSG)
  result = recvmsg_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#अन्यथा
  result = recvfrom_packets(cf, data, qctx, max_pkts, recv_cb, userp, &nread);
#endif
  जर(!result) {
    /* EAGAIN is no error, only count actual packets as a reply */
    जर(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  परत result;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(संरचना Curl_easy *data,
                      अचिन्हित अक्षर *scid,
                      माप_प्रकार scidlen,
                      पूर्णांक *qlogfdp)
{
  अक्षर *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  जर(qlog_dir) {
    संरचना dynbuf fname;
    CURLcode result;
    अचिन्हित पूर्णांक i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    result = curlx_dyn_add(&fname, qlog_dir);
    जर(!result)
      result = curlx_dyn_add(&fname, "/");
    साठी(i = 0; (i < scidlen) && !result; i++) {
      अक्षर hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      result = curlx_dyn_add(&fname, hex);
    }
    जर(!result)
      result = curlx_dyn_add(&fname, ".sqlog");

    जर(!result) {
      पूर्णांक qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_द्विमान,
#ifdef _WIN32
                              data->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#अन्यथा
                              (mode_t)data->set.new_file_perms
#endif
                              );
      जर(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    जर(result)
      परत result;
  }

  परत CURLE_OK;
}

CURLcode Curl_cf_quic_घाला_after(संरचना Curl_cfilter *cf_at,
                                   संरचना Curl_easy *data,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  (शून्य)data; /* not used in all cases and compilers are stupid */
#जर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  result = Curl_cf_ngtcp2_घाला_after(cf_at, origin, peer, &cf);
#किंवाजर defined(USE_QUICHE)
  result = Curl_cf_quiche_घाला_after(cf_at, origin, peer, &cf);
#अन्यथा
  (शून्य)cf)
  (शून्य)cf_at;
  (शून्य)origin;
  (शून्य)peer;
  result = CURLE_NOT_BUILT_IN;
#endif

#जर defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  जर(!result && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    result = Curl_conn_dns_add_https_resolve(data, cf->conn,
                                             cf->sockindex, origin);
  }
#endif /* USE_HTTPSRR && USE_ECH */
  परत result;
}

CURLcode Curl_cf_quic_create(संरचना Curl_cfilter **pcf,
                             संरचना Curl_easy *data,
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
#जर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  परत Curl_cf_ngtcp2_create(pcf, data, origin, peer, conn, addr);
#किंवाजर defined(USE_QUICHE)
  परत Curl_cf_quiche_create(pcf, data, origin, peer, conn, addr);
#अन्यथा
  *pcf = रिक्त;
  (शून्य)data;
  (शून्य)origin;
  (शून्य)peer;
  (शून्य)conn;
  (शून्य)addr;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  परत CURLE_NOT_BUILT_IN;
#endif
}

#जर !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_घाला_after(संरचना Curl_cfilter *cf_at,
                                       संरचना Curl_easy *data,
                                       संरचना Curl_peer *origin,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#जर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  परत Curl_cf_ngtcp2_proxy_घाला_after(cf_at, data, origin, peer,
                                           tunnel_peer, tunnel_transport);
#अन्यथा
  (शून्य)cf_at;
  (शून्य)data;
  (शून्य)origin;
  (शून्य)peer;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  परत CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_cf_h3_proxy_create(संरचना Curl_cfilter **pcf,
                                 संरचना Curl_easy *data,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 uint8_t transport_peer,
                                 संरचना connectdata *conn,
                                 संरचना Curl_sockaddr_ex *addr,
                                 संरचना Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#जर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  परत Curl_cf_ngtcp2_proxy_create(pcf, data, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#अन्यथा
  *pcf = रिक्त;
  (शून्य)data;
  (शून्य)conn;
  (शून्य)addr;
  (शून्य)peer;
  (शून्य)transport_peer;
  (शून्य)tunnel_peer;
  (शून्य)tunnel_transport;
  परत CURLE_NOT_BUILT_IN;
#endif
}

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(संरचना Curl_easy *data,
                             संरचना connectdata *conn)
{
  जर(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(data, "HTTP/3 cannot be used over UNIX domain sockets");
    परत CURLE_QUIC_CONNECT_ERROR;
  }
  जर(!(data->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(data, "HTTP/3 requested for non-HTTPS URL");
    परत CURLE_URL_MALFORMAT;
  }
#ifndef CURL_DISABLE_PROXY
  जर(conn->socks_proxy.peer) {
    failf(data, "HTTP/3 is not supported over a SOCKS proxy");
    परत CURLE_URL_MALFORMAT;
  }
#अन्यथा
  (शून्य)conn;
#endif

  परत CURLE_OK;
}

#ifdef CURLVERBOSE
स्थिरांक अक्षर *Curl_vquic_h3_err_str(uint64_t error_code)
{
  जर(error_code <= अच_पूर्णांक_उच्च) {
    निवड((अचिन्हित पूर्णांक)error_code) {
    स्थिती CURL_H3_ERR_NO_ERROR:
      परत "NO_ERROR";
    स्थिती CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      परत "GENERAL_PROTOCOL_ERROR";
    स्थिती CURL_H3_ERR_INTERNAL_ERROR:
      परत "INTERNAL_ERROR";
    स्थिती CURL_H3_ERR_STREAM_CREATION_ERROR:
      परत "STREAM_CREATION_ERROR";
    स्थिती CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      परत "CLOSED_CRITICAL_STREAM";
    स्थिती CURL_H3_ERR_FRAME_UNEXPECTED:
      परत "FRAME_UNEXPECTED";
    स्थिती CURL_H3_ERR_FRAME_ERROR:
      परत "FRAME_ERROR";
    स्थिती CURL_H3_ERR_EXCESSIVE_LOAD:
      परत "EXCESSIVE_LOAD";
    स्थिती CURL_H3_ERR_ID_ERROR:
      परत "ID_ERROR";
    स्थिती CURL_H3_ERR_SETTINGS_ERROR:
      परत "SETTINGS_ERROR";
    स्थिती CURL_H3_ERR_MISSING_SETTINGS:
      परत "MISSING_SETTINGS";
    स्थिती CURL_H3_ERR_REQUEST_REJECTED:
      परत "REQUEST_REJECTED";
    स्थिती CURL_H3_ERR_REQUEST_CANCELLED:
      परत "REQUEST_CANCELLED";
    स्थिती CURL_H3_ERR_REQUEST_INCOMPLETE:
      परत "REQUEST_INCOMPLETE";
    स्थिती CURL_H3_ERR_MESSAGE_ERROR:
      परत "MESSAGE_ERROR";
    स्थिती CURL_H3_ERR_CONNECT_ERROR:
      परत "CONNECT_ERROR";
    स्थिती CURL_H3_ERR_VERSION_FALLBACK:
      परत "VERSION_FALLBACK";
    पूर्वनिर्धारित:
      थांबा;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  जर((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    परत "NO_ERROR";
  परत "unknown";
}
#endif /* CURLVERBOSE */

#जर defined(USE_NGTCP2) || defined(USE_NGHTTP3)

स्थिर शून्य *vquic_ngtcp2_malloc(माप_प्रकार size, शून्य *user_data)
{
  (शून्य)user_data;
  परत Curl_cmalloc(size);
}

स्थिर शून्य vquic_ngtcp2_free(शून्य *ptr, शून्य *user_data)
{
  (शून्य)user_data;
  Curl_cfree(ptr);
}

स्थिर शून्य *vquic_ngtcp2_calloc(माप_प्रकार nmemb, माप_प्रकार size, शून्य *user_data)
{
  (शून्य)user_data;
  परत Curl_ccalloc(nmemb, size);
}

स्थिर शून्य *vquic_ngtcp2_realloc(शून्य *ptr, माप_प्रकार size, शून्य *user_data)
{
  (शून्य)user_data;
  परत Curl_crealloc(ptr, size);
}

#ifdef USE_NGTCP2
स्थिर संरचना ngtcp2_mem vquic_ngtcp2_mem = {
  रिक्त,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
संरचना ngtcp2_mem *Curl_ngtcp2_mem(शून्य)
{
  परत &vquic_ngtcp2_mem;
}
#endif

#ifdef USE_NGHTTP3
स्थिर संरचना nghttp3_mem vquic_nghttp3_mem = {
  रिक्त,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
संरचना nghttp3_mem *Curl_nghttp3_mem(शून्य)
{
  परत &vquic_nghttp3_mem;
}
#endif

#endif /* USE_NGTCP2 || USE_NGHTTP3 */

#अन्यथा /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(संरचना Curl_easy *data,
                             संरचना connectdata *conn)
{
  (शून्य)data;
  (शून्य)conn;
  DEBUGF(infof(data, "QUIC is not supported in this build"));
  परत CURLE_NOT_BUILT_IN;
}

#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */
