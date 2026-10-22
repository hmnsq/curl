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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ defined(USE_NTLM) && !defined(USE_WINDOWS_SSPI)

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

#నిర్వచించు DEBUG_ME 0

#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curl_ntlm_core.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curl_endian.h"

/* "NTLMSSP" signature is always in ASCII regardless of the platform */
#నిర్వచించు NTLMSSP_SIGNATURE "\x4e\x54\x4c\x4d\x53\x53\x50"

/* NTLM buffer fixed size, large enough for long user + host + domain */
#నిర్వచించు NTLM_BUFSIZE 1024

/* Flag bits definitions based on
   https://davenport.sourceforge.net/ntlm.html */

#నిర్వచించు NTLMFLAG_NEGOTIATE_UNICODE               (1 << 0)
/* Indicates that Unicode strings are supported for use in security buffer
   data. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_OEM                   (1 << 1)
/* Indicates that OEM strings are supported for use in security buffer data. */

#నిర్వచించు NTLMFLAG_REQUEST_లక్ష్యం                  (1 << 2)
/* Requests that the server's authentication realm be included in the Type 2
   message. */

#ఒకవేళ DEBUG_ME
/* unknown (1 << 3) */
#నిర్వచించు NTLMFLAG_NEGOTIATE_SIGN                  (1 << 4)
/* Specifies that authenticated communication between the client and server
   should carry a digital signature (message integrity). */

#నిర్వచించు NTLMFLAG_NEGOTIATE_SEAL                  (1 << 5)
/* Specifies that authenticated communication between the client and server
   should be encrypted (message confidentiality). */

#నిర్వచించు NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE        (1 << 6)
/* Indicates that datagram authentication is being used. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_LM_కీలకం                (1 << 7)
/* Indicates that the LAN Manager session key should be used for signing and
   sealing authenticated communications. */
#అయితే_ముగింపు

#నిర్వచించు NTLMFLAG_NEGOTIATE_NTLM_కీలకం              (1 << 9)
/* Indicates that NTLM authentication is being used. */

#ఒకవేళ DEBUG_ME
/* unknown (1 << 10) */

#నిర్వచించు NTLMFLAG_NEGOTIATE_ANONYMOUS             (1 << 11)
/* Sent by the client in the Type 3 message to indicate that an anonymous
   context has been established. This also affects the response fields. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED       (1 << 12)
/* Sent by the client in the Type 1 message to indicate that a desired
   authentication realm is included in the message. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED  (1 << 13)
/* Sent by the client in the Type 1 message to indicate that the client
   workstation's name is included in the message. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_LOCAL_CALL            (1 << 14)
/* Sent by the server to indicate that the server and client are on the same
   machine. Implies that the client may use a pre-established local security
   context rather than responding to the challenge. */
#అయితే_ముగింపు

#నిర్వచించు NTLMFLAG_NEGOTIATE_ALWAYS_SIGN           (1 << 15)
/* Indicates that authenticated communication between the client and server
   should be signed with a "dummy" signature. */

#ఒకవేళ DEBUG_ME
#నిర్వచించు NTLMFLAG_లక్ష్యం_TYPE_DOMAIN              (1 << 16)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a domain. */

#నిర్వచించు NTLMFLAG_లక్ష్యం_TYPE_SERVER              (1 << 17)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a server. */

#నిర్వచించు NTLMFLAG_లక్ష్యం_TYPE_SHARE               (1 << 18)
/* Sent by the server in the Type 2 message to indicate that the target
   authentication realm is a share. Presumably, this is for share-level
   authentication. Usage is unclear. */
#అయితే_ముగింపు

#నిర్వచించు NTLMFLAG_NEGOTIATE_NTLM2_కీలకం             (1 << 19)
/* Indicates that the NTLM2 signing and sealing scheme should be used for
   protecting authenticated communications. */

#ఒకవేళ DEBUG_ME
#నిర్వచించు NTLMFLAG_REQUEST_INIT_RESPONSE           (1 << 20)
/* unknown purpose */

