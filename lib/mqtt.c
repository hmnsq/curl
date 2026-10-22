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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MQTT

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "mqtt.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

/* first byte is command.
   second byte is for flags. */
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_ಸಂದೇಶ_CONNECT    0x10
/* #define MQTT_MSG_CONNACK    0x20 */
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_ಸಂದೇಶ_PUBLISH    0x30
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_ಸಂದೇಶ_SUBSCRIBE  0x82
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_ಸಂದೇಶ_SUBACK     0x90
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_ಸಂದೇಶ_DISCONNECT 0xe0
/* #define MQTT_MSG_PINGREQ    0xC0 */
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_ಸಂದೇಶ_PINGRESP   0xD0

#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_CONNACK_LEN  2
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_SUBACK_LEN   3
#ವ್ಯಾಖ್ಯಾನಿಸು MQTT_CLIENTID_LEN 12 /* "curl0123abcd" */

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_MQTT_EASY   "meta:proto:mqtt:easy"
/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_MQTT_CONN   "meta:proto:mqtt:conn"

ಎಣಿಕೆ mqttstate {
  MQTT_ಮೊದಲ,             /* 0 */
  MQTT_ಉಳಿದ_ಉದ್ದಳತೆ,  /* 1 */
  MQTT_CONNACK,           /* 2 */
  MQTT_SUBACK,            /* 3 */
  MQTT_SUBACK_COMING,     /* 4 - the SUBACK remainder */
  MQTT_PUBWAIT,    /* 5 - wait for publish */
  MQTT_PUB_REMAIN,  /* 6 - wait for the remainder of the publish */
  MQTT_POST_DRAIN,        /* 7 - finish sending PUBLISH */
  MQTT_DISCONNECT_DRAIN,  /* 8 - finish sending DISCONNECT */

  MQTT_NOSTATE /* 9 - never used an actual state */
};

ರಚನೆ mqtt_conn {
  ಎಣಿಕೆ mqttstate state;
  ಎಣಿಕೆ mqttstate nextstate; /* switch to this after remaining length is
ಮುಗಿದ*/
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ packetid;
};

/* protocol-specific transfer-related data */
ರಚನೆ MQTT {
  ರಚನೆ dynbuf sendbuf;
  /* when receiving */
  ರಚನೆ dynbuf recvbuf;
  size_t npacket; /* byte counter */
  size_t ಉಳಿದ_ಉದ್ದಳತೆ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ pkt_hd[4]; /* for decoding the arriving packet length */
  ರಚನೆ curltime ಕೊನೆಯ_ಸಮಯ; /* last time we sent or received data */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ firstbyte;
  ಬಿಟ್(pingsent); /* 1 while we wait for ping response */
};

ಸ್ಥಿರ ಶೂನ್ಯ mqtt_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ MQTT *mq = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_dyn_free(&mq->sendbuf);
  curlx_dyn_free(&mq->recvbuf);
  curlx_free(mq);
}

ಸ್ಥಿರ ಶೂನ್ಯ mqtt_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(ನಮೂದು);
}

ಸ್ಥಿರ CURLcode mqtt_ಸಿದ್ಧತೆ_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn)
{
  /* setup MQTT specific meta data at easy handle and connection */
  ರಚನೆ mqtt_conn *mqtt;
  ರಚನೆ MQTT *mq;

  mqtt = curlx_calloc(1, ಗಾತ್ರ(*mqtt));
  ಆದರೆ(!mqtt ||
     Curl_conn_meta_ಗಣ(conn, CURL_META_MQTT_CONN, mqtt, mqtt_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  mq = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ MQTT));
  ಆದರೆ(!mq)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  curlx_dyn_init(&mq->recvbuf, DYN_MQTT_RECV);
  curlx_dyn_init(&mq->sendbuf, DYN_MQTT_SEND);
  ಆದರೆ(Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY, mq, mqtt_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mqtt_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t len)
{
  size_t n;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);

  ಆದರೆ(!mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* A new packet must not replace a previously queued packet tail. */
  DEBUGASSERT(!curlx_dyn_len(&mq->sendbuf));
  ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, buf, len, FALSE, &n);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  mq->ಕೊನೆಯ_ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, buf, n);
  ಆದರೆ(len != n)
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&mq->sendbuf, &buf[n], len - n);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Send the queued tail, preserving any bytes that still cannot be sent. */
ಸ್ಥಿರ CURLcode mqtt_flush(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  size_t len, n;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  len = curlx_dyn_len(&mq->sendbuf);
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, curlx_dyn_ptr(&mq->sendbuf), len, FALSE, &n);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  mq->ಕೊನೆಯ_ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, curlx_dyn_ptr(&mq->sendbuf), n);
  ಹಿಂತಿರುಗಿ curlx_dyn_ಬಾಲ(&mq->sendbuf, len - n);
}

