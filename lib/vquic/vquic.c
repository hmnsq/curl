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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "vquic/vquic.h"
#समावेश "vtls/vtls.h"

#समावेश "curl_trc.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)

#अगर_घोषित HAVE_NETINET_UDP_H
#समावेश <netinet/udp.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IP_H
#समावेश <netinet/ip.h>
#पूर्ण_अगर

#अगर_घोषित USE_NGHTTP3
#समावेश <nghttp3/nghttp3.h>
#पूर्ण_अगर

#अगर defined(USE_APPLE_FAST_UDP) && defined(__APPLE__)
#समावेश <sys/syscall.h>
#अगर defined(SYS_recvmsg_x) && defined(SYS_sendmsg_x)
#घोषणा HAVE_APPLE_संदेश_X
#पूर्ण_अगर
#पूर्ण_अगर

#समावेश "bufq.h"
#समावेश "curlx/dynbuf.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "connect.h"
#समावेश "cfilters.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vquic/cf-ngtcp2.h"
#समावेश "vquic/cf-ngtcp2-cmn.h"
#समावेश "vquic/cf-ngtcp2-proxy.h"
#समावेश "vquic/cf-quiche.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "rand.h"
#समावेश "vquic/vquic_int.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strparse.h"


#घोषणा NW_CHUNK_आकार     (64 * 1024)
#घोषणा NW_SEND_CHUNKS    1

#अगर_घोषित HAVE_APPLE_संदेश_X

/* this is `struct msghdr` with an additional field at the end */
काष्ठा msghdr_x {
  व्योम *संदेश_name;           /* optional address */
  socklen_t msg_namelen;    /* size of address */
  काष्ठा iovec *msg_iov;    /* scatter/gather array */
  पूर्णांक संदेश_iovlen;           /* # elements in msg_iov */
  व्योम *संदेश_control;        /* ancillary data, see below */
  socklen_t msg_controllen; /* ancillary data buffer len */
  पूर्णांक संदेश_flags;            /* flags on received message */
  माप_प्रकार संदेश_datalen;       /* byte length of buffer in msg_iov */
};
#पूर्ण_अगर

#अगर_घोषित CURLVERBOSE
#अगर_घोषित HAVE_APPLE_संदेश_X
#घोषणा VQUIC_SEND_METHOD   "sendmsg_x"
#या_अगर defined(HAVE_SENDMSG)
#घोषणा VQUIC_SEND_METHOD   "sendmsg"
#अन्यथा
#घोषणा VQUIC_SEND_METHOD   "send"
#पूर्ण_अगर
#पूर्ण_अगर

पूर्णांक Curl_vquic_init(व्योम)
{
#अगर defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2)
  अगर(ngtcp2_crypto_ossl_init())
    वापस 0;
#पूर्ण_अगर

  वापस 1;
}

व्योम Curl_vquic_cleanup(व्योम)
{
#अगर defined(USE_NGTCP2) && defined(OPENSSL_QUIC_API2) && \
  (NGTCP2_VERSION_अंक >= 0x011800)
  ngtcp2_crypto_ossl_free();
#पूर्ण_अगर
}

व्योम Curl_quic_ver(अक्षर *p, माप_प्रकार len)
{
#अगर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  Curl_ngtcp2_ver(p, len);
#या_अगर defined(USE_QUICHE)
  Curl_quiche_ver(p, len);
#पूर्ण_अगर
}

CURLcode Curl_vquic_ctx_init(काष्ठा Curl_easy *तथ्य,
                             काष्ठा cf_quic_ctx *qctx)
{
  Curl_bufq_init2(&qctx->sendbuf, NW_CHUNK_आकार, NW_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
#अगर defined(__linux__) && defined(UDP_SEGMENT) && defined(HAVE_SENDMSG)
  qctx->no_gso = FALSE;
#या_अगर defined(HAVE_APPLE_संदेश_X)
  qctx->no_gso = FALSE;
#अन्यथा
  qctx->no_gso = TRUE;
#पूर्ण_अगर
#अगर_घोषित DEBUGBUILD
  {
    स्थिर अक्षर *p = दो_पर्या("CURL_DBG_QUIC_WBLOCK");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, 100))
        qctx->wblock_percent = (पूर्णांक)l;
    }
  }
#पूर्ण_अगर
  Curl_vquic_ctx_set_time(qctx, Curl_pgrs_now(तथ्य));

  वापस CURLE_OK;
}

व्योम Curl_vquic_ctx_free(काष्ठा cf_quic_ctx *qctx)
{
  Curl_bufq_free(&qctx->sendbuf);
}

व्योम Curl_vquic_ctx_set_time(काष्ठा cf_quic_ctx *qctx,
                             स्थिर काष्ठा curltime *pnow)
{
  qctx->last_op = *pnow;
}

व्योम Curl_vquic_ctx_update_time(काष्ठा cf_quic_ctx *qctx,
                                स्थिर काष्ठा curltime *pnow)
{
  qctx->last_op = *pnow;
}

