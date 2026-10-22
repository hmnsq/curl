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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MQTT

#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mqtt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"

/* first byte is command.
   second byte is for flags. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_ਸੁਨੇਹਾ_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_ਸੁਨੇਹਾ_PUBLISH    0x30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_ਸੁਨੇਹਾ_SUBSCRIBE  0x82
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_ਸੁਨੇਹਾ_SUBACK     0x90
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_ਸੁਨੇਹਾ_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_ਸੁਨੇਹਾ_PINGRESP   0xD0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_CONNACK_LEN  2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_SUBACK_LEN   3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

ਗਿਣਤੀ mqttstate {
  MQTT_ਪਹਿਲਾ,             /* 0 */
  MQTT_ਬਚਿਆ_ਲੰਮਾਈ,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

ਬਣਤਰ mqtt_conn {
  ਗਿਣਤੀ mqttstate state;
  ਗਿਣਤੀ mqttstate nextstate; /* switch to this after remaining length is
ਪੂਰਾ*/
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ packetid;
};

/* protocol-specific transfer-related data */
ਬਣਤਰ MQTT {
  ਬਣਤਰ dynbuf sendbuf;
  /* when receiving */
  ਬਣਤਰ dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t ਬਚਿਆ_ਲੰਮਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ pkt_hd[4]; /* for decoding the arriving packet length */
  ਬਣਤਰ curltime ਆਖਰੀ_ਸਮਾਂ; /* last time we sent or received data */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ firstbyte;
  ਦੋਆਧਾਰੀ_ਅੰਕ(pingsent); /* 1 while we wait for ping response */
};

ਸਥਿਰ ਖਾਲੀ mqtt_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ MQTT *mq = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ਸਥਿਰ ਖਾਲੀ mqtt_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_free(ਇੰਦਰਾਜ);
}

ਸਥਿਰ CURLcode mqtt_ਤਿਆਰੀ_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ਬਣਤਰ mqtt_conn *mqtt;
  ਬਣਤਰ MQTT *mq;

  mqtt = curlx_calloc(1, ਆਕਾਰ(*mqtt));
  ਜੇ(!mqtt ||
     Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  mq = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ MQTT));
  ਜੇ(!mq)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  ਜੇ(Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mqtt_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਅੱਖਰ *buf, size_t len)
{
  size_t n;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);

  ਜੇ(!mq)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, buf, len, FALSE, &n);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  mq->ਆਖਰੀ_ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, buf, n);
  ਜੇ(len != n)
    ਨਤੀਜਾ = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ਸਥਿਰ CURLcode mqtt_flush(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!mq)
    ਵਾਪਸ CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  ਜੇ(!len)
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  mq->ਆਖਰੀ_ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, curlx_dyn_ptr(&mq->sendbuf), n);
  ਵਾਪਸ curlx_dyn_ਪੂਛ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
ਹਾਲਤਾਂ*/
ਸਥਿਰ CURLcode mqtt_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  ਜੇ(mq && curlx_dyn_len(&mq->sendbuf))
    ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]);
  ਵਾਪਸ Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]);
}

ਸਥਿਰ ਪੂਰਨ mqtt_encode_len(ਅੱਖਰ *buf, size_t len)
{
  ਪੂਰਨ i;

  ਲਈ(i = 0; (len > 0) && (i < 4); i++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ encoded;
    encoded = len % 0x80;
    len /= 0x80;
    ਜੇ(len)
      encoded |= 0x80;
    buf[i] = (ਅੱਖਰ)encoded;
  }

  ਵਾਪਸ i;
}