/* Generic function called by the multi interface to figure out what socket(s)
   to wait for and for what actions during the DOING and PROTOCONNECT
ಸ್ಥಿತಿಗಳು*/
ಸ್ಥಿರ CURLcode mqtt_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  ಆದರೆ(mq && curlx_dyn_len(&mq->sendbuf))
    ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]);
  ಹಿಂತಿರುಗಿ Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mqtt_encode_len(ಅಕ್ಷರ *buf, size_t len)
{
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; (len > 0) && (i < 4); i++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ encoded;
    encoded = len % 0x80;
    len /= 0x80;
    ಆದರೆ(len)
      encoded |= 0x80;
    buf[i] = (ಅಕ್ಷರ)encoded;
  }

  ಹಿಂತಿರುಗಿ i;
}

/* add the passwd to the CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_passwd(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd, ಸ್ಥಿರಾಂಕ size_t plen,
                      ಅಕ್ಷರ *pkt, ಸ್ಥಿರಾಂಕ size_t ಆರಂಭ, ಪೂರ್ಣಾಂಕ remain_pos)
{
  /* magic number that need to be set properly */
  ಸ್ಥಿರಾಂಕ size_t conn_flags_pos = remain_pos + 8;
  ಆದರೆ(plen > 0xffff)
    ಹಿಂತಿರುಗಿ 1;

  /* set password flag */
  pkt[conn_flags_pos] |= 0x40;

  /* length of password provided */
  pkt[ಆರಂಭ] = (ಅಕ್ಷರ)((plen >> 8) & 0xFF);
  pkt[ಆರಂಭ + 1] = (ಅಕ್ಷರ)(plen & 0xFF);
  ಸ್ಮರಣೆ_ನಕಲು(&pkt[ಆರಂಭ + 2], passwd, plen);
  ಹಿಂತಿರುಗಿ 0;
}

/* add user to the CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_user(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username, ಸ್ಥಿರಾಂಕ size_t ulen,
                    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pkt, ಸ್ಥಿರಾಂಕ size_t ಆರಂಭ, ಪೂರ್ಣಾಂಕ remain_pos)
{
  /* magic number that need to be set properly */
  ಸ್ಥಿರಾಂಕ size_t conn_flags_pos = remain_pos + 8;
  ಆದರೆ(ulen > 0xffff)
    ಹಿಂತಿರುಗಿ 1;

  /* set username flag */
  pkt[conn_flags_pos] |= 0x80;
  /* length of username provided */
  pkt[ಆರಂಭ] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((ulen >> 8) & 0xFF);
  pkt[ಆರಂಭ + 1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ulen & 0xFF);
  ಸ್ಮರಣೆ_ನಕಲು(&pkt[ಆರಂಭ + 2], username, ulen);
  ಹಿಂತಿರುಗಿ 0;
}

/* add client ID to the CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_client_id(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *client_id, ಸ್ಥಿರಾಂಕ size_t client_id_len,
                         ಅಕ್ಷರ *pkt, ಸ್ಥಿರಾಂಕ size_t ಆರಂಭ)
{
  ಆದರೆ(client_id_len != MQTT_CLIENTID_LEN)
    ಹಿಂತಿರುಗಿ 1;
  pkt[ಆರಂಭ] = 0x00;
  pkt[ಆರಂಭ + 1] = MQTT_CLIENTID_LEN;
  ಸ್ಮರಣೆ_ನಕಲು(&pkt[ಆರಂಭ + 2], client_id, MQTT_CLIENTID_LEN);
  ಹಿಂತಿರುಗಿ 0;
}

/* Set initial values of CONNECT packet */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ init_connpack(ಅಕ್ಷರ *packet, ಅಕ್ಷರ *remain, ಪೂರ್ಣಾಂಕ remain_pos)
{
  /* Fixed header starts */
  /* packet type */
  packet[0] = MQTT_ಸಂದೇಶ_CONNECT;
  /* remaining length field */
  ಸ್ಮರಣೆ_ನಕಲು(&packet[1], remain, remain_pos);
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
  ಹಿಂತಿರುಗಿ remain_pos + 10;
}

