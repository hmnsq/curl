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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ME 0

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curl_ntlm_core.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#ವ್ಯಾಖ್ಯಾನಿಸು NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#ವ್ಯಾಖ್ಯಾನಿಸು NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_REQUEST_ಗುರಿ                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#ಆದರೆ DEBUG_ME
/* unknown (1 << 3) */
#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_LM_ಕೀಲಿ                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_NTLM_ಕೀಲಿ              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#ಆದರೆ DEBUG_ME
/* unknown (1 << 10) */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#ಆದರೆ DEBUG_ME
#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_ಗುರಿ_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_ಗುರಿ_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_ಗುರಿ_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#ಆದರೆ DEBUG_ME
#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_REQUEST_NONNT_SESSION_ಕೀಲಿ       (1 << 22)
/* unknown purpose */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_ಗುರಿ_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#ಆದರೆ DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_ಕೀಲಿ_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ಹೊರಹೋಗು(x) x
ಸ್ಥಿರ ಶೂನ್ಯ ntlm_print_flags(FILE *handle, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags)
{
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  ಆದರೆ(flags & NTLMFLAG_REQUEST_ಗುರಿ)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  ಆದರೆ(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_LM_ಕೀಲಿ)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_NTLM_ಕೀಲಿ)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  ಆದರೆ(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  ಆದರೆ(flags & NTLMFLAG_ಗುರಿ_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  ಆದರೆ(flags & NTLMFLAG_ಗುರಿ_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  ಆದರೆ(flags & NTLMFLAG_ಗುರಿ_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  ಆದರೆ(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  ಆದರೆ(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  ಆದರೆ(flags & NTLMFLAG_REQUEST_NONNT_SESSION_ಕೀಲಿ)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_ಗುರಿ_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  ಆದರೆ(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  ಆದರೆ(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  ಆದರೆ(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  ಆದರೆ(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  ಆದರೆ(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_ಕೀಲಿ_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  ಆದರೆ(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

ಸ್ಥಿರ ಶೂನ್ಯ ntlm_print_hex(FILE *handle, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = buf;

  (ಶೂನ್ಯ)handle;

  curl_mfprintf(stderr, "0x");
  ಆಗ(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)*p++);
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ಹೊರಹೋಗು(x) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ CURLcode ntlm_decode_type2_ಗುರಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *type2ref,
                                         ರಚನೆ ntlmdata *ntlm)
{
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಗುರಿ_info_len = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗುರಿ_info_ಸ್ಥಾನಾಂತರ = 0;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ಆದರೆ(type2len >= 48) {
    ಗುರಿ_info_len = Curl_read16_le(&type2[40]);
    ಗುರಿ_info_ಸ್ಥಾನಾಂತರ = Curl_read32_le(&type2[44]);
    ಆದರೆ(ಗುರಿ_info_len > 0) {
      ಆದರೆ((ಗುರಿ_info_ಸ್ಥಾನಾಂತರ > type2len) ||
         (ಗುರಿ_info_ಸ್ಥಾನಾಂತರ + ಗುರಿ_info_len) > type2len ||
         ಗುರಿ_info_ಸ್ಥಾನಾಂತರ < 48) {
        infof(ದತ್ತಾಂಶ, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->ಗುರಿ_info); /* replace any previous data */
      ntlm->ಗುರಿ_info = curlx_memdup(&type2[ಗುರಿ_info_ಸ್ಥಾನಾಂತರ],
                                       ಗುರಿ_info_len);
      ಆದರೆ(!ntlm->ಗುರಿ_info)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ntlm->ಗುರಿ_info_len = ಗುರಿ_info_len;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *type2ref,
                                             ರಚನೆ ntlmdata *ntlm)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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

  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  ಆದರೆ((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, ಗಾತ್ರ(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(ದತ್ತಾಂಶ, "NTLM handshake failure (bad type-2 message)");
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  ಸ್ಮರಣೆ_ನಕಲು(ntlm->nonce, &type2[24], 8);

  ಆದರೆ(ntlm->flags & NTLMFLAG_NEGOTIATE_ಗುರಿ_INFO) {
    ಫಲಿತಾಂಶ = ntlm_decode_type2_ಗುರಿ(ದತ್ತಾಂಶ, type2ref, ntlm);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      infof(ದತ್ತಾಂಶ, "NTLM handshake failure (bad type-2 message)");
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  DEBUG_ಹೊರಹೋಗು({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (ಅಕ್ಷರ *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", ಶೀರ್ಷಿಕೆ);
  });

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
ಸ್ಥಿರ ಶೂನ್ಯ unicodecpy(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t ಉದ್ದಳತೆ)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < ಉದ್ದಳತೆ; i++) {
    dest[2 * i] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_creds *creds,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_service,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                             ರಚನೆ ntlmdata *ntlm,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು)
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

  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t ಗಾತ್ರ_2;

  ಅಕ್ಷರ *ntlmbuf;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = "";          /*ಖಾಲಿಗಳು*/
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain = "";            /*ಖಾಲಿಗಳು*/
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)creds;
  (ಶೂನ್ಯ)service;
  (ಶೂನ್ಯ)host;

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
                                      NTLMFLAG_REQUEST_ಗುರಿ |
                                      NTLMFLAG_NEGOTIATE_NTLM_ಕೀಲಿ |
                                      NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ |
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

  ಆದರೆ(!ntlmbuf)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Initial packet length */
  ಗಾತ್ರ_2 = 32 + hostlen + domlen;

  DEBUG_ಹೊರಹೋಗು({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_ಗುರಿ |
                              NTLMFLAG_NEGOTIATE_NTLM_ಕೀಲಿ |
                              NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_ಗುರಿ |
                  NTLMFLAG_NEGOTIATE_NTLM_ಕೀಲಿ |
                  NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_ಗುರಿ |
                     NTLMFLAG_NEGOTIATE_NTLM_ಕೀಲಿ |
                     NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ntlmbuf, ಗಾತ್ರ_2, curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ Curl_creds *creds,
                                             ರಚನೆ ntlmdata *ntlm,
                                             ರಚನೆ bufref *ಹೊರಹೋಗು)
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

  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t ಗಾತ್ರ_2;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ntlmbuf[NTLM_BUFSIZE];
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ lmrespoff;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ lmresp[24]; /* fixed-size */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ntrespoff;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ntresplen = 24;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ntresp[24]; /* fixed-size */
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ptr_ntresp = &ntresp[0];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2resp = ಖಾಲಿ;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ host[] = "WORKSTATION";
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *userp = Curl_creds_user(creds);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwdp = Curl_creds_passwd(creds);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain = "";
  size_t hostoff = 0;
  size_t useroff = 0;
  size_t domoff = 0;
  size_t hostlen = 0;
  size_t userlen = 0;
  size_t domlen = 0;

  memset(lmresp, 0, ಗಾತ್ರ(lmresp));
  memset(ntresp, 0, ಗಾತ್ರ(ntresp));
  user = strchr(userp, '\\');
  ಆದರೆ(!user)
    user = strchr(userp, '/');

  ಆದರೆ(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    user = userp;

  userlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(user);
  hostlen = ಗಾತ್ರ(host) - 1;

  ಆದರೆ(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ntbuffer[0x18];
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ entropy[8];
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, entropy, 8);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* LMv2 response */
    ಫಲಿತಾಂಶ = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* NTLMv2 response */
    ಫಲಿತಾಂಶ = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ptr_ntresp = ntlmv2resp;
  }
  ಇಲ್ಲದಿದ್ದರೆ {

    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ntbuffer[0x18];
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ lmbuffer[0x18];

    /* NTLM version 1 */

    ಫಲಿತಾಂಶ = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    ಫಲಿತಾಂಶ = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)NTLMFLAG_NEGOTIATE_NTLM2_ಕೀಲಿ;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  ಆದರೆ(unicode) {
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
  ಗಾತ್ರ_2 = curl_msnprintf((ಅಕ್ಷರ *)ntlmbuf, NTLM_BUFSIZE,
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

  DEBUGASSERT(ಗಾತ್ರ_2 == 64);
  DEBUGASSERT(ಗಾತ್ರ_2 == (size_t)lmrespoff);

  /* We append the binary hashes */
  ಆದರೆ(ಗಾತ್ರ_2 < (NTLM_BUFSIZE - 0x18)) {
    ಸ್ಮರಣೆ_ನಕಲು(&ntlmbuf[ಗಾತ್ರ_2], lmresp, 0x18);
    ಗಾತ್ರ_2 += 0x18;
  }

  DEBUG_ಹೊರಹೋಗು({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (ಅಕ್ಷರ *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  ಆದರೆ(ntresplen + ಗಾತ್ರ_2 > ಗಾತ್ರ(ntlmbuf)) {
    failf(ದತ್ತಾಂಶ, "incoming NTLM message too big");
    ಫಲಿತಾಂಶ = CURLE_TOO_LARGE;
    ಹೋಗು error;
  }
  DEBUGASSERT(ಗಾತ್ರ_2 == (size_t)ntrespoff);
  ಸ್ಮರಣೆ_ನಕಲು(&ntlmbuf[ಗಾತ್ರ_2], ptr_ntresp, ntresplen);
  ಗಾತ್ರ_2 += ntresplen;

  DEBUG_ಹೊರಹೋಗು({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (ಅಕ್ಷರ *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_ಹೊರಹೋಗು({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  ಆದರೆ(ಗಾತ್ರ_2 + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(ದತ್ತಾಂಶ, "user + domain + hostname too big for NTLM");
    ಫಲಿತಾಂಶ = CURLE_TOO_LARGE;
    ಹೋಗು error;
  }

  DEBUGASSERT(ಗಾತ್ರ_2 == domoff);
  ಆದರೆ(unicode)
    unicodecpy(&ntlmbuf[ಗಾತ್ರ_2], domain, domlen / 2);
  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಮರಣೆ_ನಕಲು(&ntlmbuf[ಗಾತ್ರ_2], domain, domlen);

  ಗಾತ್ರ_2 += domlen;

  DEBUGASSERT(ಗಾತ್ರ_2 == useroff);
  ಆದರೆ(unicode)
    unicodecpy(&ntlmbuf[ಗಾತ್ರ_2], user, userlen / 2);
  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಮರಣೆ_ನಕಲು(&ntlmbuf[ಗಾತ್ರ_2], user, userlen);

  ಗಾತ್ರ_2 += userlen;

  DEBUGASSERT(ಗಾತ್ರ_2 == hostoff);
  ಆದರೆ(unicode)
    unicodecpy(&ntlmbuf[ಗಾತ್ರ_2], host, hostlen / 2);
  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಮರಣೆ_ನಕಲು(&ntlmbuf[ಗಾತ್ರ_2], host, hostlen);

  ಗಾತ್ರ_2 += hostlen;

  /* Return the binary blob. */
  ಫಲಿತಾಂಶ = Curl_bufref_memdup0(ಹೊರಹೋಗು, ntlmbuf, ಗಾತ್ರ_2);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಶೂನ್ಯ Curl_auth_cleanup_ntlm(ರಚನೆ ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->ಗುರಿ_info);

  /* Reset any variables */
  ntlm->ಗುರಿ_info_len = 0;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_NTLM && !USE_WINDOWS_SSPI */