#अगर_घोषित HAVE_APPLE_संदेश_X
अटल CURLcode do_sendmsg(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा cf_quic_ctx *qctx,
                           स्थिर uint8_t *pkt, माप_प्रकार pktlen, माप_प्रकार gsolen,
                           माप_प्रकार *psent)
{
#घोषणा संदेश_X_SNUM  64
  काष्ठा iovec msg_iov[संदेश_X_SNUM];
  काष्ठा msghdr_x mmsg[संदेश_X_SNUM];
  अक्षर errstr[STRERROR_LEN];
  माप_प्रकार n, i = 0, sent = 0;
  पूर्णांक rc;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(माप_प्रकार calls = 0);

  *psent = 0;
  अगर(!pktlen)
    वापस CURLE_OK;
  अगर(!gsolen || (pktlen < gsolen))
    gsolen = pktlen;
  n = (pktlen + gsolen - 1) / gsolen;
  जबतक(i < n) {
    माप_प्रकार j, batch = CURLMIN(n - i, संदेश_X_SNUM), pkts_sent = 0;

    क्रम(j = 0; j < batch; ++j) {
      स्थिर माप_प्रकार offset = (i + j) * gsolen;
      msg_iov[j].iov_base = CURL_UNCONST(pkt + offset);
      msg_iov[j].iov_len = CURLMIN(gsolen, pktlen - offset);
      स_रखो(&mmsg[j], 0, माप(mmsg[j]));
      mmsg[j].msg_iov = &msg_iov[j];
      mmsg[j].संदेश_iovlen = 1;
      mmsg[j].संदेश_datalen = msg_iov[j].iov_len;
    }

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
#पूर्ण_अगर
    जबतक((rc = syscall(SYS_sendmsg_x, qctx->sockfd, &mmsg, batch, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR))
      ;
#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

    अगर(rc < 0) {
      अगर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(तथ्य, cf, "egress, sendmsg_x -> EAGAIN");
        परिणाम = sent ? CURLE_OK : CURLE_AGAIN;
        जाओ out;
      }
      अगर(SOCKERRNO != SOCKEMSGSIZE) {
        curlx_strerror(SOCKERRNO, errstr, माप(errstr));
        failf(तथ्य, "QUIC: sendmsg_x() returned %d (errno=%d; %s)",
              rc, SOCKERRNO, errstr);
        परिणाम = CURLE_SEND_ERROR;
        जाओ out;
      }
      /* Error was SOCKEMSGSIZE. Network stack does not accept the packet
       * length(s). This might be a PMTUD. Just drop it into the void
       * and fall through to the success handling. */
      pkts_sent = batch;
    }
    अन्यथा
      pkts_sent = CURLMIN((माप_प्रकार)rc, batch);

    VERBOSE(++calls);
    अगर(!pkts_sent) {  /* no packets of the current batch were sent */
      परिणाम = sent ? CURLE_OK : CURLE_AGAIN;
      जाओ out;
    }
    i += pkts_sent; /* Some have been sent */
    क्रम(j = 0; j < pkts_sent; ++j)
      sent += msg_iov[j].iov_len;
    अगर(pkts_sent < batch)
      जाओ out; /* but not all of them */
  }

out:
  *psent = sent;
  अगर(sent || परिणाम)
    CURL_TRC_CF(तथ्य, cf,
                "vquic_sendmsg_x(len=%zu, gso=%zu, packets=%zu, "
                "calls=%zu) -> %d",
                sent, gsolen, n, calls, (पूर्णांक)परिणाम);
  वापस परिणाम;
}

#अन्यथा /* HAVE_APPLE_MSG_X */

अटल CURLcode send_packet_no_gso(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा cf_quic_ctx *qctx,
                                   स्थिर uint8_t *pkt, माप_प्रकार pktlen,
                                   माप_प्रकार gsolen, माप_प्रकार *psent);

