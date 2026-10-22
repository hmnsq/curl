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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ME 0

#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ntlm_core.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_REQUEST_ਨਿਸ਼ਾਨਾ                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#ਜੇ DEBUG_ME
/* unknown (1 << 3) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_LM_ਕੁੰਜੀ                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_NTLM_ਕੁੰਜੀ              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#ਜੇ DEBUG_ME
/* unknown (1 << 10) */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#ਜੇ DEBUG_ME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_ਨਿਸ਼ਾਨਾ_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_ਨਿਸ਼ਾਨਾ_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_ਨਿਸ਼ਾਨਾ_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#ਜੇ DEBUG_ME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_REQUEST_NONNT_SESSION_ਕੁੰਜੀ       (1 << 22)
/* unknown purpose */
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_ਨਿਸ਼ਾਨਾ_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#ਜੇ DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_ਕੁੰਜੀ_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ਬਾਹਰਲਾ(x) x
ਸਥਿਰ ਖਾਲੀ ntlm_print_flags(FILE *handle, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags)
{
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  ਜੇ(flags & NTLMFLAG_REQUEST_ਨਿਸ਼ਾਨਾ)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  ਜੇ(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_LM_ਕੁੰਜੀ)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_NTLM_ਕੁੰਜੀ)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  ਜੇ(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  ਜੇ(flags & NTLMFLAG_ਨਿਸ਼ਾਨਾ_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  ਜੇ(flags & NTLMFLAG_ਨਿਸ਼ਾਨਾ_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  ਜੇ(flags & NTLMFLAG_ਨਿਸ਼ਾਨਾ_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  ਜੇ(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  ਜੇ(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  ਜੇ(flags & NTLMFLAG_REQUEST_NONNT_SESSION_ਕੁੰਜੀ)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_ਨਿਸ਼ਾਨਾ_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  ਜੇ(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  ਜੇ(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  ਜੇ(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  ਜੇ(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  ਜੇ(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_ਕੁੰਜੀ_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  ਜੇ(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

ਸਥਿਰ ਖਾਲੀ ntlm_print_hex(FILE *handle, ਅਚਲ ਅੱਖਰ *buf, size_t len)
{
  ਅਚਲ ਅੱਖਰ *p = buf;

  (ਖਾਲੀ)handle;

  curl_mfprintf(stderr, "0x");
  ਜਦੋਂ(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)*p++);
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ਬਾਹਰਲਾ(x) Curl_nop_stmt
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ CURLcode ntlm_decode_type2_ਨਿਸ਼ਾਨਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਅਚਲ ਬਣਤਰ bufref *type2ref,
                                         ਬਣਤਰ ntlmdata *ntlm)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਨਿਸ਼ਾਨਾ_info_len = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਿਸ਼ਾਨਾ_info_ਵਿੱਥ = 0;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ਜੇ(type2len >= 48) {
    ਨਿਸ਼ਾਨਾ_info_len = Curl_read16_le(&type2[40]);
    ਨਿਸ਼ਾਨਾ_info_ਵਿੱਥ = Curl_read32_le(&type2[44]);
    ਜੇ(ਨਿਸ਼ਾਨਾ_info_len > 0) {
      ਜੇ((ਨਿਸ਼ਾਨਾ_info_ਵਿੱਥ > type2len) ||
         (ਨਿਸ਼ਾਨਾ_info_ਵਿੱਥ + ਨਿਸ਼ਾਨਾ_info_len) > type2len ||
         ਨਿਸ਼ਾਨਾ_info_ਵਿੱਥ < 48) {
        infof(ਅੰਕੜੇ, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->ਨਿਸ਼ਾਨਾ_info); /* replace any previous data */
      ntlm->ਨਿਸ਼ਾਨਾ_info = curlx_memdup(&type2[ਨਿਸ਼ਾਨਾ_info_ਵਿੱਥ],
                                       ਨਿਸ਼ਾਨਾ_info_len);
      ਜੇ(!ntlm->ਨਿਸ਼ਾਨਾ_info)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ntlm->ਨਿਸ਼ਾਨਾ_info_len = ਨਿਸ਼ਾਨਾ_info_len;

  ਵਾਪਸ CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(ਖਾਲੀ)
{
  ਵਾਪਸ TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ bufref *type2ref,
                                             ਬਣਤਰ ntlmdata *ntlm)
{
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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

  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  ਜੇ((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, ਆਕਾਰ(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(ਅੰਕੜੇ, "NTLM handshake failure (bad type-2 message)");
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ntlm->nonce, &type2[24], 8);

  ਜੇ(ntlm->flags & NTLMFLAG_NEGOTIATE_ਨਿਸ਼ਾਨਾ_INFO) {
    ਨਤੀਜਾ = ntlm_decode_type2_ਨਿਸ਼ਾਨਾ(ਅੰਕੜੇ, type2ref, ntlm);
    ਜੇ(ਨਤੀਜਾ) {
      infof(ਅੰਕੜੇ, "NTLM handshake failure (bad type-2 message)");
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  DEBUG_ਬਾਹਰਲਾ({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (ਅੱਖਰ *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", ਸਿਰਲੇਖ);
  });

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
ਸਥਿਰ ਖਾਲੀ unicodecpy(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src, size_t ਲੰਮਾਈ)
{
  size_t i;
  ਲਈ(i = 0; i < ਲੰਮਾਈ; i++) {
    dest[2 * i] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਅਚਲ ਅੱਖਰ *default_service,
                                             ਅਚਲ ਅੱਖਰ *host,
                                             ਬਣਤਰ ntlmdata *ntlm,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
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

  ਅਚਲ ਅੱਖਰ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t ਮਾਪ;

  ਅੱਖਰ *ntlmbuf;
  ਅਚਲ ਅੱਖਰ *hostname = "";          /*ਖਾਲੀ_ਥਾਵਾਂ*/
  ਅਚਲ ਅੱਖਰ *domain = "";            /*ਖਾਲੀ_ਥਾਵਾਂ*/
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)creds;
  (ਖਾਲੀ)service;
  (ਖਾਲੀ)host;

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
                                      NTLMFLAG_REQUEST_ਨਿਸ਼ਾਨਾ |
                                      NTLMFLAG_NEGOTIATE_NTLM_ਕੁੰਜੀ |
                                      NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ |
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

  ਜੇ(!ntlmbuf)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Initial packet length */
  ਮਾਪ = 32 + hostlen + domlen;

  DEBUG_ਬਾਹਰਲਾ({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_ਨਿਸ਼ਾਨਾ |
                              NTLMFLAG_NEGOTIATE_NTLM_ਕੁੰਜੀ |
                              NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_ਨਿਸ਼ਾਨਾ |
                  NTLMFLAG_NEGOTIATE_NTLM_ਕੁੰਜੀ |
                  NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_ਨਿਸ਼ਾਨਾ |
                     NTLMFLAG_NEGOTIATE_NTLM_ਕੁੰਜੀ |
                     NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ntlmbuf, ਮਾਪ, curl_free);
  ਵਾਪਸ CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ Curl_creds *creds,
                                             ਬਣਤਰ ntlmdata *ntlm,
                                             ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
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

  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t ਮਾਪ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ntlmbuf[NTLM_BUFSIZE];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ lmrespoff;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ lmresp[24]; /* fixed-size */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ntrespoff;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ntresplen = 24;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ntresp[24]; /* fixed-size */
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ptr_ntresp = &ntresp[0];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2resp = ਨਿਰਾ;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ host[] = "WORKSTATION";
  ਅਚਲ ਅੱਖਰ *userp = Curl_creds_user(creds);
  ਅਚਲ ਅੱਖਰ *passwdp = Curl_creds_passwd(creds);
  ਅਚਲ ਅੱਖਰ *user;
  ਅਚਲ ਅੱਖਰ *domain = "";
  size_t hostoff = 0;
  size_t useroff = 0;
  size_t domoff = 0;
  size_t hostlen = 0;
  size_t userlen = 0;
  size_t domlen = 0;

  memset(lmresp, 0, ਆਕਾਰ(lmresp));
  memset(ntresp, 0, ਆਕਾਰ(ntresp));
  user = strchr(userp, '\\');
  ਜੇ(!user)
    user = strchr(userp, '/');

  ਜੇ(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  ਨਹੀਂਤਾਂ
    user = userp;

  userlen = ਲੜੀ_ਲੰਬਾਈ(user);
  hostlen = ਆਕਾਰ(host) - 1;

  ਜੇ(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ntbuffer[0x18];
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ entropy[8];
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, entropy, 8);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਨਤੀਜਾ = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਨਤੀਜਾ = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* LMv2 response */
    ਨਤੀਜਾ = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* NTLMv2 response */
    ਨਤੀਜਾ = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ptr_ntresp = ntlmv2resp;
  }
  ਨਹੀਂਤਾਂ {

    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ntbuffer[0x18];
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ lmbuffer[0x18];

    /* NTLM version 1 */

    ਨਤੀਜਾ = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    ਨਤੀਜਾ = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)NTLMFLAG_NEGOTIATE_NTLM2_ਕੁੰਜੀ;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  ਜੇ(unicode) {
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
  ਮਾਪ = curl_msnprintf((ਅੱਖਰ *)ntlmbuf, NTLM_BUFSIZE,
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

  DEBUGASSERT(ਮਾਪ == 64);
  DEBUGASSERT(ਮਾਪ == (size_t)lmrespoff);

  /* We append the binary hashes */
  ਜੇ(ਮਾਪ < (NTLM_BUFSIZE - 0x18)) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ntlmbuf[ਮਾਪ], lmresp, 0x18);
    ਮਾਪ += 0x18;
  }

  DEBUG_ਬਾਹਰਲਾ({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (ਅੱਖਰ *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  ਜੇ(ntresplen + ਮਾਪ > ਆਕਾਰ(ntlmbuf)) {
    failf(ਅੰਕੜੇ, "incoming NTLM message too big");
    ਨਤੀਜਾ = CURLE_TOO_LARGE;
    ਜਾਓ error;
  }
  DEBUGASSERT(ਮਾਪ == (size_t)ntrespoff);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ntlmbuf[ਮਾਪ], ptr_ntresp, ntresplen);
  ਮਾਪ += ntresplen;

  DEBUG_ਬਾਹਰਲਾ({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (ਅੱਖਰ *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_ਬਾਹਰਲਾ({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  ਜੇ(ਮਾਪ + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(ਅੰਕੜੇ, "user + domain + hostname too big for NTLM");
    ਨਤੀਜਾ = CURLE_TOO_LARGE;
    ਜਾਓ error;
  }

  DEBUGASSERT(ਮਾਪ == domoff);
  ਜੇ(unicode)
    unicodecpy(&ntlmbuf[ਮਾਪ], domain, domlen / 2);
  ਨਹੀਂਤਾਂ
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ntlmbuf[ਮਾਪ], domain, domlen);

  ਮਾਪ += domlen;

  DEBUGASSERT(ਮਾਪ == useroff);
  ਜੇ(unicode)
    unicodecpy(&ntlmbuf[ਮਾਪ], user, userlen / 2);
  ਨਹੀਂਤਾਂ
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ntlmbuf[ਮਾਪ], user, userlen);

  ਮਾਪ += userlen;

  DEBUGASSERT(ਮਾਪ == hostoff);
  ਜੇ(unicode)
    unicodecpy(&ntlmbuf[ਮਾਪ], host, hostlen / 2);
  ਨਹੀਂਤਾਂ
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ntlmbuf[ਮਾਪ], host, hostlen);

  ਮਾਪ += hostlen;

  /* Return the binary blob. */
  ਨਤੀਜਾ = Curl_bufref_memdup0(ਬਾਹਰਲਾ, ntlmbuf, ਮਾਪ);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਖਾਲੀ Curl_auth_cleanup_ntlm(ਬਣਤਰ ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->ਨਿਸ਼ਾਨਾ_info);

  /* Reset any variables */
  ntlm->ਨਿਸ਼ਾਨਾ_info_len = 0;
}

#ਜੇ_ਅੰਤ /* USE_NTLM && !USE_WINDOWS_SSPI */
