/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Björn Stenberg, <bjorn@haxx.se>
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

#اگر_تعریف_نشده CURL_DISABLE_MQTT

#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "mqtt.h"
#ضمن "select.h"
#ضمن "url.h"
#ضمن "escape.h"
#ضمن "rand.h"
#ضمن "cfilters.h"
#ضمن "connect.h"

/* first byte is command.
   second byte is for flags. */
#تعریف_کن MQTT_پیام_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#تعریف_کن MQTT_پیام_PUBLISH    0x30
#تعریف_کن MQTT_پیام_SUBSCRIBE  0x82
#تعریف_کن MQTT_پیام_SUBACK     0x90
#تعریف_کن MQTT_پیام_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#تعریف_کن MQTT_پیام_PINGRESP   0xD0

#تعریف_کن MQTT_CONNACK_LEN  2
#تعریف_کن MQTT_SUBACK_LEN   3
#تعریف_کن MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#تعریف_کن CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

تعداد mqttstate {
  MQTT_اولی,             /* 0 */
  MQTT_مانده_درازی,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

بنية mqtt_conn {
  تعداد mqttstate state;
  تعداد mqttstate nextstate; /* switch to this after remaining length is
انجامشده*/
  بیعلامت صحيح packetid;
};

/* protocol-specific transfer-related data */
بنية MQTT {
  بنية dynbuf sendbuf;
  /* when receiving */
  بنية dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t مانده_درازی;
  بیعلامت حرف pkt_hd[4]; /* for decoding the arriving packet length */
  بنية curltime آخرین_زمان; /* last time we sent or received data */
  بیعلامت حرف firstbyte;
  بیت(pingsent); /* 1 while we wait for ping response */
};

ساكن فراغ mqtt_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية MQTT *mq = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ساكن فراغ mqtt_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(مدخل);
}

