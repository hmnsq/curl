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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MQTT

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
#وصف_ڪريو MQTT_نياپو_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#وصف_ڪريو MQTT_نياپو_PUBLISH    0x30
#وصف_ڪريو MQTT_نياپو_SUBSCRIBE  0x82
#وصف_ڪريو MQTT_نياپو_SUBACK     0x90
#وصف_ڪريو MQTT_نياپو_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#وصف_ڪريو MQTT_نياپو_PINGRESP   0xD0

#وصف_ڪريو MQTT_CONNACK_LEN  2
#وصف_ڪريو MQTT_SUBACK_LEN   3
#وصف_ڪريو MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#وصف_ڪريو CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#وصف_ڪريو CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

تعداد mqttstate {
  MQTT_پهريون,             /* 0 */
  MQTT_بچيل_ڊيگهائي,  /* 1 */
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
پورو*/
  بنانشان صحيح packetid;
};

/* protocol-specific transfer-related data */
بنية MQTT {
  بنية dynbuf sendbuf;
  /* when receiving */
  بنية dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t بچيل_ڊيگهائي;
  بنانشان حرف pkt_hd[4]; /* for decoding the arriving packet length */
  بنية curltime آخري_وقت; /* last time we sent or received data */
  بنانشان حرف firstbyte;
  ٻٽو_انگ(pingsent); /* 1 while we wait for ping response */
};

ساكن فراغ mqtt_easy_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية MQTT *mq = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ساكن فراغ mqtt_conn_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  (فراغ)ڪنجي;
  (فراغ)klen;
  curlx_free(داخلا_2);
}

