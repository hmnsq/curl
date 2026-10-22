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
#অন্তর্ভাব "curl_setup.h"

#যদি defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#define DEBUG_ME 0

#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curl_ntlm_core.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#define NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#define NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#define NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#define NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#define NTLMFLAG_REQUEST_TARGET                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#যদি DEBUG_ME
/* unknown (1 << 3) */
#define NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#define NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#define NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#define NTLMFLAG_NEGOTIATE_LM_চাবি                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#endif

#define NTLMFLAG_NEGOTIATE_NTLM_চাবি              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#যদি DEBUG_ME
/* unknown (1 << 10) */

#define NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#define NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#define NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#define NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#endif

#define NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#যদি DEBUG_ME
#define NTLMFLAG_TARGET_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#define NTLMFLAG_TARGET_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#define NTLMFLAG_TARGET_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#endif

#define NTLMFLAG_NEGOTIATE_NTLM2_চাবি             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#যদি DEBUG_ME
#define NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#define NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#define NTLMFLAG_REQUEST_NONNT_SESSION_চাবি       (1 << 22)
/* unknown purpose */
#endif

#define NTLMFLAG_NEGOTIATE_TARGET_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#যদি DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#define NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#define NTLMFLAG_NEGOTIATE_চাবি_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#define NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#define DEBUG_OUT(x) x
স্থির শূন্য ntlm_print_flags(FILE *handle, unsigned long flags)
{
  যদি(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  যদি(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  যদি(flags & NTLMFLAG_REQUEST_TARGET)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  যদি(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  যদি(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  যদি(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  যদি(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  যদি(flags & NTLMFLAG_NEGOTIATE_LM_চাবি)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  যদি(flags & NTLMFLAG_NEGOTIATE_NTLM_চাবি)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  যদি(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  যদি(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  যদি(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  যদি(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  যদি(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  যদি(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  যদি(flags & NTLMFLAG_TARGET_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  যদি(flags & NTLMFLAG_TARGET_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  যদি(flags & NTLMFLAG_TARGET_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  যদি(flags & NTLMFLAG_NEGOTIATE_NTLM2_চাবি)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  যদি(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  যদি(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  যদি(flags & NTLMFLAG_REQUEST_NONNT_SESSION_চাবি)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  যদি(flags & NTLMFLAG_NEGOTIATE_TARGET_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  যদি(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  যদি(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  যদি(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  যদি(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  যদি(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  যদি(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  যদি(flags & NTLMFLAG_NEGOTIATE_চাবি_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  যদি(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

স্থির শূন্য ntlm_print_hex(FILE *handle, ধ্রুব বর্ণ *buf, size_t len)
{
  ধ্রুব বর্ণ *p = buf;

  (শূন্য)handle;

  curl_mfprintf(stderr, "0x");
  যাবত্(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (unsigned পূর্ণাঙ্ক)*p++);
}
#অন্যথা
#define DEBUG_OUT(x) Curl_nop_stmt
#endif

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
স্থির CURLcode ntlm_decode_type2_target(সংরচনা Curl_easy *data,
                                         ধ্রুব সংরচনা bufref *type2ref,
                                         সংরচনা ntlmdata *ntlm)
{
  unsigned short target_info_len = 0;
  unsigned পূর্ণাঙ্ক target_info_offset = 0;
  ধ্রুব unsigned বর্ণ *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  যদি(type2len >= 48) {
    target_info_len = Curl_read16_le(&type2[40]);
    target_info_offset = Curl_read32_le(&type2[44]);
    যদি(target_info_len > 0) {
      যদি((target_info_offset > type2len) ||
         (target_info_offset + target_info_len) > type2len ||
         target_info_offset < 48) {
        infof(data, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->target_info); /* replace any previous data */
      ntlm->target_info = curlx_memdup(&type2[target_info_offset],
                                       target_info_len);
      যদি(!ntlm->target_info)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }

  ntlm->target_info_len = target_info_len;

  প্রত্যাগম CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(শূন্য)
{
  প্রত্যাগম TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_message(সংরচনা Curl_easy *data,
                                             ধ্রুব সংরচনা bufref *type2ref,
                                             সংরচনা ntlmdata *ntlm)
{
  স্থির ধ্রুব বর্ণ type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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

  CURLcode result = CURLE_OK;
  ধ্রুব unsigned বর্ণ *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  যদি((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, পরিমাণ(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(data, "NTLM handshake failure (bad type-2 message)");
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  memcpy(ntlm->nonce, &type2[24], 8);

  যদি(ntlm->flags & NTLMFLAG_NEGOTIATE_TARGET_INFO) {
    result = ntlm_decode_type2_target(data, type2ref, ntlm);
    যদি(result) {
      infof(data, "NTLM handshake failure (bad type-2 message)");
      প্রত্যাগম result;
    }
  }

  DEBUG_OUT({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (বর্ণ *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", header);
  });

  প্রত্যাগম result;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
স্থির শূন্য unicodecpy(unsigned বর্ণ *dest, ধ্রুব বর্ণ *src, size_t length)
{
  size_t i;
  প্রতি(i = 0; i < length; i++) {
    dest[2 * i] = (unsigned বর্ণ)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_message(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_creds *creds,
                                             ধ্রুব বর্ণ *default_service,
                                             ধ্রুব বর্ণ *host,
                                             সংরচনা ntlmdata *ntlm,
                                             সংরচনা bufref *out)
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

  ধ্রুব বর্ণ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t size;

  বর্ণ *ntlmbuf;
  ধ্রুব বর্ণ *hostname = "";          /* empty */
  ধ্রুব বর্ণ *domain = "";            /* empty */
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (শূন্য)data;
  (শূন্য)creds;
  (শূন্য)service;
  (শূন্য)host;

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
                                      NTLMFLAG_REQUEST_TARGET |
                                      NTLMFLAG_NEGOTIATE_NTLM_চাবি |
                                      NTLMFLAG_NEGOTIATE_NTLM2_চাবি |
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

  যদি(!ntlmbuf)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  /* Initial packet length */
  size = 32 + hostlen + domlen;

  DEBUG_OUT({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_TARGET |
                              NTLMFLAG_NEGOTIATE_NTLM_চাবি |
                              NTLMFLAG_NEGOTIATE_NTLM2_চাবি |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_TARGET |
                  NTLMFLAG_NEGOTIATE_NTLM_চাবি |
                  NTLMFLAG_NEGOTIATE_NTLM2_চাবি |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_TARGET |
                     NTLMFLAG_NEGOTIATE_NTLM_চাবি |
                     NTLMFLAG_NEGOTIATE_NTLM2_চাবি |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_set(out, ntlmbuf, size, curl_free);
  প্রত্যাগম CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_message(সংরচনা Curl_easy *data,
                                             সংরচনা Curl_creds *creds,
                                             সংরচনা ntlmdata *ntlm,
                                             সংরচনা bufref *out)
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

  CURLcode result = CURLE_OK;
  size_t size;
  unsigned বর্ণ ntlmbuf[NTLM_BUFSIZE];
  unsigned পূর্ণাঙ্ক lmrespoff;
  unsigned বর্ণ lmresp[24]; /* fixed-size */
  unsigned পূর্ণাঙ্ক ntrespoff;
  unsigned পূর্ণাঙ্ক ntresplen = 24;
  unsigned বর্ণ ntresp[24]; /* fixed-size */
  ধ্রুব unsigned বর্ণ *ptr_ntresp = &ntresp[0];
  unsigned বর্ণ *ntlmv2resp = রিক্ত;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  স্থির ধ্রুব বর্ণ host[] = "WORKSTATION";
  ধ্রুব বর্ণ *userp = Curl_creds_user(creds);
  ধ্রুব বর্ণ *passwdp = Curl_creds_passwd(creds);
  ধ্রুব বর্ণ *user;
  ধ্রুব বর্ণ *domain = "";
  size_t hostoff = 0;
  size_t useroff = 0;
  size_t domoff = 0;
  size_t hostlen = 0;
  size_t userlen = 0;
  size_t domlen = 0;

  memset(lmresp, 0, পরিমাণ(lmresp));
  memset(ntresp, 0, পরিমাণ(ntresp));
  user = strchr(userp, '\\');
  যদি(!user)
    user = strchr(userp, '/');

  যদি(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  অন্যথা
    user = userp;

  userlen = strlen(user);
  hostlen = পরিমাণ(host) - 1;

  যদি(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_চাবি) {
    unsigned বর্ণ ntbuffer[0x18];
    unsigned বর্ণ entropy[8];
    unsigned বর্ণ ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    result = Curl_rand(data, entropy, 8);
    যদি(result)
      প্রত্যাগম result;

    result = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    যদি(result)
      প্রত্যাগম result;

    result = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    যদি(result)
      প্রত্যাগম result;

    /* LMv2 response */
    result = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    যদি(result)
      প্রত্যাগম result;

    /* NTLMv2 response */
    result = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    যদি(result)
      প্রত্যাগম result;

    ptr_ntresp = ntlmv2resp;
  }
  অন্যথা {

    unsigned বর্ণ ntbuffer[0x18];
    unsigned বর্ণ lmbuffer[0x18];

    /* NTLM version 1 */

    result = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    যদি(result)
      প্রত্যাগম result;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    result = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    যদি(result)
      প্রত্যাগম result;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(unsigned পূর্ণাঙ্ক)NTLMFLAG_NEGOTIATE_NTLM2_চাবি;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  যদি(unicode) {
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
  size = curl_msnprintf((বর্ণ *)ntlmbuf, NTLM_BUFSIZE,
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

  DEBUGASSERT(size == 64);
  DEBUGASSERT(size == (size_t)lmrespoff);

  /* We append the binary hashes */
  যদি(size < (NTLM_BUFSIZE - 0x18)) {
    memcpy(&ntlmbuf[size], lmresp, 0x18);
    size += 0x18;
  }

  DEBUG_OUT({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (বর্ণ *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  যদি(ntresplen + size > পরিমাণ(ntlmbuf)) {
    failf(data, "incoming NTLM message too big");
    result = CURLE_TOO_LARGE;
    গচ্ছ error;
  }
  DEBUGASSERT(size == (size_t)ntrespoff);
  memcpy(&ntlmbuf[size], ptr_ntresp, ntresplen);
  size += ntresplen;

  DEBUG_OUT({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (বর্ণ *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_OUT({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  যদি(size + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(data, "user + domain + hostname too big for NTLM");
    result = CURLE_TOO_LARGE;
    গচ্ছ error;
  }

  DEBUGASSERT(size == domoff);
  যদি(unicode)
    unicodecpy(&ntlmbuf[size], domain, domlen / 2);
  অন্যথা
    memcpy(&ntlmbuf[size], domain, domlen);

  size += domlen;

  DEBUGASSERT(size == useroff);
  যদি(unicode)
    unicodecpy(&ntlmbuf[size], user, userlen / 2);
  অন্যথা
    memcpy(&ntlmbuf[size], user, userlen);

  size += userlen;

  DEBUGASSERT(size == hostoff);
  যদি(unicode)
    unicodecpy(&ntlmbuf[size], host, hostlen / 2);
  অন্যথা
    memcpy(&ntlmbuf[size], host, hostlen);

  size += hostlen;

  /* Return the binary blob. */
  result = Curl_bufref_memdup0(out, ntlmbuf, size);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  প্রত্যাগম result;
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
শূন্য Curl_auth_cleanup_ntlm(সংরচনা ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->target_info);

  /* Reset any variables */
  ntlm->target_info_len = 0;
}

#endif /* USE_NTLM && !USE_WINDOWS_SSPI */
