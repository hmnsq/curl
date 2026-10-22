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
#अन्तर्भाव "curl_setup.h"

संरचना Curl_easy;

#अन्तर्भाव "mime.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/basename.h"
#अन्तर्भाव "curlx/strcopy.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/base64.h"

#यदि !defined(CURL_DISABLE_MIME) && (!defined(CURL_DISABLE_HTTP) ||      \
                                    !defined(CURL_DISABLE_SMTP) ||      \
                                    !defined(CURL_DISABLE_IMAP))

#अन्तर्भाव "rand.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curlx/dynbuf.h"

#परिभाषय MAX_MIME_स्तराः 40 /* avoid deep nesting */

#परिभाषय READ_ERROR   ((size_t)-1)
#परिभाषय STOP_FILLING ((size_t)-2)

स्थिर size_t mime_subparts_read(वर्ण *बफर, size_t आकार, size_t nitems,
                                 शून्य *instream, bool *hasread);
स्थिर curl_off_t mime_आकार(curl_mimepart *part);

/* Quoted-printable character class table.
 *
 * We cannot rely on ctype functions since quoted-printable input data
 * is assumed to be ASCII-compatible, even on non-ASCII platforms. */
#परिभाषय QP_OK           1       /* Can be represented by itself. */
#परिभाषय QP_SP           2       /* Space or tab. */
#परिभाषय QP_CR           3       /* Carriage return. */
#परिभाषय QP_LF           4       /* Line-feed. */
स्थिर ध्रुव अचिह्नित वर्ण qp_class[] = {
  0,     0,     0,     0,     0,     0,     0,     0,            /* 00 - 07 */
  0,     QP_SP, QP_LF, 0,     0,     QP_CR, 0,     0,            /* 08 - 0F */
  0,     0,     0,     0,     0,     0,     0,     0,            /* 10 - 17 */
  0,     0,     0,     0,     0,     0,     0,     0,            /* 18 - 1F */
  QP_SP, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 20 - 27 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 28 - 2F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 30 - 37 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, 0    , QP_OK, QP_OK,        /* 38 - 3F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 40 - 47 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 48 - 4F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 50 - 57 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 58 - 5F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 60 - 67 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 68 - 6F */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK,        /* 70 - 77 */
  QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, QP_OK, 0,            /* 78 - 7F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* 80 - 8F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* 90 - 9F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* A0 - AF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* B0 - BF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* C0 - CF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* D0 - DF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* E0 - EF */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0                 /* F0 - FF */
};

/* Binary --> hexadecimal ASCII table. */
स्थिर ध्रुव वर्ण aschex[] =
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x41\x42\x43\x44\x45\x46";

#यदि_अपरिभाषितम् __VMS
#परिभाषय filesize(name, stat_दत्त) stat_दत्त.st_आकार
#परिभाषय fopen_read                curlx_fopen

#अन्यथा

#अन्तर्भाव <fabdef.h>
/*
 * get_vms_file_size does what it takes to get the real size of the file
 *
 * For fixed files, find out the size of the EOF block and adjust.
 *
 * For all others, have to read the entire file in, discarding the contents.
 * Most posted text files will be small, and binary files like zlib archives
 * and CD/DVD images should be either a STREAM_LF format or a fixed format.
 *
 */
curl_off_t Vms_Real_File_आकार(ध्रुव वर्ण *name, ध्रुव curlx_struct_stat *stat_buf)
{
  वर्ण बफर[8192];
  curl_off_t गणनम्;
  पूर्णाङ्क ret_stat;
  FILE *file;

  file = curlx_fopen(name, FOPEN_READTEXT); /* VMS */
  यदि(!file)
    प्रत्यागम 0;

  गणनम् = 0;
  ret_stat = 1;
  यावत्(ret_stat > 0) {
    ret_stat = fread(बफर, 1, परिमाण(बफर), file);
    यदि(ret_stat)
      गणनम् += ret_stat;
  }
  curlx_fclose(file);

  प्रत्यागम गणनम्;
}

/*
 *
 *  VmsSpecialSize checks to see if the stat st_size can be trusted and
 *  if not to call a routine to get the correct size.
 *
 */
स्थिर curl_off_t Vms_Special_आकार(ध्रुव वर्ण *name,
                                 ध्रुव curlx_struct_stat *stat_buf)
{
  विकल्प(stat_buf->st_fab_rfm) {
  पक्ष FAB$C_VAR:
  पक्ष FAB$C_VFC:
    प्रत्यागम Vms_Real_File_आकार(name, stat_buf);
    भङ्ग;
  स्वभाव:
    प्रत्यागम stat_buf->st_आकार;
  }
}

#परिभाषय filesize(name, stat_दत्त) Vms_Special_आकार(name, &stat_दत्त)

/*
 * vmsfopenread
 *
 * For upload to work as expected on VMS, different optional
 * parameters must be added to the fopen command based on
 * record format of the file.
 *
 */
स्थिर FILE *vmsfopenread(ध्रुव वर्ण *file, ध्रुव वर्ण *बहुलकम्)
{
  curlx_struct_stat statbuf;
  पूर्णाङ्क res = curlx_stat(file, &statbuf);

  यदि(res != -1) {
    विकल्प(statbuf.st_fab_rfm) {
    पक्ष FAB$C_VAR:
    पक्ष FAB$C_VFC:
    पक्ष FAB$C_STMCR:
      प्रत्यागम curlx_fopen(file, FOPEN_READTEXT); /* VMS */
      भङ्ग;
    स्वभाव:
      प्रत्यागम curlx_fopen(file, FOPEN_READTEXT, "rfm=stmlf", "ctx=stm");
    }
  }
  प्रत्यागम रिक्त;
}

#परिभाषय fopen_read vmsfopenread
#यदि_अन्तः /* !__VMS */

/* Set readback state. */
स्थिर शून्य mimesetstate(संरचना mime_state *state,
                         गणना mimestate tok, शून्य *ptr)
{
  state->state = tok;
  state->ptr = ptr;
  state->विस्थापनम् = 0;
}

/* Escape header string into allocated memory. */
स्थिर वर्ण *escape_string(संरचना Curl_easy *दत्त,
                           ध्रुव वर्ण *src, गणना mimestrategy strategy)
{
  CURLcode परिणाम;
  संरचना dynbuf db;
  ध्रुव वर्ण * ध्रुव *सारणी;
  ध्रुव वर्ण * ध्रुव *p;
  /* replace first character by rest of string. */
  स्थिर ध्रुव वर्ण * ध्रुव mimetable[] = {
    "\\\\\\",
    "\"\\\"",
    रिक्त
  };
  /* WHATWG HTML living standard 4.10.21.8 2 specifies:
     For field names and filenames for file fields, the result of the
     encoding in the previous bullet point must be escaped by replacing
     any 0x0A (LF) bytes with the byte sequence `%0A`, 0x0D (CR) with `%0D`
     and 0x22 (") with `%22`.
     The user agent must not perform any other escapes. */
  स्थिर ध्रुव वर्ण * ध्रुव formtable[] = {
    "\"%22",
    "\r%0D",
    "\n%0A",
    रिक्त
  };

  सारणी = formtable;
  /* data can be NULL when this function is called indirectly from
     curl_formget(). */
  यदि(strategy == MIMESTRATEGY_MAIL || (दत्त && दत्त->समुच्चयः.mime_formescape))
    सारणी = mimetable;

  curlx_dyn_init(&db, CURL_MAX_निवेशः_दीर्घता);

  प्रति(परिणाम = curlx_dyn_addn(&db, STRCONST("")); !परिणाम && *src; src++) {
    प्रति(p = सारणी; *p && **p != *src; p++)
      ;

    यदि(*p)
      परिणाम = curlx_dyn_सङ्कलय(&db, *p + 1);
    अन्यथा
      परिणाम = curlx_dyn_addn(&db, src, 1);
  }

  प्रत्यागम curlx_dyn_ptr(&db);
}

/* Check if header matches. */
स्थिर वर्ण *मेलनम्_शीर्षकम्(संरचना curl_slist *hdr, ध्रुव वर्ण *lbl, size_t len)
{
  वर्ण *मानम् = रिक्त;

  यदि(curl_strnequal(hdr->दत्त, lbl, len) && hdr->दत्त[len] == ':')
    प्रति(मानम् = hdr->दत्त + len + 1; *मानम् == ' '; मानम्++)
      ;
  प्रत्यागम मानम्;
}

/* Get a header from an slist. */
स्थिर वर्ण *अन्वेषणम्_शीर्षकम्(संरचना curl_slist *hdrlist,
                           ध्रुव वर्ण *hdr, size_t len)
{
  वर्ण *मानम् = रिक्त;

  प्रति(; !मानम् && hdrlist; hdrlist = hdrlist->अग्र)
    मानम् = मेलनम्_शीर्षकम्(hdrlist, hdr, len);

  प्रत्यागम मानम्;
}

स्थिर वर्ण *strippath(ध्रुव वर्ण *fullfile)
{
  वर्ण *filename;
  वर्ण *आधारः;
  filename = curlx_strdup(fullfile); /* duplicate since basename() may ruin
                                        the buffer it works on */
  यदि(!filename)
    प्रत्यागम रिक्त;
  आधारः = curlx_strdup(curlx_basename(filename));

  curlx_free(filename); /* free temporary buffer */

  प्रत्यागम आधारः; /* returns an allocated string or NULL ! */
}

/* Initialize data encoder state. */
स्थिर शून्य cleanup_encoder_state(संरचना mime_encoder_state *p)
{
  p->pos = 0;
  p->bufbeg = 0;
  p->bufend = 0;
}