अटल CURLcode do_sendmsg(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा cf_quic_ctx *qctx,
                           स्थिर uint8_t *pkt, माप_प्रकार pktlen, माप_प्रकार gsolen,
                           माप_प्रकार *psent)
{
  CURLcode परिणाम = CURLE_OK;
#अगर_घोषित HAVE_SENDMSG
  काष्ठा iovec msg_iov;
  काष्ठा msghdr संदेश = { 0 };
  ssize_t rv;
#अगर defined(__linux__) && defined(UDP_SEGMENT)
  uint8_t संदेश_ctrl[32];
  काष्ठा cmsghdr *cm;
#पूर्ण_अगर

  *psent = 0;
  msg_iov.iov_base = (uint8_t *)CURL_UNCONST(pkt);
  msg_iov.iov_len = pktlen;
  संदेश.msg_iov = &msg_iov;
  संदेश.संदेश_iovlen = 1;

#अगर defined(__linux__) && defined(UDP_SEGMENT)
  अगर(pktlen > gsolen) {
    /* Only set this, when we need it. macOS, for example,
     * does not seem to like a msg_control of length 0. */
    स_रखो(संदेश_ctrl, 0, माप(संदेश_ctrl));
    संदेश.संदेश_control = संदेश_ctrl;
    DEBUGASSERT(माप(संदेश_ctrl) >= CMSG_SPACE(माप(पूर्णांक)));
    संदेश.msg_controllen = CMSG_SPACE(माप(पूर्णांक));
    cm = CMSG_FIRSTHDR(&संदेश);
    cm->cmsg_level = SOL_UDP;
    cm->cmsg_type = UDP_SEGMENT;
    cm->cmsg_len = CMSG_LEN(माप(uint16_t));
    *(uint16_t *)(व्योम *)CMSG_DATA(cm) = gsolen & 0xffff;
  }
#पूर्ण_अगर

  जबतक((rv = sendmsg(qctx->sockfd, &संदेश, 0)) == -1 && SOCKERRNO == SOCKEINTR)
    ;

  अगर(!curlx_sztouz(rv, psent)) {
    पूर्णांक sockerr = SOCKERRNO;
    अगर(SOCK_EAGAIN(sockerr))
      वापस CURLE_AGAIN;
    चयन(sockerr) {
    हाल SOCKEMSGSIZE:
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
      अवरोध;
    हाल EIO:
      अगर(pktlen > gsolen) {
        /* GSO failure */
        infof(तथ्य, "sendmsg() returned %zd (errno %d); disable GSO", rv,
              sockerr);
        qctx->no_gso = TRUE;
        वापस send_packet_no_gso(cf, तथ्य, qctx, pkt, pktlen, gsolen, psent);
      }
      FALLTHROUGH();
    शेष:
      failf(तथ्य, "sendmsg() returned %zd (errno %d)", rv, sockerr);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }
  अन्यथा अगर(pktlen != *psent) {
    failf(तथ्य, "sendmsg() sent only %zu/%zu bytes", *psent, pktlen);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
#अन्यथा
  ssize_t rv;
  (व्योम)gsolen;

  *psent = 0;

  जबतक((rv = swrite(qctx->sockfd, pkt, pktlen)) == -1 &&
        SOCKERRNO == SOCKEINTR)
    ;

  अगर(!curlx_sztouz(rv, psent)) {
    अगर(SOCK_EAGAIN(SOCKERRNO)) {
      परिणाम = CURLE_AGAIN;
      जाओ out;
    }
    अन्यथा {
      अगर(SOCKERRNO != SOCKEMSGSIZE) {
        failf(तथ्य, "send() returned %zd (errno %d)", rv, SOCKERRNO);
        परिणाम = CURLE_SEND_ERROR;
        जाओ out;
      }
      /* UDP datagram is too large; caused by PMTUD. Let it be lost. */
      *psent = pktlen;
    }
  }
#पूर्ण_अगर
  (व्योम)cf;

out:
  CURL_TRC_CF(तथ्य, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=1) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, (पूर्णांक)परिणाम, *psent);
  वापस परिणाम;
}

#पूर्ण_अगर /* !HAVE_APPLE_MSG_X */

अटल CURLcode send_packet_no_gso(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा cf_quic_ctx *qctx,
                                   स्थिर uint8_t *pkt, माप_प्रकार pktlen,
                                   माप_प्रकार gsolen, माप_प्रकार *psent)
{
  स्थिर uint8_t *p, *end = pkt + pktlen;
  माप_प्रकार sent, len;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(माप_प्रकार calls = 0);

  *psent = 0;

  क्रम(p = pkt; p < end; p += gsolen) {
    len = CURLMIN(gsolen, (माप_प्रकार)(end - p));
    परिणाम = do_sendmsg(cf, तथ्य, qctx, p, len, len, &sent);
    अगर(परिणाम)
      जाओ out;
    *psent += sent;
    VERBOSE(++calls);
  }
out:
  CURL_TRC_CF(तथ्य, cf,
              "vquic_%s(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              VQUIC_SEND_METHOD, pktlen, gsolen, calls, (पूर्णांक)परिणाम, *psent);
  वापस परिणाम;
}

/* Split QUIC payload by datagram (gso) boundaries when sending over a
 * non-UDP lower filter (for example CONNECT-UDP proxy tunnel). */
अटल CURLcode send_packet_no_gso_cf(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      स्थिर uint8_t *pkt, माप_प्रकार pktlen,
                                      माप_प्रकार gsolen, माप_प्रकार *psent)
{
  स्थिर uint8_t *p, *end = pkt + pktlen;
  माप_प्रकार sent, len;
  CURLcode परिणाम = CURLE_OK;
  VERBOSE(माप_प्रकार calls = 0);

  *psent = 0;

  /* Send one datagram-sized chunk per call into the lower filter. */
  क्रम(p = pkt; p < end; p += len) {
    len = CURLMIN(gsolen, (माप_प्रकार)(end - p));
    परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, p, len, FALSE, &sent);
    /* Report forward progress even if we return CURLE_AGAIN later. */
    VERBOSE(++calls);
    /* Preserve lower-filter errors (including CURLE_AGAIN). */
    अगर(परिणाम)
      जाओ out;

    अगर(sent != len) {
      /* We can only send the complete datagram, not parts. */
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    *psent += sent;
  }

out:
  CURL_TRC_CF(तथ्य, cf,
              "vquic_cf_send(len=%zu, gso=%zu, calls=%zu) -> %d, sent=%zu",
              pktlen, gsolen, calls, (पूर्णांक)परिणाम, *psent);
  वापस परिणाम;
}

अटल CURLcode vquic_send_packets(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा cf_quic_ctx *qctx,
                                   स्थिर uint8_t *pkt, माप_प्रकार pktlen,
                                   माप_प्रकार gsolen, माप_प्रकार *psent)
{
  CURLcode परिणाम;
#अगर_घोषित DEBUGBUILD
  /* simulate network blocking/partial writes */
  अगर(qctx->wblock_percent > 0) {
    अचिन्हित अक्षर c;
    *psent = 0;
    Curl_rand(तथ्य, &c, 1);
    अगर(c >= ((100 - qctx->wblock_percent) * 256 / 100)) {
      CURL_TRC_CF(तथ्य, cf, "vquic_flush() simulate EWOULDBLOCK");
      वापस CURLE_AGAIN;
    }
  }
#पूर्ण_अगर
  अगर(qctx->no_gso && pktlen > gsolen) {
    परिणाम = send_packet_no_gso(cf, तथ्य, qctx, pkt, pktlen, gsolen, psent);
  }
  अन्यथा {
    परिणाम = do_sendmsg(cf, तथ्य, qctx, pkt, pktlen, gsolen, psent);
  }
  अगर(!परिणाम)
    qctx->last_io = qctx->last_op;
  वापस परिणाम;
}

CURLcode Curl_vquic_flush(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          काष्ठा cf_quic_ctx *qctx)
{
  स्थिर अचिन्हित अक्षर *buf;
  माप_प्रकार blen, sent;
  CURLcode परिणाम;
  माप_प्रकार gsolen;

  जबतक(Curl_bufq_peek(&qctx->sendbuf, &buf, &blen)) {
    gsolen = qctx->gsolen;
    अगर(qctx->split_len) {
      gsolen = qctx->split_gsolen;
      अगर(blen > qctx->split_len)
        blen = qctx->split_len;
    }

    अगर(qctx->sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket (via happy eyeballs) */
      परिणाम = vquic_send_packets(cf, तथ्य, qctx, buf, blen, gsolen, &sent);
    }
    अन्यथा {
      /* Tunneled QUIC (CONNECT-UDP through proxy) */
      अगर(gsolen && (blen > gsolen)) {
        /* Send one datagram at a time to preserve packet boundaries. */
        परिणाम = send_packet_no_gso_cf(cf, तथ्य, buf, blen, gsolen, &sent);
      }
      अन्यथा {
        /* No GSO aggregate to split, regular lower-filter send is enough. */
        परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, buf, blen, FALSE, &sent);
      }
    }

    अगर(परिणाम) {
      अगर(परिणाम == CURLE_AGAIN) {
        Curl_bufq_skip(&qctx->sendbuf, sent);
        अगर(qctx->split_len)
          qctx->split_len -= sent;
      }
      वापस परिणाम;
    }
    Curl_bufq_skip(&qctx->sendbuf, sent);
    अगर(qctx->split_len)
      qctx->split_len -= sent;
  }
  वापस CURLE_OK;
}

CURLcode Curl_vquic_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                         काष्ठा cf_quic_ctx *qctx, माप_प्रकार gsolen)
{
  qctx->gsolen = gsolen;
  वापस Curl_vquic_flush(cf, तथ्य, qctx);
}

CURLcode Curl_vquic_send_पुच्छ_split(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा cf_quic_ctx *qctx, माप_प्रकार gsolen,
                                    माप_प्रकार पुच्छ_len, माप_प्रकार पुच्छ_gsolen)
{
  DEBUGASSERT(Curl_bufq_len(&qctx->sendbuf) > पुच्छ_len);
  qctx->split_len = Curl_bufq_len(&qctx->sendbuf) - पुच्छ_len;
  qctx->split_gsolen = gsolen;
  qctx->gsolen = पुच्छ_gsolen;
  CURL_TRC_CF(तथ्य, cf, "vquic_send_tail_split: [%zu gso=%zu][%zu gso=%zu]",
              qctx->split_len, qctx->split_gsolen, पुच्छ_len, qctx->gsolen);
  वापस Curl_vquic_flush(cf, तथ्य, qctx);
}

