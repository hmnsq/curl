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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "smtp.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "escape.h"
#شامل "pingpong.h"
#شامل "mime.h"
#شامل "vtls/vtls.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "url.h"
#شامل "curl_gethostname.h"
#شامل "bufref.h"
#شامل "curl_sasl.h"
#شامل "idn.h"
#شامل "curlx/strparse.h"

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_SMTP_EASY   "meta:proto:smtp:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کرو CURL_META_SMTP_CONN   "meta:proto:smtp:conn"

/****************************************************************************
 * SMTP unique setup
 ***************************************************************************/
قسمتعریف شمار {
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
  SMTP_ڈیٹا,
  SMTP_POSTDATA,
  SMTP_QUIT,
  SMTP_آخری         /* never used */
} smtpstate;

/* smtp_conn is used for struct connection-oriented data in the connectdata
   struct */
ساخت smtp_conn {
  ساخت pingpong pp;
  ساخت SASL sasl;        /* SASL-related storage */
  smtpstate state;         /* Always use smtp.c:state() to change state! */
  حرف *domain;            /* Client address/name to send in the EHLO */
  ثنائی_ہندسہ(ssldone);            /* Is connect() over SSL done? */
  ثنائی_ہندسہ(tls_supported);      /* StartTLS capability supported by server */
  ثنائی_ہندسہ(سائز_supported);     /* If server supports SIZE extension according to
                              RFC 1870 */
  ثنائی_ہندسہ(utf8_supported);     /* If server supports SMTPUTF8 extension according
                              to RFC 6531 */
  ثنائی_ہندسہ(auth_supported);     /* AUTH capability supported by server */
};

/* This SMTP struct is used in the Curl_easy. All SMTP data that is
   connection-oriented must be in smtp_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ساخت SMTP {
  curl_pp_transfer transfer;
  حرف *custom;            /* Custom Request */
  ساخت curl_slist *rcpt; /* Recipient list */
  صحیح rcpt_آخری_error;     /* The last error received for RCPT TO command */
  size_t eob;              /* Number of bytes of the EOB (End Of Body) that
                              have been received so far */
  ثنائی_ہندسہ(rcpt_had_ok);        /* Whether any of RCPT TO commands (depends on
                              total number of recipients) succeeded so far */
  ثنائی_ہندسہ(trailing_crlf);      /* Specifies if the trailing CRLF is present */
};

/***********************************************************************
 *
 * smtp_parse_url_options()
 *
 * Parse the URL login options.
 */
ساکن CURLcode smtp_parse_url_options(ساخت connectdata *conn,
                                       ساخت smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = conn->options;

  جب(!نتیجہ && ptr && *ptr) {
    ثابت حرف *کلید = ptr;
    ثابت حرف *قدر;

    جب(*ptr && *ptr != '=')
      ptr++;

    قدر = ptr + 1;

    جب(*ptr && *ptr != ';')
      ptr++;

    اگر(curl_strnequal(کلید, "AUTH=", 5))
      نتیجہ = Curl_sasl_parse_url_auth_option(&smtpc->sasl,
                                               قدر, ptr - قدر);
    ورنہ
      نتیجہ = CURLE_URL_MALFORMAT;

    اگر(*ptr == ';')
      ptr++;
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_parse_url_path()
 *
 * Parse the URL path into separate path components.
 */
ساکن CURLcode smtp_parse_url_راستہ(ساخت Curl_easy *ڈیٹا,
                                    ساخت smtp_conn *smtpc)
{
  /* The SMTP struct is already initialized in smtp_connect() */
  ثابت حرف *راستہ = &ڈیٹا->state.اوپر.راستہ[1]; /* skip leading path */
  حرف localhost[HOSTNAME_MAX + 1];

  /* Calculate the path if necessary */
  اگر(!*راستہ) {
    اگر(!Curl_gethostname(localhost, حجم(localhost)))
      راستہ = localhost;
    ورنہ
      راستہ = "localhost";
  }

  /* URL decode the path and use it as the domain in our EHLO */
  واپس Curl_urldecode(راستہ, 0, &smtpc->domain, تہی, REJECT_CTRL);
}

/***********************************************************************
 *
 * smtp_parse_custom_request()
 *
 * Parse the custom request.
 */
ساکن CURLcode smtp_parse_custom_request(ساخت Curl_easy *ڈیٹا,
                                          ساخت SMTP *smtp)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *custom = CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST);

  /* URL decode the custom request */
  اگر(custom)
    نتیجہ = Curl_urldecode(custom, 0, &smtp->custom, تہی, REJECT_CTRL);

  واپس نتیجہ;
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
ساکن CURLcode smtp_parse_پتہ(ساخت Curl_easy *ڈیٹا, ثابت حرف *fqma,
                                   حرف **پتہ, ساخت hostname *host,
                                   ثابت حرف **suffix)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t لمبائی;
  حرف *addressend;
  حرف *dup;

  /* A CR or LF in the address ends up verbatim in the MAIL FROM/RCPT TO
     command line, so a crafted address could smuggle further SMTP commands
     onto the wire. Reject it before the command is built. */
  اگر(strpbrk(fqma, "\r\n")) {
    failf(ڈیٹا, "Refusing to send email address with a CR or LF");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  /* Duplicate the fully qualified email address so we can manipulate it,
     ensuring it does not contain the delimiters if specified */
  dup = curlx_strdup(fqma[0] == '<' ? fqma + 1 : fqma);
  اگر(!dup)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(fqma[0] != '<') {
    لمبائی = لڑی_لمبائی(dup);
    اگر(لمبائی && dup[لمبائی - 1] == '>')
      dup[لمبائی - 1] = '\0';
  }
  ورنہ {
    addressend = strrchr(dup, '>');
    اگر(addressend) {
      *addressend = '\0';
      *suffix = addressend + 1;
    }
  }

  /* Extract the hostname from the address (if we can) */
  host->name = strpbrk(dup, "@");
  اگر(host->name) {
    *host->name = '\0';
    host->name = host->name + 1;

    /* Attempt to convert the hostname to IDN ACE */
    (خالی)Curl_idnconvert_hostname(host);

    /* If Curl_idnconvert_hostname() fails then we shall attempt to continue
       and send the hostname using UTF-8 rather than as 7-bit ACE (which is
       our preference) */
  }

  /* Extract the local address from the mailbox */
  *پتہ = dup;

  واپس نتیجہ;
}

ساخت cr_eob_ctx {
  ساخت Curl_creader super;
  ساخت bufq buf;
  size_t n_eob; /* how many EOB bytes we matched so far */
  size_t eob;       /* Number of bytes of the EOB (End Of Body) that
                       have been received so far */
  ثنائی_ہندسہ(read_eos);  /* we read an EOS from the next reader */
  ثنائی_ہندسہ(processed_eos);  /* we read and processed an EOS */
  ثنائی_ہندسہ(eos);       /* we have returned an EOS */
};

ساکن CURLcode cr_eob_init(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader)
{
  ساخت cr_eob_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  /* The first char we read is the first on a line, as if we had
   * read CRLF before */
  ctx->n_eob = 2;
  Curl_bufq_init2(&ctx->buf, (16 * 1024), 1, BUFQ_OPT_SOFT_حد_بندی);
  واپس CURLE_OK;
}

ساکن خالی cr_eob_close(ساخت Curl_easy *ڈیٹا, ساخت Curl_creader *reader)
{
  ساخت cr_eob_ctx *ctx = reader->ctx;
  (خالی)ڈیٹا;
  Curl_bufq_free(&ctx->buf);
}

/* this is the 5-bytes End-Of-Body marker for SMTP */
#تعریف_کرو SMTP_EOB          "\r\n.\r\n"
#تعریف_کرو SMTP_EOB_ڈھونڈو_LEN 3

/* client reader doing SMTP End-Of-Body escaping. */
ساکن CURLcode cr_eob_read(ساخت Curl_easy *ڈیٹا,
                            ساخت Curl_creader *reader,
                            حرف *buf, size_t blen,
                            size_t *pnread, bool *peos)
{
  ساخت cr_eob_ctx *ctx = reader->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread, i, آغاز, n;
  bool eos;

  اگر(!ctx->read_eos && Curl_bufq_is_خالی_خانے(&ctx->buf)) {
    /* Get more and convert it when needed */
    نتیجہ = Curl_creader_read(ڈیٹا, reader->اگلا, buf, blen, &nread, &eos);
    CURL_TRC_SMTP(ڈیٹا, "cr_eob_read, next_read(len=%zu) -> %d, %zu eos=%d",
                  blen, (صحیح)نتیجہ, nread, eos);
    اگر(نتیجہ)
      واپس نتیجہ;

    ctx->read_eos = eos;
    اگر(nread) {
      اگر(!ctx->n_eob && !memchr(buf, SMTP_EOB[0], nread)) {
        /* not in the middle of a match, no EOB start found, pass */
        *pnread = nread;
        *peos = FALSE;
        واپس CURLE_OK;
      }
      /* scan for EOB (continuation) and convert */
      برائے(i = آغاز = 0; i < nread; ++i) {
        اگر(ctx->n_eob >= SMTP_EOB_ڈھونڈو_LEN) {
          /* matched the EOB prefix and seeing additional char, add '.' */
          نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + آغاز, i - آغاز, &n);
          اگر(نتیجہ)
            واپس نتیجہ;
          نتیجہ = Curl_bufq_cwrite(&ctx->buf, ".", 1, &n);
          اگر(نتیجہ)
            واپس نتیجہ;
          ctx->n_eob = 0;
          آغاز = i;
          اگر(ڈیٹا->state.infilesize > 0)
            ڈیٹا->state.infilesize++;
        }

        اگر(buf[i] != SMTP_EOB[ctx->n_eob])
          ctx->n_eob = 0;

        اگر(buf[i] == SMTP_EOB[ctx->n_eob]) {
          /* matching another char of the EOB */
          ++ctx->n_eob;
        }
      }

      /* add any remainder to buf */
      اگر(آغاز < nread) {
        نتیجہ = Curl_bufq_cwrite(&ctx->buf, buf + آغاز, nread - آغاز, &n);
        اگر(نتیجہ)
          واپس نتیجہ;
      }
    }
  }

  *peos = FALSE;

  اگر(ctx->read_eos && !ctx->processed_eos) {
    /* if we last matched a CRLF or if the data was empty, add ".\r\n"
     * to end the body. If we sent something and it did not end with "\r\n",
     * add "\r\n.\r\n" to end the body */
    ثابت حرف *eob = SMTP_EOB;
    CURL_TRC_SMTP(ڈیٹا, "auto-ending mail body with '\\r\\n.\\r\\n'");
    انتخاب(ctx->n_eob) {
    حالت 2:
      /* seen a CRLF at the end, add the remainder */
      eob = &SMTP_EOB[2];
      توڑو;
    حالت 3:
      /* ended with '\r\n.', we should escape the last '.' */
      eob = "." SMTP_EOB;
      توڑو;
    طےشدہ:
      توڑو;
    }
    نتیجہ = Curl_bufq_cwrite(&ctx->buf, eob, لڑی_لمبائی(eob), &n);
    اگر(نتیجہ)
      واپس نتیجہ;
    ctx->processed_eos = TRUE;
  }

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->buf)) {
    نتیجہ = Curl_bufq_cread(&ctx->buf, buf, blen, pnread);
  }
  ورنہ
    *pnread = 0;

  اگر(ctx->read_eos && Curl_bufq_is_خالی_خانے(&ctx->buf)) {
    /* no more data, read all, done. */
    CURL_TRC_SMTP(ڈیٹا, "mail body complete, returning EOS");
    ctx->eos = TRUE;
  }
  *peos = (bool)ctx->eos;
  DEBUGF(infof(ڈیٹا, "cr_eob_read(%zu) -> %d, %zu, %d",
               blen, (صحیح)نتیجہ, *pnread, *peos));
  واپس نتیجہ;
}

