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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "smtp.h"

#यदि_अपरिभाषितम् CURL_DISABLE_SMTP

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "pingpong.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curl_gethostname.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curl_sasl.h"
#अन्तर्भाव "idn.h"
#अन्तर्भाव "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#परिभाषय CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
प्रकारनिर्देश गणना {
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
  SMTP_दत्त,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_चरमः         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
संरचना smtp_conn {
  संरचना pingpong pp;
  संरचना SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  वर्ण *domain;            /* Client address/name to send in the EHLO */
  BIT(ssldone);            /* Is connect() over SSL done? */
  BIT(tls_supported);      /* StartTLS capability supported by server */
  BIT(आकार_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  BIT(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  BIT(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
संरचना SMTP {
  curl_pp_transfer transfer;
  वर्ण *custom;            /* Custom Request */
  संरचना curl_slist *rcpt; /* Recipient list */
  पूर्णाङ्क rcpt_चरमः_error;     /* The last error received for RCPT TO command */
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
स्थिर CURLcode smtp_parse_url_options(संरचना connectdata *conn,
                                       संरचना smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ptr = conn->options;

  यावत्(!परिणाम && ptr && *ptr) {
    ध्रुव वर्ण *कुञ्चिका = ptr;
    ध्रुव वर्ण *मानम्;

    यावत्(*ptr && *ptr != '=')
      ptr++;

    मानम् = ptr + 1;

    यावत्(*ptr && *ptr != ';')
      ptr++;

    यदि(curl_strnequal(कुञ्चिका, "AUTH=", 5))
      परिणाम = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               मानम्, ptr - मानम्);
    अन्यथा
      परिणाम = CURLE_URL_MALFORMAT;

    यदि(*ptr == ';')
      ptr++;
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
स्थिर CURLcode smtp_parse_url_मार्गः(संरचना Curl_easy *दत्त,
                                    संरचना smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ध्रुव वर्ण *मार्गः = &दत्त->state.ऊर्ध्वम्.मार्गः[1]; /* skip leading path */
  वर्ण localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  यदि(!*मार्गः) {
    यदि(!Curl_gethostname(localhost, परिमाण(localhost)))
      मार्गः = localhost;
    अन्यथा
      मार्गः = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  प्रत्यागम Curl_urldecode(मार्गः, 0, &smtpc->domain, रिक्त, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
स्थिर CURLcode smtp_parse_custom_request(संरचना Curl_easy *दत्त,
                                          संरचना SMTP *smtp)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *custom = CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  यदि(custom)
    परिणाम = Curl_urldecode(custom, 0, &smtp->custom, रिक्त, REJECT_CTRL);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode smtp_parse_स्थानाङ्कः(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fqma,
                                   वर्ण **स्थानाङ्कः, संरचना hostname *host,
                                   ध्रुव वर्ण **suffix)
{
  CURLcode परिणाम = CURLE_OK;
  size_t दीर्घता;
  वर्ण *addressend;
  वर्ण *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  यदि(strpbrk(fqma, "\r\n")) {
    failf(दत्त, "Refusing to send email address with a CR or LF");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  यदि(!dup)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(fqma[0] != '<') {
    दीर्घता = वाक्यदैर्घ्यम्(dup);
    यदि(दीर्घता && dup[दीर्घता - 1] == '>')
      dup[दीर्घता - 1] = '\0';
  }
  अन्यथा {
    addressend = strrchr(dup, '>');
    यदि(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  यदि(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (शून्य)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *स्थानाङ्कः = dup;

  प्रत्यागम परिणाम;
}

संरचना cr_eob_ctx {
  संरचना Curl_creader super;
  संरचना bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  BIT(read_eos);  /* we read an EOS from the next reader */
  BIT(processed_eos);  /* we read and processed an EOS */
  BIT(eos);       /* we have returned an EOS */
};

स्थिर CURLcode cr_eob_init(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader)
{
  संरचना cr_eob_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_सीमा);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_eob_close(संरचना Curl_easy *दत्त, संरचना Curl_creader *reader)
{
  संरचना cr_eob_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#परिभाषय SMTP_EOB          "\r\n.\r\n"
#परिभाषय SMTP_EOB_FIND_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
स्थिर CURLcode cr_eob_read(संरचना Curl_easy *दत्त,
                            संरचना Curl_creader *reader,
                            वर्ण *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  संरचना cr_eob_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nread, i, start, n;
  bool eos;

  यदि(!ctx->read_eos && Curl_bufq_is_रिक्तानि(&ctx->buf)) {
    /* Get more and convert it when needed */
    परिणाम = Curl_creader_read(दत्त, reader->अग्र, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(दत्त, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (पूर्णाङ्क)परिणाम, nread, eos);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    ctx->read_eos = eos;
    यदि(nread) {
      यदि(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        प्रत्यागम CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      प्रति(i = start = 0; i < nread; ++i) {
        यदि(ctx->n_eob >= SMTP_EOB_FIND_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, i - start, &n);
          यदि(परिणाम)
            प्रत्यागम परिणाम;
          परिणाम = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          यदि(परिणाम)
            प्रत्यागम परिणाम;
          ctx->n_eob = 0;
          start = i;
          यदि(दत्त->state.infilesize > 0)
            दत्त->state.infilesize++;
        }

        यदि(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        यदि(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      यदि(start < nread) {
        परिणाम = Curl_bufq_cwrite(&ctx->buf, buf + start, nread - start, &n);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
      }
    }
  }

  *peos = FALSE;

  यदि(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ध्रुव वर्ण *eob = SMTP_EOB;
    CURL_TRC_SMTP(दत्त, "auto-ending mail body with '\\r\\n.\\r\\n'");
    विकल्प(ctx->n_eob) {
    पक्ष 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      भङ्ग;
    पक्ष 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
    परिणाम = Curl_bufq_cwrite(&ctx->buf, eob, वाक्यदैर्घ्यम्(eob), &n);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    ctx->processed_eos = TRUE;
  }

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->buf)) {
    परिणाम = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  अन्यथा
    *pnread = 0;

  यदि(ctx->read_eos && Curl_bufq_is_रिक्तानि(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(दत्त, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(दत्त, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (पूर्णाङ्क)परिणाम, *pnread, *peos));
  प्रत्यागम परिणाम;
}

स्थिर curl_off_t cr_eob_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                      संरचना Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (शून्य)दत्त;
  (शून्य)reader;
  प्रत्यागम -1;
}

स्थिर ध्रुव संरचना Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_योग_दीर्घता,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना cr_eob_ctx)
};

स्थिर CURLcode cr_eob_सङ्कलय(संरचना Curl_easy *दत्त)
{
  संरचना Curl_creader *reader = रिक्त;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&reader, दत्त, &cr_eob, CURL_CR_CONTENT_ENCODE);
  यदि(!परिणाम)
    परिणाम = Curl_creader_सङ्कलय(दत्त, reader);

  यदि(परिणाम && reader)
    Curl_creader_free(दत्त, reader);
  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
स्थिर bool smtp_endofresp(संरचना Curl_easy *दत्त, संरचना connectdata *conn,
                           ध्रुव वर्ण *पङ्क्तिः, size_t len, पूर्णाङ्क *resp)
{
  संरचना smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool अन्तः = FALSE;
  (शून्य)दत्त;

  DEBUGASSERT(smtpc);
  यदि(!smtpc)
    प्रत्यागम FALSE;

  /* Nothing for us */
  यदि(len < 4 || !ISDIGIT(पङ्क्तिः[0]) || !ISDIGIT(पङ्क्तिः[1]) || !ISDIGIT(पङ्क्तिः[2]))
    प्रत्यागम FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  यदि(पङ्क्तिः[3] == ' ' || len == 5) {
    वर्ण tmpline[6];
    curl_off_t संहिता;
    ध्रुव वर्ण *p = tmpline;
    अन्तः = TRUE;
    स्मृतिं_प्रतिलिख(tmpline, पङ्क्तिः, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    यदि(curlx_str_सङ्ख्या(&p, &संहिता, len == 5 ? 99999 : 999))
      प्रत्यागम FALSE;
    *resp = (पूर्णाङ्क)संहिता;

    /* Make sure real server never sends internal value */
    यदि(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  अन्यथा यदि(पङ्क्तिः[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    अन्तः = TRUE;
    *resp = 1;  /* Internal response code */
  }

  प्रत्यागम अन्तः;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
स्थिर CURLcode smtp_get_सन्देशः(संरचना Curl_easy *दत्त, संरचना bufref *बहिर्गमनम्)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);
  वर्ण *सन्देशः_2;
  size_t len;

  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  सन्देशः_2 = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  यदि(len > 4) {
    /* Find the start of the message */
    len -= 4;
    प्रति(सन्देशः_2 += 4; ISBLANK(*सन्देशः_2); सन्देशः_2++, len--)
      ;

    /* Find the end of the message */
    यावत्(len--)
      यदि(!ISNEWLINE(सन्देशः_2[len]) && !ISBLANK(सन्देशः_2[len]))
        भङ्ग;

    /* Terminate the message */
    सन्देशः_2[++len] = '\0';
    Curl_bufref_समुच्चयः(बहिर्गमनम्, सन्देशः_2, len, रिक्त);
  }
  अन्यथा
    /* junk input => zero length output */
    Curl_bufref_समुच्चयः(बहिर्गमनम्, "", 0, रिक्त);

  प्रत्यागम CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
स्थिर शून्य smtp_state(संरचना Curl_easy *दत्त,
                       संरचना smtp_conn *smtpc,
                       smtpstate newstate)
{
#यदि_परिभाषितम् CURLVERBOSE
  /* for debug purposes */
  स्थिर ध्रुव वर्ण * ध्रुव नामानि[] = {
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
    /*चरमः*/
  };

  यदि(smtpc->state != newstate)
    CURL_TRC_SMTP(दत्त, "state change from %s to %s",
                  नामानि[smtpc->state], नामानि[newstate]);
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
स्थिर CURLcode smtp_perform_ehlo(संरचना Curl_easy *दत्त,
                                  संरचना smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "EHLO %s", smtpc->domain);

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_EHLO);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
स्थिर CURLcode smtp_perform_helo(संरचना Curl_easy *दत्त,
                                  संरचना smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "HELO %s", smtpc->domain);

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_HELO);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
स्थिर CURLcode smtp_perform_starttls(संरचना Curl_easy *दत्त,
                                      संरचना smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "%s", "STARTTLS");

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_STARTTLS);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
स्थिर CURLcode smtp_perform_upgrade_tls(संरचना Curl_easy *दत्त,
                                         संरचना smtp_conn *smtpc)
{
#यदि_परिभाषितम् USE_SSL
  /* Start the SSL connection */
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  यदि(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    परिणाम = Curl_ssl_cfilter_सङ्कलय(
      दत्त, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(दत्त, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (पूर्णाङ्क)परिणाम, ssldone));
  यदि(!परिणाम && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    परिणाम = smtp_perform_ehlo(दत्त, smtpc);
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)दत्त;
  (शून्य)smtpc;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
स्थिर CURLcode smtp_perform_auth(संरचना Curl_easy *दत्त,
                                  ध्रुव वर्ण *mech,
                                  ध्रुव संरचना bufref *initresp)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);
  ध्रुव वर्ण *ir = Curl_bufref_ptr(initresp);

  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  अन्यथा {
    /* Send the AUTH command */
    परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "AUTH %s", mech);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
स्थिर CURLcode smtp_continue_auth(संरचना Curl_easy *दत्त,
                                   ध्रुव वर्ण *mech,
                                   ध्रुव संरचना bufref *resp)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);

  (शून्य)mech;
  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(दत्त, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
स्थिर CURLcode smtp_cancel_auth(संरचना Curl_easy *दत्त, ध्रुव वर्ण *mech)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);

  (शून्य)mech;
  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_pp_sendf(दत्त, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
स्थिर CURLcode smtp_perform_authentication(संरचना Curl_easy *दत्त,
                                            संरचना smtp_conn *smtpc)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress प्रगतिः;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  यदि(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, दत्त)) {
    smtp_state(दत्त, smtpc, SMTP_STOP);
    प्रत्यागम परिणाम;
  }

  /* Calculate the SASL login details */
  परिणाम = Curl_sasl_start(&smtpc->sasl, दत्त, FALSE, &प्रगतिः);

  यदि(!परिणाम) {
    यदि(प्रगतिः == SASL_INPROGRESS)
      smtp_state(दत्त, smtpc, SMTP_AUTH);
    अन्यथा
      परिणाम = Curl_sasl_is_blocked(&smtpc->sasl, दत्त);
  }

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
स्थिर CURLcode smtp_perform_command(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    यदि(!smtp->custom || !smtp->custom[0]) {
      वर्ण *स्थानाङ्कः = रिक्त;
      संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
      ध्रुव वर्ण *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      परिणाम = smtp_parse_स्थानाङ्कः(दत्त, smtp->rcpt->दत्त,
                                  &स्थानाङ्कः, &host, &suffix);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(स्थानाङ्कः) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "VRFY %s%s%s%s",
                             स्थानाङ्कः,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(स्थानाङ्कः);
    }
    अन्यथा {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !वाक्यं_तुलय(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->दत्त,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  अन्यथा
    /* Send the non-recipient based command such as HELP */
    परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_COMMAND);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
स्थिर CURLcode smtp_perform_mail(संरचना Curl_easy *दत्त,
                                  संरचना smtp_conn *smtpc,
                                  संरचना SMTP *smtp)
{
  वर्ण *from = रिक्त;
  वर्ण *auth = रिक्त;
  वर्ण *आकार = रिक्त;
  ध्रुव वर्ण *str;
  CURLcode परिणाम = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(दत्त, STRING_MAIL_FROM);
  यदि(str) {
    वर्ण *स्थानाङ्कः = रिक्त;
    संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
    ध्रुव वर्ण *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    परिणाम = smtp_parse_स्थानाङ्कः(दत्त, str, &स्थानाङ्कः, &host, &suffix);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(स्थानाङ्कः) ||
            !Curl_is_ASCII_name(host.name));

    यदि(host.name) {
      from = curl_maprintf("<%s@%s>%s", स्थानाङ्कः, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    अन्यथा
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", स्थानाङ्कः, suffix);

    curlx_free(स्थानाङ्कः);
  }
  अन्यथा
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  यदि(!from) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(दत्त, STRING_MAIL_AUTH);
  यदि(str && smtpc->sasl.authused) {
    यदि(str[0] != '\0') {
      वर्ण *स्थानाङ्कः = रिक्त;
      संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
      ध्रुव वर्ण *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      परिणाम = smtp_parse_स्थानाङ्कः(दत्त, str, &स्थानाङ्कः, &host, &suffix);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      यदि(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(स्थानाङ्कः) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      यदि(host.name) {
        auth = curl_maprintf("<%s@%s>%s", स्थानाङ्कः, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      अन्यथा
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", स्थानाङ्कः, suffix);
      curlx_free(स्थानाङ्कः);
    }
    अन्यथा
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    यदि(!auth) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_अपरिभाषितम् CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  यदि(IS_MIME_POST(दत्त)) {
    curl_mimepart *postp = दत्त->समुच्चयः.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(अचिह्नित पूर्णाङ्क)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_शीर्षकाणि(postp, दत्त->समुच्चयः.शीर्षकाणि, 0);
    परिणाम = Curl_mime_prepare_शीर्षकाणि(दत्त, postp, रिक्त,
                                       रिक्त, MIMESTRATEGY_MAIL);

    यदि(!परिणाम && !Curl_checkheaders(दत्त, STRCONST("Mime-Version")))
      परिणाम = Curl_mime_सङ्कलय_शीर्षकम्(&postp->curlheaders, "Mime-Version: 1.0");
    यदि(!परिणाम)
      परिणाम = Curl_creader_समुच्चयः_mime(दत्त, postp);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    दत्त->state.infilesize = Curl_creader_योग_दीर्घता(दत्त);
  }
  अन्यथा
#यदि_अन्तः
  {
    परिणाम = Curl_creader_समुच्चयः_fread(दत्त, दत्त->state.infilesize);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /* Calculate the optional SIZE parameter */
  यदि(smtpc->आकार_supported && दत्त->state.infilesize > 0) {
    आकार = curl_maprintf("%" FMT_OFF_T, दत्त->state.infilesize);

    यदि(!आकार) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  यदि(smtpc->utf8_supported && !utf8) {
    संरचना curl_slist *rcpt = smtp->rcpt;

    यावत्(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      यदि(!Curl_is_ASCII_name(rcpt->दत्त))
        utf8 = TRUE;

      rcpt = rcpt->अग्र;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  परिणाम = cr_eob_सङ्कलय(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* Send the MAIL command */
  परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         आकार ? " SIZE=" : "", /* Optional on SIZE support  */
                         आकार ? आकार : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

बहिर्गमनम्:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(आकार);

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_MAIL);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
स्थिर CURLcode smtp_perform_rcpt_गन्तव्यम्(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp)
{
  CURLcode परिणाम = CURLE_OK;
  वर्ण *स्थानाङ्कः = रिक्त;
  संरचना hostname host = { रिक्त, रिक्त, रिक्त, रिक्त };
  ध्रुव वर्ण *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  परिणाम = smtp_parse_स्थानाङ्कः(दत्त, smtp->rcpt->दत्त,
                              &स्थानाङ्कः, &host, &suffix);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Send the RCPT TO command */
  यदि(host.name)
    परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           स्थानाङ्कः, host.name, suffix);
  अन्यथा
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "RCPT TO:<%s>%s",
                           स्थानाङ्कः, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(स्थानाङ्कः);

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_RCPT);

  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
स्थिर CURLcode smtp_perform_quit(संरचना Curl_easy *दत्त,
                                  संरचना smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "%s", "QUIT");

  यदि(!परिणाम)
    smtp_state(दत्त, smtpc, SMTP_QUIT);

  प्रत्यागम परिणाम;
}

/* For the initial server greeting */
स्थिर CURLcode smtp_state_servergreet_resp(संरचना Curl_easy *दत्त,
                                            संरचना smtp_conn *smtpc,
                                            पूर्णाङ्क smtpcode,
                                            smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(smtpcode / 100 != 2) {
    failf(दत्त, "Got unexpected smtp-server response: %d", smtpcode);
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }
  अन्यथा
    परिणाम = smtp_perform_ehlo(दत्त, smtpc);

  प्रत्यागम परिणाम;
}

/* For STARTTLS responses */
स्थिर CURLcode smtp_state_starttls_resp(संरचना Curl_easy *दत्त,
                                         संरचना smtp_conn *smtpc,
                                         पूर्णाङ्क smtpcode,
                                         smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  /* Pipelining in response is forbidden. */
  यदि(smtpc->pp.अतिप्रवाहः)
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

  यदि(smtpcode != 220) {
    यदि(दत्त->समुच्चयः.use_ssl != CURLUSESSL_TRY) {
      failf(दत्त, "STARTTLS denied, code %d", smtpcode);
      परिणाम = CURLE_USE_SSL_FAILED;
    }
    अन्यथा
      परिणाम = smtp_perform_authentication(दत्त, smtpc);
  }
  अन्यथा
    smtp_state(दत्त, smtpc, SMTP_UPGRADETLS);

  प्रत्यागम परिणाम;
}

/* For EHLO responses */
स्थिर CURLcode smtp_state_ehlo_resp(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (शून्य)instate;

  यदि(smtpcode / 100 != 2 && smtpcode != 1) {
    यदि(दत्त->समुच्चयः.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(दत्त->conn, FIRSTSOCKET))
      परिणाम = smtp_perform_helo(दत्त, smtpc);
    अन्यथा {
      failf(दत्त, "Remote access denied: %d", smtpcode);
      परिणाम = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  अन्यथा यदि(len >= 4) {
    पङ्क्तिः += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    यदि(len >= 8 && curl_strnequal(पङ्क्तिः, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    अन्यथा यदि(len >= 4 && curl_strnequal(पङ्क्तिः, "SIZE", 4))
      smtpc->आकार_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    अन्यथा यदि(len >= 8 && curl_strnequal(पङ्क्तिः, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    अन्यथा यदि(len >= 5 && curl_strnequal(पङ्क्तिः, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      पङ्क्तिः += 5;
      len -= 5;

      /* Loop through the data line */
      प्रति(;;) {
        size_t llen;
        size_t wordlen;
        अचिह्नित ह्रस्व mechbit;

        यावत्(len && (ISBLANK(*पङ्क्तिः) || ISNEWLINE(*पङ्क्तिः))) {
          पङ्क्तिः++;
          len--;
        }

        यदि(!len)
          भङ्ग;

        /* Extract the word */
        प्रति(wordlen = 0; wordlen < len && !ISBLANK(पङ्क्तिः[wordlen]) &&
              !ISNEWLINE(पङ्क्तिः[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(पङ्क्तिः, wordlen, &llen);
        यदि(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        पङ्क्तिः += wordlen;
        len -= wordlen;
      }
    }

    यदि(smtpcode != 1) {
      यदि(दत्त->समुच्चयः.use_ssl && !Curl_conn_is_ssl(दत्त->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        यदि(smtpc->tls_supported)
          /* Switch to TLS connection now */
          परिणाम = smtp_perform_starttls(दत्त, smtpc);
        अन्यथा यदि(दत्त->समुच्चयः.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          परिणाम = smtp_perform_authentication(दत्त, smtpc);
        अन्यथा {
          failf(दत्त, "STARTTLS not supported.");
          परिणाम = CURLE_USE_SSL_FAILED;
        }
      }
      अन्यथा
        परिणाम = smtp_perform_authentication(दत्त, smtpc);
    }
  }
  अन्यथा {
    failf(दत्त, "Unexpectedly short EHLO response");
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

  प्रत्यागम परिणाम;
}

/* For HELO responses */
स्थिर CURLcode smtp_state_helo_resp(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(smtpcode / 100 != 2) {
    failf(दत्त, "Remote access denied: %d", smtpcode);
    परिणाम = CURLE_REMOTE_ACCESS_DENIED;
  }
  अन्यथा
    /* End of connect phase */
    smtp_state(दत्त, smtpc, SMTP_STOP);

  प्रत्यागम परिणाम;
}

/* For SASL authentication responses */
स्थिर CURLcode smtp_state_auth_resp(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  saslprogress प्रगतिः;

  (शून्य)instate;

  परिणाम = Curl_sasl_continue(&smtpc->sasl, दत्त, smtpcode, &प्रगतिः);
  यदि(!परिणाम)
    विकल्प(प्रगतिः) {
    पक्ष SASL_समाप्तम्:
      smtp_state(दत्त, smtpc, SMTP_STOP);  /* Authenticated */
      भङ्ग;
    पक्ष SASL_IDLE:            /* No mechanism left after cancellation */
      failf(दत्त, "Authentication cancelled");
      परिणाम = CURLE_LOGIN_DENIED;
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }

  प्रत्यागम परिणाम;
}

/* For command responses */
स्थिर CURLcode smtp_state_command_resp(संरचना Curl_easy *दत्त,
                                        संरचना smtp_conn *smtpc,
                                        संरचना SMTP *smtp,
                                        पूर्णाङ्क smtpcode,
                                        smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *पङ्क्तिः = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (शून्य)instate;

  यदि((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(दत्त, "Command failed: %d", smtpcode);
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }
  अन्यथा {
    यदि(!दत्त->req.no_body)
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, पङ्क्तिः, len);

    यदि(!परिणाम && (smtpcode != 1)) {
      यदि(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->अग्र;

        यदि(smtp->rcpt) {
          /* Send the next command */
          परिणाम = smtp_perform_command(दत्त, smtpc, smtp);
        }
        अन्यथा
          /* End of DO phase */
          smtp_state(दत्त, smtpc, SMTP_STOP);
      }
      अन्यथा
        /* End of DO phase */
        smtp_state(दत्त, smtpc, SMTP_STOP);
    }
  }

  प्रत्यागम परिणाम;
}

/* For MAIL responses */
स्थिर CURLcode smtp_state_mail_resp(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(smtpcode / 100 != 2) {
    failf(दत्त, "MAIL failed: %d", smtpcode);
    परिणाम = CURLE_SEND_ERROR;
  }
  अन्यथा
    /* Start the RCPT TO command */
    परिणाम = smtp_perform_rcpt_गन्तव्यम्(दत्त, smtpc, smtp);

  प्रत्यागम परिणाम;
}

/* For RCPT responses */
स्थिर CURLcode smtp_state_rcpt_resp(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     संरचना SMTP *smtp,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (शून्य)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !दत्त->समुच्चयः.mail_rcpt_allowfails);

  यदि(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_चरमः_error = smtpcode;

    यदि(is_smtp_blocking_err) {
      failf(दत्त, "RCPT failed: %d", smtpcode);
      परिणाम = CURLE_SEND_ERROR;
    }
  }
  अन्यथा {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  यदि(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->अग्र;

    यदि(smtp->rcpt)
      /* Send the next RCPT TO command */
      परिणाम = smtp_perform_rcpt_गन्तव्यम्(दत्त, smtpc, smtp);
    अन्यथा {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      यदि(!smtp->rcpt_had_ok) {
        failf(दत्त, "RCPT failed: %d (last error)", smtp->rcpt_चरमः_error);
        परिणाम = CURLE_SEND_ERROR;
      }
      अन्यथा {
        /* Send the DATA command */
        परिणाम = Curl_pp_sendf(दत्त, &smtpc->pp, "%s", "DATA");

        यदि(!परिणाम)
          smtp_state(दत्त, smtpc, SMTP_दत्त);
      }
    }
  }

  प्रत्यागम परिणाम;
}

/* For DATA response */
स्थिर CURLcode smtp_state_दत्त_resp(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     पूर्णाङ्क smtpcode,
                                     smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)instate;

  यदि(smtpcode != 354) {
    failf(दत्त, "DATA failed: %d", smtpcode);
    परिणाम = CURLE_SEND_ERROR;
  }
  अन्यथा {
    /* Set the progress upload size */
    Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);

    /* SMTP upload */
    Curl_xfer_सज्जता_send(दत्त, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(दत्त, smtpc, SMTP_STOP);
  }

  प्रत्यागम परिणाम;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
स्थिर CURLcode smtp_state_postdata_resp(संरचना Curl_easy *दत्त,
                                         संरचना smtp_conn *smtpc,
                                         पूर्णाङ्क smtpcode,
                                         smtpstate instate)
{
  CURLcode परिणाम = CURLE_OK;

  (शून्य)instate;

  यदि(smtpcode != 250)
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

  /* End of DONE phase */
  smtp_state(दत्त, smtpc, SMTP_STOP);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode smtp_pp_statemachine(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क smtpcode;
  संरचना smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  संरचना SMTP *smtp = Curl_meta_get(दत्त, CURL_META_SMTP_EASY);
  size_t nread = 0;

  यदि(!smtpc || !smtp)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  यदि(smtpc->state == SMTP_UPGRADETLS) {
    परिणाम = smtp_perform_upgrade_tls(दत्त, smtpc);
    यदि(परिणाम || (smtpc->state == SMTP_UPGRADETLS))
      प्रत्यागम परिणाम;
  }

  /* Flush any data that needs to be sent */
  यदि(smtpc->pp.sendleft)
    प्रत्यागम Curl_pp_flushsend(दत्त, &smtpc->pp);

  कुरु {
    /* Read the response from the server */
    परिणाम = Curl_pp_readresp(दत्त, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* Store the latest response for later retrieval if necessary */
    यदि(smtpc->state != SMTP_QUIT && smtpcode != 1)
      दत्त->info.httpcode = smtpcode;

    यदि(!smtpcode)
      भङ्ग;

    /* We have now received a full SMTP server response */
    विकल्प(smtpc->state) {
    पक्ष SMTP_SERVERGREET:
      परिणाम = smtp_state_servergreet_resp(दत्त, smtpc,
                                           smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_EHLO:
      परिणाम = smtp_state_ehlo_resp(दत्त, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_HELO:
      परिणाम = smtp_state_helo_resp(दत्त, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_STARTTLS:
      परिणाम = smtp_state_starttls_resp(दत्त, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      यदि(!परिणाम && (smtpc->state == SMTP_UPGRADETLS))
        गच्छ upgrade_tls;
      भङ्ग;

    पक्ष SMTP_AUTH:
      परिणाम = smtp_state_auth_resp(दत्त, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_COMMAND:
      परिणाम = smtp_state_command_resp(दत्त, smtpc, smtp,
                                       smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_MAIL:
      परिणाम = smtp_state_mail_resp(दत्त, smtpc, smtp, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_RCPT:
      परिणाम = smtp_state_rcpt_resp(दत्त, smtpc, smtp, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_दत्त:
      परिणाम = smtp_state_दत्त_resp(दत्त, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_POSTDATA:
      परिणाम = smtp_state_postdata_resp(दत्त, smtpc, smtpcode, smtpc->state);
      भङ्ग;

    पक्ष SMTP_QUIT:
    स्वभाव:
      /* internal error */
      smtp_state(दत्त, smtpc, SMTP_STOP);
      भङ्ग;
    }
  } यावत्(!परिणाम && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  प्रत्यागम परिणाम;
}

/* Called repeatedly until done from multi.c */
स्थिर CURLcode smtp_multi_statemach(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);

  *समाप्तम् = FALSE;
  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = Curl_pp_statemach(दत्त, &smtpc->pp, FALSE, FALSE);
  *समाप्तम् = (smtpc->state == SMTP_STOP);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode smtp_खण्डम्_statemach(संरचना Curl_easy *दत्त,
                                     संरचना smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode परिणाम = CURLE_OK;

  यावत्(smtpc->state != SMTP_STOP && !परिणाम)
    परिणाम = Curl_pp_statemach(दत्त, &smtpc->pp, TRUE, disconnecting);

  प्रत्यागम परिणाम;
}

/* For the SMTP "protocol connect" and "doing" phases only */
स्थिर CURLcode smtp_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);
  प्रत्यागम smtpc ? Curl_pp_pollset(दत्त, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
स्थिर ध्रुव संरचना SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_सन्देशः,     /* Get SASL response message */
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
स्थिर CURLcode smtp_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);
  CURLcode परिणाम = CURLE_OK;

  *समाप्तम् = FALSE; /* default to not done yet */
  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  PINGPONG_सज्जता(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, दत्त, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(दत्त));

  /* Parse the URL options */
  परिणाम = smtp_parse_url_options(दत्त->conn, smtpc);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Parse the URL path */
  परिणाम = smtp_parse_url_मार्गः(दत्त, smtpc);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Start off waiting for the server greeting response */
  smtp_state(दत्त, smtpc, SMTP_SERVERGREET);

  परिणाम = smtp_multi_statemach(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
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
स्थिर CURLcode smtp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                          bool premature)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;
  संरचना SMTP *smtp = Curl_meta_get(दत्त, CURL_META_SMTP_EASY);

  (शून्य)premature;

  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!smtp)
    प्रत्यागम CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  यदि(स्थितिः) {
    CURL_TRC_M(दत्त, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    परिणाम = स्थितिः;         /* use the already set error code */
  }
  अन्यथा यदि(!दत्त->समुच्चयः.connect_only && दत्त->समुच्चयः.mail_rcpt &&
          (दत्त->state.upload || IS_MIME_POST(दत्त))) {

    smtp_state(दत्त, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    परिणाम = smtp_खण्डम्_statemach(दत्त, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(दत्त, "smtp_done(status=%d, premature=%d) -> %d",
                (पूर्णाङ्क)स्थितिः, premature, (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
स्थिर CURLcode smtp_perform(संरचना Curl_easy *दत्त,
                             संरचना smtp_conn *smtpc,
                             संरचना SMTP *smtp,
                             bool *connected,
                             bool *dophase_समाप्तम्)
{
  /* This is SMTP and no proxy */
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SMTP(दत्त, "smtp_perform(), start");

  यदि(दत्त->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_समाप्तम् = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = दत्त->समुच्चयः.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_चरमः_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  यदि((दत्त->state.upload || IS_MIME_POST(दत्त)) && दत्त->समुच्चयः.mail_rcpt)
    /* MAIL transfer */
    परिणाम = smtp_perform_mail(दत्त, smtpc, smtp);
  अन्यथा
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    परिणाम = smtp_perform_command(दत्त, smtpc, smtp);

  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* Run the state-machine */
  परिणाम = smtp_multi_statemach(दत्त, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET);

बहिर्गमनम्:
  CURL_TRC_SMTP(दत्त, "smtp_perform() -> %d, connected=%d, done=%d",
                (पूर्णाङ्क)परिणाम, *connected, *dophase_समाप्तम्);
  प्रत्यागम परिणाम;
}

/* Call this when the DO phase has completed */
स्थिर CURLcode smtp_dophase_समाप्तम्(संरचना Curl_easy *दत्त,
                                  संरचना SMTP *smtp,
                                  bool connected)
{
  (शून्य)connected;

  यदि(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_सज्जता_nop(दत्त);

  प्रत्यागम CURLE_OK;
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
स्थिर CURLcode smtp_regular_transfer(संरचना Curl_easy *दत्त,
                                      संरचना smtp_conn *smtpc,
                                      संरचना SMTP *smtp,
                                      bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  दत्त->req.आकार = -1;

  /* Set the progress data */
  Curl_pgrsReset(दत्त);

  /* Carry out the perform */
  परिणाम = smtp_perform(दत्त, smtpc, smtp, &connected, dophase_समाप्तम्);

  /* Perform post DO phase operations if necessary */
  यदि(!परिणाम && *dophase_समाप्तम्)
    परिणाम = smtp_dophase_समाप्तम्(दत्त, smtp, connected);

  CURL_TRC_SMTP(दत्त, "smtp_regular_transfer() -> %d, done=%d",
                (पूर्णाङ्क)परिणाम, *dophase_समाप्तम्);
  प्रत्यागम परिणाम;
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
स्थिर CURLcode smtp_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना smtp_conn *smtpc =
    Curl_conn_meta_get(दत्त->conn, CURL_META_SMTP_CONN);
  संरचना SMTP *smtp = Curl_meta_get(दत्त, CURL_META_SMTP_EASY);
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->conn);
  *समाप्तम् = FALSE; /* default to false */
  यदि(!smtpc || !smtp)
    प्रत्यागम CURLE_FAILED_INIT;

  /* Parse the custom request */
  परिणाम = smtp_parse_custom_request(दत्त, smtp);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = smtp_regular_transfer(दत्त, smtpc, smtp, समाप्तम्);
  CURL_TRC_SMTP(दत्त, "smtp_do() -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  प्रत्यागम परिणाम;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
स्थिर CURLcode smtp_disconnect(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  संरचना smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  यदि(!smtpc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  यदि(!dead_connection && conn->द्वयङ्काः.protoconnstart &&
     !Curl_pp_needs_flush(दत्त, &smtpc->pp) &&
     !smtp_perform_quit(दत्त, smtpc))
    (शून्य)smtp_खण्डम्_statemach(दत्त, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(दत्त, "smtp_disconnect(), finished");
  प्रत्यागम CURLE_OK;
}

/* Called from multi.c while DOing */
स्थिर CURLcode smtp_doing(संरचना Curl_easy *दत्त, bool *dophase_समाप्तम्)
{
  संरचना SMTP *smtp = Curl_meta_get(दत्त, CURL_META_SMTP_EASY);
  CURLcode परिणाम;

  यदि(!smtp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = smtp_multi_statemach(दत्त, dophase_समाप्तम्);
  यदि(परिणाम)
    DEBUGF(infof(दत्त, "DO phase failed"));
  अन्यथा यदि(*dophase_समाप्तम्) {
    परिणाम = smtp_dophase_समाप्तम्(दत्त, smtp, FALSE /* not connected */);

    DEBUGF(infof(दत्त, "DO phase is complete"));
  }

  CURL_TRC_SMTP(दत्त, "smtp_doing() -> %d, done=%d", (पूर्णाङ्क)परिणाम,
                *dophase_समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर शून्य smtp_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना SMTP *smtp = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_free(smtp);
}

स्थिर शून्य smtp_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना smtp_conn *smtpc = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

स्थिर CURLcode smtp_सज्जता_connection(संरचना Curl_easy *दत्त,
                                      संरचना connectdata *conn)
{
  संरचना smtp_conn *smtpc;
  संरचना SMTP *smtp;
  CURLcode परिणाम = CURLE_OK;

  smtpc = curlx_calloc(1, परिमाण(*smtpc));
  यदि(!smtpc ||
     Curl_conn_meta_समुच्चयः(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  smtp = curlx_calloc(1, परिमाण(*smtp));
  यदि(!smtp ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

बहिर्गमनम्:
  CURL_TRC_SMTP(दत्त, "smtp_setup_connection() -> %d", (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

/*
 * SMTP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_smtp = {
  smtp_सज्जता_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_समाप्तम्,                        /* done */
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

#यदि_अन्तः /* CURL_DISABLE_SMTP */
