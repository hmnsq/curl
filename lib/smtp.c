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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "smtp.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "pingpong.h"
#అన్తర్భావ "mime.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "curl_gethostname.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curl_sasl.h"
#అన్తర్భావ "idn.h"
#అన్తర్భావ "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
రకనిర్వచనం గణన {
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
  SMTP_దత్తాంశం,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_చివరి         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
నిర్మాణం smtp_conn {
  నిర్మాణం pingpong pp;
  నిర్మాణం SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  అక్షరం *domain;            /* Client address/name to send in the EHLO */
  బిట్టు(ssldone);            /* Is connect() over SSL done? */
  బిట్టు(tls_supported);      /* StartTLS capability supported by server */
  బిట్టు(కొలత_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  బిట్టు(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  బిట్టు(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
నిర్మాణం SMTP {
  curl_pp_transfer transfer;
  అక్షరం *custom;            /* Custom Request */
  నిర్మాణం curl_slist *rcpt; /* Recipient list */
  పూర్ణాంకం rcpt_చివరి_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  బిట్టు(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  బిట్టు(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
స్థిర CURLcode smtp_parse_url_options(నిర్మాణం connectdata *conn,
                                       నిర్మాణం smtp_conn *smtpc)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ptr = conn->options;

  అయితే(!ఫలితం && ptr && *ptr) {
    స్థిరరాశి అక్షరం *కీలకం = ptr;
    స్థిరరాశి అక్షరం *విలువ;

    అయితే(*ptr && *ptr != '=')
      ptr++;

    విలువ = ptr + 1;

    అయితే(*ptr && *ptr != ';')
      ptr++;

    ఒకవేళ(curl_strnequal(కీలకం, "AUTH=", 5))
      ఫలితం = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               విలువ, ptr - విలువ);
    లేకపోతే
      ఫలితం = CURLE_URL_MALFORMAT;

    ఒకవేళ(*ptr == ';')
      ptr++;
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
స్థిర CURLcode smtp_parse_url_మార్గం(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  స్థిరరాశి అక్షరం *మార్గం = &దత్తాంశం->state.పైకి.మార్గం[1]; /* skip leading path */
  అక్షరం localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  ఒకవేళ(!*మార్గం) {
    ఒకవేళ(!Curl_gethostname(localhost, పరిమాణం(localhost)))
      మార్గం = localhost;
    లేకపోతే
      మార్గం = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  తిరిగి Curl_urldecode(మార్గం, 0, &smtpc->domain, ఖాళీ, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
స్థిర CURLcode smtp_parse_custom_request(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం SMTP *smtp)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *custom = CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  ఒకవేళ(custom)
    ఫలితం = Curl_urldecode(custom, 0, &smtp->custom, ఖాళీ, REJECT_CTRL);

  తిరిగి ఫలితం;
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
స్థిర CURLcode smtp_parse_చిరునామా(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fqma,
                                   అక్షరం **చిరునామా, నిర్మాణం hostname *host,
                                   స్థిరరాశి అక్షరం **suffix)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t నిడివి;
  అక్షరం *addressend;
  అక్షరం *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  ఒకవేళ(strpbrk(fqma, "\r\n")) {
    failf(దత్తాంశం, "Refusing to send email address with a CR or LF");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  ఒకవేళ(!dup)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(fqma[0] != '<') {
    నిడివి = అక్షరశ్రేణి_పొడవు(dup);
    ఒకవేళ(నిడివి && dup[నిడివి - 1] == '>')
      dup[నిడివి - 1] = '\0';
  }
  లేకపోతే {
    addressend = strrchr(dup, '>');
    ఒకవేళ(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  ఒకవేళ(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (శూన్య)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *చిరునామా = dup;

  తిరిగి ఫలితం;
}

నిర్మాణం cr_eob_ctx {
  నిర్మాణం Curl_creader super;
  నిర్మాణం bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  బిట్టు(read_eos);  /* we read an EOS from the next reader */
  బిట్టు(processed_eos);  /* we read and processed an EOS */
  బిట్టు(eos);       /* we have returned an EOS */
};

స్థిర CURLcode cr_eob_init(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_eob_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_హద్దు);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cr_eob_close(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_creader *reader)
{
  నిర్మాణం cr_eob_ctx *ctx = reader->ctx;
  (శూన్య)దత్తాంశం;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#నిర్వచించు SMTP_EOB          "\r\n.\r\n"
#నిర్వచించు SMTP_EOB_కనుగొను_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
స్థిర CURLcode cr_eob_read(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం Curl_creader *reader,
                            అక్షరం *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  నిర్మాణం cr_eob_ctx *ctx = reader->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nread, i, ప్రారంభం, n;
  bool eos;

  ఒకవేళ(!ctx->read_eos && Curl_bufq_is_ఖాళీలు(&ctx->buf)) {
    /* Get more and convert it when needed */
    ఫలితం = Curl_creader_read(దత్తాంశం, reader->తదుపరి, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(దత్తాంశం, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (పూర్ణాంకం)ఫలితం, nread, eos);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    ctx->read_eos = eos;
    ఒకవేళ(nread) {
      ఒకవేళ(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        తిరిగి CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      కొరకు(i = ప్రారంభం = 0; i < nread; ++i) {
        ఒకవేళ(ctx->n_eob >= SMTP_EOB_కనుగొను_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          ఫలితం = Curl_bufq_cwrite(&ctx->buf, buf + ప్రారంభం, i - ప్రారంభం, &n);
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
          ఫలితం = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
          ctx->n_eob = 0;
          ప్రారంభం = i;
          ఒకవేళ(దత్తాంశం->state.infilesize > 0)
            దత్తాంశం->state.infilesize++;
        }

        ఒకవేళ(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        ఒకవేళ(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      ఒకవేళ(ప్రారంభం < nread) {
        ఫలితం = Curl_bufq_cwrite(&ctx->buf, buf + ప్రారంభం, nread - ప్రారంభం, &n);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
      }
    }
  }

  *peos = FALSE;

  ఒకవేళ(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    స్థిరరాశి అక్షరం *eob = SMTP_EOB;
    CURL_TRC_SMTP(దత్తాంశం, "auto-ending mail body with '\\r\\n.\\r\\n'");
    ఎంపిక(ctx->n_eob) {
    స్థితి 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      ఆపు;
    స్థితి 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }
    ఫలితం = Curl_bufq_cwrite(&ctx->buf, eob, అక్షరశ్రేణి_పొడవు(eob), &n);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ctx->processed_eos = TRUE;
  }

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->buf)) {
    ఫలితం = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  లేకపోతే
    *pnread = 0;

  ఒకవేళ(ctx->read_eos && Curl_bufq_is_ఖాళీలు(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(దత్తాంశం, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(దత్తాంశం, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (పూర్ణాంకం)ఫలితం, *pnread, *peos));
  తిరిగి ఫలితం;
}

స్థిర curl_off_t cr_eob_మొత్తం_నిడివి(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (శూన్య)దత్తాంశం;
  (శూన్య)reader;
  తిరిగి -1;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_మొత్తం_నిడివి,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_పూర్తి,
  పరిమాణం(నిర్మాణం cr_eob_ctx)
};

స్థిర CURLcode cr_eob_కూడు(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_creader *reader = ఖాళీ;
  CURLcode ఫలితం;

  ఫలితం = Curl_creader_create(&reader, దత్తాంశం, &cr_eob, CURL_CR_CONTENT_ENCODE);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_creader_కూడు(దత్తాంశం, reader);

  ఒకవేళ(ఫలితం && reader)
    Curl_creader_free(దత్తాంశం, reader);
  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
స్థిర bool smtp_endofresp(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                           స్థిరరాశి అక్షరం *పంక్తి, size_t len, పూర్ణాంకం *resp)
{
  నిర్మాణం smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool చివర = FALSE;
  (శూన్య)దత్తాంశం;

  DEBUGASSERT(smtpc);
  ఒకవేళ(!smtpc)
    తిరిగి FALSE;

  /* Nothing for us */
  ఒకవేళ(len < 4 || !ISDIGIT(పంక్తి[0]) || !ISDIGIT(పంక్తి[1]) || !ISDIGIT(పంక్తి[2]))
    తిరిగి FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  ఒకవేళ(పంక్తి[3] == ' ' || len == 5) {
    అక్షరం tmpline[6];
    curl_off_t సంకేతం_2;
    స్థిరరాశి అక్షరం *p = tmpline;
    చివర = TRUE;
    జ్ఞాపకం_నకలు(tmpline, పంక్తి, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    ఒకవేళ(curlx_str_సంఖ్య(&p, &సంకేతం_2, len == 5 ? 99999 : 999))
      తిరిగి FALSE;
    *resp = (పూర్ణాంకం)సంకేతం_2;

    /* Make sure real server never sends internal value */
    ఒకవేళ(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  లేకపోతే ఒకవేళ(పంక్తి[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    చివర = TRUE;
    *resp = 1;  /* Internal response code */
  }

  తిరిగి చివర;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
స్థిర CURLcode smtp_get_సందేశం(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం bufref *బహిర్గతం)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);
  అక్షరం *సందేశం_2;
  size_t len;

  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;

  సందేశం_2 = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  ఒకవేళ(len > 4) {
    /* Find the start of the message */
    len -= 4;
    కొరకు(సందేశం_2 += 4; ISBLANK(*సందేశం_2); సందేశం_2++, len--)
      ;

    /* Find the end of the message */
    అయితే(len--)
      ఒకవేళ(!ISNEWLINE(సందేశం_2[len]) && !ISBLANK(సందేశం_2[len]))
        ఆపు;

    /* Terminate the message */
    సందేశం_2[++len] = '\0';
    Curl_bufref_సమితి(బహిర్గతం, సందేశం_2, len, ఖాళీ);
  }
  లేకపోతే
    /* junk input => zero length output */
    Curl_bufref_సమితి(బహిర్గతం, "", 0, ఖాళీ);

  తిరిగి CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
స్థిర శూన్య smtp_state(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం smtp_conn *smtpc,
                       smtpstate newstate)
{
#నిర్వచితమైతే CURLVERBOSE
  /* for debug purposes */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి పేర్లు[] = {
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
    /*చివరి*/
  };

  ఒకవేళ(smtpc->state != newstate)
    CURL_TRC_SMTP(దత్తాంశం, "state change from %s to %s",
                  పేర్లు[smtpc->state], పేర్లు[newstate]);
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
స్థిర CURLcode smtp_perform_ehlo(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం smtp_conn *smtpc)
{
  CURLcode ఫలితం = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "EHLO %s", smtpc->domain);

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_EHLO);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
స్థిర CURLcode smtp_perform_helo(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం smtp_conn *smtpc)
{
  CURLcode ఫలితం = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "HELO %s", smtpc->domain);

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_HELO);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
స్థిర CURLcode smtp_perform_starttls(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "%s", "STARTTLS");

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_STARTTLS);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
స్థిర CURLcode smtp_perform_upgrade_tls(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం smtp_conn *smtpc)
{
#నిర్వచితమైతే USE_SSL
  /* Start the SSL connection */
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  ఒకవేళ(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    ఫలితం = Curl_ssl_cfilter_కూడు(
      దత్తాంశం, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(దత్తాంశం, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (పూర్ణాంకం)ఫలితం, ssldone));
  ఒకవేళ(!ఫలితం && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    ఫలితం = smtp_perform_ehlo(దత్తాంశం, smtpc);
  }
బహిర్గతం:
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)smtpc;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
స్థిర CURLcode smtp_perform_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *mech,
                                  స్థిరరాశి నిర్మాణం bufref *initresp)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);
  స్థిరరాశి అక్షరం *ir = Curl_bufref_ptr(initresp);

  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  లేకపోతే {
    /* Send the AUTH command */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "AUTH %s", mech);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
స్థిర CURLcode smtp_continue_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి అక్షరం *mech,
                                   స్థిరరాశి నిర్మాణం bufref *resp)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);

  (శూన్య)mech;
  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
స్థిర CURLcode smtp_cancel_auth(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *mech)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);

  (శూన్య)mech;
  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
స్థిర CURLcode smtp_perform_authentication(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం smtp_conn *smtpc)
{
  CURLcode ఫలితం = CURLE_OK;
  saslprogress పురోగతి;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  ఒకవేళ(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, దత్తాంశం)) {
    smtp_state(దత్తాంశం, smtpc, SMTP_STOP);
    తిరిగి ఫలితం;
  }

  /* Calculate the SASL login details */
  ఫలితం = Curl_sasl_ప్రారంభం(&smtpc->sasl, దత్తాంశం, FALSE, &పురోగతి);

  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(పురోగతి == SASL_INPROGRESS)
      smtp_state(దత్తాంశం, smtpc, SMTP_AUTH);
    లేకపోతే
      ఫలితం = Curl_sasl_is_blocked(&smtpc->sasl, దత్తాంశం);
  }

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
స్థిర CURLcode smtp_perform_command(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     నిర్మాణం SMTP *smtp)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    ఒకవేళ(!smtp->custom || !smtp->custom[0]) {
      అక్షరం *చిరునామా = ఖాళీ;
      నిర్మాణం hostname host = { ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ };
      స్థిరరాశి అక్షరం *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      ఫలితం = smtp_parse_చిరునామా(దత్తాంశం, smtp->rcpt->దత్తాంశం,
                                  &చిరునామా, &host, &suffix);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(చిరునామా) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "VRFY %s%s%s%s",
                             చిరునామా,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(చిరునామా);
    }
    లేకపోతే {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->దత్తాంశం,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  లేకపోతే
    /* Send the non-recipient based command such as HELP */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_COMMAND);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
స్థిర CURLcode smtp_perform_mail(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం smtp_conn *smtpc,
                                  నిర్మాణం SMTP *smtp)
{
  అక్షరం *from = ఖాళీ;
  అక్షరం *auth = ఖాళీ;
  అక్షరం *కొలత = ఖాళీ;
  స్థిరరాశి అక్షరం *str;
  CURLcode ఫలితం = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(దత్తాంశం, STRING_MAIL_FROM);
  ఒకవేళ(str) {
    అక్షరం *చిరునామా = ఖాళీ;
    నిర్మాణం hostname host = { ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ };
    స్థిరరాశి అక్షరం *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    ఫలితం = smtp_parse_చిరునామా(దత్తాంశం, str, &చిరునామా, &host, &suffix);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(చిరునామా) ||
            !Curl_is_ASCII_name(host.name));

    ఒకవేళ(host.name) {
      from = curl_maprintf("<%s@%s>%s", చిరునామా, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    లేకపోతే
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", చిరునామా, suffix);

    curlx_free(చిరునామా);
  }
  లేకపోతే
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  ఒకవేళ(!from) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(దత్తాంశం, STRING_MAIL_AUTH);
  ఒకవేళ(str && smtpc->sasl.authused) {
    ఒకవేళ(str[0] != '\0') {
      అక్షరం *చిరునామా = ఖాళీ;
      నిర్మాణం hostname host = { ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ };
      స్థిరరాశి అక్షరం *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      ఫలితం = smtp_parse_చిరునామా(దత్తాంశం, str, &చిరునామా, &host, &suffix);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      ఒకవేళ(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(చిరునామా) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      ఒకవేళ(host.name) {
        auth = curl_maprintf("<%s@%s>%s", చిరునామా, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      లేకపోతే
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", చిరునామా, suffix);
      curlx_free(చిరునామా);
    }
    లేకపోతే
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    ఒకవేళ(!auth) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  ఒకవేళ(IS_MIME_POST(దత్తాంశం)) {
    curl_mimepart *postp = దత్తాంశం->సమితి.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(గుర్తులేని పూర్ణాంకం)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_శీర్షికలు(postp, దత్తాంశం->సమితి.శీర్షికలు, 0);
    ఫలితం = Curl_mime_prepare_శీర్షికలు(దత్తాంశం, postp, ఖాళీ,
                                       ఖాళీ, MIMESTRATEGY_MAIL);

    ఒకవేళ(!ఫలితం && !Curl_checkheaders(దత్తాంశం, STRCONST("Mime-Version")))
      ఫలితం = Curl_mime_కూడు_శీర్షిక(&postp->curlheaders, "Mime-Version: 1.0");
    ఒకవేళ(!ఫలితం)
      ఫలితం = Curl_creader_సమితి_mime(దత్తాంశం, postp);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    దత్తాంశం->state.infilesize = Curl_creader_మొత్తం_నిడివి(దత్తాంశం);
  }
  లేకపోతే
#అయితే_ముగింపు
  {
    ఫలితం = Curl_creader_సమితి_fread(దత్తాంశం, దత్తాంశం->state.infilesize);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /* Calculate the optional SIZE parameter */
  ఒకవేళ(smtpc->కొలత_supported && దత్తాంశం->state.infilesize > 0) {
    కొలత = curl_maprintf("%" FMT_OFF_T, దత్తాంశం->state.infilesize);

    ఒకవేళ(!కొలత) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  ఒకవేళ(smtpc->utf8_supported && !utf8) {
    నిర్మాణం curl_slist *rcpt = smtp->rcpt;

    అయితే(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      ఒకవేళ(!Curl_is_ASCII_name(rcpt->దత్తాంశం))
        utf8 = TRUE;

      rcpt = rcpt->తదుపరి;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  ఫలితం = cr_eob_కూడు(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* Send the MAIL command */
  ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         కొలత ? " SIZE=" : "", /* Optional on SIZE support  */
                         కొలత ? కొలత : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

బహిర్గతం:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(కొలత);

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_MAIL);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
స్థిర CURLcode smtp_perform_rcpt_గమ్యం(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     నిర్మాణం SMTP *smtp)
{
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం *చిరునామా = ఖాళీ;
  నిర్మాణం hostname host = { ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ };
  స్థిరరాశి అక్షరం *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  ఫలితం = smtp_parse_చిరునామా(దత్తాంశం, smtp->rcpt->దత్తాంశం,
                              &చిరునామా, &host, &suffix);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Send the RCPT TO command */
  ఒకవేళ(host.name)
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           చిరునామా, host.name, suffix);
  లేకపోతే
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "RCPT TO:<%s>%s",
                           చిరునామా, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(చిరునామా);

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_RCPT);

  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
స్థిర CURLcode smtp_perform_quit(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "%s", "QUIT");

  ఒకవేళ(!ఫలితం)
    smtp_state(దత్తాంశం, smtpc, SMTP_QUIT);

  తిరిగి ఫలితం;
}

/* For the initial server greeting */
స్థిర CURLcode smtp_state_servergreet_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం smtp_conn *smtpc,
                                            పూర్ణాంకం smtpcode,
                                            smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(smtpcode / 100 != 2) {
    failf(దత్తాంశం, "Got unexpected smtp-server response: %d", smtpcode);
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  }
  లేకపోతే
    ఫలితం = smtp_perform_ehlo(దత్తాంశం, smtpc);

  తిరిగి ఫలితం;
}

/* For STARTTLS responses */
స్థిర CURLcode smtp_state_starttls_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం smtp_conn *smtpc,
                                         పూర్ణాంకం smtpcode,
                                         smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  /* Pipelining in response is forbidden. */
  ఒకవేళ(smtpc->pp.అధికప్రవాహం)
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;

  ఒకవేళ(smtpcode != 220) {
    ఒకవేళ(దత్తాంశం->సమితి.use_ssl != CURLUSESSL_TRY) {
      failf(దత్తాంశం, "STARTTLS denied, code %d", smtpcode);
      ఫలితం = CURLE_USE_SSL_FAILED;
    }
    లేకపోతే
      ఫలితం = smtp_perform_authentication(దత్తాంశం, smtpc);
  }
  లేకపోతే
    smtp_state(దత్తాంశం, smtpc, SMTP_UPGRADETLS);

  తిరిగి ఫలితం;
}

/* For EHLO responses */
స్థిర CURLcode smtp_state_ehlo_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     పూర్ణాంకం smtpcode,
                                     smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *పంక్తి = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (శూన్య)instate;

  ఒకవేళ(smtpcode / 100 != 2 && smtpcode != 1) {
    ఒకవేళ(దత్తాంశం->సమితి.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(దత్తాంశం->conn, FIRSTSOCKET))
      ఫలితం = smtp_perform_helo(దత్తాంశం, smtpc);
    లేకపోతే {
      failf(దత్తాంశం, "Remote access denied: %d", smtpcode);
      ఫలితం = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  లేకపోతే ఒకవేళ(len >= 4) {
    పంక్తి += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    ఒకవేళ(len >= 8 && curl_strnequal(పంక్తి, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    లేకపోతే ఒకవేళ(len >= 4 && curl_strnequal(పంక్తి, "SIZE", 4))
      smtpc->కొలత_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    లేకపోతే ఒకవేళ(len >= 8 && curl_strnequal(పంక్తి, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    లేకపోతే ఒకవేళ(len >= 5 && curl_strnequal(పంక్తి, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      పంక్తి += 5;
      len -= 5;

      /* Loop through the data line */
      కొరకు(;;) {
        size_t llen;
        size_t wordlen;
        గుర్తులేని పొట్టి mechbit;

        అయితే(len && (ISBLANK(*పంక్తి) || ISNEWLINE(*పంక్తి))) {
          పంక్తి++;
          len--;
        }

        ఒకవేళ(!len)
          ఆపు;

        /* Extract the word */
        కొరకు(wordlen = 0; wordlen < len && !ISBLANK(పంక్తి[wordlen]) &&
              !ISNEWLINE(పంక్తి[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(పంక్తి, wordlen, &llen);
        ఒకవేళ(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        పంక్తి += wordlen;
        len -= wordlen;
      }
    }

    ఒకవేళ(smtpcode != 1) {
      ఒకవేళ(దత్తాంశం->సమితి.use_ssl && !Curl_conn_is_ssl(దత్తాంశం->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        ఒకవేళ(smtpc->tls_supported)
          /* Switch to TLS connection now */
          ఫలితం = smtp_perform_starttls(దత్తాంశం, smtpc);
        లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          ఫలితం = smtp_perform_authentication(దత్తాంశం, smtpc);
        లేకపోతే {
          failf(దత్తాంశం, "STARTTLS not supported.");
          ఫలితం = CURLE_USE_SSL_FAILED;
        }
      }
      లేకపోతే
        ఫలితం = smtp_perform_authentication(దత్తాంశం, smtpc);
    }
  }
  లేకపోతే {
    failf(దత్తాంశం, "Unexpectedly short EHLO response");
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  }

  తిరిగి ఫలితం;
}

/* For HELO responses */
స్థిర CURLcode smtp_state_helo_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     పూర్ణాంకం smtpcode,
                                     smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(smtpcode / 100 != 2) {
    failf(దత్తాంశం, "Remote access denied: %d", smtpcode);
    ఫలితం = CURLE_REMOTE_ACCESS_DENIED;
  }
  లేకపోతే
    /* End of connect phase */
    smtp_state(దత్తాంశం, smtpc, SMTP_STOP);

  తిరిగి ఫలితం;
}

/* For SASL authentication responses */
స్థిర CURLcode smtp_state_auth_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     పూర్ణాంకం smtpcode,
                                     smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  saslprogress పురోగతి;

  (శూన్య)instate;

  ఫలితం = Curl_sasl_continue(&smtpc->sasl, దత్తాంశం, smtpcode, &పురోగతి);
  ఒకవేళ(!ఫలితం)
    ఎంపిక(పురోగతి) {
    స్థితి SASL_పూర్తి:
      smtp_state(దత్తాంశం, smtpc, SMTP_STOP);  /* Authenticated */
      ఆపు;
    స్థితి SASL_IDLE:            /* No mechanism left after cancellation */
      failf(దత్తాంశం, "Authentication cancelled");
      ఫలితం = CURLE_LOGIN_DENIED;
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }

  తిరిగి ఫలితం;
}

/* For command responses */
స్థిర CURLcode smtp_state_command_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం smtp_conn *smtpc,
                                        నిర్మాణం SMTP *smtp,
                                        పూర్ణాంకం smtpcode,
                                        smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *పంక్తి = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (శూన్య)instate;

  ఒకవేళ((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(దత్తాంశం, "Command failed: %d", smtpcode);
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  }
  లేకపోతే {
    ఒకవేళ(!దత్తాంశం->req.no_body)
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, పంక్తి, len);

    ఒకవేళ(!ఫలితం && (smtpcode != 1)) {
      ఒకవేళ(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->తదుపరి;

        ఒకవేళ(smtp->rcpt) {
          /* Send the next command */
          ఫలితం = smtp_perform_command(దత్తాంశం, smtpc, smtp);
        }
        లేకపోతే
          /* End of DO phase */
          smtp_state(దత్తాంశం, smtpc, SMTP_STOP);
      }
      లేకపోతే
        /* End of DO phase */
        smtp_state(దత్తాంశం, smtpc, SMTP_STOP);
    }
  }

  తిరిగి ఫలితం;
}

/* For MAIL responses */
స్థిర CURLcode smtp_state_mail_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     నిర్మాణం SMTP *smtp,
                                     పూర్ణాంకం smtpcode,
                                     smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(smtpcode / 100 != 2) {
    failf(దత్తాంశం, "MAIL failed: %d", smtpcode);
    ఫలితం = CURLE_SEND_ERROR;
  }
  లేకపోతే
    /* Start the RCPT TO command */
    ఫలితం = smtp_perform_rcpt_గమ్యం(దత్తాంశం, smtpc, smtp);

  తిరిగి ఫలితం;
}

/* For RCPT responses */
స్థిర CURLcode smtp_state_rcpt_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     నిర్మాణం SMTP *smtp,
                                     పూర్ణాంకం smtpcode,
                                     smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (శూన్య)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !దత్తాంశం->సమితి.mail_rcpt_allowfails);

  ఒకవేళ(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_చివరి_error = smtpcode;

    ఒకవేళ(is_smtp_blocking_err) {
      failf(దత్తాంశం, "RCPT failed: %d", smtpcode);
      ఫలితం = CURLE_SEND_ERROR;
    }
  }
  లేకపోతే {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  ఒకవేళ(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->తదుపరి;

    ఒకవేళ(smtp->rcpt)
      /* Send the next RCPT TO command */
      ఫలితం = smtp_perform_rcpt_గమ్యం(దత్తాంశం, smtpc, smtp);
    లేకపోతే {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      ఒకవేళ(!smtp->rcpt_had_ok) {
        failf(దత్తాంశం, "RCPT failed: %d (last error)", smtp->rcpt_చివరి_error);
        ఫలితం = CURLE_SEND_ERROR;
      }
      లేకపోతే {
        /* Send the DATA command */
        ఫలితం = Curl_pp_sendf(దత్తాంశం, &smtpc->pp, "%s", "DATA");

        ఒకవేళ(!ఫలితం)
          smtp_state(దత్తాంశం, smtpc, SMTP_దత్తాంశం);
      }
    }
  }

  తిరిగి ఫలితం;
}

/* For DATA response */
స్థిర CURLcode smtp_state_దత్తాంశం_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     పూర్ణాంకం smtpcode,
                                     smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)instate;

  ఒకవేళ(smtpcode != 354) {
    failf(దత్తాంశం, "DATA failed: %d", smtpcode);
    ఫలితం = CURLE_SEND_ERROR;
  }
  లేకపోతే {
    /* Set the progress upload size */
    Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);

    /* SMTP upload */
    Curl_xfer_అమరిక_send(దత్తాంశం, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(దత్తాంశం, smtpc, SMTP_STOP);
  }

  తిరిగి ఫలితం;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
స్థిర CURLcode smtp_state_postdata_resp(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం smtp_conn *smtpc,
                                         పూర్ణాంకం smtpcode,
                                         smtpstate instate)
{
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)instate;

  ఒకవేళ(smtpcode != 250)
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;

  /* End of DONE phase */
  smtp_state(దత్తాంశం, smtpc, SMTP_STOP);

  తిరిగి ఫలితం;
}

స్థిర CURLcode smtp_pp_statemachine(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం smtpcode;
  నిర్మాణం smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  నిర్మాణం SMTP *smtp = Curl_meta_get(దత్తాంశం, CURL_META_SMTP_EASY);
  size_t nread = 0;

  ఒకవేళ(!smtpc || !smtp)
    తిరిగి CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  ఒకవేళ(smtpc->state == SMTP_UPGRADETLS) {
    ఫలితం = smtp_perform_upgrade_tls(దత్తాంశం, smtpc);
    ఒకవేళ(ఫలితం || (smtpc->state == SMTP_UPGRADETLS))
      తిరిగి ఫలితం;
  }

  /* Flush any data that needs to be sent */
  ఒకవేళ(smtpc->pp.sendleft)
    తిరిగి Curl_pp_flushsend(దత్తాంశం, &smtpc->pp);

  చేయి {
    /* Read the response from the server */
    ఫలితం = Curl_pp_readresp(దత్తాంశం, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* Store the latest response for later retrieval if necessary */
    ఒకవేళ(smtpc->state != SMTP_QUIT && smtpcode != 1)
      దత్తాంశం->info.httpcode = smtpcode;

    ఒకవేళ(!smtpcode)
      ఆపు;

    /* We have now received a full SMTP server response */
    ఎంపిక(smtpc->state) {
    స్థితి SMTP_SERVERGREET:
      ఫలితం = smtp_state_servergreet_resp(దత్తాంశం, smtpc,
                                           smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_EHLO:
      ఫలితం = smtp_state_ehlo_resp(దత్తాంశం, smtpc, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_HELO:
      ఫలితం = smtp_state_helo_resp(దత్తాంశం, smtpc, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_STARTTLS:
      ఫలితం = smtp_state_starttls_resp(దత్తాంశం, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      ఒకవేళ(!ఫలితం && (smtpc->state == SMTP_UPGRADETLS))
        వెళ్ళు upgrade_tls;
      ఆపు;

    స్థితి SMTP_AUTH:
      ఫలితం = smtp_state_auth_resp(దత్తాంశం, smtpc, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_COMMAND:
      ఫలితం = smtp_state_command_resp(దత్తాంశం, smtpc, smtp,
                                       smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_MAIL:
      ఫలితం = smtp_state_mail_resp(దత్తాంశం, smtpc, smtp, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_RCPT:
      ఫలితం = smtp_state_rcpt_resp(దత్తాంశం, smtpc, smtp, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_దత్తాంశం:
      ఫలితం = smtp_state_దత్తాంశం_resp(దత్తాంశం, smtpc, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_POSTDATA:
      ఫలితం = smtp_state_postdata_resp(దత్తాంశం, smtpc, smtpcode, smtpc->state);
      ఆపు;

    స్థితి SMTP_QUIT:
    అప్రమేయ:
      /* internal error */
      smtp_state(దత్తాంశం, smtpc, SMTP_STOP);
      ఆపు;
    }
  } అయితే(!ఫలితం && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  తిరిగి ఫలితం;
}

/* Called repeatedly until done from multi.c */
స్థిర CURLcode smtp_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);

  *పూర్తి = FALSE;
  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = Curl_pp_statemach(దత్తాంశం, &smtpc->pp, FALSE, FALSE);
  *పూర్తి = (smtpc->state == SMTP_STOP);
  తిరిగి ఫలితం;
}

స్థిర CURLcode smtp_ఖండం_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode ఫలితం = CURLE_OK;

  అయితే(smtpc->state != SMTP_STOP && !ఫలితం)
    ఫలితం = Curl_pp_statemach(దత్తాంశం, &smtpc->pp, TRUE, disconnecting);

  తిరిగి ఫలితం;
}

/* For the SMTP "protocol connect" and "doing" phases only */
స్థిర CURLcode smtp_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);
  తిరిగి smtpc ? Curl_pp_pollset(దత్తాంశం, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
స్థిర స్థిరరాశి నిర్మాణం SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_సందేశం,     /* Get SASL response message */
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
స్థిర CURLcode smtp_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);
  CURLcode ఫలితం = CURLE_OK;

  *పూర్తి = FALSE; /* default to not done yet */
  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;

  PINGPONG_అమరిక(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, దత్తాంశం, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(దత్తాంశం));

  /* Parse the URL options */
  ఫలితం = smtp_parse_url_options(దత్తాంశం->conn, smtpc);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Parse the URL path */
  ఫలితం = smtp_parse_url_మార్గం(దత్తాంశం, smtpc);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Start off waiting for the server greeting response */
  smtp_state(దత్తాంశం, smtpc, SMTP_SERVERGREET);

  ఫలితం = smtp_multi_statemach(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
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
స్థిర CURLcode smtp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                          bool premature)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం SMTP *smtp = Curl_meta_get(దత్తాంశం, CURL_META_SMTP_EASY);

  (శూన్య)premature;

  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!smtp)
    తిరిగి CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  ఒకవేళ(స్థితి_2) {
    CURL_TRC_M(దత్తాంశం, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    ఫలితం = స్థితి_2;         /* use the already set error code */
  }
  లేకపోతే ఒకవేళ(!దత్తాంశం->సమితి.connect_only && దత్తాంశం->సమితి.mail_rcpt &&
          (దత్తాంశం->state.upload || IS_MIME_POST(దత్తాంశం))) {

    smtp_state(దత్తాంశం, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    ఫలితం = smtp_ఖండం_statemach(దత్తాంశం, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(దత్తాంశం, "smtp_done(status=%d, premature=%d) -> %d",
                (పూర్ణాంకం)స్థితి_2, premature, (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
స్థిర CURLcode smtp_perform(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం smtp_conn *smtpc,
                             నిర్మాణం SMTP *smtp,
                             bool *connected,
                             bool *dophase_పూర్తి)
{
  /* This is SMTP and no proxy */
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_SMTP(దత్తాంశం, "smtp_perform(), start");

  ఒకవేళ(దత్తాంశం->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_పూర్తి = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = దత్తాంశం->సమితి.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_చివరి_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  ఒకవేళ((దత్తాంశం->state.upload || IS_MIME_POST(దత్తాంశం)) && దత్తాంశం->సమితి.mail_rcpt)
    /* MAIL transfer */
    ఫలితం = smtp_perform_mail(దత్తాంశం, smtpc, smtp);
  లేకపోతే
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    ఫలితం = smtp_perform_command(దత్తాంశం, smtpc, smtp);

  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* Run the state-machine */
  ఫలితం = smtp_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET);

బహిర్గతం:
  CURL_TRC_SMTP(దత్తాంశం, "smtp_perform() -> %d, connected=%d, done=%d",
                (పూర్ణాంకం)ఫలితం, *connected, *dophase_పూర్తి);
  తిరిగి ఫలితం;
}

/* Call this when the DO phase has completed */
స్థిర CURLcode smtp_dophase_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం SMTP *smtp,
                                  bool connected)
{
  (శూన్య)connected;

  ఒకవేళ(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_అమరిక_nop(దత్తాంశం);

  తిరిగి CURLE_OK;
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
స్థిర CURLcode smtp_regular_transfer(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం smtp_conn *smtpc,
                                      నిర్మాణం SMTP *smtp,
                                      bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  దత్తాంశం->req.కొలత = -1;

  /* Set the progress data */
  Curl_pgrsReset(దత్తాంశం);

  /* Carry out the perform */
  ఫలితం = smtp_perform(దత్తాంశం, smtpc, smtp, &connected, dophase_పూర్తి);

  /* Perform post DO phase operations if necessary */
  ఒకవేళ(!ఫలితం && *dophase_పూర్తి)
    ఫలితం = smtp_dophase_పూర్తి(దత్తాంశం, smtp, connected);

  CURL_TRC_SMTP(దత్తాంశం, "smtp_regular_transfer() -> %d, done=%d",
                (పూర్ణాంకం)ఫలితం, *dophase_పూర్తి);
  తిరిగి ఫలితం;
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
స్థిర CURLcode smtp_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం smtp_conn *smtpc =
    Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SMTP_CONN);
  నిర్మాణం SMTP *smtp = Curl_meta_get(దత్తాంశం, CURL_META_SMTP_EASY);
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  *పూర్తి = FALSE; /* default to false */
  ఒకవేళ(!smtpc || !smtp)
    తిరిగి CURLE_FAILED_INIT;

  /* Parse the custom request */
  ఫలితం = smtp_parse_custom_request(దత్తాంశం, smtp);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = smtp_regular_transfer(దత్తాంశం, smtpc, smtp, పూర్తి);
  CURL_TRC_SMTP(దత్తాంశం, "smtp_do() -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  తిరిగి ఫలితం;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
స్థిర CURLcode smtp_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn,
                                bool dead_connection)
{
  నిర్మాణం smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  ఒకవేళ(!smtpc)
    తిరిగి CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  ఒకవేళ(!dead_connection && conn->బిట్లు.protoconnstart &&
     !Curl_pp_needs_flush(దత్తాంశం, &smtpc->pp) &&
     !smtp_perform_quit(దత్తాంశం, smtpc))
    (శూన్య)smtp_ఖండం_statemach(దత్తాంశం, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(దత్తాంశం, "smtp_disconnect(), finished");
  తిరిగి CURLE_OK;
}

/* Called from multi.c while DOing */
స్థిర CURLcode smtp_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *dophase_పూర్తి)
{
  నిర్మాణం SMTP *smtp = Curl_meta_get(దత్తాంశం, CURL_META_SMTP_EASY);
  CURLcode ఫలితం;

  ఒకవేళ(!smtp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = smtp_multi_statemach(దత్తాంశం, dophase_పూర్తి);
  ఒకవేళ(ఫలితం)
    DEBUGF(infof(దత్తాంశం, "DO phase failed"));
  లేకపోతే ఒకవేళ(*dophase_పూర్తి) {
    ఫలితం = smtp_dophase_పూర్తి(దత్తాంశం, smtp, FALSE /* not connected */);

    DEBUGF(infof(దత్తాంశం, "DO phase is complete"));
  }

  CURL_TRC_SMTP(దత్తాంశం, "smtp_doing() -> %d, done=%d", (పూర్ణాంకం)ఫలితం,
                *dophase_పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర శూన్య smtp_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం SMTP *smtp = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_free(smtp);
}

స్థిర శూన్య smtp_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం smtp_conn *smtpc = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

స్థిర CURLcode smtp_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం smtp_conn *smtpc;
  నిర్మాణం SMTP *smtp;
  CURLcode ఫలితం = CURLE_OK;

  smtpc = curlx_calloc(1, పరిమాణం(*smtpc));
  ఒకవేళ(!smtpc ||
     Curl_conn_meta_సమితి(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  smtp = curlx_calloc(1, పరిమాణం(*smtp));
  ఒకవేళ(!smtp ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

బహిర్గతం:
  CURL_TRC_SMTP(దత్తాంశం, "smtp_setup_connection() -> %d", (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

/*
 * SMTP protocol handler.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_smtp = {
  smtp_అమరిక_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_పూర్తి,                        /* done */
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

#అయితే_ముగింపు /* CURL_DISABLE_SMTP */
