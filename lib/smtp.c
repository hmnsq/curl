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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "smtp.h"

#ifndef CURL_DISABLE_SMTP

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "pingpong.h"
#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "curl_gethostname.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curl_sasl.h"
#অন্তর্ভাব "idn.h"
#অন্তর্ভাব "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
প্রকারলেখ গণনা {
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
  SMTP_তথ্য,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_LAST         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
কাঠামো smtp_conn {
  কাঠামো pingpong pp;
  কাঠামো SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  অক্ষর *domain;            /* Client address/name to send in the EHLO */
  BIT(ssldone);            /* Is connect() over SSL done? */
  BIT(tls_supported);      /* StartTLS capability supported by server */
  BIT(আকার_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  BIT(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  BIT(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
কাঠামো SMTP {
  curl_pp_transfer transfer;
  অক্ষর *custom;            /* Custom Request */
  কাঠামো curl_slist *rcpt; /* Recipient list */
  পূর্ণসংখ্যা rcpt_last_error;     /* The last error received for RCPT TO command */
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
স্থির CURLcode smtp_parse_url_options(কাঠামো connectdata *conn,
                                       কাঠামো smtp_conn *smtpc)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ptr = conn->options;

  যখন(!ফলাফল && ptr && *ptr) {
    ধ্রুবক অক্ষর *চাবি = ptr;
    ধ্রুবক অক্ষর *মান;

    যখন(*ptr && *ptr != '=')
      ptr++;

    মান = ptr + 1;

    যখন(*ptr && *ptr != ';')
      ptr++;

    যদি(curl_strnequal(চাবি, "AUTH=", 5))
      ফলাফল = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               মান, ptr - মান);
    অন্যথা
      ফলাফল = CURLE_URL_MALFORMAT;

    যদি(*ptr == ';')
      ptr++;
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
স্থির CURLcode smtp_parse_url_path(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ধ্রুবক অক্ষর *path = &তথ্য->state.up.path[1]; /* skip leading path */
  অক্ষর localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  যদি(!*path) {
    যদি(!Curl_gethostname(localhost, আকার(localhost)))
      path = localhost;
    অন্যথা
      path = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  ফেরত Curl_urldecode(path, 0, &smtpc->domain, ফাঁকা, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
স্থির CURLcode smtp_parse_custom_request(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো SMTP *smtp)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *custom = CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  যদি(custom)
    ফলাফল = Curl_urldecode(custom, 0, &smtp->custom, ফাঁকা, REJECT_CTRL);

  ফেরত ফলাফল;
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
স্থির CURLcode smtp_parse_address(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fqma,
                                   অক্ষর **address, কাঠামো hostname *host,
                                   ধ্রুবক অক্ষর **suffix)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t দৈর্ঘ্য;
  অক্ষর *addressend;
  অক্ষর *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  যদি(strpbrk(fqma, "\r\n")) {
    failf(তথ্য, "Refusing to send email address with a CR or LF");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  যদি(!dup)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(fqma[0] != '<') {
    দৈর্ঘ্য = strlen(dup);
    যদি(দৈর্ঘ্য && dup[দৈর্ঘ্য - 1] == '>')
      dup[দৈর্ঘ্য - 1] = '\0';
  }
  অন্যথা {
    addressend = strrchr(dup, '>');
    যদি(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  যদি(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (শূন্য)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *address = dup;

  ফেরত ফলাফল;
}

কাঠামো cr_eob_ctx {
  কাঠামো Curl_creader super;
  কাঠামো bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

স্থির CURLcode cr_eob_init(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader)
{
  কাঠামো cr_eob_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_LIMIT);
  ফেরত CURLE_OK;
}

স্থির শূন্য cr_eob_close(কাঠামো Curl_easy *তথ্য, কাঠামো Curl_creader *reader)
{
  কাঠামো cr_eob_ctx *ctx = reader->ctx;
  (শূন্য)তথ্য;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#define SMTP_EOB          "\r\n.\r\n"
#define SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
স্থির CURLcode cr_eob_read(কাঠামো Curl_easy *তথ্য,
                            কাঠামো Curl_creader *reader,
                            অক্ষর *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  কাঠামো cr_eob_ctx *ctx = reader->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  যদি(!ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* Get more and convert it when needed */
    ফলাফল = Curl_creader_read(তথ্য, reader->পরবর্তী, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(তথ্য, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (পূর্ণসংখ্যা)ফলাফল, nread, eos);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    ctx->read_eos = eos;
    যদি(nread) {
      যদি(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        ফেরত CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      জন্য(i = start = 0; i < nread; ++i) {
        যদি(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          ফলাফল = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          যদি(ফলাফল)
            ফেরত ফলাফল;
          ফলাফল = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          যদি(ফলাফল)
            ফেরত ফলাফল;
          ctx->n_eob = 0;
          start = i;
          যদি(তথ্য->state.infilesize > 0)
            তথ্য->state.infilesize++;
        }

        যদি(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        যদি(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      যদি(start < nread) {
        ফলাফল = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        যদি(ফলাফল)
          ফেরত ফলাফল;
      }
    }
  }

  *peos = FALSE;

  যদি(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ধ্রুবক অক্ষর *eob = SMTP_EOB;
    CURL_TRC_SMTP(তথ্য, "auto-ending mail body with '\\r\\n.\\r\\n'");
    নির্বাচন(ctx->n_eob) {
    ক্ষেত্র 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      বিরতি;
    ক্ষেত্র 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      বিরতি;
    পূর্বনির্ধারিত:
      বিরতি;
    }
    ফলাফল = Curl_bufq_cwrite(&ctx->buf, eob, strlen(eob), &n);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    ctx->processed_eos = TRUE;
  }

  যদি(!Curl_bufq_is_empty(&ctx->buf)) {
    ফলাফল = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  অন্যথা
    *pnread = 0;

  যদি(ctx->read_eos && Curl_bufq_is_empty(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(তথ্য, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(তথ্য, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (পূর্ণসংখ্যা)ফলাফল, *pnread, *peos));
  ফেরত ফলাফল;
}

স্থির curl_off_t cr_eob_যোগ_দৈর্ঘ্য(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (শূন্য)তথ্য;
  (শূন্য)reader;
  ফেরত -1;
}

স্থির ধ্রুবক কাঠামো Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_যোগ_দৈর্ঘ্য,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_done,
  আকার(কাঠামো cr_eob_ctx)
};

স্থির CURLcode cr_eob_add(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_creader *reader = ফাঁকা;
  CURLcode ফলাফল;

  ফলাফল = Curl_creader_create(&reader, তথ্য, &cr_eob, CURL_CR_CONTENT_ENCODE);
  যদি(!ফলাফল)
    ফলাফল = Curl_creader_add(তথ্য, reader);

  যদি(ফলাফল && reader)
    Curl_creader_free(তথ্য, reader);
  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
স্থির bool smtp_endofresp(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                           ধ্রুবক অক্ষর *সারি, size_t len, পূর্ণসংখ্যা *resp)
{
  কাঠামো smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool end = FALSE;
  (শূন্য)তথ্য;

  DEBUGASSERT(smtpc);
  যদি(!smtpc)
    ফেরত FALSE;

  /* Nothing for us */
  যদি(len < 4 || !ISDIGIT(সারি[0]) || !ISDIGIT(সারি[1]) || !ISDIGIT(সারি[2]))
    ফেরত FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  যদি(সারি[3] == ' ' || len == 5) {
    অক্ষর tmpline[6];
    curl_off_t code;
    ধ্রুবক অক্ষর *p = tmpline;
    end = TRUE;
    memcpy(tmpline, সারি, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    যদি(curlx_str_সংখ্যা(&p, &code, len == 5 ? 99999 : 999))
      ফেরত FALSE;
    *resp = (পূর্ণসংখ্যা)code;

    /* Make sure real server never sends internal value */
    যদি(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  অন্যথা যদি(সারি[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    end = TRUE;
    *resp = 1;  /* Internal response code */
  }

  ফেরত end;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
স্থির CURLcode smtp_get_message(কাঠামো Curl_easy *তথ্য, কাঠামো bufref *out)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);
  অক্ষর *message;
  size_t len;

  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;

  message = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  যদি(len > 4) {
    /* Find the start of the message */
    len -= 4;
    জন্য(message += 4; ISBLANK(*message); message++, len--)
      ;

    /* Find the end of the message */
    যখন(len--)
      যদি(!ISNEWLINE(message[len]) && !ISBLANK(message[len]))
        বিরতি;

    /* Terminate the message */
    message[++len] = '\0';
    Curl_bufref_set(out, message, len, ফাঁকা);
  }
  অন্যথা
    /* junk input => zero length output */
    Curl_bufref_set(out, "", 0, ফাঁকা);

  ফেরত CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
স্থির শূন্য smtp_state(কাঠামো Curl_easy *তথ্য,
                       কাঠামো smtp_conn *smtpc,
                       smtpstate newstate)
{
#ifdef CURLVERBOSE
  /* for debug purposes */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক names[] = {
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

  যদি(smtpc->state != newstate)
    CURL_TRC_SMTP(তথ্য, "state change from %s to %s",
                  names[smtpc->state], names[newstate]);
#অন্যথা
  (শূন্য)তথ্য;
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
স্থির CURLcode smtp_perform_ehlo(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো smtp_conn *smtpc)
{
  CURLcode ফলাফল = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "EHLO %s", smtpc->domain);

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_EHLO);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
স্থির CURLcode smtp_perform_helo(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো smtp_conn *smtpc)
{
  CURLcode ফলাফল = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "HELO %s", smtpc->domain);

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_HELO);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
স্থির CURLcode smtp_perform_starttls(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "%s", "STARTTLS");

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_STARTTLS);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
স্থির CURLcode smtp_perform_upgrade_tls(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো smtp_conn *smtpc)
{
#ifdef USE_SSL
  /* Start the SSL connection */
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  যদি(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ফলাফল = Curl_ssl_cfilter_add(
      তথ্য, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    যদি(ফলাফল)
      যাও out;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(তথ্য, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (পূর্ণসংখ্যা)ফলাফল, ssldone));
  যদি(!ফলাফল && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    ফলাফল = smtp_perform_ehlo(তথ্য, smtpc);
  }
out:
  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)smtpc;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
স্থির CURLcode smtp_perform_auth(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক অক্ষর *mech,
                                  ধ্রুবক কাঠামো bufref *initresp)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);
  ধ্রুবক অক্ষর *ir = Curl_bufref_ptr(initresp);

  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;

  যদি(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  অন্যথা {
    /* Send the AUTH command */
    ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "AUTH %s", mech);
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
স্থির CURLcode smtp_continue_auth(কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক অক্ষর *mech,
                                   ধ্রুবক কাঠামো bufref *resp)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);

  (শূন্য)mech;
  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_pp_sendf(তথ্য, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
স্থির CURLcode smtp_cancel_auth(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *mech)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);

  (শূন্য)mech;
  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_pp_sendf(তথ্য, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
স্থির CURLcode smtp_perform_authentication(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো smtp_conn *smtpc)
{
  CURLcode ফলাফল = CURLE_OK;
  saslprogress progress;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  যদি(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, তথ্য)) {
    smtp_state(তথ্য, smtpc, SMTP_STOP);
    ফেরত ফলাফল;
  }

  /* Calculate the SASL login details */
  ফলাফল = Curl_sasl_start(&smtpc->sasl, তথ্য, FALSE, &progress);

  যদি(!ফলাফল) {
    যদি(progress == SASL_INPROGRESS)
      smtp_state(তথ্য, smtpc, SMTP_AUTH);
    অন্যথা
      ফলাফল = Curl_sasl_is_blocked(&smtpc->sasl, তথ্য);
  }

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
স্থির CURLcode smtp_perform_command(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     কাঠামো SMTP *smtp)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    যদি(!smtp->custom || !smtp->custom[0]) {
      অক্ষর *address = ফাঁকা;
      কাঠামো hostname host = { ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা };
      ধ্রুবক অক্ষর *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      ফলাফল = smtp_parse_address(তথ্য, smtp->rcpt->তথ্য,
                                  &address, &host, &suffix);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(address) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "VRFY %s%s%s%s",
                             address,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(address);
    }
    অন্যথা {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->তথ্য,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  অন্যথা
    /* Send the non-recipient based command such as HELP */
    ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_COMMAND);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
স্থির CURLcode smtp_perform_mail(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো smtp_conn *smtpc,
                                  কাঠামো SMTP *smtp)
{
  অক্ষর *from = ফাঁকা;
  অক্ষর *auth = ফাঁকা;
  অক্ষর *আকার_2 = ফাঁকা;
  ধ্রুবক অক্ষর *str;
  CURLcode ফলাফল = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(তথ্য, STRING_MAIL_FROM);
  যদি(str) {
    অক্ষর *address = ফাঁকা;
    কাঠামো hostname host = { ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা };
    ধ্রুবক অক্ষর *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    ফলাফল = smtp_parse_address(তথ্য, str, &address, &host, &suffix);
    যদি(ফলাফল)
      যাও out;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(address) ||
            !Curl_is_ASCII_name(host.name));

    যদি(host.name) {
      from = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    অন্যথা
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", address, suffix);

    curlx_free(address);
  }
  অন্যথা
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  যদি(!from) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(তথ্য, STRING_MAIL_AUTH);
  যদি(str && smtpc->sasl.authused) {
    যদি(str[0] != '\0') {
      অক্ষর *address = ফাঁকা;
      কাঠামো hostname host = { ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা };
      ধ্রুবক অক্ষর *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      ফলাফল = smtp_parse_address(তথ্য, str, &address, &host, &suffix);
      যদি(ফলাফল)
        যাও out;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      যদি(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(address) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      যদি(host.name) {
        auth = curl_maprintf("<%s@%s>%s", address, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      অন্যথা
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", address, suffix);
      curlx_free(address);
    }
    অন্যথা
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    যদি(!auth) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

#ifndef CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  যদি(IS_MIME_POST(তথ্য)) {
    curl_mimepart *postp = তথ্য->set.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(unsigned পূর্ণসংখ্যা)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_headers(postp, তথ্য->set.headers, 0);
    ফলাফল = Curl_mime_prepare_headers(তথ্য, postp, ফাঁকা,
                                       ফাঁকা, MIMESTRATEGY_MAIL);

    যদি(!ফলাফল && !Curl_checkheaders(তথ্য, STRCONST("Mime-Version")))
      ফলাফল = Curl_mime_add_header(&postp->curlheaders, "Mime-Version: 1.0");
    যদি(!ফলাফল)
      ফলাফল = Curl_creader_set_mime(তথ্য, postp);
    যদি(ফলাফল)
      যাও out;
    তথ্য->state.infilesize = Curl_creader_যোগ_দৈর্ঘ্য(তথ্য);
  }
  অন্যথা
#endif
  {
    ফলাফল = Curl_creader_set_fread(তথ্য, তথ্য->state.infilesize);
    যদি(ফলাফল)
      যাও out;
  }

  /* Calculate the optional SIZE parameter */
  যদি(smtpc->আকার_supported && তথ্য->state.infilesize > 0) {
    আকার_2 = curl_maprintf("%" FMT_OFF_T, তথ্য->state.infilesize);

    যদি(!আকার_2) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  যদি(smtpc->utf8_supported && !utf8) {
    কাঠামো curl_slist *rcpt = smtp->rcpt;

    যখন(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      যদি(!Curl_is_ASCII_name(rcpt->তথ্য))
        utf8 = TRUE;

      rcpt = rcpt->পরবর্তী;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  ফলাফল = cr_eob_add(তথ্য);
  যদি(ফলাফল)
    যাও out;

  /* Send the MAIL command */
  ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         আকার_2 ? " SIZE=" : "", /* Optional on SIZE support  */
                         আকার_2 ? আকার_2 : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

out:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(আকার_2);

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_MAIL);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
স্থির CURLcode smtp_perform_rcpt_to(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     কাঠামো SMTP *smtp)
{
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর *address = ফাঁকা;
  কাঠামো hostname host = { ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা };
  ধ্রুবক অক্ষর *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  ফলাফল = smtp_parse_address(তথ্য, smtp->rcpt->তথ্য,
                              &address, &host, &suffix);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Send the RCPT TO command */
  যদি(host.name)
    ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           address, host.name, suffix);
  অন্যথা
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "RCPT TO:<%s>%s",
                           address, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(address);

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_RCPT);

  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
স্থির CURLcode smtp_perform_quit(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "%s", "QUIT");

  যদি(!ফলাফল)
    smtp_state(তথ্য, smtpc, SMTP_QUIT);

  ফেরত ফলাফল;
}

/* For the initial server greeting */
স্থির CURLcode smtp_state_servergreet_resp(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো smtp_conn *smtpc,
                                            পূর্ণসংখ্যা smtpcode,
                                            smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode / 100 != 2) {
    failf(তথ্য, "Got unexpected smtp-server response: %d", smtpcode);
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
  }
  অন্যথা
    ফলাফল = smtp_perform_ehlo(তথ্য, smtpc);

  ফেরত ফলাফল;
}

/* For STARTTLS responses */
স্থির CURLcode smtp_state_starttls_resp(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো smtp_conn *smtpc,
                                         পূর্ণসংখ্যা smtpcode,
                                         smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  /* Pipelining in response is forbidden. */
  যদি(smtpc->pp.overflow)
    ফেরত CURLE_WEIRD_SERVER_REPLY;

  যদি(smtpcode != 220) {
    যদি(তথ্য->set.use_ssl != CURLUSESSL_TRY) {
      failf(তথ্য, "STARTTLS denied, code %d", smtpcode);
      ফলাফল = CURLE_USE_SSL_FAILED;
    }
    অন্যথা
      ফলাফল = smtp_perform_authentication(তথ্য, smtpc);
  }
  অন্যথা
    smtp_state(তথ্য, smtpc, SMTP_UPGRADETLS);

  ফেরত ফলাফল;
}

/* For EHLO responses */
স্থির CURLcode smtp_state_ehlo_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     পূর্ণসংখ্যা smtpcode,
                                     smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *সারি = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (শূন্য)instate;

  যদি(smtpcode / 100 != 2 && smtpcode != 1) {
    যদি(তথ্য->set.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(তথ্য->conn, FIRSTSOCKET))
      ফলাফল = smtp_perform_helo(তথ্য, smtpc);
    অন্যথা {
      failf(তথ্য, "Remote access denied: %d", smtpcode);
      ফলাফল = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  অন্যথা যদি(len >= 4) {
    সারি += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    যদি(len >= 8 && curl_strnequal(সারি, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    অন্যথা যদি(len >= 4 && curl_strnequal(সারি, "SIZE", 4))
      smtpc->আকার_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    অন্যথা যদি(len >= 8 && curl_strnequal(সারি, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    অন্যথা যদি(len >= 5 && curl_strnequal(সারি, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      সারি += 5;
      len -= 5;

      /* Loop through the data line */
      জন্য(;;) {
        size_t llen;
        size_t wordlen;
        unsigned short mechbit;

        যখন(len && (ISBLANK(*সারি) || ISNEWLINE(*সারি))) {
          সারি++;
          len--;
        }

        যদি(!len)
          বিরতি;

        /* Extract the word */
        জন্য(wordlen = 0; wordlen < len && !ISBLANK(সারি[wordlen]) &&
              !ISNEWLINE(সারি[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(সারি, wordlen, &llen);
        যদি(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        সারি += wordlen;
        len -= wordlen;
      }
    }

    যদি(smtpcode != 1) {
      যদি(তথ্য->set.use_ssl && !Curl_conn_is_ssl(তথ্য->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        যদি(smtpc->tls_supported)
          /* Switch to TLS connection now */
          ফলাফল = smtp_perform_starttls(তথ্য, smtpc);
        অন্যথা যদি(তথ্য->set.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          ফলাফল = smtp_perform_authentication(তথ্য, smtpc);
        অন্যথা {
          failf(তথ্য, "STARTTLS not supported.");
          ফলাফল = CURLE_USE_SSL_FAILED;
        }
      }
      অন্যথা
        ফলাফল = smtp_perform_authentication(তথ্য, smtpc);
    }
  }
  অন্যথা {
    failf(তথ্য, "Unexpectedly short EHLO response");
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
  }

  ফেরত ফলাফল;
}

/* For HELO responses */
স্থির CURLcode smtp_state_helo_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     পূর্ণসংখ্যা smtpcode,
                                     smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode / 100 != 2) {
    failf(তথ্য, "Remote access denied: %d", smtpcode);
    ফলাফল = CURLE_REMOTE_ACCESS_DENIED;
  }
  অন্যথা
    /* End of connect phase */
    smtp_state(তথ্য, smtpc, SMTP_STOP);

  ফেরত ফলাফল;
}

/* For SASL authentication responses */
স্থির CURLcode smtp_state_auth_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     পূর্ণসংখ্যা smtpcode,
                                     smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  saslprogress progress;

  (শূন্য)instate;

  ফলাফল = Curl_sasl_continue(&smtpc->sasl, তথ্য, smtpcode, &progress);
  যদি(!ফলাফল)
    নির্বাচন(progress) {
    ক্ষেত্র SASL_DONE:
      smtp_state(তথ্য, smtpc, SMTP_STOP);  /* Authenticated */
      বিরতি;
    ক্ষেত্র SASL_IDLE:            /* No mechanism left after cancellation */
      failf(তথ্য, "Authentication cancelled");
      ফলাফল = CURLE_LOGIN_DENIED;
      বিরতি;
    পূর্বনির্ধারিত:
      বিরতি;
    }

  ফেরত ফলাফল;
}

/* For command responses */
স্থির CURLcode smtp_state_command_resp(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো smtp_conn *smtpc,
                                        কাঠামো SMTP *smtp,
                                        পূর্ণসংখ্যা smtpcode,
                                        smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *সারি = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (শূন্য)instate;

  যদি((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(তথ্য, "Command failed: %d", smtpcode);
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
  }
  অন্যথা {
    যদি(!তথ্য->req.no_body)
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, সারি, len);

    যদি(!ফলাফল && (smtpcode != 1)) {
      যদি(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->পরবর্তী;

        যদি(smtp->rcpt) {
          /* Send the next command */
          ফলাফল = smtp_perform_command(তথ্য, smtpc, smtp);
        }
        অন্যথা
          /* End of DO phase */
          smtp_state(তথ্য, smtpc, SMTP_STOP);
      }
      অন্যথা
        /* End of DO phase */
        smtp_state(তথ্য, smtpc, SMTP_STOP);
    }
  }

  ফেরত ফলাফল;
}

/* For MAIL responses */
স্থির CURLcode smtp_state_mail_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     কাঠামো SMTP *smtp,
                                     পূর্ণসংখ্যা smtpcode,
                                     smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode / 100 != 2) {
    failf(তথ্য, "MAIL failed: %d", smtpcode);
    ফলাফল = CURLE_SEND_ERROR;
  }
  অন্যথা
    /* Start the RCPT TO command */
    ফলাফল = smtp_perform_rcpt_to(তথ্য, smtpc, smtp);

  ফেরত ফলাফল;
}

/* For RCPT responses */
স্থির CURLcode smtp_state_rcpt_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     কাঠামো SMTP *smtp,
                                     পূর্ণসংখ্যা smtpcode,
                                     smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (শূন্য)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !তথ্য->set.mail_rcpt_allowfails);

  যদি(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_last_error = smtpcode;

    যদি(is_smtp_blocking_err) {
      failf(তথ্য, "RCPT failed: %d", smtpcode);
      ফলাফল = CURLE_SEND_ERROR;
    }
  }
  অন্যথা {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  যদি(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->পরবর্তী;

    যদি(smtp->rcpt)
      /* Send the next RCPT TO command */
      ফলাফল = smtp_perform_rcpt_to(তথ্য, smtpc, smtp);
    অন্যথা {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      যদি(!smtp->rcpt_had_ok) {
        failf(তথ্য, "RCPT failed: %d (last error)", smtp->rcpt_last_error);
        ফলাফল = CURLE_SEND_ERROR;
      }
      অন্যথা {
        /* Send the DATA command */
        ফলাফল = Curl_pp_sendf(তথ্য, &smtpc->pp, "%s", "DATA");

        যদি(!ফলাফল)
          smtp_state(তথ্য, smtpc, SMTP_তথ্য);
      }
    }
  }

  ফেরত ফলাফল;
}

/* For DATA response */
স্থির CURLcode smtp_state_তথ্য_resp(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     পূর্ণসংখ্যা smtpcode,
                                     smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)instate;

  যদি(smtpcode != 354) {
    failf(তথ্য, "DATA failed: %d", smtpcode);
    ফলাফল = CURLE_SEND_ERROR;
  }
  অন্যথা {
    /* Set the progress upload size */
    Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);

    /* SMTP upload */
    Curl_xfer_setup_send(তথ্য, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(তথ্য, smtpc, SMTP_STOP);
  }

  ফেরত ফলাফল;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
স্থির CURLcode smtp_state_postdata_resp(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো smtp_conn *smtpc,
                                         পূর্ণসংখ্যা smtpcode,
                                         smtpstate instate)
{
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)instate;

  যদি(smtpcode != 250)
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;

  /* End of DONE phase */
  smtp_state(তথ্য, smtpc, SMTP_STOP);

  ফেরত ফলাফল;
}

স্থির CURLcode smtp_pp_statemachine(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা smtpcode;
  কাঠামো smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  কাঠামো SMTP *smtp = Curl_meta_get(তথ্য, CURL_META_SMTP_EASY);
  size_t nread = 0;

  যদি(!smtpc || !smtp)
    ফেরত CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  যদি(smtpc->state == SMTP_UPGRADETLS) {
    ফলাফল = smtp_perform_upgrade_tls(তথ্য, smtpc);
    যদি(ফলাফল || (smtpc->state == SMTP_UPGRADETLS))
      ফেরত ফলাফল;
  }

  /* Flush any data that needs to be sent */
  যদি(smtpc->pp.sendleft)
    ফেরত Curl_pp_flushsend(তথ্য, &smtpc->pp);

  করো {
    /* Read the response from the server */
    ফলাফল = Curl_pp_readresp(তথ্য, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* Store the latest response for later retrieval if necessary */
    যদি(smtpc->state != SMTP_QUIT && smtpcode != 1)
      তথ্য->info.httpcode = smtpcode;

    যদি(!smtpcode)
      বিরতি;

    /* We have now received a full SMTP server response */
    নির্বাচন(smtpc->state) {
    ক্ষেত্র SMTP_SERVERGREET:
      ফলাফল = smtp_state_servergreet_resp(তথ্য, smtpc,
                                           smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_EHLO:
      ফলাফল = smtp_state_ehlo_resp(তথ্য, smtpc, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_HELO:
      ফলাফল = smtp_state_helo_resp(তথ্য, smtpc, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_STARTTLS:
      ফলাফল = smtp_state_starttls_resp(তথ্য, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      যদি(!ফলাফল && (smtpc->state == SMTP_UPGRADETLS))
        যাও upgrade_tls;
      বিরতি;

    ক্ষেত্র SMTP_AUTH:
      ফলাফল = smtp_state_auth_resp(তথ্য, smtpc, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_COMMAND:
      ফলাফল = smtp_state_command_resp(তথ্য, smtpc, smtp,
                                       smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_MAIL:
      ফলাফল = smtp_state_mail_resp(তথ্য, smtpc, smtp, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_RCPT:
      ফলাফল = smtp_state_rcpt_resp(তথ্য, smtpc, smtp, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_তথ্য:
      ফলাফল = smtp_state_তথ্য_resp(তথ্য, smtpc, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_POSTDATA:
      ফলাফল = smtp_state_postdata_resp(তথ্য, smtpc, smtpcode, smtpc->state);
      বিরতি;

    ক্ষেত্র SMTP_QUIT:
    পূর্বনির্ধারিত:
      /* internal error */
      smtp_state(তথ্য, smtpc, SMTP_STOP);
      বিরতি;
    }
  } যখন(!ফলাফল && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  ফেরত ফলাফল;
}

/* Called repeatedly until done from multi.c */
স্থির CURLcode smtp_multi_statemach(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);

  *done = FALSE;
  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = Curl_pp_statemach(তথ্য, &smtpc->pp, FALSE, FALSE);
  *done = (smtpc->state == SMTP_STOP);
  ফেরত ফলাফল;
}

স্থির CURLcode smtp_block_statemach(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode ফলাফল = CURLE_OK;

  যখন(smtpc->state != SMTP_STOP && !ফলাফল)
    ফলাফল = Curl_pp_statemach(তথ্য, &smtpc->pp, TRUE, disconnecting);

  ফেরত ফলাফল;
}

/* For the SMTP "protocol connect" and "doing" phases only */
স্থির CURLcode smtp_pollset(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);
  ফেরত smtpc ? Curl_pp_pollset(তথ্য, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
স্থির ধ্রুবক কাঠামো SASLproto saslsmtp = {
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
স্থির CURLcode smtp_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);
  CURLcode ফলাফল = CURLE_OK;

  *done = FALSE; /* default to not done yet */
  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;

  PINGPONG_SETUP(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, তথ্য, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(তথ্য));

  /* Parse the URL options */
  ফলাফল = smtp_parse_url_options(তথ্য->conn, smtpc);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Parse the URL path */
  ফলাফল = smtp_parse_url_path(তথ্য, smtpc);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Start off waiting for the server greeting response */
  smtp_state(তথ্য, smtpc, SMTP_SERVERGREET);

  ফলাফল = smtp_multi_statemach(তথ্য, done);

  ফেরত ফলাফল;
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
স্থির CURLcode smtp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                          bool premature)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো SMTP *smtp = Curl_meta_get(তথ্য, CURL_META_SMTP_EASY);

  (শূন্য)premature;

  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;
  যদি(!smtp)
    ফেরত CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  যদি(status) {
    CURL_TRC_M(তথ্য, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    ফলাফল = status;         /* use the already set error code */
  }
  অন্যথা যদি(!তথ্য->set.connect_only && তথ্য->set.mail_rcpt &&
          (তথ্য->state.upload || IS_MIME_POST(তথ্য))) {

    smtp_state(তথ্য, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    ফলাফল = smtp_block_statemach(তথ্য, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(তথ্য, "smtp_done(status=%d, premature=%d) -> %d",
                (পূর্ণসংখ্যা)status, premature, (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
স্থির CURLcode smtp_perform(কাঠামো Curl_easy *তথ্য,
                             কাঠামো smtp_conn *smtpc,
                             কাঠামো SMTP *smtp,
                             bool *connected,
                             bool *dophase_done)
{
  /* This is SMTP and no proxy */
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_SMTP(তথ্য, "smtp_perform(), start");

  যদি(তথ্য->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = তথ্য->set.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_last_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  যদি((তথ্য->state.upload || IS_MIME_POST(তথ্য)) && তথ্য->set.mail_rcpt)
    /* MAIL transfer */
    ফলাফল = smtp_perform_mail(তথ্য, smtpc, smtp);
  অন্যথা
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    ফলাফল = smtp_perform_command(তথ্য, smtpc, smtp);

  যদি(ফলাফল)
    যাও out;

  /* Run the state-machine */
  ফলাফল = smtp_multi_statemach(তথ্য, dophase_done);

  *connected = Curl_conn_is_connected(তথ্য->conn, FIRSTSOCKET);

out:
  CURL_TRC_SMTP(তথ্য, "smtp_perform() -> %d, connected=%d, done=%d",
                (পূর্ণসংখ্যা)ফলাফল, *connected, *dophase_done);
  ফেরত ফলাফল;
}

/* Call this when the DO phase has completed */
স্থির CURLcode smtp_dophase_done(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো SMTP *smtp,
                                  bool connected)
{
  (শূন্য)connected;

  যদি(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_setup_nop(তথ্য);

  ফেরত CURLE_OK;
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
স্থির CURLcode smtp_regular_transfer(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো smtp_conn *smtpc,
                                      কাঠামো SMTP *smtp,
                                      bool *dophase_done)
{
  CURLcode ফলাফল = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  তথ্য->req.আকার_2 = -1;

  /* Set the progress data */
  Curl_pgrsReset(তথ্য);

  /* Carry out the perform */
  ফলাফল = smtp_perform(তথ্য, smtpc, smtp, &connected, dophase_done);

  /* Perform post DO phase operations if necessary */
  যদি(!ফলাফল && *dophase_done)
    ফলাফল = smtp_dophase_done(তথ্য, smtp, connected);

  CURL_TRC_SMTP(তথ্য, "smtp_regular_transfer() -> %d, done=%d",
                (পূর্ণসংখ্যা)ফলাফল, *dophase_done);
  ফেরত ফলাফল;
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
স্থির CURLcode smtp_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো smtp_conn *smtpc =
    Curl_conn_meta_get(তথ্য->conn, CURL_META_SMTP_CONN);
  কাঠামো SMTP *smtp = Curl_meta_get(তথ্য, CURL_META_SMTP_EASY);
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  *done = FALSE; /* default to false */
  যদি(!smtpc || !smtp)
    ফেরত CURLE_FAILED_INIT;

  /* Parse the custom request */
  ফলাফল = smtp_parse_custom_request(তথ্য, smtp);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = smtp_regular_transfer(তথ্য, smtpc, smtp, done);
  CURL_TRC_SMTP(তথ্য, "smtp_do() -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
  ফেরত ফলাফল;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
স্থির CURLcode smtp_disconnect(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn,
                                bool dead_connection)
{
  কাঠামো smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  যদি(!smtpc)
    ফেরত CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  যদি(!dead_connection && conn->bits.protoconnstart &&
     !Curl_pp_needs_flush(তথ্য, &smtpc->pp) &&
     !smtp_perform_quit(তথ্য, smtpc))
    (শূন্য)smtp_block_statemach(তথ্য, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(তথ্য, "smtp_disconnect(), finished");
  ফেরত CURLE_OK;
}

/* Called from multi.c while DOing */
স্থির CURLcode smtp_doing(কাঠামো Curl_easy *তথ্য, bool *dophase_done)
{
  কাঠামো SMTP *smtp = Curl_meta_get(তথ্য, CURL_META_SMTP_EASY);
  CURLcode ফলাফল;

  যদি(!smtp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = smtp_multi_statemach(তথ্য, dophase_done);
  যদি(ফলাফল)
    DEBUGF(infof(তথ্য, "DO phase failed"));
  অন্যথা যদি(*dophase_done) {
    ফলাফল = smtp_dophase_done(তথ্য, smtp, FALSE /* not connected */);

    DEBUGF(infof(তথ্য, "DO phase is complete"));
  }

  CURL_TRC_SMTP(তথ্য, "smtp_doing() -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল,
                *dophase_done);
  ফেরত ফলাফল;
}

স্থির শূন্য smtp_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো SMTP *smtp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_free(smtp);
}

স্থির শূন্য smtp_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো smtp_conn *smtpc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

স্থির CURLcode smtp_setup_connection(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn)
{
  কাঠামো smtp_conn *smtpc;
  কাঠামো SMTP *smtp;
  CURLcode ফলাফল = CURLE_OK;

  smtpc = curlx_calloc(1, আকার(*smtpc));
  যদি(!smtpc ||
     Curl_conn_meta_set(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  smtp = curlx_calloc(1, আকার(*smtp));
  যদি(!smtp ||
     Curl_meta_set(তথ্য, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    ফলাফল = CURLE_OUT_OF_MEMORY;

out:
  CURL_TRC_SMTP(তথ্য, "smtp_setup_connection() -> %d", (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

/*
 * SMTP protocol handler.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_smtp = {
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