ساکن curl_off_t cr_eob_کل_لمبائی(ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_creader *reader)
{
  /* this reader changes length depending on input */
  (خالی)ڈیٹا;
  (خالی)reader;
  واپس -1;
}

ساکن ثابت ساخت Curl_crtype cr_eob = {
  "cr-smtp-eob",
  cr_eob_init,
  cr_eob_read,
  cr_eob_close,
  Curl_creader_def_needs_rewind,
  cr_eob_کل_لمبائی,
  Curl_creader_def_resume_from,
  Curl_creader_def_cntrl,
  Curl_creader_def_is_paused,
  Curl_creader_def_مکمل,
  حجم(ساخت cr_eob_ctx)
};

ساکن CURLcode cr_eob_جوڑو(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_creader *reader = تہی;
  CURLcode نتیجہ;

  نتیجہ = Curl_creader_create(&reader, ڈیٹا, &cr_eob, CURL_CR_CONTENT_ENCODE);
  اگر(!نتیجہ)
    نتیجہ = Curl_creader_جوڑو(ڈیٹا, reader);

  اگر(نتیجہ && reader)
    Curl_creader_free(ڈیٹا, reader);
  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_endofresp()
 *
 * Checks for an ending SMTP status code at the start of the given string, but
 * also detects various capabilities from the EHLO response including the
 * supported authentication mechanisms.
 */
ساکن bool smtp_endofresp(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                           ثابت حرف *سطر, size_t len, صحیح *resp)
{
  ساخت smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  bool انتہا = FALSE;
  (خالی)ڈیٹا;

  DEBUGASSERT(smtpc);
  اگر(!smtpc)
    واپس FALSE;

  /* Nothing for us */
  اگر(len < 4 || !ISDIGIT(سطر[0]) || !ISDIGIT(سطر[1]) || !ISDIGIT(سطر[2]))
    واپس FALSE;

  /* Do we have a command response? This should be the response code followed
     by a space and optionally some text as per RFC-5321 and as outlined in
     Section 4. Examples of RFC-4954 but some email servers ignore this and
     only send the response code instead as per Section 4.2. */
  اگر(سطر[3] == ' ' || len == 5) {
    حرف tmpline[6];
    curl_off_t رمز;
    ثابت حرف *p = tmpline;
    انتہا = TRUE;
    حافظہ_نقل(tmpline, سطر, (len == 5 ? 5 : 3));
    tmpline[len == 5 ? 5 : 3] = 0;
    اگر(curlx_str_نمبر(&p, &رمز, len == 5 ? 99999 : 999))
      واپس FALSE;
    *resp = (صحیح)رمز;

    /* Make sure real server never sends internal value */
    اگر(*resp == 1)
      *resp = 0;
  }
  /* Do we have a multiline (continuation) response? */
  ورنہ اگر(سطر[3] == '-' &&
          (smtpc->state == SMTP_EHLO || smtpc->state == SMTP_COMMAND)) {
    انتہا = TRUE;
    *resp = 1;  /* Internal response code */
  }

  واپس انتہا;
}

/***********************************************************************
 *
 * smtp_get_message()
 *
 * Gets the authentication message from the response buffer.
 */
ساکن CURLcode smtp_get_پیغام(ساخت Curl_easy *ڈیٹا, ساخت bufref *برآمد)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);
  حرف *پیغام_2;
  size_t len;

  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;

  پیغام_2 = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  len = smtpc->pp.nfinal;
  اگر(len > 4) {
    /* Find the start of the message */
    len -= 4;
    برائے(پیغام_2 += 4; ISBLANK(*پیغام_2); پیغام_2++, len--)
      ;

    /* Find the end of the message */
    جب(len--)
      اگر(!ISNEWLINE(پیغام_2[len]) && !ISBLANK(پیغام_2[len]))
        توڑو;

    /* Terminate the message */
    پیغام_2[++len] = '\0';
    Curl_bufref_سیٹ(برآمد, پیغام_2, len, تہی);
  }
  ورنہ
    /* junk input => zero length output */
    Curl_bufref_سیٹ(برآمد, "", 0, تہی);

  واپس CURLE_OK;
}

