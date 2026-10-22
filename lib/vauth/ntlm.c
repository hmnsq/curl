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
#ضمن "curl_setup.h"

#إذا defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#تعریف_کړه DEBUG_ME 0

#ضمن "vauth/vauth.h"
#ضمن "curl_trc.h"
#ضمن "curl_ntlm_core.h"
#ضمن "rand.h"
#ضمن "curlx/strdup.h"
#ضمن "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#تعریف_کړه NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#تعریف_کړه NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#تعریف_کړه NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#تعریف_کړه NTLMFLAG_REQUEST_موخه                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#إذا DEBUG_ME
/* unknown (1 << 3) */
#تعریف_کړه NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#تعریف_کړه NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#تعریف_کړه NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_LM_کلی                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#د_که_پای

#تعریف_کړه NTLMFLAG_NEGOTIATE_NTLM_کلی              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#إذا DEBUG_ME
/* unknown (1 << 10) */

#تعریف_کړه NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#د_که_پای

#تعریف_کړه NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#إذا DEBUG_ME
#تعریف_کړه NTLMFLAG_موخه_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#تعریف_کړه NTLMFLAG_موخه_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#تعریف_کړه NTLMFLAG_موخه_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#د_که_پای

#تعریف_کړه NTLMFLAG_NEGOTIATE_NTLM2_کلی             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#إذا DEBUG_ME
#تعریف_کړه NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#تعریف_کړه NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#تعریف_کړه NTLMFLAG_REQUEST_NONNT_SESSION_کلی       (1 << 22)
/* unknown purpose */
#د_که_پای

