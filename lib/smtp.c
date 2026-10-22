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

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

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
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
  SMTP_DATA,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_LAST         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
ಸಂರಚನಾ smtp_conn {
  ಸಂರಚನಾ pingpong pp;
  ಸಂರಚನಾ SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  ವರ್ಣ *domain;            /* Client address/name to send in the EHLO */
  BIT(ssldone);            /* Is connect() over SSL done? */
  BIT(tls_supported);      /* StartTLS capability supported by server */
  BIT(size_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  BIT(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  BIT(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ಸಂರಚನಾ SMTP {
  curl_pp_transfer transfer;
  ವರ್ಣ *custom;            /* Custom Request */
  ಸಂರಚನಾ curl_slist *rcpt; /* Recipient list */
  ಪೂರ್ಣಾಙ್ಕ rcpt_last_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  BIT(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  BIT(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
ಸ್ಥಿರ CURLcode smtp_parse_url_options(ಸಂರಚನಾ connectdata *conn,
                                       ಸಂರಚನಾ smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ptr = conn->options;

  ಯಾವತ್(!result && ptr && *ptr) {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = ptr;
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;

    ಯಾವತ್(*ptr && *ptr != '=')
      ptr++;

    ಮೌಲ್ಯ = ptr + 1;

    ಯಾವತ್(*ptr && *ptr != ';')
      ptr++;

    ಯದಿ(curl_strnequal(ಕೀಲಿ, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               ಮೌಲ್ಯ, ptr - ಮೌಲ್ಯ);
    ಅನ್ಯಥಾ
      result = CURLE_URL_MALFORMAT;

    ಯದಿ(*ptr == ';')
      ptr++;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ಸ್ಥಿರ CURLcode smtp_parse_url_path(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ಧ್ರುವ ವರ್ಣ *path = &data->state.up.path[1]; /* skip leading path */
  ವರ್ಣ localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  ಯದಿ(!*path) {
    ಯದಿ(!Curl_gethostname(localhost, ಪರಿಮಾಣ(localhost)))
      path = localhost;
    ಅನ್ಯಥಾ
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  ಪ್ರತ್ಯಾಗಮ Curl_urldecode(path, 0, &smtpc->domain, ರಿಕ್ತ, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ಸ್ಥಿರ CURLcode smtp_parse_custom_request(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  ಯದಿ(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, ರಿಕ್ತ, REJECT_CTRL);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode smtp_parse_address(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fqma,
                                   ವರ್ಣ **address, ಸಂರಚನಾ hostname *host,
                                   ಧ್ರುವ ವರ್ಣ **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  ವರ್ಣ *addressend;
  ವರ್ಣ *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  ಯದಿ(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  ಯದಿ(!dup)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(fqma[0] != '<') {
    length = strlen(dup);
    ಯದಿ(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  ಅನ್ಯಥಾ {
    addressend = strrchr(dup, '>');
    ಯದಿ(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  ಯದಿ(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (ಶೂನ್ಯ)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ cr_eob_ctx {
  ಸಂರಚನಾ Curl_creader super;
  ಸಂರಚನಾ bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

ಸ್ಥಿರ CURLcode cr_eob_init(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_eob_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_eob_close(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Curl_creader *reader)
{
  ಸಂರಚನಾ cr_eob_ctx *ctx = reader->ctx;
  (ಶೂನ್ಯ)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ಸ್ಥಿರ CURLcode cr_eob_read(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ Curl_creader *reader,
                            ವರ್ಣ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ಸಂರಚನಾ cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  ಯದಿ(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (ಪೂರ್ಣಾಙ್ಕ)result, nread, eos);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    ctx->read_eos = eos;
    ಯದಿ(nread) {
      ಯದಿ(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      ಪ್ರತಿ(i = start = 0; i < nread; ++i) {
        ಯದಿ(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
          ctx->n_eob = 0;
          start = i;
          ಯದಿ(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        ಯದಿ(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        ಯದಿ(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      ಯದಿ(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }

  *peos = FALSE;

  ಯದಿ(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ಧ್ರುವ ವರ್ಣ *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    ವಿಕಲ್ಪ(ctx->n_eob) {
    ಪಕ್ಷ 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      ಭಙ್ಗ;
    ಪಕ್ಷ 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ctx->processed_eos = TRUE;
  }

  ಯದಿ(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  ಅನ್ಯಥಾ
    *pnread = 0;

  ಯದಿ(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread, *peos));
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ curl_off_t cr_eob_total_length(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)reader;
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_total_length,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  ಪರಿಮಾಣ(ಸಂರಚನಾ cr_eob_ctx)
};

ಸ್ಥಿರ CURLcode cr_eob_add(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_creader *reader = ರಿಕ್ತ;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  ಯದಿ(!result)
    result = Curl_creader_add(data, reader);

  ಯದಿ(result && reader)
    Curl_creader_free(data, reader);
  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ಸ್ಥಿರ bool smtp_endofresp(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn,
                           ಧ್ರುವ ವರ್ಣ *ಸಾಲು, size_t len, ಪೂರ್ಣಾಙ್ಕ *resp)
{
  ಸಂರಚನಾ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (ಶೂನ್ಯ)data;

  DEBUGASSERT(smtpc);
  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* Nothing for us */
  ಯದಿ(len < 4 || !ISDIGIT(ಸಾಲು[0]) || !ISDIGIT(ಸಾಲು[1]) || !ISDIGIT(ಸಾಲು[2]))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  ಯದಿ(ಸಾಲು[3] == ' ' || len == 5) {
    ವರ್ಣ tmpline[6];
    curl_off_t code;
    ಧ್ರುವ ವರ್ಣ *p = tmpline;
    end = TRUE;
    memcpy(tmpline, ಸಾಲು, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &code, len == 5 ? 99999 : 999))
      ಪ್ರತ್ಯಾಗಮ FALSE;
    *resp = (ಪೂರ್ಣಾಙ್ಕ)code;

    /* Make sure real server never sends internal value */
    ಯದಿ(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  ಅನ್ಯಥಾ ಯದಿ(ಸಾಲು[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  ಪ್ರತ್ಯಾಗಮ end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ಸ್ಥಿರ CURLcode smtp_get_message(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ bufref *out)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ವರ್ಣ *message;
  size_t len;

  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  ಯದಿ(len > 4) {
    /* Find the start of the message */
    len -= 4;
    ಪ್ರತಿ(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ಯಾವತ್(len--)
      ಯದಿ(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        ಭಙ್ಗ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ರಿಕ್ತ);
  }
  ಅನ್ಯಥಾ
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ರಿಕ್ತ);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ಸ್ಥಿರ ಶೂನ್ಯ smtp_state(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ names[] = {
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
    /* LAST */
  };

  ಯದಿ(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
#endif

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ಸ್ಥಿರ CURLcode smtp_perform_ehlo(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ಸ್ಥಿರ CURLcode smtp_perform_helo(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode smtp_perform_starttls(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ಸ್ಥಿರ CURLcode smtp_perform_upgrade_tls(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  ಯದಿ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (ಪೂರ್ಣಾಙ್ಕ)result, ssldone));
  ಯದಿ(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)smtpc;
  ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ಸ್ಥಿರ CURLcode smtp_perform_auth(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ವರ್ಣ *mech,
                                  ಧ್ರುವ ಸಂರಚನಾ bufref *initresp)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ಧ್ರುವ ವರ್ಣ *ir = Curl_bufref_ptr(initresp);

  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ಅನ್ಯಥಾ {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ಸ್ಥಿರ CURLcode smtp_continue_auth(ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ವರ್ಣ *mech,
                                   ಧ್ರುವ ಸಂರಚನಾ bufref *resp)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ಸ್ಥಿರ CURLcode smtp_cancel_auth(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *mech)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (ಶೂನ್ಯ)mech;
  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ಸ್ಥಿರ CURLcode smtp_perform_authentication(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  ಯದಿ(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  ಯದಿ(!result) {
    ಯದಿ(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    ಅನ್ಯಥಾ
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ಸ್ಥಿರ CURLcode smtp_perform_command(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಸಂರಚನಾ SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    ಯದಿ(!smtp->custom || !smtp->custom[0]) {
      ವರ್ಣ *address = ರಿಕ್ತ;
      ಸಂರಚನಾ hostname host = { ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ };
      ಧ್ರುವ ವರ್ಣ *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(address) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      result = Curl_pp_sendf(data, &smtpc->pp, "VRFY %s%s%s%s",
                             address,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(address);
    }
    ಅನ್ಯಥಾ {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      result = Curl_pp_sendf(data, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->data,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  ಅನ್ಯಥಾ
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ಸ್ಥಿರ CURLcode smtp_perform_mail(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ smtp_conn *smtpc,
                                  ಸಂರಚನಾ SMTP *smtp)
{
  ವರ್ಣ *from = ರಿಕ್ತ;
  ವರ್ಣ *auth = ರಿಕ್ತ;
  ವರ್ಣ *size = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  ಯದಿ(str) {
    ವರ್ಣ *address = ರಿಕ್ತ;
    ಸಂರಚನಾ hostname host = { ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ };
    ಧ್ರುವ ವರ್ಣ *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    ಯದಿ(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    ಅನ್ಯಥಾ
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  ಅನ್ಯಥಾ
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  ಯದಿ(!from) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  ಯದಿ(str && smtpc->sasl.authused) {
    ಯದಿ(str[0] != '\0') {
      ವರ್ಣ *address = ರಿಕ್ತ;
      ಸಂರಚನಾ hostname host = { ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ };
      ಧ್ರುವ ವರ್ಣ *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      ಯದಿ(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      ಯದಿ(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      ಅನ್ಯಥಾ
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    ಅನ್ಯಥಾ
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    ಯದಿ(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ಯದಿ(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned ಪೂರ್ಣಾಙ್ಕ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ರಿಕ್ತ,
                                       ರಿಕ್ತ, MIMESTRATEGY_MAIL);

    ಯದಿ(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    ಯದಿ(!result)
      result = Curl_creader_set_mime(data, postp);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  ಅನ್ಯಥಾ
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /* Calculate the optional SIZE parameter */
  ಯದಿ(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    ಯದಿ(!size) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  ಯದಿ(smtpc->utf8_supported && !utf8) {
    ಸಂರಚನಾ curl_slist *rcpt = smtp->rcpt;

    ಯಾವತ್(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      ಯದಿ(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* Send the MAIL command */
  result = Curl_pp_sendf(data, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         size ? " SIZE=" : "", /* Optional on SIZE support  */
                         size ? size : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

out:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(size);

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ಸ್ಥಿರ CURLcode smtp_perform_rcpt_to(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಸಂರಚನಾ SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ವರ್ಣ *address = ರಿಕ್ತ;
  ಸಂರಚನಾ hostname host = { ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ };
  ಧ್ರುವ ವರ್ಣ *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Send the RCPT TO command */
  ಯದಿ(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  ಅನ್ಯಥಾ
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ಸ್ಥಿರ CURLcode smtp_perform_quit(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  ಯದಿ(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the initial server greeting */
ಸ್ಥಿರ CURLcode smtp_state_servergreet_resp(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ smtp_conn *smtpc,
                                            ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  ಅನ್ಯಥಾ
    result = smtp_perform_ehlo(data, smtpc);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For STARTTLS responses */
ಸ್ಥಿರ CURLcode smtp_state_starttls_resp(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ smtp_conn *smtpc,
                                         ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  /* Pipelining in response is forbidden. */
  ಯದಿ(smtpc->pp.overflow)
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;

  ಯದಿ(smtpcode != 220) {
    ಯದಿ(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    ಅನ್ಯಥಾ
      result = smtp_perform_authentication(data, smtpc);
  }
  ಅನ್ಯಥಾ
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For EHLO responses */
ಸ್ಥಿರ CURLcode smtp_state_ehlo_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ಶೂನ್ಯ)instate;

  ಯದಿ(smtpcode / 100 != 2 && smtpcode != 1) {
    ಯದಿ(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    ಅನ್ಯಥಾ {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(len >= 4) {
    ಸಾಲು += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    ಯದಿ(len >= 8 && curl_strnequal(ಸಾಲು, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    ಅನ್ಯಥಾ ಯದಿ(len >= 4 && curl_strnequal(ಸಾಲು, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    ಅನ್ಯಥಾ ಯದಿ(len >= 8 && curl_strnequal(ಸಾಲು, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    ಅನ್ಯಥಾ ಯದಿ(len >= 5 && curl_strnequal(ಸಾಲು, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      ಸಾಲು += 5;
      len -= 5;

      /* Loop through the data line */
      ಪ್ರತಿ(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        ಯಾವತ್(len && (ISBLANK(*ಸಾಲು) || ISNEWLINE(*ಸಾಲು))) {
          ಸಾಲು++;
          len--;
        }

        ಯದಿ(!len)
          ಭಙ್ಗ;

        /* Extract the word */
        ಪ್ರತಿ(wordlen = 0; wordlen < len && !ISBLANK(ಸಾಲು[wordlen]) &&
              !ISNEWLINE(ಸಾಲು[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ಸಾಲು, wordlen, &llen);
        ಯದಿ(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        ಸಾಲು += wordlen;
        len -= wordlen;
      }
    }

    ಯದಿ(smtpcode != 1) {
      ಯದಿ(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        ಯದಿ(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        ಅನ್ಯಥಾ ಯದಿ(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        ಅನ್ಯಥಾ {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      ಅನ್ಯಥಾ
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  ಅನ್ಯಥಾ {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For HELO responses */
ಸ್ಥಿರ CURLcode smtp_state_helo_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  ಅನ್ಯಥಾ
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For SASL authentication responses */
ಸ್ಥಿರ CURLcode smtp_state_auth_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (ಶೂನ್ಯ)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  ಯದಿ(!result)
    ವಿಕಲ್ಪ(progress) {
    ಪಕ್ಷ SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      ಭಙ್ಗ;
    ಪಕ್ಷ SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For command responses */
ಸ್ಥಿರ CURLcode smtp_state_command_resp(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ smtp_conn *smtpc,
                                        ಸಂರಚನಾ SMTP *smtp,
                                        ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ಸಾಲು = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ಶೂನ್ಯ)instate;

  ಯದಿ((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ಸಾಲು, len);

    ಯದಿ(!result && (smtpcode != 1)) {
      ಯದಿ(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        ಯದಿ(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        ಅನ್ಯಥಾ
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      ಅನ್ಯಥಾ
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For MAIL responses */
ಸ್ಥಿರ CURLcode smtp_state_mail_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಸಂರಚನಾ SMTP *smtp,
                                     ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  ಅನ್ಯಥಾ
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For RCPT responses */
ಸ್ಥಿರ CURLcode smtp_state_rcpt_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಸಂರಚನಾ SMTP *smtp,
                                     ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (ಶೂನ್ಯ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  ಯದಿ(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    ಯದಿ(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  ಅನ್ಯಥಾ {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  ಯದಿ(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    ಯದಿ(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    ಅನ್ಯಥಾ {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      ಯದಿ(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      ಅನ್ಯಥಾ {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        ಯದಿ(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For DATA response */
ಸ್ಥಿರ CURLcode smtp_state_data_resp(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)instate;

  ಯದಿ(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  ಅನ್ಯಥಾ {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ಸ್ಥಿರ CURLcode smtp_state_postdata_resp(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ smtp_conn *smtpc,
                                         ಪೂರ್ಣಾಙ್ಕ smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)instate;

  ಯದಿ(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode smtp_pp_statemachine(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ smtpcode;
  ಸಂರಚನಾ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  ಸಂರಚನಾ SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  ಯದಿ(!smtpc || !smtp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  ಯದಿ(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    ಯದಿ(result || (smtpc->state == SMTP_UPGRADETLS))
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Flush any data that needs to be sent */
  ಯದಿ(smtpc->pp.sendleft)
    ಪ್ರತ್ಯಾಗಮ Curl_pp_flushsend(data, &smtpc->pp);

  ಕುರು {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    /* Store the latest response for later retrieval if necessary */
    ಯದಿ(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    ಯದಿ(!smtpcode)
      ಭಙ್ಗ;

    /* We have now received a full SMTP server response */
    ವಿಕಲ್ಪ(smtpc->state) {
    ಪಕ್ಷ SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ಯದಿ(!result && (smtpc->state == SMTP_UPGRADETLS))
        ಗಚ್ಛ upgrade_tls;
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      ಭಙ್ಗ;

    ಪಕ್ಷ SMTP_QUIT:
    ಸ್ವಭಾವ:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      ಭಙ್ಗ;
    }
  } ಯಾವತ್(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  ಪ್ರತ್ಯಾಗಮ result;
}

/* Called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode smtp_multi_statemach(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode smtp_block_statemach(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  ಯಾವತ್(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ಸ್ಥಿರ CURLcode smtp_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ಪ್ರತ್ಯಾಗಮ smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_message,     /* Get SASL response message */
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
ಸ್ಥಿರ CURLcode smtp_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode smtp_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                          bool premature)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (ಶೂನ್ಯ)premature;

  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(!smtp)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  ಯದಿ(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  ಅನ್ಯಥಾ ಯದಿ(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (ಪೂರ್ಣಾಙ್ಕ)status, premature, (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ಸ್ಥಿರ CURLcode smtp_perform(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ smtp_conn *smtpc,
                             ಸಂರಚನಾ SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  ಯದಿ(data->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = data->set.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_last_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  ಯದಿ((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  ಅನ್ಯಥಾ
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (ಪೂರ್ಣಾಙ್ಕ)result, *connected, *dophase_done);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Call this when the DO phase has completed */
ಸ್ಥಿರ CURLcode smtp_dophase_done(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ SMTP *smtp,
                                  bool connected)
{
  (ಶೂನ್ಯ)connected;

  ಯದಿ(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಸ್ಥಿರ CURLcode smtp_regular_transfer(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ smtp_conn *smtpc,
                                      ಸಂರಚನಾ SMTP *smtp,
                                      bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  data->req.size = -1;

  /* Set the progress data */
  Curl_pgrsReset(data);

  /* Carry out the perform */
  result = smtp_perform(data, smtpc, smtp, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  ಯದಿ(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (ಪೂರ್ಣಾಙ್ಕ)result, *dophase_done);
  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode smtp_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ಸಂರಚನಾ SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  ಯದಿ(!smtpc || !smtp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ಸ್ಥಿರ CURLcode smtp_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn,
                                bool dead_connection)
{
  ಸಂರಚನಾ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  ಯದಿ(!smtpc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ಯದಿ(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (ಶೂನ್ಯ)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Called from multi.c while DOing */
ಸ್ಥಿರ CURLcode smtp_doing(ಸಂರಚನಾ Curl_easy *data, bool *dophase_done)
{
  ಸಂರಚನಾ SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  ಯದಿ(!smtp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  ಯದಿ(result)
    DEBUGF(infof(data, "DO phase failed"));
  ಅನ್ಯಥಾ ಯದಿ(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result,
                *dophase_done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ smtp_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ SMTP *smtp = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_free(smtp);
}

ಸ್ಥಿರ ಶೂನ್ಯ smtp_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ smtp_conn *smtpc = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ಸ್ಥಿರ CURLcode smtp_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ smtp_conn *smtpc;
  ಸಂರಚನಾ SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, ಪರಿಮಾಣ(*smtpc));
  ಯದಿ(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  smtp = curlx_calloc(1, ಪರಿಮಾಣ(*smtp));
  ಯದಿ(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * SMTP protocol handler.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_smtp = {
  smtp_setup_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_done,                        /* done */
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

#endif /* CURL_DISABLE_SMTP */
