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
#شامل "curl_setup.h"
#شامل "urldata.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_MQTT

#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "mqtt.h"
#شامل "select.h"
#شامل "url.h"
#شامل "escape.h"
#شامل "rand.h"
#شامل "cfilters.h"
#شامل "connect.h"

/* first byte is command.
   second byte is for flags. */
#تعریف_کرو MQTT_پیغام_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#تعریف_کرو MQTT_پیغام_PUBLISH    0x30
#تعریف_کرو MQTT_پیغام_SUBSCRIBE  0x82
#تعریف_کرو MQTT_پیغام_SUBACK     0x90
#تعریف_کرو MQTT_پیغام_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#تعریف_کرو MQTT_پیغام_PINGRESP   0xD0

#تعریف_کرو MQTT_CONNACK_LEN  2
#تعریف_کرو MQTT_SUBACK_LEN   3
#تعریف_کرو MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

شمار mqttstate {
  MQTT_پہلا,             /* 0 */
  MQTT_بقایا_لمبائی,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

ساخت mqtt_conn {
  شمار mqttstate state;
  شمار mqttstate nextstate; /* switch to this after remaining length is
مکمل*/
  غیرعلامتی صحیح packetid;
};

/* protocol-specific transfer-related data */
ساخت MQTT {
  ساخت dynbuf sendbuf;
  /* when receiving */
  ساخت dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t بقایا_لمبائی;
  غیرعلامتی حرف pkt_hd[4]; /* for decoding the arriving packet length */
  ساخت curltime آخری_وقت; /* last time we sent or received data */
  غیرعلامتی حرف firstbyte;
  ثنائی_ہندسہ(pingsent); /* 1 while we wait for ping response */
};

ساکن خالی mqtt_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت MQTT *mq = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ساکن خالی mqtt_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  (خالی)کلید;
  (خالی)klen;
  curlx_free(اندراج_2);
}

ساکن CURLcode mqtt_تیاری_conn(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ساخت mqtt_conn *mqtt;
  ساخت MQTT *mq;

  mqtt = curlx_calloc(1, حجم(*mqtt));
  اگر(!mqtt ||
     Curl_conn_meta_سیٹ(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  mq = curlx_calloc(1, حجم(ساخت MQTT));
  اگر(!mq)
    واپس CURLE_برآمد_کا_حافظہ;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  اگر(Curl_meta_سیٹ(ڈیٹا, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;
  واپس CURLE_OK;
}

ساکن CURLcode mqtt_send(ساخت Curl_easy *ڈیٹا,
                          ثابت حرف *buf, size_t len)
{
  size_t n;
  CURLcode نتیجہ;
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);

  اگر(!mq)
    واپس CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  نتیجہ = Curl_xfer_send(ڈیٹا, buf, len, FALSE, &n);
  اگر(نتیجہ)
    واپس نتیجہ;
  mq->آخری_وقت = *Curl_pgrs_now(ڈیٹا);
  Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, buf, n);
  اگر(len != n)
    نتیجہ = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  واپس نتیجہ;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ساکن CURLcode mqtt_flush(ساخت Curl_easy *ڈیٹا)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode نتیجہ;

  اگر(!mq)
    واپس CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  اگر(!len)
    واپس CURLE_OK;

  نتیجہ = Curl_xfer_send(ڈیٹا, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  اگر(نتیجہ)
    واپس نتیجہ;
  mq->آخری_وقت = *Curl_pgrs_now(ڈیٹا);
  Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, curlx_dyn_ptr(&mq->sendbuf), n);
  واپس curlx_dyn_دم(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
حالتیں*/
ساکن CURLcode mqtt_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  اگر(mq && curlx_dyn_len(&mq->sendbuf))
    واپس Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]);
  واپس Curl_pollset_جوڑو_اندر(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]);
}

ساکن صحیح mqtt_encode_len(حرف *buf, size_t len)
{
  صحیح i;

  برائے(i = 0; (len > 0) && (i < 4); i++) {
    غیرعلامتی حرف encoded;
    encoded = len % 0x80;
    len /= 0x80;
    اگر(len)
      encoded |= 0x80;
    buf[i] = (حرف)encoded;
  }

  واپس i;
}