/* Dummy encoder. This is used for 8-bit and binary content encodings. */
स्थिर size_t encoder_nop_read(वर्ण *बफर, size_t आकार, bool ateof,
                               संरचना curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t insize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  यदि(!आकार)
    प्रत्यागम STOP_FILLING;

  यदि(आकार > insize)
    आकार = insize;

  यदि(आकार)
    स्मृतिं_प्रतिलिख(बफर, st->buf + st->bufbeg, आकार);

  st->bufbeg += आकार;
  प्रत्यागम आकार;
}

स्थिर curl_off_t encoder_nop_आकार(curl_mimepart *part)
{
  प्रत्यागम part->datasize;
}

/* 7-bit encoder: the encoder is a data validity check. */
स्थिर size_t encoder_7bit_read(वर्ण *बफर, size_t आकार, bool ateof,
                                curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t cursize = st->bufend - st->bufbeg;

  (शून्य)ateof;

  यदि(!आकार)
    प्रत्यागम STOP_FILLING;

  यदि(आकार > cursize)
    आकार = cursize;

  प्रति(cursize = 0; cursize < आकार; cursize++) {
    *बफर = st->buf[st->bufbeg];
    यदि(*बफर++ & 0x80)
      प्रत्यागम cursize ? cursize : READ_ERROR;
    st->bufbeg++;
  }

  प्रत्यागम cursize;
}

/* Base64 content encoder. */
स्थिर size_t encoder_base64_read(वर्ण *बफर, size_t आकार, bool ateof,
                                  curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  पूर्णाङ्क i;
  वर्ण *ptr = बफर;

  यावत्(st->bufbeg < st->bufend) {
    /* Line full ? */
    यदि(st->pos > MAX_ENCODED_पङ्क्तिः_दीर्घता - 4) {
      /* Yes, we need 2 characters for CRLF. */
      यदि(आकार < 2) {
        यदि(!cursize)
          प्रत्यागम STOP_FILLING;
        भङ्ग;
      }
      *ptr++ = '\r';
      *ptr++ = '\n';
      st->pos = 0;
      cursize += 2;
      आकार -= 2;
    }

    /* Be sure there is enough space and input data for a base64 group. */
    यदि(आकार < 4) {
      यदि(!cursize)
        प्रत्यागम STOP_FILLING;
      भङ्ग;
    }
    यदि(st->bufend - st->bufbeg < 3)
      भङ्ग;

    /* Encode three bytes as four characters. */
    i = st->buf[st->bufbeg++] & 0xFF;
    i = (i << 8) | (st->buf[st->bufbeg++] & 0xFF);
    i = (i << 8) | (st->buf[st->bufbeg++] & 0xFF);
    *ptr++ = curlx_base64encdec[(i >> 18) & 0x3F];
    *ptr++ = curlx_base64encdec[(i >> 12) & 0x3F];
    *ptr++ = curlx_base64encdec[(i >> 6) & 0x3F];
    *ptr++ = curlx_base64encdec[i & 0x3F];
    cursize += 4;
    st->pos += 4;
    आकार -= 4;
  }

  /* If at eof, we have to flush the buffered data. */
  यदि(ateof) {
    यदि(आकार < 4) {
      यदि(!cursize)
        प्रत्यागम STOP_FILLING;
    }
    अन्यथा {
      /* Buffered data size can only be 0, 1 or 2. */
      ptr[2] = ptr[3] = '=';
      i = 0;

      /* If there is buffered data */
      यदि(st->bufend != st->bufbeg) {

        यदि(st->bufend - st->bufbeg == 2)
          i = (st->buf[st->bufbeg + 1] & 0xFF) << 8;

        i |= (st->buf[st->bufbeg] & 0xFF) << 16;
        ptr[0] = curlx_base64encdec[(i >> 18) & 0x3F];
        ptr[1] = curlx_base64encdec[(i >> 12) & 0x3F];
        यदि(++st->bufbeg != st->bufend) {
          ptr[2] = curlx_base64encdec[(i >> 6) & 0x3F];
          st->bufbeg++;
        }
        cursize += 4;
        st->pos += 4;
      }
    }
  }

  प्रत्यागम cursize;
}

/* The maximum input size that does not cause an overflow. */
#परिभाषय BASE64_MAX_निवेशः_आकार                                           \
  (((CURL_OFF_T_MAX / (MAX_ENCODED_पङ्क्तिः_दीर्घता + 2)) *                  \
    MAX_ENCODED_पङ्क्तिः_दीर्घता / 4) * 3 - 3)

स्थिर curl_off_t encoder_base64_आकार(curl_mimepart *part)
{
  curl_off_t आकार = part->datasize;

  यदि(आकार <= 0)
    प्रत्यागम आकार;    /* Unknown size or no data. */

  /* Prevent integer overflows */
  यदि(आकार > BASE64_MAX_निवेशः_आकार)
    प्रत्यागम -1;

  /* Compute base64 character count. */
  आकार = 4 * (1 + ((आकार - 1) / 3));

  /* Effective character count must include CRLFs. */
  प्रत्यागम आकार + (2 * ((आकार - 1) / MAX_ENCODED_पङ्क्तिः_दीर्घता));
}

/* Quoted-printable lookahead.
 *
 * Check if a CRLF or end of data is in input buffer at current position + n.
 * Return -1 if more data needed, 1 if CRLF or end of data, else 0.
 */
स्थिर पूर्णाङ्क qp_lookahead_eol(संरचना mime_encoder_state *st, पूर्णाङ्क ateof, size_t n)
{
  n += st->bufbeg;
  यदि(n >= st->bufend && ateof)
    प्रत्यागम 1;
  यदि(n + 2 > st->bufend)
    प्रत्यागम ateof ? 0 : -1;
  यदि(qp_class[st->buf[n] & 0xFF] == QP_CR &&
     qp_class[st->buf[n + 1] & 0xFF] == QP_LF)
    प्रत्यागम 1;
  प्रत्यागम 0;
}