/* add the passwd to the CONNECT packet */
ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_passwd(ਅਚਲ ਅੱਖਰ *passwd, ਅਚਲ size_t plen,
                      ਅੱਖਰ *pkt, ਅਚਲ size_t ਸ਼ੁਰੂ, ਪੂਰਨ remain_pos)
{
  /* magic number that need to be set properly */
  ਅਚਲ size_t conn_flags_pos = remain_pos + 8;
  ਜੇ(plen > 0xffff)
    ਵਾਪਸ 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[ਸ਼ੁਰੂ] = (ਅੱਖਰ)((plen >> 8) & 0xFF);
  pkt[ਸ਼ੁਰੂ + 1] = (ਅੱਖਰ)(plen & 0xFF);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&pkt[ਸ਼ੁਰੂ + 2], passwd, plen);
  ਵਾਪਸ 0;
}

/* add user to the CONNECT packet */
ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_user(ਅਚਲ ਅੱਖਰ *username, ਅਚਲ size_t ulen,
                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pkt, ਅਚਲ size_t ਸ਼ੁਰੂ, ਪੂਰਨ remain_pos)
{
  /* magic number that need to be set properly */
  ਅਚਲ size_t conn_flags_pos = remain_pos + 8;
  ਜੇ(ulen > 0xffff)
    ਵਾਪਸ 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[ਸ਼ੁਰੂ] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((ulen >> 8) & 0xFF);
  pkt[ਸ਼ੁਰੂ + 1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(ulen & 0xFF);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&pkt[ਸ਼ੁਰੂ + 2], username, ulen);
  ਵਾਪਸ 0;
}

/* add client ID to the CONNECT packet */
ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_client_id(ਅਚਲ ਅੱਖਰ *client_id, ਅਚਲ size_t client_id_len,
                         ਅੱਖਰ *pkt, ਅਚਲ size_t ਸ਼ੁਰੂ)
{
  ਜੇ(client_id_len != MQTT_CLIENTID_LEN)
    ਵਾਪਸ 1;
  pkt[ਸ਼ੁਰੂ] = 0x00;
  pkt[ਸ਼ੁਰੂ + 1] = MQTT_CLIENTID_LEN;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&pkt[ਸ਼ੁਰੂ + 2], client_id, MQTT_CLIENTID_LEN);
  ਵਾਪਸ 0;
}

/* Set initial values of CONNECT packet */
ਸਥਿਰ ਪੂਰਨ init_connpack(ਅੱਖਰ *packet, ਅੱਖਰ *remain, ਪੂਰਨ remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_ਸੁਨੇਹਾ_CONNECT;
  /* remaining length field */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&packet[1], remain, remain_pos);
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
  ਵਾਪਸ remain_pos + 10;
}

ਸਥਿਰ CURLcode mqtt_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ pos = 0;
  ਪੂਰਨ rc = 0;
  /* remain length */
  ਪੂਰਨ remain_pos = 0;
  ਅੱਖਰ remain[4] = { 0 };
  size_t packetlen = 0;
  size_t ਸ਼ੁਰੂ_user = 0;
  size_t ਸ਼ੁਰੂ_pwd = 0;
  ਅੱਖਰ client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ਅਚਲ size_t clen = CURL_CSTRLEN("curl");
  ਅੱਖਰ *packet = ਨਿਰਾ;

  /* extracting username from request */
  ਬਣਤਰ Curl_creds *creds = ਅੰਕੜੇ->state.creds;
  ਅਚਲ size_t ulen = creds ? ਲੜੀ_ਲੰਬਾਈ(creds->user) : 0;
  ਅਚਲ size_t plen = creds ? ਲੜੀ_ਲੰਬਾਈ(creds->passwd) : 0;
  ਅਚਲ size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  ਜੇ(packetlen > 0xFFFFFFF)
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  packet = curlx_calloc(1, packetlen);
  ਜੇ(!packet)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  ਨਤੀਜਾ = Curl_rand_alnum(ਅੰਕੜੇ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = ਜੋੜੋ_client_id(client_id, ਲੜੀ_ਲੰਬਾਈ(client_id), packet, pos + 1);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "Client ID length mismatched: [%zu]", ਲੜੀ_ਲੰਬਾਈ(client_id));
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
    ਜਾਓ ਸਿਰਾ;
  }
  infof(ਅੰਕੜੇ, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  ਸ਼ੁਰੂ_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  ਸ਼ੁਰੂ_pwd = ਸ਼ੁਰੂ_user + ulen;
  /* if username was provided, add it to the packet */
  ਜੇ(ulen) {
    ਸ਼ੁਰੂ_pwd += 2;

    rc = ਜੋੜੋ_user(creds->user, ulen,
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)packet, ਸ਼ੁਰੂ_user, remain_pos);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "Username too long: [%zu]", ulen);
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
      ਜਾਓ ਸਿਰਾ;
    }
  }

  /* if passwd was provided, add it to the packet */
  ਜੇ(plen) {
    rc = ਜੋੜੋ_passwd(creds->passwd, plen, packet, ਸ਼ੁਰੂ_pwd, remain_pos);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "Password too long: [%zu]", plen);
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
      ਜਾਓ ਸਿਰਾ;
    }
  }

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = mqtt_send(ਅੰਕੜੇ, packet, packetlen);

