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

#اگر defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#تعریف_کرو DEBUG_ME 0

#شامل "vauth/vauth.h"
#شامل "curl_trc.h"
#شامل "curl_ntlm_core.h"
#شامل "rand.h"
#شامل "curlx/strdup.h"
#شامل "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#تعریف_کرو NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#تعریف_کرو NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#تعریف_کرو NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#تعریف_کرو NTLMFLAG_REQUEST_ہدف                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#اگر DEBUG_ME
/* unknown (1 << 3) */
#تعریف_کرو NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#تعریف_کرو NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#تعریف_کرو NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_LM_کلید                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#اگر_ختم

#تعریف_کرو NTLMFLAG_NEGOTIATE_NTLM_کلید              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#اگر DEBUG_ME
/* unknown (1 << 10) */

#تعریف_کرو NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#اگر_ختم

#تعریف_کرو NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#اگر DEBUG_ME
#تعریف_کرو NTLMFLAG_ہدف_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#تعریف_کرو NTLMFLAG_ہدف_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#تعریف_کرو NTLMFLAG_ہدف_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#اگر_ختم

#تعریف_کرو NTLMFLAG_NEGOTIATE_NTLM2_کلید             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#اگر DEBUG_ME
#تعریف_کرو NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#تعریف_کرو NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#تعریف_کرو NTLMFLAG_REQUEST_NONNT_SESSION_کلید       (1 << 22)
/* unknown purpose */
#اگر_ختم