/* Quoted-printable encoder. */
स्थिर size_t encoder_qp_read(वर्ण *बफर, size_t आकार, bool ateof,
                              curl_mimepart *part)
{
  संरचना mime_encoder_state *st = &part->encstate;
  वर्ण *ptr = बफर;
  size_t cursize = 0;
  पूर्णाङ्क softlinebreak;
  वर्ण buf[4];

  /* On all platforms, input is supposed to be ASCII compatible: for this
     reason, we use hexadecimal ASCII codes in this function rather than
     character constants that can be interpreted as non-ASCII on some
     platforms. Preserve ASCII encoding on output too. */
  यावत्(st->bufbeg < st->bufend) {
    size_t len = 1;
    size_t consumed = 1;
    पूर्णाङ्क i = (अचिह्नित वर्ण)st->buf[st->bufbeg];
    buf[0] = (वर्ण)i;
    buf[1] = aschex[(i >> 4) & 0xF];
    buf[2] = aschex[i & 0xF];

    विकल्प(qp_class[st->buf[st->bufbeg] & 0xFF]) {
    पक्ष QP_OK:          /* Not a special character. */
      भङ्ग;
    पक्ष QP_SP:          /* Space or tab. */
      /* Spacing must be escaped if followed by CRLF. */
      विकल्प(qp_lookahead_eol(st, ateof, 1)) {
      पक्ष -1:          /* More input data needed. */
        प्रत्यागम cursize;
      पक्ष 0:           /* No encoding needed. */
        भङ्ग;
      स्वभाव:          /* CRLF after space or tab. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        भङ्ग;
      }
      भङ्ग;
    पक्ष QP_CR:         /* Carriage return. */
      /* If followed by a line-feed, output the CRLF pair.
         Else escape it. */
      विकल्प(qp_lookahead_eol(st, ateof, 0)) {
      पक्ष -1:          /* Need more data. */
        प्रत्यागम cursize;
      पक्ष 1:           /* CRLF found. */
        buf[len++] = '\x0A';    /* Append '\n'. */
        consumed = 2;
        भङ्ग;
      स्वभाव:          /* Not followed by LF: escape. */
        buf[0] = '\x3D';    /* '=' */
        len = 3;
        भङ्ग;
      }
      भङ्ग;
    स्वभाव:            /* Character must be escaped. */
      buf[0] = '\x3D';    /* '=' */
      len = 3;
      भङ्ग;
    }

    /* Be sure the encoded character fits within maximum line length. */
    यदि(buf[len - 1] != '\x0A') {    /* '\n' */
      softlinebreak = st->pos + len > MAX_ENCODED_पङ्क्तिः_दीर्घता;
      यदि(!softlinebreak && st->pos + len == MAX_ENCODED_पङ्क्तिः_दीर्घता) {
        /* We may use the current line only if end of data or followed by
           a CRLF. */
        विकल्प(qp_lookahead_eol(st, ateof, consumed)) {
        पक्ष -1:        /* Need more data. */
          प्रत्यागम cursize;
        पक्ष 0:         /* Not followed by a CRLF. */
          softlinebreak = 1;
          भङ्ग;
        }
      }
      यदि(softlinebreak) {
        curlx_strcopy(buf, परिमाण(buf), STRCONST("\x3D\x0D\x0A")); /* =\r\n */
        len = 3;
        consumed = 0;
      }
    }

    /* If the output buffer would overflow, do not store. */
    यदि(len > आकार) {
      यदि(!cursize)
        प्रत्यागम STOP_FILLING;
      भङ्ग;
    }

    /* Append to output buffer. */
    स्मृतिं_प्रतिलिख(ptr, buf, len);
    cursize += len;
    ptr += len;
    आकार -= len;
    st->pos += len;
    यदि(buf[len - 1] == '\x0A')    /* '\n' */
      st->pos = 0;
    st->bufbeg += consumed;
  }

  प्रत्यागम cursize;
}

स्थिर curl_off_t encoder_qp_आकार(curl_mimepart *part)
{
  /* Determining the size can only be done by reading the data: unless the
     data size is 0, we return it as unknown (-1). */
  प्रत्यागम part->datasize ? -1 : 0;
}

/* In-memory data callbacks. */
/* Argument is a pointer to the mime part. */
स्थिर size_t mime_mem_read(वर्ण *बफर, size_t आकार, size_t nitems,
                            शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t sz = curlx_sotouz(part->datasize - part->state.विस्थापनम्);
  (शून्य)आकार;  /* Always 1 */

  यदि(!nitems)
    प्रत्यागम STOP_FILLING;

  यदि(sz > nitems)
    sz = nitems;

  यदि(sz)
    स्मृतिं_प्रतिलिख(बफर, part->दत्त + curlx_sotouz(part->state.विस्थापनम्), sz);

  प्रत्यागम sz;
}

स्थिर पूर्णाङ्क mime_mem_seek(शून्य *instream, curl_off_t विस्थापनम्, पूर्णाङ्क whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  विकल्प(whence) {
  पक्ष SEEK_CUR:
    विस्थापनम् += part->state.विस्थापनम्;
    भङ्ग;
  पक्ष SEEK_END:
    विस्थापनम् += part->datasize;
    भङ्ग;
  }

  यदि(विस्थापनम् < 0 || विस्थापनम् > part->datasize)
    प्रत्यागम CURL_SEEKFUNC_FAIL;

  part->state.विस्थापनम् = विस्थापनम्;
  प्रत्यागम CURL_SEEKFUNC_OK;
}

स्थिर शून्य mime_mem_free(शून्य *ptr)
{
  curlx_safefree(((curl_mimepart *)ptr)->दत्त);
}

/* Named file callbacks. */
/* Argument is a pointer to the mime part. */
स्थिर bool mime_open_file(curl_mimepart *part)
{
  /* Open a MIMEKIND_FILE part. */

  यदि(part->fp)
    प्रत्यागम FALSE;
  part->fp = fopen_read(part->दत्त, "rb");
  प्रत्यागम part->fp ? FALSE : TRUE;
}

स्थिर size_t mime_file_read(वर्ण *बफर, size_t आकार, size_t nitems,
                             शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  यदि(!nitems)
    प्रत्यागम STOP_FILLING;

  यदि(mime_open_file(part))
    प्रत्यागम READ_ERROR;

  प्रत्यागम fread(बफर, आकार, nitems, part->fp);
}

स्थिर पूर्णाङ्क mime_file_seek(शून्य *instream, curl_off_t विस्थापनम्, पूर्णाङ्क whence)
{
  curl_mimepart *part = (curl_mimepart *)instream;

  यदि(whence == SEEK_SET && !विस्थापनम् && !part->fp)
    प्रत्यागम CURL_SEEKFUNC_OK;   /* Not open: implicitly already at BOF. */

  यदि(mime_open_file(part))
    प्रत्यागम CURL_SEEKFUNC_FAIL;

  प्रत्यागम curlx_fseek(part->fp, विस्थापनम्, whence) ?
    CURL_SEEKFUNC_CANTSEEK : CURL_SEEKFUNC_OK;
}

स्थिर शून्य mime_file_free(शून्य *ptr)
{
  curl_mimepart *part = (curl_mimepart *)ptr;

  यदि(part->fp) {
    curlx_fclose(part->fp);
    part->fp = रिक्त;
  }
  curlx_safefree(part->दत्त);
}

/* Subparts callbacks. */
/* Argument is a pointer to the mime structure. */

/* Readback a byte string segment. */
स्थिर size_t readback_अष्टकानि(संरचना mime_state *state,
                             वर्ण *बफर, size_t bufsize,
                             ध्रुव वर्ण *अष्टकानि, size_t numbytes,
                             ध्रुव वर्ण *trail, size_t traillen)
{
  size_t sz;
  size_t विस्थापनम् = curlx_sotouz(state->विस्थापनम्);

  यदि(numbytes > विस्थापनम्) {
    sz = numbytes - विस्थापनम्;
    अष्टकानि += विस्थापनम्;
  }
  अन्यथा {
    sz = विस्थापनम् - numbytes;
    यदि(sz >= traillen)
      प्रत्यागम 0;
    अष्टकानि = trail + sz;
    sz = traillen - sz;
  }

  यदि(sz > bufsize)
    sz = bufsize;

  स्मृतिं_प्रतिलिख(बफर, अष्टकानि, sz);
  state->विस्थापनम् += sz;
  प्रत्यागम sz;
}

/* Read a non-encoded part content. */

स्थिर size_t read_part_content(curl_mimepart *part, वर्ण *बफर,
                                size_t bufsize, bool *hasread)
{
  size_t sz = 0;

  विकल्प(part->lastreadstatus) {
  पक्ष 0:
  पक्ष CURL_READFUNC_ABORT:
  पक्ष CURL_READFUNC_PAUSE:
  पक्ष READ_ERROR:
    प्रत्यागम part->lastreadstatus;
  स्वभाव:
    भङ्ग;
  }

  /* If we can determine we are at end of part data, spare a read. */
  यदि(part->datasize != (curl_off_t)-1 &&
     part->state.विस्थापनम् >= part->datasize) {
    /* sz is already zero. */
  }
  अन्यथा {
    विकल्प(part->विधा) {
    पक्ष MIMEKIND_MULTIPART:
      /*
       * Cannot be processed as other kinds since read function requires
       * an additional parameter and is highly recursive.
       */
      sz = mime_subparts_read(बफर, 1, bufsize, part->arg, hasread);
      भङ्ग;
    पक्ष MIMEKIND_FILE:
      यदि(part->fp && feof(part->fp))
        भङ्ग;  /* At EOF. */
      FALLTHROUGH();
    स्वभाव:
      यदि(part->readfunc) {
        यदि(!(part->flags & MIME_FAST_READ)) {
          यदि(*hasread)
            प्रत्यागम STOP_FILLING;
          *hasread = TRUE;
        }
        sz = part->readfunc(बफर, 1, bufsize, part->arg);
      }
      भङ्ग;
    }
  }

  विकल्प(sz) {
  पक्ष STOP_FILLING:
    भङ्ग;
  पक्ष 0:
  पक्ष CURL_READFUNC_ABORT:
  पक्ष CURL_READFUNC_PAUSE:
  पक्ष READ_ERROR:
    part->lastreadstatus = sz;
    भङ्ग;
  स्वभाव:
    part->state.विस्थापनम् += sz;
    part->lastreadstatus = sz;
    भङ्ग;
  }

  प्रत्यागम sz;
}

/* Read and encode part content. */
स्थिर size_t read_encoded_part_content(curl_mimepart *part, वर्ण *बफर,
                                        size_t bufsize, bool *hasread)
{
  संरचना mime_encoder_state *st = &part->encstate;
  size_t cursize = 0;
  size_t sz;
  bool ateof = FALSE;

  प्रति(;;) {
    यदि(st->bufbeg < st->bufend || ateof) {
      /* Encode buffered data. */
      sz = part->encoder->encodefunc(बफर, bufsize, ateof, part);
      विकल्प(sz) {
      पक्ष 0:
        यदि(ateof)
          प्रत्यागम cursize;
        भङ्ग;
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        प्रत्यागम cursize ? cursize : sz;
      स्वभाव:
        cursize += sz;
        बफर += sz;
        bufsize -= sz;
        अनुवर्त;
      }
    }

    /* We need more data in input buffer. */
    यदि(st->bufbeg) {
      size_t len = st->bufend - st->bufbeg;

      यदि(len)
        memmove(st->buf, st->buf + st->bufbeg, len);
      st->bufbeg = 0;
      st->bufend = len;
    }
    यदि(st->bufend >= परिमाण(st->buf))
      प्रत्यागम cursize ? cursize : READ_ERROR;    /* Buffer full. */
    sz = read_part_content(part, st->buf + st->bufend,
                           परिमाण(st->buf) - st->bufend, hasread);
    विकल्प(sz) {
    पक्ष 0:
      ateof = TRUE;
      भङ्ग;
    पक्ष CURL_READFUNC_ABORT:
    पक्ष CURL_READFUNC_PAUSE:
    पक्ष READ_ERROR:
    पक्ष STOP_FILLING:
      प्रत्यागम cursize ? cursize : sz;
    स्वभाव:
      st->bufend += sz;
      भङ्ग;
    }
  }

  /* NOTREACHED */
}

/* Readback a mime part. */
स्थिर size_t readback_part(curl_mimepart *part,
                            वर्ण *बफर, size_t bufsize, bool *hasread)
{
  size_t cursize = 0;

  /* Readback from part. */
  यावत्(bufsize) {
    size_t sz = 0;
    संरचना curl_slist *hdr = (संरचना curl_slist *)part->state.ptr;
    विकल्प(part->state.state) {
    पक्ष MIMESTATE_BEGIN:
      mimesetstate(&part->state,
                   (part->flags & MIME_BODY_ONLY) ?
                   MIMESTATE_BODY : MIMESTATE_CURLHEADERS,
                   part->curlheaders);
      भङ्ग;
    पक्ष MIMESTATE_USERHEADERS:
      यदि(!hdr) {
        mimesetstate(&part->state, MIMESTATE_EOH, रिक्त);
        भङ्ग;
      }
      यदि(मेलनम्_शीर्षकम्(hdr, "Content-Type", 12)) {
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, hdr->अग्र);
        भङ्ग;
      }
      FALLTHROUGH();
    पक्ष MIMESTATE_CURLHEADERS:
      यदि(!hdr)
        mimesetstate(&part->state, MIMESTATE_USERHEADERS, part->userheaders);
      अन्यथा {
        sz = readback_अष्टकानि(&part->state, बफर, bufsize,
                            hdr->दत्त, वाक्यदैर्घ्यम्(hdr->दत्त), STRCONST("\r\n"));
        यदि(!sz)
          mimesetstate(&part->state, part->state.state, hdr->अग्र);
      }
      भङ्ग;
    पक्ष MIMESTATE_EOH:
      sz = readback_अष्टकानि(&part->state, बफर, bufsize, STRCONST("\r\n"),
                          STRCONST(""));
      यदि(!sz)
        mimesetstate(&part->state, MIMESTATE_BODY, रिक्त);
      भङ्ग;
    पक्ष MIMESTATE_BODY:
      cleanup_encoder_state(&part->encstate);
      mimesetstate(&part->state, MIMESTATE_CONTENT, रिक्त);
      भङ्ग;
    पक्ष MIMESTATE_CONTENT:
      यदि(part->encoder)
        sz = read_encoded_part_content(part, बफर, bufsize, hasread);
      अन्यथा
        sz = read_part_content(part, बफर, bufsize, hasread);
      विकल्प(sz) {
      पक्ष 0:
        mimesetstate(&part->state, MIMESTATE_अन्तः, रिक्त);
        /* Try sparing open file descriptors. */
        यदि(part->विधा == MIMEKIND_FILE && part->fp) {
          curlx_fclose(part->fp);
          part->fp = रिक्त;
        }
        FALLTHROUGH();
      पक्ष CURL_READFUNC_ABORT:
      पक्ष CURL_READFUNC_PAUSE:
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        प्रत्यागम cursize ? cursize : sz;
      }
      भङ्ग;
    पक्ष MIMESTATE_अन्तः:
      प्रत्यागम cursize;
    स्वभाव:
      भङ्ग;    /* Other values not in part state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    बफर += sz;
    bufsize -= sz;
  }

  प्रत्यागम cursize;
}

/* Readback from mime. Warning: not a read callback function. */
स्थिर size_t mime_subparts_read(वर्ण *बफर, size_t आकार, size_t nitems,
                                 शून्य *instream, bool *hasread)
{
  curl_mime *mime = (curl_mime *)instream;
  size_t cursize = 0;
  (शून्य)आकार;  /* Always 1 */

  यावत्(nitems) {
    size_t sz = 0;
    curl_mimepart *part = mime->state.ptr;
    विकल्प(mime->state.state) {
    पक्ष MIMESTATE_BEGIN:
    पक्ष MIMESTATE_BODY:
      mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, mime->firstpart);
      /* The first boundary always follows the header termination empty line,
         so is always preceded by a CRLF. We can then spare 2 characters
         by skipping the leading CRLF in boundary. */
      mime->state.विस्थापनम् += 2;
      भङ्ग;
    पक्ष MIMESTATE_BOUNDARY1:
      sz = readback_अष्टकानि(&mime->state, बफर, nitems, STRCONST("\r\n--"),
                          STRCONST(""));
      यदि(!sz)
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY2, part);
      भङ्ग;
    पक्ष MIMESTATE_BOUNDARY2:
      यदि(part)
        sz = readback_अष्टकानि(&mime->state, बफर, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("\r\n"));
      अन्यथा
        sz = readback_अष्टकानि(&mime->state, बफर, nitems, mime->boundary,
                            MIME_BOUNDARY_LEN, STRCONST("--\r\n"));
      यदि(!sz) {
        mimesetstate(&mime->state, MIMESTATE_CONTENT, part);
      }
      भङ्ग;
    पक्ष MIMESTATE_CONTENT:
      यदि(!part) {
        mimesetstate(&mime->state, MIMESTATE_अन्तः, रिक्त);
        भङ्ग;
      }
      sz = readback_part(part, बफर, nitems, hasread);
      विकल्प(sz) {
      पक्ष CURL_READFUNC_ABORT:
      पक्ष CURL_READFUNC_PAUSE:
      पक्ष READ_ERROR:
      पक्ष STOP_FILLING:
        प्रत्यागम cursize ? cursize : sz;
      पक्ष 0:
        mimesetstate(&mime->state, MIMESTATE_BOUNDARY1, part->nextpart);
        भङ्ग;
      }
      भङ्ग;
    पक्ष MIMESTATE_अन्तः:
      प्रत्यागम cursize;
    स्वभाव:
      भङ्ग;    /* other values not used in mime state. */
    }

    /* Bump buffer and counters according to read size. */
    cursize += sz;
    बफर += sz;
    nitems -= sz;
  }

  प्रत्यागम cursize;
}

स्थिर पूर्णाङ्क mime_part_rewind(curl_mimepart *part)
{
  पूर्णाङ्क res = CURL_SEEKFUNC_OK;
  गणना mimestate targetstate = MIMESTATE_BEGIN;

  यदि(part->flags & MIME_BODY_ONLY)
    targetstate = MIMESTATE_BODY;
  cleanup_encoder_state(&part->encstate);
  यदि(part->state.state > targetstate) {
    res = CURL_SEEKFUNC_CANTSEEK;
    यदि(part->seekfunc) {
      res = part->seekfunc(part->arg, (curl_off_t)0, SEEK_SET);
      विकल्प(res) {
      पक्ष CURL_SEEKFUNC_OK:
      पक्ष CURL_SEEKFUNC_FAIL:
      पक्ष CURL_SEEKFUNC_CANTSEEK:
        भङ्ग;
      पक्ष -1:    /* For fseek() error. */
        res = CURL_SEEKFUNC_CANTSEEK;
        भङ्ग;
      स्वभाव:
        res = CURL_SEEKFUNC_FAIL;
        भङ्ग;
      }
    }
  }

  यदि(res == CURL_SEEKFUNC_OK)
    mimesetstate(&part->state, targetstate, रिक्त);

  part->lastreadstatus = 1; /* Successful read status. */
  प्रत्यागम res;
}

स्थिर पूर्णाङ्क mime_subparts_seek(शून्य *instream, curl_off_t विस्थापनम्, पूर्णाङ्क whence)
{
  curl_mime *mime = (curl_mime *)instream;
  curl_mimepart *part;
  पूर्णाङ्क rc = CURL_SEEKFUNC_OK;

  यदि(whence != SEEK_SET || विस्थापनम्)
    प्रत्यागम CURL_SEEKFUNC_CANTSEEK;    /* Only support full rewind. */

  यदि(mime->state.state == MIMESTATE_BEGIN)
    प्रत्यागम CURL_SEEKFUNC_OK;           /* Already rewound. */

  प्रति(part = mime->firstpart; part; part = part->nextpart) {
    पूर्णाङ्क res = mime_part_rewind(part);
    यदि(res != CURL_SEEKFUNC_OK)
      rc = res;
  }

  यदि(rc == CURL_SEEKFUNC_OK)
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);

  प्रत्यागम rc;
}