#अगर (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG)) && \
  !defined(HAVE_APPLE_संदेश_X)
अटल माप_प्रकार vquic_msghdr_get_udp_gro(काष्ठा msghdr *संदेश)
{
  पूर्णांक gso_आकार = 0;
#अगर defined(__linux__) && defined(UDP_GRO)
  काष्ठा cmsghdr *cmsg;

  /* Workaround musl CMSG_NXTHDR issue */
#अगर defined(__clang__) && !defined(__GLIBC__)
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wsign-compare"
#आशय clang diagnostic ignored "-Wcast-align"
#पूर्ण_अगर
  क्रम(cmsg = CMSG_FIRSTHDR(संदेश); cmsg; cmsg = CMSG_NXTHDR(संदेश, cmsg)) {
#अगर defined(__clang__) && !defined(__GLIBC__)
#आशय clang diagnostic निकालो
#पूर्ण_अगर
    अगर(cmsg->cmsg_level == SOL_UDP && cmsg->cmsg_type == UDP_GRO) {
      स_नकल(&gso_आकार, CMSG_DATA(cmsg), माप(gso_आकार));

      अवरोध;
    }
  }
#पूर्ण_अगर /* linux && UDP_GRO */
  (व्योम)संदेश;

  वापस (माप_प्रकार)gso_आकार;
}
#पूर्ण_अगर /* (HAVE_SENDMMSG || HAVE_SENDMSG) && !HAVE_APPLE_MSG_X */

/* IP_RECVTOS was added in the macOS 10.13 SDK */
#अगर (defined(HAVE_SENDMMSG) || defined(HAVE_SENDMSG) || \
  defined(HAVE_APPLE_संदेश_X)) && \
  ((defined(__APPLE__) && defined(IP_RECVTOS)) || \
   (!defined(__APPLE__) && defined(IP_TOS))) && \
  defined(IPTOS_ECN_MASK)
अटल uint8_t vquic_msghdr_get_ecn(काष्ठा msghdr *संदेश, पूर्णांक family)
{
  काष्ठा cmsghdr *cmsg;
  चयन(family) {
  हाल AF_INET:
  /* Workaround musl CMSG_NXTHDR issue */
#अगर defined(__clang__) && !defined(__GLIBC__)
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wsign-compare"
#आशय clang diagnostic ignored "-Wcast-align"
#पूर्ण_अगर
    क्रम(cmsg = CMSG_FIRSTHDR(संदेश); cmsg; cmsg = CMSG_NXTHDR(संदेश, cmsg)) {
#अगर defined(__clang__) && !defined(__GLIBC__)
#आशय clang diagnostic निकालो
#पूर्ण_अगर
      अगर(cmsg->cmsg_level == IPPROTO_IP &&
#अगर_घोषित __APPLE__
          cmsg->cmsg_type == IP_RECVTOS
#अन्यथा
          cmsg->cmsg_type == IP_TOS
#पूर्ण_अगर
          && cmsg->cmsg_len) {
        वापस *(uint8_t *)(CMSG_DATA(cmsg)) & IPTOS_ECN_MASK;
      }
    }
    अवरोध;
  हाल AF_INET6:
  /* Workaround musl CMSG_NXTHDR issue */
#अगर defined(__clang__) && !defined(__GLIBC__)
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wsign-compare"
#आशय clang diagnostic ignored "-Wcast-align"
#पूर्ण_अगर
    क्रम(cmsg = CMSG_FIRSTHDR(संदेश); cmsg; cmsg = CMSG_NXTHDR(संदेश, cmsg)) {
#अगर defined(__clang__) && !defined(__GLIBC__)
#आशय clang diagnostic निकालो
#पूर्ण_अगर
      अगर(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS &&
         cmsg->cmsg_len) {
        अचिन्हित पूर्णांक tos;

        स_नकल(&tos, CMSG_DATA(cmsg), माप(पूर्णांक));

        वापस (uint8_t)(tos & IPTOS_ECN_MASK);
      }
    }
    अवरोध;
  }
  वापस 0;
}
#अन्यथा
#घोषणा vquic_msghdr_get_ecn(a, b)      0
#पूर्ण_अगर /* HAVE_SENDMMSG || HAVE_SENDMSG || HAVE_APPLE_MSG_X ... */

#अगर_घोषित HAVE_SENDMMSG