ساكن CURLcode mqtt_راهاندازی_conn(بنية Curl_easy *داده,
                                بنية connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  بنية mqtt_conn *mqtt;
  بنية MQTT *mq;

  mqtt = curlx_calloc(1, حجم(*mqtt));
  إذا(!mqtt ||
     Curl_conn_meta_مجموعه(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  mq = curlx_calloc(1, حجم(بنية MQTT));
  إذا(!mq)
    أرجع CURLE_بیرون_از_حافظه;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  إذا(Curl_meta_مجموعه(داده, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_send(بنية Curl_easy *داده,
                          ثابت حرف *buf, size_t len)
{
  size_t n;
  CURLcode نتیجه;
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  نتیجه = Curl_xfer_send(داده, buf, len, FALSE, &n);
  إذا(نتیجه)
    أرجع نتیجه;
  mq->آخرین_زمان = *Curl_pgrs_now(داده);
  Curl_debug(داده, CURLINFO_سرآیند_بیرون, buf, n);
  إذا(len != n)
    نتیجه = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  أرجع نتیجه;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ساكن CURLcode mqtt_flush(بنية Curl_easy *داده)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode نتیجه;

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  إذا(!len)
    أرجع CURLE_OK;

  نتیجه = Curl_xfer_send(داده, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  إذا(نتیجه)
    أرجع نتیجه;
  mq->آخرین_زمان = *Curl_pgrs_now(داده);
  Curl_debug(داده, CURLINFO_سرآیند_بیرون, curlx_dyn_ptr(&mq->sendbuf), n);
  أرجع curlx_dyn_دم(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
حالتها*/
ساكن CURLcode mqtt_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  إذا(mq && curlx_dyn_len(&mq->sendbuf))
    أرجع Curl_pollset_جمع_کن_بیرون(داده, ps, داده->conn->sock[FIRSTSOCKET]);
  أرجع Curl_pollset_جمع_کن_درون(داده, ps, داده->conn->sock[FIRSTSOCKET]);
}

ساكن صحيح mqtt_encode_len(حرف *buf, size_t len)
{
  صحيح i;

  لكل(i = 0; (len > 0) && (i < 4); i++) {
    بیعلامت حرف encoded;
    encoded = len % 0x80;
    len /= 0x80;
    إذا(len)
      encoded |= 0x80;
    buf[i] = (حرف)encoded;
  }

  أرجع i;
}

/* add the passwd to the CONNECT packet */
ساكن صحيح جمع_کن_passwd(ثابت حرف *passwd, ثابت size_t plen,
                      حرف *pkt, ثابت size_t شروع, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  إذا(plen > 0xffff)
    أرجع 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[شروع] = (حرف)((plen >> 8) & 0xFF);
  pkt[شروع + 1] = (حرف)(plen & 0xFF);
  کپی_حافظه(&pkt[شروع + 2], passwd, plen);
  أرجع 0;
}

/* add user to the CONNECT packet */
ساكن صحيح جمع_کن_user(ثابت حرف *username, ثابت size_t ulen,
                    بیعلامت حرف *pkt, ثابت size_t شروع, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  إذا(ulen > 0xffff)
    أرجع 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[شروع] = (بیعلامت حرف)((ulen >> 8) & 0xFF);
  pkt[شروع + 1] = (بیعلامت حرف)(ulen & 0xFF);
  کپی_حافظه(&pkt[شروع + 2], username, ulen);
  أرجع 0;
}

/* add client ID to the CONNECT packet */
ساكن صحيح جمع_کن_client_id(ثابت حرف *client_id, ثابت size_t client_id_len,
                         حرف *pkt, ثابت size_t شروع)
{
  إذا(client_id_len != MQTT_CLIENTID_LEN)
    أرجع 1;
  pkt[شروع] = 0x00;
  pkt[شروع + 1] = MQTT_CLIENTID_LEN;
  کپی_حافظه(&pkt[شروع + 2], client_id, MQTT_CLIENTID_LEN);
  أرجع 0;
}

/* Set initial values of CONNECT packet */
ساكن صحيح init_connpack(حرف *packet, حرف *remain, صحيح remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_پیام_CONNECT;
  /* remaining length field */
  کپی_حافظه(&packet[1], remain, remain_pos);
  /* Fixed header ends */

  /* Variable header starts */
  /* protocol length */
  packet[remain_pos + 1] = 0x00;
  packet[remain_pos + 2] = 0x04;
  /* protocol name */
  packet[remain_pos + 3] = 'M';
  packet[remain_pos + 4] = 'Q';
  packet[remain_pos + 5] = 'T';
  packet[remain_pos + 6] = 'T';
  /* protocol level */
  packet[remain_pos + 7] = 0x04;
  /* CONNECT flag: CleanSession */
  packet[remain_pos + 8] = 0x02;
  /* keep-alive 0 = disabled */
  packet[remain_pos + 9] = 0x00;
  packet[remain_pos + 10] = 0x3c;
  /* end of variable header */
  أرجع remain_pos + 10;
}

ساكن CURLcode mqtt_connect(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح pos = 0;
  صحيح rc = 0;
  /* remain length */
  صحيح remain_pos = 0;
  حرف remain[4] = { 0 };
  size_t packetlen = 0;
  size_t شروع_user = 0;
  size_t شروع_pwd = 0;
  حرف client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ثابت size_t clen = CURL_CSTRLEN("curl");
  حرف *packet = عدم;

  /* extracting username from request */
  بنية Curl_creds *creds = داده->state.creds;
  ثابت size_t ulen = creds ? طول_رشته(creds->user) : 0;
  ثابت size_t plen = creds ? طول_رشته(creds->passwd) : 0;
  ثابت size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  إذا(packetlen > 0xFFFFFFF)
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  packet = curlx_calloc(1, packetlen);
  إذا(!packet)
    أرجع CURLE_بیرون_از_حافظه;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  نتیجه = Curl_rand_alnum(داده, (بیعلامت حرف *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = جمع_کن_client_id(client_id, طول_رشته(client_id), packet, pos + 1);
  إذا(rc) {
    failf(داده, "Client ID length mismatched: [%zu]", طول_رشته(client_id));
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
    اذهب انتها;
  }
  infof(داده, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  شروع_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  شروع_pwd = شروع_user + ulen;
  /* if username was provided, add it to the packet */
  إذا(ulen) {
    شروع_pwd += 2;

    rc = جمع_کن_user(creds->user, ulen,
                  (بیعلامت حرف *)packet, شروع_user, remain_pos);
    إذا(rc) {
      failf(داده, "Username too long: [%zu]", ulen);
      نتیجه = CURLE_WEIRD_SERVER_پاسخ;
      اذهب انتها;
    }
  }

  /* if passwd was provided, add it to the packet */
  إذا(plen) {
    rc = جمع_کن_passwd(creds->passwd, plen, packet, شروع_pwd, remain_pos);
    إذا(rc) {
      failf(داده, "Password too long: [%zu]", plen);
      نتیجه = CURLE_WEIRD_SERVER_پاسخ;
      اذهب انتها;
    }
  }

  إذا(!نتیجه)
    نتیجه = mqtt_send(داده, packet, packetlen);

انتها:
  إذا(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&داده->state.creds);
  أرجع نتیجه;
}

ساكن CURLcode mqtt_disconnect(بنية Curl_easy *داده)
{
  أرجع mqtt_send(داده, "\xe0\x00", 2);
}

ساكن CURLcode mqtt_recv_atleast(بنية Curl_easy *داده, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode نتیجه;

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  إذا(rlen < nbytes) {
    بیعلامت حرف readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < حجم(readbuf));
    نتیجه = Curl_xfer_recv(داده, (حرف *)readbuf, nbytes - rlen, &nread);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(!nread) /* EOF */
       أرجع CURLE_RECV_ERROR;
    إذا(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      أرجع CURLE_بیرون_از_حافظه;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  أرجع (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ mqtt_recv_consume(بنية Curl_easy *داده, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  إذا(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    إذا(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    وإلا
      curlx_dyn_دم(&mq->recvbuf, rlen - nbytes);
  }
}

ساكن CURLcode mqtt_verify_connack(بنية Curl_easy *داده)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  CURLcode نتیجه;
  ثابت حرف *ptr;

  DEBUGASSERT(mq);
  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  إذا(mq->مانده_درازی != 2) {
    failf(داده, "CONNACK expected Remaining Length 2, got %zu",
          mq->مانده_درازی);
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  نتیجه = mqtt_recv_atleast(داده, MQTT_CONNACK_LEN);
  إذا(نتیجه)
    أرجع نتیجه;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(داده, CURLINFO_سرآیند_درون, ptr, MQTT_CONNACK_LEN);

  إذا(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(داده, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (بیعلامت حرف)ptr[0], (بیعلامت حرف)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }
  mqtt_recv_consume(داده, MQTT_CONNACK_LEN);
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_get_topic(بنية Curl_easy *داده,
                               حرف **topic, size_t *topiclen)
{
  ثابت حرف *مسیر = داده->state.به_بالا.مسیر;
  CURLcode نتیجه = CURLE_URL_MALFORMAT;
  إذا(طول_رشته(مسیر) > 1) {
    نتیجه = Curl_urldecode(مسیر + 1, 0, topic, topiclen, REJECT_CTRL);
    إذا(!نتیجه && (*topiclen > 0xffff)) {
      failf(داده, "Too long MQTT topic");
      نتیجه = CURLE_URL_MALFORMAT;
    }
  }
  وإلا
    failf(داده, "No MQTT topic found. Forgot to URL encode it?");

  أرجع نتیجه;
}

ساكن CURLcode mqtt_subscribe(بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  حرف *topic = عدم;
  size_t topiclen;
  بیعلامت حرف *packet = عدم;
  size_t packetlen;
  حرف encodedsize[4];
  size_t n;
  بنية connectdata *conn = داده->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  إذا(!mqtt)
    أرجع CURLE_FAILED_INIT;

  نتیجه = mqtt_get_topic(داده, &topic, &topiclen);
  إذا(نتیجه)
    اذهب fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((حرف *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  إذا(!packet) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

  packet[0] = MQTT_پیام_SUBSCRIBE;
  کپی_حافظه(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  کپی_حافظه(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  نتیجه = mqtt_send(داده, (ثابت حرف *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  أرجع نتیجه;
}

/*
 * Called when the first byte was already read.
 */
ساكن CURLcode mqtt_verify_suback(بنية Curl_easy *داده)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  بنية connectdata *conn = داده->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode نتیجه;
  ثابت حرف *ptr;

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  إذا(mq->مانده_درازی != 3) {
    failf(داده, "SUBACK expected Remaining Length 3, got %zu",
          mq->مانده_درازی);
    أرجع CURLE_WEIRD_SERVER_پاسخ;
  }

  نتیجه = mqtt_recv_atleast(داده, MQTT_SUBACK_LEN);
  إذا(نتیجه)
    اذهب fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(داده, CURLINFO_سرآیند_درون, ptr, MQTT_SUBACK_LEN);

  إذا(((بیعلامت حرف)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((بیعلامت حرف)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
    اذهب fail;
  }
  mqtt_recv_consume(داده, MQTT_SUBACK_LEN);
fail:
  أرجع نتیجه;
}

#تعریف_کن MAX_MQTT_پیام_اندازه 0xFFFFFFF

ساكن CURLcode mqtt_publish(بنية Curl_easy *داده)
{
  CURLcode نتیجه;
  حرف *payload = داده->مجموعه.postfields;
  size_t payloadlen;
  حرف *topic = عدم;
  size_t topiclen;
  بیعلامت حرف *pkt = عدم;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  حرف encodedbytes[4];
  curl_off_t postfieldsize = داده->مجموعه.postfieldsize;

  إذا(!payload) {
    DEBUGF(infof(داده, "mqtt_publish without payload, return bad arg"));
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  إذا(!curlx_sotouz_میگنجد(postfieldsize, &payloadlen)) {
    إذا(postfieldsize > 0) /* off_t does not fit into size_t */
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    payloadlen = طول_رشته(payload);
  }

  نتیجه = mqtt_get_topic(داده, &topic, &topiclen);
  إذا(نتیجه)
    اذهب fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  إذا((SIZE_MAX - topiclen - 2) < payloadlen) {
    نتیجه = CURLE_TOO_LARGE;
    اذهب fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  إذا(remaininglength > (MAX_MQTT_پیام_اندازه - encodelen - 1)) {
    نتیجه = CURLE_TOO_LARGE;
    اذهب fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  إذا(!pkt) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_پیام_PUBLISH;
  کپی_حافظه(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  کپی_حافظه(&pkt[i], topic, topiclen);
  i += topiclen;
  کپی_حافظه(&pkt[i], payload, payloadlen);
  i += payloadlen;
  نتیجه = mqtt_send(داده, (ثابت حرف *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  أرجع نتیجه;
}

/* return FALSE on success, TRUE on error */
ساكن bool mqtt_decode_len(size_t *lenp, ثابت بیعلامت حرف *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  بیعلامت حرف encoded = 128;

  لكل(i = 0; (i < buflen) && (encoded & 128); i++) {
    إذا(i == 4)
      أرجع TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  إذا(encoded & 128)
    /* truncated size */
    أرجع TRUE;

  *lenp = len;
  أرجع FALSE;
}

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن ثابت حرف * ثابت statenames[] = {
  "MQTT_FIRST",
  "MQTT_REMAINING_LENGTH",
  "MQTT_CONNACK",
  "MQTT_SUBACK",
  "MQTT_SUBACK_COMING",
  "MQTT_PUBWAIT",
  "MQTT_PUB_REMAIN",
  "MQTT_POST_DRAIN",
  "MQTT_DISCONNECT_DRAIN",

  "NOT A STATE"
};
#پایان_اگر

/* The only way to change state */
ساكن فراغ mqstate(بنية Curl_easy *داده,
                    تعداد mqttstate state,
                    تعداد mqttstate nextstate) /* used if state == FIRST */
{
  بنية connectdata *conn = داده->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  إذا(!mqtt)
    أرجع;
#اگر_تعریف_شده DEBUGBUILD
  infof(داده, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_اولی) ? statenames[nextstate] : "");
#پایان_اگر
  mqtt->state = state;
  إذا(state == MQTT_اولی)
    mqtt->nextstate = nextstate;
}

ساكن CURLcode mqtt_read_publish(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  size_t nread;
  size_t remlen;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  بیعلامت حرف packet;

  DEBUGASSERT(mqtt);
  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  بدل(mqtt->state) {
MQTT_SUBACK_COMING:
  حالة MQTT_SUBACK_COMING:
    نتیجه = mqtt_verify_suback(داده);
    إذا(نتیجه)
      اكسر;

    mqstate(داده, MQTT_اولی, MQTT_PUBWAIT);
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    إذا(packet == MQTT_پیام_PUBLISH)
      mqstate(داده, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    وإلا إذا(packet == MQTT_پیام_SUBACK) {
      mqstate(داده, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      اذهب MQTT_SUBACK_COMING;
    }
    وإلا إذا(packet == MQTT_پیام_DISCONNECT) {
      infof(داده, "Got DISCONNECT");
      *انجامشده = TRUE;
      اذهب انتها;
    }
    وإلا {
      نتیجه = CURLE_WEIRD_SERVER_پاسخ;
      اذهب انتها;
    }

    /* -- switched state -- */
    remlen = mq->مانده_درازی;
    infof(داده, "Remaining length: %zu bytes", remlen);
    إذا(داده->مجموعه.max_filesize &&
       (curl_off_t)remlen > داده->مجموعه.max_filesize) {
      failf(داده, "Maximum file size exceeded");
      نتیجه = CURLE_FILESIZE_EXCEEDED;
      اذهب انتها;
    }
    Curl_pgrs_مجموعه_Download_اندازه(داده, remlen);
    داده->req.bytecount = 0;
    داده->req.اندازه = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  حالة MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    حرف میانگیر[4 * 1024];
    size_t rest = mq->npacket;
    إذا(rest > حجم(میانگیر))
      rest = حجم(میانگیر);
    نتیجه = Curl_xfer_recv(داده, میانگیر, rest, &nread);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_AGAIN) {
        infof(داده, "EEEE AAAAGAIN");
      }
      اذهب انتها;
    }
    إذا(!nread) {
      infof(داده, "server disconnected");
      نتیجه = CURLE_PARTIAL_FILE;
      اذهب انتها;
    }

    /* we received something */
    mq->آخرین_زمان = *Curl_pgrs_now(داده);

    /* if QoS is set, message contains packet id */
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, میانگیر, nread);
    إذا(نتیجه)
      اذهب انتها;

    mq->npacket -= nread;
    إذا(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(داده, MQTT_اولی, MQTT_PUBWAIT);
    اكسر;
  }
  افتراضي:
    DEBUGASSERT(عدم); /* illegal state */
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
    اذهب انتها;
  }
انتها:
  أرجع نتیجه;
}

ساكن CURLcode mqtt_do(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  CURLcode نتیجه = CURLE_OK;
  *انجامشده = FALSE; /* unconditionally */

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  mq->آخرین_زمان = *Curl_pgrs_now(داده);
  mq->pingsent = FALSE;

  نتیجه = mqtt_connect(داده);
  إذا(نتیجه) {
    failf(داده, "Error %d sending MQTT CONNECT request", (صحيح)نتیجه);
    أرجع نتیجه;
  }
  mqstate(داده, MQTT_اولی, MQTT_CONNACK);
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_انجامشده(بنية Curl_easy *داده,
                          CURLcode وضعیت, bool premature)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  (فراغ)وضعیت;
  (فراغ)premature;
  إذا(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  أرجع CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ساكن CURLcode mqtt_ping(بنية Curl_easy *داده)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  إذا(mqtt->state == MQTT_اولی &&
     !mq->pingsent &&
     داده->مجموعه.upkeep_interval_ms > 0) {
    بنية curltime t = *Curl_pgrs_now(داده);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->آخرین_زمان);

    إذا(diff > داده->مجموعه.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      بیعلامت حرف packet[2] = { 0xC0, 0x00 };
      size_t packetlen = حجم(packet);

      نتیجه = mqtt_send(داده, (حرف *)packet, packetlen);
      إذا(!نتیجه) {
        mq->pingsent = TRUE;
      }
      infof(داده, "mqtt_ping: sent ping request.");
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode mqtt_doing(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية MQTT *mq = Curl_meta_get(داده, CURL_META_MQTT_EASY);
  CURLcode نتیجه = CURLE_OK;
  size_t nread;
  بیعلامت حرف recvbyte;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(داده->conn, CURL_META_MQTT_CONN);

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  *انجامشده = FALSE;

  إذا(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    نتیجه = mqtt_flush(داده);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    إذا(نتیجه || curlx_dyn_len(&mq->sendbuf))
      أرجع نتیجه;
  }

  نتیجه = mqtt_ping(داده);
  /* Packet processing may send more output, so finish PINGREQ first. */
  إذا(نتیجه || curlx_dyn_len(&mq->sendbuf))
    أرجع نتیجه;

  infof(داده, "mqtt_doing: state [%d]", (صحيح)mqtt->state);
  بدل(mqtt->state) {
  حالة MQTT_اولی:
    /* Read the initial byte only */
    نتیجه = Curl_xfer_recv(داده, (حرف *)&mq->firstbyte, 1, &nread);
    إذا(نتیجه)
      اكسر;
    وإلا إذا(!nread) {
      failf(داده, "Connection disconnected");
      *انجامشده = TRUE;
      نتیجه = CURLE_RECV_ERROR;
      اكسر;
    }
    Curl_debug(داده, CURLINFO_سرآیند_درون, (ثابت حرف *)&mq->firstbyte, 1);

    /* we received something */
    mq->آخرین_زمان = *Curl_pgrs_now(داده);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(داده, MQTT_مانده_درازی, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_مانده_درازی:
    افعل {
      نتیجه = Curl_xfer_recv(داده, (حرف *)&recvbyte, 1, &nread);
      إذا(نتیجه || !nread)
        اكسر;
      Curl_debug(داده, CURLINFO_سرآیند_درون, (ثابت حرف *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } طالما((recvbyte & 0x80) && (mq->npacket < 4));
    إذا(!نتیجه && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      نتیجه = CURLE_WEIRD_SERVER_پاسخ;
    إذا(نتیجه)
      اكسر;
    إذا(mqtt_decode_len(&mq->مانده_درازی, mq->pkt_hd, mq->npacket)) {
      نتیجه = CURLE_WEIRD_SERVER_پاسخ;
      اكسر;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ثابت بیعلامت حرف type = mq->firstbyte & 0xF0;
      ثابت بیعلامت حرف reserved = mq->firstbyte & 0x0F;
      إذا((type == MQTT_پیام_DISCONNECT || type == MQTT_پیام_PINGRESP) &&
         (mq->مانده_درازی || reserved)) {
        failf(داده,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_پیام_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->مانده_درازی, mq->firstbyte);
        نتیجه = CURLE_WEIRD_SERVER_پاسخ;
        اكسر;
      }
    }
    إذا(mq->مانده_درازی) {
      mqstate(داده, mqtt->nextstate, MQTT_NOSTATE);
      اكسر;
    }
    mqstate(داده, MQTT_اولی, MQTT_اولی);

    إذا(mq->firstbyte == MQTT_پیام_DISCONNECT) {
      infof(داده, "Got DISCONNECT");
      *انجامشده = TRUE;
    }

    /* ping response */
    إذا(mq->firstbyte == MQTT_پیام_PINGRESP) {
      infof(داده, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(داده, MQTT_اولی, MQTT_PUBWAIT);
    }
    اكسر;
  حالة MQTT_CONNACK:
    نتیجه = mqtt_verify_connack(داده);
    إذا(نتیجه)
      اكسر;

    إذا(داده->state.httpreq != HTTPREQ_POST) {
      نتیجه = mqtt_subscribe(داده);
      إذا(!نتیجه)
        mqstate(داده, MQTT_اولی, MQTT_SUBACK);
      اكسر;
    }

    نتیجه = mqtt_publish(داده);
    إذا(نتیجه)
      اكسر;
    mqstate(داده, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    إذا(curlx_dyn_len(&mq->sendbuf))
      اكسر;
    نتیجه = mqtt_disconnect(داده);
    إذا(نتیجه)
      اكسر;
    mqstate(داده, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    إذا(!curlx_dyn_len(&mq->sendbuf))
      *انجامشده = TRUE;
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
  حالة MQTT_PUB_REMAIN:
    نتیجه = mqtt_read_publish(داده, انجامشده);
    اكسر;

  افتراضي:
    failf(داده, "State not handled yet");
    *انجامشده = TRUE;
    اكسر;
  }

  إذا(نتیجه == CURLE_AGAIN)
    نتیجه = CURLE_OK;
  أرجع نتیجه;
}

#اگر_تعریف_شده USE_SSL

ساكن CURLcode mqtts_connecting(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه;

  نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, TRUE, انجامشده);
  إذا(نتیجه)
    connclose(conn);
  أرجع نتیجه;
}

/*
 * MQTTS protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtts = {
  mqtt_راهاندازی_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_انجامشده,                          /* done */
  ZERO_NULL,                          /* do_more */
  ZERO_NULL,                          /* connect_it */
  mqtts_connecting,                   /* connecting */
  mqtt_doing,                         /* doing */
  ZERO_NULL,                          /* proto_pollset */
  mqtt_pollset,                       /* doing_pollset */
  ZERO_NULL,                          /* domore_pollset */
  ZERO_NULL,                          /* perform_pollset */
  ZERO_NULL,                          /* disconnect */
  ZERO_NULL,                          /* write_resp */
  ZERO_NULL,                          /* write_resp_hd */
  ZERO_NULL,                          /* connection_is_dead */
  ZERO_NULL,                          /* attach connection */
  ZERO_NULL,                          /* follow */
};

#پایان_اگر

/*
 * MQTT protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtt = {
  mqtt_راهاندازی_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_انجامشده,                          /* done */
  ZERO_NULL,                          /* do_more */
  ZERO_NULL,                          /* connect_it */
  ZERO_NULL,                          /* connecting */
  mqtt_doing,                         /* doing */
  ZERO_NULL,                          /* proto_pollset */
  mqtt_pollset,                       /* doing_pollset */
  ZERO_NULL,                          /* domore_pollset */
  ZERO_NULL,                          /* perform_pollset */
  ZERO_NULL,                          /* disconnect */
  ZERO_NULL,                          /* write_resp */
  ZERO_NULL,                          /* write_resp_hd */
  ZERO_NULL,                          /* connection_is_dead */
  ZERO_NULL,                          /* attach connection */
  ZERO_NULL,                          /* follow */
};

#پایان_اگر /* CURL_DISABLE_MQTT */