/* Release part content. */
स्थिर शून्य cleanup_part_content(curl_mimepart *part)
{
  यदि(part->freefunc)
    part->freefunc(part->arg);

  part->readfunc = रिक्त;
  part->seekfunc = रिक्त;
  part->freefunc = रिक्त;
  part->arg = (शून्य *)part;           /* Defaults to part itself. */
  part->दत्त = रिक्त;
  part->fp = रिक्त;
  part->datasize = (curl_off_t)0;     /* No size yet. */
  cleanup_encoder_state(&part->encstate);
  part->विधा = MIMEKIND_NONE;
  part->flags &= ~(अचिह्नित पूर्णाङ्क)MIME_FAST_READ;
  part->lastreadstatus = 1; /* Successful read status. */
  part->state.state = MIMESTATE_BEGIN;
}

स्थिर शून्य mime_subparts_free(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  यदि(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
  }
  curl_mime_free(mime);
}

/* Do not free subparts: unbind them. This is used for the top level only. */
स्थिर शून्य mime_subparts_unbind(शून्य *ptr)
{
  curl_mime *mime = (curl_mime *)ptr;

  यदि(mime && mime->parent) {
    mime->parent->freefunc = रिक्त;  /* Be sure we will not be called again. */
    cleanup_part_content(mime->parent);  /* Avoid dangling pointer in part. */
    mime->parent = रिक्त;
  }
}

शून्य Curl_mime_cleanpart(curl_mimepart *part)
{
  यदि(part) {
    cleanup_part_content(part);
    curl_slist_free_all(part->curlheaders);
    यदि(part->flags & MIME_USERHEADERS_OWNER)
      curl_slist_free_all(part->userheaders);
    curlx_safefree(part->mimetype);
    curlx_safefree(part->name);
    curlx_safefree(part->filename);
    Curl_mime_initpart(part);
  }
}

/* Non-recursively delete a mime handle and its parts. */
शून्य curl_mime_free(curl_mime *mime)
{
  curl_mimepart *part;

  यदि(!mime)
    प्रत्यागम;

  mime_subparts_unbind(mime);  /* Be sure it is not referenced anymore. */

  यावत्(mime) {
    part = mime->firstpart;
    यदि(part) {
      mime->firstpart = part->nextpart;
      यदि(part->विधा == MIMEKIND_MULTIPART && part->arg &&
         part->freefunc == mime_subparts_free) {
        curl_mime *subparts = (curl_mime *)part->arg;
        part->freefunc = रिक्त;
        cleanup_part_content(part);
        subparts->parent = mime->parent;
        mime->parent = (curl_mimepart *)subparts;
      }
      Curl_mime_cleanpart(part);
      curlx_free(part);
    }
    अन्यथा {
      curl_mime *parent = (curl_mime *)mime->parent;
      curlx_free(mime);
      mime = parent;
    }
  }
}

