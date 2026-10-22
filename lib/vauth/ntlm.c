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
#समावेश "curl_setup.h"

#अगर defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#घोषणा DEBUG_ME 0

#समावेश "vauth/vauth.h"
#समावेश "curl_trc.h"
#समावेश "curl_ntlm_core.h"
#समावेश "rand.h"
#समावेश "curlx/strdup.h"
#समावेश "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#घोषणा NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#घोषणा NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#घोषणा NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#घोषणा NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#घोषणा NTLMFLAG_REQUEST_TARGET                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#अगर DEBUG_ME
/* unknown (1 << 3) */
#घोषणा NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#घोषणा NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#घोषणा NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#घोषणा NTLMFLAG_NEGOTIATE_LM_कुंजी                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#पूर्ण_अगर

#घोषणा NTLMFLAG_NEGOTIATE_NTLM_कुंजी              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#अगर DEBUG_ME
/* unknown (1 << 10) */

#घोषणा NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#घोषणा NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#घोषणा NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#घोषणा NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#पूर्ण_अगर

#घोषणा NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#अगर DEBUG_ME
#घोषणा NTLMFLAG_TARGET_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#घोषणा NTLMFLAG_TARGET_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#घोषणा NTLMFLAG_TARGET_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#पूर्ण_अगर

#घोषणा NTLMFLAG_NEGOTIATE_NTLM2_कुंजी             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#अगर DEBUG_ME
#घोषणा NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#घोषणा NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#घोषणा NTLMFLAG_REQUEST_NONNT_SESSION_कुंजी       (1 << 22)
/* unknown purpose */
#पूर्ण_अगर

