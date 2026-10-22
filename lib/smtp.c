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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "pingpong.h"
#ଅନ୍ତର୍ଭାଵ "mime.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "curl_gethostname.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curl_sasl.h"
#ଅନ୍ତର୍ଭାଵ "idn.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
ସଂରଚନା smtp_conn {
  ସଂରଚନା pingpong pp;
  ସଂରଚନା SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  ଅକ୍ଷର *domain;            /* Client address/name to send in the EHLO */
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
ସଂରଚନା SMTP {
  curl_pp_transfer transfer;
  ଅକ୍ଷର *custom;            /* Custom Request */
  ସଂରଚନା curl_slist *rcpt; /* Recipient list */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rcpt_last_error;     /* The last error received for RCPT TO command */
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
ସ୍ଥିର CURLcode smtp_parse_url_options(ସଂରଚନା connectdata *conn,
                                       ସଂରଚନା smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ptr = conn->options;

  ଯେତେବେଳେ(!result && ptr && *ptr) {
    ଅଚଳ ଅକ୍ଷର *ଚାବି = ptr;
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;

    ଯେତେବେଳେ(*ptr && *ptr != '=')
      ptr++;

    ମୂଲ୍ୟ = ptr + 1;

    ଯେତେବେଳେ(*ptr && *ptr != ';')
      ptr++;

    ଯଦି(curl_strnequal(ଚାବି, "AUTH=", 5))
      result = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               ମୂଲ୍ୟ, ptr - ମୂଲ୍ୟ);
    ଅନ୍ୟଥା
      result = CURLE_URL_MALFORMAT;

    ଯଦି(*ptr == ';')
      ptr++;
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ସ୍ଥିର CURLcode smtp_parse_url_path(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ଅଚଳ ଅକ୍ଷର *path = &data->state.up.path[1]; /* skip leading path */
  ଅକ୍ଷର localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  ଯଦି(!*path) {
    ଯଦି(!Curl_gethostname(localhost, ଆକାର(localhost)))
      path = localhost;
    ଅନ୍ୟଥା
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  ଫେରନ୍ତୁ Curl_urldecode(path, 0, &smtpc->domain, ଖାଲି, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ସ୍ଥିର CURLcode smtp_parse_custom_request(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *custom = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  ଯଦି(custom)
    result = Curl_urldecode(custom, 0, &smtp->custom, ଖାଲି, REJECT_CTRL);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode smtp_parse_address(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fqma,
                                   ଅକ୍ଷର **address, ସଂରଚନା hostname *host,
                                   ଅଚଳ ଅକ୍ଷର **suffix)
{
  CURLcode result = CURLE_OK;
  size_t length;
  ଅକ୍ଷର *addressend;
  ଅକ୍ଷର *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  ଯଦି(strpbrk(fqma, "\r\n")) {
    failf(data, "Refusing to send email address with a CR or LF");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  ଯଦି(!dup)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(fqma[0] != '<') {
    length = strlen(dup);
    ଯଦି(length && dup[length - 1] == '>')
      dup[length - 1] = '\0';
  }
  ଅନ୍ୟଥା {
    addressend = strrchr(dup, '>');
    ଯଦି(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  ଯଦି(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (ଶୂନ୍ୟ)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  ଫେରନ୍ତୁ result;
}

ସଂରଚନା cr_eob_ctx {
  ସଂରଚନା Curl_creader super;
  ସଂରଚନା bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

ସ୍ଥିର CURLcode cr_eob_init(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_eob_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_eob_close(ସଂରଚନା Curl_easy *data, ସଂରଚନା Curl_creader *reader)
{
  ସଂରଚନା cr_eob_ctx *ctx = reader->ctx;
  (ଶୂନ୍ୟ)data;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ସ୍ଥିର CURLcode cr_eob_read(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା Curl_creader *reader,
                            ଅକ୍ଷର *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ସଂରଚନା cr_eob_ctx *ctx = reader->ctx;
  CURLcode result = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  ଯଦି(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    result = Curl_creader_read(data, reader->next, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(data, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread, eos);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    ctx->read_eos = eos;
    ଯଦି(nread) {
      ଯଦି(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      ପାଇଁ(i = start = 0; i < nread; ++i) {
        ଯଦି(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          result = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
          result = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
          ctx->n_eob = 0;
          start = i;
          ଯଦି(data->state.infilesize > 0)
            data->state.infilesize++;
        }

        ଯଦି(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        ଯଦି(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      ଯଦି(start < nread) {
        result = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
    }
  }

  *peos = FALSE;

  ଯଦି(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ଅଚଳ ଅକ୍ଷର *eob = SMTP_EOB;
    CURL_TRC_SMTP(data, "auto-ending mail body with '\\r\\n.\\r\\n'");
    ବାଛ(ctx->n_eob) {
    ସ୍ଥିତି 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
    result = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ctx->processed_eos = TRUE;
  }

  ଯଦି(!Curl_bufq_is_empty(&ctx->buf)) {
    result = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  ଅନ୍ୟଥା
    *pnread = 0;

  ଯଦି(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(data, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(data, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread, *peos));
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର curl_off_t cr_eob_total_length(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)reader;
  ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_crtype cr_eob = {
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
  ଆକାର(ସଂରଚନା cr_eob_ctx)
};

ସ୍ଥିର CURLcode cr_eob_add(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_creader *reader = ଖାଲି;
  CURLcode result;

  result = Curl_creader_create(&reader, data, &cr_eob, CURL_CR_CONTENT_ENCODE);
  ଯଦି(!result)
    result = Curl_creader_add(data, reader);

  ଯଦି(result && reader)
    Curl_creader_free(data, reader);
  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ସ୍ଥିର bool smtp_endofresp(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn,
                           ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *resp)
{
  ସଂରଚନା smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (ଶୂନ୍ୟ)data;

  DEBUGASSERT(smtpc);
  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ FALSE;

  /* Nothing for us */
  ଯଦି(len < 4 || !ISDIGIT(ଧାଡ଼ି[0]) || !ISDIGIT(ଧାଡ଼ି[1]) || !ISDIGIT(ଧାଡ଼ି[2]))
    ଫେରନ୍ତୁ FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  ଯଦି(ଧାଡ଼ି[3] == ' ' || len == 5) {
    ଅକ୍ଷର tmpline[6];
    curl_off_t code;
    ଅଚଳ ଅକ୍ଷର *p = tmpline;
    end = TRUE;
    memcpy(tmpline, ଧାଡ଼ି, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &code, len == 5 ? 99999 : 999))
      ଫେରନ୍ତୁ FALSE;
    *resp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)code;

    /* Make sure real server never sends internal value */
    ଯଦି(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  ଅନ୍ୟଥା ଯଦି(ଧାଡ଼ି[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  ଫେରନ୍ତୁ end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ସ୍ଥିର CURLcode smtp_get_message(ସଂରଚନା Curl_easy *data, ସଂରଚନା bufref *out)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ଅକ୍ଷର *message;
  size_t len;

  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  ଯଦି(len > 4) {
    /* Find the start of the message */
    len -= 4;
    ପାଇଁ(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    ଯେତେବେଳେ(len--)
      ଯଦି(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        ଭାଙ୍ଗ;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ଖାଲି);
  }
  ଅନ୍ୟଥା
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ଖାଲି);

  ଫେରନ୍ତୁ CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ସ୍ଥିର ଶୂନ୍ୟ smtp_state(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ names[] = {
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

  ଯଦି(smtpc->state != newstate)
    CURL_TRC_SMTP(data, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
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
ସ୍ଥିର CURLcode smtp_perform_ehlo(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "EHLO %s", smtpc->domain);

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_EHLO);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ସ୍ଥିର CURLcode smtp_perform_helo(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  result = Curl_pp_sendf(data, &smtpc->pp, "HELO %s", smtpc->domain);

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_HELO);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ସ୍ଥିର CURLcode smtp_perform_starttls(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "STARTTLS");

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_STARTTLS);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ସ୍ଥିର CURLcode smtp_perform_upgrade_tls(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  ଯଦି(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    result = Curl_ssl_cfilter_add(
      data, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ଯଦି(result)
      ଯାଅ out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(data, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, ssldone));
  ଯଦି(!result && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    result = smtp_perform_ehlo(data, smtpc);
  }
out:
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)smtpc;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ସ୍ଥିର CURLcode smtp_perform_auth(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ଅକ୍ଷର *mech,
                                  ଅଚଳ ସଂରଚନା bufref *initresp)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ଅଚଳ ଅକ୍ଷର *ir = Curl_bufref_ptr(initresp);

  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ଅନ୍ୟଥା {
    /* Send the AUTH command */
    result = Curl_pp_sendf(data, &smtpc->pp, "AUTH %s", mech);
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ସ୍ଥିର CURLcode smtp_continue_auth(ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ଅକ୍ଷର *mech,
                                   ଅଚଳ ସଂରଚନା bufref *resp)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (ଶୂନ୍ୟ)mech;
  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_pp_sendf(data, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ସ୍ଥିର CURLcode smtp_cancel_auth(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *mech)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  (ଶୂନ୍ୟ)mech;
  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_pp_sendf(data, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ସ୍ଥିର CURLcode smtp_perform_authentication(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା smtp_conn *smtpc)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  ଯଦି(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, data)) {
    smtp_state(data, smtpc, SMTP_STOP);
    ଫେରନ୍ତୁ result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&smtpc->sasl, data, FALSE, &progress);

  ଯଦି(!result) {
    ଯଦି(progress == SASL_INPROGRESS)
      smtp_state(data, smtpc, SMTP_AUTH);
    ଅନ୍ୟଥା
      result = Curl_sasl_is_blocked(&smtpc->sasl, data);
  }

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ସ୍ଥିର CURLcode smtp_perform_command(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ସଂରଚନା SMTP *smtp)
{
  CURLcode result = CURLE_OK;

  ଯଦି(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    ଯଦି(!smtp->custom || !smtp->custom[0]) {
      ଅକ୍ଷର *address = ଖାଲି;
      ସଂରଚନା hostname host = { ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି };
      ଅଚଳ ଅକ୍ଷର *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, smtp->rcpt->data,
                                  &address, &host, &suffix);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

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
    ଅନ୍ୟଥା {
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
  ଅନ୍ୟଥା
    /* Send the non-recipient based command such as HELP */
    result = Curl_pp_sendf(data, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_COMMAND);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ସ୍ଥିର CURLcode smtp_perform_mail(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା smtp_conn *smtpc,
                                  ସଂରଚନା SMTP *smtp)
{
  ଅକ୍ଷର *from = ଖାଲି;
  ଅକ୍ଷର *auth = ଖାଲି;
  ଅକ୍ଷର *size = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *str;
  CURLcode result = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_FROM);
  ଯଦି(str) {
    ଅକ୍ଷର *address = ଖାଲି;
    ସଂରଚନା hostname host = { ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି };
    ଅଚଳ ଅକ୍ଷର *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    result = smtp_parse_address(data, str, &address, &host, &suffix);
    ଯଦି(result)
      ଯାଅ out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    ଯଦି(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    ଅନ୍ୟଥା
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  ଅନ୍ୟଥା
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  ଯଦି(!from) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(data, STRING_MAIL_AUTH);
  ଯଦି(str && smtpc->sasl.authused) {
    ଯଦି(str[0] != '\0') {
      ଅକ୍ଷର *address = ଖାଲି;
      ସଂରଚନା hostname host = { ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି };
      ଅଚଳ ଅକ୍ଷର *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      result = smtp_parse_address(data, str, &address, &host, &suffix);
      ଯଦି(result)
        ଯାଅ out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      ଯଦି(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      ଯଦି(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      ଅନ୍ୟଥା
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    ଅନ୍ୟଥା
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    ଯଦି(!auth) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ଯଦି(IS_MIME_POST(data)) {
    curl_mimepart *postp = data->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, data->set.headers, 0);
    result = Curl_mime_prepare_headers(data, postp, ଖାଲି,
                                       ଖାଲି, MIMESTRATEGY_MAIL);

    ଯଦି(!result && !Curl_checkheaders(data, STRCONST("Mime-Version")))
      result = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    ଯଦି(!result)
      result = Curl_creader_set_mime(data, postp);
    ଯଦି(result)
      ଯାଅ out;
    data->state.infilesize = Curl_creader_total_length(data);
  }
  ଅନ୍ୟଥା
#endif
  {
    result = Curl_creader_set_fread(data, data->state.infilesize);
    ଯଦି(result)
      ଯାଅ out;
  }

  /* Calculate the optional SIZE parameter */
  ଯଦି(smtpc->size_supported && data->state.infilesize > 0) {
    size = curl_maprintf("%" FMT_OFF_T, data->state.infilesize);

    ଯଦି(!size) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  ଯଦି(smtpc->utf8_supported && !utf8) {
    ସଂରଚନା curl_slist *rcpt = smtp->rcpt;

    ଯେତେବେଳେ(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      ଯଦି(!Curl_is_ASCII_name(rcpt->data))
        utf8 = TRUE;

      rcpt = rcpt->next;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  result = cr_eob_add(data);
  ଯଦି(result)
    ଯାଅ out;

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

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_MAIL);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ସ୍ଥିର CURLcode smtp_perform_rcpt_to(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ସଂରଚନା SMTP *smtp)
{
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର *address = ଖାଲି;
  ସଂରଚନା hostname host = { ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି };
  ଅଚଳ ଅକ୍ଷର *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  result = smtp_parse_address(data, smtp->rcpt->data,
                              &address, &host, &suffix);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Send the RCPT TO command */
  ଯଦି(host.name)
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  ଅନ୍ୟଥା
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    result = Curl_pp_sendf(data, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_RCPT);

  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ସ୍ଥିର CURLcode smtp_perform_quit(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode result = Curl_pp_sendf(data, &smtpc->pp, "%s", "QUIT");

  ଯଦି(!result)
    smtp_state(data, smtpc, SMTP_QUIT);

  ଫେରନ୍ତୁ result;
}

/* For the initial server greeting */
ସ୍ଥିର CURLcode smtp_state_servergreet_resp(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା smtp_conn *smtpc,
                                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                            smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(smtpcode / 100 != 2) {
    failf(data, "Got unexpected smtp-server response: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  ଅନ୍ୟଥା
    result = smtp_perform_ehlo(data, smtpc);

  ଫେରନ୍ତୁ result;
}

/* For STARTTLS responses */
ସ୍ଥିର CURLcode smtp_state_starttls_resp(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା smtp_conn *smtpc,
                                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  /* Pipelining in response is forbidden. */
  ଯଦି(smtpc->pp.overflow)
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;

  ଯଦି(smtpcode != 220) {
    ଯଦି(data->set.use_ssl != CURLUSESSL_TRY) {
      failf(data, "STARTTLS denied, code %d", smtpcode);
      result = CURLE_USE_SSL_FAILED;
    }
    ଅନ୍ୟଥା
      result = smtp_perform_authentication(data, smtpc);
  }
  ଅନ୍ୟଥା
    smtp_state(data, smtpc, SMTP_UPGRADETLS);

  ଫେରନ୍ତୁ result;
}

/* For EHLO responses */
ସ୍ଥିର CURLcode smtp_state_ehlo_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ଶୂନ୍ୟ)instate;

  ଯଦି(smtpcode / 100 != 2 && smtpcode != 1) {
    ଯଦି(data->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(data->conn, FIRSTSOCKET))
      result = smtp_perform_helo(data, smtpc);
    ଅନ୍ୟଥା {
      failf(data, "Remote access denied: %d", smtpcode);
      result = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  ଅନ୍ୟଥା ଯଦି(len >= 4) {
    ଧାଡ଼ି += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    ଯଦି(len >= 8 && curl_strnequal(ଧାଡ଼ି, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    ଅନ୍ୟଥା ଯଦି(len >= 4 && curl_strnequal(ଧାଡ଼ି, "SIZE", 4))
      smtpc->size_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    ଅନ୍ୟଥା ଯଦି(len >= 8 && curl_strnequal(ଧାଡ଼ି, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    ଅନ୍ୟଥା ଯଦି(len >= 5 && curl_strnequal(ଧାଡ଼ି, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      ଧାଡ଼ି += 5;
      len -= 5;

      /* Loop through the data line */
      ପାଇଁ(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        ଯେତେବେଳେ(len && (ISBLANK(*ଧାଡ଼ି) || ISNEWLINE(*ଧାଡ଼ି))) {
          ଧାଡ଼ି++;
          len--;
        }

        ଯଦି(!len)
          ଭାଙ୍ଗ;

        /* Extract the word */
        ପାଇଁ(wordlen = 0; wordlen < len && !ISBLANK(ଧାଡ଼ି[wordlen]) &&
              !ISNEWLINE(ଧାଡ଼ି[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(ଧାଡ଼ି, wordlen, &llen);
        ଯଦି(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        ଧାଡ଼ି += wordlen;
        len -= wordlen;
      }
    }

    ଯଦି(smtpcode != 1) {
      ଯଦି(data->set.use_ssl && !Curl_conn_is_ssl(data->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        ଯଦି(smtpc->tls_supported)
          /* Switch to TLS connection now */
          result = smtp_perform_starttls(data, smtpc);
        ଅନ୍ୟଥା ଯଦି(data->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          result = smtp_perform_authentication(data, smtpc);
        ଅନ୍ୟଥା {
          failf(data, "STARTTLS not supported.");
          result = CURLE_USE_SSL_FAILED;
        }
      }
      ଅନ୍ୟଥା
        result = smtp_perform_authentication(data, smtpc);
    }
  }
  ଅନ୍ୟଥା {
    failf(data, "Unexpectedly short EHLO response");
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  ଫେରନ୍ତୁ result;
}

/* For HELO responses */
ସ୍ଥିର CURLcode smtp_state_helo_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(smtpcode / 100 != 2) {
    failf(data, "Remote access denied: %d", smtpcode);
    result = CURLE_REMOTE_ACCESS_DENIED;
  }
  ଅନ୍ୟଥା
    /* End of connect phase */
    smtp_state(data, smtpc, SMTP_STOP);

  ଫେରନ୍ତୁ result;
}

/* For SASL authentication responses */
ସ୍ଥିର CURLcode smtp_state_auth_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  saslprogress progress;

  (ଶୂନ୍ୟ)instate;

  result = Curl_sasl_continue(&smtpc->sasl, data, smtpcode, &progress);
  ଯଦି(!result)
    ବାଛ(progress) {
    ସ୍ଥିତି SASL_DONE:
      smtp_state(data, smtpc, SMTP_STOP);  /* Authenticated */
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SASL_IDLE:            /* No mechanism left after cancellation */
      failf(data, "Authentication cancelled");
      result = CURLE_LOGIN_DENIED;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }

  ଫେରନ୍ତୁ result;
}

/* For command responses */
ସ୍ଥିର CURLcode smtp_state_command_resp(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା smtp_conn *smtpc,
                                        ସଂରଚନା SMTP *smtp,
                                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                        smtpstate instate)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (ଶୂନ୍ୟ)instate;

  ଯଦି((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(data, "Command failed: %d", smtpcode);
    result = CURLE_WEIRD_SERVER_REPLY;
  }
  ଅନ୍ୟଥା {
    ଯଦି(!data->req.no_body)
      result = Curl_client_write(data, CLIENTWRITE_BODY, ଧାଡ଼ି, len);

    ଯଦି(!result && (smtpcode != 1)) {
      ଯଦି(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->next;

        ଯଦି(smtp->rcpt) {
          /* Send the next command */
          result = smtp_perform_command(data, smtpc, smtp);
        }
        ଅନ୍ୟଥା
          /* End of DO phase */
          smtp_state(data, smtpc, SMTP_STOP);
      }
      ଅନ୍ୟଥା
        /* End of DO phase */
        smtp_state(data, smtpc, SMTP_STOP);
    }
  }

  ଫେରନ୍ତୁ result;
}

/* For MAIL responses */
ସ୍ଥିର CURLcode smtp_state_mail_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ସଂରଚନା SMTP *smtp,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(smtpcode / 100 != 2) {
    failf(data, "MAIL failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  ଅନ୍ୟଥା
    /* Start the RCPT TO command */
    result = smtp_perform_rcpt_to(data, smtpc, smtp);

  ଫେରନ୍ତୁ result;
}

/* For RCPT responses */
ସ୍ଥିର CURLcode smtp_state_rcpt_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ସଂରଚନା SMTP *smtp,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (ଶୂନ୍ୟ)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !data->set.mail_rcpt_allowfails);

  ଯଦି(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    ଯଦି(is_smtp_blocking_err) {
      failf(data, "RCPT failed: %d", smtpcode);
      result = CURLE_SEND_ERROR;
    }
  }
  ଅନ୍ୟଥା {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  ଯଦି(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->next;

    ଯଦି(smtp->rcpt)
      /* Send the next RCPT TO command */
      result = smtp_perform_rcpt_to(data, smtpc, smtp);
    ଅନ୍ୟଥା {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      ଯଦି(!smtp->rcpt_had_ok) {
        failf(data, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        result = CURLE_SEND_ERROR;
      }
      ଅନ୍ୟଥା {
        /* Send the DATA command */
        result = Curl_pp_sendf(data, &smtpc->pp, "%s", "DATA");

        ଯଦି(!result)
          smtp_state(data, smtpc, SMTP_DATA);
      }
    }
  }

  ଫେରନ୍ତୁ result;
}

/* For DATA response */
ସ୍ଥିର CURLcode smtp_state_data_resp(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                     smtpstate instate)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)instate;

  ଯଦି(smtpcode != 354) {
    failf(data, "DATA failed: %d", smtpcode);
    result = CURLE_SEND_ERROR;
  }
  ଅନ୍ୟଥା {
    /* Set the progress upload size */
    Curl_pgrsSetUploadSize(data, data->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(data, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(data, smtpc, SMTP_STOP);
  }

  ଫେରନ୍ତୁ result;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ସ୍ଥିର CURLcode smtp_state_postdata_resp(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା smtp_conn *smtpc,
                                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode,
                                         smtpstate instate)
{
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)instate;

  ଯଦି(smtpcode != 250)
    result = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(data, smtpc, SMTP_STOP);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode smtp_pp_statemachine(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା smtpcode;
  ସଂରଚନା smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  ସଂରଚନା SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  size_t nread = 0;

  ଯଦି(!smtpc || !smtp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  ଯଦି(smtpc->state == SMTP_UPGRADETLS) {
    result = smtp_perform_upgrade_tls(data, smtpc);
    ଯଦି(result || (smtpc->state == SMTP_UPGRADETLS))
      ଫେରନ୍ତୁ result;
  }

  /* Flush any data that needs to be sent */
  ଯଦି(smtpc->pp.sendleft)
    ଫେରନ୍ତୁ Curl_pp_flushsend(data, &smtpc->pp);

  କର {
    /* Read the response from the server */
    result = Curl_pp_readresp(data, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    /* Store the latest response for later retrieval if necessary */
    ଯଦି(smtpc->state != SMTP_QUIT && smtpcode != 1)
      data->info.httpcode = smtpcode;

    ଯଦି(!smtpcode)
      ଭାଙ୍ଗ;

    /* We have now received a full SMTP server response */
    ବାଛ(smtpc->state) {
    ସ୍ଥିତି SMTP_SERVERGREET:
      result = smtp_state_servergreet_resp(data, smtpc,
                                           smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_EHLO:
      result = smtp_state_ehlo_resp(data, smtpc, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_HELO:
      result = smtp_state_helo_resp(data, smtpc, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_STARTTLS:
      result = smtp_state_starttls_resp(data, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ଯଦି(!result && (smtpc->state == SMTP_UPGRADETLS))
        ଯାଅ upgrade_tls;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_AUTH:
      result = smtp_state_auth_resp(data, smtpc, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_COMMAND:
      result = smtp_state_command_resp(data, smtpc, smtp,
                                       smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_MAIL:
      result = smtp_state_mail_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_RCPT:
      result = smtp_state_rcpt_resp(data, smtpc, smtp, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_DATA:
      result = smtp_state_data_resp(data, smtpc, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_POSTDATA:
      result = smtp_state_postdata_resp(data, smtpc, smtpcode, smtpc->state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SMTP_QUIT:
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* internal error */
      smtp_state(data, smtpc, SMTP_STOP);
      ଭାଙ୍ଗ;
    }
  } ଯେତେବେଳେ(!result && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  ଫେରନ୍ତୁ result;
}

/* Called repeatedly until done from multi.c */
ସ୍ଥିର CURLcode smtp_multi_statemach(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = Curl_pp_statemach(data, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode smtp_block_statemach(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(smtpc->state != SMTP_STOP && !result)
    result = Curl_pp_statemach(data, &smtpc->pp, TRUE, disconnecting);

  ଫେରନ୍ତୁ result;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ସ୍ଥିର CURLcode smtp_pollset(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ଫେରନ୍ତୁ smtpc ? Curl_pp_pollset(data, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ସ୍ଥିର ଅଚଳ ସଂରଚନା SASLproto saslsmtp = {
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
ସ୍ଥିର CURLcode smtp_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, data, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(data));

  /* Parse the URL options */
  result = smtp_parse_url_options(data->conn, smtpc);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Parse the URL path */
  result = smtp_parse_url_path(data, smtpc);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Start off waiting for the server greeting response */
  smtp_state(data, smtpc, SMTP_SERVERGREET);

  result = smtp_multi_statemach(data, done);

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode smtp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                          bool premature)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  CURLcode result = CURLE_OK;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);

  (ଶୂନ୍ୟ)premature;

  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!smtp)
    ଫେରନ୍ତୁ CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  ଯଦି(status) {
    CURL_TRC_M(data, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  ଅନ୍ୟଥା ଯଦି(!data->set.connect_only && data->set.mail_rcpt &&
          (data->state.upload || IS_MIME_POST(data))) {

    smtp_state(data, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    result = smtp_block_statemach(data, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(data, "smtp_done(status=%d, premature=%d) -> %d",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status, premature, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ସ୍ଥିର CURLcode smtp_perform(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା smtp_conn *smtpc,
                             ସଂରଚନା SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode result = CURLE_OK;

  CURL_TRC_SMTP(data, "smtp_perform(), start");

  ଯଦି(data->req.no_body) {
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
  ଯଦି((data->state.upload || IS_MIME_POST(data)) && data->set.mail_rcpt)
    /* MAIL transfer */
    result = smtp_perform_mail(data, smtpc, smtp);
  ଅନ୍ୟଥା
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    result = smtp_perform_command(data, smtpc, smtp);

  ଯଦି(result)
    ଯାଅ out;

  /* Run the state-machine */
  result = smtp_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(data, "smtp_perform() -> %d, connected=%d, done=%d",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *connected, *dophase_done);
  ଫେରନ୍ତୁ result;
}

/* Call this when the DO phase has completed */
ସ୍ଥିର CURLcode smtp_dophase_done(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା SMTP *smtp,
                                  bool connected)
{
  (ଶୂନ୍ୟ)connected;

  ଯଦି(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(data);

  ଫେରନ୍ତୁ CURLE_OK;
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
ସ୍ଥିର CURLcode smtp_regular_transfer(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା smtp_conn *smtpc,
                                      ସଂରଚନା SMTP *smtp,
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
  ଯଦି(!result && *dophase_done)
    result = smtp_dophase_done(data, smtp, connected);

  CURL_TRC_SMTP(data, "smtp_regular_transfer() -> %d, done=%d",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *dophase_done);
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode smtp_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା smtp_conn *smtpc =
    Curl_conn_meta_get(data->conn, CURL_META_SMTP_CONN);
  ସଂରଚନା SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  *done = FALSE; /* default to false */
  ଯଦି(!smtpc || !smtp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* Parse the custom request */
  result = smtp_parse_custom_request(data, smtp);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = smtp_regular_transfer(data, smtpc, smtp, done);
  CURL_TRC_SMTP(data, "smtp_do() -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  ଫେରନ୍ତୁ result;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ସ୍ଥିର CURLcode smtp_disconnect(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn,
                                bool dead_connection)
{
  ସଂରଚନା smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  ଯଦି(!smtpc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ଯଦି(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(data, &smtpc->pp) &&
     !smtp_perform_quit(data, smtpc))
    (ଶୂନ୍ୟ)smtp_block_statemach(data, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(data, "smtp_disconnect(), finished");
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Called from multi.c while DOing */
ସ୍ଥିର CURLcode smtp_doing(ସଂରଚନା Curl_easy *data, bool *dophase_done)
{
  ସଂରଚନା SMTP *smtp = Curl_meta_get(data, CURL_META_SMTP_EASY);
  CURLcode result;

  ଯଦି(!smtp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = smtp_multi_statemach(data, dophase_done);
  ଯଦି(result)
    DEBUGF(infof(data, "DO phase failed"));
  ଅନ୍ୟଥା ଯଦି(*dophase_done) {
    result = smtp_dophase_done(data, smtp, FALSE /* not connected */);

    DEBUGF(infof(data, "DO phase is complete"));
  }

  CURL_TRC_SMTP(data, "smtp_doing() -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                *dophase_done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ smtp_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା SMTP *smtp = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_free(smtp);
}

ସ୍ଥିର ଶୂନ୍ୟ smtp_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା smtp_conn *smtpc = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ସ୍ଥିର CURLcode smtp_setup_connection(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା smtp_conn *smtpc;
  ସଂରଚନା SMTP *smtp;
  CURLcode result = CURLE_OK;

  smtpc = curlx_calloc(1, ଆକାର(*smtpc));
  ଯଦି(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  smtp = curlx_calloc(1, ଆକାର(*smtp));
  ଯଦି(!smtp ||
     Curl_meta_set(data, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    result = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(data, "smtp_setup_connection() -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

/*
 * SMTP protocol handler.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_smtp = {
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
