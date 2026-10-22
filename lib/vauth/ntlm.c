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
#अन्तर्भाव "curl_setup.h"

#यदि defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#परिभाषय DEBUG_ME 0

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curl_ntlm_core.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#परिभाषय NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#परिभाषय NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#परिभाषय NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#परिभाषय NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#परिभाषय NTLMFLAG_REQUEST_TARGET                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#यदि DEBUG_ME
/* unknown (1 << 3) */
#परिभाषय NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#परिभाषय NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#परिभाषय NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#परिभाषय NTLMFLAG_NEGOTIATE_LM_कुञ्चिका                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#यदि_अन्तः

#परिभाषय NTLMFLAG_NEGOTIATE_NTLM_कुञ्चिका              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#यदि DEBUG_ME
/* unknown (1 << 10) */

#परिभाषय NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#परिभाषय NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#परिभाषय NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#परिभाषय NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#यदि_अन्तः

#परिभाषय NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#यदि DEBUG_ME
#परिभाषय NTLMFLAG_TARGET_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#परिभाषय NTLMFLAG_TARGET_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#परिभाषय NTLMFLAG_TARGET_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#यदि_अन्तः

#परिभाषय NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#यदि DEBUG_ME
#परिभाषय NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#परिभाषय NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#परिभाषय NTLMFLAG_REQUEST_NONNT_SESSION_कुञ्चिका       (1 << 22)
/* unknown purpose */
#यदि_अन्तः