ساكن CURLcode mqtt_تياري_conn(بنية Curl_easy *معلومات,
                                بنية connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  بنية mqtt_conn *mqtt;
  بنية MQTT *mq;

  mqtt = curlx_calloc(1, حجم(*mqtt));
  إذا(!mqtt ||
     Curl_conn_meta_سيٽ(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  mq = curlx_calloc(1, حجم(بنية MQTT));
  إذا(!mq)
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  إذا(Curl_meta_سيٽ(معلومات, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_send(بنية Curl_easy *معلومات,
                          ثابت حرف *buf, size_t len)
{
  size_t n;
  CURLcode نتيجو;
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  نتيجو = Curl_xfer_send(معلومات, buf, len, FALSE, &n);
  إذا(نتيجو)
    أرجع نتيجو;
  mq->آخري_وقت = *Curl_pgrs_now(معلومات);
  Curl_debug(معلومات, CURLINFO_سرو_ٻاهريون, buf, n);
  إذا(len != n)
    نتيجو = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  أرجع نتيجو;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ساكن CURLcode mqtt_flush(بنية Curl_easy *معلومات)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode نتيجو;

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  إذا(!len)
    أرجع CURLE_OK;

  نتيجو = Curl_xfer_send(معلومات, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  إذا(نتيجو)
    أرجع نتيجو;
  mq->آخري_وقت = *Curl_pgrs_now(معلومات);
  Curl_debug(معلومات, CURLINFO_سرو_ٻاهريون, curlx_dyn_ptr(&mq->sendbuf), n);
  أرجع curlx_dyn_پڇ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
حالتون*/
ساكن CURLcode mqtt_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  إذا(mq && curlx_dyn_len(&mq->sendbuf))
    أرجع Curl_pollset_جوڙيو_ٻاهريون(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]);
  أرجع Curl_pollset_جوڙيو_اندريون(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]);
}

ساكن صحيح mqtt_encode_len(حرف *buf, size_t len)
{
  صحيح i;

  لكل(i = 0; (len > 0) && (i < 4); i++) {
    بنانشان حرف encoded;
    encoded = len % 0x80;
    len /= 0x80;
    إذا(len)
      encoded |= 0x80;
    buf[i] = (حرف)encoded;
  }

  أرجع i;
}

/* add the passwd to the CONNECT packet */
ساكن صحيح جوڙيو_passwd(ثابت حرف *passwd, ثابت size_t plen,
                      حرف *pkt, ثابت size_t شروعات, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  إذا(plen > 0xffff)
    أرجع 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[شروعات] = (حرف)((plen >> 8) & 0xFF);
  pkt[شروعات + 1] = (حرف)(plen & 0xFF);
  يادگيري_نقل(&pkt[شروعات + 2], passwd, plen);
  أرجع 0;
}

/* add user to the CONNECT packet */
ساكن صحيح جوڙيو_user(ثابت حرف *username, ثابت size_t ulen,
                    بنانشان حرف *pkt, ثابت size_t شروعات, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  إذا(ulen > 0xffff)
    أرجع 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[شروعات] = (بنانشان حرف)((ulen >> 8) & 0xFF);
  pkt[شروعات + 1] = (بنانشان حرف)(ulen & 0xFF);
  يادگيري_نقل(&pkt[شروعات + 2], username, ulen);
  أرجع 0;
}

/* add client ID to the CONNECT packet */
ساكن صحيح جوڙيو_client_id(ثابت حرف *client_id, ثابت size_t client_id_len,
                         حرف *pkt, ثابت size_t شروعات)
{
  إذا(client_id_len != MQTT_CLIENTID_LEN)
    أرجع 1;
  pkt[شروعات] = 0x00;
  pkt[شروعات + 1] = MQTT_CLIENTID_LEN;
  يادگيري_نقل(&pkt[شروعات + 2], client_id, MQTT_CLIENTID_LEN);
  أرجع 0;
}

/* Set initial values of CONNECT packet */
ساكن صحيح init_connpack(حرف *packet, حرف *remain, صحيح remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_نياپو_CONNECT;
  /* remaining length field */
  يادگيري_نقل(&packet[1], remain, remain_pos);
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

ساكن CURLcode mqtt_connect(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  صحيح pos = 0;
  صحيح rc = 0;
  /* remain length */
  صحيح remain_pos = 0;
  حرف remain[4] = { 0 };
  size_t packetlen = 0;
  size_t شروعات_user = 0;
  size_t شروعات_pwd = 0;
  حرف client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ثابت size_t clen = CURL_CSTRLEN("curl");
  حرف *packet = عدم;

  /* extracting username from request */
  بنية Curl_creds *creds = معلومات->state.creds;
  ثابت size_t ulen = creds ? لڙي_ڊيگهه(creds->user) : 0;
  ثابت size_t plen = creds ? لڙي_ڊيگهه(creds->passwd) : 0;
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
    أرجع CURLE_WEIRD_SERVER_جواب;
  packet = curlx_calloc(1, packetlen);
  إذا(!packet)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  نتيجو = Curl_rand_alnum(معلومات, (بنانشان حرف *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = جوڙيو_client_id(client_id, لڙي_ڊيگهه(client_id), packet, pos + 1);
  إذا(rc) {
    failf(معلومات, "Client ID length mismatched: [%zu]", لڙي_ڊيگهه(client_id));
    نتيجو = CURLE_WEIRD_SERVER_جواب;
    اذهب پڇاڙي;
  }
  infof(معلومات, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  شروعات_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  شروعات_pwd = شروعات_user + ulen;
  /* if username was provided, add it to the packet */
  إذا(ulen) {
    شروعات_pwd += 2;

    rc = جوڙيو_user(creds->user, ulen,
                  (بنانشان حرف *)packet, شروعات_user, remain_pos);
    إذا(rc) {
      failf(معلومات, "Username too long: [%zu]", ulen);
      نتيجو = CURLE_WEIRD_SERVER_جواب;
      اذهب پڇاڙي;
    }
  }

  /* if passwd was provided, add it to the packet */
  إذا(plen) {
    rc = جوڙيو_passwd(creds->passwd, plen, packet, شروعات_pwd, remain_pos);
    إذا(rc) {
      failf(معلومات, "Password too long: [%zu]", plen);
      نتيجو = CURLE_WEIRD_SERVER_جواب;
      اذهب پڇاڙي;
    }
  }

  إذا(!نتيجو)
    نتيجو = mqtt_send(معلومات, packet, packetlen);

پڇاڙي:
  إذا(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&معلومات->state.creds);
  أرجع نتيجو;
}

ساكن CURLcode mqtt_disconnect(بنية Curl_easy *معلومات)
{
  أرجع mqtt_send(معلومات, "\xe0\x00", 2);
}

ساكن CURLcode mqtt_recv_atleast(بنية Curl_easy *معلومات, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode نتيجو;

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  إذا(rlen < nbytes) {
    بنانشان حرف readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < حجم(readbuf));
    نتيجو = Curl_xfer_recv(معلومات, (حرف *)readbuf, nbytes - rlen, &nread);
    إذا(نتيجو)
      أرجع نتيجو;
    إذا(!nread) /* EOF */
       أرجع CURLE_RECV_ERROR;
    إذا(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  أرجع (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ mqtt_recv_consume(بنية Curl_easy *معلومات, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  إذا(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    إذا(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    وإلا
      curlx_dyn_پڇ(&mq->recvbuf, rlen - nbytes);
  }
}

ساكن CURLcode mqtt_verify_connack(بنية Curl_easy *معلومات)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  CURLcode نتيجو;
  ثابت حرف *ptr;

  DEBUGASSERT(mq);
  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  إذا(mq->بچيل_ڊيگهائي != 2) {
    failf(معلومات, "CONNACK expected Remaining Length 2, got %zu",
          mq->بچيل_ڊيگهائي);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  نتيجو = mqtt_recv_atleast(معلومات, MQTT_CONNACK_LEN);
  إذا(نتيجو)
    أرجع نتيجو;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(معلومات, CURLINFO_سرو_اندريون, ptr, MQTT_CONNACK_LEN);

  إذا(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(معلومات, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (بنانشان حرف)ptr[0], (بنانشان حرف)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }
  mqtt_recv_consume(معلومات, MQTT_CONNACK_LEN);
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_get_topic(بنية Curl_easy *معلومات,
                               حرف **topic, size_t *topiclen)
{
  ثابت حرف *رستو = معلومات->state.مٿي.رستو;
  CURLcode نتيجو = CURLE_URL_MALFORMAT;
  إذا(لڙي_ڊيگهه(رستو) > 1) {
    نتيجو = Curl_urldecode(رستو + 1, 0, topic, topiclen, REJECT_CTRL);
    إذا(!نتيجو && (*topiclen > 0xffff)) {
      failf(معلومات, "Too long MQTT topic");
      نتيجو = CURLE_URL_MALFORMAT;
    }
  }
  وإلا
    failf(معلومات, "No MQTT topic found. Forgot to URL encode it?");

  أرجع نتيجو;
}

ساكن CURLcode mqtt_subscribe(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_OK;
  حرف *topic = عدم;
  size_t topiclen;
  بنانشان حرف *packet = عدم;
  size_t packetlen;
  حرف encodedsize[4];
  size_t n;
  بنية connectdata *conn = معلومات->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  إذا(!mqtt)
    أرجع CURLE_FAILED_INIT;

  نتيجو = mqtt_get_topic(معلومات, &topic, &topiclen);
  إذا(نتيجو)
    اذهب fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((حرف *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  إذا(!packet) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب fail;
  }

  packet[0] = MQTT_نياپو_SUBSCRIBE;
  يادگيري_نقل(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  يادگيري_نقل(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  نتيجو = mqtt_send(معلومات, (ثابت حرف *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  أرجع نتيجو;
}

/*
 * Called when the first byte was already read.
 */
ساكن CURLcode mqtt_verify_suback(بنية Curl_easy *معلومات)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  بنية connectdata *conn = معلومات->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode نتيجو;
  ثابت حرف *ptr;

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  إذا(mq->بچيل_ڊيگهائي != 3) {
    failf(معلومات, "SUBACK expected Remaining Length 3, got %zu",
          mq->بچيل_ڊيگهائي);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  نتيجو = mqtt_recv_atleast(معلومات, MQTT_SUBACK_LEN);
  إذا(نتيجو)
    اذهب fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(معلومات, CURLINFO_سرو_اندريون, ptr, MQTT_SUBACK_LEN);

  إذا(((بنانشان حرف)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((بنانشان حرف)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    نتيجو = CURLE_WEIRD_SERVER_جواب;
    اذهب fail;
  }
  mqtt_recv_consume(معلومات, MQTT_SUBACK_LEN);
fail:
  أرجع نتيجو;
}

#وصف_ڪريو MAX_MQTT_پيغام_سائيز 0xFFFFFFF

ساكن CURLcode mqtt_publish(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  حرف *payload = معلومات->سيٽ.postfields;
  size_t payloadlen;
  حرف *topic = عدم;
  size_t topiclen;
  بنانشان حرف *pkt = عدم;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  حرف encodedbytes[4];
  curl_off_t postfieldsize = معلومات->سيٽ.postfieldsize;

  إذا(!payload) {
    DEBUGF(infof(معلومات, "mqtt_publish without payload, return bad arg"));
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
  إذا(!curlx_sotouz_سمائي(postfieldsize, &payloadlen)) {
    إذا(postfieldsize > 0) /* off_t does not fit into size_t */
      أرجع CURLE_BAD_FUNCTION_دليل;
    payloadlen = لڙي_ڊيگهه(payload);
  }

  نتيجو = mqtt_get_topic(معلومات, &topic, &topiclen);
  إذا(نتيجو)
    اذهب fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  إذا((SIZE_MAX - topiclen - 2) < payloadlen) {
    نتيجو = CURLE_TOO_LARGE;
    اذهب fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  إذا(remaininglength > (MAX_MQTT_پيغام_سائيز - encodelen - 1)) {
    نتيجو = CURLE_TOO_LARGE;
    اذهب fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  إذا(!pkt) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_نياپو_PUBLISH;
  يادگيري_نقل(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  يادگيري_نقل(&pkt[i], topic, topiclen);
  i += topiclen;
  يادگيري_نقل(&pkt[i], payload, payloadlen);
  i += payloadlen;
  نتيجو = mqtt_send(معلومات, (ثابت حرف *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  أرجع نتيجو;
}

/* return FALSE on success, TRUE on error */
ساكن bool mqtt_decode_len(size_t *lenp, ثابت بنانشان حرف *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  بنانشان حرف encoded = 128;

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
#جيڪڏهن_پورو

/* The only way to change state */
ساكن فراغ mqstate(بنية Curl_easy *معلومات,
                    تعداد mqttstate state,
                    تعداد mqttstate nextstate) /* used if state == FIRST */
{
  بنية connectdata *conn = معلومات->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  إذا(!mqtt)
    أرجع;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  infof(معلومات, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_پهريون) ? statenames[nextstate] : "");
#جيڪڏهن_پورو
  mqtt->state = state;
  إذا(state == MQTT_پهريون)
    mqtt->nextstate = nextstate;
}

ساكن CURLcode mqtt_read_publish(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  size_t nread;
  size_t remlen;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  بنانشان حرف packet;

  DEBUGASSERT(mqtt);
  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  بدل(mqtt->state) {
MQTT_SUBACK_COMING:
  حالة MQTT_SUBACK_COMING:
    نتيجو = mqtt_verify_suback(معلومات);
    إذا(نتيجو)
      اكسر;

    mqstate(معلومات, MQTT_پهريون, MQTT_PUBWAIT);
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    إذا(packet == MQTT_نياپو_PUBLISH)
      mqstate(معلومات, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    وإلا إذا(packet == MQTT_نياپو_SUBACK) {
      mqstate(معلومات, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      اذهب MQTT_SUBACK_COMING;
    }
    وإلا إذا(packet == MQTT_نياپو_DISCONNECT) {
      infof(معلومات, "Got DISCONNECT");
      *پورو = TRUE;
      اذهب پڇاڙي;
    }
    وإلا {
      نتيجو = CURLE_WEIRD_SERVER_جواب;
      اذهب پڇاڙي;
    }

    /* -- switched state -- */
    remlen = mq->بچيل_ڊيگهائي;
    infof(معلومات, "Remaining length: %zu bytes", remlen);
    إذا(معلومات->سيٽ.max_filesize &&
       (curl_off_t)remlen > معلومات->سيٽ.max_filesize) {
      failf(معلومات, "Maximum file size exceeded");
      نتيجو = CURLE_FILESIZE_EXCEEDED;
      اذهب پڇاڙي;
    }
    Curl_pgrs_سيٽ_Download_سائيز(معلومات, remlen);
    معلومات->req.bytecount = 0;
    معلومات->req.سائيز = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  حالة MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    حرف بفر[4 * 1024];
    size_t rest = mq->npacket;
    إذا(rest > حجم(بفر))
      rest = حجم(بفر);
    نتيجو = Curl_xfer_recv(معلومات, بفر, rest, &nread);
    إذا(نتيجو) {
      إذا(نتيجو == CURLE_AGAIN) {
        infof(معلومات, "EEEE AAAAGAIN");
      }
      اذهب پڇاڙي;
    }
    إذا(!nread) {
      infof(معلومات, "server disconnected");
      نتيجو = CURLE_PARTIAL_FILE;
      اذهب پڇاڙي;
    }

    /* we received something */
    mq->آخري_وقت = *Curl_pgrs_now(معلومات);

    /* if QoS is set, message contains packet id */
    نتيجو = Curl_client_write(معلومات, CLIENTWRITE_BODY, بفر, nread);
    إذا(نتيجو)
      اذهب پڇاڙي;

    mq->npacket -= nread;
    إذا(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(معلومات, MQTT_پهريون, MQTT_PUBWAIT);
    اكسر;
  }
  افتراضي:
    DEBUGASSERT(عدم); /* illegal state */
    نتيجو = CURLE_WEIRD_SERVER_جواب;
    اذهب پڇاڙي;
  }
پڇاڙي:
  أرجع نتيجو;
}

ساكن CURLcode mqtt_do(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  CURLcode نتيجو = CURLE_OK;
  *پورو = FALSE; /* unconditionally */

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  mq->آخري_وقت = *Curl_pgrs_now(معلومات);
  mq->pingsent = FALSE;

  نتيجو = mqtt_connect(معلومات);
  إذا(نتيجو) {
    failf(معلومات, "Error %d sending MQTT CONNECT request", (صحيح)نتيجو);
    أرجع نتيجو;
  }
  mqstate(معلومات, MQTT_پهريون, MQTT_CONNACK);
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_پورو(بنية Curl_easy *معلومات,
                          CURLcode حالت, bool premature)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  (فراغ)حالت;
  (فراغ)premature;
  إذا(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  أرجع CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ساكن CURLcode mqtt_ping(بنية Curl_easy *معلومات)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  CURLcode نتيجو = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  إذا(mqtt->state == MQTT_پهريون &&
     !mq->pingsent &&
     معلومات->سيٽ.upkeep_interval_ms > 0) {
    بنية curltime t = *Curl_pgrs_now(معلومات);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->آخري_وقت);

    إذا(diff > معلومات->سيٽ.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      بنانشان حرف packet[2] = { 0xC0, 0x00 };
      size_t packetlen = حجم(packet);

      نتيجو = mqtt_send(معلومات, (حرف *)packet, packetlen);
      إذا(!نتيجو) {
        mq->pingsent = TRUE;
      }
      infof(معلومات, "mqtt_ping: sent ping request.");
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode mqtt_doing(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية MQTT *mq = Curl_meta_get(معلومات, CURL_META_MQTT_EASY);
  CURLcode نتيجو = CURLE_OK;
  size_t nread;
  بنانشان حرف recvbyte;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(معلومات->conn, CURL_META_MQTT_CONN);

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  *پورو = FALSE;

  إذا(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    نتيجو = mqtt_flush(معلومات);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    إذا(نتيجو || curlx_dyn_len(&mq->sendbuf))
      أرجع نتيجو;
  }

  نتيجو = mqtt_ping(معلومات);
  /* Packet processing may send more output, so finish PINGREQ first. */
  إذا(نتيجو || curlx_dyn_len(&mq->sendbuf))
    أرجع نتيجو;

  infof(معلومات, "mqtt_doing: state [%d]", (صحيح)mqtt->state);
  بدل(mqtt->state) {
  حالة MQTT_پهريون:
    /* Read the initial byte only */
    نتيجو = Curl_xfer_recv(معلومات, (حرف *)&mq->firstbyte, 1, &nread);
    إذا(نتيجو)
      اكسر;
    وإلا إذا(!nread) {
      failf(معلومات, "Connection disconnected");
      *پورو = TRUE;
      نتيجو = CURLE_RECV_ERROR;
      اكسر;
    }
    Curl_debug(معلومات, CURLINFO_سرو_اندريون, (ثابت حرف *)&mq->firstbyte, 1);

    /* we received something */
    mq->آخري_وقت = *Curl_pgrs_now(معلومات);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(معلومات, MQTT_بچيل_ڊيگهائي, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_بچيل_ڊيگهائي:
    افعل {
      نتيجو = Curl_xfer_recv(معلومات, (حرف *)&recvbyte, 1, &nread);
      إذا(نتيجو || !nread)
        اكسر;
      Curl_debug(معلومات, CURLINFO_سرو_اندريون, (ثابت حرف *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } طالما((recvbyte & 0x80) && (mq->npacket < 4));
    إذا(!نتيجو && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      نتيجو = CURLE_WEIRD_SERVER_جواب;
    إذا(نتيجو)
      اكسر;
    إذا(mqtt_decode_len(&mq->بچيل_ڊيگهائي, mq->pkt_hd, mq->npacket)) {
      نتيجو = CURLE_WEIRD_SERVER_جواب;
      اكسر;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ثابت بنانشان حرف type = mq->firstbyte & 0xF0;
      ثابت بنانشان حرف reserved = mq->firstbyte & 0x0F;
      إذا((type == MQTT_نياپو_DISCONNECT || type == MQTT_نياپو_PINGRESP) &&
         (mq->بچيل_ڊيگهائي || reserved)) {
        failf(معلومات,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_نياپو_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->بچيل_ڊيگهائي, mq->firstbyte);
        نتيجو = CURLE_WEIRD_SERVER_جواب;
        اكسر;
      }
    }
    إذا(mq->بچيل_ڊيگهائي) {
      mqstate(معلومات, mqtt->nextstate, MQTT_NOSTATE);
      اكسر;
    }
    mqstate(معلومات, MQTT_پهريون, MQTT_پهريون);

    إذا(mq->firstbyte == MQTT_نياپو_DISCONNECT) {
      infof(معلومات, "Got DISCONNECT");
      *پورو = TRUE;
    }

    /* ping response */
    إذا(mq->firstbyte == MQTT_نياپو_PINGRESP) {
      infof(معلومات, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(معلومات, MQTT_پهريون, MQTT_PUBWAIT);
    }
    اكسر;
  حالة MQTT_CONNACK:
    نتيجو = mqtt_verify_connack(معلومات);
    إذا(نتيجو)
      اكسر;

    إذا(معلومات->state.httpreq != HTTPREQ_POST) {
      نتيجو = mqtt_subscribe(معلومات);
      إذا(!نتيجو)
        mqstate(معلومات, MQTT_پهريون, MQTT_SUBACK);
      اكسر;
    }

    نتيجو = mqtt_publish(معلومات);
    إذا(نتيجو)
      اكسر;
    mqstate(معلومات, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    إذا(curlx_dyn_len(&mq->sendbuf))
      اكسر;
    نتيجو = mqtt_disconnect(معلومات);
    إذا(نتيجو)
      اكسر;
    mqstate(معلومات, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    إذا(!curlx_dyn_len(&mq->sendbuf))
      *پورو = TRUE;
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
  حالة MQTT_PUB_REMAIN:
    نتيجو = mqtt_read_publish(معلومات, پورو);
    اكسر;

  افتراضي:
    failf(معلومات, "State not handled yet");
    *پورو = TRUE;
    اكسر;
  }

  إذا(نتيجو == CURLE_AGAIN)
    نتيجو = CURLE_OK;
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل USE_SSL

ساكن CURLcode mqtts_connecting(بنية Curl_easy *معلومات, bool *پورو)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode نتيجو;

  نتيجو = Curl_conn_connect(معلومات, FIRSTSOCKET, TRUE, پورو);
  إذا(نتيجو)
    connclose(conn);
  أرجع نتيجو;
}

/*
 * MQTTS protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtts = {
  mqtt_تياري_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_پورو,                          /* done */
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

#جيڪڏهن_پورو

/*
 * MQTT protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtt = {
  mqtt_تياري_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_پورو,                          /* done */
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

#جيڪڏهن_پورو /* CURL_DISABLE_MQTT */