ಸ್ಥಿರ CURLcode mqtt_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ pos = 0;
  ಪೂರ್ಣಾಂಕ rc = 0;
  /* remain length */
  ಪೂರ್ಣಾಂಕ remain_pos = 0;
  ಅಕ್ಷರ remain[4] = { 0 };
  size_t packetlen = 0;
  size_t ಆರಂಭ_user = 0;
  size_t ಆರಂಭ_pwd = 0;
  ಅಕ್ಷರ client_id[MQTT_CLIENTID_LEN + 1] = "curl";
  ಸ್ಥಿರಾಂಕ size_t clen = CURL_CSTRLEN("curl");
  ಅಕ್ಷರ *packet = ಖಾಲಿ;

  /* extracting username from request */
  ರಚನೆ Curl_creds *creds = ದತ್ತಾಂಶ->state.creds;
  ಸ್ಥಿರಾಂಕ size_t ulen = creds ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(creds->user) : 0;
  ಸ್ಥಿರಾಂಕ size_t plen = creds ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(creds->passwd) : 0;
  ಸ್ಥಿರಾಂಕ size_t payloadlen = ulen + plen + MQTT_CLIENTID_LEN + 2 +
  /* The plus 2s below are for the MSB and LSB describing the length of the
     string to be added on the payload. Refer to spec 1.5.2 and 1.5.4 */
    (ulen ? 2 : 0) +
    (plen ? 2 : 0);

  /* getting how much occupy the remain length */
  remain_pos = mqtt_encode_len(remain, payloadlen + 10);

  /* 10 length of variable header and 1 the first byte of the fixed header */
  packetlen = payloadlen + 10 + remain_pos + 1;

  /* allocating packet */
  ಆದರೆ(packetlen > 0xFFFFFFF)
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  packet = curlx_calloc(1, packetlen);
  ಆದರೆ(!packet)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* set initial values for the CONNECT packet */
  pos = init_connpack(packet, remain, remain_pos);

  ಫಲಿತಾಂಶ = Curl_rand_alnum(ದತ್ತಾಂಶ, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&client_id[clen],
                           MQTT_CLIENTID_LEN - clen + 1);
  /* add client id */
  rc = ಕೂಡಿಸು_client_id(client_id, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client_id), packet, pos + 1);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "Client ID length mismatched: [%zu]", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(client_id));
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
    ಹೋಗು ಕೊನೆ;
  }
  infof(ದತ್ತಾಂಶ, "Using client id '%s'", client_id);

  /* position where the user payload starts */
  ಆರಂಭ_user = pos + 3 + MQTT_CLIENTID_LEN;
  /* position where the password payload starts */
  ಆರಂಭ_pwd = ಆರಂಭ_user + ulen;
  /* if username was provided, add it to the packet */
  ಆದರೆ(ulen) {
    ಆರಂಭ_pwd += 2;

    rc = ಕೂಡಿಸು_user(creds->user, ulen,
                  (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)packet, ಆರಂಭ_user, remain_pos);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "Username too long: [%zu]", ulen);
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
      ಹೋಗು ಕೊನೆ;
    }
  }

  /* if passwd was provided, add it to the packet */
  ಆದರೆ(plen) {
    rc = ಕೂಡಿಸು_passwd(creds->passwd, plen, packet, ಆರಂಭ_pwd, remain_pos);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "Password too long: [%zu]", plen);
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
      ಹೋಗು ಕೊನೆ;
    }
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = mqtt_send(ದತ್ತಾಂಶ, packet, packetlen);

ಕೊನೆ:
  ಆದರೆ(packet) {
    curlx_memzero(packet, packetlen);
    curlx_free(packet);
  }
  Curl_creds_unlink(&ದತ್ತಾಂಶ->state.creds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mqtt_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ mqtt_send(ದತ್ತಾಂಶ, "\xe0\x00", 2);
}

ಸ್ಥಿರ CURLcode mqtt_recv_atleast(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t nbytes)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  size_t rlen;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  rlen = curlx_dyn_len(&mq->recvbuf);

  ಆದರೆ(rlen < nbytes) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ readbuf[1024];
    size_t nread;

    DEBUGASSERT(nbytes - rlen < ಗಾತ್ರ(readbuf));
    ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)readbuf, nbytes - rlen, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(!nread) /* EOF */
       ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    ಆದರೆ(curlx_dyn_addn(&mq->recvbuf, readbuf, nread))
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    rlen = curlx_dyn_len(&mq->recvbuf);
  }
  ಹಿಂತಿರುಗಿ (rlen >= nbytes) ? CURLE_OK : CURLE_AGAIN;
}