#परिभाषय NTLMFLAG_NEGOTIATE_TARGET_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#यदि DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#परिभाषय NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#परिभाषय NTLMFLAG_NEGOTIATE_कुञ्चिका_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#परिभाषय NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#परिभाषय DEBUG_बहिर्गमनम्(x) x
स्थिर शून्य ntlm_print_flags(FILE *handle, अचिह्नित दीर्घ flags)
{
  यदि(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  यदि(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  यदि(flags & NTLMFLAG_REQUEST_TARGET)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  यदि(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  यदि(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  यदि(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  यदि(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  यदि(flags & NTLMFLAG_NEGOTIATE_LM_कुञ्चिका)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  यदि(flags & NTLMFLAG_NEGOTIATE_NTLM_कुञ्चिका)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  यदि(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  यदि(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  यदि(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  यदि(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  यदि(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  यदि(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  यदि(flags & NTLMFLAG_TARGET_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  यदि(flags & NTLMFLAG_TARGET_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  यदि(flags & NTLMFLAG_TARGET_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  यदि(flags & NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  यदि(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  यदि(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  यदि(flags & NTLMFLAG_REQUEST_NONNT_SESSION_कुञ्चिका)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  यदि(flags & NTLMFLAG_NEGOTIATE_TARGET_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  यदि(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  यदि(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  यदि(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  यदि(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  यदि(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  यदि(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  यदि(flags & NTLMFLAG_NEGOTIATE_कुञ्चिका_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  यदि(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

स्थिर शून्य ntlm_print_hex(FILE *handle, ध्रुव वर्ण *buf, size_t len)
{
  ध्रुव वर्ण *p = buf;

  (शून्य)handle;

  curl_mfprintf(stderr, "0x");
  यावत्(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (अचिह्नित पूर्णाङ्क)*p++);
}
#अन्यथा
#परिभाषय DEBUG_बहिर्गमनम्(x) Curl_nop_stmt
#यदि_अन्तः

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
स्थिर CURLcode ntlm_decode_type2_target(संरचना Curl_easy *दत्त,
                                         ध्रुव संरचना bufref *type2ref,
                                         संरचना ntlmdata *ntlm)
{
  अचिह्नित ह्रस्व target_info_len = 0;
  अचिह्नित पूर्णाङ्क target_info_विस्थापनम् = 0;
  ध्रुव अचिह्नित वर्ण *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  यदि(type2len >= 48) {
    target_info_len = Curl_read16_le(&type2[40]);
    target_info_विस्थापनम् = Curl_read32_le(&type2[44]);
    यदि(target_info_len > 0) {
      यदि((target_info_विस्थापनम् > type2len) ||
         (target_info_विस्थापनम् + target_info_len) > type2len ||
         target_info_विस्थापनम् < 48) {
        infof(दत्त, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->target_info); /* replace any previous data */
      ntlm->target_info = curlx_memdup(&type2[target_info_विस्थापनम्],
                                       target_info_len);
      यदि(!ntlm->target_info)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  ntlm->target_info_len = target_info_len;

  प्रत्यागम CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(शून्य)
{
  प्रत्यागम TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_सन्देशः(संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना bufref *type2ref,
                                             संरचना ntlmdata *ntlm)
{
  स्थिर ध्रुव वर्ण type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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

  CURLcode परिणाम = CURLE_OK;
  ध्रुव अचिह्नित वर्ण *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  यदि((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, परिमाण(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(दत्त, "NTLM handshake failure (bad type-2 message)");
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  स्मृतिं_प्रतिलिख(ntlm->nonce, &type2[24], 8);

  यदि(ntlm->flags & NTLMFLAG_NEGOTIATE_TARGET_INFO) {
    परिणाम = ntlm_decode_type2_target(दत्त, type2ref, ntlm);
    यदि(परिणाम) {
      infof(दत्त, "NTLM handshake failure (bad type-2 message)");
      प्रत्यागम परिणाम;
    }
  }

  DEBUG_बहिर्गमनम्({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (वर्ण *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", शीर्षकम्);
  });

  प्रत्यागम परिणाम;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
स्थिर शून्य unicodecpy(अचिह्नित वर्ण *dest, ध्रुव वर्ण *src, size_t दीर्घता)
{
  size_t i;
  प्रति(i = 0; i < दीर्घता; i++) {
    dest[2 * i] = (अचिह्नित वर्ण)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_सन्देशः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_creds *creds,
                                             ध्रुव वर्ण *default_service,
                                             ध्रुव वर्ण *host,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *बहिर्गमनम्)
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

  ध्रुव वर्ण *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t आकार;

  वर्ण *ntlmbuf;
  ध्रुव वर्ण *hostname = "";          /*रिक्तानि*/
  ध्रुव वर्ण *domain = "";            /*रिक्तानि*/
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (शून्य)दत्त;
  (शून्य)creds;
  (शून्य)service;
  (शून्य)host;

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
                                      NTLMFLAG_NEGOTIATE_NTLM_कुञ्चिका |
                                      NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका |
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

  यदि(!ntlmbuf)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Initial packet length */
  आकार = 32 + hostlen + domlen;

  DEBUG_बहिर्गमनम्({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_TARGET |
                              NTLMFLAG_NEGOTIATE_NTLM_कुञ्चिका |
                              NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_TARGET |
                  NTLMFLAG_NEGOTIATE_NTLM_कुञ्चिका |
                  NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_TARGET |
                     NTLMFLAG_NEGOTIATE_NTLM_कुञ्चिका |
                     NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_समुच्चयः(बहिर्गमनम्, ntlmbuf, आकार, curl_free);
  प्रत्यागम CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_सन्देशः(संरचना Curl_easy *दत्त,
                                             संरचना Curl_creds *creds,
                                             संरचना ntlmdata *ntlm,
                                             संरचना bufref *बहिर्गमनम्)
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

  CURLcode परिणाम = CURLE_OK;
  size_t आकार;
  अचिह्नित वर्ण ntlmbuf[NTLM_BUFSIZE];
  अचिह्नित पूर्णाङ्क lmrespoff;
  अचिह्नित वर्ण lmresp[24]; /* fixed-size */
  अचिह्नित पूर्णाङ्क ntrespoff;
  अचिह्नित पूर्णाङ्क ntresplen = 24;
  अचिह्नित वर्ण ntresp[24]; /* fixed-size */
  ध्रुव अचिह्नित वर्ण *ptr_ntresp = &ntresp[0];
  अचिह्नित वर्ण *ntlmv2resp = रिक्त;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  स्थिर ध्रुव वर्ण host[] = "WORKSTATION";
  ध्रुव वर्ण *userp = Curl_creds_user(creds);
  ध्रुव वर्ण *passwdp = Curl_creds_passwd(creds);
  ध्रुव वर्ण *user;
  ध्रुव वर्ण *domain = "";
  size_t hostoff = 0;
  size_t useroff = 0;
  size_t domoff = 0;
  size_t hostlen = 0;
  size_t userlen = 0;
  size_t domlen = 0;

  स्मृतिं_पूरय(lmresp, 0, परिमाण(lmresp));
  स्मृतिं_पूरय(ntresp, 0, परिमाण(ntresp));
  user = strchr(userp, '\\');
  यदि(!user)
    user = strchr(userp, '/');

  यदि(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  अन्यथा
    user = userp;

  userlen = वाक्यदैर्घ्यम्(user);
  hostlen = परिमाण(host) - 1;

  यदि(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका) {
    अचिह्नित वर्ण ntbuffer[0x18];
    अचिह्नित वर्ण entropy[8];
    अचिह्नित वर्ण ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    परिणाम = Curl_rand(दत्त, entropy, 8);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    परिणाम = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    परिणाम = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* LMv2 response */
    परिणाम = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* NTLMv2 response */
    परिणाम = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    ptr_ntresp = ntlmv2resp;
  }
  अन्यथा {

    अचिह्नित वर्ण ntbuffer[0x18];
    अचिह्नित वर्ण lmbuffer[0x18];

    /* NTLM version 1 */

    परिणाम = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    परिणाम = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(अचिह्नित पूर्णाङ्क)NTLMFLAG_NEGOTIATE_NTLM2_कुञ्चिका;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  यदि(unicode) {
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
  आकार = curl_msnprintf((वर्ण *)ntlmbuf, NTLM_BUFSIZE,
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

  DEBUGASSERT(आकार == 64);
  DEBUGASSERT(आकार == (size_t)lmrespoff);

  /* We append the binary hashes */
  यदि(आकार < (NTLM_BUFSIZE - 0x18)) {
    स्मृतिं_प्रतिलिख(&ntlmbuf[आकार], lmresp, 0x18);
    आकार += 0x18;
  }

  DEBUG_बहिर्गमनम्({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (वर्ण *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  यदि(ntresplen + आकार > परिमाण(ntlmbuf)) {
    failf(दत्त, "incoming NTLM message too big");
    परिणाम = CURLE_TOO_LARGE;
    गच्छ error;
  }
  DEBUGASSERT(आकार == (size_t)ntrespoff);
  स्मृतिं_प्रतिलिख(&ntlmbuf[आकार], ptr_ntresp, ntresplen);
  आकार += ntresplen;

  DEBUG_बहिर्गमनम्({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (वर्ण *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_बहिर्गमनम्({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  यदि(आकार + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(दत्त, "user + domain + hostname too big for NTLM");
    परिणाम = CURLE_TOO_LARGE;
    गच्छ error;
  }

  DEBUGASSERT(आकार == domoff);
  यदि(unicode)
    unicodecpy(&ntlmbuf[आकार], domain, domlen / 2);
  अन्यथा
    स्मृतिं_प्रतिलिख(&ntlmbuf[आकार], domain, domlen);

  आकार += domlen;

  DEBUGASSERT(आकार == useroff);
  यदि(unicode)
    unicodecpy(&ntlmbuf[आकार], user, userlen / 2);
  अन्यथा
    स्मृतिं_प्रतिलिख(&ntlmbuf[आकार], user, userlen);

  आकार += userlen;

  DEBUGASSERT(आकार == hostoff);
  यदि(unicode)
    unicodecpy(&ntlmbuf[आकार], host, hostlen / 2);
  अन्यथा
    स्मृतिं_प्रतिलिख(&ntlmbuf[आकार], host, hostlen);

  आकार += hostlen;

  /* Return the binary blob. */
  परिणाम = Curl_bufref_memdup0(बहिर्गमनम्, ntlmbuf, आकार);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  प्रत्यागम परिणाम;
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
शून्य Curl_auth_cleanup_ntlm(संरचना ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->target_info);

  /* Reset any variables */
  ntlm->target_info_len = 0;
}

#यदि_अन्तः /* USE_NTLM && !USE_WINDOWS_SSPI */