#నిర్వచించు NTLMFLAG_REQUEST_ACCEPT_RESPONSE         (1 << 21)
/* unknown purpose */

#నిర్వచించు NTLMFLAG_REQUEST_NONNT_SESSION_కీలకం       (1 << 22)
/* unknown purpose */
#అయితే_ముగింపు

#నిర్వచించు NTLMFLAG_NEGOTIATE_లక్ష్యం_INFO           (1 << 23)
/* Sent by the server in the Type 2 message to indicate that it is including a
   Target Information block in the message. */

#ఒకవేళ DEBUG_ME
/* unknown (1<24) */
/* unknown (1<25) */
/* unknown (1<26) */
/* unknown (1<27) */
/* unknown (1<28) */

#నిర్వచించు NTLMFLAG_NEGOTIATE_128                   (1 << 29)
/* Indicates that 128-bit encryption is supported. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_కీలకం_EXCHANGE          (1 << 30)
/* Indicates that the client provides an encrypted master key in
   the "Session Key" field of the Type 3 message. */

#నిర్వచించు NTLMFLAG_NEGOTIATE_56                    (1 << 31)
/* Indicates that 56-bit encryption is supported. */

#నిర్వచించు DEBUG_బహిర్గతం(x) x
స్థిర శూన్య ntlm_print_flags(FILE *handle, గుర్తులేని దీర్ఘ flags)
{
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_UNICODE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_UNICODE ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_OEM)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_OEM ");
  ఒకవేళ(flags & NTLMFLAG_REQUEST_లక్ష్యం)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_TARGET ");
  ఒకవేళ(flags & (1 << 3))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_3 ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SIGN ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_SEAL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_SEAL ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DATAGRAM_STYLE ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_LM_కీలకం)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LM_KEY ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_NTLM_కీలకం)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM_KEY ");
  ఒకవేళ(flags & (1 << 10))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_10 ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_ANONYMOUS)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ANONYMOUS ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_DOMAIN_SUPPLIED ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_WORKSTATION_SUPPLIED ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_LOCAL_CALL)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_LOCAL_CALL ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_ALWAYS_SIGN)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_ALWAYS_SIGN ");
  ఒకవేళ(flags & NTLMFLAG_లక్ష్యం_TYPE_DOMAIN)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_DOMAIN ");
  ఒకవేళ(flags & NTLMFLAG_లక్ష్యం_TYPE_SERVER)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SERVER ");
  ఒకవేళ(flags & NTLMFLAG_లక్ష్యం_TYPE_SHARE)
    curl_mfprintf(handle, "NTLMFLAG_TARGET_TYPE_SHARE ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_NTLM2_కీలకం)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_NTLM2_KEY ");
  ఒకవేళ(flags & NTLMFLAG_REQUEST_INIT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_INIT_RESPONSE ");
  ఒకవేళ(flags & NTLMFLAG_REQUEST_ACCEPT_RESPONSE)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_ACCEPT_RESPONSE ");
  ఒకవేళ(flags & NTLMFLAG_REQUEST_NONNT_SESSION_కీలకం)
    curl_mfprintf(handle, "NTLMFLAG_REQUEST_NONNT_SESSION_KEY ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_లక్ష్యం_INFO)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_TARGET_INFO ");
  ఒకవేళ(flags & (1 << 24))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_24 ");
  ఒకవేళ(flags & (1 << 25))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_25 ");
  ఒకవేళ(flags & (1 << 26))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_26 ");
  ఒకవేళ(flags & (1 << 27))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_27 ");
  ఒకవేళ(flags & (1 << 28))
    curl_mfprintf(handle, "NTLMFLAG_UNKNOWN_28 ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_128)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_128 ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_కీలకం_EXCHANGE)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_KEY_EXCHANGE ");
  ఒకవేళ(flags & NTLMFLAG_NEGOTIATE_56)
    curl_mfprintf(handle, "NTLMFLAG_NEGOTIATE_56 ");
}