ಸ್ಥಿರ ಶೂನ್ಯ mqtt_recv_consume(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t nbytes)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  DEBUGASSERT(mq);
  ಆದರೆ(mq) {
    size_t rlen = curlx_dyn_len(&mq->recvbuf);
    ಆದರೆ(rlen <= nbytes)
      curlx_dyn_reset(&mq->recvbuf);
    ಇಲ್ಲದಿದ್ದರೆ
      curlx_dyn_ಬಾಲ(&mq->recvbuf, rlen - nbytes);
  }
}

ಸ್ಥಿರ CURLcode mqtt_verify_connack(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;

  DEBUGASSERT(mq);
  ಆದರೆ(!mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(mq->ಉಳಿದ_ಉದ್ದಳತೆ != 2) {
    failf(ದತ್ತಾಂಶ, "CONNACK expected Remaining Length 2, got %zu",
          mq->ಉಳಿದ_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಫಲಿತಾಂಶ = mqtt_recv_atleast(ದತ್ತಾಂಶ, MQTT_CONNACK_LEN);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* verify CONNACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_CONNACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, ptr, MQTT_CONNACK_LEN);

  ಆದರೆ(ptr[0] != 0x00 || ptr[1] != 0x00) {
    failf(ದತ್ತಾಂಶ, "Expected %02x%02x but got %02x%02x",
          0x00U, 0x00U, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ptr[0], (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ptr[1]);
    curlx_dyn_reset(&mq->recvbuf);
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }
  mqtt_recv_consume(ದತ್ತಾಂಶ, MQTT_CONNACK_LEN);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mqtt_get_topic(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಅಕ್ಷರ **topic, size_t *topiclen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) > 1) {
    ಫಲಿತಾಂಶ = Curl_urldecode(ಮಾರ್ಗ + 1, 0, topic, topiclen, REJECT_CTRL);
    ಆದರೆ(!ಫಲಿತಾಂಶ && (*topiclen > 0xffff)) {
      failf(ದತ್ತಾಂಶ, "Too long MQTT topic");
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "No MQTT topic found. Forgot to URL encode it?");

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mqtt_subscribe(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *topic = ಖಾಲಿ;
  size_t topiclen;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *packet = ಖಾಲಿ;
  size_t packetlen;
  ಅಕ್ಷರ encodedsize[4];
  size_t n;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ಆದರೆ(!mqtt)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = mqtt_get_topic(ದತ್ತಾಂಶ, &topic, &topiclen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  mqtt->packetid++;

  packetlen = topiclen + 5; /* packetid + topic (has a two byte length field)
                               + 2 bytes topic length + QoS byte */
  n = mqtt_encode_len((ಅಕ್ಷರ *)encodedsize, packetlen);
  packetlen += n + 1; /* add one for the control packet type byte */

  packet = curlx_malloc(packetlen);
  ಆದರೆ(!packet) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  packet[0] = MQTT_ಸಂದೇಶ_SUBSCRIBE;
  ಸ್ಮರಣೆ_ನಕಲು(&packet[1], encodedsize, n);
  packet[1 + n] = (mqtt->packetid >> 8) & 0xff;
  packet[2 + n] = mqtt->packetid & 0xff;
  packet[3 + n] = (topiclen >> 8) & 0xff;
  packet[4 + n] = topiclen & 0xff;
  ಸ್ಮರಣೆ_ನಕಲು(&packet[5 + n], topic, topiclen);
  packet[5 + n + topiclen] = 0; /* QoS zero */

  ಫಲಿತಾಂಶ = mqtt_send(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)packet, packetlen);

fail:
  curlx_free(topic);
  curlx_free(packet);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Called when the first byte was already read.
 */
ಸ್ಥಿರ CURLcode mqtt_verify_suback(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;

  ಆದರೆ(!mqtt || !mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(mq->ಉಳಿದ_ಉದ್ದಳತೆ != 3) {
    failf(ದತ್ತಾಂಶ, "SUBACK expected Remaining Length 3, got %zu",
          mq->ಉಳಿದ_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಫಲಿತಾಂಶ = mqtt_recv_atleast(ದತ್ತಾಂಶ, MQTT_SUBACK_LEN);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  /* verify SUBACK */
  DEBUGASSERT(curlx_dyn_len(&mq->recvbuf) >= MQTT_SUBACK_LEN);
  ptr = curlx_dyn_ptr(&mq->recvbuf);
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, ptr, MQTT_SUBACK_LEN);

  ಆದರೆ(((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ptr[0]) != ((mqtt->packetid >> 8) & 0xff) ||
     ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ptr[1]) != (mqtt->packetid & 0xff) ||
     ptr[2] != 0x00) {
    curlx_dyn_reset(&mq->recvbuf);
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
    ಹೋಗು fail;
  }
  mqtt_recv_consume(ದತ್ತಾಂಶ, MQTT_SUBACK_LEN);
fail:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_MQTT_ಸಂದೇಶ_ಗಾತ್ರ 0xFFFFFFF

ಸ್ಥಿರ CURLcode mqtt_publish(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ *payload = ದತ್ತಾಂಶ->ಗಣ.postfields;
  size_t payloadlen;
  ಅಕ್ಷರ *topic = ಖಾಲಿ;
  size_t topiclen;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pkt = ಖಾಲಿ;
  size_t i = 0;
  size_t remaininglength;
  size_t encodelen;
  ಅಕ್ಷರ encodedbytes[4];
  curl_off_t postfieldsize = ದತ್ತಾಂಶ->ಗಣ.postfieldsize;

  ಆದರೆ(!payload) {
    DEBUGF(infof(ದತ್ತಾಂಶ, "mqtt_publish without payload, return bad arg"));
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
  ಆದರೆ(!curlx_sotouz_ಹೊಂದುತ್ತದೆ(postfieldsize, &payloadlen)) {
    ಆದರೆ(postfieldsize > 0) /* off_t does not fit into size_t */
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    payloadlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(payload);
  }

  ಫಲಿತಾಂಶ = mqtt_get_topic(ದತ್ತಾಂಶ, &topic, &topiclen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು fail;

  /* silly check for silly analyzers. On 32-bit systems a payload length close
     to 4GB can wrap, but it is impossible to have such a large buffer in
     memory on those systems */
  ಆದರೆ((SIZE_MAX - topiclen - 2) < payloadlen) {
    ಫಲಿತಾಂಶ = CURLE_TOO_LARGE;
    ಹೋಗು fail;
  }

  remaininglength = payloadlen + 2 + topiclen;
  encodelen = mqtt_encode_len(encodedbytes, remaininglength);
  ಆದರೆ(remaininglength > (MAX_MQTT_ಸಂದೇಶ_ಗಾತ್ರ - encodelen - 1)) {
    ಫಲಿತಾಂಶ = CURLE_TOO_LARGE;
    ಹೋಗು fail;
  }

  /* add the control byte and the encoded remaining length */
  pkt = curlx_malloc(remaininglength + 1 + encodelen);
  ಆದರೆ(!pkt) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು fail;
  }

  /* assemble packet */
  pkt[i++] = MQTT_ಸಂದೇಶ_PUBLISH;
  ಸ್ಮರಣೆ_ನಕಲು(&pkt[i], encodedbytes, encodelen);
  i += encodelen;
  pkt[i++] = (topiclen >> 8) & 0xff;
  pkt[i++] = (topiclen & 0xff);
  ಸ್ಮರಣೆ_ನಕಲು(&pkt[i], topic, topiclen);
  i += topiclen;
  ಸ್ಮರಣೆ_ನಕಲು(&pkt[i], payload, payloadlen);
  i += payloadlen;
  ಫಲಿತಾಂಶ = mqtt_send(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)pkt, i);

fail:
  curlx_free(pkt);
  curlx_free(topic);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* return FALSE on success, TRUE on error */
ಸ್ಥಿರ bool mqtt_decode_len(size_t *lenp, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf,
                            size_t buflen)
{
  size_t len = 0;
  size_t mult = 1;
  size_t i;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ encoded = 128;

  ಗಾಗಿ(i = 0; (i < buflen) && (encoded & 128); i++) {
    ಆದರೆ(i == 4)
      ಹಿಂತಿರುಗಿ TRUE; /* bad size */
    encoded = buf[i];
    len += (encoded & 127) * mult;
    mult *= 128;
  }
  ಆದರೆ(encoded & 128)
    /* truncated size */
    ಹಿಂತಿರುಗಿ TRUE;

  *lenp = len;
  ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ statenames[] = {
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
#ಆದರೆ_ಅಂತ್ಯ

/* The only way to change state */
ಸ್ಥಿರ ಶೂನ್ಯ mqstate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ಎಣಿಕೆ mqttstate state,
                    ಎಣಿಕೆ mqttstate nextstate) /* used if state == FIRST */
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  DEBUGASSERT(mqtt);
  ಆದರೆ(!mqtt)
    ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  infof(ದತ್ತಾಂಶ, "%s (from %s) (next is %s)",
        statenames[state],
        statenames[mqtt->state],
        (state == MQTT_ಮೊದಲ) ? statenames[nextstate] : "");
#ಆದರೆ_ಅಂತ್ಯ
  mqtt->state = state;
  ಆದರೆ(state == MQTT_ಮೊದಲ)
    mqtt->nextstate = nextstate;
}

ಸ್ಥಿರ CURLcode mqtt_read_publish(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  size_t nread;
  size_t remlen;
  ರಚನೆ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ packet;

  DEBUGASSERT(mqtt);
  ಆದರೆ(!mqtt || !mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಯ್ಕೆ(mqtt->state) {
MQTT_SUBACK_COMING:
  ಸ್ಥಿತಿ MQTT_SUBACK_COMING:
    ಫಲಿತಾಂಶ = mqtt_verify_suback(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    mqstate(ದತ್ತಾಂಶ, MQTT_ಮೊದಲ, MQTT_PUBWAIT);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ MQTT_SUBACK:
  ಸ್ಥಿತಿ MQTT_PUBWAIT:
    /* we are expecting PUBLISH or SUBACK */
    packet = mq->firstbyte & 0xf0;
    ಆದರೆ(packet == MQTT_ಸಂದೇಶ_PUBLISH)
      mqstate(ದತ್ತಾಂಶ, MQTT_PUB_REMAIN, MQTT_NOSTATE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(packet == MQTT_ಸಂದೇಶ_SUBACK) {
      mqstate(ದತ್ತಾಂಶ, MQTT_SUBACK_COMING, MQTT_NOSTATE);
      ಹೋಗು MQTT_SUBACK_COMING;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(packet == MQTT_ಸಂದೇಶ_DISCONNECT) {
      infof(ದತ್ತಾಂಶ, "Got DISCONNECT");
      *ಮುಗಿದ = TRUE;
      ಹೋಗು ಕೊನೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
      ಹೋಗು ಕೊನೆ;
    }

    /* -- switched state -- */
    remlen = mq->ಉಳಿದ_ಉದ್ದಳತೆ;
    infof(ದತ್ತಾಂಶ, "Remaining length: %zu bytes", remlen);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_filesize &&
       (curl_off_t)remlen > ದತ್ತಾಂಶ->ಗಣ.max_filesize) {
      failf(ದತ್ತಾಂಶ, "Maximum file size exceeded");
      ಫಲಿತಾಂಶ = CURLE_FILESIZE_EXCEEDED;
      ಹೋಗು ಕೊನೆ;
    }
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, remlen);
    ದತ್ತಾಂಶ->req.bytecount = 0;
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = remlen;
    mq->npacket = remlen; /* get this many bytes */
    FALLTHROUGH();
  ಸ್ಥಿತಿ MQTT_PUB_REMAIN: {
    /* read rest of packet, but no more. Cap to buffer size */
    ಅಕ್ಷರ ಬಫರ್[4 * 1024];
    size_t rest = mq->npacket;
    ಆದರೆ(rest > ಗಾತ್ರ(ಬಫರ್))
      rest = ಗಾತ್ರ(ಬಫರ್);
    ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, ಬಫರ್, rest, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        infof(ದತ್ತಾಂಶ, "EEEE AAAAGAIN");
      }
      ಹೋಗು ಕೊನೆ;
    }
    ಆದರೆ(!nread) {
      infof(ದತ್ತಾಂಶ, "server disconnected");
      ಫಲಿತಾಂಶ = CURLE_PARTIAL_FILE;
      ಹೋಗು ಕೊನೆ;
    }

    /* we received something */
    mq->ಕೊನೆಯ_ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);

    /* if QoS is set, message contains packet id */
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಬಫರ್, nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಕೊನೆ;

    mq->npacket -= nread;
    ಆದರೆ(!mq->npacket)
      /* no more PUBLISH payload, back to subscribe wait state */
      mqstate(ದತ್ತಾಂಶ, MQTT_ಮೊದಲ, MQTT_PUBWAIT);
    ನಿಲ್ಲಿಸು;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(ಖಾಲಿ); /* illegal state */
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
    ಹೋಗು ಕೊನೆ;
  }
ಕೊನೆ:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mqtt_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  *ಮುಗಿದ = FALSE; /* unconditionally */

  ಆದರೆ(!mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  mq->ಕೊನೆಯ_ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  mq->pingsent = FALSE;

  ಫಲಿತಾಂಶ = mqtt_connect(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "Error %d sending MQTT CONNECT request", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  mqstate(ದತ್ತಾಂಶ, MQTT_ಮೊದಲ, MQTT_CONNACK);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mqtt_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          CURLcode ಸ್ಥಿತಿ_2, bool premature)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  (ಶೂನ್ಯ)ಸ್ಥಿತಿ_2;
  (ಶೂನ್ಯ)premature;
  ಆದರೆ(mq) {
    curlx_dyn_free(&mq->sendbuf);
    curlx_dyn_free(&mq->recvbuf);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* we ping regularly to avoid being disconnected by the server */
ಸ್ಥಿರ CURLcode mqtt_ping(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ mqtt_conn *mqtt = Curl_conn_meta_get(conn, CURL_META_MQTT_CONN);

  ಆದರೆ(!mqtt || !mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(mqtt->state == MQTT_ಮೊದಲ &&
     !mq->pingsent &&
     ದತ್ತಾಂಶ->ಗಣ.upkeep_interval_ms > 0) {
    ರಚನೆ curltime t = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    timediff_t diff = curlx_ptimediff_ms(&t, &mq->ಕೊನೆಯ_ಸಮಯ);

    ಆದರೆ(diff > ದತ್ತಾಂಶ->ಗಣ.upkeep_interval_ms) {
      /* 0xC0 is PINGREQ, and 0x00 is remaining length */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ packet[2] = { 0xC0, 0x00 };
      size_t packetlen = ಗಾತ್ರ(packet);

      ಫಲಿತಾಂಶ = mqtt_send(ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)packet, packetlen);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        mq->pingsent = TRUE;
      }
      infof(ದತ್ತಾಂಶ, "mqtt_ping: sent ping request.");
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mqtt_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ MQTT *mq = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_MQTT_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ recvbyte;
  ರಚನೆ mqtt_conn *mqtt = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_MQTT_CONN);

  ಆದರೆ(!mqtt || !mq)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  *ಮುಗಿದ = FALSE;

  ಆದರೆ(curlx_dyn_len(&mq->sendbuf)) {
    /* send the remainder of an outgoing packet */
    ಫಲಿತಾಂಶ = mqtt_flush(ದತ್ತಾಂಶ);
    /* CURLE_OK can still mean a short write. Wait for writable progress
       before sending another packet or processing a reply. */
    ಆದರೆ(ಫಲಿತಾಂಶ || curlx_dyn_len(&mq->sendbuf))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಫಲಿತಾಂಶ = mqtt_ping(ದತ್ತಾಂಶ);
  /* Packet processing may send more output, so finish PINGREQ first. */
  ಆದರೆ(ಫಲಿತಾಂಶ || curlx_dyn_len(&mq->sendbuf))
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  infof(ದತ್ತಾಂಶ, "mqtt_doing: state [%d]", (ಪೂರ್ಣಾಂಕ)mqtt->state);
  ಆಯ್ಕೆ(mqtt->state) {
  ಸ್ಥಿತಿ MQTT_ಮೊದಲ:
    /* Read the initial byte only */
    ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)&mq->firstbyte, 1, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nread) {
      failf(ದತ್ತಾಂಶ, "Connection disconnected");
      *ಮುಗಿದ = TRUE;
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&mq->firstbyte, 1);

    /* we received something */
    mq->ಕೊನೆಯ_ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);

    /* remember the first byte */
    mq->npacket = 0;
    mqstate(ದತ್ತಾಂಶ, MQTT_ಉಳಿದ_ಉದ್ದಳತೆ, MQTT_NOSTATE);
    FALLTHROUGH();
  ಸ್ಥಿತಿ MQTT_ಉಳಿದ_ಉದ್ದಳತೆ:
    ಮಾಡು {
      ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)&recvbyte, 1, &nread);
      ಆದರೆ(ಫಲಿತಾಂಶ || !nread)
        ನಿಲ್ಲಿಸು;
      Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&recvbyte, 1);
      mq->pkt_hd[mq->npacket++] = recvbyte;
    } ಆಗ((recvbyte & 0x80) && (mq->npacket < 4));
    ಆದರೆ(!ಫಲಿತಾಂಶ && nread && (recvbyte & 0x80))
      /* MQTT supports up to 127 * 128^0 + 127 * 128^1 + 127 * 128^2 +
         127 * 128^3 bytes. server tried to send more */
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ(mqtt_decode_len(&mq->ಉಳಿದ_ಉದ್ದಳತೆ, mq->pkt_hd, mq->npacket)) {
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
      ನಿಲ್ಲಿಸು;
    }
    mq->npacket = 0;
    /* PINGRESP and DISCONNECT must have remaining_length == 0 and
     * reserved bits (low nibble) must be zero per MQTT 3.1.1
     * sections 2.2.2, 3.13.1 and 3.14.1. Reject before state
     * dispatch to prevent nextstate confusion. */
    {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ type = mq->firstbyte & 0xF0;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ reserved = mq->firstbyte & 0x0F;
      ಆದರೆ((type == MQTT_ಸಂದೇಶ_DISCONNECT || type == MQTT_ಸಂದೇಶ_PINGRESP) &&
         (mq->ಉಳಿದ_ಉದ್ದಳತೆ || reserved)) {
        failf(ದತ್ತಾಂಶ,
              "Broker sent malformed %s "
              "(remaining_length=%zu, header byte=0x%02x)",
              type == MQTT_ಸಂದೇಶ_DISCONNECT ? "DISCONNECT" : "PINGRESP",
              mq->ಉಳಿದ_ಉದ್ದಳತೆ, mq->firstbyte);
        ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
        ನಿಲ್ಲಿಸು;
      }
    }
    ಆದರೆ(mq->ಉಳಿದ_ಉದ್ದಳತೆ) {
      mqstate(ದತ್ತಾಂಶ, mqtt->nextstate, MQTT_NOSTATE);
      ನಿಲ್ಲಿಸು;
    }
    mqstate(ದತ್ತಾಂಶ, MQTT_ಮೊದಲ, MQTT_ಮೊದಲ);

    ಆದರೆ(mq->firstbyte == MQTT_ಸಂದೇಶ_DISCONNECT) {
      infof(ದತ್ತಾಂಶ, "Got DISCONNECT");
      *ಮುಗಿದ = TRUE;
    }

    /* ping response */
    ಆದರೆ(mq->firstbyte == MQTT_ಸಂದೇಶ_PINGRESP) {
      infof(ದತ್ತಾಂಶ, "Received ping response.");
      mq->pingsent = FALSE;
      mqstate(ದತ್ತಾಂಶ, MQTT_ಮೊದಲ, MQTT_PUBWAIT);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ MQTT_CONNACK:
    ಫಲಿತಾಂಶ = mqtt_verify_connack(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(ದತ್ತಾಂಶ->state.httpreq != HTTPREQ_POST) {
      ಫಲಿತಾಂಶ = mqtt_subscribe(ದತ್ತಾಂಶ);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        mqstate(ದತ್ತಾಂಶ, MQTT_ಮೊದಲ, MQTT_SUBACK);
      ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = mqtt_publish(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    mqstate(ದತ್ತಾಂಶ, MQTT_POST_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ಸ್ಥಿತಿ MQTT_POST_DRAIN:
    /* PUBLISH must be complete before DISCONNECT can use the send buffer. */
    ಆದರೆ(curlx_dyn_len(&mq->sendbuf))
      ನಿಲ್ಲಿಸು;
    ಫಲಿತಾಂಶ = mqtt_disconnect(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    mqstate(ದತ್ತಾಂಶ, MQTT_DISCONNECT_DRAIN, MQTT_NOSTATE);
    FALLTHROUGH();
  ಸ್ಥಿತಿ MQTT_DISCONNECT_DRAIN:
    /* Completing now would discard any DISCONNECT bytes still queued. */
    ಆದರೆ(!curlx_dyn_len(&mq->sendbuf))
      *ಮುಗಿದ = TRUE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ MQTT_SUBACK:
  ಸ್ಥಿತಿ MQTT_PUBWAIT:
  ಸ್ಥಿತಿ MQTT_PUB_REMAIN:
    ಫಲಿತಾಂಶ = mqtt_read_publish(ದತ್ತಾಂಶ, ಮುಗಿದ);
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "State not handled yet");
    *ಮುಗಿದ = TRUE;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಫಲಿತಾಂಶ = CURLE_OK;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL

ಸ್ಥಿರ CURLcode mqtts_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, TRUE, ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    connclose(conn);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * MQTTS protocol.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_mqtts = {
  mqtt_ಸಿದ್ಧತೆ_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_ಮುಗಿದ,                          /* done */
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

#ಆದರೆ_ಅಂತ್ಯ

/*
 * MQTT protocol.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_mqtt = {
  mqtt_ಸಿದ್ಧತೆ_conn,                    /* setup_connection */
  mqtt_do,                            /* do_it */
  mqtt_ಮುಗಿದ,                          /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_MQTT */