/* add the passwd to the CONNECT packet */
ساکن صحیح جوڑو_passwd(ثابت حرف *passwd, ثابت size_t plen,
                      حرف *pkt, ثابت size_t آغاز, صحیح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  اگر(plen > 0xffff)
    واپس 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[آغاز] = (حرف)((plen >> 8) & 0xFF);
  pkt[آغاز + 1] = (حرف)(plen & 0xFF);
  حافظہ_نقل(&pkt[آغاز + 2], passwd, plen);
  واپس 0;
}

/* add user to the CONNECT packet */
ساکن صحیح جوڑو_user(ثابت حرف *username, ثابت size_t ulen,
                    غیرعلامتی حرف *pkt, ثابت size_t آغاز, صحیح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  اگر(ulen > 0xffff)
    واپس 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[آغاز] = (غیرعلامتی حرف)((ulen >> 8) & 0xFF);
  pkt[آغاز + 1] = (غیرعلامتی حرف)(ulen & 0xFF);
  حافظہ_نقل(&pkt[آغاز + 2], username, ulen);
  واپس 0;
}

/* add client ID to the CONNECT packet */
ساکن صحیح جوڑو_client_id(ثابت حرف *client_id, ثابت size_t client_id_len,
                         حرف *pkt, ثابت size_t آغاز)
{
  اگر(client_id_len != MQTT_CLIENTID_LEN)
    واپس 1;
  pkt[آغاز] = 0x00;
  pkt[آغاز + 1] = MQTT_CLIENTID_LEN;
  حافظہ_نقل(&pkt[آغاز + 2], client_id, MQTT_CLIENTID_LEN);
  واپس 0;
}

/* Set initial values of CONNECT packet */
ساکن صحیح init_connpack(حرف *packet, حرف *remain, صحیح remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_پیغام_CONNECT;
  /* remaining length field */
  حافظہ_نقل(&packet[1], remain, remain_pos);
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
  واپس remain_pos + 10;
}