స్థిర శూన్య ntlm_print_hex(FILE *handle, స్థిరరాశి అక్షరం *buf, size_t len)
{
  స్థిరరాశి అక్షరం *p = buf;

  (శూన్య)handle;

  curl_mfprintf(stderr, "0x");
  అయితే(len-- > 0)
    curl_mfprintf(stderr, "%02.2x", (గుర్తులేని పూర్ణాంకం)*p++);
}
#లేకపోతే
#నిర్వచించు DEBUG_బహిర్గతం(x) Curl_nop_stmt
#అయితే_ముగింపు

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
స్థిర CURLcode ntlm_decode_type2_లక్ష్యం(నిర్మాణం Curl_easy *దత్తాంశం,
                                         స్థిరరాశి నిర్మాణం bufref *type2ref,
                                         నిర్మాణం ntlmdata *ntlm)
{
  గుర్తులేని పొట్టి లక్ష్యం_info_len = 0;
  గుర్తులేని పూర్ణాంకం లక్ష్యం_info_స్థానభ్రంశం = 0;
  స్థిరరాశి గుర్తులేని అక్షరం *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ఒకవేళ(type2len >= 48) {
    లక్ష్యం_info_len = Curl_read16_le(&type2[40]);
    లక్ష్యం_info_స్థానభ్రంశం = Curl_read32_le(&type2[44]);
    ఒకవేళ(లక్ష్యం_info_len > 0) {
      ఒకవేళ((లక్ష్యం_info_స్థానభ్రంశం > type2len) ||
         (లక్ష్యం_info_స్థానభ్రంశం + లక్ష్యం_info_len) > type2len ||
         లక్ష్యం_info_స్థానభ్రంశం < 48) {
        infof(దత్తాంశం, "NTLM handshake failure (bad type-2 message). "
              "Target Info Offset Len is set incorrect by the peer");
        తిరిగి CURLE_BAD_CONTENT_ENCODING;
      }

      curlx_free(ntlm->లక్ష్యం_info); /* replace any previous data */
      ntlm->లక్ష్యం_info = curlx_memdup(&type2[లక్ష్యం_info_స్థానభ్రంశం],
                                       లక్ష్యం_info_len);
      ఒకవేళ(!ntlm->లక్ష్యం_info)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  ntlm->లక్ష్యం_info_len = లక్ష్యం_info_len;

  తిరిగి CURLE_OK;
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
bool Curl_auth_is_ntlm_supported(శూన్య)
{
  తిరిగి TRUE;
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
CURLcode Curl_auth_decode_ntlm_type2_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం bufref *type2ref,
                                             నిర్మాణం ntlmdata *ntlm)
{
  స్థిర స్థిరరాశి అక్షరం type2_marker[] = { 0x02, 0x00, 0x00, 0x00 };

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

  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి గుర్తులేని అక్షరం *type2 = Curl_bufref_uptr(type2ref);
  size_t type2len = Curl_bufref_len(type2ref);

  ntlm->flags = 0;

  ఒకవేళ((type2len < 32) ||
     memcmp(type2, NTLMSSP_SIGNATURE, 8) ||
     memcmp(type2 + 8, type2_marker, పరిమాణం(type2_marker))) {
    /* This was not a good enough type-2 message */
    infof(దత్తాంశం, "NTLM handshake failure (bad type-2 message)");
    తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  ntlm->flags = Curl_read32_le(&type2[20]);
  జ్ఞాపకం_నకలు(ntlm->nonce, &type2[24], 8);

  ఒకవేళ(ntlm->flags & NTLMFLAG_NEGOTIATE_లక్ష్యం_INFO) {
    ఫలితం = ntlm_decode_type2_లక్ష్యం(దత్తాంశం, type2ref, ntlm);
    ఒకవేళ(ఫలితం) {
      infof(దత్తాంశం, "NTLM handshake failure (bad type-2 message)");
      తిరిగి ఫలితం;
    }
  }

  DEBUG_బహిర్గతం({
    curl_mfprintf(stderr, "**** TYPE2 header flags=0x%08.8lx ", ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n                  nonce=");
    ntlm_print_hex(stderr, (అక్షరం *)ntlm->nonce, 8);
    curl_mfprintf(stderr, "\n****\n");
    curl_mfprintf(stderr, "**** Header %s\n ", శీర్షిక);
  });

  తిరిగి ఫలితం;
}

/* copy the source to the destination and fill in zeroes in every
   other destination byte! */
స్థిర శూన్య unicodecpy(గుర్తులేని అక్షరం *dest, స్థిరరాశి అక్షరం *src, size_t నిడివి)
{
  size_t i;
  కొరకు(i = 0; i < నిడివి; i++) {
    dest[2 * i] = (గుర్తులేని అక్షరం)src[i];
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
CURLcode Curl_auth_create_ntlm_type1_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_creds *creds,
                                             స్థిరరాశి అక్షరం *default_service,
                                             స్థిరరాశి అక్షరం *host,
                                             నిర్మాణం ntlmdata *ntlm,
                                             నిర్మాణం bufref *బహిర్గతం)
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

  స్థిరరాశి అక్షరం *service = Curl_creds_has_sasl_service(creds) ?
    Curl_creds_sasl_service(creds) : default_service;
  size_t కొలత;

  అక్షరం *ntlmbuf;
  స్థిరరాశి అక్షరం *hostname = "";          /*ఖాళీలు*/
  స్థిరరాశి అక్షరం *domain = "";            /*ఖాళీలు*/
  size_t hostlen = 0;
  size_t domlen = 0;
  size_t hostoff = 0;
  size_t domoff = hostoff + hostlen;  /* This is 0: remember that host and
                                         domain are empty */
  (శూన్య)దత్తాంశం;
  (శూన్య)creds;
  (శూన్య)service;
  (శూన్య)host;

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
                                      NTLMFLAG_REQUEST_లక్ష్యం |
                                      NTLMFLAG_NEGOTIATE_NTLM_కీలకం |
                                      NTLMFLAG_NEGOTIATE_NTLM2_కీలకం |
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

  ఒకవేళ(!ntlmbuf)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Initial packet length */
  కొలత = 32 + hostlen + domlen;

  DEBUG_బహిర్గతం({
    curl_mfprintf(stderr, "* TYPE1 header flags=0x%02.2x%02.2x%02.2x%02.2x "
                  "0x%08.8x ",
                  LONGQUARTET(NTLMFLAG_NEGOTIATE_OEM |
                              NTLMFLAG_REQUEST_లక్ష్యం |
                              NTLMFLAG_NEGOTIATE_NTLM_కీలకం |
                              NTLMFLAG_NEGOTIATE_NTLM2_కీలకం |
                              NTLMFLAG_NEGOTIATE_ALWAYS_SIGN),
                  NTLMFLAG_NEGOTIATE_OEM |
                  NTLMFLAG_REQUEST_లక్ష్యం |
                  NTLMFLAG_NEGOTIATE_NTLM_కీలకం |
                  NTLMFLAG_NEGOTIATE_NTLM2_కీలకం |
                  NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    ntlm_print_flags(stderr,
                     NTLMFLAG_NEGOTIATE_OEM |
                     NTLMFLAG_REQUEST_లక్ష్యం |
                     NTLMFLAG_NEGOTIATE_NTLM_కీలకం |
                     NTLMFLAG_NEGOTIATE_NTLM2_కీలకం |
                     NTLMFLAG_NEGOTIATE_ALWAYS_SIGN);
    curl_mfprintf(stderr, "\n****\n");
  });

  Curl_bufref_సమితి(బహిర్గతం, ntlmbuf, కొలత, curl_free);
  తిరిగి CURLE_OK;
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
CURLcode Curl_auth_create_ntlm_type3_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం Curl_creds *creds,
                                             నిర్మాణం ntlmdata *ntlm,
                                             నిర్మాణం bufref *బహిర్గతం)
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

  CURLcode ఫలితం = CURLE_OK;
  size_t కొలత;
  గుర్తులేని అక్షరం ntlmbuf[NTLM_BUFSIZE];
  గుర్తులేని పూర్ణాంకం lmrespoff;
  గుర్తులేని అక్షరం lmresp[24]; /* fixed-size */
  గుర్తులేని పూర్ణాంకం ntrespoff;
  గుర్తులేని పూర్ణాంకం ntresplen = 24;
  గుర్తులేని అక్షరం ntresp[24]; /* fixed-size */
  స్థిరరాశి గుర్తులేని అక్షరం *ptr_ntresp = &ntresp[0];
  గుర్తులేని అక్షరం *ntlmv2resp = ఖాళీ;
  bool unicode = (ntlm->flags & NTLMFLAG_NEGOTIATE_UNICODE);
  /* The fixed hostname we provide, in order to not leak our real local host
     name. Copy the name used by Firefox. */
  స్థిర స్థిరరాశి అక్షరం host[] = "WORKSTATION";
  స్థిరరాశి అక్షరం *userp = Curl_creds_user(creds);
  స్థిరరాశి అక్షరం *passwdp = Curl_creds_passwd(creds);
  స్థిరరాశి అక్షరం *user;
  స్థిరరాశి అక్షరం *domain = "";
  size_t hostoff = 0;
  size_t useroff = 0;
  size_t domoff = 0;
  size_t hostlen = 0;
  size_t userlen = 0;
  size_t domlen = 0;

  memset(lmresp, 0, పరిమాణం(lmresp));
  memset(ntresp, 0, పరిమాణం(ntresp));
  user = strchr(userp, '\\');
  ఒకవేళ(!user)
    user = strchr(userp, '/');

  ఒకవేళ(user) {
    domain = userp;
    domlen = (user - domain);
    user++;
  }
  లేకపోతే
    user = userp;

  userlen = అక్షరశ్రేణి_పొడవు(user);
  hostlen = పరిమాణం(host) - 1;

  ఒకవేళ(ntlm->flags & NTLMFLAG_NEGOTIATE_NTLM2_కీలకం) {
    గుర్తులేని అక్షరం ntbuffer[0x18];
    గుర్తులేని అక్షరం entropy[8];
    గుర్తులేని అక్షరం ntlmv2hash[0x18];

    /* Full NTLM version 2
       Although this cannot be negotiated, it is used here if available, as
       servers featuring extended security are likely supporting also
       NTLMv2. */
    ఫలితం = Curl_rand(దత్తాంశం, entropy, 8);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఫలితం = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ఫలితం = Curl_ntlm_core_mk_ntlmv2_hash(user, userlen, domain, domlen,
                                           ntbuffer, ntlmv2hash);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* LMv2 response */
    ఫలితం = Curl_ntlm_core_mk_lmv2_resp(ntlmv2hash, entropy,
                                         &ntlm->nonce[0], lmresp);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* NTLMv2 response */
    ఫలితం = Curl_ntlm_core_mk_ntlmv2_resp(ntlmv2hash, entropy,
                                           ntlm, &ntlmv2resp, &ntresplen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ptr_ntresp = ntlmv2resp;
  }
  లేకపోతే {

    గుర్తులేని అక్షరం ntbuffer[0x18];
    గుర్తులేని అక్షరం lmbuffer[0x18];

    /* NTLM version 1 */

    ఫలితం = Curl_ntlm_core_mk_nt_hash(passwdp, ntbuffer);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], ntresp);

    ఫలితం = Curl_ntlm_core_mk_lm_hash(passwdp, lmbuffer);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    Curl_ntlm_core_lm_resp(lmbuffer, &ntlm->nonce[0], lmresp);
    ntlm->flags &= ~(గుర్తులేని పూర్ణాంకం)NTLMFLAG_NEGOTIATE_NTLM2_కీలకం;

    /* A safer but less compatible alternative is:
     *   Curl_ntlm_core_lm_resp(ntbuffer, &ntlm->nonce[0], lmresp);
     * See https://davenport.sourceforge.net/ntlm.html#ntlmVersion2 */
  }

  ఒకవేళ(unicode) {
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
  కొలత = curl_msnprintf((అక్షరం *)ntlmbuf, NTLM_BUFSIZE,
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

  DEBUGASSERT(కొలత == 64);
  DEBUGASSERT(కొలత == (size_t)lmrespoff);

  /* We append the binary hashes */
  ఒకవేళ(కొలత < (NTLM_BUFSIZE - 0x18)) {
    జ్ఞాపకం_నకలు(&ntlmbuf[కొలత], lmresp, 0x18);
    కొలత += 0x18;
  }

  DEBUG_బహిర్గతం({
    curl_mfprintf(stderr, "**** TYPE3 header lmresp=");
    ntlm_print_hex(stderr, (అక్షరం *)&ntlmbuf[lmrespoff], 0x18);
  });

  /* ntresplen + size should not be risking an integer overflow here */
  ఒకవేళ(ntresplen + కొలత > పరిమాణం(ntlmbuf)) {
    failf(దత్తాంశం, "incoming NTLM message too big");
    ఫలితం = CURLE_TOO_LARGE;
    వెళ్ళు error;
  }
  DEBUGASSERT(కొలత == (size_t)ntrespoff);
  జ్ఞాపకం_నకలు(&ntlmbuf[కొలత], ptr_ntresp, ntresplen);
  కొలత += ntresplen;

  DEBUG_బహిర్గతం({
    curl_mfprintf(stderr, "\n   ntresp=");
    ntlm_print_hex(stderr, (అక్షరం *)&ntlmbuf[ntrespoff], ntresplen);
  });

  DEBUG_బహిర్గతం({
    curl_mfprintf(stderr, "\n   flags=0x%02.2x%02.2x%02.2x%02.2x 0x%08.8x ",
                  LONGQUARTET(ntlm->flags), ntlm->flags);
    ntlm_print_flags(stderr, ntlm->flags);
    curl_mfprintf(stderr, "\n****\n");
  });

  /* Make sure that the domain, user and host strings fit in the
     buffer before we copy them there. */
  ఒకవేళ(కొలత + userlen + domlen + hostlen >= NTLM_BUFSIZE) {
    failf(దత్తాంశం, "user + domain + hostname too big for NTLM");
    ఫలితం = CURLE_TOO_LARGE;
    వెళ్ళు error;
  }

  DEBUGASSERT(కొలత == domoff);
  ఒకవేళ(unicode)
    unicodecpy(&ntlmbuf[కొలత], domain, domlen / 2);
  లేకపోతే
    జ్ఞాపకం_నకలు(&ntlmbuf[కొలత], domain, domlen);

  కొలత += domlen;

  DEBUGASSERT(కొలత == useroff);
  ఒకవేళ(unicode)
    unicodecpy(&ntlmbuf[కొలత], user, userlen / 2);
  లేకపోతే
    జ్ఞాపకం_నకలు(&ntlmbuf[కొలత], user, userlen);

  కొలత += userlen;

  DEBUGASSERT(కొలత == hostoff);
  ఒకవేళ(unicode)
    unicodecpy(&ntlmbuf[కొలత], host, hostlen / 2);
  లేకపోతే
    జ్ఞాపకం_నకలు(&ntlmbuf[కొలత], host, hostlen);

  కొలత += hostlen;

  /* Return the binary blob. */
  ఫలితం = Curl_bufref_memdup0(బహిర్గతం, ntlmbuf, కొలత);

error:
  curlx_free(ntlmv2resp);  /* Free the dynamic buffer allocated for NTLMv2 */

  Curl_auth_cleanup_ntlm(ntlm);

  తిరిగి ఫలితం;
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
శూన్య Curl_auth_cleanup_ntlm(నిర్మాణం ntlmdata *ntlm)
{
  /* Free the target info */
  curlx_safefree(ntlm->లక్ష్యం_info);

  /* Reset any variables */
  ntlm->లక్ష్యం_info_len = 0;
}

#అయితే_ముగింపు /* USE_NTLM && !USE_WINDOWS_SSPI */