/***********************************************************************
 *
 * smtp_state()
 *
 * This is the ONLY way to change SMTP state!
 */
ساکن خالی smtp_state(ساخت Curl_easy *ڈیٹا,
                       ساخت smtp_conn *smtpc,
                       smtpstate newstate)
{
#اگر_تعریف_شدہ CURLVERBOSE
  /* for debug purposes */
  ساکن ثابت حرف * ثابت نام[] = {
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
    /*آخری*/
  };

  اگر(smtpc->state != newstate)
    CURL_TRC_SMTP(ڈیٹا, "state change from %s to %s",
                  نام[smtpc->state], نام[newstate]);
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم

  smtpc->state = newstate;
}

/***********************************************************************
 *
 * smtp_perform_ehlo()
 *
 * Sends the EHLO command to not only initialize communication with the ESMTP
 * server but to also obtain a list of server side supported capabilities.
 */
ساکن CURLcode smtp_perform_ehlo(ساخت Curl_easy *ڈیٹا,
                                  ساخت smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;

  smtpc->sasl.authmechs = SASL_AUTH_NONE; /* No known auth. mechanism yet */
  smtpc->sasl.authused = SASL_AUTH_NONE;  /* Clear the authentication mechanism
                                             used for esmtp connections */
  smtpc->tls_supported = FALSE;           /* Clear the TLS capability */
  smtpc->auth_supported = FALSE;          /* Clear the AUTH capability */

  /* Send the EHLO command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "EHLO %s", smtpc->domain);

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_EHLO);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_helo()
 *
 * Sends the HELO command to initialize communication with the SMTP server.
 */
ساکن CURLcode smtp_perform_helo(ساخت Curl_easy *ڈیٹا,
                                  ساخت smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;

  smtpc->sasl.authused = SASL_AUTH_NONE; /* No authentication mechanism used
                                            in smtp connections */

  /* Send the HELO command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "HELO %s", smtpc->domain);

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_HELO);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_starttls()
 *
 * Sends the STLS command to start the upgrade to TLS.
 */
ساکن CURLcode smtp_perform_starttls(ساخت Curl_easy *ڈیٹا,
                                      ساخت smtp_conn *smtpc)
{
  /* Send the STARTTLS command */
  CURLcode نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "%s", "STARTTLS");

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_STARTTLS);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_upgrade_tls()
 *
 * Performs the upgrade to TLS.
 */
ساکن CURLcode smtp_perform_upgrade_tls(ساخت Curl_easy *ڈیٹا,
                                         ساخت smtp_conn *smtpc)
{
#اگر_تعریف_شدہ USE_SSL
  /* Start the SSL connection */
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ;
  bool ssldone = FALSE;

  DEBUGASSERT(smtpc->state == SMTP_UPGRADETLS);
  اگر(!Curl_conn_is_ssl(conn, FIRSTSOCKET)) {
    نتیجہ = Curl_ssl_cfilter_جوڑو(
      ڈیٹا, Curl_conn_get_origin(conn, FIRSTSOCKET), conn, FIRSTSOCKET);
    اگر(نتیجہ)
      جاؤ برآمد;
    /* Change the connection handler and SMTP state */
    conn->scheme = &Curl_scheme_smtps;
  }

  DEBUGASSERT(!smtpc->ssldone);
  نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, FALSE, &ssldone);
  DEBUGF(infof(ڈیٹا, "smtp_perform_upgrade_tls, connect -> %d, %d",
               (صحیح)نتیجہ, ssldone));
  اگر(!نتیجہ && ssldone) {
    smtpc->ssldone = ssldone;
    /* perform EHLO now, changes smtp->state out of SMTP_UPGRADETLS */
    نتیجہ = smtp_perform_ehlo(ڈیٹا, smtpc);
  }
برآمد:
  واپس نتیجہ;
#ورنہ
  (خالی)ڈیٹا;
  (خالی)smtpc;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

/***********************************************************************
 *
 * smtp_perform_auth()
 *
 * Sends an AUTH command allowing the client to login with the given SASL
 * authentication mechanism.
 */
ساکن CURLcode smtp_perform_auth(ساخت Curl_easy *ڈیٹا,
                                  ثابت حرف *mech,
                                  ثابت ساخت bufref *initresp)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);
  ثابت حرف *ir = Curl_bufref_ptr(initresp);

  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;

  اگر(ir) {                                  /* AUTH <mech> ...<crlf> */
    /* Send the AUTH command with the initial response */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "AUTH %s %s",
                           mech, *ir ? ir : "=");
  }
  ورنہ {
    /* Send the AUTH command */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "AUTH %s", mech);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_continue_auth()
 *
 * Sends SASL continuation data.
 */
ساکن CURLcode smtp_continue_auth(ساخت Curl_easy *ڈیٹا,
                                   ثابت حرف *mech,
                                   ثابت ساخت bufref *resp)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);

  (خالی)mech;
  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;
  واپس Curl_pp_sendf(ڈیٹا, &smtpc->pp, "%s", Curl_bufref_ptr(resp));
}

/***********************************************************************
 *
 * smtp_cancel_auth()
 *
 * Sends SASL cancellation.
 */
ساکن CURLcode smtp_cancel_auth(ساخت Curl_easy *ڈیٹا, ثابت حرف *mech)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);

  (خالی)mech;
  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;
  واپس Curl_pp_sendf(ڈیٹا, &smtpc->pp, "*");
}

/***********************************************************************
 *
 * smtp_perform_authentication()
 *
 * Initiates the authentication sequence, with the appropriate SASL
 * authentication mechanism.
 */
ساکن CURLcode smtp_perform_authentication(ساخت Curl_easy *ڈیٹا,
                                            ساخت smtp_conn *smtpc)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress پیش_رفت;

  /* Check we have enough data to authenticate with, and the
     server supports authentication, and end the connect phase if not */
  اگر(!smtpc->auth_supported ||
     !Curl_sasl_can_authenticate(&smtpc->sasl, ڈیٹا)) {
    smtp_state(ڈیٹا, smtpc, SMTP_STOP);
    واپس نتیجہ;
  }

  /* Calculate the SASL login details */
  نتیجہ = Curl_sasl_آغاز(&smtpc->sasl, ڈیٹا, FALSE, &پیش_رفت);

  اگر(!نتیجہ) {
    اگر(پیش_رفت == SASL_INPROGRESS)
      smtp_state(ڈیٹا, smtpc, SMTP_AUTH);
    ورنہ
      نتیجہ = Curl_sasl_is_blocked(&smtpc->sasl, ڈیٹا);
  }

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_command()
 *
 * Sends an SMTP based command.
 */