ساکن CURLcode mqtt_connect(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح pos = 0;
  صحیح rc = 0;
  /* remain length */
  صحیح remain_pos = 0;
  حرف remain[4] = { 0 };
  size_t packetlen = 0;
  size_t آغاز_user = 0;
  size_t آغاز_pwd = 0;
  حرف client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ثابت size_t clen = CURL_CSTRLEN("curl");
  حرف *packet = تہی;

  /* extracting username from request */
  ساخت Curl_creds *creds = ڈیٹا->state.creds;
  ثابت size_t ulen = creds ? لڑی_لمبائی(creds->user) : 0;
  ثابت size_t plen = creds ? لڑی_لمبائی(creds->passwd) : 0;
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
  اگر(packetlen > 0xFFFFFFF)
    واپس CURLE_WEIRD_SERVER_جواب;
  packet = curlx_calloc(1, packetlen);
  اگر(!packet)
    واپس CURLE_برآمد_کا_حافظہ;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  نتیجہ = Curl_rand_alnum(ڈیٹا, (غیرعلامتی حرف *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = جوڑو_client_id(client_id, لڑی_لمبائی(client_id), packet, pos + 1);
  اگر(rc) {
    failf(ڈیٹا, "Client ID length mismatched: [%zu]", لڑی_لمبائی(client_id));
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
    جاؤ انتہا;
  }
  infof(ڈیٹا, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  آغاز_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  آغاز_pwd = آغاز_user + ulen;
  /* if username was provided, add it to the packet */
  اگر(ulen) {
    آغاز_pwd += 2;

    rc = جوڑو_user(creds->user, ulen,
                  (غیرعلامتی حرف *)packet, آغاز_user, remain_pos);
    اگر(rc) {
      failf(ڈیٹا, "Username too long: [%zu]", ulen);
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      جاؤ انتہا;
    }
  }

  /* if passwd was provided, add it to the packet */
  اگر(plen) {
    rc = جوڑو_passwd(creds->passwd, plen, packet, آغاز_pwd, remain_pos);
    اگر(rc) {
      failf(ڈیٹا, "Password too long: [%zu]", plen);
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      جاؤ انتہا;
    }
  }

  اگر(!نتیجہ)
    نتیجہ = mqtt_send(ڈیٹا, packet, packetlen);

انتہا:
  اگر(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&ڈیٹا->state.creds);
  واپس نتیجہ;
}

ساکن CURLcode mqtt_disconnect(ساخت Curl_easy *ڈیٹا)
{
  واپس mqtt_send(ڈیٹا, "\xe0\x00", 2);
}

ساکن CURLcode mqtt_recv_atleast(ساخت Curl_easy *ڈیٹا, size_t nbytes)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode نتیجہ;

  اگر(!mq)
    واپس CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  اگر(rlen < nbytes) {
    غیرعلامتی حرف readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < حجم(readbuf));
    نتیجہ = Curl_xfer_recv(ڈیٹا, (حرف *)readbuf, nbytes - rlen, &nread);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(!nread) /* EOF */
       واپس CURLE_RECV_ERROR;
    اگر(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      واپس CURLE_برآمد_کا_حافظہ;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  واپس (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ساکن خالی mqtt_recv_consume(ساخت Curl_easy *ڈیٹا, size_t nbytes)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  اگر(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    اگر(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    ورنہ
      curlx_dyn_دم(&mq->recvbuf, rlen - nbytes);
  }
}

ساکن CURLcode mqtt_verify_connack(ساخت Curl_easy *ڈیٹا)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  CURLcode نتیجہ;
  ثابت حرف *ptr;

  DEBUGASSERT(mq);
  اگر(!mq)
    واپس CURLE_FAILED_INIT;
  اگر(mq->بقایا_لمبائی != 2) {
    failf(ڈیٹا, "CONNACK expected Remaining Length 2, got %zu",
          mq->بقایا_لمبائی);
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  نتیجہ = mqtt_recv_atleast(ڈیٹا, MQTT_CONNACK_LEN);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, ptr, MQTT_CONNACK_LEN);

  اگر(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(ڈیٹا, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (غیرعلامتی حرف)ptr[0], (غیرعلامتی حرف)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    واپس CURLE_WEIRD_SERVER_جواب;
  }
  mqtt_recv_consume(ڈیٹا, MQTT_CONNACK_LEN);
  واپس CURLE_OK;
}

ساکن CURLcode mqtt_get_topic(ساخت Curl_easy *ڈیٹا,
                               حرف **topic, size_t *topiclen)
{
  ثابت حرف *راستہ = ڈیٹا->state.اوپر.راستہ;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT;
  اگر(لڑی_لمبائی(راستہ) > 1) {
    نتیجہ = Curl_urldecode(راستہ + 1, 0, topic, topiclen, REJECT_CTRL);
    اگر(!نتیجہ && (*topiclen > 0xffff)) {
      failf(ڈیٹا, "Too long MQTT topic");
      نتیجہ = CURLE_URL_MALFORMAT;
    }
  }
  ورنہ
    failf(ڈیٹا, "No MQTT topic found. Forgot to URL encode it?");

  واپس نتیجہ;
}

ساکن CURLcode mqtt_subscribe(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *topic = تہی;
  size_t topiclen;
  غیرعلامتی حرف *packet = تہی;
  size_t packetlen;
  حرف encodedsize[4];
  size_t n;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  اگر(!mqtt)
    واپس CURLE_FAILED_INIT;

  نتیجہ = mqtt_get_topic(ڈیٹا, &topic, &topiclen);
  اگر(نتیجہ)
    جاؤ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((حرف *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  اگر(!packet) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  packet[0] = MQTT_پیغام_SUBSCRIBE;
  حافظہ_نقل(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  حافظہ_نقل(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  نتیجہ = mqtt_send(ڈیٹا, (ثابت حرف *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  واپس نتیجہ;
}

/*
 * Called when the first byte was already read.
 */
ساکن CURLcode mqtt_verify_suback(ساخت Curl_easy *ڈیٹا)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode نتیجہ;
  ثابت حرف *ptr;

  اگر(!mqtt || !mq)
    واپس CURLE_FAILED_INIT;

  اگر(mq->بقایا_لمبائی != 3) {
    failf(ڈیٹا, "SUBACK expected Remaining Length 3, got %zu",
          mq->بقایا_لمبائی);
    واپس CURLE_WEIRD_SERVER_جواب;
  }

  نتیجہ = mqtt_recv_atleast(ڈیٹا, MQTT_SUBACK_LEN);
  اگر(نتیجہ)
    جاؤ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, ptr, MQTT_SUBACK_LEN);

  اگر(((غیرعلامتی حرف)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((غیرعلامتی حرف)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
    جاؤ fail;
  }
  mqtt_recv_consume(ڈیٹا, MQTT_SUBACK_LEN);
fail:
  واپس نتیجہ;
}

#تعریف_کرو MAX_MQTT_پیغام_سائز 0xFFFFFFF

ساکن CURLcode mqtt_publish(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  حرف *payload = ڈیٹا->سیٹ.postfields;
  size_t payloadlen;
  حرف *topic = تہی;
  size_t topiclen;
  غیرعلامتی حرف *pkt = تہی;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  حرف encodedbytes[4];
  curl_off_t postfieldsize = ڈیٹا->سیٹ.postfieldsize;

  اگر(!payload) {
    DEBUGF(infof(ڈیٹا, "mqtt_publish without payload, return bad arg"));
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  اگر(!curlx_sotouz_سماتا(postfieldsize, &payloadlen)) {
    اگر(postfieldsize > 0) /* off_t does not fit into size_t */
      واپس CURLE_BAD_FUNCTION_دلیل;
    payloadlen = لڑی_لمبائی(payload);
  }

  نتیجہ = mqtt_get_topic(ڈیٹا, &topic, &topiclen);
  اگر(نتیجہ)
    جاؤ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  اگر((SIZE_MAX - topiclen - 2) < payloadlen) {
    نتیجہ = CURLE_TOO_LARGE;
    جاؤ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  اگر(remaininglength > (MAX_MQTT_پیغام_سائز - encodelen - 1)) {
    نتیجہ = CURLE_TOO_LARGE;
    جاؤ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  اگر(!pkt) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_پیغام_PUBLISH;
  حافظہ_نقل(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  حافظہ_نقل(&pkt[i], topic, topiclen);
  i += topiclen;
  حافظہ_نقل(&pkt[i], payload, payloadlen);
  i += payloadlen;
  نتیجہ = mqtt_send(ڈیٹا, (ثابت حرف *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  واپس نتیجہ;
}

/* return FALSE on success, TRUE on error */
ساکن bool mqtt_decode_len(size_t *lenp, ثابت غیرعلامتی حرف *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  غیرعلامتی حرف encoded = 128;

  برائے(i = 0; (i < buflen) && (encoded & 128); i++) {
    اگر(i == 4)
      واپس TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  اگر(encoded & 128)
    /* truncated size */
    واپس TRUE;

  *lenp = len;
  واپس FALSE;
}

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساکن ثابت حرف * ثابت statenames[] = {
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
#اگر_ختم

/* The only way to change state */
ساکن خالی mqstate(ساخت Curl_easy *ڈیٹا,
                    شمار mqttstate state,
                    شمار mqttstate nextstate) /* used if state == FIRST */
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  اگر(!mqtt)
    واپس;
#اگر_تعریف_شدہ DEBUGBUILD
  infof(ڈیٹا, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_پہلا) ? statenames[nextstate] : "");
#اگر_ختم
  mqtt->state = state;
  اگر(state == MQTT_پہلا)
    mqtt->nextstate = nextstate;
}

ساکن CURLcode mqtt_read_publish(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  size_t nread;
  size_t remlen;
  ساخت mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  غیرعلامتی حرف packet;

  DEBUGASSERT(mqtt);
  اگر(!mqtt || !mq)
    واپس CURLE_FAILED_INIT;

  انتخاب(mqtt->state) {
MQTT_SUBACK_COMING:
  حالت MQTT_SUBACK_COMING:
    نتیجہ = mqtt_verify_suback(ڈیٹا);
    اگر(نتیجہ)
      توڑو;

    mqstate(ڈیٹا, MQTT_پہلا, MQTT_PUBWAIT);
    توڑو;

  حالت MQTT_SUBACK:
  حالت MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    اگر(packet == MQTT_پیغام_PUBLISH)
      mqstate(ڈیٹا, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    ورنہ اگر(packet == MQTT_پیغام_SUBACK) {
      mqstate(ڈیٹا, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      جاؤ MQTT_SUBACK_COMING;
    }
    ورنہ اگر(packet == MQTT_پیغام_DISCONNECT) {
      infof(ڈیٹا, "Got DISCONNECT");
      *مکمل = TRUE;
      جاؤ انتہا;
    }
    ورنہ {
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      جاؤ انتہا;
    }

    /* -- switched state -- */
    remlen = mq->بقایا_لمبائی;
    infof(ڈیٹا, "Remaining length: %zu bytes", remlen);
    اگر(ڈیٹا->سیٹ.max_filesize &&
       (curl_off_t)remlen > ڈیٹا->سیٹ.max_filesize) {
      failf(ڈیٹا, "Maximum file size exceeded");
      نتیجہ = CURLE_FILESIZE_EXCEEDED;
      جاؤ انتہا;
    }
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, remlen);
    ڈیٹا->req.bytecount = 0;
    ڈیٹا->req.سائز = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  حالت MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    حرف بفر[4 * 1024];
    size_t rest = mq->npacket;
    اگر(rest > حجم(بفر))
      rest = حجم(بفر);
    نتیجہ = Curl_xfer_recv(ڈیٹا, بفر, rest, &nread);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_AGAIN) {
        infof(ڈیٹا, "EEEE AAAAGAIN");
      }
      جاؤ انتہا;
    }
    اگر(!nread) {
      infof(ڈیٹا, "server disconnected");
      نتیجہ = CURLE_PARTIAL_FILE;
      جاؤ انتہا;
    }

    /* we received something */
    mq->آخری_وقت = *Curl_pgrs_now(ڈیٹا);

    /* if QoS is set, message contains packet id */
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, بفر, nread);
    اگر(نتیجہ)
      جاؤ انتہا;

    mq->npacket -= nread;
    اگر(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(ڈیٹا, MQTT_پہلا, MQTT_PUBWAIT);
    توڑو;
  }
  طےشدہ:
    DEBUGASSERT(تہی); /* illegal state */
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
    جاؤ انتہا;
  }
انتہا:
  واپس نتیجہ;
}

ساکن CURLcode mqtt_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  CURLcode نتیجہ = CURLE_OK;
  *مکمل = FALSE; /* unconditionally */

  اگر(!mq)
    واپس CURLE_FAILED_INIT;
  mq->آخری_وقت = *Curl_pgrs_now(ڈیٹا);
  mq->pingsent = FALSE;

  نتیجہ = mqtt_connect(ڈیٹا);
  اگر(نتیجہ) {
    failf(ڈیٹا, "Error %d sending MQTT CONNECT request", (صحیح)نتیجہ);
    واپس نتیجہ;
  }
  mqstate(ڈیٹا, MQTT_پہلا, MQTT_CONNACK);
  واپس CURLE_OK;
}

ساکن CURLcode mqtt_مکمل(ساخت Curl_easy *ڈیٹا,
                          CURLcode حالت_2, bool premature)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  (خالی)حالت_2;
  (خالی)premature;
  اگر(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  واپس CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ساکن CURLcode mqtt_ping(ساخت Curl_easy *ڈیٹا)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  اگر(!mqtt || !mq)
    واپس CURLE_FAILED_INIT;

  اگر(mqtt->state == MQTT_پہلا &&
     !mq->pingsent &&
     ڈیٹا->سیٹ.upkeep_interval_ms > 0) {
    ساخت curltime t = *Curl_pgrs_now(ڈیٹا);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->آخری_وقت);

    اگر(diff > ڈیٹا->سیٹ.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      غیرعلامتی حرف packet[2] = { 0xC0, 0x00 };
      size_t packetlen = حجم(packet);

      نتیجہ = mqtt_send(ڈیٹا, (حرف *)packet, packetlen);
      اگر(!نتیجہ) {
        mq->pingsent = TRUE;
      }
      infof(ڈیٹا, "mqtt_ping: sent ping request.");
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode mqtt_doing(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت MQTT *mq = Curl_meta_get(ڈیٹا, CURL_META_MQTT_EASY);
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;
  غیرعلامتی حرف recvbyte;
  ساخت mqtt_conn *mqtt = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_MQTT_CONN);

  اگر(!mqtt || !mq)
    واپس CURLE_FAILED_INIT;

  *مکمل = FALSE;

  اگر(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    نتیجہ = mqtt_flush(ڈیٹا);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    اگر(نتیجہ || curlx_dyn_len(&mq->sendbuf))
      واپس نتیجہ;
  }

  نتیجہ = mqtt_ping(ڈیٹا);
  /* Packet processing may send more output, so finish PINGREQ first. */
  اگر(نتیجہ || curlx_dyn_len(&mq->sendbuf))
    واپس نتیجہ;

  infof(ڈیٹا, "mqtt_doing: state [%d]", (صحیح)mqtt->state);
  انتخاب(mqtt->state) {
  حالت MQTT_پہلا:
    /* Read the initial byte only */
    نتیجہ = Curl_xfer_recv(ڈیٹا, (حرف *)&mq->firstbyte, 1, &nread);
    اگر(نتیجہ)
      توڑو;
    ورنہ اگر(!nread) {
      failf(ڈیٹا, "Connection disconnected");
      *مکمل = TRUE;
      نتیجہ = CURLE_RECV_ERROR;
      توڑو;
    }
    Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, (ثابت حرف *)&mq->firstbyte, 1);

    /* we received something */
    mq->آخری_وقت = *Curl_pgrs_now(ڈیٹا);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(ڈیٹا, MQTT_بقایا_لمبائی, MQTT_NOSTATE);
    FALLTHROUGH();
  حالت MQTT_بقایا_لمبائی:
    کرو {
      نتیجہ = Curl_xfer_recv(ڈیٹا, (حرف *)&recvbyte, 1, &nread);
      اگر(نتیجہ || !nread)
        توڑو;
      Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, (ثابت حرف *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } جب((recvbyte & 0x80) && (mq->npacket < 4));
    اگر(!نتیجہ && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
    اگر(نتیجہ)
      توڑو;
    اگر(mqtt_decode_len(&mq->بقایا_لمبائی, mq->pkt_hd, mq->npacket)) {
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      توڑو;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ثابت غیرعلامتی حرف type = mq->firstbyte & 0xF0;
      ثابت غیرعلامتی حرف reserved = mq->firstbyte & 0x0F;
      اگر((type == MQTT_پیغام_DISCONNECT || type == MQTT_پیغام_PINGRESP) &&
         (mq->بقایا_لمبائی || reserved)) {
        failf(ڈیٹا,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_پیغام_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->بقایا_لمبائی, mq->firstbyte);
        نتیجہ = CURLE_WEIRD_SERVER_جواب;
        توڑو;
      }
    }
    اگر(mq->بقایا_لمبائی) {
      mqstate(ڈیٹا, mqtt->nextstate, MQTT_NOSTATE);
      توڑو;
    }
    mqstate(ڈیٹا, MQTT_پہلا, MQTT_پہلا);

    اگر(mq->firstbyte == MQTT_پیغام_DISCONNECT) {
      infof(ڈیٹا, "Got DISCONNECT");
      *مکمل = TRUE;
    }

    /* ping response */
    اگر(mq->firstbyte == MQTT_پیغام_PINGRESP) {
      infof(ڈیٹا, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(ڈیٹا, MQTT_پہلا, MQTT_PUBWAIT);
    }
    توڑو;
  حالت MQTT_CONNACK:
    نتیجہ = mqtt_verify_connack(ڈیٹا);
    اگر(نتیجہ)
      توڑو;

    اگر(ڈیٹا->state.httpreq != HTTPREQ_POST) {
      نتیجہ = mqtt_subscribe(ڈیٹا);
      اگر(!نتیجہ)
        mqstate(ڈیٹا, MQTT_پہلا, MQTT_SUBACK);
      توڑو;
    }

    نتیجہ = mqtt_publish(ڈیٹا);
    اگر(نتیجہ)
      توڑو;
    mqstate(ڈیٹا, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالت MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    اگر(curlx_dyn_len(&mq->sendbuf))
      توڑو;
    نتیجہ = mqtt_disconnect(ڈیٹا);
    اگر(نتیجہ)
      توڑو;
    mqstate(ڈیٹا, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالت MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    اگر(!curlx_dyn_len(&mq->sendbuf))
      *مکمل = TRUE;
    توڑو;

  حالت MQTT_SUBACK:
  حالت MQTT_PUBWAIT:
  حالت MQTT_PUB_REMAIN:
    نتیجہ = mqtt_read_publish(ڈیٹا, مکمل);
    توڑو;

  طےشدہ:
    failf(ڈیٹا, "State not handled yet");
    *مکمل = TRUE;
    توڑو;
  }

  اگر(نتیجہ == CURLE_AGAIN)
    نتیجہ = CURLE_OK;
  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_SSL

ساکن CURLcode mqtts_connecting(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ;

  نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, TRUE, مکمل);
  اگر(نتیجہ)
    connclose(conn);
  واپس نتیجہ;
}

/*
 * MQTTS protocol.
 */
ثابت ساخت Curl_protocol Curl_protocol_mqtts = {
  mqtt_تیاری_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_مکمل,                          /* done */
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

#اگر_ختم

/*
 * MQTT protocol.
 */
ثابت ساخت Curl_protocol Curl_protocol_mqtt = {
  mqtt_تیاری_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_مکمل,                          /* done */
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

#اگر_ختم /* CURL_DISABLE_MQTT */