#घोषणा NTLMFLAG_NEGOTIATE_TARGET_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#अगर DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#घोषणा NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#घोषणा NTLMFLAG_NEGOTIATE_कुंजी_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#घोषणा NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#घोषणा DEBUG_OUT(x) x
अटल व्योम ntlm_print_flags(खाता *handle, अचिन्हित दीर्घ flags)
{
  अगर(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  अगर(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  अगर(flags & NTLMFLAG_REQUEST_TARGET)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  अगर(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  अगर(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  अगर(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  अगर(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  अगर(flags & NTLMFLAG_NEGOTIATE_LM_कुंजी)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  अगर(flags & NTLMFLAG_NEGOTIATE_NTLM_कुंजी)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  अगर(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  अगर(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  अगर(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  अगर(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  अगर(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  अगर(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  अगर(flags & NTLMFLAG_TARGET_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  अगर(flags & NTLMFLAG_TARGET_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  अगर(flags & NTLMFLAG_TARGET_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  अगर(flags & NTLMFLAG_NEGOTIATE_NTLM2_कुंजी)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  अगर(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  अगर(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  अगर(flags & NTLMFLAG_REQUEST_NONNT_SESSION_कुंजी)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  अगर(flags & NTLMFLAG_NEGOTIATE_TARGET_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  अगर(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  अगर(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  अगर(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  अगर(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  अगर(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  अगर(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  अगर(flags & NTLMFLAG_NEGOTIATE_कुंजी_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  अगर(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

अटल व्योम ntlm_print_hex(खाता *handle, स्थिर अक्षर *buf, माप_प्रकार len)
{
  स्थिर अक्षर *p = buf;

  (व्योम)handle;

  curl_mfprintf(मानक_त्रुटि, "0x");
  जबतक(len-- > 0)
    curl_mfprintf(मानक_त्रुटि, "%02.2x", (अचिन्हित पूर्णांक)*p++);
}
#अन्यथा
#घोषणा DEBUG_OUT(x) Curl_nop_stmt
#पूर्ण_अगर

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
अटल CURLcode ntlm_decode_type2_target(काष्ठा Curl_easy *तथ्य,
                                         स्थिर काष्ठा bufref *type2ref,
                                         काष्ठा ntlmdata *ntlm)
{
  अचिन्हित लघु target_info_len = 0;
  अचिन्हित पूर्णांक target_info_offset = 0;
  स्थिर अचिन्हित अक्षर *type2 = Curl_bufref_uptr(type2ref);
  माप_प्रकार type2len = Curl_bufref_len(type2ref);

  अगर(type2len >= 48) {
    target_info_len = Curl_read16_le(&type2[40]);
    target_info_offset = Curl_read32_le(&type2[44]);
    अगर(target_info_len > 0) {
      अगर((target_info_offset > type2len) ||
         (target_info_offset + target_info_len) > type2len ||
         target_info_offset < 48) {
        infof(तथ्य, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        वापस CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->target_info); /* replace any previous data */
      ntlm->target_info = curlx_memdup(&type2[target_info_offset],
                                       target_info_len);
      अगर(!ntlm->target_info)
        वापस CURLE_OUT_OF_MEMORY;
    }
  }

  ntlm->target_info_len = target_info_len;

  वापस CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(व्योम)
{
  वापस TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_message(काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा bufref *type2ref,
                                             काष्ठा ntlmdata *ntlm)
{
  अटल स्थिर अक्षर type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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
  स्थिर अचिन्हित अक्षर *type2 = Curl_bufref_uptr(type2ref);
  माप_प्रकार type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  अगर((type2len < 32) ||
     स_भेद(type2, NTLMSSP_SIGNATURE, 8) ||
     स_भेद(type2 + 8, type2_marker, माप(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(तथ्य, "NTLM handshake failure (bad type-2 message)");
    वापस CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  स_नकल(ntlm->nonce, &type2[24], 8);

  अगर(ntlm->flags & NTLMFLAG_NEGOTIATE_TARGET_INFO) {
    परिणाम = ntlm_decode_type2_target(तथ्य, type2ref, ntlm);
    अगर(परिणाम) {
      infof(तथ्य, "NTLM handshake failure (bad type-2 message)");
      वापस परिणाम;
    }
  }

  DEBUG_OUT({
    curl_mfprintf(मानक_त्रुटि, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(मानक_त्रुटि, ntlm->flags);
    curl_mfprintf(मानक_त्रुटि, "\n                  nonce=");
    ntlm_print_hex(मानक_त्रुटि, (अक्षर *)ntlm->nonce, 8);
    curl_mfprintf(मानक_त्रुटि, "\n****\n");
    curl_mfprintf(मानक_त्रुटि, "**** Header %s\n ", header);
  });

  वापस परिणाम;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
अटल व्योम unicodecpy(अचिन्हित अक्षर *dest, स्थिर अक्षर *src, माप_प्रकार लंबाई)
{
  माप_प्रकार i;
  क्रम(i = 0; i < लंबाई; i++) {
    dest[2 * i] = (अचिन्हित अक्षर)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_message(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_creds *creds,
                                             स्थिर अक्षर *default_service,
                                             स्थिर अक्षर *host,
                                             काष्ठा ntlmdata *ntlm,
                                             काष्ठा bufref *out)
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

  स्थिर अक्षर *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  माप_प्रकार आकार;

  अक्षर *ntlmbuf;
  स्थिर अक्षर *hostname = "";          /* empty */
  स्थिर अक्षर *domain = "";            /* empty */
  माप_प्रकार hostlen = 0;
  माप_प्रकार domlen = 0;
  माप_प्रकार hostoff = 0;
  माप_प्रकार domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (व्योम)तथ्य;
  (व्योम)creds;
  (व्योम)service;
  (व्योम)host;

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
                                      NTLMFLAG_NEGOTIATE_NTLM_कुंजी |
                                      NTLMFLAG_NEGOTIATE_NTLM2_कुंजी |
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

  अगर(!ntlmbuf)
    वापस CURLE_OUT_OF_MEMORY;

  /* Initial packet length */
  आकार = 32 + hostlen + domlen;

  DEBUG_OUT({
    curl_mfprintf(मानक_त्रुटि, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_TARGET |
                              NTLMFLAG_NEGOTIATE_NTLM_कुंजी |
                              NTLMFLAG_NEGOTIATE_NTLM2_कुंजी |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_TARGET |
                  NTLMFLAG_NEGOTIATE_NTLM_कुंजी |
                  NTLMFLAG_NEGOTIATE_NTLM2_कुंजी |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(मानक_त्रुटि,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_TARGET |
                     NTLMFLAG_NEGOTIATE_NTLM_कुंजी |
                     NTLMFLAG_NEGOTIATE_NTLM2_कुंजी |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(मानक_त्रुटि, "\n****\n");
  });

  Curl_bufref_set(out, ntlmbuf, आकार, curl_free);
  वापस CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_message(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा Curl_creds *creds,
                                             काष्ठा ntlmdata *ntlm,
                                             काष्ठा bufref *out)
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
  माप_प्रकार आकार;
  अचिन्हित अक्षर ntlmbuf[NTLM_BUFSIZE];
  अचिन्हित पूर्णांक lmrespoff;
  अचिन्हित अक्षर lmresp[24]; /* fixed-size */
  अचिन्हित पूर्णांक ntrespoff;
  अचिन्हित पूर्णांक ntresplen = 24;
  अचिन्हित अक्षर ntresp[24]; /* fixed-size */
  स्थिर अचिन्हित अक्षर *ptr_ntresp = &ntresp[0];
  अचिन्हित अक्षर *ntlmv2resp = शून्य;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  अटल स्थिर अक्षर host[] = "WORKSTATION";
  स्थिर अक्षर *userp = Curl_creds_user(creds);
  स्थिर अक्षर *passwdp = Curl_creds_passwd(creds);
  स्थिर अक्षर *user;
  स्थिर अक्षर *domain = "";
  माप_प्रकार hostoff = 0;
  माप_प्रकार useroff = 0;
  माप_प्रकार domoff = 0;
  माप_प्रकार hostlen = 0;
  माप_प्रकार userlen = 0;
  माप_प्रकार domlen = 0;

  स_रखो(lmresp, 0, माप(lmresp));
  स_रखो(ntresp, 0, माप(ntresp));
  user = म_अक्षर(userp, '\\');
  अगर(!user)
    user = म_अक्षर(userp, '/');

  अगर(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  अन्यथा
    user = userp;

  userlen = म_माप(user);
  hostlen = माप(host) - 1;

  अगर(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_कुंजी) {
    अचिन्हित अक्षर ntbuffer[0x18];
    अचिन्हित अक्षर entropy[8];
    अचिन्हित अक्षर ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    परिणाम = Curl_rand(तथ्य, entropy, 8);
    अगर(परिणाम)
      वापस परिणाम;

    परिणाम = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    अगर(परिणाम)
      वापस परिणाम;

    परिणाम = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    अगर(परिणाम)
      वापस परिणाम;

    /* LMv2 response */
    परिणाम = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    अगर(परिणाम)
      वापस परिणाम;

    /* NTLMv2 response */
    परिणाम = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    अगर(परिणाम)
      वापस परिणाम;

    ptr_ntresp = ntlmv2resp;
  }
  अन्यथा {

    अचिन्हित अक्षर ntbuffer[0x18];
    अचिन्हित अक्षर lmbuffer[0x18];

    /* NTLM version 1 */

    परिणाम = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    अगर(परिणाम)
      वापस परिणाम;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    परिणाम = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    अगर(परिणाम)
      वापस परिणाम;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(अचिन्हित पूर्णांक)NTLMFLAG_NEGOTIATE_NTLM2_कुंजी;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  अगर(unicode) {
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
  आकार = curl_msnprintf((अक्षर *)ntlmbuf, NTLM_BUFSIZE,
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
  DEBUGASSERT(आकार == (माप_प्रकार)lmrespoff);

  /* We append the binary hashes */
  अगर(आकार < (NTLM_BUFSIZE - 0x18)) {
    स_नकल(&ntlmbuf[आकार], lmresp, 0x18);
    आकार += 0x18;
  }

  DEBUG_OUT({
    curl_mfprintf(मानक_त्रुटि, "**** TYPE3 header lmresp=");
    ntlm_print_hex(मानक_त्रुटि, (अक्षर *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  अगर(ntresplen + आकार > माप(ntlmbuf)) {
    failf(तथ्य, "incoming NTLM message too big");
    परिणाम = CURLE_TOO_LARGE;
    जाओ error;
  }
  DEBUGASSERT(आकार == (माप_प्रकार)ntrespoff);
  स_नकल(&ntlmbuf[आकार], ptr_ntresp, ntresplen);
  आकार += ntresplen;

  DEBUG_OUT({
    curl_mfprintf(मानक_त्रुटि, "\n   ntresp=");
    ntlm_print_hex(मानक_त्रुटि, (अक्षर *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_OUT({
    curl_mfprintf(मानक_त्रुटि, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(मानक_त्रुटि, ntlm->flags);
    curl_mfprintf(मानक_त्रुटि, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  अगर(आकार + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(तथ्य, "user + domain + hostname too big for NTLM");
    परिणाम = CURLE_TOO_LARGE;
    जाओ error;
  }

  DEBUGASSERT(आकार == domoff);
  अगर(unicode)
    unicodecpy(&ntlmbuf[आकार], domain, domlen / 2);
  अन्यथा
    स_नकल(&ntlmbuf[आकार], domain, domlen);

  आकार += domlen;

  DEBUGASSERT(आकार == useroff);
  अगर(unicode)
    unicodecpy(&ntlmbuf[आकार], user, userlen / 2);
  अन्यथा
    स_नकल(&ntlmbuf[आकार], user, userlen);

  आकार += userlen;

  DEBUGASSERT(आकार == hostoff);
  अगर(unicode)
    unicodecpy(&ntlmbuf[आकार], host, hostlen / 2);
  अन्यथा
    स_नकल(&ntlmbuf[आकार], host, hostlen);

  आकार += hostlen;

  /* Return the binary blob. */
  परिणाम = Curl_bufref_memdup0(out, ntlmbuf, आकार);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  वापस परिणाम;
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
व्योम Curl_auth_cleanup_ntlm(काष्ठा ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->target_info);

  /* Reset any variables */
  ntlm->target_info_len = 0;
}

#पूर्ण_अगर /* USE_NTLM && !USE_WINDOWS_SSPI */
