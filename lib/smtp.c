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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "smtp.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pingpong.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_gethostname.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sasl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "idn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
  SMTP_ਅੰਕੜੇ,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_ਆਖਰੀ         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
ਬਣਤਰ smtp_conn {
  ਬਣਤਰ pingpong pp;
  ਬਣਤਰ SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  ਅੱਖਰ *domain;            /* Client address/name to send in the EHLO */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ssldone);            /* Is connect() over SSL done? */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tls_supported);      /* StartTLS capability supported by server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ਬਣਤਰ SMTP {
  curl_pp_transfer transfer;
  ਅੱਖਰ *custom;            /* Custom Request */
  ਬਣਤਰ curl_slist *rcpt; /* Recipient list */
  ਪੂਰਨ rcpt_ਆਖਰੀ_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  ਦੋਆਧਾਰੀ_ਅੰਕ(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  ਦੋਆਧਾਰੀ_ਅੰਕ(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
ਸਥਿਰ CURLcode smtp_parse_url_options(ਬਣਤਰ connectdata *conn,
                                       ਬਣਤਰ smtp_conn *smtpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ptr = conn->options;

  ਜਦੋਂ(!ਨਤੀਜਾ && ptr && *ptr) {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ = ptr;
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;

    ਜਦੋਂ(*ptr && *ptr != '=')
      ptr++;

    ਮੁੱਲ = ptr + 1;

    ਜਦੋਂ(*ptr && *ptr != ';')
      ptr++;

    ਜੇ(curl_strnequal(ਕੁੰਜੀ, "AUTH=", 5))
      ਨਤੀਜਾ = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               ਮੁੱਲ, ptr - ਮੁੱਲ);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;

    ਜੇ(*ptr == ';')
      ptr++;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ਸਥਿਰ CURLcode smtp_parse_url_ਰਸਤਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = &ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ[1]; /* skip leading path */
  ਅੱਖਰ localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  ਜੇ(!*ਰਸਤਾ) {
    ਜੇ(!Curl_gethostname(localhost, ਆਕਾਰ(localhost)))
      ਰਸਤਾ = localhost;
    ਨਹੀਂਤਾਂ
      ਰਸਤਾ = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  ਵਾਪਸ Curl_urldecode(ਰਸਤਾ, 0, &smtpc->domain, ਨਿਰਾ, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ਸਥਿਰ CURLcode smtp_parse_custom_request(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ SMTP *smtp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *custom = CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  ਜੇ(custom)
    ਨਤੀਜਾ = Curl_urldecode(custom, 0, &smtp->custom, ਨਿਰਾ, REJECT_CTRL);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode smtp_parse_ਪਤਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fqma,
                                   ਅੱਖਰ **ਪਤਾ, ਬਣਤਰ hostname *host,
                                   ਅਚਲ ਅੱਖਰ **suffix)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t ਲੰਮਾਈ;
  ਅੱਖਰ *addressend;
  ਅੱਖਰ *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  ਜੇ(strpbrk(fqma, "\r\n")) {
    failf(ਅੰਕੜੇ, "Refusing to send email address with a CR or LF");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  ਜੇ(!dup)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(fqma[0] != '<') {
    ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(dup);
    ਜੇ(ਲੰਮਾਈ && dup[ਲੰਮਾਈ - 1] == '>')
      dup[ਲੰਮਾਈ - 1] = '\0';
  }
  ਨਹੀਂਤਾਂ {
    addressend = strrchr(dup, '>');
    ਜੇ(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  ਜੇ(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (ਖਾਲੀ)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *ਪਤਾ = dup;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ cr_eob_ctx {
  ਬਣਤਰ Curl_creader super;
  ਬਣਤਰ bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  ਦੋਆਧਾਰੀ_ਅੰਕ(read_eos);  /* we read an EOS from the next reader */
  ਦੋਆਧਾਰੀ_ਅੰਕ(processed_eos);  /* we read and processed an EOS */
  ਦੋਆਧਾਰੀ_ਅੰਕ(eos);       /* we have returned an EOS */
};

ਸਥਿਰ CURLcode cr_eob_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_eob_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cr_eob_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_creader *reader)
{
  ਬਣਤਰ cr_eob_ctx *ctx = reader->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SMTP_EOB          "\r\n.\r\n"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SMTP_EOB_ਲੱਭੋ_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ਸਥਿਰ CURLcode cr_eob_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ Curl_creader *reader,
                            ਅੱਖਰ *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ਬਣਤਰ cr_eob_ctx *ctx = reader->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread, i, ਸ਼ੁਰੂ, n;
  bool eos;

  ਜੇ(!ctx->read_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf)) {
    /* Get more and convert it when needed */
    ਨਤੀਜਾ = Curl_creader_read(ਅੰਕੜੇ, reader->ਅਗਲਾ, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(ਅੰਕੜੇ, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (ਪੂਰਨ)ਨਤੀਜਾ, nread, eos);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ctx->read_eos = eos;
    ਜੇ(nread) {
      ਜੇ(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        ਵਾਪਸ CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      ਲਈ(i = ਸ਼ੁਰੂ = 0; i < nread; ++i) {
        ਜੇ(ctx->n_eob >= SMTP_EOB_ਲੱਭੋ_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, buf + ਸ਼ੁਰੂ, i - ਸ਼ੁਰੂ, &n);
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
          ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
          ctx->n_eob = 0;
          ਸ਼ੁਰੂ = i;
          ਜੇ(ਅੰਕੜੇ->state.infilesize > 0)
            ਅੰਕੜੇ->state.infilesize++;
        }

        ਜੇ(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        ਜੇ(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      ਜੇ(ਸ਼ੁਰੂ < nread) {
        ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, buf + ਸ਼ੁਰੂ, nread - ਸ਼ੁਰੂ, &n);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }

  *peos = FALSE;

  ਜੇ(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ਅਚਲ ਅੱਖਰ *eob = SMTP_EOB;
    CURL_TRC_SMTP(ਅੰਕੜੇ, "auto-ending mail body with '\\r\\n.\\r\\n'");
    ਚੋਣ(ctx->n_eob) {
    ਹਾਲਤ 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      ਰੋਕੋ;
    ਹਾਲਤ 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }
    ਨਤੀਜਾ = Curl_bufq_cwrite(&ctx->buf, eob, ਲੜੀ_ਲੰਬਾਈ(eob), &n);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ctx->processed_eos = TRUE;
  }

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf)) {
    ਨਤੀਜਾ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  ਨਹੀਂਤਾਂ
    *pnread = 0;

  ਜੇ(ctx->read_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(ਅੰਕੜੇ, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(ਅੰਕੜੇ, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread, *peos));
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ curl_off_t cr_eob_ਕੁੱਲ_ਲੰਮਾਈ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)reader;
  ਵਾਪਸ -1;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_ਕੁੱਲ_ਲੰਮਾਈ,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_ਪੂਰਾ,
  ਆਕਾਰ(ਬਣਤਰ cr_eob_ctx)
};

ਸਥਿਰ CURLcode cr_eob_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_creader *reader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_creader_create(&reader, ਅੰਕੜੇ, &cr_eob, CURL_CR_CONTENT_ENCODE);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_creader_ਜੋੜੋ(ਅੰਕੜੇ, reader);

  ਜੇ(ਨਤੀਜਾ && reader)
    Curl_creader_free(ਅੰਕੜੇ, reader);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ਸਥਿਰ bool smtp_endofresp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn,
                           ਅਚਲ ਅੱਖਰ *ਸਤਰ, size_t len, ਪੂਰਨ *resp)
{
  ਬਣਤਰ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool ਸਿਰਾ = FALSE;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  DEBUGASSERT(smtpc);
  ਜੇ(!smtpc)
    ਵਾਪਸ FALSE;

  /* Nothing for us */
  ਜੇ(len < 4 || !ISDIGIT(ਸਤਰ[0]) || !ISDIGIT(ਸਤਰ[1]) || !ISDIGIT(ਸਤਰ[2]))
    ਵਾਪਸ FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  ਜੇ(ਸਤਰ[3] == ' ' || len == 5) {
    ਅੱਖਰ tmpline[6];
    curl_off_t ਕੋਡ;
    ਅਚਲ ਅੱਖਰ *p = tmpline;
    ਸਿਰਾ = TRUE;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(tmpline, ਸਤਰ, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &ਕੋਡ, len == 5 ? 99999 : 999))
      ਵਾਪਸ FALSE;
    *resp = (ਪੂਰਨ)ਕੋਡ;

    /* Make sure real server never sends internal value */
    ਜੇ(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  ਨਹੀਂਤਾਂ ਜੇ(ਸਤਰ[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    ਸਿਰਾ = TRUE;
    *resp = 1;  /* Internal response code */
  }

  ਵਾਪਸ ਸਿਰਾ;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ਸਥਿਰ CURLcode smtp_get_ਸੁਨੇਹਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ bufref *ਬਾਹਰਲਾ)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);
  ਅੱਖਰ *ਸੁਨੇਹਾ_2;
  size_t len;

  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਸੁਨੇਹਾ_2 = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  ਜੇ(len > 4) {
    /* Find the start of the message */
    len -= 4;
    ਲਈ(ਸੁਨੇਹਾ_2 += 4; ISBLANK(*ਸੁਨੇਹਾ_2); ਸੁਨੇਹਾ_2++, len--)
      ;

    /* Find the end of the message */
    ਜਦੋਂ(len--)
      ਜੇ(!ISNEWLINE(ਸੁਨੇਹਾ_2[len]) && !ISBLANK(ਸੁਨੇਹਾ_2[len]))
        ਰੋਕੋ;

    /* Terminate the message */
    ਸੁਨੇਹਾ_2[++len] = '\0';
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, ਸੁਨੇਹਾ_2, len, ਨਿਰਾ);
  }
  ਨਹੀਂਤਾਂ
    /* junk input => zero length output */
    Curl_bufref_ਸੈੱਟ(ਬਾਹਰਲਾ, "", 0, ਨਿਰਾ);

  ਵਾਪਸ CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ਸਥਿਰ ਖਾਲੀ smtp_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ smtp_conn *smtpc,
                       smtpstate newstate)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  /* for debug purposes */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਨਾਂ[] = {
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
    /*ਆਖਰੀ*/
  };

  ਜੇ(smtpc->state != newstate)
    CURL_TRC_SMTP(ਅੰਕੜੇ, "state change from %s to %s",
                  ਨਾਂ[smtpc->state], ਨਾਂ[newstate]);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ਸਥਿਰ CURLcode smtp_perform_ehlo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ smtp_conn *smtpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "EHLO %s", smtpc->domain);

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_EHLO);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ਸਥਿਰ CURLcode smtp_perform_helo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ smtp_conn *smtpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "HELO %s", smtpc->domain);

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_HELO);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ਸਥਿਰ CURLcode smtp_perform_starttls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "%s", "STARTTLS");

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STARTTLS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ਸਥਿਰ CURLcode smtp_perform_upgrade_tls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ smtp_conn *smtpc)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  /* Start the SSL connection */
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  ਜੇ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ਨਤੀਜਾ = Curl_ssl_cfilter_ਜੋੜੋ(
      ਅੰਕੜੇ, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ਅੰਕੜੇ, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (ਪੂਰਨ)ਨਤੀਜਾ, ssldone));
  ਜੇ(!ਨਤੀਜਾ && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    ਨਤੀਜਾ = smtp_perform_ehlo(ਅੰਕੜੇ, smtpc);
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)smtpc;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ਸਥਿਰ CURLcode smtp_perform_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *mech,
                                  ਅਚਲ ਬਣਤਰ bufref *initresp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);
  ਅਚਲ ਅੱਖਰ *ir = Curl_bufref_ptr(initresp);

  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ਨਹੀਂਤਾਂ {
    /* Send the AUTH command */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "AUTH %s", mech);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ਸਥਿਰ CURLcode smtp_continue_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਅੱਖਰ *mech,
                                   ਅਚਲ ਬਣਤਰ bufref *resp)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);

  (ਖਾਲੀ)mech;
  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ਸਥਿਰ CURLcode smtp_cancel_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *mech)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);

  (ਖਾਲੀ)mech;
  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ਸਥਿਰ CURLcode smtp_perform_authentication(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ smtp_conn *smtpc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  saslprogress ਤਰੱਕੀ;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  ਜੇ(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, ਅੰਕੜੇ)) {
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Calculate the SASL login details */
  ਨਤੀਜਾ = Curl_sasl_ਸ਼ੁਰੂ(&smtpc->sasl, ਅੰਕੜੇ, FALSE, &ਤਰੱਕੀ);

  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(ਤਰੱਕੀ == SASL_INPROGRESS)
      smtp_state(ਅੰਕੜੇ, smtpc, SMTP_AUTH);
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_sasl_is_blocked(&smtpc->sasl, ਅੰਕੜੇ);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ਸਥਿਰ CURLcode smtp_perform_command(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਬਣਤਰ SMTP *smtp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    ਜੇ(!smtp->custom || !smtp->custom[0]) {
      ਅੱਖਰ *ਪਤਾ = ਨਿਰਾ;
      ਬਣਤਰ hostname host = { ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ };
      ਅਚਲ ਅੱਖਰ *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      ਨਤੀਜਾ = smtp_parse_ਪਤਾ(ਅੰਕੜੇ, smtp->rcpt->ਅੰਕੜੇ,
                                  &ਪਤਾ, &host, &suffix);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(ਪਤਾ) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "VRFY %s%s%s%s",
                             ਪਤਾ,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(ਪਤਾ);
    }
    ਨਹੀਂਤਾਂ {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->ਅੰਕੜੇ,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  ਨਹੀਂਤਾਂ
    /* Send the non-recipient based command such as HELP */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_COMMAND);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ਸਥਿਰ CURLcode smtp_perform_mail(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ smtp_conn *smtpc,
                                  ਬਣਤਰ SMTP *smtp)
{
  ਅੱਖਰ *from = ਨਿਰਾ;
  ਅੱਖਰ *auth = ਨਿਰਾ;
  ਅੱਖਰ *ਮਾਪ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *str;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_MAIL_FROM);
  ਜੇ(str) {
    ਅੱਖਰ *ਪਤਾ = ਨਿਰਾ;
    ਬਣਤਰ hostname host = { ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ };
    ਅਚਲ ਅੱਖਰ *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    ਨਤੀਜਾ = smtp_parse_ਪਤਾ(ਅੰਕੜੇ, str, &ਪਤਾ, &host, &suffix);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(ਪਤਾ) ||
            !Curl_is_ASCII_name(host.name));

    ਜੇ(host.name) {
      from = curl_maprintf("<%s@%s>%s", ਪਤਾ, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    ਨਹੀਂਤਾਂ
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", ਪਤਾ, suffix);

    curlx_free(ਪਤਾ);
  }
  ਨਹੀਂਤਾਂ
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  ਜੇ(!from) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_MAIL_AUTH);
  ਜੇ(str && smtpc->sasl.authused) {
    ਜੇ(str[0] != '\0') {
      ਅੱਖਰ *ਪਤਾ = ਨਿਰਾ;
      ਬਣਤਰ hostname host = { ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ };
      ਅਚਲ ਅੱਖਰ *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      ਨਤੀਜਾ = smtp_parse_ਪਤਾ(ਅੰਕੜੇ, str, &ਪਤਾ, &host, &suffix);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      ਜੇ(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(ਪਤਾ) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      ਜੇ(host.name) {
        auth = curl_maprintf("<%s@%s>%s", ਪਤਾ, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      ਨਹੀਂਤਾਂ
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", ਪਤਾ, suffix);
      curlx_free(ਪਤਾ);
    }
    ਨਹੀਂਤਾਂ
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    ਜੇ(!auth) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ਜੇ(IS_MIME_POST(ਅੰਕੜੇ)) {
    curl_mimepart *postp = ਅੰਕੜੇ->ਸੈੱਟ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_ਸਿਰਲੇਖਾਂ(postp, ਅੰਕੜੇ->ਸੈੱਟ.ਸਿਰਲੇਖਾਂ, 0);
    ਨਤੀਜਾ = Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(ਅੰਕੜੇ, postp, ਨਿਰਾ,
                                       ਨਿਰਾ, MIMESTRATEGY_MAIL);

    ਜੇ(!ਨਤੀਜਾ && !Curl_checkheaders(ਅੰਕੜੇ, STRCONST("Mime-Version")))
      ਨਤੀਜਾ = Curl_mime_ਜੋੜੋ_ਸਿਰਲੇਖ(&postp->curlheaders, "Mime-Version: 1.0");
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_mime(ਅੰਕੜੇ, postp);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਅੰਕੜੇ->state.infilesize = Curl_creader_ਕੁੱਲ_ਲੰਮਾਈ(ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  {
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_fread(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Calculate the optional SIZE parameter */
  ਜੇ(smtpc->ਮਾਪ_supported && ਅੰਕੜੇ->state.infilesize > 0) {
    ਮਾਪ = curl_maprintf("%" FMT_OFF_T, ਅੰਕੜੇ->state.infilesize);

    ਜੇ(!ਮਾਪ) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  ਜੇ(smtpc->utf8_supported && !utf8) {
    ਬਣਤਰ curl_slist *rcpt = smtp->rcpt;

    ਜਦੋਂ(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      ਜੇ(!Curl_is_ASCII_name(rcpt->ਅੰਕੜੇ))
        utf8 = TRUE;

      rcpt = rcpt->ਅਗਲਾ;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  ਨਤੀਜਾ = cr_eob_ਜੋੜੋ(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Send the MAIL command */
  ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         ਮਾਪ ? " SIZE=" : "", /* Optional on SIZE support  */
                         ਮਾਪ ? ਮਾਪ : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

ਬਾਹਰਲਾ:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(ਮਾਪ);

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_MAIL);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ਸਥਿਰ CURLcode smtp_perform_rcpt_ਮੰਜ਼ਿਲ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਬਣਤਰ SMTP *smtp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ *ਪਤਾ = ਨਿਰਾ;
  ਬਣਤਰ hostname host = { ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ };
  ਅਚਲ ਅੱਖਰ *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  ਨਤੀਜਾ = smtp_parse_ਪਤਾ(ਅੰਕੜੇ, smtp->rcpt->ਅੰਕੜੇ,
                              &ਪਤਾ, &host, &suffix);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Send the RCPT TO command */
  ਜੇ(host.name)
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           ਪਤਾ, host.name, suffix);
  ਨਹੀਂਤਾਂ
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "RCPT TO:<%s>%s",
                           ਪਤਾ, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(ਪਤਾ);

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_RCPT);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ਸਥਿਰ CURLcode smtp_perform_quit(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "%s", "QUIT");

  ਜੇ(!ਨਤੀਜਾ)
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_QUIT);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the initial server greeting */
ਸਥਿਰ CURLcode smtp_state_servergreet_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ smtp_conn *smtpc,
                                            ਪੂਰਨ smtpcode,
                                            smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(smtpcode / 100 != 2) {
    failf(ਅੰਕੜੇ, "Got unexpected smtp-server response: %d", smtpcode);
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = smtp_perform_ehlo(ਅੰਕੜੇ, smtpc);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For STARTTLS responses */
ਸਥਿਰ CURLcode smtp_state_starttls_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ smtp_conn *smtpc,
                                         ਪੂਰਨ smtpcode,
                                         smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  /* Pipelining in response is forbidden. */
  ਜੇ(smtpc->pp.ਉਛਾਲ)
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;

  ਜੇ(smtpcode != 220) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl != CURLUSESSL_TRY) {
      failf(ਅੰਕੜੇ, "STARTTLS denied, code %d", smtpcode);
      ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = smtp_perform_authentication(ਅੰਕੜੇ, smtpc);
  }
  ਨਹੀਂਤਾਂ
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_UPGRADETLS);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For EHLO responses */
ਸਥਿਰ CURLcode smtp_state_ehlo_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਪੂਰਨ smtpcode,
                                     smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ਸਤਰ = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ਖਾਲੀ)instate;

  ਜੇ(smtpcode / 100 != 2 && smtpcode != 1) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(ਅੰਕੜੇ->conn, FIRSTSOCKET))
      ਨਤੀਜਾ = smtp_perform_helo(ਅੰਕੜੇ, smtpc);
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "Remote access denied: %d", smtpcode);
      ਨਤੀਜਾ = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(len >= 4) {
    ਸਤਰ += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    ਜੇ(len >= 8 && curl_strnequal(ਸਤਰ, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    ਨਹੀਂਤਾਂ ਜੇ(len >= 4 && curl_strnequal(ਸਤਰ, "SIZE", 4))
      smtpc->ਮਾਪ_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    ਨਹੀਂਤਾਂ ਜੇ(len >= 8 && curl_strnequal(ਸਤਰ, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    ਨਹੀਂਤਾਂ ਜੇ(len >= 5 && curl_strnequal(ਸਤਰ, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      ਸਤਰ += 5;
      len -= 5;

      /* Loop through the data line */
      ਲਈ(;;) {
        size_t llen;
        size_t wordlen;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ mechbit;

        ਜਦੋਂ(len && (ISBLANK(*ਸਤਰ) || ISNEWLINE(*ਸਤਰ))) {
          ਸਤਰ++;
          len--;
        }

        ਜੇ(!len)
          ਰੋਕੋ;

        /* Extract the word */
        ਲਈ(wordlen = 0; wordlen < len && !ISBLANK(ਸਤਰ[wordlen]) &&
              !ISNEWLINE(ਸਤਰ[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ਸਤਰ, wordlen, &llen);
        ਜੇ(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        ਸਤਰ += wordlen;
        len -= wordlen;
      }
    }

    ਜੇ(smtpcode != 1) {
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl && !Curl_conn_is_ssl(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        ਜੇ(smtpc->tls_supported)
          /* Switch to TLS connection now */
          ਨਤੀਜਾ = smtp_perform_starttls(ਅੰਕੜੇ, smtpc);
        ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          ਨਤੀਜਾ = smtp_perform_authentication(ਅੰਕੜੇ, smtpc);
        ਨਹੀਂਤਾਂ {
          failf(ਅੰਕੜੇ, "STARTTLS not supported.");
          ਨਤੀਜਾ = CURLE_USE_SSL_FAILED;
        }
      }
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = smtp_perform_authentication(ਅੰਕੜੇ, smtpc);
    }
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Unexpectedly short EHLO response");
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For HELO responses */
ਸਥਿਰ CURLcode smtp_state_helo_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਪੂਰਨ smtpcode,
                                     smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(smtpcode / 100 != 2) {
    failf(ਅੰਕੜੇ, "Remote access denied: %d", smtpcode);
    ਨਤੀਜਾ = CURLE_REMOTE_ACCESS_DENIED;
  }
  ਨਹੀਂਤਾਂ
    /* End of connect phase */
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For SASL authentication responses */
ਸਥਿਰ CURLcode smtp_state_auth_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਪੂਰਨ smtpcode,
                                     smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  saslprogress ਤਰੱਕੀ;

  (ਖਾਲੀ)instate;

  ਨਤੀਜਾ = Curl_sasl_continue(&smtpc->sasl, ਅੰਕੜੇ, smtpcode, &ਤਰੱਕੀ);
  ਜੇ(!ਨਤੀਜਾ)
    ਚੋਣ(ਤਰੱਕੀ) {
    ਹਾਲਤ SASL_ਪੂਰਾ:
      smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);  /* Authenticated */
      ਰੋਕੋ;
    ਹਾਲਤ SASL_IDLE:            /* No mechanism left after cancellation */
      failf(ਅੰਕੜੇ, "Authentication cancelled");
      ਨਤੀਜਾ = CURLE_LOGIN_DENIED;
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For command responses */
ਸਥਿਰ CURLcode smtp_state_command_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ smtp_conn *smtpc,
                                        ਬਣਤਰ SMTP *smtp,
                                        ਪੂਰਨ smtpcode,
                                        smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ਸਤਰ = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ਖਾਲੀ)instate;

  ਜੇ((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(ਅੰਕੜੇ, "Command failed: %d", smtpcode);
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!ਅੰਕੜੇ->req.no_body)
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, ਸਤਰ, len);

    ਜੇ(!ਨਤੀਜਾ && (smtpcode != 1)) {
      ਜੇ(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->ਅਗਲਾ;

        ਜੇ(smtp->rcpt) {
          /* Send the next command */
          ਨਤੀਜਾ = smtp_perform_command(ਅੰਕੜੇ, smtpc, smtp);
        }
        ਨਹੀਂਤਾਂ
          /* End of DO phase */
          smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);
      }
      ਨਹੀਂਤਾਂ
        /* End of DO phase */
        smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For MAIL responses */
ਸਥਿਰ CURLcode smtp_state_mail_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਬਣਤਰ SMTP *smtp,
                                     ਪੂਰਨ smtpcode,
                                     smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(smtpcode / 100 != 2) {
    failf(ਅੰਕੜੇ, "MAIL failed: %d", smtpcode);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
  }
  ਨਹੀਂਤਾਂ
    /* Start the RCPT TO command */
    ਨਤੀਜਾ = smtp_perform_rcpt_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, smtpc, smtp);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For RCPT responses */
ਸਥਿਰ CURLcode smtp_state_rcpt_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਬਣਤਰ SMTP *smtp,
                                     ਪੂਰਨ smtpcode,
                                     smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (ਖਾਲੀ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !ਅੰਕੜੇ->ਸੈੱਟ.mail_rcpt_allowfails);

  ਜੇ(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_ਆਖਰੀ_error = smtpcode;

    ਜੇ(is_smtp_blocking_err) {
      failf(ਅੰਕੜੇ, "RCPT failed: %d", smtpcode);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  ਜੇ(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->ਅਗਲਾ;

    ਜੇ(smtp->rcpt)
      /* Send the next RCPT TO command */
      ਨਤੀਜਾ = smtp_perform_rcpt_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, smtpc, smtp);
    ਨਹੀਂਤਾਂ {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      ਜੇ(!smtp->rcpt_had_ok) {
        failf(ਅੰਕੜੇ, "RCPT failed: %d (last error)", smtp->rcpt_ਆਖਰੀ_error);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
      }
      ਨਹੀਂਤਾਂ {
        /* Send the DATA command */
        ਨਤੀਜਾ = Curl_pp_sendf(ਅੰਕੜੇ, &smtpc->pp, "%s", "DATA");

        ਜੇ(!ਨਤੀਜਾ)
          smtp_state(ਅੰਕੜੇ, smtpc, SMTP_ਅੰਕੜੇ);
      }
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For DATA response */
ਸਥਿਰ CURLcode smtp_state_ਅੰਕੜੇ_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     ਪੂਰਨ smtpcode,
                                     smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)instate;

  ਜੇ(smtpcode != 354) {
    failf(ਅੰਕੜੇ, "DATA failed: %d", smtpcode);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
  }
  ਨਹੀਂਤਾਂ {
    /* Set the progress upload size */
    Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);

    /* SMTP upload */
    Curl_xfer_ਤਿਆਰੀ_send(ਅੰਕੜੇ, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ਸਥਿਰ CURLcode smtp_state_postdata_resp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ smtp_conn *smtpc,
                                         ਪੂਰਨ smtpcode,
                                         smtpstate instate)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)instate;

  ਜੇ(smtpcode != 250)
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;

  /* End of DONE phase */
  smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode smtp_pp_statemachine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ smtpcode;
  ਬਣਤਰ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  ਬਣਤਰ SMTP *smtp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SMTP_EASY);
  size_t nread = 0;

  ਜੇ(!smtpc || !smtp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  ਜੇ(smtpc->state == SMTP_UPGRADETLS) {
    ਨਤੀਜਾ = smtp_perform_upgrade_tls(ਅੰਕੜੇ, smtpc);
    ਜੇ(ਨਤੀਜਾ || (smtpc->state == SMTP_UPGRADETLS))
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Flush any data that needs to be sent */
  ਜੇ(smtpc->pp.sendleft)
    ਵਾਪਸ Curl_pp_flushsend(ਅੰਕੜੇ, &smtpc->pp);

  ਕਰੋ {
    /* Read the response from the server */
    ਨਤੀਜਾ = Curl_pp_readresp(ਅੰਕੜੇ, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* Store the latest response for later retrieval if necessary */
    ਜੇ(smtpc->state != SMTP_QUIT && smtpcode != 1)
      ਅੰਕੜੇ->info.httpcode = smtpcode;

    ਜੇ(!smtpcode)
      ਰੋਕੋ;

    /* We have now received a full SMTP server response */
    ਚੋਣ(smtpc->state) {
    ਹਾਲਤ SMTP_SERVERGREET:
      ਨਤੀਜਾ = smtp_state_servergreet_resp(ਅੰਕੜੇ, smtpc,
                                           smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_EHLO:
      ਨਤੀਜਾ = smtp_state_ehlo_resp(ਅੰਕੜੇ, smtpc, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_HELO:
      ਨਤੀਜਾ = smtp_state_helo_resp(ਅੰਕੜੇ, smtpc, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_STARTTLS:
      ਨਤੀਜਾ = smtp_state_starttls_resp(ਅੰਕੜੇ, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ਜੇ(!ਨਤੀਜਾ && (smtpc->state == SMTP_UPGRADETLS))
        ਜਾਓ upgrade_tls;
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_AUTH:
      ਨਤੀਜਾ = smtp_state_auth_resp(ਅੰਕੜੇ, smtpc, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_COMMAND:
      ਨਤੀਜਾ = smtp_state_command_resp(ਅੰਕੜੇ, smtpc, smtp,
                                       smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_MAIL:
      ਨਤੀਜਾ = smtp_state_mail_resp(ਅੰਕੜੇ, smtpc, smtp, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_RCPT:
      ਨਤੀਜਾ = smtp_state_rcpt_resp(ਅੰਕੜੇ, smtpc, smtp, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_ਅੰਕੜੇ:
      ਨਤੀਜਾ = smtp_state_ਅੰਕੜੇ_resp(ਅੰਕੜੇ, smtpc, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_POSTDATA:
      ਨਤੀਜਾ = smtp_state_postdata_resp(ਅੰਕੜੇ, smtpc, smtpcode, smtpc->state);
      ਰੋਕੋ;

    ਹਾਲਤ SMTP_QUIT:
    ਮੂਲ:
      /* internal error */
      smtp_state(ਅੰਕੜੇ, smtpc, SMTP_STOP);
      ਰੋਕੋ;
    }
  } ਜਦੋਂ(!ਨਤੀਜਾ && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode smtp_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);

  *ਪੂਰਾ = FALSE;
  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &smtpc->pp, FALSE, FALSE);
  *ਪੂਰਾ = (smtpc->state == SMTP_STOP);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode smtp_ਖੰਡ_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(smtpc->state != SMTP_STOP && !ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_pp_statemach(ਅੰਕੜੇ, &smtpc->pp, TRUE, disconnecting);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ਸਥਿਰ CURLcode smtp_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);
  ਵਾਪਸ smtpc ? Curl_pp_pollset(ਅੰਕੜੇ, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_ਸੁਨੇਹਾ,     /* Get SASL response message */
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
ਸਥਿਰ CURLcode smtp_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *ਪੂਰਾ = FALSE; /* default to not done yet */
  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  PINGPONG_ਤਿਆਰੀ(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, ਅੰਕੜੇ, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(ਅੰਕੜੇ));

  /* Parse the URL options */
  ਨਤੀਜਾ = smtp_parse_url_options(ਅੰਕੜੇ->conn, smtpc);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Parse the URL path */
  ਨਤੀਜਾ = smtp_parse_url_ਰਸਤਾ(ਅੰਕੜੇ, smtpc);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Start off waiting for the server greeting response */
  smtp_state(ਅੰਕੜੇ, smtpc, SMTP_SERVERGREET);

  ਨਤੀਜਾ = smtp_multi_statemach(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode smtp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                          bool premature)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ SMTP *smtp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SMTP_EASY);

  (ਖਾਲੀ)premature;

  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(!smtp)
    ਵਾਪਸ CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  ਜੇ(ਸਥਿਤੀ) {
    CURL_TRC_M(ਅੰਕੜੇ, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    ਨਤੀਜਾ = ਸਥਿਤੀ;         /* use the already set error code */
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.connect_only && ਅੰਕੜੇ->ਸੈੱਟ.mail_rcpt &&
          (ਅੰਕੜੇ->state.upload || IS_MIME_POST(ਅੰਕੜੇ))) {

    smtp_state(ਅੰਕੜੇ, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    ਨਤੀਜਾ = smtp_ਖੰਡ_statemach(ਅੰਕੜੇ, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_done(status=%d, premature=%d) -> %d",
                (ਪੂਰਨ)ਸਥਿਤੀ, premature, (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ਸਥਿਰ CURLcode smtp_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ smtp_conn *smtpc,
                             ਬਣਤਰ SMTP *smtp,
                             bool *connected,
                             bool *dophase_ਪੂਰਾ)
{
  /* This is SMTP and no proxy */
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_perform(), start");

  ਜੇ(ਅੰਕੜੇ->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_ਪੂਰਾ = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = ਅੰਕੜੇ->ਸੈੱਟ.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_ਆਖਰੀ_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  ਜੇ((ਅੰਕੜੇ->state.upload || IS_MIME_POST(ਅੰਕੜੇ)) && ਅੰਕੜੇ->ਸੈੱਟ.mail_rcpt)
    /* MAIL transfer */
    ਨਤੀਜਾ = smtp_perform_mail(ਅੰਕੜੇ, smtpc, smtp);
  ਨਹੀਂਤਾਂ
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    ਨਤੀਜਾ = smtp_perform_command(ਅੰਕੜੇ, smtpc, smtp);

  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Run the state-machine */
  ਨਤੀਜਾ = smtp_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  *connected = Curl_conn_is_connected(ਅੰਕੜੇ->conn, FIRSTSOCKET);

ਬਾਹਰਲਾ:
  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_perform() -> %d, connected=%d, done=%d",
                (ਪੂਰਨ)ਨਤੀਜਾ, *connected, *dophase_ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Call this when the DO phase has completed */
ਸਥਿਰ CURLcode smtp_dophase_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ SMTP *smtp,
                                  bool connected)
{
  (ਖਾਲੀ)connected;

  ਜੇ(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);

  ਵਾਪਸ CURLE_OK;
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
ਸਥਿਰ CURLcode smtp_regular_transfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ smtp_conn *smtpc,
                                      ਬਣਤਰ SMTP *smtp,
                                      bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ਅੰਕੜੇ->req.ਮਾਪ = -1;

  /* Set the progress data */
  Curl_pgrsReset(ਅੰਕੜੇ);

  /* Carry out the perform */
  ਨਤੀਜਾ = smtp_perform(ਅੰਕੜੇ, smtpc, smtp, &connected, dophase_ਪੂਰਾ);

  /* Perform post DO phase operations if necessary */
  ਜੇ(!ਨਤੀਜਾ && *dophase_ਪੂਰਾ)
    ਨਤੀਜਾ = smtp_dophase_ਪੂਰਾ(ਅੰਕੜੇ, smtp, connected);

  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_regular_transfer() -> %d, done=%d",
                (ਪੂਰਨ)ਨਤੀਜਾ, *dophase_ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
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
ਸਥਿਰ CURLcode smtp_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ smtp_conn *smtpc =
    Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SMTP_CONN);
  ਬਣਤਰ SMTP *smtp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SMTP_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  *ਪੂਰਾ = FALSE; /* default to false */
  ਜੇ(!smtpc || !smtp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* Parse the custom request */
  ਨਤੀਜਾ = smtp_parse_custom_request(ਅੰਕੜੇ, smtp);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = smtp_regular_transfer(ਅੰਕੜੇ, smtpc, smtp, ਪੂਰਾ);
  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_do() -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ਸਥਿਰ CURLcode smtp_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn,
                                bool dead_connection)
{
  ਬਣਤਰ smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  ਜੇ(!smtpc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ਜੇ(!dead_connection && conn->ਬਿੱਟ.protoconnstart &&
     !Curl_pp_needs_flush(ਅੰਕੜੇ, &smtpc->pp) &&
     !smtp_perform_quit(ਅੰਕੜੇ, smtpc))
    (ਖਾਲੀ)smtp_ਖੰਡ_statemach(ਅੰਕੜੇ, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_disconnect(), finished");
  ਵਾਪਸ CURLE_OK;
}

/* Called from multi.c while DOing */
ਸਥਿਰ CURLcode smtp_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *dophase_ਪੂਰਾ)
{
  ਬਣਤਰ SMTP *smtp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SMTP_EASY);
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!smtp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = smtp_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    DEBUGF(infof(ਅੰਕੜੇ, "DO phase failed"));
  ਨਹੀਂਤਾਂ ਜੇ(*dophase_ਪੂਰਾ) {
    ਨਤੀਜਾ = smtp_dophase_ਪੂਰਾ(ਅੰਕੜੇ, smtp, FALSE /* not connected */);

    DEBUGF(infof(ਅੰਕੜੇ, "DO phase is complete"));
  }

  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_doing() -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ,
                *dophase_ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ smtp_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ SMTP *smtp = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_free(smtp);
}

ਸਥਿਰ ਖਾਲੀ smtp_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ smtp_conn *smtpc = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ਸਥਿਰ CURLcode smtp_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ smtp_conn *smtpc;
  ਬਣਤਰ SMTP *smtp;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  smtpc = curlx_calloc(1, ਆਕਾਰ(*smtpc));
  ਜੇ(!smtpc ||
     Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  smtp = curlx_calloc(1, ਆਕਾਰ(*smtp));
  ਜੇ(!smtp ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

ਬਾਹਰਲਾ:
  CURL_TRC_SMTP(ਅੰਕੜੇ, "smtp_setup_connection() -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * SMTP protocol handler.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_smtp = {
  smtp_ਤਿਆਰੀ_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_ਪੂਰਾ,                        /* done */
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

#ਜੇ_ਅੰਤ /* CURL_DISABLE_SMTP */