ساکن CURLcode smtp_perform_command(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     ساخت SMTP *smtp)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(smtp->rcpt) {
    /* We notify the server we are sending UTF-8 data if a) it supports the
       SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
       either the local address or hostname parts. This is regardless of
       whether the hostname is encoded using IDN ACE */
    bool utf8 = FALSE;

    اگر(!smtp->custom || !smtp->custom[0]) {
      حرف *پتہ = تہی;
      ساخت hostname host = { تہی, تہی, تہی, تہی };
      ثابت حرف *suffix = "";

      /* Parse the mailbox to verify into the local address and hostname
         parts, converting the hostname to an IDN A-label if necessary */
      نتیجہ = smtp_parse_پتہ(ڈیٹا, smtp->rcpt->ڈیٹا,
                                  &پتہ, &host, &suffix);
      اگر(نتیجہ)
        واپس نتیجہ;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported &&
             (host.encalloc ||
              !Curl_is_ASCII_name(پتہ) ||
              !Curl_is_ASCII_name(host.name));

      /* Send the VRFY command (Note: The hostname part may be absent when the
         host is a local system) */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "VRFY %s%s%s%s",
                             پتہ,
                             host.name ? "@" : "",
                             host.name ? host.name : "",
                             utf8 ? " SMTPUTF8" : "");

      Curl_free_idnconverted_hostname(&host);
      curlx_free(پتہ);
    }
    ورنہ {
      /* Establish whether we should report that we support SMTPUTF8 for EXPN
         commands to the server as per RFC-6531 sect. 3.1 point 6 */
      utf8 = smtpc->utf8_supported && !strcmp(smtp->custom, "EXPN");

      /* Send the custom recipient based command such as the EXPN command */
      نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp,
                             "%s %s%s", smtp->custom,
                             smtp->rcpt->ڈیٹا,
                             utf8 ? " SMTPUTF8" : "");
    }
  }
  ورنہ
    /* Send the non-recipient based command such as HELP */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "%s",
                           smtp->custom && smtp->custom[0] != '\0' ?
                           smtp->custom : "HELP");

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_COMMAND);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_mail()
 *
 * Sends an MAIL command to initiate the upload of a message.
 */
ساکن CURLcode smtp_perform_mail(ساخت Curl_easy *ڈیٹا,
                                  ساخت smtp_conn *smtpc,
                                  ساخت SMTP *smtp)
{
  حرف *from = تہی;
  حرف *auth = تہی;
  حرف *سائز = تہی;
  ثابت حرف *str;
  CURLcode نتیجہ = CURLE_OK;

  /* We notify the server we are sending UTF-8 data if a) it supports the
     SMTPUTF8 extension and b) The mailbox contains UTF-8 characters, in
     either the local address or hostname parts. This is regardless of
     whether the hostname is encoded using IDN ACE */
  bool utf8 = FALSE;

  /* Calculate the FROM parameter */
  str = CURL_EASY_STR(ڈیٹا, STRING_MAIL_FROM);
  اگر(str) {
    حرف *پتہ = تہی;
    ساخت hostname host = { تہی, تہی, تہی, تہی };
    ثابت حرف *suffix = "";

    /* Parse the FROM mailbox into the local address and hostname parts,
       converting the hostname to an IDN A-label if necessary */
    نتیجہ = smtp_parse_پتہ(ڈیٹا, str, &پتہ, &host, &suffix);
    اگر(نتیجہ)
      جاؤ برآمد;

    /* Establish whether we should report SMTPUTF8 to the server for this
       mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
    utf8 = smtpc->utf8_supported &&
           (host.encalloc ||
            !Curl_is_ASCII_name(پتہ) ||
            !Curl_is_ASCII_name(host.name));

    اگر(host.name) {
      from = curl_maprintf("<%s@%s>%s", پتہ, host.name, suffix);

      Curl_free_idnconverted_hostname(&host);
    }
    ورنہ
      /* An invalid mailbox was provided but we let the server worry
         about that and reply with a 501 error */
      from = curl_maprintf("<%s>%s", پتہ, suffix);

    curlx_free(پتہ);
  }
  ورنہ
    /* Null reverse-path, RFC-5321, sect. 3.6.3 */
    from = curlx_strdup("<>");

  اگر(!from) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  /* Calculate the optional AUTH parameter */
  str = CURL_EASY_STR(ڈیٹا, STRING_MAIL_AUTH);
  اگر(str && smtpc->sasl.authused) {
    اگر(str[0] != '\0') {
      حرف *پتہ = تہی;
      ساخت hostname host = { تہی, تہی, تہی, تہی };
      ثابت حرف *suffix = "";

      /* Parse the AUTH mailbox into the local address and hostname parts,
         converting the hostname to an IDN A-label if necessary */
      نتیجہ = smtp_parse_پتہ(ڈیٹا, str, &پتہ, &host, &suffix);
      اگر(نتیجہ)
        جاؤ برآمد;

      /* Establish whether we should report SMTPUTF8 to the server for this
         mailbox as per RFC-6531 sect. 3.1 point 4 and sect. 3.4 */
      اگر(!utf8 && smtpc->utf8_supported &&
         (host.encalloc ||
          !Curl_is_ASCII_name(پتہ) ||
          !Curl_is_ASCII_name(host.name)))
        utf8 = TRUE;

      اگر(host.name) {
        auth = curl_maprintf("<%s@%s>%s", پتہ, host.name, suffix);

        Curl_free_idnconverted_hostname(&host);
      }
      ورنہ
        /* An invalid mailbox was provided but we let the server worry
           about it */
        auth = curl_maprintf("<%s>%s", پتہ, suffix);
      curlx_free(پتہ);
    }
    ورنہ
      /* Empty AUTH, RFC-2554, sect. 5 */
      auth = curlx_strdup("<>");

    اگر(!auth) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_MIME
  /* Prepare the mime data if some. */
  اگر(IS_MIME_POST(ڈیٹا)) {
    curl_mimepart *postp = ڈیٹا->سیٹ.mimepostp;

    /* Use the whole structure as data. */
    postp->flags &= ~(غیرعلامتی صحیح)MIME_BODY_ONLY;

    /* Add external headers and mime version. */
    curl_mime_سرخیاں(postp, ڈیٹا->سیٹ.سرخیاں, 0);
    نتیجہ = Curl_mime_prepare_سرخیاں(ڈیٹا, postp, تہی,
                                       تہی, MIMESTRATEGY_MAIL);

    اگر(!نتیجہ && !Curl_checkheaders(ڈیٹا, STRCONST("Mime-Version")))
      نتیجہ = Curl_mime_جوڑو_سرخی(&postp->curlheaders, "Mime-Version: 1.0");
    اگر(!نتیجہ)
      نتیجہ = Curl_creader_سیٹ_mime(ڈیٹا, postp);
    اگر(نتیجہ)
      جاؤ برآمد;
    ڈیٹا->state.infilesize = Curl_creader_کل_لمبائی(ڈیٹا);
  }
  ورنہ
#اگر_ختم
  {
    نتیجہ = Curl_creader_سیٹ_fread(ڈیٹا, ڈیٹا->state.infilesize);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /* Calculate the optional SIZE parameter */
  اگر(smtpc->سائز_supported && ڈیٹا->state.infilesize > 0) {
    سائز = curl_maprintf("%" FMT_OFF_T, ڈیٹا->state.infilesize);

    اگر(!سائز) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  /* If the mailboxes in the FROM and AUTH parameters do not include a UTF-8
     based address then quickly scan through the recipient list and check if
     any there do, as we need to correctly identify our support for SMTPUTF8
     in the envelope, as per RFC-6531 sect. 3.4 */
  اگر(smtpc->utf8_supported && !utf8) {
    ساخت curl_slist *rcpt = smtp->rcpt;

    جب(rcpt && !utf8) {
      /* Does the hostname contain non-ASCII characters? */
      اگر(!Curl_is_ASCII_name(rcpt->ڈیٹا))
        utf8 = TRUE;

      rcpt = rcpt->اگلا;
    }
  }

  /* Add the client reader doing STMP EOB escaping */
  نتیجہ = cr_eob_جوڑو(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* Send the MAIL command */
  نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp,
                         "MAIL FROM:%s%s%s%s%s%s",
                         from,                 /* Mandatory                 */
                         auth ? " AUTH=" : "", /* Optional on AUTH support  */
                         auth ? auth : "",
                         سائز ? " SIZE=" : "", /* Optional on SIZE support  */
                         سائز ? سائز : "",
                         utf8 ? " SMTPUTF8"    /* Internationalised mailbox */
                               : "");          /* included in our envelope  */

برآمد:
  curlx_free(from);
  curlx_free(auth);
  curlx_free(سائز);

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_MAIL);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_rcpt_to()
 *
 * Sends a RCPT TO command for a given recipient as part of the message upload
 * process.
 */
ساکن CURLcode smtp_perform_rcpt_مقصد(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     ساخت SMTP *smtp)
{
  CURLcode نتیجہ = CURLE_OK;
  حرف *پتہ = تہی;
  ساخت hostname host = { تہی, تہی, تہی, تہی };
  ثابت حرف *suffix = "";

  /* Parse the recipient mailbox into the local address and hostname parts,
     converting the hostname to an IDN A-label if necessary */
  نتیجہ = smtp_parse_پتہ(ڈیٹا, smtp->rcpt->ڈیٹا,
                              &پتہ, &host, &suffix);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Send the RCPT TO command */
  اگر(host.name)
    نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "RCPT TO:<%s@%s>%s",
                           پتہ, host.name, suffix);
  ورنہ
    /* An invalid mailbox was provided but we let the server worry about
       that and reply with a 501 error */
    نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "RCPT TO:<%s>%s",
                           پتہ, suffix);

  Curl_free_idnconverted_hostname(&host);
  curlx_free(پتہ);

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_RCPT);

  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform_quit()
 *
 * Performs the quit action prior to sclose() being called.
 */
ساکن CURLcode smtp_perform_quit(ساخت Curl_easy *ڈیٹا,
                                  ساخت smtp_conn *smtpc)
{
  /* Send the QUIT command */
  CURLcode نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "%s", "QUIT");

  اگر(!نتیجہ)
    smtp_state(ڈیٹا, smtpc, SMTP_QUIT);

  واپس نتیجہ;
}