स्थिर bool mime_is_too_deep(ध्रुव curl_mimepart *part, size_t call_गभीरता)
{
  DEBUGASSERT(part);

  यदि(++call_गभीरता > MAX_MIME_स्तराः)
    प्रत्यागम TRUE;

  यदि(part->विधा == MIMEKIND_MULTIPART && part->arg) {
    ध्रुव curl_mime *mime = (ध्रुव curl_mime *)part->arg;
    ध्रुव curl_mimepart *s;

    प्रति(s = mime->firstpart; s; s = s->nextpart) {
      यदि(mime_is_too_deep(s, call_गभीरता))
        प्रत्यागम TRUE;
    }
  }

  प्रत्यागम FALSE;
}

स्थिर CURLcode mime_duppart(संरचना Curl_easy *दत्त,
                             curl_mimepart *dst, ध्रुव curl_mimepart *src,
                             size_t call_गभीरता)
{
  curl_mime *mime;
  curl_mimepart *d;
  ध्रुव curl_mimepart *s;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(dst);
  DEBUGASSERT(src);

  यदि(++call_गभीरता > MAX_MIME_स्तराः)
    प्रत्यागम CURLE_TOO_LARGE;

  /* Duplicate content. */
  विकल्प(src->विधा) {
  पक्ष MIMEKIND_NONE:
    भङ्ग;
  पक्ष MIMEKIND_दत्त:
    परिणाम = curl_mime_दत्त(dst, src->दत्त, (size_t)src->datasize);
    भङ्ग;
  पक्ष MIMEKIND_FILE:
    परिणाम = curl_mime_filedata(dst, src->दत्त);
    /* Do not abort duplication if file is not readable. */
    यदि(परिणाम == CURLE_READ_ERROR)
      परिणाम = CURLE_OK;
    भङ्ग;
  पक्ष MIMEKIND_CALLBACK:
    परिणाम = curl_mime_दत्त_cb(dst, src->datasize, src->readfunc,
                               src->seekfunc, src->freefunc, src->arg);
    भङ्ग;
  पक्ष MIMEKIND_MULTIPART:
    /* No one knows about the cloned subparts, thus always attach ownership
       to the part. */
    mime = curl_mime_init(दत्त);
    परिणाम = mime ? curl_mime_subparts(dst, mime) : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    /* Duplicate subparts. */
    प्रति(s = ((curl_mime *)src->arg)->firstpart; !परिणाम && s;
        s = s->nextpart) {
      d = curl_mime_addpart(mime);
      परिणाम = d ? mime_duppart(दत्त, d, s, call_गभीरता) : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    भङ्ग;
  स्वभाव:  /* Invalid kind: should not occur. */
    DEBUGF(infof(दत्त, "invalid MIMEKIND* attempt"));
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;  /* Internal error? */
    भङ्ग;
  }

  /* Duplicate headers. */
  यदि(!परिणाम && src->userheaders) {
    संरचना curl_slist *hdrs = Curl_slist_duplicate(src->userheaders);

    यदि(!hdrs)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    अन्यथा {
      /* No one but this procedure knows about the new header list,
         so always take ownership. */
      परिणाम = curl_mime_शीर्षकाणि(dst, hdrs, TRUE);
      यदि(परिणाम)
        curl_slist_free_all(hdrs);
    }
  }

  यदि(!परिणाम) {
    /* Duplicate other fields. */
    dst->encoder = src->encoder;
    परिणाम = curl_mime_type(dst, src->mimetype);
  }
  यदि(!परिणाम)
    परिणाम = curl_mime_name(dst, src->name);
  यदि(!परिणाम)
    परिणाम = curl_mime_filename(dst, src->filename);

  /* If an error occurred, rollback. */
  यदि(परिणाम)
    Curl_mime_cleanpart(dst);

  प्रत्यागम परिणाम;
}

CURLcode Curl_mime_duppart(संरचना Curl_easy *दत्त,
                           curl_mimepart *dst, ध्रुव curl_mimepart *src)
{
  यदि(!dst || !src)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(mime_is_too_deep(src, 0))
    प्रत्यागम CURLE_TOO_LARGE;

  प्रत्यागम mime_duppart(दत्त, dst, src, 0);
}

/*
 * Mime build functions.
 */

/* Create a mime handle. */
curl_mime *curl_mime_init(शून्य *easy)
{
  curl_mime *mime = curlx_malloc(परिमाण(*mime));

  यदि(mime) {
    mime->parent = रिक्त;
    mime->firstpart = रिक्त;
    mime->lastpart = रिक्त;

    स्मृतिं_पूरय(mime->boundary, '-', MIME_BOUNDARY_DASHES);
    यदि(Curl_rand_alnum(easy,
                       (अचिह्नित वर्ण *)&mime->boundary[MIME_BOUNDARY_DASHES],
                       MIME_RAND_BOUNDARY_CHARS + 1)) {
      /* failed to get random separator, bail out */
      curlx_free(mime);
      प्रत्यागम रिक्त;
    }
    mimesetstate(&mime->state, MIMESTATE_BEGIN, रिक्त);
  }

  प्रत्यागम mime;
}

/* Initialize a mime part. */
शून्य Curl_mime_initpart(curl_mimepart *part)
{
  स्मृतिं_पूरय(part, 0, परिमाण(*part));
  part->lastreadstatus = 1; /* Successful read status. */
  mimesetstate(&part->state, MIMESTATE_BEGIN, रिक्त);
}

/* Create a mime part and append it to a mime handle's part list. */
curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  curl_mimepart *part;

  यदि(!mime)
    प्रत्यागम रिक्त;

  part = curlx_malloc(परिमाण(*part));

  यदि(part) {
    Curl_mime_initpart(part);
    part->parent = mime;

    यदि(mime->lastpart)
      mime->lastpart->nextpart = part;
    अन्यथा
      mime->firstpart = part;

    mime->lastpart = part;
  }

  प्रत्यागम part;
}