#تعریف_کړه NTLMFLAG_NEGOTIATE_موخه_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#إذا DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#تعریف_کړه NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_کلی_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#تعریف_کړه NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#تعریف_کړه DEBUG_بهر(x) x
ساكن فراغ ntlm_print_flags(FILE *handle, بینښه اوږد flags)
{
  إذا(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  إذا(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  إذا(flags & NTLMFLAG_REQUEST_موخه)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  إذا(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  إذا(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  إذا(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  إذا(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  إذا(flags & NTLMFLAG_NEGOTIATE_LM_کلی)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  إذا(flags & NTLMFLAG_NEGOTIATE_NTLM_کلی)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  إذا(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  إذا(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  إذا(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  إذا(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  إذا(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  إذا(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  إذا(flags & NTLMFLAG_موخه_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  إذا(flags & NTLMFLAG_موخه_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  إذا(flags & NTLMFLAG_موخه_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  إذا(flags & NTLMFLAG_NEGOTIATE_NTLM2_کلی)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  إذا(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  إذا(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  إذا(flags & NTLMFLAG_REQUEST_NONNT_SESSION_کلی)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  إذا(flags & NTLMFLAG_NEGOTIATE_موخه_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  إذا(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  إذا(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  إذا(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  إذا(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  إذا(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  إذا(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  إذا(flags & NTLMFLAG_NEGOTIATE_کلی_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  إذا(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

ساكن فراغ ntlm_print_hex(FILE *handle, ثابت حرف *buf, size_t len)
{
  ثابت حرف *p = buf;

  (فراغ)handle;

  curl_mfprintf(stderr, "0x");
  طالما(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (بینښه صحيح)*p++);
}
#وإلا
#تعریف_کړه DEBUG_بهر(x) Curl_nop_stmt
#د_که_پای

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
ساكن CURLcode ntlm_decode_type2_موخه(بنية Curl_easy *معلومات,
                                         ثابت بنية bufref *type2ref,
                                         بنية ntlmdata *ntlm)
{
  بینښه لنډ موخه_info_len = 0;
  بینښه صحيح موخه_info_دننه_واټن = 0;
  ثابت بینښه حرف *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  إذا(type2len >= 48) {
    موخه_info_len = Curl_read16_le(&type2[40]);
    موخه_info_دننه_واټن = Curl_read32_le(&type2[44]);
    إذا(موخه_info_len > 0) {
      إذا((موخه_info_دننه_واټن > type2len) ||
         (موخه_info_دننه_واټن + موخه_info_len) > type2len ||
         موخه_info_دننه_واټن < 48) {
        infof(معلومات, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        أرجع CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->موخه_info); /* replace any previous data */
      ntlm->موخه_info = curlx_memdup(&type2[موخه_info_دننه_واټن],
                                       موخه_info_len);
      إذا(!ntlm->موخه_info)
        أرجع CURLE_بهر_د_حافظه;
    }
  }

  ntlm->موخه_info_len = موخه_info_len;

  أرجع CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(فراغ)
{
  أرجع TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_پیغام(بنية Curl_easy *معلومات,
                                             ثابت بنية bufref *type2ref,
                                             بنية ntlmdata *ntlm)
{
  ساكن ثابت حرف type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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

  CURLcode پایله = CURLE_OK;
  ثابت بینښه حرف *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  إذا((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, حجم(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(معلومات, "NTLM handshake failure (bad type-2 message)");
    أرجع CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  حافظه_نقل(ntlm->nonce, &type2[24], 8);

  إذا(ntlm->flags & NTLMFLAG_NEGOTIATE_موخه_INFO) {
    پایله = ntlm_decode_type2_موخه(معلومات, type2ref, ntlm);
    إذا(پایله) {
      infof(معلومات, "NTLM handshake failure (bad type-2 message)");
      أرجع پایله;
    }
  }

  DEBUG_بهر({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (حرف *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", سرلیک);
  });

  أرجع پایله;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
ساكن فراغ unicodecpy(بینښه حرف *dest, ثابت حرف *src, size_t اوږدتیا)
{
  size_t i;
  لكل(i = 0; i < اوږدتیا; i++) {
    dest[2 * i] = (بینښه حرف)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_پیغام(بنية Curl_easy *معلومات,
                                             بنية Curl_creds *creds,
                                             ثابت حرف *default_service,
                                             ثابت حرف *host,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *بهر)
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
  size_t اندازه;

  حرف *ntlmbuf;
  ثابت حرف *hostname = "";          /*تشه*/
  ثابت حرف *domain = "";            /*تشه*/
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (فراغ)معلومات;
  (فراغ)creds;
  (فراغ)service;
  (فراغ)host;

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
                                      NTLMFLAG_REQUEST_موخه |
                                      NTLMFLAG_NEGOTIATE_NTLM_کلی |
                                      NTLMFLAG_NEGOTIATE_NTLM2_کلی |
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

  إذا(!ntlmbuf)
    أرجع CURLE_بهر_د_حافظه;

  /* Initial packet length */
  اندازه = 32 + hostlen + domlen;

  DEBUG_بهر({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_موخه |
                              NTLMFLAG_NEGOTIATE_NTLM_کلی |
                              NTLMFLAG_NEGOTIATE_NTLM2_کلی |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_موخه |
                  NTLMFLAG_NEGOTIATE_NTLM_کلی |
                  NTLMFLAG_NEGOTIATE_NTLM2_کلی |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_موخه |
                     NTLMFLAG_NEGOTIATE_NTLM_کلی |
                     NTLMFLAG_NEGOTIATE_NTLM2_کلی |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_سېټ(بهر, ntlmbuf, اندازه, curl_free);
  أرجع CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_پیغام(بنية Curl_easy *معلومات,
                                             بنية Curl_creds *creds,
                                             بنية ntlmdata *ntlm,
                                             بنية bufref *بهر)
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

  CURLcode پایله = CURLE_OK;
  size_t اندازه;
  بینښه حرف ntlmbuf[NTLM_BUFSIZE];
  بینښه صحيح lmrespoff;
  بینښه حرف lmresp[24]; /* fixed-size */
  بینښه صحيح ntrespoff;
  بینښه صحيح ntresplen = 24;
  بینښه حرف ntresp[24]; /* fixed-size */
  ثابت بینښه حرف *ptr_ntresp = &ntresp[0];
  بینښه حرف *ntlmv2resp = عدم;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  ساكن ثابت حرف host[] = "WORKSTATION";
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
  إذا(!user)
    user = strchr(userp, '/');

  إذا(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  وإلا
    user = userp;

  userlen = مزي_اوږدوالی(user);
  hostlen = حجم(host) - 1;

  إذا(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_کلی) {
    بینښه حرف ntbuffer[0x18];
    بینښه حرف entropy[8];
    بینښه حرف ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    پایله = Curl_rand(معلومات, entropy, 8);
    إذا(پایله)
      أرجع پایله;

    پایله = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    إذا(پایله)
      أرجع پایله;

    پایله = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    إذا(پایله)
      أرجع پایله;

    /* LMv2 response */
    پایله = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    إذا(پایله)
      أرجع پایله;

    /* NTLMv2 response */
    پایله = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    إذا(پایله)
      أرجع پایله;

    ptr_ntresp = ntlmv2resp;
  }
  وإلا {

    بینښه حرف ntbuffer[0x18];
    بینښه حرف lmbuffer[0x18];

    /* NTLM version 1 */

    پایله = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    إذا(پایله)
      أرجع پایله;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    پایله = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    إذا(پایله)
      أرجع پایله;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(بینښه صحيح)NTLMFLAG_NEGOTIATE_NTLM2_کلی;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  إذا(unicode) {
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
  اندازه = curl_msnprintf((حرف *)ntlmbuf, NTLM_BUFSIZE,
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

  DEBUGASSERT(اندازه == 64);
  DEBUGASSERT(اندازه == (size_t)lmrespoff);

  /* We append the binary hashes */
  إذا(اندازه < (NTLM_BUFSIZE - 0x18)) {
    حافظه_نقل(&ntlmbuf[اندازه], lmresp, 0x18);
    اندازه += 0x18;
  }

  DEBUG_بهر({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (حرف *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  إذا(ntresplen + اندازه > حجم(ntlmbuf)) {
    failf(معلومات, "incoming NTLM message too big");
    پایله = CURLE_TOO_LARGE;
    اذهب error;
  }
  DEBUGASSERT(اندازه == (size_t)ntrespoff);
  حافظه_نقل(&ntlmbuf[اندازه], ptr_ntresp, ntresplen);
  اندازه += ntresplen;

  DEBUG_بهر({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (حرف *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_بهر({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  إذا(اندازه + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(معلومات, "user + domain + hostname too big for NTLM");
    پایله = CURLE_TOO_LARGE;
    اذهب error;
  }

  DEBUGASSERT(اندازه == domoff);
  إذا(unicode)
    unicodecpy(&ntlmbuf[اندازه], domain, domlen / 2);
  وإلا
    حافظه_نقل(&ntlmbuf[اندازه], domain, domlen);

  اندازه += domlen;

  DEBUGASSERT(اندازه == useroff);
  إذا(unicode)
    unicodecpy(&ntlmbuf[اندازه], user, userlen / 2);
  وإلا
    حافظه_نقل(&ntlmbuf[اندازه], user, userlen);

  اندازه += userlen;

  DEBUGASSERT(اندازه == hostoff);
  إذا(unicode)
    unicodecpy(&ntlmbuf[اندازه], host, hostlen / 2);
  وإلا
    حافظه_نقل(&ntlmbuf[اندازه], host, hostlen);

  اندازه += hostlen;

  /* Return the binary blob. */
  پایله = Curl_bufref_memdup0(بهر, ntlmbuf, اندازه);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  أرجع پایله;
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
فراغ Curl_auth_cleanup_ntlm(بنية ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->موخه_info);

  /* Reset any variables */
  ntlm->موخه_info_len = 0;
}

#د_که_پای /* USE_NTLM && !USE_WINDOWS_SSPI */