/* For the initial server greeting */
ساکن CURLcode smtp_state_servergreet_resp(ساخت Curl_easy *ڈیٹا,
                                            ساخت smtp_conn *smtpc,
                                            صحیح smtpcode,
                                            smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(smtpcode / 100 != 2) {
    failf(ڈیٹا, "Got unexpected smtp-server response: %d", smtpcode);
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }
  ورنہ
    نتیجہ = smtp_perform_ehlo(ڈیٹا, smtpc);

  واپس نتیجہ;
}

/* For STARTTLS responses */
ساکن CURLcode smtp_state_starttls_resp(ساخت Curl_easy *ڈیٹا,
                                         ساخت smtp_conn *smtpc,
                                         صحیح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  /* Pipelining in response is forbidden. */
  اگر(smtpc->pp.حد_سے_تجاوز)
    واپس CURLE_WEIRD_SERVER_جواب;

  اگر(smtpcode != 220) {
    اگر(ڈیٹا->سیٹ.use_ssl != CURLUSESSL_TRY) {
      failf(ڈیٹا, "STARTTLS denied, code %d", smtpcode);
      نتیجہ = CURLE_USE_SSL_FAILED;
    }
    ورنہ
      نتیجہ = smtp_perform_authentication(ڈیٹا, smtpc);
  }
  ورنہ
    smtp_state(ڈیٹا, smtpc, SMTP_UPGRADETLS);

  واپس نتیجہ;
}

/* For EHLO responses */
ساکن CURLcode smtp_state_ehlo_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     صحیح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (خالی)instate;

  اگر(smtpcode / 100 != 2 && smtpcode != 1) {
    اگر(ڈیٹا->سیٹ.use_ssl <= CURLUSESSL_TRY ||
       Curl_conn_is_ssl(ڈیٹا->conn, FIRSTSOCKET))
      نتیجہ = smtp_perform_helo(ڈیٹا, smtpc);
    ورنہ {
      failf(ڈیٹا, "Remote access denied: %d", smtpcode);
      نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
    }
  }
  ورنہ اگر(len >= 4) {
    سطر += 4;
    len -= 4;

    /* Does the server support the STARTTLS capability? */
    اگر(len >= 8 && curl_strnequal(سطر, "STARTTLS", 8))
      smtpc->tls_supported = TRUE;

    /* Does the server support the SIZE capability? */
    ورنہ اگر(len >= 4 && curl_strnequal(سطر, "SIZE", 4))
      smtpc->سائز_supported = TRUE;

    /* Does the server support the UTF-8 capability? */
    ورنہ اگر(len >= 8 && curl_strnequal(سطر, "SMTPUTF8", 8))
      smtpc->utf8_supported = TRUE;

    /* Does the server support authentication? */
    ورنہ اگر(len >= 5 && curl_strnequal(سطر, "AUTH ", 5)) {
      smtpc->auth_supported = TRUE;

      /* Advance past the AUTH keyword */
      سطر += 5;
      len -= 5;

      /* Loop through the data line */
      برائے(;;) {
        size_t llen;
        size_t wordlen;
        غیرعلامتی مختصر mechbit;

        جب(len && (ISBLANK(*سطر) || ISNEWLINE(*سطر))) {
          سطر++;
          len--;
        }

        اگر(!len)
          توڑو;

        /* Extract the word */
        برائے(wordlen = 0; wordlen < len && !ISBLANK(سطر[wordlen]) &&
              !ISNEWLINE(سطر[wordlen]);)
          wordlen++;

        /* Test the word for a matching authentication mechanism */
        mechbit = Curl_sasl_decode_mech(سطر, wordlen, &llen);
        اگر(mechbit && llen == wordlen)
          smtpc->sasl.authmechs |= mechbit;

        سطر += wordlen;
        len -= wordlen;
      }
    }

    اگر(smtpcode != 1) {
      اگر(ڈیٹا->سیٹ.use_ssl && !Curl_conn_is_ssl(ڈیٹا->conn, FIRSTSOCKET)) {
        /* We do not have an SSL/TLS connection yet, but SSL is requested */
        اگر(smtpc->tls_supported)
          /* Switch to TLS connection now */
          نتیجہ = smtp_perform_starttls(ڈیٹا, smtpc);
        ورنہ اگر(ڈیٹا->سیٹ.use_ssl == CURLUSESSL_TRY)
          /* Fallback and carry on with authentication */
          نتیجہ = smtp_perform_authentication(ڈیٹا, smtpc);
        ورنہ {
          failf(ڈیٹا, "STARTTLS not supported.");
          نتیجہ = CURLE_USE_SSL_FAILED;
        }
      }
      ورنہ
        نتیجہ = smtp_perform_authentication(ڈیٹا, smtpc);
    }
  }
  ورنہ {
    failf(ڈیٹا, "Unexpectedly short EHLO response");
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }

  واپس نتیجہ;
}

