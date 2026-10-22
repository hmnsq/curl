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
 * RFC1870 SMTP Service Extension for Message Size
 * RFC2195 CRAM-MD5 authentication
 * RFC2831 DIGEST-MD5 authentication
 * RFC3207 SMTP over TLS
 * RFC4422 Simple Authentication and Security Layer (SASL)
 * RFC4616 PLAIN authentication
 * RFC4752 The Kerberos V5 ("GSSAPI") SASL Mechanism
 * RFC4954 SMTP Authentication
 * RFC5321 SMTP protocol
 * RFC5890 Internationalized Domain Names for Applications (IDNA)
 * RFC6531 SMTP Extension for Internationalized Email
 * RFC6532 Internationalized Email Headers
 * RFC6749 OAuth 2.0 Authorization Framework
 * RFC8314 Use of TLS for Email Submission and Access
 * Draft   SMTP URL Interface   <draft-earhart-url-smtp-00.txt>
 * Draft   LOGIN SASL Mechanism <draft-murchison-sasl-login-00.txt>
 *
 ***************************************************************************/
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "smtp.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "pingpong.h"
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "curl_gethostname.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curl_sasl.h"
#ಅನ್ತರ್ಭಾವ "idn.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  SMTP_STOP,        /* do nothing state, stops the state machine */
  SMTP_SERVERGREET, /* waiting for the initial greeting immediately after
                       a connect */
  SMTP_EHLO,
  SMTP_HELO,
  SMTP_STARTTLS,
  SMTP_UPGRADETLS,  /* asynchronously upgrade the connection to SSL/TLS
                       (multi mode only) */
  SMTP_AUTH,
  SMTP_COMMAND,     /* VRFY, EXPN, NOOP, RSET and HELP */
  SMTP_MAIL,        /* MAIL FROM */
  SMTP_RCPT,        /* RCPT TO */
  SMTP_ದತ್ತಾಂಶ,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_ಕೊನೆಯ         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