ਸਿਰਾ:
  ਜੇ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&ਅੰਕੜੇ->state.creds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mqtt_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ mqtt_send(ਅੰਕੜੇ, "\xe0\x00", 2);
}

ਸਥਿਰ CURLcode mqtt_recv_atleast(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t nbytes)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!mq)
    ਵਾਪਸ CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  ਜੇ(rlen < nbytes) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < ਆਕਾਰ(readbuf));
    ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, (ਅੱਖਰ *)readbuf, nbytes - rlen, &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(!nread) /* EOF */
       ਵਾਪਸ CURLE_RECV_ERROR;
    ਜੇ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ਵਾਪਸ (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ਸਥਿਰ ਖਾਲੀ mqtt_recv_consume(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t nbytes)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  ਜੇ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    ਜੇ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    ਨਹੀਂਤਾਂ
      curlx_dyn_ਪੂਛ(&mq->recvbuf, rlen - nbytes);
  }
}

ਸਥਿਰ CURLcode mqtt_verify_connack(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਅੱਖਰ *ptr;

  DEBUGASSERT(mq);
  ਜੇ(!mq)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(mq->ਬਚਿਆ_ਲੰਮਾਈ != 2) {
    failf(ਅੰਕੜੇ, "CONNACK expected Remaining Length 2, got %zu",
          mq->ਬਚਿਆ_ਲੰਮਾਈ);
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਨਤੀਜਾ = mqtt_recv_atleast(ਅੰਕੜੇ, MQTT_CONNACK_LEN);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, ptr, MQTT_CONNACK_LEN);

  ਜੇ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(ਅੰਕੜੇ, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ptr[0], (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }
  mqtt_recv_consume(ਅੰਕੜੇ, MQTT_CONNACK_LEN);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mqtt_get_topic(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅੱਖਰ **topic, size_t *topiclen)
{
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ;
  CURLcode ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
  ਜੇ(ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) > 1) {
    ਨਤੀਜਾ = Curl_urldecode(ਰਸਤਾ + 1, 0, topic, topiclen, REJECT_CTRL);
    ਜੇ(!ਨਤੀਜਾ && (*topiclen > 0xffff)) {
      failf(ਅੰਕੜੇ, "Too long MQTT topic");
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
    }
  }
  ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "No MQTT topic found. Forgot to URL encode it?");

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mqtt_subscribe(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *topic = ਨਿਰਾ;
  size_t topiclen;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *packet = ਨਿਰਾ;
  size_t packetlen;
  ਅੱਖਰ encodedsize[4];
  size_t n;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ਜੇ(!mqtt)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = mqtt_get_topic(ਅੰਕੜੇ, &topic, &topiclen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((ਅੱਖਰ *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  ਜੇ(!packet) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  packet[0] = MQTT_ਸੁਨੇਹਾ_SUBSCRIBE;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  ਨਤੀਜਾ = mqtt_send(ਅੰਕੜੇ, (ਅਚਲ ਅੱਖਰ *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Called when the first byte was already read.
 */
ਸਥਿਰ CURLcode mqtt_verify_suback(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਅੱਖਰ *ptr;

  ਜੇ(!mqtt || !mq)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(mq->ਬਚਿਆ_ਲੰਮਾਈ != 3) {
    failf(ਅੰਕੜੇ, "SUBACK expected Remaining Length 3, got %zu",
          mq->ਬਚਿਆ_ਲੰਮਾਈ);
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਨਤੀਜਾ = mqtt_recv_atleast(ਅੰਕੜੇ, MQTT_SUBACK_LEN);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, ptr, MQTT_SUBACK_LEN);

  ਜੇ(((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
    ਜਾਓ fail;
  }
  mqtt_recv_consume(ਅੰਕੜੇ, MQTT_SUBACK_LEN);
fail:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_MQTT_ਸੁਨੇਹਾ_ਮਾਪ 0xFFFFFFF

ਸਥਿਰ CURLcode mqtt_publish(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ *payload = ਅੰਕੜੇ->ਸੈੱਟ.postfields;
  size_t payloadlen;
  ਅੱਖਰ *topic = ਨਿਰਾ;
  size_t topiclen;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pkt = ਨਿਰਾ;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  ਅੱਖਰ encodedbytes[4];
  curl_off_t postfieldsize = ਅੰਕੜੇ->ਸੈੱਟ.postfieldsize;

  ਜੇ(!payload) {
    DEBUGF(infof(ਅੰਕੜੇ, "mqtt_publish without payload, return bad arg"));
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
  ਜੇ(!curlx_sotouz_ਸਮਾਉਂਦਾ(postfieldsize, &payloadlen)) {
    ਜੇ(postfieldsize > 0) /* off_t does not fit into size_t */
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    payloadlen = ਲੜੀ_ਲੰਬਾਈ(payload);
  }

  ਨਤੀਜਾ = mqtt_get_topic(ਅੰਕੜੇ, &topic, &topiclen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  ਜੇ((SIZE_MAX - topiclen - 2) < payloadlen) {
    ਨਤੀਜਾ = CURLE_TOO_LARGE;
    ਜਾਓ fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  ਜੇ(remaininglength > (MAX_MQTT_ਸੁਨੇਹਾ_ਮਾਪ - encodelen - 1)) {
    ਨਤੀਜਾ = CURLE_TOO_LARGE;
    ਜਾਓ fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  ਜੇ(!pkt) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_ਸੁਨੇਹਾ_PUBLISH;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&pkt[i], topic, topiclen);
  i += topiclen;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&pkt[i], payload, payloadlen);
  i += payloadlen;
  ਨਤੀਜਾ = mqtt_send(ਅੰਕੜੇ, (ਅਚਲ ਅੱਖਰ *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* return FALSE on success, TRUE on error */
ਸਥਿਰ bool mqtt_decode_len(size_t *lenp, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ encoded = 128;

  ਲਈ(i = 0; (i < buflen) && (encoded & 128); i++) {
    ਜੇ(i == 4)
      ਵਾਪਸ TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  ਜੇ(encoded & 128)
    /* truncated size */
    ਵਾਪਸ TRUE;

  *lenp = len;
  ਵਾਪਸ FALSE;
}

#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ statenames[] = {
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
#ਜੇ_ਅੰਤ

/* The only way to change state */
ਸਥਿਰ ਖਾਲੀ mqstate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਗਿਣਤੀ mqttstate state,
                    ਗਿਣਤੀ mqttstate nextstate) /* used if state == FIRST */
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  ਜੇ(!mqtt)
    ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  infof(ਅੰਕੜੇ, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_ਪਹਿਲਾ) ? statenames[nextstate] : "");
#ਜੇ_ਅੰਤ
  mqtt->state = state;
  ਜੇ(state == MQTT_ਪਹਿਲਾ)
    mqtt->nextstate = nextstate;
}

ਸਥਿਰ CURLcode mqtt_read_publish(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  size_t nread;
  size_t remlen;
  ਬਣਤਰ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ packet;

  DEBUGASSERT(mqtt);
  ਜੇ(!mqtt || !mq)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਚੋਣ(mqtt->state) {
MQTT_SUBACK_COMING:
  ਹਾਲਤ MQTT_SUBACK_COMING:
    ਨਤੀਜਾ = mqtt_verify_suback(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    mqstate(ਅੰਕੜੇ, MQTT_ਪਹਿਲਾ, MQTT_PUBWAIT);
    ਰੋਕੋ;

  ਹਾਲਤ MQTT_SUBACK:
  ਹਾਲਤ MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    ਜੇ(packet == MQTT_ਸੁਨੇਹਾ_PUBLISH)
      mqstate(ਅੰਕੜੇ, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    ਨਹੀਂਤਾਂ ਜੇ(packet == MQTT_ਸੁਨੇਹਾ_SUBACK) {
      mqstate(ਅੰਕੜੇ, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      ਜਾਓ MQTT_SUBACK_COMING;
    }
    ਨਹੀਂਤਾਂ ਜੇ(packet == MQTT_ਸੁਨੇਹਾ_DISCONNECT) {
      infof(ਅੰਕੜੇ, "Got DISCONNECT");
      *ਪੂਰਾ = TRUE;
      ਜਾਓ ਸਿਰਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
      ਜਾਓ ਸਿਰਾ;
    }

    /* -- switched state -- */
    remlen = mq->ਬਚਿਆ_ਲੰਮਾਈ;
    infof(ਅੰਕੜੇ, "Remaining length: %zu bytes", remlen);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_filesize &&
       (curl_off_t)remlen > ਅੰਕੜੇ->ਸੈੱਟ.max_filesize) {
      failf(ਅੰਕੜੇ, "Maximum file size exceeded");
      ਨਤੀਜਾ = CURLE_FILESIZE_EXCEEDED;
      ਜਾਓ ਸਿਰਾ;
    }
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, remlen);
    ਅੰਕੜੇ->req.bytecount = 0;
    ਅੰਕੜੇ->req.ਮਾਪ = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  ਹਾਲਤ MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    ਅੱਖਰ ਬਫਰ[4 * 1024];
    size_t rest = mq->npacket;
    ਜੇ(rest > ਆਕਾਰ(ਬਫਰ))
      rest = ਆਕਾਰ(ਬਫਰ);
    ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, ਬਫਰ, rest, &nread);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        infof(ਅੰਕੜੇ, "EEEE AAAAGAIN");
      }
      ਜਾਓ ਸਿਰਾ;
    }
    ਜੇ(!nread) {
      infof(ਅੰਕੜੇ, "server disconnected");
      ਨਤੀਜਾ = CURLE_PARTIAL_FILE;
      ਜਾਓ ਸਿਰਾ;
    }

    /* we received something */
    mq->ਆਖਰੀ_ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);

    /* if QoS is set, message contains packet id */
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਬਫਰ, nread);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਸਿਰਾ;

    mq->npacket -= nread;
    ਜੇ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(ਅੰਕੜੇ, MQTT_ਪਹਿਲਾ, MQTT_PUBWAIT);
    ਰੋਕੋ;
  }
  ਮੂਲ:
    DEBUGASSERT(ਨਿਰਾ); /* illegal state */
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
    ਜਾਓ ਸਿਰਾ;
  }
ਸਿਰਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mqtt_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  *ਪੂਰਾ = FALSE; /* unconditionally */

  ਜੇ(!mq)
    ਵਾਪਸ CURLE_FAILED_INIT;
  mq->ਆਖਰੀ_ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  mq->pingsent = FALSE;

  ਨਤੀਜਾ = mqtt_connect(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "Error %d sending MQTT CONNECT request", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  mqstate(ਅੰਕੜੇ, MQTT_ਪਹਿਲਾ, MQTT_CONNACK);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mqtt_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          CURLcode ਸਥਿਤੀ, bool premature)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  (ਖਾਲੀ)ਸਥਿਤੀ;
  (ਖਾਲੀ)premature;
  ਜੇ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ਵਾਪਸ CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ਸਥਿਰ CURLcode mqtt_ping(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ਜੇ(!mqtt || !mq)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(mqtt->state == MQTT_ਪਹਿਲਾ &&
     !mq->pingsent &&
     ਅੰਕੜੇ->ਸੈੱਟ.upkeep_interval_ms > 0) {
    ਬਣਤਰ curltime t = *Curl_pgrs_now(ਅੰਕੜੇ);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->ਆਖਰੀ_ਸਮਾਂ);

    ਜੇ(diff > ਅੰਕੜੇ->ਸੈੱਟ.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ packet[2] = { 0xC0, 0x00 };
      size_t packetlen = ਆਕਾਰ(packet);

      ਨਤੀਜਾ = mqtt_send(ਅੰਕੜੇ, (ਅੱਖਰ *)packet, packetlen);
      ਜੇ(!ਨਤੀਜਾ) {
        mq->pingsent = TRUE;
      }
      infof(ਅੰਕੜੇ, "mqtt_ping: sent ping request.");
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mqtt_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ MQTT *mq = Curl_meta_get(ਅੰਕੜੇ, CURL_META_MQTT_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ recvbyte;
  ਬਣਤਰ mqtt_conn *mqtt = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_MQTT_CONN);

  ਜੇ(!mqtt || !mq)
    ਵਾਪਸ CURLE_FAILED_INIT;

  *ਪੂਰਾ = FALSE;

  ਜੇ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    ਨਤੀਜਾ = mqtt_flush(ਅੰਕੜੇ);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    ਜੇ(ਨਤੀਜਾ || curlx_dyn_len(&mq->sendbuf))
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਨਤੀਜਾ = mqtt_ping(ਅੰਕੜੇ);
  /* Packet processing may send more output, so finish PINGREQ first. */
  ਜੇ(ਨਤੀਜਾ || curlx_dyn_len(&mq->sendbuf))
    ਵਾਪਸ ਨਤੀਜਾ;

  infof(ਅੰਕੜੇ, "mqtt_doing: state [%d]", (ਪੂਰਨ)mqtt->state);
  ਚੋਣ(mqtt->state) {
  ਹਾਲਤ MQTT_ਪਹਿਲਾ:
    /* Read the initial byte only */
    ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, (ਅੱਖਰ *)&mq->firstbyte, 1, &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    ਨਹੀਂਤਾਂ ਜੇ(!nread) {
      failf(ਅੰਕੜੇ, "Connection disconnected");
      *ਪੂਰਾ = TRUE;
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਰੋਕੋ;
    }
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, (ਅਚਲ ਅੱਖਰ *)&mq->firstbyte, 1);

    /* we received something */
    mq->ਆਖਰੀ_ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(ਅੰਕੜੇ, MQTT_ਬਚਿਆ_ਲੰਮਾਈ, MQTT_NOSTATE);
    FALLTHROUGH();
  ਹਾਲਤ MQTT_ਬਚਿਆ_ਲੰਮਾਈ:
    ਕਰੋ {
      ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, (ਅੱਖਰ *)&recvbyte, 1, &nread);
      ਜੇ(ਨਤੀਜਾ || !nread)
        ਰੋਕੋ;
      Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, (ਅਚਲ ਅੱਖਰ *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } ਜਦੋਂ((recvbyte & 0x80) && (mq->npacket < 4));
    ਜੇ(!ਨਤੀਜਾ && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    ਜੇ(mqtt_decode_len(&mq->ਬਚਿਆ_ਲੰਮਾਈ, mq->pkt_hd, mq->npacket)) {
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
      ਰੋਕੋ;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ type = mq->firstbyte & 0xF0;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ reserved = mq->firstbyte & 0x0F;
      ਜੇ((type == MQTT_ਸੁਨੇਹਾ_DISCONNECT || type == MQTT_ਸੁਨੇਹਾ_PINGRESP) &&
         (mq->ਬਚਿਆ_ਲੰਮਾਈ || reserved)) {
        failf(ਅੰਕੜੇ,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_ਸੁਨੇਹਾ_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->ਬਚਿਆ_ਲੰਮਾਈ, mq->firstbyte);
        ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
        ਰੋਕੋ;
      }
    }
    ਜੇ(mq->ਬਚਿਆ_ਲੰਮਾਈ) {
      mqstate(ਅੰਕੜੇ, mqtt->nextstate, MQTT_NOSTATE);
      ਰੋਕੋ;
    }
    mqstate(ਅੰਕੜੇ, MQTT_ਪਹਿਲਾ, MQTT_ਪਹਿਲਾ);

    ਜੇ(mq->firstbyte == MQTT_ਸੁਨੇਹਾ_DISCONNECT) {
      infof(ਅੰਕੜੇ, "Got DISCONNECT");
      *ਪੂਰਾ = TRUE;
    }

    /* ping response */
    ਜੇ(mq->firstbyte == MQTT_ਸੁਨੇਹਾ_PINGRESP) {
      infof(ਅੰਕੜੇ, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(ਅੰਕੜੇ, MQTT_ਪਹਿਲਾ, MQTT_PUBWAIT);
    }
    ਰੋਕੋ;
  ਹਾਲਤ MQTT_CONNACK:
    ਨਤੀਜਾ = mqtt_verify_connack(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ਜੇ(ਅੰਕੜੇ->state.httpreq != HTTPREQ_POST) {
      ਨਤੀਜਾ = mqtt_subscribe(ਅੰਕੜੇ);
      ਜੇ(!ਨਤੀਜਾ)
        mqstate(ਅੰਕੜੇ, MQTT_ਪਹਿਲਾ, MQTT_SUBACK);
      ਰੋਕੋ;
    }

    ਨਤੀਜਾ = mqtt_publish(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    mqstate(ਅੰਕੜੇ, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ਹਾਲਤ MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    ਜੇ(curlx_dyn_len(&mq->sendbuf))
      ਰੋਕੋ;
    ਨਤੀਜਾ = mqtt_disconnect(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    mqstate(ਅੰਕੜੇ, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ਹਾਲਤ MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    ਜੇ(!curlx_dyn_len(&mq->sendbuf))
      *ਪੂਰਾ = TRUE;
    ਰੋਕੋ;

  ਹਾਲਤ MQTT_SUBACK:
  ਹਾਲਤ MQTT_PUBWAIT:
  ਹਾਲਤ MQTT_PUB_REMAIN:
    ਨਤੀਜਾ = mqtt_read_publish(ਅੰਕੜੇ, ਪੂਰਾ);
    ਰੋਕੋ;

  ਮੂਲ:
    failf(ਅੰਕੜੇ, "State not handled yet");
    *ਪੂਰਾ = TRUE;
    ਰੋਕੋ;
  }

  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਨਤੀਜਾ = CURLE_OK;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL

ਸਥਿਰ CURLcode mqtts_connecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, TRUE, ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    connclose(conn);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * MQTTS protocol.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_mqtts = {
  mqtt_ਤਿਆਰੀ_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_ਪੂਰਾ,                          /* done */
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

#ਜੇ_ਅੰਤ

/*
 * MQTT protocol.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_mqtt = {
  mqtt_ਤਿਆਰੀ_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_ਪੂਰਾ,                          /* done */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_MQTT */