/* For HELO responses */
ساکن CURLcode smtp_state_helo_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     صحیح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(smtpcode / 100 != 2) {
    failf(ڈیٹا, "Remote access denied: %d", smtpcode);
    نتیجہ = CURLE_REMOTE_ACCESS_DENIED;
  }
  ورنہ
    /* End of connect phase */
    smtp_state(ڈیٹا, smtpc, SMTP_STOP);

  واپس نتیجہ;
}

/* For SASL authentication responses */
ساکن CURLcode smtp_state_auth_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     صحیح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  saslprogress پیش_رفت;

  (خالی)instate;

  نتیجہ = Curl_sasl_continue(&smtpc->sasl, ڈیٹا, smtpcode, &پیش_رفت);
  اگر(!نتیجہ)
    انتخاب(پیش_رفت) {
    حالت SASL_مکمل:
      smtp_state(ڈیٹا, smtpc, SMTP_STOP);  /* Authenticated */
      توڑو;
    حالت SASL_IDLE:            /* No mechanism left after cancellation */
      failf(ڈیٹا, "Authentication cancelled");
      نتیجہ = CURLE_LOGIN_DENIED;
      توڑو;
    طےشدہ:
      توڑو;
    }

  واپس نتیجہ;
}

/* For command responses */
ساکن CURLcode smtp_state_command_resp(ساخت Curl_easy *ڈیٹا,
                                        ساخت smtp_conn *smtpc,
                                        ساخت SMTP *smtp,
                                        صحیح smtpcode,
                                        smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *سطر = curlx_dyn_ptr(&smtpc->pp.recvbuf);
  size_t len = smtpc->pp.nfinal;

  (خالی)instate;

  اگر((smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 553 && smtpcode != 1) ||
     (!smtp->rcpt && smtpcode / 100 != 2 && smtpcode != 1)) {
    failf(ڈیٹا, "Command failed: %d", smtpcode);
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }
  ورنہ {
    اگر(!ڈیٹا->req.no_body)
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, سطر, len);

    اگر(!نتیجہ && (smtpcode != 1)) {
      اگر(smtp->rcpt) {
        smtp->rcpt = smtp->rcpt->اگلا;

        اگر(smtp->rcpt) {
          /* Send the next command */
          نتیجہ = smtp_perform_command(ڈیٹا, smtpc, smtp);
        }
        ورنہ
          /* End of DO phase */
          smtp_state(ڈیٹا, smtpc, SMTP_STOP);
      }
      ورنہ
        /* End of DO phase */
        smtp_state(ڈیٹا, smtpc, SMTP_STOP);
    }
  }

  واپس نتیجہ;
}

/* For MAIL responses */
ساکن CURLcode smtp_state_mail_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     ساخت SMTP *smtp,
                                     صحیح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(smtpcode / 100 != 2) {
    failf(ڈیٹا, "MAIL failed: %d", smtpcode);
    نتیجہ = CURLE_SEND_ERROR;
  }
  ورنہ
    /* Start the RCPT TO command */
    نتیجہ = smtp_perform_rcpt_مقصد(ڈیٹا, smtpc, smtp);

  واپس نتیجہ;
}

/* For RCPT responses */
ساکن CURLcode smtp_state_rcpt_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     ساخت SMTP *smtp,
                                     صحیح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  bool is_smtp_err = FALSE;
  bool is_smtp_blocking_err = FALSE;

  (خالی)instate;

  is_smtp_err = (smtpcode / 100 != 2);

  /* If there are multiple RCPT TO commands to issue, it is possible to
     ignore errors and proceed with only the valid addresses. */
  is_smtp_blocking_err = (is_smtp_err && !ڈیٹا->سیٹ.mail_rcpt_allowfails);

  اگر(is_smtp_err) {
    /* Remembering the last failure which we can report if all "RCPT TO" have
       failed and we cannot proceed. */
    smtp->rcpt_آخری_error = smtpcode;

    اگر(is_smtp_blocking_err) {
      failf(ڈیٹا, "RCPT failed: %d", smtpcode);
      نتیجہ = CURLE_SEND_ERROR;
    }
  }
  ورنہ {
    /* Some RCPT TO commands have succeeded. */
    smtp->rcpt_had_ok = TRUE;
  }

  اگر(!is_smtp_blocking_err) {
    smtp->rcpt = smtp->rcpt->اگلا;

    اگر(smtp->rcpt)
      /* Send the next RCPT TO command */
      نتیجہ = smtp_perform_rcpt_مقصد(ڈیٹا, smtpc, smtp);
    ورنہ {
      /* We were not able to issue a successful RCPT TO command while going
         over recipients (potentially multiple). Sending back last error. */
      اگر(!smtp->rcpt_had_ok) {
        failf(ڈیٹا, "RCPT failed: %d (last error)", smtp->rcpt_آخری_error);
        نتیجہ = CURLE_SEND_ERROR;
      }
      ورنہ {
        /* Send the DATA command */
        نتیجہ = Curl_pp_sendf(ڈیٹا, &smtpc->pp, "%s", "DATA");

        اگر(!نتیجہ)
          smtp_state(ڈیٹا, smtpc, SMTP_ڈیٹا);
      }
    }
  }

  واپس نتیجہ;
}

/* For DATA response */
ساکن CURLcode smtp_state_ڈیٹا_resp(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     صحیح smtpcode,
                                     smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)instate;

  اگر(smtpcode != 354) {
    failf(ڈیٹا, "DATA failed: %d", smtpcode);
    نتیجہ = CURLE_SEND_ERROR;
  }
  ورنہ {
    /* Set the progress upload size */
    Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);

    /* SMTP upload */
    Curl_xfer_تیاری_send(ڈیٹا, FIRSTSOCKET);

    /* End of DO phase */
    smtp_state(ڈیٹا, smtpc, SMTP_STOP);
  }

  واپس نتیجہ;
}

/* For POSTDATA responses, which are received after the entire DATA
   part has been sent to the server */
ساکن CURLcode smtp_state_postdata_resp(ساخت Curl_easy *ڈیٹا,
                                         ساخت smtp_conn *smtpc,
                                         صحیح smtpcode,
                                         smtpstate instate)
{
  CURLcode نتیجہ = CURLE_OK;

  (خالی)instate;

  اگر(smtpcode != 250)
    نتیجہ = CURLE_WEIRD_SERVER_جواب;

  /* End of DONE phase */
  smtp_state(ڈیٹا, smtpc, SMTP_STOP);

  واپس نتیجہ;
}