ರಚನೆ smtp_conn {
  ರಚನೆ pingpong pp;
  ರಚನೆ SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  ಅಕ್ಷರ *domain;            /* Client address/name to send in the EHLO */
  ಬಿಟ್(ssldone);            /* Is connect() over SSL done? */
  ಬಿಟ್(tls_supported);      /* StartTLS capability supported by server */
  ಬಿಟ್(ಗಾತ್ರ_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  ಬಿಟ್(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  ಬಿಟ್(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ರಚನೆ SMTP {
  curl_pp_transfer transfer;
  ಅಕ್ಷರ *custom;            /* Custom Request */
  ರಚನೆ curl_slist *rcpt; /* Recipient list */
  ಪೂರ್ಣಾಂಕ rcpt_ಕೊನೆಯ_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  ಬಿಟ್(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  ಬಿಟ್(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
ಸ್ಥಿರ CURLcode smtp_parse_url_options(ರಚನೆ connectdata *conn,
                                       ರಚನೆ smtp_conn *smtpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = conn->options;

  ಆಗ(!ಫಲಿತಾಂಶ && ptr && *ptr) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = ptr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;

    ಆಗ(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಆಗ(*ptr && *ptr != ';')
      ptr++;

    ಆದರೆ(curl_strnequal(ಕೀಲಿ, "AUTH=", 5))
      ಫಲಿತಾಂಶ = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;

    ಆದರೆ(*ptr == ';')
      ptr++;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ಸ್ಥಿರ CURLcode smtp_parse_url_ಮಾರ್ಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = &ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ[1]; /* skip leading path */
  ಅಕ್ಷರ localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  ಆದರೆ(!*ಮಾರ್ಗ) {
    ಆದರೆ(!Curl_gethostname(localhost, ಗಾತ್ರ(localhost)))
      ಮಾರ್ಗ = localhost;
    ಇಲ್ಲದಿದ್ದರೆ
      ಮಾರ್ಗ = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  ಹಿಂತಿರುಗಿ Curl_urldecode(ಮಾರ್ಗ, 0, &smtpc->domain, ಖಾಲಿ, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ಸ್ಥಿರ CURLcode smtp_parse_custom_request(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ SMTP *smtp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *custom = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  ಆದರೆ(custom)
    ಫಲಿತಾಂಶ = Curl_urldecode(custom, 0, &smtp->custom, ಖಾಲಿ, REJECT_CTRL);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_parse_address()
 *
 * Parse the fully qualified mailbox address into a local address part and the
 * hostname, converting the hostname to an IDN A-label, as per RFC-5890, if
 * necessary.
 *
 * Parameters:
 *
 * fqma           [in]     - The fully qualified mailbox address (which may or
 *                           may not contain UTF-8 characters).
 * address        [in/out] - A new allocated buffer which holds the local
 *                           address part of the mailbox. This buffer must be
 *                           free'ed by the caller.
 * host           [in/out] - The hostname structure that holds the original,
 *                           and optionally encoded, hostname.
 *                           Curl_free_idnconverted_hostname() must be called
 *                           once the caller has finished with the structure.
 *
 * Returns CURLE_OK on success.
 *
 * Notes:
 *
 * Should a UTF-8 hostname require conversion to IDN ACE and we cannot honor
 * that conversion then we shall return success. This allow the caller to send
 * the data to the server as a U-label (as per RFC-6531 sect. 3.2).
 *
 * If an mailbox '@' separator cannot be located then the mailbox is considered
 * to be either a local mailbox or an invalid mailbox (depending on what the
 * calling function deems it to be) then the input will be returned in
 * the address part with the hostname being NULL.
 */
ಸ್ಥಿರ CURLcode smtp_parse_ವಿಳಾಸ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fqma,
                                   ಅಕ್ಷರ **ವಿಳಾಸ, ರಚನೆ hostname *host,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **suffix)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t ಉದ್ದಳತೆ;
  ಅಕ್ಷರ *addressend;
  ಅಕ್ಷರ *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  ಆದರೆ(strpbrk(fqma, "\r\n")) {
    failf(ದತ್ತಾಂಶ, "Refusing to send email address with a CR or LF");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  ಆದರೆ(!dup)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(fqma[0] != '<') {
    ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(dup);
    ಆದರೆ(ಉದ್ದಳತೆ && dup[ಉದ್ದಳತೆ - 1] == '>')
      dup[ಉದ್ದಳತೆ - 1] = '\0';
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    addressend = strrchr(dup, '>');
    ಆದರೆ(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  ಆದರೆ(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (ಶೂನ್ಯ)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *ವಿಳಾಸ = dup;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ cr_eob_ctx {
  ರಚನೆ Curl_creader super;
  ರಚನೆ bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  ಬಿಟ್(read_eos);  /* we read an EOS from the next reader */
  ಬಿಟ್(processed_eos);  /* we read and processed an EOS */
  ಬಿಟ್(eos);       /* we have returned an EOS */
};

ಸ್ಥಿರ CURLcode cr_eob_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_eob_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_eob_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_creader *reader)
{
  ರಚನೆ cr_eob_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#ವ್ಯಾಖ್ಯಾನಿಸು SMTP_EOB          "\r\n.\r\n"
#ವ್ಯಾಖ್ಯಾನಿಸು SMTP_EOB_ಕಂಡುಹಿಡಿ_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ಸ್ಥಿರ CURLcode cr_eob_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ Curl_creader *reader,
                            ಅಕ್ಷರ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ರಚನೆ cr_eob_ctx *ctx = reader->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread, i, ಆರಂಭ, n;
  bool eos;

  ಆದರೆ(!ctx->read_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf)) {
    /* Get more and convert it when needed */
    ಫಲಿತಾಂಶ = Curl_creader_read(ದತ್ತಾಂಶ, reader->ಮುಂದಿನ, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(ದತ್ತಾಂಶ, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread, eos);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ctx->read_eos = eos;
    ಆದರೆ(nread) {
      ಆದರೆ(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      ಗಾಗಿ(i = ಆರಂಭ = 0; i < nread; ++i) {
        ಆದರೆ(ctx->n_eob >= SMTP_EOB_ಕಂಡುಹಿಡಿ_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, buf + ಆರಂಭ, i - ಆರಂಭ, &n);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
          ctx->n_eob = 0;
          ಆರಂಭ = i;
          ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize > 0)
            ದತ್ತಾಂಶ->state.infilesize++;
        }

        ಆದರೆ(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        ಆದರೆ(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      ಆದರೆ(ಆರಂಭ < nread) {
        ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, buf + ಆರಂಭ, nread - ಆರಂಭ, &n);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }

  *peos = FALSE;

  ಆದರೆ(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *eob = SMTP_EOB;
    CURL_TRC_SMTP(ದತ್ತಾಂಶ, "auto-ending mail body with '\\r\\n.\\r\\n'");
    ಆಯ್ಕೆ(ctx->n_eob) {
    ಸ್ಥಿತಿ 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
    ಫಲಿತಾಂಶ = Curl_bufq_cwrite(&ctx->buf, eob, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(eob), &n);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ctx->processed_eos = TRUE;
  }

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf)) {
    ಫಲಿತಾಂಶ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    *pnread = 0;

  ಆದರೆ(ctx->read_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(ದತ್ತಾಂಶ, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(ದತ್ತಾಂಶ, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread, *peos));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ curl_off_t cr_eob_ಒಟ್ಟು_ಉದ್ದಳತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)reader;
  ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_ಒಟ್ಟು_ಉದ್ದಳತೆ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ಮುಗಿದ,
  ಗಾತ್ರ(ರಚನೆ cr_eob_ctx)
};

ಸ್ಥಿರ CURLcode cr_eob_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_creader *reader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_creader_create(&reader, ದತ್ತಾಂಶ, &cr_eob, CURL_CR_CONTENT_ENCODE);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_creader_ಕೂಡಿಸು(ದತ್ತಾಂಶ, reader);

  ಆದರೆ(ಫಲಿತಾಂಶ && reader)
    Curl_creader_free(ದತ್ತಾಂಶ, reader);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ಸ್ಥಿರ bool smtp_endofresp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಂಕ *resp)
{
  ರಚನೆ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool ಕೊನೆ = FALSE;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  DEBUGASSERT(smtpc);
  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ FALSE;

  /* Nothing for us */
  ಆದರೆ(len < 4 || !ISDIGIT(ಸಾಲು[0]) || !ISDIGIT(ಸಾಲು[1]) || !ISDIGIT(ಸಾಲು[2]))
    ಹಿಂತಿರುಗಿ FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  ಆದರೆ(ಸಾಲು[3] == ' ' || len == 5) {
    ಅಕ್ಷರ tmpline[6];
    curl_off_t ಸಂಕೇತ_2;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = tmpline;
    ಕೊನೆ = TRUE;
    ಸ್ಮರಣೆ_ನಕಲು(tmpline, ಸಾಲು, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಸಂಕೇತ_2, len == 5 ? 99999 : 999))
      ಹಿಂತಿರುಗಿ FALSE;
    *resp = (ಪೂರ್ಣಾಂಕ)ಸಂಕೇತ_2;

    /* Make sure real server never sends internal value */
    ಆದರೆ(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸಾಲು[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    ಕೊನೆ = TRUE;
    *resp = 1;  /* Internal response code */
  }

  ಹಿಂತಿರುಗಿ ಕೊನೆ;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ಸ್ಥಿರ CURLcode smtp_get_ಸಂದೇಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ bufref *ಹೊರಹೋಗು)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);
  ಅಕ್ಷರ *ಸಂದೇಶ_2;
  size_t len;

  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಸಂದೇಶ_2 = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  ಆದರೆ(len > 4) {
    /* Find the start of the message */
    len -= 4;
    ಗಾಗಿ(ಸಂದೇಶ_2 += 4; ISBLANK(*ಸಂದೇಶ_2); ಸಂದೇಶ_2++, len--)
      ;

    /* Find the end of the message */
    ಆಗ(len--)
      ಆದರೆ(!ISNEWLINE(ಸಂದೇಶ_2[len]) && !ISBLANK(ಸಂದೇಶ_2[len]))
        ನಿಲ್ಲಿಸು;

    /* Terminate the message */
    ಸಂದೇಶ_2[++len] = '\0';
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, ಸಂದೇಶ_2, len, ಖಾಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* junk input => zero length output */
    Curl_bufref_ಗಣ(ಹೊರಹೋಗು, "", 0, ಖಾಲಿ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ smtp_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ smtp_conn *smtpc,
                       smtpstate newstate)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  /* for debug purposes */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೆಸರುಗಳು[] = {
    "STOP",
    "SERVERGREET",
    "EHLO",
    "HELO",
    "STARTTLS",
    "UPGRADETLS",
    "AUTH",
    "COMMAND",
    "MAIL",
    "RCPT",
    "DATA",
    "POSTDATA",
    "QUIT",
    /*ಕೊನೆಯ*/
  };

  ಆದರೆ(smtpc->state != newstate)
    CURL_TRC_SMTP(ದತ್ತಾಂಶ, "state change from %s to %s",
                  ಹೆಸರುಗಳು[smtpc->state], ಹೆಸರುಗಳು[newstate]);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ಸ್ಥಿರ CURLcode smtp_perform_ehlo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ smtp_conn *smtpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "EHLO %s", smtpc->domain);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_EHLO);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ಸ್ಥಿರ CURLcode smtp_perform_helo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ smtp_conn *smtpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "HELO %s", smtpc->domain);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_HELO);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode smtp_perform_starttls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "%s", "STARTTLS");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STARTTLS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode smtp_perform_upgrade_tls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ smtp_conn *smtpc)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  /* Start the SSL connection */
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  ಆದರೆ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ಫಲಿತಾಂಶ = Curl_ssl_cfilter_ಕೂಡಿಸು(
      ದತ್ತಾಂಶ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ದತ್ತಾಂಶ, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, ssldone));
  ಆದರೆ(!ಫಲಿತಾಂಶ && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    ಫಲಿತಾಂಶ = smtp_perform_ehlo(ದತ್ತಾಂಶ, smtpc);
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)smtpc;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ಸ್ಥಿರ CURLcode smtp_perform_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *initresp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ir = Curl_bufref_ptr(initresp);

  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Send the AUTH command */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "AUTH %s", mech);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ಸ್ಥಿರ CURLcode smtp_continue_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *resp)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ಸ್ಥಿರ CURLcode smtp_cancel_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mech)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);

  (ಶೂನ್ಯ)mech;
  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ಸ್ಥಿರ CURLcode smtp_perform_authentication(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ smtp_conn *smtpc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  saslprogress ಪ್ರಗತಿ;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  ಆದರೆ(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, ದತ್ತಾಂಶ)) {
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Calculate the SASL login details */
  ಫಲಿತಾಂಶ = Curl_sasl_ಆರಂಭ(&smtpc->sasl, ದತ್ತಾಂಶ, FALSE, &ಪ್ರಗತಿ);

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಪ್ರಗತಿ == SASL_INPROGRESS)
      smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_AUTH);
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_sasl_is_blocked(&smtpc->sasl, ದತ್ತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ಸ್ಥಿರ CURLcode smtp_perform_command(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ರಚನೆ SMTP *smtp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    ಆದರೆ(!smtp->custom || !smtp->custom[0]) {
      ಅಕ್ಷರ *ವಿಳಾಸ = ಖಾಲಿ;
      ರಚನೆ hostname host = { ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ };
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      ಫಲಿತಾಂಶ = smtp_parse_ವಿಳಾಸ(ದತ್ತಾಂಶ, smtp->rcpt->ದತ್ತಾಂಶ,
                                  &ವಿಳಾಸ, &host, &suffix);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(ವಿಳಾಸ) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "VRFY %s%s%s%s",
                             ವಿಳಾಸ,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(ವಿಳಾಸ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->ದತ್ತಾಂಶ,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Send the non-recipient based command such as HELP */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_COMMAND);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ಸ್ಥಿರ CURLcode smtp_perform_mail(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ smtp_conn *smtpc,
                                  ರಚನೆ SMTP *smtp)
{
  ಅಕ್ಷರ *from = ಖಾಲಿ;
  ಅಕ್ಷರ *auth = ಖಾಲಿ;
  ಅಕ್ಷರ *ಗಾತ್ರ_2 = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_MAIL_FROM);
  ಆದರೆ(str) {
    ಅಕ್ಷರ *ವಿಳಾಸ = ಖಾಲಿ;
    ರಚನೆ hostname host = { ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ };
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    ಫಲಿತಾಂಶ = smtp_parse_ವಿಳಾಸ(ದತ್ತಾಂಶ, str, &ವಿಳಾಸ, &host, &suffix);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(ವಿಳಾಸ) ||
            !Curl_is_ASCII_name(host.name));

    ಆದರೆ(host.name) {
      from = curl_maprintf("<%s@%s>%s", ವಿಳಾಸ, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", ವಿಳಾಸ, suffix);

    curlx_free(ವಿಳಾಸ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  ಆದರೆ(!from) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_MAIL_AUTH);
  ಆದರೆ(str && smtpc->sasl.authused) {
    ಆದರೆ(str[0] != '\0') {
      ಅಕ್ಷರ *ವಿಳಾಸ = ಖಾಲಿ;
      ರಚನೆ hostname host = { ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ };
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      ಫಲಿತಾಂಶ = smtp_parse_ವಿಳಾಸ(ದತ್ತಾಂಶ, str, &ವಿಳಾಸ, &host, &suffix);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      ಆದರೆ(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(ವಿಳಾಸ) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      ಆದರೆ(host.name) {
        auth = curl_maprintf("<%s@%s>%s", ವಿಳಾಸ, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", ವಿಳಾಸ, suffix);
      curlx_free(ವಿಳಾಸ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    ಆದರೆ(!auth) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ಆದರೆ(IS_MIME_POST(ದತ್ತಾಂಶ)) {
    curl_mimepart *postp = ದತ್ತಾಂಶ->ಗಣ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_ಶೀರ್ಷಿಕೆಗಳು(postp, ದತ್ತಾಂಶ->ಗಣ.ಶೀರ್ಷಿಕೆಗಳು, 0);
    ಫಲಿತಾಂಶ = Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ದತ್ತಾಂಶ, postp, ಖಾಲಿ,
                                       ಖಾಲಿ, MIMESTRATEGY_MAIL);

    ಆದರೆ(!ಫಲಿತಾಂಶ && !Curl_checkheaders(ದತ್ತಾಂಶ, STRCONST("Mime-Version")))
      ಫಲಿತಾಂಶ = Curl_mime_ಕೂಡಿಸು_ಶೀರ್ಷಿಕೆ(&postp->curlheaders, "Mime-Version: 1.0");
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_mime(ದತ್ತಾಂಶ, postp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ದತ್ತಾಂಶ->state.infilesize = Curl_creader_ಒಟ್ಟು_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  {
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_fread(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Calculate the optional SIZE parameter */
  ಆದರೆ(smtpc->ಗಾತ್ರ_supported && ದತ್ತಾಂಶ->state.infilesize > 0) {
    ಗಾತ್ರ_2 = curl_maprintf("%" FMT_OFF_T, ದತ್ತಾಂಶ->state.infilesize);

    ಆದರೆ(!ಗಾತ್ರ_2) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  ಆದರೆ(smtpc->utf8_supported && !utf8) {
    ರಚನೆ curl_slist *rcpt = smtp->rcpt;

    ಆಗ(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      ಆದರೆ(!Curl_is_ASCII_name(rcpt->ದತ್ತಾಂಶ))
        utf8 = TRUE;

      rcpt = rcpt->ಮುಂದಿನ;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  ಫಲಿತಾಂಶ = cr_eob_ಕೂಡಿಸು(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Send the MAIL command */
  ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         ಗಾತ್ರ_2 ? " SIZE=" : "", /* Optional on SIZE support  */
                         ಗಾತ್ರ_2 ? ಗಾತ್ರ_2 : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

ಹೊರಹೋಗು:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(ಗಾತ್ರ_2);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_MAIL);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ಸ್ಥಿರ CURLcode smtp_perform_rcpt_ಗಮ್ಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ರಚನೆ SMTP *smtp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ *ವಿಳಾಸ = ಖಾಲಿ;
  ರಚನೆ hostname host = { ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ };
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  ಫಲಿತಾಂಶ = smtp_parse_ವಿಳಾಸ(ದತ್ತಾಂಶ, smtp->rcpt->ದತ್ತಾಂಶ,
                              &ವಿಳಾಸ, &host, &suffix);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Send the RCPT TO command */
  ಆದರೆ(host.name)
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           ವಿಳಾಸ, host.name, suffix);
  ಇಲ್ಲದಿದ್ದರೆ
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "RCPT TO:<%s>%s",
                           ವಿಳಾಸ, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(ವಿಳಾಸ);

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_RCPT);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ಸ್ಥಿರ CURLcode smtp_perform_quit(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "%s", "QUIT");

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_QUIT);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the initial server greeting */
ಸ್ಥಿರ CURLcode smtp_state_servergreet_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ smtp_conn *smtpc,
                                            ಪೂರ್ಣಾಂಕ smtpcode,
                                            smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(smtpcode / 100 != 2) {
    failf(ದತ್ತಾಂಶ, "Got unexpected smtp-server response: %d", smtpcode);
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = smtp_perform_ehlo(ದತ್ತಾಂಶ, smtpc);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For STARTTLS responses */
ಸ್ಥಿರ CURLcode smtp_state_starttls_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ smtp_conn *smtpc,
                                         ಪೂರ್ಣಾಂಕ smtpcode,
                                         smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  /* Pipelining in response is forbidden. */
  ಆದರೆ(smtpc->pp.ಅತಿಪ್ರವಾಹ)
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;

  ಆದರೆ(smtpcode != 220) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl != CURLUSESSL_TRY) {
      failf(ದತ್ತಾಂಶ, "STARTTLS denied, code %d", smtpcode);
      ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = smtp_perform_authentication(ದತ್ತಾಂಶ, smtpc);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_UPGRADETLS);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For EHLO responses */
ಸ್ಥಿರ CURLcode smtp_state_ehlo_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಂಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ಶೂನ್ಯ)instate;

  ಆದರೆ(smtpcode / 100 != 2 && smtpcode != 1) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(ದತ್ತಾಂಶ->conn, FIRSTSOCKET))
      ಫಲಿತಾಂಶ = smtp_perform_helo(ದತ್ತಾಂಶ, smtpc);
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "Remote access denied: %d", smtpcode);
      ಫಲಿತಾಂಶ = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 4) {
    ಸಾಲು += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    ಆದರೆ(len >= 8 && curl_strnequal(ಸಾಲು, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 4 && curl_strnequal(ಸಾಲು, "SIZE", 4))
      smtpc->ಗಾತ್ರ_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 8 && curl_strnequal(ಸಾಲು, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len >= 5 && curl_strnequal(ಸಾಲು, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      ಸಾಲು += 5;
      len -= 5;

      /* Loop through the data line */
      ಗಾಗಿ(;;) {
        size_t llen;
        size_t wordlen;
        ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ mechbit;

        ಆಗ(len && (ISBLANK(*ಸಾಲು) || ISNEWLINE(*ಸಾಲು))) {
          ಸಾಲು++;
          len--;
        }

        ಆದರೆ(!len)
          ನಿಲ್ಲಿಸು;

        /* Extract the word */
        ಗಾಗಿ(wordlen = 0; wordlen < len && !ISBLANK(ಸಾಲು[wordlen]) &&
              !ISNEWLINE(ಸಾಲು[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ಸಾಲು, wordlen, &llen);
        ಆದರೆ(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        ಸಾಲು += wordlen;
        len -= wordlen;
      }
    }

    ಆದರೆ(smtpcode != 1) {
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl && !Curl_conn_is_ssl(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        ಆದರೆ(smtpc->tls_supported)
          /* Switch to TLS connection now */
          ಫಲಿತಾಂಶ = smtp_perform_starttls(ದತ್ತಾಂಶ, smtpc);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          ಫಲಿತಾಂಶ = smtp_perform_authentication(ದತ್ತಾಂಶ, smtpc);
        ಇಲ್ಲದಿದ್ದರೆ {
          failf(ದತ್ತಾಂಶ, "STARTTLS not supported.");
          ಫಲಿತಾಂಶ = CURLE_USE_SSL_FAILED;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = smtp_perform_authentication(ದತ್ತಾಂಶ, smtpc);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Unexpectedly short EHLO response");
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For HELO responses */
ಸ್ಥಿರ CURLcode smtp_state_helo_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಂಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(smtpcode / 100 != 2) {
    failf(ದತ್ತಾಂಶ, "Remote access denied: %d", smtpcode);
    ಫಲಿತಾಂಶ = CURLE_REMOTE_ACCESS_DENIED;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* End of connect phase */
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For SASL authentication responses */
ಸ್ಥಿರ CURLcode smtp_state_auth_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಂಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  saslprogress ಪ್ರಗತಿ;

  (ಶೂನ್ಯ)instate;

  ಫಲಿತಾಂಶ = Curl_sasl_continue(&smtpc->sasl, ದತ್ತಾಂಶ, smtpcode, &ಪ್ರಗತಿ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಆಯ್ಕೆ(ಪ್ರಗತಿ) {
    ಸ್ಥಿತಿ SASL_ಮುಗಿದ:
      smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);  /* Authenticated */
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SASL_IDLE:            /* No mechanism left after cancellation */
      failf(ದತ್ತಾಂಶ, "Authentication cancelled");
      ಫಲಿತಾಂಶ = CURLE_LOGIN_DENIED;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For command responses */
ಸ್ಥಿರ CURLcode smtp_state_command_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ smtp_conn *smtpc,
                                        ರಚನೆ SMTP *smtp,
                                        ಪೂರ್ಣಾಂಕ smtpcode,
                                        smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ಶೂನ್ಯ)instate;

  ಆದರೆ((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(ದತ್ತಾಂಶ, "Command failed: %d", smtpcode);
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!ದತ್ತಾಂಶ->req.no_body)
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, ಸಾಲು, len);

    ಆದರೆ(!ಫಲಿತಾಂಶ && (smtpcode != 1)) {
      ಆದರೆ(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->ಮುಂದಿನ;

        ಆದರೆ(smtp->rcpt) {
          /* Send the next command */
          ಫಲಿತಾಂಶ = smtp_perform_command(ದತ್ತಾಂಶ, smtpc, smtp);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          /* End of DO phase */
          smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* End of DO phase */
        smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For MAIL responses */
ಸ್ಥಿರ CURLcode smtp_state_mail_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ರಚನೆ SMTP *smtp,
                                     ಪೂರ್ಣಾಂಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(smtpcode / 100 != 2) {
    failf(ದತ್ತಾಂಶ, "MAIL failed: %d", smtpcode);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Start the RCPT TO command */
    ಫಲಿತಾಂಶ = smtp_perform_rcpt_ಗಮ್ಯ(ದತ್ತಾಂಶ, smtpc, smtp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For RCPT responses */
ಸ್ಥಿರ CURLcode smtp_state_rcpt_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ರಚನೆ SMTP *smtp,
                                     ಪೂರ್ಣಾಂಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (ಶೂನ್ಯ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !ದತ್ತಾಂಶ->ಗಣ.mail_rcpt_allowfails);

  ಆದರೆ(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_ಕೊನೆಯ_error = smtpcode;

    ಆದರೆ(is_smtp_blocking_err) {
      failf(ದತ್ತಾಂಶ, "RCPT failed: %d", smtpcode);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  ಆದರೆ(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->ಮುಂದಿನ;

    ಆದರೆ(smtp->rcpt)
      /* Send the next RCPT TO command */
      ಫಲಿತಾಂಶ = smtp_perform_rcpt_ಗಮ್ಯ(ದತ್ತಾಂಶ, smtpc, smtp);
    ಇಲ್ಲದಿದ್ದರೆ {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      ಆದರೆ(!smtp->rcpt_had_ok) {
        failf(ದತ್ತಾಂಶ, "RCPT failed: %d (last error)", smtp->rcpt_ಕೊನೆಯ_error);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Send the DATA command */
        ಫಲಿತಾಂಶ = Curl_pp_sendf(ದತ್ತಾಂಶ, &smtpc->pp, "%s", "DATA");

        ಆದರೆ(!ಫಲಿತಾಂಶ)
          smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_ದತ್ತಾಂಶ);
      }
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For DATA response */
ಸ್ಥಿರ CURLcode smtp_state_ದತ್ತಾಂಶ_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಂಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಆದರೆ(smtpcode != 354) {
    failf(ದತ್ತಾಂಶ, "DATA failed: %d", smtpcode);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Set the progress upload size */
    Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);

    /* SMTP upload */
    Curl_xfer_ಸಿದ್ಧತೆ_send(ದತ್ತಾಂಶ, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ಸ್ಥಿರ CURLcode smtp_state_postdata_resp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ smtp_conn *smtpc,
                                         ಪೂರ್ಣಾಂಕ smtpcode,
                                         smtpstate instate)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  ಆದರೆ(smtpcode != 250)
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;

  /* End of DONE phase */
  smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode smtp_pp_statemachine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ smtpcode;
  ರಚನೆ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  ರಚನೆ SMTP *smtp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SMTP_EASY);
  size_t nread = 0;

  ಆದರೆ(!smtpc || !smtp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  ಆದರೆ(smtpc->state == SMTP_UPGRADETLS) {
    ಫಲಿತಾಂಶ = smtp_perform_upgrade_tls(ದತ್ತಾಂಶ, smtpc);
    ಆದರೆ(ಫಲಿತಾಂಶ || (smtpc->state == SMTP_UPGRADETLS))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Flush any data that needs to be sent */
  ಆದರೆ(smtpc->pp.sendleft)
    ಹಿಂತಿರುಗಿ Curl_pp_flushsend(ದತ್ತಾಂಶ, &smtpc->pp);

  ಮಾಡು {
    /* Read the response from the server */
    ಫಲಿತಾಂಶ = Curl_pp_readresp(ದತ್ತಾಂಶ, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* Store the latest response for later retrieval if necessary */
    ಆದರೆ(smtpc->state != SMTP_QUIT && smtpcode != 1)
      ದತ್ತಾಂಶ->info.httpcode = smtpcode;

    ಆದರೆ(!smtpcode)
      ನಿಲ್ಲಿಸು;

    /* We have now received a full SMTP server response */
    ಆಯ್ಕೆ(smtpc->state) {
    ಸ್ಥಿತಿ SMTP_SERVERGREET:
      ಫಲಿತಾಂಶ = smtp_state_servergreet_resp(ದತ್ತಾಂಶ, smtpc,
                                           smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_EHLO:
      ಫಲಿತಾಂಶ = smtp_state_ehlo_resp(ದತ್ತಾಂಶ, smtpc, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_HELO:
      ಫಲಿತಾಂಶ = smtp_state_helo_resp(ದತ್ತಾಂಶ, smtpc, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_STARTTLS:
      ಫಲಿತಾಂಶ = smtp_state_starttls_resp(ದತ್ತಾಂಶ, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && (smtpc->state == SMTP_UPGRADETLS))
        ಹೋಗು upgrade_tls;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_AUTH:
      ಫಲಿತಾಂಶ = smtp_state_auth_resp(ದತ್ತಾಂಶ, smtpc, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_COMMAND:
      ಫಲಿತಾಂಶ = smtp_state_command_resp(ದತ್ತಾಂಶ, smtpc, smtp,
                                       smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_MAIL:
      ಫಲಿತಾಂಶ = smtp_state_mail_resp(ದತ್ತಾಂಶ, smtpc, smtp, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_RCPT:
      ಫಲಿತಾಂಶ = smtp_state_rcpt_resp(ದತ್ತಾಂಶ, smtpc, smtp, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_ದತ್ತಾಂಶ:
      ಫಲಿತಾಂಶ = smtp_state_ದತ್ತಾಂಶ_resp(ದತ್ತಾಂಶ, smtpc, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_POSTDATA:
      ಫಲಿತಾಂಶ = smtp_state_postdata_resp(ದತ್ತಾಂಶ, smtpc, smtpcode, smtpc->state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SMTP_QUIT:
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* internal error */
      smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_STOP);
      ನಿಲ್ಲಿಸು;
    }
  } ಆಗ(!ಫಲಿತಾಂಶ && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode smtp_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &smtpc->pp, FALSE, FALSE);
  *ಮುಗಿದ = (smtpc->state == SMTP_STOP);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode smtp_ಖಂಡ_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(smtpc->state != SMTP_STOP && !ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_pp_statemach(ದತ್ತಾಂಶ, &smtpc->pp, TRUE, disconnecting);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode smtp_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);
  ಹಿಂತಿರುಗಿ smtpc ? Curl_pp_pollset(ದತ್ತಾಂಶ, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_ಸಂದೇಶ,     /* Get SASL response message */
  512 - 8,              /* Max line len - strlen("AUTH ") - 1 space - CRLF */
  334,                  /* Code received when continuation is expected */
  235,                  /* Code to receive upon authentication success */
  SASL_AUTH_DEFAULT,    /* Default mechanisms */
  SASL_FLAG_BASE64      /* Configuration flags */
};

/***********************************************************************
 *
 * smtp_connect()
 *
 * This function should do everything that is to be considered a part of
 * the connection phase.
 *
 * The variable pointed to by 'done' will be TRUE if the protocol-layer
 * connect phase is done when this function returns, or FALSE if not.
 */
ಸ್ಥಿರ CURLcode smtp_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *ಮುಗಿದ = FALSE; /* default to not done yet */
  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  PINGPONG_ಸಿದ್ಧತೆ(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, ದತ್ತಾಂಶ, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(ದತ್ತಾಂಶ));

  /* Parse the URL options */
  ಫಲಿತಾಂಶ = smtp_parse_url_options(ದತ್ತಾಂಶ->conn, smtpc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Parse the URL path */
  ಫಲಿತಾಂಶ = smtp_parse_url_ಮಾರ್ಗ(ದತ್ತಾಂಶ, smtpc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Start off waiting for the server greeting response */
  smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_SERVERGREET);

  ಫಲಿತಾಂಶ = smtp_multi_statemach(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_done()
 *
 * The DONE function. This does what needs to be done after a single DO has
 * performed.
 *
 * Input argument is already checked for validity.
 */
ಸ್ಥಿರ CURLcode smtp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                          bool premature)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ SMTP *smtp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SMTP_EASY);

  (ಶೂನ್ಯ)premature;

  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(!smtp)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  ಆದರೆ(ಸ್ಥಿತಿ_2) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    ಫಲಿತಾಂಶ = ಸ್ಥಿತಿ_2;         /* use the already set error code */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.connect_only && ದತ್ತಾಂಶ->ಗಣ.mail_rcpt &&
          (ದತ್ತಾಂಶ->state.upload || IS_MIME_POST(ದತ್ತಾಂಶ))) {

    smtp_state(ದತ್ತಾಂಶ, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    ಫಲಿತಾಂಶ = smtp_ಖಂಡ_statemach(ದತ್ತಾಂಶ, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_done(status=%d, premature=%d) -> %d",
                (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2, premature, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ಸ್ಥಿರ CURLcode smtp_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ smtp_conn *smtpc,
                             ರಚನೆ SMTP *smtp,
                             bool *connected,
                             bool *dophase_ಮುಗಿದ)
{
  /* This is SMTP and no proxy */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_perform(), start");

  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_ಮುಗಿದ = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = ದತ್ತಾಂಶ->ಗಣ.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_ಕೊನೆಯ_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  ಆದರೆ((ದತ್ತಾಂಶ->state.upload || IS_MIME_POST(ದತ್ತಾಂಶ)) && ದತ್ತಾಂಶ->ಗಣ.mail_rcpt)
    /* MAIL transfer */
    ಫಲಿತಾಂಶ = smtp_perform_mail(ದತ್ತಾಂಶ, smtpc, smtp);
  ಇಲ್ಲದಿದ್ದರೆ
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    ಫಲಿತಾಂಶ = smtp_perform_command(ದತ್ತಾಂಶ, smtpc, smtp);

  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Run the state-machine */
  ಫಲಿತಾಂಶ = smtp_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  *connected = Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, FIRSTSOCKET);

ಹೊರಹೋಗು:
  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_perform() -> %d, connected=%d, done=%d",
                (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *connected, *dophase_ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode smtp_dophase_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ SMTP *smtp,
                                  bool connected)
{
  (ಶೂನ್ಯ)connected;

  ಆದರೆ(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/***********************************************************************
 *
 * smtp_regular_transfer()
 *
 * The input argument is already checked for validity.
 *
 * Performs all commands done before a regular transfer between a local and a
 * remote host.
 */
ಸ್ಥಿರ CURLcode smtp_regular_transfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ smtp_conn *smtpc,
                                      ರಚನೆ SMTP *smtp,
                                      bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1;

  /* Set the progress data */
  Curl_pgrsReset(ದತ್ತಾಂಶ);

  /* Carry out the perform */
  ಫಲಿತಾಂಶ = smtp_perform(ದತ್ತಾಂಶ, smtpc, smtp, &connected, dophase_ಮುಗಿದ);

  /* Perform post DO phase operations if necessary */
  ಆದರೆ(!ಫಲಿತಾಂಶ && *dophase_ಮುಗಿದ)
    ಫಲಿತಾಂಶ = smtp_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, smtp, connected);

  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_regular_transfer() -> %d, done=%d",
                (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *dophase_ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_do()
 *
 * This function is registered as 'curl_do' function. It decodes the path
 * parts etc as a wrapper to the actual DO function (smtp_perform).
 *
 * The input argument is already checked for validity.
 */
ಸ್ಥಿರ CURLcode smtp_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ smtp_conn *smtpc =
    Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SMTP_CONN);
  ರಚನೆ SMTP *smtp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SMTP_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  *ಮುಗಿದ = FALSE; /* default to false */
  ಆದರೆ(!smtpc || !smtp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* Parse the custom request */
  ಫಲಿತಾಂಶ = smtp_parse_custom_request(ದತ್ತಾಂಶ, smtp);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = smtp_regular_transfer(ದತ್ತಾಂಶ, smtpc, smtp, ಮುಗಿದ);
  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_do() -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode smtp_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn,
                                bool dead_connection)
{
  ರಚನೆ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  ಆದರೆ(!smtpc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ಆದರೆ(!dead_connection && conn->ಬಿಟ್ಟುಗಳು.protoconnstart &&
     !Curl_pp_needs_flush(ದತ್ತಾಂಶ, &smtpc->pp) &&
     !smtp_perform_quit(ದತ್ತಾಂಶ, smtpc))
    (ಶೂನ್ಯ)smtp_ಖಂಡ_statemach(ದತ್ತಾಂಶ, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_disconnect(), finished");
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Called from multi.c while DOing */
ಸ್ಥಿರ CURLcode smtp_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *dophase_ಮುಗಿದ)
{
  ರಚನೆ SMTP *smtp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SMTP_EASY);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!smtp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = smtp_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase failed"));
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*dophase_ಮುಗಿದ) {
    ಫಲಿತಾಂಶ = smtp_dophase_ಮುಗಿದ(ದತ್ತಾಂಶ, smtp, FALSE /* not connected */);

    DEBUGF(infof(ದತ್ತಾಂಶ, "DO phase is complete"));
  }

  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_doing() -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                *dophase_ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ smtp_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ SMTP *smtp = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(smtp);
}

ಸ್ಥಿರ ಶೂನ್ಯ smtp_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ smtp_conn *smtpc = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ಸ್ಥಿರ CURLcode smtp_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ connectdata *conn)
{
  ರಚನೆ smtp_conn *smtpc;
  ರಚನೆ SMTP *smtp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  smtpc = curlx_calloc(1, ಗಾತ್ರ(*smtpc));
  ಆದರೆ(!smtpc ||
     Curl_conn_meta_ಗಣ(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  smtp = curlx_calloc(1, ಗಾತ್ರ(*smtp));
  ಆದರೆ(!smtp ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

ಹೊರಹೋಗು:
  CURL_TRC_SMTP(ದತ್ತಾಂಶ, "smtp_setup_connection() -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * SMTP protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_smtp = {
  smtp_ಸಿದ್ಧತೆ_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_ಮುಗಿದ,                        /* done */
  ZERO_NULL,                        /* do_more */
  smtp_connect,                     /* connect_it */
  smtp_multi_statemach,             /* connecting */
  smtp_doing,                       /* doing */
  smtp_pollset,                     /* proto_pollset */
  smtp_pollset,                     /* doing_pollset */
  ZERO_NULL,                        /* domore_pollset */
  ZERO_NULL,                        /* perform_pollset */
  smtp_disconnect,                  /* disconnect */
  ZERO_NULL,                        /* write_resp */
  ZERO_NULL,                        /* write_resp_hd */
  ZERO_NULL,                        /* connection_is_dead */
  ZERO_NULL,                        /* attach connection */
  ZERO_NULL,                        /* follow */
};

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_SMTP */