/* Set mime part name. */
CURLcode curl_mime_name(curl_mimepart *part, ध्रुव वर्ण *name)
{
  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  curlx_safefree(part->name);

  यदि(name) {
    part->name = curlx_strdup(name);
    यदि(!part->name)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  प्रत्यागम CURLE_OK;
}

/* Set mime part remote filename. */
CURLcode curl_mime_filename(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  curlx_safefree(part->filename);

  यदि(filename) {
    part->filename = curlx_strdup(filename);
    यदि(!part->filename)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  प्रत्यागम CURLE_OK;
}

/* Set mime part content from memory data. */
CURLcode curl_mime_दत्त(curl_mimepart *part, ध्रुव वर्ण *दत्त, size_t datasize)
{
  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  cleanup_part_content(part);

  यदि(दत्त) {
    यदि(datasize == CURL_ZERO_TERMINATED)
      datasize = वाक्यदैर्घ्यम्(दत्त);

    part->दत्त = curlx_memdup0(दत्त, datasize);
    यदि(!part->दत्त)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    part->datasize = datasize;
    part->readfunc = mime_mem_read;
    part->seekfunc = mime_mem_seek;
    part->freefunc = mime_mem_free;
    part->flags |= MIME_FAST_READ;
    part->विधा = MIMEKIND_दत्त;
  }

  प्रत्यागम CURLE_OK;
}

/* Set mime part content from named local file. */
CURLcode curl_mime_filedata(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  cleanup_part_content(part);

  यदि(filename) {
    वर्ण *आधारः;
    curlx_struct_stat sbuf;

    यदि(curlx_stat(filename, &sbuf))
      परिणाम = CURLE_READ_ERROR;
    अन्यथा {
      part->दत्त = curlx_strdup(filename);
      यदि(!part->दत्त)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      अन्यथा {
        part->datasize = -1;
        यदि(S_ISREG(sbuf.st_बहुलकम्)) {
          part->datasize = filesize(filename, sbuf);
          part->seekfunc = mime_file_seek;
        }

        part->readfunc = mime_file_read;
        part->freefunc = mime_file_free;
        part->विधा = MIMEKIND_FILE;

        /* As a side effect, set the filename to the current file's base name.
           It is possible to withdraw this by explicitly calling
           curl_mime_filename() with a NULL filename argument after the current
           call. */
        आधारः = strippath(filename);
        यदि(!आधारः)
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        अन्यथा {
          परिणाम = curl_mime_filename(part, आधारः);
          curlx_free(आधारः);
        }
      }
    }
  }
  प्रत्यागम परिणाम;
}

/* Set mime part type. */
CURLcode curl_mime_type(curl_mimepart *part, ध्रुव वर्ण *mimetype)
{
  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  curlx_safefree(part->mimetype);

  यदि(mimetype) {
    part->mimetype = curlx_strdup(mimetype);
    यदि(!part->mimetype)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर ध्रुव संरचना mime_encoder encoders[] = {
  { "binary", encoder_nop_read, encoder_nop_आकार },
  { "8bit", encoder_nop_read, encoder_nop_आकार },
  { "7bit", encoder_7bit_read, encoder_nop_आकार },
  { "base64", encoder_base64_read, encoder_base64_आकार },
  { "quoted-printable", encoder_qp_read, encoder_qp_आकार },
  { ZERO_NULL, ZERO_NULL, ZERO_NULL }
};

/* Set mime data transfer encoder. */
CURLcode curl_mime_encoder(curl_mimepart *part, ध्रुव वर्ण *encoding)
{
  CURLcode परिणाम = CURLE_BAD_FUNCTION_तर्कः;
  ध्रुव संरचना mime_encoder *mep;

  यदि(!part)
    प्रत्यागम परिणाम;

  part->encoder = रिक्त;

  यदि(!encoding)
    प्रत्यागम CURLE_OK;    /* Removing current encoder. */

  प्रति(mep = encoders; mep->name; mep++)
    यदि(curl_strequal(encoding, mep->name)) {
      part->encoder = mep;
      परिणाम = CURLE_OK;
    }

  प्रत्यागम परिणाम;
}

/* Set mime part headers. */
CURLcode curl_mime_शीर्षकाणि(curl_mimepart *part,
                           संरचना curl_slist *शीर्षकाणि, पूर्णाङ्क take_ownership)
{
  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(part->flags & MIME_USERHEADERS_OWNER) {
    यदि(part->userheaders != शीर्षकाणि)  /* Allow setting twice the same list. */
      curl_slist_free_all(part->userheaders);
    part->flags &= ~(अचिह्नित पूर्णाङ्क)MIME_USERHEADERS_OWNER;
  }
  part->userheaders = शीर्षकाणि;
  यदि(शीर्षकाणि && take_ownership)
    part->flags |= MIME_USERHEADERS_OWNER;
  प्रत्यागम CURLE_OK;
}

/* Set mime part content from callback. */
CURLcode curl_mime_दत्त_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc, शून्य *arg)
{
  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  cleanup_part_content(part);

  यदि(readfunc) {
    part->readfunc = readfunc;
    part->seekfunc = seekfunc;
    part->freefunc = freefunc;
    part->arg = arg;
    part->datasize = datasize;
    part->विधा = MIMEKIND_CALLBACK;
  }

  प्रत्यागम CURLE_OK;
}

/* Set mime part content from subparts. */
CURLcode Curl_mime_समुच्चयः_subparts(curl_mimepart *part,
                                curl_mime *subparts, पूर्णाङ्क take_ownership)
{
  curl_mime *मूलम्;

  यदि(!part)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* Accept setting twice the same subparts. */
  यदि(part->विधा == MIMEKIND_MULTIPART && part->arg == subparts)
    प्रत्यागम CURLE_OK;

  cleanup_part_content(part);

  यदि(subparts) {
    /* Should not have been attached already. */
    यदि(subparts->parent)
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

    /* Should not be the part's root. */
    मूलम् = part->parent;
    यदि(मूलम्) {
      यावत्(मूलम्->parent && मूलम्->parent->parent)
        मूलम् = मूलम्->parent->parent;
      यदि(subparts == मूलम्) {
        /* cannot add as a subpart of itself. */
        प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
      }
    }

    /* If subparts have already been used as a top-level MIMEPOST,
       they might not be positioned at start. Rewind them now, as
       a future check while rewinding the parent may cause this
       content to be skipped. */
    यदि(mime_subparts_seek(subparts, (curl_off_t)0, SEEK_SET) !=
       CURL_SEEKFUNC_OK)
      प्रत्यागम CURLE_SEND_FAIL_REWIND;

    subparts->parent = part;
    /* Subparts are processed internally: no read callback. */
    part->seekfunc = mime_subparts_seek;
    part->freefunc = take_ownership ? mime_subparts_free :
      mime_subparts_unbind;
    part->arg = subparts;
    part->datasize = -1;
    part->विधा = MIMEKIND_MULTIPART;
  }

  प्रत्यागम CURLE_OK;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  प्रत्यागम Curl_mime_समुच्चयः_subparts(part, subparts, TRUE);
}

/* Readback from top mime. */
/* Argument is the dummy top part. */
size_t Curl_mime_read(वर्ण *बफर, size_t आकार, size_t nitems, शून्य *instream)
{
  curl_mimepart *part = (curl_mimepart *)instream;
  size_t ret;
  bool hasread;

  (शून्य)आकार;  /* Always 1 */

  /* If `nitems` is <= 4, some encoders will return STOP_FILLING without
   * adding any data and this loops infinitely. */
  कुरु {
    hasread = FALSE;
    ret = readback_part(part, बफर, nitems, &hasread);
    /*
     * If this is not possible to get some data without calling more than
     * one read callback (probably because a content encoder is not able to
     * deliver a new bunch for the few data accumulated so far), force another
     * read until we get enough data or a special exit code.
     */
  } यावत्(ret == STOP_FILLING);

  प्रत्यागम ret;
}

/* Rewind mime stream. */
स्थिर CURLcode mime_rewind(curl_mimepart *part)
{
  प्रत्यागम mime_part_rewind(part) == CURL_SEEKFUNC_OK ?
         CURLE_OK : CURLE_SEND_FAIL_REWIND;
}

/* Compute header list size. */
स्थिर size_t slist_आकार(संरचना curl_slist *s,
                         size_t overhead, ध्रुव वर्ण *skip, size_t skiplen)
{
  size_t आकार = 0;

  प्रति(; s; s = s->अग्र)
    यदि(!skip || !मेलनम्_शीर्षकम्(s, skip, skiplen))
      आकार += वाक्यदैर्घ्यम्(s->दत्त) + overhead;
  प्रत्यागम आकार;
}

/* Get/compute multipart size. */
स्थिर curl_off_t multipart_आकार(curl_mime *mime)
{
  curl_off_t आकार;
  curl_off_t boundarysize;
  curl_mimepart *part;

  यदि(!mime)
    प्रत्यागम 0;           /* Not present -> empty. */

  boundarysize = 4 + MIME_BOUNDARY_LEN + 2;
  आकार = boundarysize;  /* Final boundary - CRLF after headers. */

  प्रति(part = mime->firstpart; part; part = part->nextpart) {
    curl_off_t sz = mime_आकार(part);

    यदि(sz < 0)
      आकार = sz;

    यदि(आकार >= 0)
      आकार += boundarysize + sz;
  }

  प्रत्यागम आकार;
}

/* Get/compute mime size. */
स्थिर curl_off_t mime_आकार(curl_mimepart *part)
{
  curl_off_t आकार;

  यदि(part->विधा == MIMEKIND_MULTIPART)
    part->datasize = multipart_आकार(part->arg);

  आकार = part->datasize;

  यदि(part->encoder)
    आकार = part->encoder->sizefunc(part);

  यदि(आकार >= 0 && !(part->flags & MIME_BODY_ONLY)) {
    /* Compute total part size. */
    आकार += slist_आकार(part->curlheaders, 2, रिक्त, 0);
    आकार += slist_आकार(part->userheaders, 2, STRCONST("Content-Type"));
    आकार += 2;    /* CRLF after headers. */
  }
  प्रत्यागम आकार;
}

/* Add a header. */
/* VARARGS2 */
CURLcode Curl_mime_सङ्कलय_शीर्षकम्(संरचना curl_slist **slp, ध्रुव वर्ण *fmt, ...)
{
  संरचना curl_slist *hdr = रिक्त;
  वर्ण *s = रिक्त;
  va_list ap;

  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap);
  va_end(ap);

  यदि(s) {
    hdr = Curl_slist_append_nodup(*slp, s);
    यदि(hdr)
      *slp = hdr;
    अन्यथा
      curlx_free(s);
  }

  प्रत्यागम hdr ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}

/* Add a content type header. */
स्थिर CURLcode सङ्कलय_content_type(संरचना curl_slist **slp,
                                 ध्रुव वर्ण *type, ध्रुव वर्ण *boundary)
{
  प्रत्यागम Curl_mime_सङ्कलय_शीर्षकम्(slp, "Content-Type: %s%s%s", type,
                              boundary ? "; boundary=" : "",
                              boundary ? boundary : "");
}

ध्रुव वर्ण *Curl_mime_contenttype(ध्रुव वर्ण *filename)
{
  /*
   * If no content type was specified, we scan through a few well-known
   * extensions and pick the first we match!
   */
  संरचना ContentType {
    ध्रुव वर्ण *extension;
    ध्रुव वर्ण *type;
  };
  स्थिर ध्रुव संरचना ContentType ctts[] = {
    { ".gif",  "image/gif" },
    { ".jpg",  "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".png",  "image/png" },
    { ".svg",  "image/svg+xml" },
    { ".txt",  "text/plain" },
    { ".htm",  "text/html" },
    { ".html", "text/html" },
    { ".pdf",  "application/pdf" },
    { ".xml",  "application/xml" }
  };

  यदि(filename) {
    size_t len1 = वाक्यदैर्घ्यम्(filename);
    ध्रुव वर्ण *nameend = filename + len1;
    अचिह्नित पूर्णाङ्क i;

    प्रति(i = 0; i < CURL_ARRAYSIZE(ctts); i++) {
      size_t len2 = वाक्यदैर्घ्यम्(ctts[i].extension);

      यदि(len1 >= len2 && curl_strequal(nameend - len2, ctts[i].extension))
        प्रत्यागम ctts[i].type;
    }
  }
  प्रत्यागम रिक्त;
}

स्थिर bool content_type_मेलनम्(ध्रुव वर्ण *contenttype,
                               ध्रुव वर्ण *target, size_t len)
{
  यदि(contenttype && curl_strnequal(contenttype, target, len))
    विकल्प(contenttype[len]) {
    पक्ष '\0':
    पक्ष '\t':
    पक्ष '\r':
    पक्ष '\n':
    पक्ष ' ':
    पक्ष ';':
      प्रत्यागम TRUE;
    }
  प्रत्यागम FALSE;
}

स्थिर CURLcode सङ्कलय_content_disposition(संरचना Curl_easy *दत्त,
                                        curl_mimepart *part,
                                        ध्रुव वर्ण *disposition,
                                        ध्रुव वर्ण *contenttype,
                                        गणना mimestrategy strategy)
{
  यदि(!disposition &&
     (part->filename || part->name ||
      (contenttype && !curl_strnequal(contenttype, "multipart/", 10))))
    disposition = DISPOSITION_DEFAULT;

  यदि(disposition && curl_strequal(disposition, "attachment") &&
     !part->name && !part->filename)
    disposition = रिक्त;

  यदि(disposition) {
    CURLcode परिणाम = CURLE_OK;
    वर्ण *name = रिक्त;
    वर्ण *filename = रिक्त;
    /* The mail (and legacy mime_formescape) strategy quotes the name and
       filename with a backslash and has no in-band way to represent a CR or
       LF, so one embedded in the value would split the generated header. The
       form strategy percent-encodes CR/LF (see escape_string) and is safe. */
    bool backslash = (strategy == MIMESTRATEGY_MAIL) ||
                     (दत्त && दत्त->समुच्चयः.mime_formescape);
    यदि(backslash &&
       ((part->name && part->name[strcspn(part->name, "\r\n")]) ||
        (part->filename && part->filename[strcspn(part->filename, "\r\n")])))
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

    यदि(part->name) {
      name = escape_string(दत्त, part->name, strategy);
      यदि(!name)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    यदि(part->filename) {
      filename = escape_string(दत्त, part->filename, strategy);
      यदि(!filename)
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    यदि(!परिणाम)
      परिणाम = Curl_mime_सङ्कलय_शीर्षकम्(&part->curlheaders,
                                    "Content-Disposition: %s%s%s%s%s%s%s",
                                    disposition,
                                    name ? "; name=\"" : "",
                                    name ? name : "",
                                    name ? "\"" : "",
                                    filename ? "; filename=\"" : "",
                                    filename ? filename : "",
                                    filename ? "\"" : "");
    curlx_safefree(name);
    curlx_safefree(filename);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mime_prepare_शीर्षकाणि(संरचना Curl_easy *दत्त,
                                     curl_mimepart *part,
                                     ध्रुव वर्ण *contenttype,
                                     ध्रुव वर्ण *disposition,
                                     गणना mimestrategy strategy,
                                     size_t call_गभीरता)
{
  curl_mime *mime = रिक्त;
  ध्रुव वर्ण *boundary = रिक्त;
  वर्ण *customct;
  ध्रुव वर्ण *cte = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(++call_गभीरता > MAX_MIME_स्तराः)
    प्रत्यागम CURLE_TOO_LARGE;

  /* Get rid of previously prepared headers. */
  curl_slist_free_all(part->curlheaders);
  part->curlheaders = रिक्त;

  /* Be sure we will not access old headers later. */
  यदि(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, रिक्त);

  /* Check if content type is specified. */
  customct = part->mimetype;
  यदि(!customct)
    customct = अन्वेषणम्_शीर्षकम्(part->userheaders, STRCONST("Content-Type"));
  यदि(customct)
    contenttype = customct;

  /* If content type is not specified, try to determine it. */
  यदि(!contenttype) {
    विकल्प(part->विधा) {
    पक्ष MIMEKIND_MULTIPART:
      contenttype = MULTIPART_CONTENTTYPE_DEFAULT;
      भङ्ग;
    पक्ष MIMEKIND_FILE:
      contenttype = Curl_mime_contenttype(part->filename);
      यदि(!contenttype)
        contenttype = Curl_mime_contenttype(part->दत्त);
      यदि(!contenttype && part->filename)
        contenttype = FILE_CONTENTTYPE_DEFAULT;
      भङ्ग;
    स्वभाव:
      contenttype = Curl_mime_contenttype(part->filename);
      भङ्ग;
    }
  }

  यदि(part->विधा == MIMEKIND_MULTIPART) {
    mime = (curl_mime *)part->arg;
    यदि(mime)
      boundary = mime->boundary;
  }
  अन्यथा यदि(contenttype && !customct &&
          content_type_मेलनम्(contenttype, STRCONST("text/plain")) &&
          (strategy == MIMESTRATEGY_MAIL || !part->filename))
    contenttype = रिक्त;

  /* Issue content-disposition header only if not already set by caller. */
  यदि(!अन्वेषणम्_शीर्षकम्(part->userheaders, STRCONST("Content-Disposition"))) {
    परिणाम = सङ्कलय_content_disposition(दत्त, part, disposition,
                                     contenttype, strategy);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Issue Content-Type header. */
  यदि(contenttype) {
    परिणाम = सङ्कलय_content_type(&part->curlheaders, contenttype, boundary);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Content-Transfer-Encoding header. */
  यदि(!अन्वेषणम्_शीर्षकम्(part->userheaders,
                    STRCONST("Content-Transfer-Encoding"))) {
    यदि(part->encoder)
      cte = part->encoder->name;
    अन्यथा यदि(contenttype && strategy == MIMESTRATEGY_MAIL &&
            part->विधा != MIMEKIND_MULTIPART)
      cte = "8bit";
    यदि(cte) {
      परिणाम = Curl_mime_सङ्कलय_शीर्षकम्(&part->curlheaders,
                                    "Content-Transfer-Encoding: %s", cte);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }

  /* If we were reading curl-generated headers, restart with new ones (this
     should not occur). */
  यदि(part->state.state == MIMESTATE_CURLHEADERS)
    mimesetstate(&part->state, MIMESTATE_CURLHEADERS, part->curlheaders);

  /* Process subparts. */
  यदि(part->विधा == MIMEKIND_MULTIPART && mime) {
    curl_mimepart *subpart;

    disposition = रिक्त;
    यदि(content_type_मेलनम्(contenttype, STRCONST("multipart/form-data")))
      disposition = "form-data";
    प्रति(subpart = mime->firstpart; subpart; subpart = subpart->nextpart) {
      परिणाम = mime_prepare_शीर्षकाणि(दत्त, subpart, रिक्त,
                                    disposition, strategy, call_गभीरता);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_mime_prepare_शीर्षकाणि(संरचना Curl_easy *दत्त,
                                   curl_mimepart *part,
                                   ध्रुव वर्ण *contenttype,
                                   ध्रुव वर्ण *disposition,
                                   गणना mimestrategy strategy)
{
  प्रत्यागम mime_prepare_शीर्षकाणि(दत्त, part, contenttype, disposition,
                              strategy, 0);
}

/* Recursively reset paused status in the given part. */
स्थिर शून्य mime_unpause(curl_mimepart *part)
{
  यदि(part) {
    यदि(part->lastreadstatus == CURL_READFUNC_PAUSE)
      part->lastreadstatus = 1; /* Successful read status. */
    यदि(part->विधा == MIMEKIND_MULTIPART) {
      curl_mime *mime = (curl_mime *)part->arg;

      यदि(mime) {
        curl_mimepart *subpart;

        प्रति(subpart = mime->firstpart; subpart; subpart = subpart->nextpart)
          mime_unpause(subpart);
      }
    }
  }
}

संरचना cr_mime_ctx {
  संरचना Curl_creader super;
  curl_mimepart *part;
  curl_off_t योग_len;
  curl_off_t read_len;
  CURLcode error_परिणाम;
  संरचना bufq tmpbuf;
  BIT(आलोकितम्_eos);
  BIT(errored);
};

स्थिर CURLcode cr_mime_init(संरचना Curl_easy *दत्त,
                             संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  ctx->योग_len = -1;
  ctx->read_len = 0;
  Curl_bufq_init2(&ctx->tmpbuf, 1024, 1, BUFQ_OPT_NO_SPARES);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cr_mime_close(संरचना Curl_easy *दत्त,
                          संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  Curl_bufq_free(&ctx->tmpbuf);
}

/* Real client reader to installed client callbacks. */
स्थिर CURLcode cr_mime_read(संरचना Curl_easy *दत्त,
                             संरचना Curl_creader *reader,
                             वर्ण *buf, size_t blen,
                             size_t *pnread, bool *peos)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nread;
  वर्ण tmp[256];

  /* Once we have errored, we will return the same error forever */
  यदि(ctx->errored) {
    CURL_TRC_READ(दत्त, "cr_mime_read(len=%zu) is errored -> %d, eos=0",
                  blen, (पूर्णाङ्क)ctx->error_परिणाम);
    *pnread = 0;
    *peos = FALSE;
    प्रत्यागम ctx->error_परिणाम;
  }
  यदि(ctx->आलोकितम्_eos) {
    CURL_TRC_READ(दत्त, "cr_mime_read(len=%zu) seen eos -> 0, eos=1", blen);
    *pnread = 0;
    *peos = TRUE;
    प्रत्यागम CURLE_OK;
  }
  /* respect length limitations */
  यदि(ctx->योग_len >= 0) {
    curl_off_t remain = ctx->योग_len - ctx->read_len;
    यदि(remain <= 0)
      blen = 0;
    अन्यथा यदि(remain < (curl_off_t)blen)
      blen = (size_t)remain;
  }

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->tmpbuf)) {
    परिणाम = Curl_bufq_read(&ctx->tmpbuf, (अचिह्नित वर्ण *)buf, blen, &nread);
    यदि(परिणाम) {
      ctx->errored = TRUE;
      ctx->error_परिणाम = परिणाम;
      प्रत्यागम परिणाम;
    }
  }
  अन्यथा यदि(blen <= 4) {
    /* Curl_mime_read() may go into an infinite loop when reading
     * via a base64 encoder, as it stalls when the read buffer is too small
     * to contain a complete 3-byte encoding. Read into a larger buffer
     * and use that until empty. */
    CURL_TRC_READ(दत्त, "cr_mime_read(len=%zu), small read, using tmp", blen);
    nread = Curl_mime_read(tmp, 1, परिमाण(tmp), ctx->part);
    यदि(nread <= परिमाण(tmp)) {
      size_t n;
      परिणाम = Curl_bufq_write(&ctx->tmpbuf, (अचिह्नित वर्ण *)tmp, nread, &n);
      यदि(परिणाम) {
        ctx->errored = TRUE;
        ctx->error_परिणाम = परिणाम;
        प्रत्यागम परिणाम;
      }
      /* stored it, read again */
      परिणाम = Curl_bufq_cread(&ctx->tmpbuf, buf, blen, &nread);
      यदि(परिणाम) {
        ctx->errored = TRUE;
        ctx->error_परिणाम = परिणाम;
        प्रत्यागम परिणाम;
      }
    }
  }
  अन्यथा
    nread = Curl_mime_read(buf, 1, blen, ctx->part);

  CURL_TRC_READ(दत्त, "cr_mime_read(len=%zu), mime_read() -> %zu",
                blen, nread);

  विकल्प(nread) {
  पक्ष 0:
    यदि((ctx->योग_len >= 0) && (ctx->read_len < ctx->योग_len)) {
      failf(दत्त, "client mime read EOF fail, "
            "only %" FMT_OFF_T "/%" FMT_OFF_T
            " of needed bytes read", ctx->read_len, ctx->योग_len);
      प्रत्यागम CURLE_READ_ERROR;
    }
    *pnread = 0;
    *peos = TRUE;
    ctx->आलोकितम्_eos = TRUE;
    भङ्ग;

  पक्ष CURL_READFUNC_ABORT:
    failf(दत्त, "operation aborted by callback");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_परिणाम = CURLE_ABORTED_BY_CALLBACK;
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    भङ्ग;

  पक्ष CURL_READFUNC_PAUSE:
    /* CURL_READFUNC_PAUSE pauses read callbacks that feed socket writes */
    CURL_TRC_READ(दत्त, "cr_mime_read(len=%zu), paused by callback", blen);
    *pnread = 0;
    *peos = FALSE;
    परिणाम = Curl_xfer_pause_send(दत्त, TRUE);
    भङ्ग; /* nothing was read */

  पक्ष STOP_FILLING:
  पक्ष READ_ERROR:
    failf(दत्त, "read error getting mime data");
    *pnread = 0;
    *peos = FALSE;
    ctx->errored = TRUE;
    ctx->error_परिणाम = CURLE_READ_ERROR;
    परिणाम = CURLE_READ_ERROR;
    भङ्ग;

  स्वभाव:
    यदि(nread > blen) {
      /* the read function returned a too large value */
      failf(दत्त, "read function returned funny value");
      *pnread = 0;
      *peos = FALSE;
      ctx->errored = TRUE;
      ctx->error_परिणाम = CURLE_READ_ERROR;
      प्रत्यागम CURLE_READ_ERROR;
    }
    ctx->read_len += nread;
    यदि(ctx->योग_len >= 0)
      ctx->आलोकितम्_eos = (ctx->read_len >= ctx->योग_len);
    *pnread = nread;
    *peos = (bool)ctx->आलोकितम्_eos;
    भङ्ग;
  }

  CURL_TRC_READ(दत्त, "cr_mime_read(len=%zu, total=%" FMT_OFF_T
                ", read=%" FMT_OFF_T ") -> %d, %zu, %d", blen,
                ctx->योग_len, ctx->read_len, (पूर्णाङ्क)परिणाम, *pnread, *peos);
  प्रत्यागम परिणाम;
}

स्थिर bool cr_mime_needs_rewind(संरचना Curl_easy *दत्त,
                                 संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम ctx->read_len > 0;
}

स्थिर curl_off_t cr_mime_योग_दीर्घता(संरचना Curl_easy *दत्त,
                                       संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम ctx->योग_len;
}

स्थिर CURLcode cr_mime_resume_from(संरचना Curl_easy *दत्त,
                                    संरचना Curl_creader *reader,
                                    curl_off_t विस्थापनम्)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;

  यदि(विस्थापनम् > 0) {
    curl_off_t passed = 0;

    कुरु {
      वर्ण scratch[4 * 1024];
      size_t readthisamountnow =
        (विस्थापनम् - passed > (curl_off_t)परिमाण(scratch)) ?
        परिमाण(scratch) :
        curlx_sotouz(विस्थापनम् - passed);
      size_t nread;

      nread = Curl_mime_read(scratch, 1, readthisamountnow, ctx->part);
      passed += (curl_off_t)nread;
      यदि((nread == 0) || (nread > readthisamountnow)) {
        /* this checks for greater-than only to make sure that the
           CURL_READFUNC_ABORT return code still aborts */
        failf(दत्त, "Could only read %" FMT_OFF_T
              " bytes from the mime post", passed);
        प्रत्यागम CURLE_READ_ERROR;
      }
    } यावत्(passed < विस्थापनम्);

    /* now, decrease the size of the read */
    यदि(ctx->योग_len > 0) {
      ctx->योग_len -= विस्थापनम्;

      यदि(ctx->योग_len <= 0) {
        failf(दत्त, "Mime post already completely uploaded");
        प्रत्यागम CURLE_PARTIAL_FILE;
      }
    }
    /* we have passed, proceed as normal */
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cr_mime_cntrl(संरचना Curl_easy *दत्त,
                              संरचना Curl_creader *reader,
                              Curl_creader_cntrl opcode)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  विकल्प(opcode) {
  पक्ष CURL_CRCNTRL_REWIND: {
    CURLcode परिणाम = mime_rewind(ctx->part);
    यदि(परिणाम)
      failf(दत्त, "Cannot rewind mime/post data");
    प्रत्यागम परिणाम;
  }
  पक्ष CURL_CRCNTRL_UNPAUSE:
    mime_unpause(ctx->part);
    भङ्ग;
  पक्ष CURL_CRCNTRL_CLEAR_EOS:
    ctx->आलोकितम्_eos = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool cr_mime_is_paused(संरचना Curl_easy *दत्त,
                              संरचना Curl_creader *reader)
{
  संरचना cr_mime_ctx *ctx = reader->ctx;
  (शून्य)दत्त;
  प्रत्यागम ctx->part && ctx->part->lastreadstatus == CURL_READFUNC_PAUSE;
}

स्थिर ध्रुव संरचना Curl_crtype cr_mime = {
  "cr-mime",
  cr_mime_init,
  cr_mime_read,
  cr_mime_close,
  cr_mime_needs_rewind,
  cr_mime_योग_दीर्घता,
  cr_mime_resume_from,
  cr_mime_cntrl,
  cr_mime_is_paused,
  Curl_creader_def_समाप्तम्,
  परिमाण(संरचना cr_mime_ctx)
};

CURLcode Curl_creader_समुच्चयः_mime(संरचना Curl_easy *दत्त, curl_mimepart *part)
{
  संरचना Curl_creader *r;
  संरचना cr_mime_ctx *ctx;
  CURLcode परिणाम;

  परिणाम = Curl_creader_create(&r, दत्त, &cr_mime, CURL_CR_CLIENT);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  ctx = r->ctx;
  ctx->part = part;
  /* Make sure we will read the entire mime structure. */
  परिणाम = mime_rewind(ctx->part);
  यदि(परिणाम) {
    Curl_creader_free(दत्त, r);
    प्रत्यागम परिणाम;
  }
  ctx->योग_len = mime_आकार(ctx->part);

  प्रत्यागम Curl_creader_समुच्चयः(दत्त, r);
}

#अन्यथा /* !CURL_DISABLE_MIME && (!CURL_DISABLE_HTTP ||
                                !CURL_DISABLE_SMTP ||
                                !CURL_DISABLE_IMAP) */

/* Mime not compiled in: define stubs for externally-referenced functions. */
curl_mime *curl_mime_init(CURL *easy)
{
  (शून्य)easy;
  प्रत्यागम रिक्त;
}

शून्य curl_mime_free(curl_mime *mime)
{
  (शून्य)mime;
}

curl_mimepart *curl_mime_addpart(curl_mime *mime)
{
  (शून्य)mime;
  प्रत्यागम रिक्त;
}

CURLcode curl_mime_name(curl_mimepart *part, ध्रुव वर्ण *name)
{
  (शून्य)part;
  (शून्य)name;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_filename(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  (शून्य)part;
  (शून्य)filename;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_type(curl_mimepart *part, ध्रुव वर्ण *mimetype)
{
  (शून्य)part;
  (शून्य)mimetype;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_encoder(curl_mimepart *part, ध्रुव वर्ण *encoding)
{
  (शून्य)part;
  (शून्य)encoding;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_दत्त(curl_mimepart *part, ध्रुव वर्ण *दत्त, size_t datasize)
{
  (शून्य)part;
  (शून्य)दत्त;
  (शून्य)datasize;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_filedata(curl_mimepart *part, ध्रुव वर्ण *filename)
{
  (शून्य)part;
  (शून्य)filename;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_दत्त_cb(curl_mimepart *part, curl_off_t datasize,
                           curl_read_callback readfunc,
                           curl_seek_callback seekfunc,
                           curl_free_callback freefunc,
                           शून्य *arg)
{
  (शून्य)part;
  (शून्य)datasize;
  (शून्य)readfunc;
  (शून्य)seekfunc;
  (शून्य)freefunc;
  (शून्य)arg;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_subparts(curl_mimepart *part, curl_mime *subparts)
{
  (शून्य)part;
  (शून्य)subparts;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode curl_mime_शीर्षकाणि(curl_mimepart *part,
                           संरचना curl_slist *शीर्षकाणि, पूर्णाङ्क take_ownership)
{
  (शून्य)part;
  (शून्य)शीर्षकाणि;
  (शून्य)take_ownership;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

CURLcode Curl_mime_सङ्कलय_शीर्षकम्(संरचना curl_slist **slp, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)slp;
  (शून्य)fmt;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

#यदि_अन्तः /* if disabled */