अटल CURLcode recvmmsg_packets(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, व्योम *userp,
                                 माप_प्रकार *pnread)
{
#अगर defined(__linux__) && defined(UDP_GRO)
#घोषणा MMSG_अंक  16
#घोषणा UDP_GRO_CNT_MAX  64
#घोषणा CMSG_PER_संदेश_आकार    (2 * CMSG_SPACE(माप(पूर्णांक)))
#अन्यथा
#घोषणा MMSG_अंक  64
#घोषणा UDP_GRO_CNT_MAX  1
#घोषणा CMSG_PER_संदेश_आकार    CMSG_SPACE(माप(पूर्णांक))
#पूर्ण_अगर
#घोषणा संदेश_BUF_आकार  (UDP_GRO_CNT_MAX * 1500)
  काष्ठा iovec msg_iov[MMSG_अंक];
  काष्ठा mmsghdr mmsg[MMSG_अंक];
  uint8_t संदेश_ctrl[MMSG_अंक * CMSG_PER_संदेश_आकार];
  काष्ठा sockaddr_storage remote_addr[MMSG_अंक];
  माप_प्रकार योग_nread = 0, pkts = 0;
#अगर_घोषित CURLVERBOSE
  माप_प्रकार calls = 0;
#पूर्ण_अगर
  पूर्णांक mcount, i, n;
  अक्षर errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार gso_आकार;
  अक्षर *sockbuf = शून्य;
  uint8_t (*bufs)[संदेश_BUF_आकार] = शून्य;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  परिणाम = Curl_multi_xfer_sockbuf_borrow(तथ्य, MMSG_अंक * संदेश_BUF_आकार,
                                          &sockbuf);
  अगर(परिणाम)
    जाओ out;
  bufs = (uint8_t (*)[संदेश_BUF_आकार])sockbuf;

  योग_nread = 0;
  जबतक(pkts < max_pkts) {
    n = (पूर्णांक)CURLMIN(CURLMIN(MMSG_अंक, IOV_MAX), max_pkts);
    स_रखो(&mmsg, 0, माप(mmsg));
    क्रम(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = माप(bufs[i]);
      mmsg[i].संदेश_hdr.msg_iov = &msg_iov[i];
      mmsg[i].संदेश_hdr.संदेश_iovlen = 1;
      mmsg[i].संदेश_hdr.संदेश_name = &remote_addr[i];
      mmsg[i].संदेश_hdr.msg_namelen = माप(remote_addr[i]);
      mmsg[i].संदेश_hdr.संदेश_control = &संदेश_ctrl[i * CMSG_PER_संदेश_आकार];
      mmsg[i].संदेश_hdr.msg_controllen = CMSG_PER_संदेश_आकार;
    }

    जबतक((mcount = recvmmsg(qctx->sockfd, mmsg, n, 0, शून्य)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    अगर(mcount == -1) {
      अगर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(तथ्य, cf, "ingress, recvmmsg -> EAGAIN");
        जाओ out;
      }
      अगर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        काष्ठा ip_quadruple ip;
        अगर(!Curl_cf_socket_peek(cf->अग्र, तथ्य, शून्य, शून्य, &ip))
          failf(तथ्य, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        परिणाम = CURLE_COULDNT_CONNECT;
        जाओ out;
      }
      curlx_strerror(SOCKERRNO, errstr, माप(errstr));
      failf(तथ्य, "QUIC: recvmmsg() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }

    VERBOSE(++calls);
    क्रम(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      अगर(!mmsg[i].संदेश_len) {
        ++pkts;
        जारी;
      }
      योग_nread += mmsg[i].संदेश_len;

      ecn = vquic_msghdr_get_ecn(&mmsg[i].संदेश_hdr, remote_addr[i].ss_family);
      gso_आकार = vquic_msghdr_get_udp_gro(&mmsg[i].संदेश_hdr);
      अगर(gso_आकार == 0)
        gso_आकार = mmsg[i].संदेश_len;

      परिणाम = recv_cb(bufs[i], mmsg[i].संदेश_len, gso_आकार,
                       mmsg[i].संदेश_hdr.संदेश_name,
                       mmsg[i].संदेश_hdr.msg_namelen, ecn, userp);
      अगर(परिणाम)
        जाओ out;
      pkts += (mmsg[i].संदेश_len + gso_आकार - 1) / gso_आकार;
    }
  }

out:
  अगर(योग_nread || परिणाम)
    CURL_TRC_CF(तथ्य, cf,
                "vquic_recvmmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णांक)परिणाम);
  Curl_multi_xfer_sockbuf_release(तथ्य, sockbuf);
  *pnread = योग_nread;
  वापस परिणाम;
}

#या_अगर defined(HAVE_APPLE_संदेश_X)

अटल CURLcode recvmsg_x_packets(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा cf_quic_ctx *qctx,
                                  माप_प्रकार max_pkts,
                                  Curl_vquic_recv_pkts_cb *recv_cb,
                                  व्योम *userp, माप_प्रकार *pnread)
{
#घोषणा संदेश_X_अंक  64
#घोषणा संदेश_BUF_आकार  (2048)
#घोषणा CMSG_PER_संदेश_आकार     CMSG_SPACE(माप(पूर्णांक))
  काष्ठा iovec msg_iov[संदेश_X_अंक];
  काष्ठा msghdr_x mmsg[संदेश_X_अंक];
  uint8_t संदेश_ctrl[संदेश_X_अंक * CMSG_PER_संदेश_आकार];
  काष्ठा sockaddr_storage remote_addr[संदेश_X_अंक];
  माप_प्रकार योग_nread = 0, pkts = 0;
#अगर_घोषित CURLVERBOSE
  माप_प्रकार calls = 0;
#पूर्ण_अगर
  पूर्णांक mcount, i;
  अक्षर errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार gso_आकार;
  अक्षर *sockbuf = शून्य;
  uint8_t (*bufs)[संदेश_BUF_आकार] = शून्य;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  परिणाम = Curl_multi_xfer_sockbuf_borrow(तथ्य, संदेश_X_अंक * संदेश_BUF_आकार,
                                          &sockbuf);
  अगर(परिणाम)
    जाओ out;
  bufs = (uint8_t (*)[संदेश_BUF_आकार])sockbuf;

  योग_nread = 0;
  जबतक(pkts < max_pkts) {
    पूर्णांक n = (पूर्णांक)CURLMIN(CURLMIN(संदेश_X_अंक, IOV_MAX), max_pkts);
    स_रखो(&mmsg, 0, माप(mmsg));
    क्रम(i = 0; i < n; ++i) {
      msg_iov[i].iov_base = bufs[i];
      msg_iov[i].iov_len = माप(bufs[i]);
      mmsg[i].msg_iov = &msg_iov[i];
      mmsg[i].संदेश_iovlen = 1;
      mmsg[i].संदेश_name = &remote_addr[i];
      mmsg[i].msg_namelen = माप(remote_addr[i]);
      mmsg[i].संदेश_control = &संदेश_ctrl[i * CMSG_PER_संदेश_आकार];
      mmsg[i].msg_controllen = CMSG_PER_संदेश_आकार;
    }

#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
#पूर्ण_अगर
    जबतक((mcount = syscall(SYS_recvmsg_x, qctx->sockfd, mmsg, n, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
#अगर defined(CURL_HAVE_DIAG) && defined(__APPLE__)
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
    अगर(mcount == -1) {
      अगर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(तथ्य, cf, "ingress, recvmsg_x -> EAGAIN");
        जाओ out;
      }
      अगर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        काष्ठा ip_quadruple ip;
        अगर(!Curl_cf_socket_peek(cf->अग्र, तथ्य, शून्य, शून्य, &ip))
          failf(तथ्य, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        परिणाम = CURLE_COULDNT_CONNECT;
        जाओ out;
      }
      curlx_strerror(SOCKERRNO, errstr, माप(errstr));
      failf(तथ्य, "QUIC: recvmsg_x() unexpectedly returned %d (errno=%d; %s)",
            mcount, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }

    VERBOSE(++calls);
    क्रम(i = 0; i < mcount; ++i) {
      /* A zero-length UDP packet is no QUIC packet. Ignore. */
      अगर(!mmsg[i].संदेश_datalen) {
        ++pkts;
        जारी;
      }
      योग_nread += mmsg[i].संदेश_datalen;
      ecn = vquic_msghdr_get_ecn((काष्ठा msghdr *)&mmsg[i],
                                 remote_addr[i].ss_family);
      gso_आकार = mmsg[i].संदेश_datalen;

      परिणाम = recv_cb(bufs[i], mmsg[i].संदेश_datalen, gso_आकार,
                       mmsg[i].संदेश_name,
                       mmsg[i].msg_namelen, ecn, userp);
      अगर(परिणाम)
        जाओ out;
      pkts += (mmsg[i].संदेश_datalen + gso_आकार - 1) / gso_आकार;
    }
  }

out:
  अगर(योग_nread || परिणाम)
    CURL_TRC_CF(तथ्य, cf,
                "vquic_recvmsg_x(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णांक)परिणाम);
  Curl_multi_xfer_sockbuf_release(तथ्य, sockbuf);
  *pnread = योग_nread;
  वापस परिणाम;
}

#या_अगर defined(HAVE_SENDMSG)
अटल CURLcode recvmsg_packets(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा cf_quic_ctx *qctx,
                                माप_प्रकार max_pkts,
                                Curl_vquic_recv_pkts_cb *recv_cb, व्योम *userp,
                                माप_प्रकार *pnread)
{
#घोषणा CMSG_PER_संदेश_आकार    CMSG_SPACE(माप(पूर्णांक))
  काष्ठा iovec msg_iov;
  काष्ठा msghdr संदेश;
  uint8_t buf[64 * 1024];
  काष्ठा sockaddr_storage remote_addr;
  माप_प्रकार योग_nread, pkts, calls;
  ssize_t rc;
  माप_प्रकार nread;
  अक्षर errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;
  uint8_t संदेश_ctrl[CMSG_PER_संदेश_आकार];
  माप_प्रकार gso_आकार;
  uint8_t ecn = 0;

  DEBUGASSERT(max_pkts > 0);
  क्रम(pkts = 0, योग_nread = 0, calls = 0; pkts < max_pkts;) {
    /* fully initialize this on each call to `recvmsg()`. There seem to
     * operating systems out there that mess with `msg_iov.iov_len`. */
    स_रखो(&संदेश, 0, माप(संदेश));
    msg_iov.iov_base = buf;
    msg_iov.iov_len = माप(buf);
    संदेश.msg_iov = &msg_iov;
    संदेश.संदेश_iovlen = 1;
    संदेश.संदेश_control = संदेश_ctrl;
    संदेश.संदेश_name = &remote_addr;
    संदेश.msg_namelen = माप(remote_addr);
    संदेश.msg_controllen = CMSG_PER_संदेश_आकार;

    जबतक((rc = recvmsg(qctx->sockfd, &संदेश, 0)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    अगर(!curlx_sztouz(rc, &nread)) {
      अगर(SOCK_EAGAIN(SOCKERRNO)) {
        जाओ out;
      }
      अगर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        काष्ठा ip_quadruple ip;
        अगर(!Curl_cf_socket_peek(cf->अग्र, तथ्य, शून्य, शून्य, &ip))
          failf(तथ्य, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        परिणाम = CURLE_COULDNT_CONNECT;
        जाओ out;
      }
      curlx_strerror(SOCKERRNO, errstr, माप(errstr));
      failf(तथ्य, "QUIC: recvmsg() unexpectedly returned %zd (errno=%d; %s)",
            rc, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }

    योग_nread += nread;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    अगर(!nread) {
      ++pkts;
      जारी;
    }

    ecn = vquic_msghdr_get_ecn(&संदेश, remote_addr.ss_family);
    gso_आकार = vquic_msghdr_get_udp_gro(&संदेश);
    अगर(gso_आकार == 0)
      gso_आकार = nread;

    परिणाम = recv_cb(buf, nread, gso_आकार,
                     संदेश.संदेश_name, संदेश.msg_namelen, ecn, userp);
    अगर(परिणाम)
      जाओ out;
    pkts += (nread + gso_आकार - 1) / gso_आकार;
  }

out:
  अगर(योग_nread || परिणाम)
    CURL_TRC_CF(तथ्य, cf,
                "vquic_recvmsg(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णांक)परिणाम);
  *pnread = योग_nread;
  वापस परिणाम;
}

#अन्यथा /* HAVE_SENDMMSG || HAVE_SENDMSG */
अटल CURLcode recvfrom_packets(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, व्योम *userp,
                                 माप_प्रकार *pnread)
{
  uint8_t buf[64 * 1024];
  पूर्णांक bufsize = (पूर्णांक)माप(buf);
  काष्ठा sockaddr_storage remote_addr;
  socklen_t remote_addrlen = माप(remote_addr);
  माप_प्रकार योग_nread, pkts, calls = 0, nread;
  ssize_t rv;
  अक्षर errstr[STRERROR_LEN];
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(max_pkts > 0);
  क्रम(pkts = 0, योग_nread = 0; pkts < max_pkts;) {
    जबतक((rv = recvfrom(qctx->sockfd, (अक्षर *)buf, bufsize, 0,
                         (काष्ठा sockaddr *)&remote_addr,
                         &remote_addrlen)) == -1 &&
          (SOCKERRNO == SOCKEINTR || SOCKERRNO == SOCKEMSGSIZE))
      ;
    अगर(!curlx_sztouz(rv, &nread)) {
      अगर(SOCK_EAGAIN(SOCKERRNO)) {
        CURL_TRC_CF(तथ्य, cf, "ingress, recvfrom -> EAGAIN");
        जाओ out;
      }
      अगर(!cf->connected && SOCKERRNO == SOCKECONNREFUSED) {
        काष्ठा ip_quadruple ip;
        अगर(!Curl_cf_socket_peek(cf->अग्र, तथ्य, शून्य, शून्य, &ip))
          failf(तथ्य, "QUIC: connection to %s port %u refused",
                ip.remote_ip, ip.remote_port);
        परिणाम = CURLE_COULDNT_CONNECT;
        जाओ out;
      }
      curlx_strerror(SOCKERRNO, errstr, माप(errstr));
      failf(तथ्य, "QUIC: recvfrom() unexpectedly returned %zd (errno=%d; %s)",
            rv, SOCKERRNO, errstr);
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }

    ++pkts;
    ++calls;

    /* A 0-length UDP packet is no QUIC packet */
    अगर(!nread)
      जारी;

    योग_nread += nread;
    परिणाम = recv_cb(buf, nread, nread, &remote_addr, remote_addrlen,
                     0, userp);
    अगर(परिणाम)
      जाओ out;
  }

out:
  अगर(योग_nread || परिणाम)
    CURL_TRC_CF(तथ्य, cf,
                "vquic_recvfrom(len=%zu, packets=%zu, calls=%zu) -> %d",
                योग_nread, pkts, calls, (पूर्णांक)परिणाम);
  *pnread = योग_nread;
  वापस परिणाम;
}
#पूर्ण_अगर /* !HAVE_SENDMMSG && !HAVE_SENDMSG */

CURLcode Curl_vquic_recv_packets(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb, व्योम *userp)
{
  CURLcode परिणाम;
  माप_प्रकार nread = 0;
#अगर_घोषित HAVE_SENDMMSG
  परिणाम = recvmmsg_packets(cf, तथ्य, qctx, max_pkts, recv_cb, userp, &nread);
#या_अगर defined(HAVE_APPLE_संदेश_X)
  परिणाम = recvmsg_x_packets(cf, तथ्य, qctx, max_pkts, recv_cb, userp,
                             &nread);
#या_अगर defined(HAVE_SENDMSG)
  परिणाम = recvmsg_packets(cf, तथ्य, qctx, max_pkts, recv_cb, userp, &nread);
#अन्यथा
  परिणाम = recvfrom_packets(cf, तथ्य, qctx, max_pkts, recv_cb, userp, &nread);
#पूर्ण_अगर
  अगर(!परिणाम) {
    /* EAGAIN is no error, only count actual packets as a reply */
    अगर(!qctx->got_first_byte && nread) {
      qctx->got_first_byte = TRUE;
      qctx->first_byte_at = qctx->last_op;
    }
    qctx->last_io = qctx->last_op;
  }
  वापस परिणाम;
}

/*
 * If the QLOGDIR environment variable is set, open and return a file
 * descriptor to write the log to.
 *
 * This function returns error if something failed outside of failing to
 * create the file. Open file success is deemed by seeing if the returned fd
 * is != -1.
 */
CURLcode Curl_qlogdir(काष्ठा Curl_easy *तथ्य,
                      अचिन्हित अक्षर *scid,
                      माप_प्रकार scidlen,
                      पूर्णांक *qlogfdp)
{
  अक्षर *qlog_dir = curl_getenv("QLOGDIR");
  *qlogfdp = -1;
  अगर(qlog_dir) {
    काष्ठा dynbuf fname;
    CURLcode परिणाम;
    अचिन्हित पूर्णांक i;
    curlx_dyn_init(&fname, DYN_QLOG_NAME);
    परिणाम = curlx_dyn_add(&fname, qlog_dir);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_add(&fname, "/");
    क्रम(i = 0; (i < scidlen) && !परिणाम; i++) {
      अक्षर hex[3];
      curl_msnprintf(hex, 3, "%02x", scid[i]);
      परिणाम = curlx_dyn_add(&fname, hex);
    }
    अगर(!परिणाम)
      परिणाम = curlx_dyn_add(&fname, ".sqlog");

    अगर(!परिणाम) {
      पूर्णांक qlogfd = curlx_open(curlx_dyn_ptr(&fname),
                              O_WRONLY | O_CREAT | CURL_O_द्विआधारी,
#अगर_घोषित _WIN32
                              तथ्य->set.new_file_perms & (_S_IREAD | _S_IWRITE)
#अन्यथा
                              (mode_t)तथ्य->set.new_file_perms
#पूर्ण_अगर
                              );
      अगर(qlogfd != -1)
        *qlogfdp = qlogfd;
    }
    curlx_dyn_free(&fname);
    curlx_free(qlog_dir);
    अगर(परिणाम)
      वापस परिणाम;
  }

  वापस CURLE_OK;
}

CURLcode Curl_cf_quic_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_peer *origin,
                                   काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  (व्योम)तथ्य; /* not used in all cases and compilers are stupid */
#अगर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  परिणाम = Curl_cf_ngtcp2_जोड़ो_after(cf_at, origin, peer, &cf);
#या_अगर defined(USE_QUICHE)
  परिणाम = Curl_cf_quiche_जोड़ो_after(cf_at, origin, peer, &cf);
#अन्यथा
  (व्योम)cf)
  (व्योम)cf_at;
  (व्योम)origin;
  (व्योम)peer;
  परिणाम = CURLE_NOT_BUILT_IN;
#पूर्ण_अगर

#अगर defined(USE_HTTPSRR) && defined(USE_ECH)
  /* When using ECH, kick off the HTTPS-RR resolve */
  अगर(!परिणाम && (origin->scheme->family == CURLPROTO_HTTP) &&
     Curl_ssl_need_httpsrr(cf)) {
    परिणाम = Curl_conn_dns_add_https_resolve(तथ्य, cf->conn,
                                             cf->sockindex, origin);
  }
#पूर्ण_अगर /* USE_HTTPSRR && USE_ECH */
  वापस परिणाम;
}

CURLcode Curl_cf_quic_create(काष्ठा Curl_cfilter **pcf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_peer *origin,
                             काष्ठा Curl_peer *peer,
                             uint8_t transport_peer,
                             काष्ठा connectdata *conn,
                             काष्ठा Curl_sockaddr_ex *addr,
                             काष्ठा Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport)
{
  (व्योम)transport_peer;
  (व्योम)tunnel_transport;
  (व्योम)tunnel_peer;
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#अगर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  वापस Curl_cf_ngtcp2_create(pcf, तथ्य, origin, peer, conn, addr);
#या_अगर defined(USE_QUICHE)
  वापस Curl_cf_quiche_create(pcf, तथ्य, origin, peer, conn, addr);
#अन्यथा
  *pcf = शून्य;
  (व्योम)तथ्य;
  (व्योम)origin;
  (व्योम)peer;
  (व्योम)conn;
  (व्योम)addr;
  (व्योम)tunnel_peer;
  (व्योम)tunnel_transport;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

#अगर !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_peer *origin,
                                       काष्ठा Curl_peer *peer,
                                       काष्ठा Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport)
{
#अगर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  वापस Curl_cf_ngtcp2_proxy_जोड़ो_after(cf_at, तथ्य, origin, peer,
                                           tunnel_peer, tunnel_transport);
#अन्यथा
  (व्योम)cf_at;
  (व्योम)तथ्य;
  (व्योम)origin;
  (व्योम)peer;
  (व्योम)tunnel_peer;
  (व्योम)tunnel_transport;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

CURLcode Curl_cf_h3_proxy_create(काष्ठा Curl_cfilter **pcf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_peer *origin,
                                 काष्ठा Curl_peer *peer,
                                 uint8_t transport_peer,
                                 काष्ठा connectdata *conn,
                                 काष्ठा Curl_sockaddr_ex *addr,
                                 काष्ठा Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport)
{
  DEBUGASSERT(transport_peer == TRNSPRT_QUIC);
#अगर defined(USE_NGTCP2) && defined(USE_NGHTTP3)
  वापस Curl_cf_ngtcp2_proxy_create(pcf, तथ्य, origin, peer, transport_peer,
                                     conn, addr,
                                     tunnel_peer, tunnel_transport);
#अन्यथा
  *pcf = शून्य;
  (व्योम)तथ्य;
  (व्योम)conn;
  (व्योम)addr;
  (व्योम)peer;
  (व्योम)transport_peer;
  (व्योम)tunnel_peer;
  (व्योम)tunnel_transport;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

CURLcode Curl_conn_may_http3(काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn)
{
  अगर(Curl_conn_get_first_peer(conn, FIRSTSOCKET)->unix_socket) {
    failf(तथ्य, "HTTP/3 cannot be used over UNIX domain sockets");
    वापस CURLE_QUIC_CONNECT_ERROR;
  }
  अगर(!(तथ्य->state.origin->scheme->flags & PROTOPT_SSL)) {
    failf(तथ्य, "HTTP/3 requested for non-HTTPS URL");
    वापस CURLE_URL_MALFORMAT;
  }
#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(conn->socks_proxy.peer) {
    failf(तथ्य, "HTTP/3 is not supported over a SOCKS proxy");
    वापस CURLE_URL_MALFORMAT;
  }
#अन्यथा
  (व्योम)conn;
#पूर्ण_अगर

  वापस CURLE_OK;
}

#अगर_घोषित CURLVERBOSE
स्थिर अक्षर *Curl_vquic_h3_err_str(uint64_t error_code)
{
  अगर(error_code <= अच_पूर्णांक_उच्च) {
    चयन((अचिन्हित पूर्णांक)error_code) {
    हाल CURL_H3_ERR_NO_ERROR:
      वापस "NO_ERROR";
    हाल CURL_H3_ERR_GENERAL_PROTOCOL_ERROR:
      वापस "GENERAL_PROTOCOL_ERROR";
    हाल CURL_H3_ERR_INTERNAL_ERROR:
      वापस "INTERNAL_ERROR";
    हाल CURL_H3_ERR_STREAM_CREATION_ERROR:
      वापस "STREAM_CREATION_ERROR";
    हाल CURL_H3_ERR_CLOSED_CRITICAL_STREAM:
      वापस "CLOSED_CRITICAL_STREAM";
    हाल CURL_H3_ERR_FRAME_UNEXPECTED:
      वापस "FRAME_UNEXPECTED";
    हाल CURL_H3_ERR_FRAME_ERROR:
      वापस "FRAME_ERROR";
    हाल CURL_H3_ERR_EXCESSIVE_LOAD:
      वापस "EXCESSIVE_LOAD";
    हाल CURL_H3_ERR_ID_ERROR:
      वापस "ID_ERROR";
    हाल CURL_H3_ERR_SETTINGS_ERROR:
      वापस "SETTINGS_ERROR";
    हाल CURL_H3_ERR_MISSING_SETTINGS:
      वापस "MISSING_SETTINGS";
    हाल CURL_H3_ERR_REQUEST_REJECTED:
      वापस "REQUEST_REJECTED";
    हाल CURL_H3_ERR_REQUEST_CANCELLED:
      वापस "REQUEST_CANCELLED";
    हाल CURL_H3_ERR_REQUEST_INCOMPLETE:
      वापस "REQUEST_INCOMPLETE";
    हाल CURL_H3_ERR_MESSAGE_ERROR:
      वापस "MESSAGE_ERROR";
    हाल CURL_H3_ERR_CONNECT_ERROR:
      वापस "CONNECT_ERROR";
    हाल CURL_H3_ERR_VERSION_FALLBACK:
      वापस "VERSION_FALLBACK";
    शेष:
      अवरोध;
    }
  }
  /* RFC 9114 ch. 8.1 + 9, reserved future error codes that are NO_ERROR */
  अगर((error_code >= 0x21) && !((error_code - 0x21) % 0x1f))
    वापस "NO_ERROR";
  वापस "unknown";
}
#पूर्ण_अगर /* CURLVERBOSE */

#अगर defined(USE_NGTCP2) || defined(USE_NGHTTP3)

अटल व्योम *vquic_ngtcp2_malloc(माप_प्रकार आकार, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  वापस Curl_cmalloc(आकार);
}

अटल व्योम vquic_ngtcp2_free(व्योम *ptr, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  Curl_cfree(ptr);
}

अटल व्योम *vquic_ngtcp2_calloc(माप_प्रकार nmemb, माप_प्रकार आकार, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  वापस Curl_ccalloc(nmemb, आकार);
}

अटल व्योम *vquic_ngtcp2_realloc(व्योम *ptr, माप_प्रकार आकार, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  वापस Curl_crealloc(ptr, आकार);
}

#अगर_घोषित USE_NGTCP2
अटल काष्ठा ngtcp2_mem vquic_ngtcp2_mem = {
  शून्य,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
काष्ठा ngtcp2_mem *Curl_ngtcp2_mem(व्योम)
{
  वापस &vquic_ngtcp2_mem;
}
#पूर्ण_अगर

#अगर_घोषित USE_NGHTTP3
अटल काष्ठा nghttp3_mem vquic_nghttp3_mem = {
  शून्य,
  vquic_ngtcp2_malloc,
  vquic_ngtcp2_free,
  vquic_ngtcp2_calloc,
  vquic_ngtcp2_realloc
};
काष्ठा nghttp3_mem *Curl_nghttp3_mem(व्योम)
{
  वापस &vquic_nghttp3_mem;
}
#पूर्ण_अगर

#पूर्ण_अगर /* USE_NGTCP2 || USE_NGHTTP3 */

#अन्यथा /* CURL_DISABLE_HTTP || !USE_HTTP3 */

CURLcode Curl_conn_may_http3(काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn)
{
  (व्योम)तथ्य;
  (व्योम)conn;
  DEBUGF(infof(तथ्य, "QUIC is not supported in this build"));
  वापस CURLE_NOT_BUILT_IN;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_HTTP3 */