ساکن CURLcode smtp_pp_statemachine(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح smtpcode;
  ساخت smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);
  ساخت SMTP *smtp = Curl_meta_get(ڈیٹا, CURL_META_SMTP_EASY);
  size_t nread = 0;

  اگر(!smtpc || !smtp)
    واپس CURLE_FAILED_INIT;

  /* Busy upgrading the connection; right now all I/O is SSL/TLS, not SMTP */
upgrade_tls:
  اگر(smtpc->state == SMTP_UPGRADETLS) {
    نتیجہ = smtp_perform_upgrade_tls(ڈیٹا, smtpc);
    اگر(نتیجہ || (smtpc->state == SMTP_UPGRADETLS))
      واپس نتیجہ;
  }

  /* Flush any data that needs to be sent */
  اگر(smtpc->pp.sendleft)
    واپس Curl_pp_flushsend(ڈیٹا, &smtpc->pp);

  کرو {
    /* Read the response from the server */
    نتیجہ = Curl_pp_readresp(ڈیٹا, FIRSTSOCKET, &smtpc->pp,
                              &smtpcode, &nread);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* Store the latest response for later retrieval if necessary */
    اگر(smtpc->state != SMTP_QUIT && smtpcode != 1)
      ڈیٹا->info.httpcode = smtpcode;

    اگر(!smtpcode)
      توڑو;

    /* We have now received a full SMTP server response */
    انتخاب(smtpc->state) {
    حالت SMTP_SERVERGREET:
      نتیجہ = smtp_state_servergreet_resp(ڈیٹا, smtpc,
                                           smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_EHLO:
      نتیجہ = smtp_state_ehlo_resp(ڈیٹا, smtpc, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_HELO:
      نتیجہ = smtp_state_helo_resp(ڈیٹا, smtpc, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_STARTTLS:
      نتیجہ = smtp_state_starttls_resp(ڈیٹا, smtpc, smtpcode, smtpc->state);
      /* During UPGRADETLS, leave the read loop as we need to connect
       * (e.g. TLS handshake) before we continue sending/receiving. */
      اگر(!نتیجہ && (smtpc->state == SMTP_UPGRADETLS))
        جاؤ upgrade_tls;
      توڑو;

    حالت SMTP_AUTH:
      نتیجہ = smtp_state_auth_resp(ڈیٹا, smtpc, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_COMMAND:
      نتیجہ = smtp_state_command_resp(ڈیٹا, smtpc, smtp,
                                       smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_MAIL:
      نتیجہ = smtp_state_mail_resp(ڈیٹا, smtpc, smtp, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_RCPT:
      نتیجہ = smtp_state_rcpt_resp(ڈیٹا, smtpc, smtp, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_ڈیٹا:
      نتیجہ = smtp_state_ڈیٹا_resp(ڈیٹا, smtpc, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_POSTDATA:
      نتیجہ = smtp_state_postdata_resp(ڈیٹا, smtpc, smtpcode, smtpc->state);
      توڑو;

    حالت SMTP_QUIT:
    طےشدہ:
      /* internal error */
      smtp_state(ڈیٹا, smtpc, SMTP_STOP);
      توڑو;
    }
  } جب(!نتیجہ && smtpc->state != SMTP_STOP &&
          Curl_pp_moredata(&smtpc->pp));

  واپس نتیجہ;
}

/* Called repeatedly until done from multi.c */
ساکن CURLcode smtp_multi_statemach(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);

  *مکمل = FALSE;
  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;

  نتیجہ = Curl_pp_statemach(ڈیٹا, &smtpc->pp, FALSE, FALSE);
  *مکمل = (smtpc->state == SMTP_STOP);
  واپس نتیجہ;
}

ساکن CURLcode smtp_بلاک_statemach(ساخت Curl_easy *ڈیٹا,
                                     ساخت smtp_conn *smtpc,
                                     bool disconnecting)
{
  CURLcode نتیجہ = CURLE_OK;

  جب(smtpc->state != SMTP_STOP && !نتیجہ)
    نتیجہ = Curl_pp_statemach(ڈیٹا, &smtpc->pp, TRUE, disconnecting);

  واپس نتیجہ;
}

/* For the SMTP "protocol connect" and "doing" phases only */
ساکن CURLcode smtp_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);
  واپس smtpc ? Curl_pp_pollset(ڈیٹا, &smtpc->pp, ps) : CURLE_OK;
}

/* SASL parameters for the smtp protocol */
ساکن ثابت ساخت SASLproto saslsmtp = {
  "smtp",               /* The service name */
  smtp_perform_auth,    /* Send authentication command */
  smtp_continue_auth,   /* Send authentication continuation */
  smtp_cancel_auth,     /* Cancel authentication */
  smtp_get_پیغام,     /* Get SASL response message */
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
ساکن CURLcode smtp_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);
  CURLcode نتیجہ = CURLE_OK;

  *مکمل = FALSE; /* default to not done yet */
  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;

  PINGPONG_تیاری(&smtpc->pp, smtp_pp_statemachine, smtp_endofresp);

  /* Initialize the SASL storage */
  Curl_sasl_init(&smtpc->sasl, ڈیٹا, &saslsmtp);

  /* Initialize the pingpong layer */
  Curl_pp_init(&smtpc->pp, Curl_pgrs_now(ڈیٹا));

  /* Parse the URL options */
  نتیجہ = smtp_parse_url_options(ڈیٹا->conn, smtpc);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Parse the URL path */
  نتیجہ = smtp_parse_url_راستہ(ڈیٹا, smtpc);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Start off waiting for the server greeting response */
  smtp_state(ڈیٹا, smtpc, SMTP_SERVERGREET);

  نتیجہ = smtp_multi_statemach(ڈیٹا, مکمل);

  واپس نتیجہ;
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
ساکن CURLcode smtp_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                          bool premature)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت SMTP *smtp = Curl_meta_get(ڈیٹا, CURL_META_SMTP_EASY);

  (خالی)premature;

  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;
  اگر(!smtp)
    واپس CURLE_OK;

  /* Cleanup our per-request based variables */
  curlx_safefree(smtp->custom);

  اگر(حالت_2) {
    CURL_TRC_M(ڈیٹا, "SMTP done with bad status");
    connclose(conn); /* marked for closure */
    نتیجہ = حالت_2;         /* use the already set error code */
  }
  ورنہ اگر(!ڈیٹا->سیٹ.connect_only && ڈیٹا->سیٹ.mail_rcpt &&
          (ڈیٹا->state.upload || IS_MIME_POST(ڈیٹا))) {

    smtp_state(ڈیٹا, smtpc, SMTP_POSTDATA);

    /* Run the state-machine */
    نتیجہ = smtp_بلاک_statemach(ڈیٹا, smtpc, FALSE);
  }

  /* Clear the transfer mode for the next request */
  smtp->transfer = PPTRANSFER_BODY;
  CURL_TRC_SMTP(ڈیٹا, "smtp_done(status=%d, premature=%d) -> %d",
                (صحیح)حالت_2, premature, (صحیح)نتیجہ);
  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_perform()
 *
 * This is the actual DO function for SMTP. Transfer a mail, send a command
 * or get some data according to the options previously setup.
 */
ساکن CURLcode smtp_perform(ساخت Curl_easy *ڈیٹا,
                             ساخت smtp_conn *smtpc,
                             ساخت SMTP *smtp,
                             bool *connected,
                             bool *dophase_مکمل)
{
  /* This is SMTP and no proxy */
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_SMTP(ڈیٹا, "smtp_perform(), start");

  اگر(ڈیٹا->req.no_body) {
    /* Requested no body means no transfer */
    smtp->transfer = PPTRANSFER_INFO;
  }

  *dophase_مکمل = FALSE; /* not done yet */

  /* Store the first recipient (or NULL if not specified) */
  smtp->rcpt = ڈیٹا->سیٹ.mail_rcpt;

  /* Track of whether we have successfully sent at least one RCPT TO command */
  smtp->rcpt_had_ok = FALSE;

  /* Track of the last error we have received by sending RCPT TO command */
  smtp->rcpt_آخری_error = 0;

  /* Initial data character is the first character in line: it is implicitly
     preceded by a virtual CRLF. */
  smtp->trailing_crlf = TRUE;
  smtp->eob = 2;

  /* Start the first command in the DO phase */
  اگر((ڈیٹا->state.upload || IS_MIME_POST(ڈیٹا)) && ڈیٹا->سیٹ.mail_rcpt)
    /* MAIL transfer */
    نتیجہ = smtp_perform_mail(ڈیٹا, smtpc, smtp);
  ورنہ
    /* SMTP based command (VRFY, EXPN, NOOP, RSET or HELP) */
    نتیجہ = smtp_perform_command(ڈیٹا, smtpc, smtp);

  اگر(نتیجہ)
    جاؤ برآمد;

  /* Run the state-machine */
  نتیجہ = smtp_multi_statemach(ڈیٹا, dophase_مکمل);

  *connected = Curl_conn_is_connected(ڈیٹا->conn, FIRSTSOCKET);

برآمد:
  CURL_TRC_SMTP(ڈیٹا, "smtp_perform() -> %d, connected=%d, done=%d",
                (صحیح)نتیجہ, *connected, *dophase_مکمل);
  واپس نتیجہ;
}

/* Call this when the DO phase has completed */
ساکن CURLcode smtp_dophase_مکمل(ساخت Curl_easy *ڈیٹا,
                                  ساخت SMTP *smtp,
                                  bool connected)
{
  (خالی)connected;

  اگر(smtp->transfer != PPTRANSFER_BODY)
    /* no data to transfer */
    Curl_xfer_تیاری_nop(ڈیٹا);

  واپس CURLE_OK;
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
ساکن CURLcode smtp_regular_transfer(ساخت Curl_easy *ڈیٹا,
                                      ساخت smtp_conn *smtpc,
                                      ساخت SMTP *smtp,
                                      bool *dophase_مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  bool connected = FALSE;

  /* Make sure size is unknown at this point */
  ڈیٹا->req.سائز = -1;

  /* Set the progress data */
  Curl_pgrsReset(ڈیٹا);

  /* Carry out the perform */
  نتیجہ = smtp_perform(ڈیٹا, smtpc, smtp, &connected, dophase_مکمل);

  /* Perform post DO phase operations if necessary */
  اگر(!نتیجہ && *dophase_مکمل)
    نتیجہ = smtp_dophase_مکمل(ڈیٹا, smtp, connected);

  CURL_TRC_SMTP(ڈیٹا, "smtp_regular_transfer() -> %d, done=%d",
                (صحیح)نتیجہ, *dophase_مکمل);
  واپس نتیجہ;
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
ساکن CURLcode smtp_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت smtp_conn *smtpc =
    Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SMTP_CONN);
  ساخت SMTP *smtp = Curl_meta_get(ڈیٹا, CURL_META_SMTP_EASY);
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  *مکمل = FALSE; /* default to false */
  اگر(!smtpc || !smtp)
    واپس CURLE_FAILED_INIT;

  /* Parse the custom request */
  نتیجہ = smtp_parse_custom_request(ڈیٹا, smtp);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = smtp_regular_transfer(ڈیٹا, smtpc, smtp, مکمل);
  CURL_TRC_SMTP(ڈیٹا, "smtp_do() -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
  واپس نتیجہ;
}

/***********************************************************************
 *
 * smtp_disconnect()
 *
 * Disconnect from an SMTP server. Cleanup protocol-specific per-connection
 * resources. BLOCKING.
 */
ساکن CURLcode smtp_disconnect(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn,
                                bool dead_connection)
{
  ساخت smtp_conn *smtpc = Curl_conn_meta_get(conn, CURL_META_SMTP_CONN);

  اگر(!smtpc)
    واپس CURLE_FAILED_INIT;

  /* We cannot send quit unconditionally. If this connection is stale or
     bad in any way, sending quit and waiting around here will make the
     disconnect wait in vain and cause more problems than we need to. */

  اگر(!dead_connection && conn->بٹ.protoconnstart &&
     !Curl_pp_needs_flush(ڈیٹا, &smtpc->pp) &&
     !smtp_perform_quit(ڈیٹا, smtpc))
    (خالی)smtp_بلاک_statemach(ڈیٹا, smtpc, TRUE); /* ignore on QUIT */

  CURL_TRC_SMTP(ڈیٹا, "smtp_disconnect(), finished");
  واپس CURLE_OK;
}

/* Called from multi.c while DOing */
ساکن CURLcode smtp_doing(ساخت Curl_easy *ڈیٹا, bool *dophase_مکمل)
{
  ساخت SMTP *smtp = Curl_meta_get(ڈیٹا, CURL_META_SMTP_EASY);
  CURLcode نتیجہ;

  اگر(!smtp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = smtp_multi_statemach(ڈیٹا, dophase_مکمل);
  اگر(نتیجہ)
    DEBUGF(infof(ڈیٹا, "DO phase failed"));
  ورنہ اگر(*dophase_مکمل) {
    نتیجہ = smtp_dophase_مکمل(ڈیٹا, smtp, FALSE /* not connected */);

    DEBUGF(infof(ڈیٹا, "DO phase is complete"));
  }

  CURL_TRC_SMTP(ڈیٹا, "smtp_doing() -> %d, done=%d", (صحیح)نتیجہ,
                *dophase_مکمل);
  واپس نتیجہ;
}

ساکن خالی smtp_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت SMTP *smtp = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_free(smtp);
}

ساکن خالی smtp_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت smtp_conn *smtpc = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  Curl_pp_disconnect(&smtpc->pp);
  curlx_safefree(smtpc->domain);
  curlx_free(smtpc);
}

ساکن CURLcode smtp_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت smtp_conn *smtpc;
  ساخت SMTP *smtp;
  CURLcode نتیجہ = CURLE_OK;

  smtpc = curlx_calloc(1, حجم(*smtpc));
  اگر(!smtpc ||
     Curl_conn_meta_سیٹ(conn, CURL_META_SMTP_CONN, smtpc, smtp_conn_dtor)) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  smtp = curlx_calloc(1, حجم(*smtp));
  اگر(!smtp ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_SMTP_EASY, smtp, smtp_easy_dtor))
    نتیجہ = CURLE_برآمد_کا_حافظہ;

برآمد:
  CURL_TRC_SMTP(ڈیٹا, "smtp_setup_connection() -> %d", (صحیح)نتیجہ);
  واپس نتیجہ;
}

/*
 * SMTP protocol handler.
 */
ثابت ساخت Curl_protocol Curl_protocol_smtp = {
  smtp_تیاری_connection,            /* setup_connection */
  smtp_do,                          /* do_it */
  smtp_مکمل,                        /* done */
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

#اگر_ختم /* CURL_DISABLE_SMTP */