#تعریف_کرو NTLMFLAG_NEGOTIATE_ہدف_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#اگر DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#تعریف_کرو NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_کلید_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#تعریف_کرو NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#تعریف_کرو DEBUG_برآمد(x) x
ساکن خالی ntlm_print_flags(FILE *handle, غیرعلامتی طویل flags)
{
  اگر(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  اگر(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  اگر(flags & NTLMFLAG_REQUEST_ہدف)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  اگر(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  اگر(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  اگر(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  اگر(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  اگر(flags & NTLMFLAG_NEGOTIATE_LM_کلید)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  اگر(flags & NTLMFLAG_NEGOTIATE_NTLM_کلید)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  اگر(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  اگر(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  اگر(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  اگر(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  اگر(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  اگر(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  اگر(flags & NTLMFLAG_ہدف_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  اگر(flags & NTLMFLAG_ہدف_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  اگر(flags & NTLMFLAG_ہدف_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  اگر(flags & NTLMFLAG_NEGOTIATE_NTLM2_کلید)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  اگر(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  اگر(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  اگر(flags & NTLMFLAG_REQUEST_NONNT_SESSION_کلید)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  اگر(flags & NTLMFLAG_NEGOTIATE_ہدف_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  اگر(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  اگر(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  اگر(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  اگر(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  اگر(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  اگر(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  اگر(flags & NTLMFLAG_NEGOTIATE_کلید_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  اگر(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

ساکن خالی ntlm_print_hex(FILE *handle, ثابت حرف *buf, size_t len)
{
  ثابت حرف *p = buf;

  (خالی)handle;

  curl_mfprintf(stderr, "0x");
  جب(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (غیرعلامتی صحیح)*p++);
}
#ورنہ
#تعریف_کرو DEBUG_برآمد(x) Curl_nop_stmt
#اگر_ختم

/*
 * ntlm_decode_type2_target()
 *
 * This is used to decode the "target info" in the NTLM type-2 message
 * received.
 *
 * Parameters:
 *
 * data      [in]     - The session handle.
 * type2ref  [in]     - The type-2 message.
 * ntlm      [in/out] - The NTLM data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
ساکن CURLcode ntlm_decode_type2_ہدف(ساخت Curl_easy *ڈیٹا,
                                         ثابت ساخت bufref *type2ref,
                                         ساخت ntlmdata *ntlm)
{
  غیرعلامتی مختصر ہدف_info_len = 0;
  غیرعلامتی صحیح ہدف_info_انحراف = 0;
  ثابت غیرعلامتی حرف *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  اگر(type2len >= 48) {
    ہدف_info_len = Curl_read16_le(&type2[40]);
    ہدف_info_انحراف = Curl_read32_le(&type2[44]);
    اگر(ہدف_info_len > 0) {
      اگر((ہدف_info_انحراف > type2len) ||
         (ہدف_info_انحراف + ہدف_info_len) > type2len ||
         ہدف_info_انحراف < 48) {
        infof(ڈیٹا, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        واپس CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->ہدف_info); /* replace any previous data */
      ntlm->ہدف_info = curlx_memdup(&type2[ہدف_info_انحراف],
                                       ہدف_info_len);
      اگر(!ntlm->ہدف_info)
        واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  ntlm->ہدف_info_len = ہدف_info_len;

  واپس CURLE_OK;
}

/*
  NTLM message structure notes:

  A 'short' is a 'network short', a little-endian 16-bit unsigned value.

  A 'long' is a 'network long', a little-endian, 32-bit unsigned value.

  A 'security buffer' represents a triplet used to point to a buffer,
  consisting of two shorts and one long:

    1. A 'short' containing the length of the buffer content in bytes.
    2. A 'short' containing the allocated space for the buffer in bytes.
    3. A 'long' containing the offset to the start of the buffer in bytes,
       from the beginning of the NTLM message.
 */

/*
 * Curl_auth_is_ntlm_supported()
 *
 * This is used to evaluate if NTLM is supported.
 *
 * Parameters: None
 *
 * Returns TRUE as NTLM as handled by libcurl.
 */
bool Curl_auth_is_ntlm_supported(خالی)
{
  واپس TRUE;
}

/*
 * Curl_auth_decode_ntlm_type2_message()
 *
 * This is used to decode an NTLM type-2 message. The raw NTLM message is
 * checked * for validity before the appropriate data for creating a type-3
 * message is * written to the given NTLM data structure.
 *
 * Parameters:
 *
 * data     [in]     - The session handle.
 * type2ref [in]     - The type-2 message.
 * ntlm     [in/out] - The NTLM data struct being used and modified.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_decode_ntlm_type2_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت bufref *type2ref,
                                             ساخت ntlmdata *ntlm)
{
  ساکن ثابت حرف type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

  /* NTLM type-2 message structure:

          Index  Description            Content
            0    NTLMSSP Signature      null-terminated ASCII "NTLMSSP"
                                        (0x4e544c4d53535000)
            8    NTLM Message Type      long (0x02000000)
           12    Target Name            security buffer
           20    Flags                  long
           24    Challenge              8 bytes
          (32)   Context                8 bytes (two consecutive longs) (*)
          (40)   Target Information     security buffer (*)
          (48)   OS Version Structure   8 bytes (*)
  32 (48) (56)   Start of data block    (*)
                                        (*) -> Optional
  */

  CURLcode نتیجہ = CURLE_OK;
  ثابت غیرعلامتی حرف *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  اگر((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, حجم(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(ڈیٹا, "NTLM handshake failure (bad type-2 message)");
    واپس CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  حافظہ_نقل(ntlm->nonce, &type2[24], 8);

  اگر(ntlm->flags & NTLMFLAG_NEGOTIATE_ہدف_INFO) {
    نتیجہ = ntlm_decode_type2_ہدف(ڈیٹا, type2ref, ntlm);
    اگر(نتیجہ) {
      infof(ڈیٹا, "NTLM handshake failure (bad type-2 message)");
      واپس نتیجہ;
    }
  }

  DEBUG_برآمد({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (حرف *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", سرخی);
  });

  واپس نتیجہ;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
ساکن خالی unicodecpy(غیرعلامتی حرف *dest, ثابت حرف *src, size_t لمبائی)
{
  size_t i;
  برائے(i = 0; i < لمبائی; i++) {
    dest[2 * i] = (غیرعلامتی حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Curl_auth_create_ntlm_type1_message()
 *
 * This is used to generate an NTLM type-1 message ready for sending to the
 * recipient using the appropriate compile time crypto API.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The username in the format User or Domain\User.
 * passwdp [in]     - The user's password.
 * service [in]     - The service type such as http, smtp, pop or imap.
 * host    [in]     - The hostname.
 * ntlm    [in/out] - The NTLM data struct being used and modified.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_ntlm_type1_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             ساخت ntlmdata *ntlm,
                                             ساخت bufref *برآمد)
{
  /* NTLM type-1 message structure:

       Index  Description            Content
         0    NTLMSSP Signature      null-terminated ASCII "NTLMSSP"
                                     (0x4e544c4d53535000)
         8    NTLM Message Type      long (0x01000000)
        12    Flags                  long
       (16)   Supplied Domain        security buffer (*)
       (24)   Supplied Workstation   security buffer (*)
       (32)   OS Version Structure   8 bytes (*)
  (32) (40)   Start of data block    (*)
                                     (*) -> Optional
  */

  ثابت حرف *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t سائز;

  حرف *ntlmbuf;
  ثابت حرف *hostname = "";          /*خالی_خانے*/
  ثابت حرف *domain = "";            /*خالی_خانے*/
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (خالی)ڈیٹا;
  (خالی)creds;
  (خالی)service;
  (خالی)host;

  /* Clean up any former leftovers and initialize to defaults */
  Curl_auth_cleanup_ntlm(ntlm);

  ntlmbuf = curl_maprintf(NTLMSSP_SIGNATURE "%c"
                          "\x01%c%c%c" /* 32-bit type = 1 */
                          "%c%c%c%c"   /* 32-bit NTLM flag field */
                          "%c%c"       /* domain length */
                          "%c%c"       /* domain allocated space */
                          "%c%c"       /* domain name offset */
                          "%c%c"       /* 2 zeroes */
                          "%c%c"       /* host length */
                          "%c%c"       /* host allocated space */
                          "%c%c"       /* hostname offset */
                          "%c%c"       /* 2 zeroes */
                          "%s"         /* hostname */
                          "%s",        /* domain string */
                          0,           /* null-terminator */
                          0, 0, 0,     /* part of type-1 long */

                          LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                                      NTLMFLAG_REQUEST_ہدف |
                                      NTLMFLAG_NEGOTIATE_NTLM_کلید |
                                      NTLMFLAG_NEGOTIATE_NTLM2_کلید |
                                      NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                          SHORTPAIR(domlen),
                          SHORTPAIR(domlen),
                          SHORTPAIR(domoff),
                          0, 0,
                          SHORTPAIR(hostlen),
                          SHORTPAIR(hostlen),
                          SHORTPAIR(hostoff),
                          0, 0,
                          hostname, /* this is empty */
                          domain /* this is empty */);

  اگر(!ntlmbuf)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Initial packet length */
  سائز = 32 + hostlen + domlen;

  DEBUG_برآمد({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_ہدف |
                              NTLMFLAG_NEGOTIATE_NTLM_کلید |
                              NTLMFLAG_NEGOTIATE_NTLM2_کلید |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_ہدف |
                  NTLMFLAG_NEGOTIATE_NTLM_کلید |
                  NTLMFLAG_NEGOTIATE_NTLM2_کلید |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_ہدف |
                     NTLMFLAG_NEGOTIATE_NTLM_کلید |
                     NTLMFLAG_NEGOTIATE_NTLM2_کلید |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_سیٹ(برآمد, ntlmbuf, سائز, curl_free);
  واپس CURLE_OK;
}

/*
 * Curl_auth_create_ntlm_type3_message()
 *
 * This is used to generate an already encoded NTLM type-3 message ready for
 * sending to the recipient using the appropriate compile time crypto API.
 *
 * Parameters:
 *
 * data    [in]     - The session handle.
 * userp   [in]     - The username in the format User or Domain\User.
 * passwdp [in]     - The user's password.
 * ntlm    [in/out] - The NTLM data struct being used and modified.
 * out     [out]    - The result storage.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_auth_create_ntlm_type3_پیغام(ساخت Curl_easy *ڈیٹا,
                                             ساخت Curl_creds *creds,
                                             ساخت ntlmdata *ntlm,
                                             ساخت bufref *برآمد)
{
  /* NTLM type-3 message structure:

          Index  Description            Content
            0    NTLMSSP Signature      null-terminated ASCII "NTLMSSP"
                                        (0x4e544c4d53535000)
            8    NTLM Message Type      long (0x03000000)
           12    LM/LMv2 Response       security buffer
           20    NTLM/NTLMv2 Response   security buffer
           28    Target Name            security buffer
           36    Username               security buffer
           44    Workstation Name       security buffer
          (52)   Session Key            security buffer (*)
          (60)   Flags                  long (*)
          (64)   OS Version Structure   8 bytes (*)
  52 (64) (72)   Start of data block
                                          (*) -> Optional
  */

  CURLcode نتیجہ = CURLE_OK;
  size_t سائز;
  غیرعلامتی حرف ntlmbuf[NTLM_BUFSIZE];
  غیرعلامتی صحیح lmrespoff;
  غیرعلامتی حرف lmresp[24]; /* fixed-size */
  غیرعلامتی صحیح ntrespoff;
  غیرعلامتی صحیح ntresplen = 24;
  غیرعلامتی حرف ntresp[24]; /* fixed-size */
  ثابت غیرعلامتی حرف *ptr_ntresp = &ntresp[0];
  غیرعلامتی حرف *ntlmv2resp = تہی;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  ساکن ثابت حرف host[] = "WORKSTATION";
  ثابت حرف *userp = Curl_creds_user(creds);
  ثابت حرف *passwdp = Curl_creds_passwd(creds);
  ثابت حرف *user;
  ثابت حرف *domain = "";
  size_t hostoff = 0;
  size_t useroff = 0;
  size_t domoff = 0;
  size_t hostlen = 0;
  size_t userlen = 0;
  size_t domlen = 0;

  memset(lmresp, 0, حجم(lmresp));
  memset(ntresp, 0, حجم(ntresp));
  user = strchr(userp, '\\');
  اگر(!user)
    user = strchr(userp, '/');

  اگر(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  ورنہ
    user = userp;

  userlen = لڑی_لمبائی(user);
  hostlen = حجم(host) - 1;

  اگر(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_کلید) {
    غیرعلامتی حرف ntbuffer[0x18];
    غیرعلامتی حرف entropy[8];
    غیرعلامتی حرف ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    نتیجہ = Curl_rand(ڈیٹا, entropy, 8);
    اگر(نتیجہ)
      واپس نتیجہ;

    نتیجہ = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    اگر(نتیجہ)
      واپس نتیجہ;

    نتیجہ = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* LMv2 response */
    نتیجہ = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* NTLMv2 response */
    نتیجہ = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    اگر(نتیجہ)
      واپس نتیجہ;

    ptr_ntresp = ntlmv2resp;
  }
  ورنہ {

    غیرعلامتی حرف ntbuffer[0x18];
    غیرعلامتی حرف lmbuffer[0x18];

    /* NTLM version 1 */

    نتیجہ = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    اگر(نتیجہ)
      واپس نتیجہ;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    نتیجہ = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    اگر(نتیجہ)
      واپس نتیجہ;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(غیرعلامتی صحیح)NTLMFLAG_NEGOTIATE_NTLM2_کلید;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  اگر(unicode) {
    domlen = domlen * 2;
    userlen = userlen * 2;
    hostlen = hostlen * 2;
  }

  lmrespoff = 64; /* size of the message header */
  ntrespoff = lmrespoff + 0x18;
  domoff = ntrespoff + ntresplen;
  useroff = domoff + domlen;
  hostoff = useroff + userlen;

  /* Create the big type-3 message binary blob */
  سائز = curl_msnprintf((حرف *)ntlmbuf, NTLM_BUFSIZE,
                         NTLMSSP_SIGNATURE "%c"
                         "\x03%c%c%c"  /* 32-bit type = 3 */

                         "%c%c"  /* LanManager length */
                         "%c%c"  /* LanManager allocated space */
                         "%c%c"  /* LanManager offset */
                         "%c%c"  /* 2 zeroes */

                         "%c%c"  /* NT-response length */
                         "%c%c"  /* NT-response allocated space */
                         "%c%c"  /* NT-response offset */
                         "%c%c"  /* 2 zeroes */

                         "%c%c"  /* domain length */
                         "%c%c"  /* domain allocated space */
                         "%c%c"  /* domain name offset */
                         "%c%c"  /* 2 zeroes */

                         "%c%c"  /* user length */
                         "%c%c"  /* user allocated space */
                         "%c%c"  /* user offset */
                         "%c%c"  /* 2 zeroes */

                         "%c%c"  /* host length */
                         "%c%c"  /* host allocated space */
                         "%c%c"  /* host offset */
                         "%c%c"  /* 2 zeroes */

                         "%c%c"  /* session key length (unknown purpose) */
                         "%c%c"  /* session key allocated space
                                    (unknown purpose) */
                         "%c%c"  /* session key offset (unknown purpose) */
                         "%c%c"  /* 2 zeroes */

                         "%c%c%c%c",  /* flags */

                         /* domain string */
                         /* user string */
                         /* host string */
                         /* LanManager response */
                         /* NT response */

                         0,                /* null-termination */
                         0, 0, 0,          /* type-3 long, the 24 upper bits */

                         SHORTPAIR(0x18),  /* LanManager response length,
                                              twice */
                         SHORTPAIR(0x18),
                         SHORTPAIR(lmrespoff),
                         0x0, 0x0,

                         SHORTPAIR(ntresplen),  /* NT-response length, twice */
                         SHORTPAIR(ntresplen),
                         SHORTPAIR(ntrespoff),
                         0x0, 0x0,

                         SHORTPAIR(domlen),
                         SHORTPAIR(domlen),
                         SHORTPAIR(domoff),
                         0x0, 0x0,

                         SHORTPAIR(userlen),
                         SHORTPAIR(userlen),
                         SHORTPAIR(useroff),
                         0x0, 0x0,

                         SHORTPAIR(hostlen),
                         SHORTPAIR(hostlen),
                         SHORTPAIR(hostoff),
                         0x0, 0x0,

                         0x0, 0x0,
                         0x0, 0x0,
                         0x0, 0x0,
                         0x0, 0x0,

                         LONGQUARTET(ntlm->flags));

  DEBUGASSERT(سائز == 64);
  DEBUGASSERT(سائز == (size_t)lmrespoff);

  /* We append the binary hashes */
  اگر(سائز < (NTLM_BUFSIZE - 0x18)) {
    حافظہ_نقل(&ntlmbuf[سائز], lmresp, 0x18);
    سائز += 0x18;
  }

  DEBUG_برآمد({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (حرف *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  اگر(ntresplen + سائز > حجم(ntlmbuf)) {
    failf(ڈیٹا, "incoming NTLM message too big");
    نتیجہ = CURLE_TOO_LARGE;
    جاؤ error;
  }
  DEBUGASSERT(سائز == (size_t)ntrespoff);
  حافظہ_نقل(&ntlmbuf[سائز], ptr_ntresp, ntresplen);
  سائز += ntresplen;

  DEBUG_برآمد({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (حرف *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_برآمد({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  اگر(سائز + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(ڈیٹا, "user + domain + hostname too big for NTLM");
    نتیجہ = CURLE_TOO_LARGE;
    جاؤ error;
  }

  DEBUGASSERT(سائز == domoff);
  اگر(unicode)
    unicodecpy(&ntlmbuf[سائز], domain, domlen / 2);
  ورنہ
    حافظہ_نقل(&ntlmbuf[سائز], domain, domlen);

  سائز += domlen;

  DEBUGASSERT(سائز == useroff);
  اگر(unicode)
    unicodecpy(&ntlmbuf[سائز], user, userlen / 2);
  ورنہ
    حافظہ_نقل(&ntlmbuf[سائز], user, userlen);

  سائز += userlen;

  DEBUGASSERT(سائز == hostoff);
  اگر(unicode)
    unicodecpy(&ntlmbuf[سائز], host, hostlen / 2);
  ورنہ
    حافظہ_نقل(&ntlmbuf[سائز], host, hostlen);

  سائز += hostlen;

  /* Return the binary blob. */
  نتیجہ = Curl_bufref_memdup0(برآمد, ntlmbuf, سائز);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  واپس نتیجہ;
}

/*
 * Curl_auth_cleanup_ntlm()
 *
 * This is used to clean up the NTLM specific data.
 *
 * Parameters:
 *
 * ntlm    [in/out] - The NTLM data struct being cleaned up.
 *
 */
خالی Curl_auth_cleanup_ntlm(ساخت ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->ہدف_info);

  /* Reset any variables */
  ntlm->ہدف_info_len = 0;
}

#اگر_ختم /* USE_NTLM && !USE_WINDOWS_SSPI */
