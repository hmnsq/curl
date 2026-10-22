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

#جے_بنا_تعریف CURL_DISABLE_MQTT

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

تعداد mqttstate {
  MQTT_پہلا,             /* 0 */
  MQTT_بچیا_لمائی,  /* 1 */
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
پورا*/
  بنانشانی صحيح packetid;
};

/* protocol-specific transfer-related data */
بنية MQTT {
  بنية dynbuf sendbuf;
  /* when receiving */
  بنية dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t بچیا_لمائی;
  بنانشانی حرف pkt_hd[4]; /* for decoding the arriving packet length */
  بنية curltime آخری_ویلا; /* last time we sent or received data */
  بنانشانی حرف firstbyte;
  ثنائی_ہندسہ(pingsent); /* 1 while we wait for ping response */
};

ساكن فراغ mqtt_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية MQTT *mq = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ساكن فراغ mqtt_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  (فراغ)کلید;
  (فراغ)klen;
  curlx_free(اندراج);
}

ساكن CURLcode mqtt_تیاری_conn(بنية Curl_easy *انکڑے,
                                بنية connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  بنية mqtt_conn *mqtt;
  بنية MQTT *mq;

  mqtt = curlx_calloc(1, حجم(*mqtt));
  إذا(!mqtt ||
     Curl_conn_meta_سیٹ(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  mq = curlx_calloc(1, حجم(بنية MQTT));
  إذا(!mq)
    أرجع CURLE_باہرلا_دا_حافظہ;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  إذا(Curl_meta_سیٹ(انکڑے, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_send(بنية Curl_easy *انکڑے,
                          ثابت حرف *buf, size_t len)
{
  size_t n;
  CURLcode نتیجہ;
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  نتیجہ = Curl_xfer_send(انکڑے, buf, len, FALSE, &n);
  إذا(نتیجہ)
    أرجع نتیجہ;
  mq->آخری_ویلا = *Curl_pgrs_now(انکڑے);
  Curl_debug(انکڑے, CURLINFO_سرخی_باہرلا, buf, n);
  إذا(len != n)
    نتیجہ = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  أرجع نتیجہ;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ساكن CURLcode mqtt_flush(بنية Curl_easy *انکڑے)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode نتیجہ;

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  إذا(!len)
    أرجع CURLE_OK;

  نتیجہ = Curl_xfer_send(انکڑے, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  إذا(نتیجہ)
    أرجع نتیجہ;
  mq->آخری_ویلا = *Curl_pgrs_now(انکڑے);
  Curl_debug(انکڑے, CURLINFO_سرخی_باہرلا, curlx_dyn_ptr(&mq->sendbuf), n);
  أرجع curlx_dyn_پوچھ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
حالتاں*/
ساكن CURLcode mqtt_pollset(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  إذا(mq && curlx_dyn_len(&mq->sendbuf))
    أرجع Curl_pollset_جوڑو_باہرلا(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]);
  أرجع Curl_pollset_جوڑو_وچ(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]);
}

ساكن صحيح mqtt_encode_len(حرف *buf, size_t len)
{
  صحيح i;

  لكل(i = 0; (len > 0) && (i < 4); i++) {
    بنانشانی حرف encoded;
    encoded = len % 0x80;
    len /= 0x80;
    إذا(len)
      encoded |= 0x80;
    buf[i] = (حرف)encoded;
  }

  أرجع i;
}

/* add the passwd to the CONNECT packet */
ساكن صحيح جوڑو_passwd(ثابت حرف *passwd, ثابت size_t plen,
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
  حافظہ_نقل(&pkt[شروع + 2], passwd, plen);
  أرجع 0;
}

/* add user to the CONNECT packet */
ساكن صحيح جوڑو_user(ثابت حرف *username, ثابت size_t ulen,
                    بنانشانی حرف *pkt, ثابت size_t شروع, صحيح remain_pos)
{
  /* magic number that need to be set properly */
  ثابت size_t conn_flags_pos = remain_pos + 8;
  إذا(ulen > 0xffff)
    أرجع 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[شروع] = (بنانشانی حرف)((ulen >> 8) & 0xFF);
  pkt[شروع + 1] = (بنانشانی حرف)(ulen & 0xFF);
  حافظہ_نقل(&pkt[شروع + 2], username, ulen);
  أرجع 0;
}

/* add client ID to the CONNECT packet */
ساكن صحيح جوڑو_client_id(ثابت حرف *client_id, ثابت size_t client_id_len,
                         حرف *pkt, ثابت size_t شروع)
{
  إذا(client_id_len != MQTT_CLIENTID_LEN)
    أرجع 1;
  pkt[شروع] = 0x00;
  pkt[شروع + 1] = MQTT_CLIENTID_LEN;
  حافظہ_نقل(&pkt[شروع + 2], client_id, MQTT_CLIENTID_LEN);
  أرجع 0;
}

/* Set initial values of CONNECT packet */
ساكن صحيح init_connpack(حرف *packet, حرف *remain, صحيح remain_pos)
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
  أرجع remain_pos + 10;
}

ساكن CURLcode mqtt_connect(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_OK;
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
  بنية Curl_creds *creds = انکڑے->state.creds;
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
  إذا(packetlen > 0xFFFFFFF)
    أرجع CURLE_WEIRD_SERVER_جواب;
  packet = curlx_calloc(1, packetlen);
  إذا(!packet)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  نتیجہ = Curl_rand_alnum(انکڑے, (بنانشانی حرف *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = جوڑو_client_id(client_id, لڑی_لمبائی(client_id), packet, pos + 1);
  إذا(rc) {
    failf(انکڑے, "Client ID length mismatched: [%zu]", لڑی_لمبائی(client_id));
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
    اذهب سرا;
  }
  infof(انکڑے, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  شروع_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  شروع_pwd = شروع_user + ulen;
  /* if username was provided, add it to the packet */
  إذا(ulen) {
    شروع_pwd += 2;

    rc = جوڑو_user(creds->user, ulen,
                  (بنانشانی حرف *)packet, شروع_user, remain_pos);
    إذا(rc) {
      failf(انکڑے, "Username too long: [%zu]", ulen);
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      اذهب سرا;
    }
  }

  /* if passwd was provided, add it to the packet */
  إذا(plen) {
    rc = جوڑو_passwd(creds->passwd, plen, packet, شروع_pwd, remain_pos);
    إذا(rc) {
      failf(انکڑے, "Password too long: [%zu]", plen);
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      اذهب سرا;
    }
  }

  إذا(!نتیجہ)
    نتیجہ = mqtt_send(انکڑے, packet, packetlen);

سرا:
  إذا(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&انکڑے->state.creds);
  أرجع نتیجہ;
}

ساكن CURLcode mqtt_disconnect(بنية Curl_easy *انکڑے)
{
  أرجع mqtt_send(انکڑے, "\xe0\x00", 2);
}

ساكن CURLcode mqtt_recv_atleast(بنية Curl_easy *انکڑے, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode نتیجہ;

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  إذا(rlen < nbytes) {
    بنانشانی حرف readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < حجم(readbuf));
    نتیجہ = Curl_xfer_recv(انکڑے, (حرف *)readbuf, nbytes - rlen, &nread);
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(!nread) /* EOF */
       أرجع CURLE_RECV_ERROR;
    إذا(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      أرجع CURLE_باہرلا_دا_حافظہ;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  أرجع (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ mqtt_recv_consume(بنية Curl_easy *انکڑے, size_t nbytes)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  إذا(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    إذا(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    وإلا
      curlx_dyn_پوچھ(&mq->recvbuf, rlen - nbytes);
  }
}

ساكن CURLcode mqtt_verify_connack(بنية Curl_easy *انکڑے)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  CURLcode نتیجہ;
  ثابت حرف *ptr;

  DEBUGASSERT(mq);
  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  إذا(mq->بچیا_لمائی != 2) {
    failf(انکڑے, "CONNACK expected Remaining Length 2, got %zu",
          mq->بچیا_لمائی);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  نتیجہ = mqtt_recv_atleast(انکڑے, MQTT_CONNACK_LEN);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(انکڑے, CURLINFO_سرخی_وچ, ptr, MQTT_CONNACK_LEN);

  إذا(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(انکڑے, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (بنانشانی حرف)ptr[0], (بنانشانی حرف)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }
  mqtt_recv_consume(انکڑے, MQTT_CONNACK_LEN);
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_get_topic(بنية Curl_easy *انکڑے,
                               حرف **topic, size_t *topiclen)
{
  ثابت حرف *رستہ = انکڑے->state.اتے.رستہ;
  CURLcode نتیجہ = CURLE_URL_MALFORMAT;
  إذا(لڑی_لمبائی(رستہ) > 1) {
    نتیجہ = Curl_urldecode(رستہ + 1, 0, topic, topiclen, REJECT_CTRL);
    إذا(!نتیجہ && (*topiclen > 0xffff)) {
      failf(انکڑے, "Too long MQTT topic");
      نتیجہ = CURLE_URL_MALFORMAT;
    }
  }
  وإلا
    failf(انکڑے, "No MQTT topic found. Forgot to URL encode it?");

  أرجع نتیجہ;
}

ساكن CURLcode mqtt_subscribe(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *topic = عدم;
  size_t topiclen;
  بنانشانی حرف *packet = عدم;
  size_t packetlen;
  حرف encodedsize[4];
  size_t n;
  بنية connectdata *conn = انکڑے->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  إذا(!mqtt)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = mqtt_get_topic(انکڑے, &topic, &topiclen);
  إذا(نتیجہ)
    اذهب fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((حرف *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  إذا(!packet) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
  }

  packet[0] = MQTT_پیغام_SUBSCRIBE;
  حافظہ_نقل(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  حافظہ_نقل(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  نتیجہ = mqtt_send(انکڑے, (ثابت حرف *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  أرجع نتیجہ;
}

/*
 * Called when the first byte was already read.
 */
ساكن CURLcode mqtt_verify_suback(بنية Curl_easy *انکڑے)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  بنية connectdata *conn = انکڑے->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode نتیجہ;
  ثابت حرف *ptr;

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  إذا(mq->بچیا_لمائی != 3) {
    failf(انکڑے, "SUBACK expected Remaining Length 3, got %zu",
          mq->بچیا_لمائی);
    أرجع CURLE_WEIRD_SERVER_جواب;
  }

  نتیجہ = mqtt_recv_atleast(انکڑے, MQTT_SUBACK_LEN);
  إذا(نتیجہ)
    اذهب fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(انکڑے, CURLINFO_سرخی_وچ, ptr, MQTT_SUBACK_LEN);

  إذا(((بنانشانی حرف)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((بنانشانی حرف)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
    اذهب fail;
  }
  mqtt_recv_consume(انکڑے, MQTT_SUBACK_LEN);
fail:
  أرجع نتیجہ;
}

#تعریف_کرو MAX_MQTT_سنیہا_ناپ 0xFFFFFFF

ساكن CURLcode mqtt_publish(بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;
  حرف *payload = انکڑے->سیٹ.postfields;
  size_t payloadlen;
  حرف *topic = عدم;
  size_t topiclen;
  بنانشانی حرف *pkt = عدم;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  حرف encodedbytes[4];
  curl_off_t postfieldsize = انکڑے->سیٹ.postfieldsize;

  إذا(!payload) {
    DEBUGF(infof(انکڑے, "mqtt_publish without payload, return bad arg"));
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  إذا(!curlx_sotouz_سماندا(postfieldsize, &payloadlen)) {
    إذا(postfieldsize > 0) /* off_t does not fit into size_t */
      أرجع CURLE_BAD_FUNCTION_دلیل;
    payloadlen = لڑی_لمبائی(payload);
  }

  نتیجہ = mqtt_get_topic(انکڑے, &topic, &topiclen);
  إذا(نتیجہ)
    اذهب fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  إذا((SIZE_MAX - topiclen - 2) < payloadlen) {
    نتیجہ = CURLE_TOO_LARGE;
    اذهب fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  إذا(remaininglength > (MAX_MQTT_سنیہا_ناپ - encodelen - 1)) {
    نتیجہ = CURLE_TOO_LARGE;
    اذهب fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  إذا(!pkt) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب fail;
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
  نتیجہ = mqtt_send(انکڑے, (ثابت حرف *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  أرجع نتیجہ;
}

/* return FALSE on success, TRUE on error */
ساكن bool mqtt_decode_len(size_t *lenp, ثابت بنانشانی حرف *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  بنانشانی حرف encoded = 128;

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
#جے_انت

/* The only way to change state */
ساكن فراغ mqstate(بنية Curl_easy *انکڑے,
                    تعداد mqttstate state,
                    تعداد mqttstate nextstate) /* used if state == FIRST */
{
  بنية connectdata *conn = انکڑے->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  إذا(!mqtt)
    أرجع;
#جے_تعریف_شدہ DEBUGBUILD
  infof(انکڑے, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_پہلا) ? statenames[nextstate] : "");
#جے_انت
  mqtt->state = state;
  إذا(state == MQTT_پہلا)
    mqtt->nextstate = nextstate;
}

ساكن CURLcode mqtt_read_publish(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  size_t nread;
  size_t remlen;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  بنانشانی حرف packet;

  DEBUGASSERT(mqtt);
  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  بدل(mqtt->state) {
MQTT_SUBACK_COMING:
  حالة MQTT_SUBACK_COMING:
    نتیجہ = mqtt_verify_suback(انکڑے);
    إذا(نتیجہ)
      اكسر;

    mqstate(انکڑے, MQTT_پہلا, MQTT_PUBWAIT);
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    إذا(packet == MQTT_پیغام_PUBLISH)
      mqstate(انکڑے, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    وإلا إذا(packet == MQTT_پیغام_SUBACK) {
      mqstate(انکڑے, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      اذهب MQTT_SUBACK_COMING;
    }
    وإلا إذا(packet == MQTT_پیغام_DISCONNECT) {
      infof(انکڑے, "Got DISCONNECT");
      *پورا = TRUE;
      اذهب سرا;
    }
    وإلا {
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      اذهب سرا;
    }

    /* -- switched state -- */
    remlen = mq->بچیا_لمائی;
    infof(انکڑے, "Remaining length: %zu bytes", remlen);
    إذا(انکڑے->سیٹ.max_filesize &&
       (curl_off_t)remlen > انکڑے->سیٹ.max_filesize) {
      failf(انکڑے, "Maximum file size exceeded");
      نتیجہ = CURLE_FILESIZE_EXCEEDED;
      اذهب سرا;
    }
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, remlen);
    انکڑے->req.bytecount = 0;
    انکڑے->req.ناپ = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  حالة MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    حرف بفر[4 * 1024];
    size_t rest = mq->npacket;
    إذا(rest > حجم(بفر))
      rest = حجم(بفر);
    نتیجہ = Curl_xfer_recv(انکڑے, بفر, rest, &nread);
    إذا(نتیجہ) {
      إذا(نتیجہ == CURLE_AGAIN) {
        infof(انکڑے, "EEEE AAAAGAIN");
      }
      اذهب سرا;
    }
    إذا(!nread) {
      infof(انکڑے, "server disconnected");
      نتیجہ = CURLE_PARTIAL_FILE;
      اذهب سرا;
    }

    /* we received something */
    mq->آخری_ویلا = *Curl_pgrs_now(انکڑے);

    /* if QoS is set, message contains packet id */
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, بفر, nread);
    إذا(نتیجہ)
      اذهب سرا;

    mq->npacket -= nread;
    إذا(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(انکڑے, MQTT_پہلا, MQTT_PUBWAIT);
    اكسر;
  }
  افتراضي:
    DEBUGASSERT(عدم); /* illegal state */
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
    اذهب سرا;
  }
سرا:
  أرجع نتیجہ;
}

ساكن CURLcode mqtt_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  CURLcode نتیجہ = CURLE_OK;
  *پورا = FALSE; /* unconditionally */

  إذا(!mq)
    أرجع CURLE_FAILED_INIT;
  mq->آخری_ویلا = *Curl_pgrs_now(انکڑے);
  mq->pingsent = FALSE;

  نتیجہ = mqtt_connect(انکڑے);
  إذا(نتیجہ) {
    failf(انکڑے, "Error %d sending MQTT CONNECT request", (صحيح)نتیجہ);
    أرجع نتیجہ;
  }
  mqstate(انکڑے, MQTT_پہلا, MQTT_CONNACK);
  أرجع CURLE_OK;
}

ساكن CURLcode mqtt_پورا(بنية Curl_easy *انکڑے,
                          CURLcode حالت, bool premature)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  (فراغ)حالت;
  (فراغ)premature;
  إذا(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  أرجع CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ساكن CURLcode mqtt_ping(بنية Curl_easy *انکڑے)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  إذا(mqtt->state == MQTT_پہلا &&
     !mq->pingsent &&
     انکڑے->سیٹ.upkeep_interval_ms > 0) {
    بنية curltime t = *Curl_pgrs_now(انکڑے);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->آخری_ویلا);

    إذا(diff > انکڑے->سیٹ.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      بنانشانی حرف packet[2] = { 0xC0, 0x00 };
      size_t packetlen = حجم(packet);

      نتیجہ = mqtt_send(انکڑے, (حرف *)packet, packetlen);
      إذا(!نتیجہ) {
        mq->pingsent = TRUE;
      }
      infof(انکڑے, "mqtt_ping: sent ping request.");
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode mqtt_doing(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية MQTT *mq = Curl_meta_get(انکڑے, CURL_META_MQTT_EASY);
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;
  بنانشانی حرف recvbyte;
  بنية mqtt_conn *mqtt = Curl_conn_meta_get(انکڑے->conn, CURL_META_MQTT_CONN);

  إذا(!mqtt || !mq)
    أرجع CURLE_FAILED_INIT;

  *پورا = FALSE;

  إذا(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    نتیجہ = mqtt_flush(انکڑے);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    إذا(نتیجہ || curlx_dyn_len(&mq->sendbuf))
      أرجع نتیجہ;
  }

  نتیجہ = mqtt_ping(انکڑے);
  /* Packet processing may send more output, so finish PINGREQ first. */
  إذا(نتیجہ || curlx_dyn_len(&mq->sendbuf))
    أرجع نتیجہ;

  infof(انکڑے, "mqtt_doing: state [%d]", (صحيح)mqtt->state);
  بدل(mqtt->state) {
  حالة MQTT_پہلا:
    /* Read the initial byte only */
    نتیجہ = Curl_xfer_recv(انکڑے, (حرف *)&mq->firstbyte, 1, &nread);
    إذا(نتیجہ)
      اكسر;
    وإلا إذا(!nread) {
      failf(انکڑے, "Connection disconnected");
      *پورا = TRUE;
      نتیجہ = CURLE_RECV_ERROR;
      اكسر;
    }
    Curl_debug(انکڑے, CURLINFO_سرخی_وچ, (ثابت حرف *)&mq->firstbyte, 1);

    /* we received something */
    mq->آخری_ویلا = *Curl_pgrs_now(انکڑے);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(انکڑے, MQTT_بچیا_لمائی, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_بچیا_لمائی:
    افعل {
      نتیجہ = Curl_xfer_recv(انکڑے, (حرف *)&recvbyte, 1, &nread);
      إذا(نتیجہ || !nread)
        اكسر;
      Curl_debug(انکڑے, CURLINFO_سرخی_وچ, (ثابت حرف *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } طالما((recvbyte & 0x80) && (mq->npacket < 4));
    إذا(!نتیجہ && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
    إذا(نتیجہ)
      اكسر;
    إذا(mqtt_decode_len(&mq->بچیا_لمائی, mq->pkt_hd, mq->npacket)) {
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      اكسر;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ثابت بنانشانی حرف type = mq->firstbyte & 0xF0;
      ثابت بنانشانی حرف reserved = mq->firstbyte & 0x0F;
      إذا((type == MQTT_پیغام_DISCONNECT || type == MQTT_پیغام_PINGRESP) &&
         (mq->بچیا_لمائی || reserved)) {
        failf(انکڑے,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_پیغام_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->بچیا_لمائی, mq->firstbyte);
        نتیجہ = CURLE_WEIRD_SERVER_جواب;
        اكسر;
      }
    }
    إذا(mq->بچیا_لمائی) {
      mqstate(انکڑے, mqtt->nextstate, MQTT_NOSTATE);
      اكسر;
    }
    mqstate(انکڑے, MQTT_پہلا, MQTT_پہلا);

    إذا(mq->firstbyte == MQTT_پیغام_DISCONNECT) {
      infof(انکڑے, "Got DISCONNECT");
      *پورا = TRUE;
    }

    /* ping response */
    إذا(mq->firstbyte == MQTT_پیغام_PINGRESP) {
      infof(انکڑے, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(انکڑے, MQTT_پہلا, MQTT_PUBWAIT);
    }
    اكسر;
  حالة MQTT_CONNACK:
    نتیجہ = mqtt_verify_connack(انکڑے);
    إذا(نتیجہ)
      اكسر;

    إذا(انکڑے->state.httpreq != HTTPREQ_POST) {
      نتیجہ = mqtt_subscribe(انکڑے);
      إذا(!نتیجہ)
        mqstate(انکڑے, MQTT_پہلا, MQTT_SUBACK);
      اكسر;
    }

    نتیجہ = mqtt_publish(انکڑے);
    إذا(نتیجہ)
      اكسر;
    mqstate(انکڑے, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    إذا(curlx_dyn_len(&mq->sendbuf))
      اكسر;
    نتیجہ = mqtt_disconnect(انکڑے);
    إذا(نتیجہ)
      اكسر;
    mqstate(انکڑے, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  حالة MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    إذا(!curlx_dyn_len(&mq->sendbuf))
      *پورا = TRUE;
    اكسر;

  حالة MQTT_SUBACK:
  حالة MQTT_PUBWAIT:
  حالة MQTT_PUB_REMAIN:
    نتیجہ = mqtt_read_publish(انکڑے, پورا);
    اكسر;

  افتراضي:
    failf(انکڑے, "State not handled yet");
    *پورا = TRUE;
    اكسر;
  }

  إذا(نتیجہ == CURLE_AGAIN)
    نتیجہ = CURLE_OK;
  أرجع نتیجہ;
}

#جے_تعریف_شدہ USE_SSL

ساكن CURLcode mqtts_connecting(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ;

  نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, TRUE, پورا);
  إذا(نتیجہ)
    connclose(conn);
  أرجع نتیجہ;
}

/*
 * MQTTS protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtts = {
  mqtt_تیاری_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_پورا,                          /* done */
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

#جے_انت

/*
 * MQTT protocol.
 */
ثابت بنية Curl_protocol Curl_protocol_mqtt = {
  mqtt_تیاری_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_پورا,                          /* done */
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

#جے_انت /* CURL_DISABLE_MQTT */
